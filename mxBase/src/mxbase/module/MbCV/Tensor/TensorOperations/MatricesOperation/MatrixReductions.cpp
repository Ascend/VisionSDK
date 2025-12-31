/*
* -------------------------------------------------------------------------
*  This file is part of the Vision SDK project.
* Copyright (c) 2025 Huawei Technologies Co.,Ltd.
*
* Vision SDK is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*
*           http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
* -------------------------------------------------------------------------
 * Description: Manage Tensor Matrix Reductions.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/MatrixReductions.h"
#include <iostream>
#include "MxBase/Log/Log.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/CommonUtils.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/OpCall.h"
#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/PerElementOperations.h"

namespace {
    constexpr uint32_t SIZE_OF_UINT8 = 1;
    constexpr uint32_t SIZE_OF_FLOAT16 = 2;
    constexpr uint32_t SIZE_OF_FLOAT32 = 4;
    constexpr uint32_t SIZE_OF_UINT32 = 4;
    constexpr size_t HW_SHAPE_SIZE = 2;
    constexpr size_t HWC_SHAPE_SIZE = 3;
    constexpr size_t NHWC_SHAPE_SIZE = 4;
    constexpr size_t TENSOR_DIMENSION_ZERO = 0;
    constexpr size_t TENSOR_DIMENSION_ONE = 1;
    constexpr size_t TENSOR_DIMENSION_TWO = 2;
    constexpr size_t TENSOR_DIMENSION_THREE = 3;
    constexpr int SCALAR_DIMENSION_ZERO = 0;
    constexpr int SCALAR_DIMENSION_ONE = 1;
    constexpr int SCALAR_DIMENSION_TWO = 2;
    constexpr int SCALAR_DIMENSION_THREE = 3;
    constexpr uint32_t ONE_CHANNEL = 1;
    constexpr uint32_t THREE_CHANNEL = 3;
    constexpr uint32_t FOUR_CHANNEL = 4;
    constexpr uint32_t AICORE_NUMS = 8;
    constexpr size_t MAX_SIZE = 4096;
    constexpr uint32_t MIN_MAX_LOC_SHAPE_SIZE = 2;
    constexpr uint32_t DATA_BYTES_PER_BLOCK = 32;
    constexpr uint32_t BLOCK_NUM = 8;
}

namespace MxBase {
    std::map<TensorDType, uint32_t> TensorTypeSizeMap = {
        {TensorDType::UINT8, SIZE_OF_UINT8},
        {TensorDType::FLOAT16, SIZE_OF_FLOAT16},
        {TensorDType::FLOAT32, SIZE_OF_FLOAT32}
    };
 
    struct MinMaxDstTensors {
        Tensor minVal;
        Tensor maxVal;
        Tensor minLoc;
        Tensor maxLoc;
    };

    std::vector<uint32_t> GetSumDstShape(const std::vector<uint32_t> &inputShape)
    {
        std::vector<uint32_t> dstShape;
        size_t srcDimNum = inputShape.size();
        if (srcDimNum == NHWC_SHAPE_SIZE) {
            dstShape = {inputShape[0], inputShape[srcDimNum - 1]}; // [n, c]
        } else if (srcDimNum == HWC_SHAPE_SIZE) {
            dstShape = {inputShape[srcDimNum - 1]}; // [c]
        }
        return dstShape;
    }

    APP_ERROR CheckSumOpParams(const Tensor &src)
    {
        size_t srcDimNum = src.GetShape().size();
        if (srcDimNum != HWC_SHAPE_SIZE && srcDimNum != NHWC_SHAPE_SIZE) {
            LogError << "CheckSumOpParams: Input Tensors must be NHWC or HWC, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (src.GetShape()[srcDimNum - 1] < ONE_CHANNEL || src.GetShape()[srcDimNum - 1] > FOUR_CHANNEL) {
            LogError << "CheckSumOpParams: Input Tensors channel number should be in range [1, 4], please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        return APP_ERR_OK;
    }
    static APP_ERROR RunSumOpU8AndFp16(const Tensor &src, Tensor &dst, AscendStream &stream)
    {
        std::string opType = "Sum";
        if (IsSetReferRect(src)) {
            LogError << opType << ": Inplace is unsupported.";
            return APP_ERR_COMM_INVALID_PARAM;
        }
        Tensor srcFp32;
        APP_ERROR ret = ConvertTo(src, srcFp32, TensorDType::FLOAT32, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": ConvertTo float32 failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> srcVec = {srcFp32};
        Tensor dstFp32;
        ExpectedTensorInfo expectedFp32Info = {GetSumDstShape(src.GetShape()), TensorDType::FLOAT32, src.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dstFp32, expectedFp32Info);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc failed or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dstFp32};
        RunOpParam sumParam{opType, srcVec, dstVec};
        ret = RunOp(sumParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
            return ret;
        }
        ret = ConvertTo(dstFp32, dst, src.GetDataType(), stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": ConvertTo dst data type failed." << GetErrorInfo(ret);
        }
        return ret;
    }

    APP_ERROR Sum(const Tensor &src, Tensor &dst, AscendStream &stream)
    {
        LogDebug << "Start to execute Sum op.";
        std::string opType = "Sum";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype, true, false);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Failed to check parameters." << GetErrorInfo(ret);
            return ret;
        }
        ret = CheckSumOpParams(src);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Failed to check parameters." << GetErrorInfo(ret);
            return ret;
        }
        // 2. OperatorImplicitMalloc
        const auto srcDType = src.GetDataType();
        ExpectedTensorInfo expectedTensorInfo = {GetSumDstShape(src.GetShape()), srcDType, src.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc failed or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        // 3. Execute RunOp
        if (srcDType == TensorDType::FLOAT16 || srcDType == TensorDType::UINT8) {
            ret = RunSumOpU8AndFp16(src, dst, stream);
        } else {
            std::vector<Tensor> dstVec = {dst};
            RunOpParam sumParam{opType, srcVec, dstVec};
            ret = RunOp(sumParam, stream);
        }
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        }
        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    static APP_ERROR GetReduceDimAndType(const MxBase::ReduceDim &rDim, const MxBase::ReduceType &rType,
                                         const size_t &shapeSize, int32_t &dim, std::string &opType)
    {
        switch (rDim) {
            case MxBase::ReduceDim::REDUCE_HEIGHT:
                dim = (shapeSize == NHWC_SHAPE_SIZE) ? SCALAR_DIMENSION_ONE : SCALAR_DIMENSION_ZERO;
                break;
            case MxBase::ReduceDim::REDUCE_WIDTH:
                dim = (shapeSize == NHWC_SHAPE_SIZE) ? SCALAR_DIMENSION_TWO : SCALAR_DIMENSION_ONE;
                break;
            default:
                LogError << "GetReduceDimAndType: Get reduce dim abnormal, please check."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
        }

        switch (rType) {
            case MxBase::ReduceType::REDUCE_SUM:
                opType = "ReduceSum";
                break;
            case MxBase::ReduceType::REDUCE_MEAN:
                opType = "ReduceMean";
                break;
            case MxBase::ReduceType::REDUCE_MAX:
                opType = "ReduceMax";
                break;
            case MxBase::ReduceType::REDUCE_MIN:
                opType = "ReduceMin";
                break;
            default:
                LogError << "GetReduceDimAndType: Get reduce type abnormal, please check."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
        }

        return APP_ERR_OK;
    }

    static std::vector<uint32_t> GetReduceDstShape(const std::vector<uint32_t> &inputShape,
                                                   const MxBase::ReduceDim &rDim)
    {
        std::vector<uint32_t> dstShape = {};
        size_t srcDimNum = inputShape.size();
        switch (srcDimNum) {
            case HW_SHAPE_SIZE:
                 // {W} or {H}
                dstShape = (rDim == MxBase::ReduceDim::REDUCE_HEIGHT) ?
                           std::vector<uint32_t>{inputShape[TENSOR_DIMENSION_ONE]} :
                           std::vector<uint32_t>{inputShape[TENSOR_DIMENSION_ZERO]};
                break;
            case NHWC_SHAPE_SIZE:
                // {N, W, C} or {N, H, C}
                dstShape = (rDim == MxBase::ReduceDim::REDUCE_HEIGHT) ?
                           std::vector<uint32_t>{inputShape[0], inputShape[TENSOR_DIMENSION_TWO],
                                                 inputShape[srcDimNum - 1]} :
                           std::vector<uint32_t>{inputShape[0], inputShape[TENSOR_DIMENSION_ONE],
                                                 inputShape[srcDimNum - 1]};
                break;
            case HWC_SHAPE_SIZE:
                // {W, C} or {H, C}
                dstShape = (rDim == MxBase::ReduceDim::REDUCE_HEIGHT) ?
                           std::vector<uint32_t>{inputShape[TENSOR_DIMENSION_ONE],
                                                 inputShape[srcDimNum - 1]} :
                           std::vector<uint32_t>{inputShape[TENSOR_DIMENSION_ZERO],
                                                 inputShape[srcDimNum - 1]};
                break;
            default:
                LogDebug << "GetReduceDstShape: Get reduce dst shape abnormal, default value is empty vector.";
                break;
        }

        return dstShape;
    }

    static APP_ERROR CheckReduceOpParams(const Tensor &src)
    {
        size_t srcDimNum = src.GetShape().size();
        if (srcDimNum != HW_SHAPE_SIZE && srcDimNum != HWC_SHAPE_SIZE && srcDimNum != NHWC_SHAPE_SIZE) {
            LogError << "CheckReduceOpParams: Input Tensors must be HW, HWC or NHWC, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (srcDimNum != HW_SHAPE_SIZE &&
            (src.GetShape().back() < ONE_CHANNEL || src.GetShape().back() > FOUR_CHANNEL)) {
            LogError << "CheckReduceOpParams: Input Tensors channel number should be in range [1, 4], please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        return APP_ERR_OK;
    }

    APP_ERROR Reduce(const Tensor &src, Tensor &dst,
                     const MxBase::ReduceDim &rDim, const MxBase::ReduceType &rType,
                     AscendStream &stream)
    {
        LogDebug << "Start to execute Reduce op.";
        int32_t dim = 0;
        std::string opType = "Reduce";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype, true, false);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Failed to check parameters." << GetErrorInfo(ret);
            return ret;
        }
        ret = CheckReduceOpParams(src);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Failed to check parameters." << GetErrorInfo(ret);
            return ret;
        }
        ret = GetReduceDimAndType(rDim, rType, src.GetShape().size(), dim, opType);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Failed to get reduce dim or type." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        auto dstShape = GetReduceDstShape(src.GetShape(), rDim);
        ExpectedTensorInfo expectedTensorInfo = {dstShape, src.GetDataType(), src.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc failed or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3.Calculate the reduce axis
        Tensor dimTensor(&dim, std::vector<uint32_t>{1}, TensorDType::INT32);
        ret = dimTensor.ToDevice(src.GetDeviceId());
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Tensor to device failed." << GetErrorInfo(ret);
            return ret;
        }

        // 4. Execute RunOp
        RunOpParam reduceParam{opType, srcVec, dstVec};
        reduceParam.fixedType = "dsl";
        reduceParam.dslParam.dslPropertyVec = {dimTensor};
        ret = RunOp(reduceParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }
        LogDebug << opType << ": Run op finished.";

        return ret;
    }

    APP_ERROR CheckXSumOpParams(const Tensor &src)
    {
        size_t srcDim = src.GetShape().size();
        if (srcDim != HWC_SHAPE_SIZE) {
            LogError << "Check AbsSum/SqrSum OpParams: Input Tensors must be HWC, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (src.GetShape()[0] > MAX_SIZE || src.GetShape()[1] > MAX_SIZE) {
            LogError << "Check AbsSum/SqrSum OpParams: The width and height of input tensor should be not larger than "
                     << MAX_SIZE << ", please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        if (src.GetShape()[srcDim - 1] != ONE_CHANNEL && src.GetShape()[srcDim - 1] != THREE_CHANNEL) {
            LogError << "Check AbsSum/SqrSum OpParams: Input Tensors channel number should be 1 or 3, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        return APP_ERR_OK;
    }

    APP_ERROR BuildDstTmp(const Tensor &src, Tensor &dst, std::vector<Tensor>& dstVec)
    {
        std::vector<uint32_t> dstShape = {src.GetShape()[HWC_SHAPE_SIZE - 1]};
        ExpectedTensorInfo expectedTensorInfo = {dstShape, TensorDType::FLOAT32, src.GetDeviceId()};
        APP_ERROR ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        const size_t zGmDim1 = 8;
        const size_t zGmDim2 = 9;
        Tensor tmpDst(std::vector<uint32_t>{zGmDim1, zGmDim2}, MxBase::TensorDType::FLOAT32, src.GetDeviceId());
        ret = tmpDst.Malloc();
        if (ret != APP_ERR_OK) {
            return ret;
        }
        dstVec.emplace_back(tmpDst);
        return APP_ERR_OK;
    }

    APP_ERROR AbsSum(const Tensor &src, Tensor &dst, AscendStream& stream)
    {
        LogDebug << "Start to execute AbsSum op.";
        std::string opType = "AbsSum";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src};
        OpSupportDtype opSupportDtype;
        opSupportDtype.isU8AndFP32 = true;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        ret = CheckXSumOpParams(src);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        std::vector<Tensor> dstVec;
        ret = BuildDstTmp(src, dst, dstVec);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }

        // 3. Execute RunOp
        RunOpParam absSumParam{opType, srcVec, dstVec};
        ret = RunOp(absSumParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
            return ret;
        } else {
            LogDebug << opType << ": Run op success.";
        }

        // 4. Set the result
        const size_t offset = 64;
        size_t channelNum = src.GetShape()[HWC_SHAPE_SIZE - 1];
        ret = aclrtMemcpyAsync(dst.GetData(), channelNum * sizeof(float), dstVec[0].GetData() + offset * sizeof(float),
                               channelNum * sizeof(float), ACL_MEMCPY_DEVICE_TO_DEVICE, stream.stream);
        if (ret != ACL_SUCCESS) {
            LogError << "Failed to aclrtMemcpyAsync in AbsSum operation." << GetErrorInfo(ret, "aclrtMemcpyAsync");
            return ret;
        }

        ret = AddStreamRef(dstVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Failed to add stream reference." << GetErrorInfo(ret);
        }
        LogDebug << opType << ": Run op finished.";

        return ret;
    }


    APP_ERROR SqrSum(const Tensor &src, Tensor &dst, AscendStream& stream)
    {
        LogDebug << "Start to execute SqrSum op.";
        std::string opType = "SqrSum";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src};
        OpSupportDtype opSupportDtype;
        opSupportDtype.isU8AndFP32 = true;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        ret = CheckXSumOpParams(src);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        std::vector<Tensor> dstVec;
        ret = BuildDstTmp(src, dst, dstVec);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }

        // 3. Execute RunOp
        RunOpParam sqrSumParam{opType, srcVec, dstVec};
        ret = RunOp(sqrSumParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
            return ret;
        } else {
            LogDebug << opType << ": Run op success.";
        }

        // 4. Set the result
        const size_t offset = 64;
        size_t channelNum = src.GetShape()[HWC_SHAPE_SIZE - 1];
        ret = aclrtMemcpyAsync(dst.GetData(), channelNum * sizeof(float), dstVec[0].GetData() + offset * sizeof(float),
                               channelNum * sizeof(float), ACL_MEMCPY_DEVICE_TO_DEVICE, stream.stream);
        if (ret != ACL_SUCCESS) {
            LogError << "Failed to aclrtMemcpyAsync in SqrSum operation." << GetErrorInfo(ret, "aclrtMemcpyAsync");
            return ret;
        }

        ret = AddStreamRef(dstVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Failed to add stream reference." << GetErrorInfo(ret);
        }
        LogDebug << opType << ": Run op finished.";

        return ret;
    }

        APP_ERROR CheckMinMaxLocOpParams(const Tensor &src)
    {
        size_t srcDim = src.GetShape().size();
        if (srcDim < HW_SHAPE_SIZE || srcDim > HWC_SHAPE_SIZE) {
            LogError << "CheckMinMaxLocOpParams: Input dimension must be 2 or 3."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (srcDim == HWC_SHAPE_SIZE && src.GetShape()[TENSOR_DIMENSION_TWO] != ONE_CHANNEL) {
            LogError << "CheckMinMaxLocOpParams: Input tensor's channel must be 1."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }
 
    APP_ERROR CheckMinMaxLocParams(const Tensor &src, Tensor &minVal, Tensor &maxVal, Tensor &minLoc, Tensor &maxLoc,
        AscendStream &)
    {
        std::string opType = "MinMaxLoc";
 
        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype, false, false);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Failed to check parameters." << GetErrorInfo(ret);
            return ret;
        }
        ret = CheckMinMaxLocOpParams(src);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Failed to check parameters." << GetErrorInfo(ret);
            return ret;
        }
 
        // 2. OperatorImplicitMalloc
        std::vector<uint32_t> minMaxValShape = { 1 };
        ExpectedTensorInfo expectedMinMaxValTensorInfo = { minMaxValShape, src.GetDataType(), src.GetDeviceId() };
        ret = OperatorImplicitMallocTensor(minVal, expectedMinMaxValTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc failed or min value tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        ret = OperatorImplicitMallocTensor(maxVal, expectedMinMaxValTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc failed or max value tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
 
        std::vector<uint32_t> minMaxLocShape = { 2 };
        ExpectedTensorInfo expectedMinMaxLocTensorInfo = { minMaxLocShape, TensorDType::UINT32, src.GetDeviceId() };
        ret = OperatorImplicitMallocTensor(minLoc, expectedMinMaxLocTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc failed or min location tensor check failed."
                     << GetErrorInfo(ret);
            return ret;
        }
        ret = OperatorImplicitMallocTensor(maxLoc, expectedMinMaxLocTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc failed or max location tensor check failed."
                     << GetErrorInfo(ret);
        }
        return ret;
    }
 
    APP_ERROR MinMaxGMTensorMalloc(Tensor &minValGM, Tensor &maxValGM, Tensor &minLocGM, Tensor &maxLocGM)
    {
        APP_ERROR ret = minValGM.Malloc();
        if (ret != APP_ERR_OK) {
            LogError << "MinMaxGMTensorMalloc: Malloc min value GM Tensor failed." << GetErrorInfo(ret);
            return ret;
        }
        ret = maxValGM.Malloc();
        if (ret != APP_ERR_OK) {
            LogError << "MinMaxGMTensorMalloc: Malloc max value GM Tensor failed." << GetErrorInfo(ret);
            return ret;
        }
        ret = minLocGM.Malloc();
        if (ret != APP_ERR_OK) {
            LogError << "MinMaxGMTensorMalloc: Malloc min location GM Tensor failed." << GetErrorInfo(ret);
            return ret;
        }
        ret = maxLocGM.Malloc();
        if (ret != APP_ERR_OK) {
            LogError << "MinMaxGMTensorMalloc: Malloc max location GM Tensor failed." << GetErrorInfo(ret);
        }
        return ret;
    }
 
    APP_ERROR MinMaxLocDstDataCopy(MinMaxDstTensors &minMaxDst, MinMaxDstTensors &minMaxGM, AscendStream & stream)
    {
        uint32_t srcDataTypeSize = TensorTypeSizeMap[minMaxDst.minVal.GetDataType()];
        APP_ERROR ret = aclrtMemcpyAsync(minMaxDst.minVal.GetData(), srcDataTypeSize, minMaxGM.minVal.GetData(),
            srcDataTypeSize, ACL_MEMCPY_DEVICE_TO_DEVICE, stream.stream);
        if (ret != ACL_SUCCESS) {
            LogError << "Failed to conduct aclrtMemcpyAsync in MinMaxLoc operation."
                     << GetErrorInfo(ret, "aclrtMemcpyAsync");
            return ret;
        }
        ret = aclrtMemcpyAsync(minMaxDst.maxVal.GetData(), srcDataTypeSize, minMaxGM.maxVal.GetData(), srcDataTypeSize,
            ACL_MEMCPY_DEVICE_TO_DEVICE, stream.stream);
        if (ret != ACL_SUCCESS) {
            LogError << "Failed to conduct aclrtMemcpyAsync in MinMaxLoc operation."
                     << GetErrorInfo(ret, "aclrtMemcpyAsync");
            return ret;
        }
        size_t minMaxLocMemCopyLen = SIZE_OF_UINT32 * MIN_MAX_LOC_SHAPE_SIZE;
        ret = aclrtMemcpyAsync(minMaxDst.minLoc.GetData(), minMaxLocMemCopyLen, minMaxGM.minLoc.GetData(),
                               minMaxLocMemCopyLen, ACL_MEMCPY_DEVICE_TO_DEVICE, stream.stream);
        if (ret != ACL_SUCCESS) {
            LogError << "Failed to conduct aclrtMemcpyAsync in MinMaxLoc operation."
                     << GetErrorInfo(ret, "aclrtMemcpyAsync");
            return ret;
        }
        ret = aclrtMemcpyAsync(minMaxDst.maxLoc.GetData(), minMaxLocMemCopyLen, minMaxGM.maxLoc.GetData(),
                               minMaxLocMemCopyLen, ACL_MEMCPY_DEVICE_TO_DEVICE, stream.stream);
        if (ret != ACL_SUCCESS) {
            LogError << "Failed to conduct aclrtMemcpyAsync in MinMaxLoc operation."
                     << GetErrorInfo(ret, "aclrtMemcpyAsync");
            return ret;
        }
 
        std::vector<Tensor> dstVec{minMaxGM.minVal, minMaxGM.maxVal, minMaxGM.minLoc, minMaxGM.maxLoc};
        ret = AddStreamRef(dstVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Add stream reference failed." << GetErrorInfo(ret);
        }
        return ret;
    }
 
    APP_ERROR MinMaxLoc(const Tensor &src, Tensor &minVal, Tensor &maxVal, Tensor &minLoc, Tensor &maxLoc,
                        AscendStream &stream)
    {
        LogDebug << "Start to execute MinMaxLoc op.";
        std::string opType = "MinMaxLoc";
 
        std::vector<Tensor> srcVec = {src};
 
        // 1.Check parameters
        APP_ERROR ret = CheckMinMaxLocParams(src, minVal, maxVal, minLoc, maxLoc, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Check params failed." << GetErrorInfo(ret);
            return ret;
        }
 
        // 2.Tensor malloc
        uint32_t srcDataTypeSize = TensorTypeSizeMap[src.GetDataType()];
        Tensor minValGM({ BLOCK_NUM, DATA_BYTES_PER_BLOCK / srcDataTypeSize }, src.GetDataType(), src.GetDeviceId());
        Tensor maxValGM({ BLOCK_NUM, DATA_BYTES_PER_BLOCK / srcDataTypeSize }, src.GetDataType(), src.GetDeviceId());
        Tensor minLocGM({ BLOCK_NUM, DATA_BYTES_PER_BLOCK / sizeof(uint32_t) }, TensorDType::UINT32, src.GetDeviceId());
        Tensor maxLocGM({ BLOCK_NUM, DATA_BYTES_PER_BLOCK / sizeof(uint32_t) }, TensorDType::UINT32, src.GetDeviceId());
        ret = MinMaxGMTensorMalloc(minValGM, maxValGM, minLocGM, maxLocGM);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Malloc Global Memory Tensor failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = { minValGM, maxValGM, minLocGM, maxLocGM };
 
        // 3. Execute RunOp
        RunOpParam minMaxLocParam{opType, srcVec, dstVec};
        ret = RunOp(minMaxLocParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
            return ret;
        } else {
            LogDebug << opType << ": Run op success.";
        }
 
        // 4. Set final result
        MinMaxDstTensors minMaxDst = { minVal, maxVal, minLoc, maxLoc };
        MinMaxDstTensors minMaxGM = { minValGM, maxValGM, minLocGM, maxLocGM };
        ret = MinMaxLocDstDataCopy(minMaxDst, minMaxGM, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Dst tensor data copy failed." << GetErrorInfo(ret);
        }
 
        LogDebug << opType << ": Run op finished.";
 
        return ret;
    }

    APP_ERROR MinMax(const Tensor &src, Tensor &minVal, Tensor &maxVal, AscendStream &stream)
    {
        Tensor minLoc;
        Tensor maxLoc;
        LogDebug << "Start to execute MinMax op.";
        std::string opType = "MinMax";

        std::vector<Tensor> srcVec = {src};

        // 1.Check parameters
        APP_ERROR ret = CheckMinMaxLocParams(src, minVal, maxVal, minLoc, maxLoc, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Check params failed." << GetErrorInfo(ret);
            return ret;
        }

        // 2.Tensor malloc
        uint32_t srcDataTypeSize = TensorTypeSizeMap[src.GetDataType()];
        Tensor minValGM({ BLOCK_NUM, DATA_BYTES_PER_BLOCK / srcDataTypeSize }, src.GetDataType(), src.GetDeviceId());
        Tensor maxValGM({ BLOCK_NUM, DATA_BYTES_PER_BLOCK / srcDataTypeSize }, src.GetDataType(), src.GetDeviceId());
        Tensor minLocGM({ BLOCK_NUM, DATA_BYTES_PER_BLOCK / sizeof(uint32_t) }, TensorDType::UINT32, src.GetDeviceId());
        Tensor maxLocGM({ BLOCK_NUM, DATA_BYTES_PER_BLOCK / sizeof(uint32_t) }, TensorDType::UINT32, src.GetDeviceId());
        ret = MinMaxGMTensorMalloc(minValGM, maxValGM, minLocGM, maxLocGM);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Malloc Global Memory Tensor failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = { minValGM, maxValGM, minLocGM, maxLocGM };

        // 3. Execute RunOp
        RunOpParam minMaxLocParam{"MinMaxLoc", srcVec, dstVec};
        ret = RunOp(minMaxLocParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
            return ret;
        }
        LogDebug << opType << ": Run op success.";

        // 4. Set final result
        MinMaxDstTensors minMaxDst = { minVal, maxVal, minLoc, maxLoc };
        MinMaxDstTensors minMaxGM = { minValGM, maxValGM, minLocGM, maxLocGM };
        ret = MinMaxLocDstDataCopy(minMaxDst, minMaxGM, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Dst tensor data copy failed." << GetErrorInfo(ret);
        }

        LogDebug << opType << ": Run op finished.";

        return ret;
    }
}
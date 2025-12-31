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
* Description: Core Operations On Tensors.
* Author: MindX SDK
* Create: 2023
* History: NA
*/

#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/CoreOperationsOnTensors.h"
#include <algorithm>
#include "acl/ops/acl_dvpp.h"
#include "acl/dvpp/hi_dvpp.h"
#include "dvpp/securec.h"
#include "MxBase/Log/Log.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/CommonUtils.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/OpCall.h"
#include "MxBase/E2eInfer/DataType.h"
#include "MxBase/DvppWrapper/DvppWrapperBase.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MbCV/Tensor/Tensor/TensorBuffer/TensorBuffer.h"
#include "ResourceManager/DvppPool/DvppPool.h"

namespace MxBase {
    constexpr size_t HW_SHAPE_SIZE = 2;
    constexpr size_t HWC_SHAPE_SIZE = 3;
    constexpr size_t NHWC_SHAPE_SIZE = 4;
    constexpr size_t HW_HWC_DIMENSION_ZERO = 0;
    constexpr size_t HW_HWC_DIMENSION_ONE = 1;
    constexpr size_t HW_HWC_DIMENSION_TWO = 2;
    constexpr size_t NHWC_DIMENSION_ZERO = 0;
    constexpr size_t NHWC_DIMENSION_ONE = 1;
    constexpr size_t NHWC_DIMENSION_TWO = 2;
    constexpr size_t NHWC_DIMENSION_THREE = 3;
    constexpr size_t SCALAR_SHAPE_SIZE = 1;
    constexpr size_t VSTACK_HSTACK_MIN_SIZE = 2;
    constexpr size_t DVPP_ALIGN_WIDTH = 16;
    constexpr size_t RGB_WIDTH_EXTEND = 3;
    constexpr size_t MIN_TENSOR_DIM = 2;
    constexpr size_t GRAY_WIDTH_EXTEND = 1;
    constexpr size_t MIN_TENSOR_WIDTH = 10;
    constexpr size_t MIN_GRAY_TENSOR_WIDTH = 18;
    constexpr size_t MIN_TENSOR_HEIGHT = 6;
    constexpr size_t MAX_TENSOR_WIDTH = 4096;
    constexpr size_t MAX_TENSOR_HEIGHT = 4096;
    constexpr uint32_t UINT32_MAX_VALUE = 0xffffffff;
    constexpr int SCALAR_DIMENSION_THREE = 3;
    constexpr int SCALAR_DIMENSION_TWO = 2;
    constexpr int SHAPE_DIMENSION_THREE = 3;
    constexpr int SHAPE_DIMENSION_TWO = 2;
    constexpr int SHAPE_DIMENSION_ONE = 1;
    constexpr int SHAPE_DIMENSION_ZERO = 0;
    constexpr uint32_t ONE_CHANNEL = 1;
    constexpr uint32_t THREE_CHANNEL = 3;
    constexpr uint32_t FOUR_CHANNEL = 4;
    constexpr uint32_t MAX_ERODE_ITER = 100;
    constexpr uint32_t MIN_ERODE_ITER = 1;
    constexpr uint32_t MAX_ERODE_KERNEL = 9;
    constexpr uint32_t MIN_ERODE_KERNEL = 3;
    constexpr uint32_t MAX_ERODE_SHAPE = 4096;
    constexpr uint32_t MIN_ERODE_SHAPE = 64;
    const std::pair<int, int> DEFAULT_ANCHOR = std::make_pair(-1, -1);
    constexpr int YUV_HEIGHT_STRIDE = 3;
    constexpr int YUV_HEIGHT_DIV = 2;
    constexpr int YUV_EVEN_ALIGN = 2;
    constexpr int RGBA_CHANNEL_NUM = 4;
    constexpr int RGBA_CHANNEL_R = 0;
    constexpr int RGBA_CHANNEL_G = 1;
    constexpr int RGBA_CHANNEL_B = 2;
    constexpr int RGBA_CHANNEL_A = 3;
    struct InOutFormat {
        ImageFormat inputFormat;
        ImageFormat outputFormat;
    };

    static const std::map<CvtColorMode, InOutFormat> CvtColorMap = {
        {CvtColorMode::COLOR_YUVSP4202GRAY, {ImageFormat::YUV_SP_420, ImageFormat::YUV_400}},
        {CvtColorMode::COLOR_YVUSP4202GRAY, {ImageFormat::YVU_SP_420, ImageFormat::YUV_400}},
        {CvtColorMode::COLOR_YUVSP4202RGB, {ImageFormat::YUV_SP_420, ImageFormat::RGB_888}},
        {CvtColorMode::COLOR_YVUSP4202RGB, {ImageFormat::YVU_SP_420, ImageFormat::RGB_888}},
        {CvtColorMode::COLOR_YUVSP4202BGR, {ImageFormat::YUV_SP_420, ImageFormat::BGR_888}},
        {CvtColorMode::COLOR_YVUSP4202BGR, {ImageFormat::YVU_SP_420, ImageFormat::BGR_888}},
        {CvtColorMode::COLOR_RGB2GRAY, {ImageFormat::RGB_888, ImageFormat::YUV_400}},
        {CvtColorMode::COLOR_BGR2GRAY, {ImageFormat::BGR_888, ImageFormat::YUV_400}},
        {CvtColorMode::COLOR_RGB2BGR, {ImageFormat::RGB_888, ImageFormat::BGR_888}},
        {CvtColorMode::COLOR_BGR2RGB, {ImageFormat::BGR_888, ImageFormat::RGB_888}},
        {CvtColorMode::COLOR_RGB2RGBA, {ImageFormat::RGB_888, ImageFormat::RGBA_8888}},
        {CvtColorMode::COLOR_RGBA2GRAY, {ImageFormat::RGBA_8888, ImageFormat::YUV_400}},
        {CvtColorMode::COLOR_RGBA2RGB, {ImageFormat::RGBA_8888, ImageFormat::RGB_888}},
        {CvtColorMode::COLOR_BGR2YUVSP420, {ImageFormat::BGR_888, ImageFormat::YUV_SP_420}},
        {CvtColorMode::COLOR_RGB2YUVSP420, {ImageFormat::RGB_888, ImageFormat::YUV_SP_420}},
        {CvtColorMode::COLOR_RGB2YVUSP420, {ImageFormat::RGB_888, ImageFormat::YVU_SP_420}},
        {CvtColorMode::COLOR_BGR2YVUSP420, {ImageFormat::BGR_888, ImageFormat::YVU_SP_420}}
    };
    struct CvtColorProperties {
        MxBase::Tensor inputTensor;
        MxBase::Tensor outputTensor;
        bool isYuv420Input;
        bool isYuv420Output;
        bool keepMargin;
        uint32_t* width;
        uint32_t* height;
        MxBase::Rect* validRoiRect;
    };
}

namespace MxBase {
APP_ERROR CheckBatchSplitParams(const Tensor &src, const std::vector<Tensor> &)
{
    LogDebug << "Start run BatchSplit......";
    if (!(DeviceManager::IsAscend310P() || DeviceManager::IsAscend310B())) {
        LogError << "CheckBatchSplitParams: current op only supported on device 310P/310B now, current device is "
                 << DeviceManager::GetSocName() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.IsEmpty()) {
        LogError << "CheckBatchSplitParams: Input tensor is empty, please check !"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetShape().size() != NHWC_SHAPE_SIZE && src.GetShape().size() != HWC_SHAPE_SIZE) {
        LogError << "The src must be 4(NHWC/NCHW) or 3(HWC) dims, but get dims(" << src.GetShape().size() << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetDataType() != TensorDType::FLOAT32 && src.GetDataType() != TensorDType::FLOAT16 &&
        src.GetDataType() != TensorDType::UINT8) {
        LogError << "Only support dataType float16/float32/uint8, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR CheckBatchSplitDst(const std::vector<Tensor> &dst)
{
    for (auto tensor : dst) {
        if (tensor.IsEmpty()) {
            LogError << "CheckBatchSplitParams: Dst exist empty tensor, please check !"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR CheckBatchSplitDevice(const Tensor &src, std::vector<Tensor> &, AscendStream &stream)
{
    if (src.GetMemoryType() == MemoryData::MEMORY_DEVICE || src.GetMemoryType() == MemoryData::MEMORY_DVPP) {
        if (src.GetDeviceId() != stream.GetDeviceId()) {
            LogError << "CheckBatchSplitDevice: Input Tensor Device("
                     << src.GetDeviceId() << ") and stream Device("
                     << stream.GetDeviceId() << ") do not match, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR BatchSplitProcess(const Tensor &src, std::vector<Tensor> &dst, std::vector<uint32_t> &dstShape,
                            size_t batchSize, bool isReplace)
{
    APP_ERROR ret = APP_ERR_OK;
    size_t srcTensorSize = src.GetByteSize();
    if (batchSize == 0) {
        LogError << "BatchSplit: Current batch dim is 0, please check."
                 << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    size_t dstTensorSize = srcTensorSize / batchSize;

    // 1. Get srcTensor info
    void *tensorPtr = src.GetData();
    auto tensor_type = src.GetMemoryType();
    int32_t deviceId = src.GetDeviceId();
    TensorDType tensorDType = src.GetDataType();

    // 2. Split Tensor
    uint32_t offset = 0;
    if (!isReplace) {
        for (size_t i = 0; i < batchSize; i++) {
            // 1. Calculate memory size
            uint8_t *ptr = static_cast<uint8_t *>(tensorPtr) + offset;
            offset += dstTensorSize;

            // 2. Create TensorBuffer
            TensorBuffer srcBuffer = TensorBuffer(static_cast<void *>(ptr), dstTensorSize, tensor_type, deviceId);
            TensorBuffer dstBuffer = TensorBuffer(dst[i].GetData(), dstTensorSize, tensor_type, deviceId);

            // 3. Copy
            ret = TensorBuffer::TensorBufferCopy(dstBuffer, srcBuffer);
            if (ret != APP_ERR_OK) {
                LogError << "BatchSplit: copy src tensor buffer failed." << GetErrorInfo(ret);
                return APP_ERR_COMM_ALLOC_MEM;
            }
        }
    } else {
        if (dst.empty()) {
            dst.resize(batchSize);
        }
        for (size_t i = 0; i < batchSize; i++) {
            uint8_t *ptr = static_cast<uint8_t *>(tensorPtr) + offset;
            offset += dstTensorSize;
            Tensor dstTensor(static_cast<void *>(ptr), dstShape, tensorDType, deviceId, false, true);
            dst[i] = dstTensor;
        }
    }
    return ret;
}

void CallBack::batchSplitCallBackFunc(void *arg)
{
    APP_ERROR ret = APP_ERR_OK;
    BatchSplitCallbackParam *input = static_cast<BatchSplitCallbackParam *>(arg);
    ret = BatchSplitProcess(input->src, *(input->dst), input->dstShape, input->batchSize, input->isReplace);
    delete input;
    if (ret != APP_ERR_OK) {
        LogError << "BatchSplit: Failed to execute BatchSplitProcess." << GetErrorInfo(ret);
    }
}

APP_ERROR BatchSplit(const Tensor &src, std::vector<Tensor> &dst, bool isReplace, AscendStream &stream)
{
    APP_ERROR ret = CheckBatchSplitParams(src, dst);
    if (ret != APP_ERR_OK) {
        LogError << "BatchSplit: Fail to check BatchSplit parameters." << GetErrorInfo(ret);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // 1. Get dst shape
    std::vector<uint32_t> shape = src.GetShape();
    size_t dstShapeSize = shape.size() - 1;
    std::vector<uint32_t> dstShape(shape.end() - dstShapeSize, shape.end());
    size_t batchSize = static_cast<size_t>(shape[0]);

    // 2. reallocated without memory reuse
    if (!isReplace || !dst.empty()) {
        ret = CheckBatchSplitDst(dst);
        if (ret != APP_ERR_OK) {
            LogError << "BatchSplit: Check dst failed." << GetErrorInfo(ret);
            return ret;
        }
        ExpectedTensorInfo expectedTensorInfo = {dstShape, src.GetDataType(), src.GetDeviceId()};
        ret = OperatorImplicitMallocVector(dst, batchSize, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << "BatchSplit: implicit malloc failed." << GetErrorInfo(ret);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }

    ret = CheckBatchSplitDevice(src, dst, stream);
    if (ret != APP_ERR_OK) {
        LogError << "BatchSplit: Check device Id failed." << GetErrorInfo(ret);
        return ret;
    }

    // 3. Execute BatchSplit
    if (stream.isDefault_ or isReplace) {
        LogDebug << "AscendStream is default, ready to execute BatchSplitProcess.";
        ret = BatchSplitProcess(src, dst, dstShape, batchSize, isReplace);
        if (ret != APP_ERR_OK) {
            LogError << "BatchSplit: Failed to execute BatchSplitProcess." << GetErrorInfo(ret);
        }
    } else {
        CallBack callback;
        BatchSplitCallbackParam *batchSplitCallbackParam = new BatchSplitCallbackParam{src, &dst, dstShape,
                                                                                       batchSize,
                                                                                       isReplace};
        ret = aclrtLaunchCallback(CallBack::batchSplitCallBackFunc, static_cast<void *>(batchSplitCallbackParam),
                                  ACL_CALLBACK_BLOCK, stream.stream);
        if (ret != APP_ERR_OK) {
            delete batchSplitCallbackParam;
            LogError << "BatchSplit: Failed to execute callback." << GetErrorInfo(ret, "aclrtLaunchCallback");
            return APP_ERR_COMM_FAILURE;
        }
    }
    return ret;
}

static APP_ERROR CheckHstackShape(const std::vector<Tensor> &srcVec, size_t &widthDim, uint32_t &widthSum)
{
    APP_ERROR ret = APP_ERR_OK;
    auto srcShape = srcVec[0].GetShape();
    if (srcVec.size() < VSTACK_HSTACK_MIN_SIZE) {
        LogError << "Hstack srcVec size should be 2 or more, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (auto src : srcVec) {
        if (src.GetShape().size() == SCALAR_SHAPE_SIZE) {
            LogError << "Hstack shape only support HW/HWC/NHWC, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    if (srcVec[0].GetShape().size() == NHWC_SHAPE_SIZE) {
        widthDim = NHWC_DIMENSION_TWO;
    }
    for (auto src : srcVec) {
        for (size_t i = 0; i < src.GetShape().size(); ++i) {
            if (i == widthDim && src.GetShape()[i] > UINT32_MAX_VALUE - widthSum) {
                LogError << "CheckGeneralOpParams: Input Tensors width over max value, please check!"
                            << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            } else if (i == widthDim) {
                widthSum += src.GetShape()[widthDim];
            } else if (src.GetShape()[i] != srcShape[i]) {
                LogError << "CheckGeneralOpParams: Input Tensors have different shapes(except dimension of width)!"
                            << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
    }
    return ret;
}

APP_ERROR Hstack(const std::vector<Tensor> &tv, Tensor &dst, AscendStream &stream)
{
    LogDebug << "Start to execute Hstack op.";
    std::string opType = "ConcatD";

    // 1. Check parameters.
    OpSupportDtype opSupportDtype;
    APP_ERROR ret = CheckGeneralOpParams(tv, opSupportDtype, true, false);
    if (ret != APP_ERR_OK) {
        LogError << "Hstack: Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }
    auto srcShape = tv[0].GetShape();
    size_t widthDim = HW_HWC_DIMENSION_ONE;
    uint32_t widthSum = 0;
    ret = CheckHstackShape(tv, widthDim, widthSum);
    if (ret != APP_ERR_OK) {
        LogError << "Hstack: Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }

    // 2. OperatorImplicitMalloc
    srcShape[widthDim] = widthSum;
    ExpectedTensorInfo expectedTensorInfo = {srcShape, tv[0].GetDataType(), tv[0].GetDeviceId()};
    ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Hstack: Implicit malloc failed or dst tensor check failed." << GetErrorInfo(ret);
        return ret;
    }
    std::vector<Tensor> dstVec = {dst};

    // 3. Construct the op compile description
    std::vector<OpAttrDesc> opAttrDescs;
    auto srcSize = tv.size();
    opAttrDescs.push_back(OpAttrDesc(OpAttrType::INT, "N", static_cast<void *>(&srcSize)));
    opAttrDescs.push_back(OpAttrDesc(OpAttrType::INT, "concat_dim", static_cast<void *>(&widthDim)));

    // 4. Execute RunOp
    RunOpParam hstackParam{opType, tv, dstVec};
    hstackParam.fixedType = "dsl";
    hstackParam.dslParam.dslCompileOptValue = "enable";
    hstackParam.dslParam.opAttrDesc = opAttrDescs;
    ret = RunOp(hstackParam, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Hstack: Run op failed." << GetErrorInfo(ret);
    } else {
        LogDebug << "Hstack: Run op success.";
    }

    LogDebug << "Hstack: Run op finished.";
    return ret;
}

static APP_ERROR CheckVstackShape(const std::vector<Tensor> &srcVec, size_t &heightDim, uint32_t &heightSum)
{
    APP_ERROR ret = APP_ERR_OK;
    auto srcShape = srcVec[0].GetShape();
    if (srcVec.size() < VSTACK_HSTACK_MIN_SIZE) {
        LogError << "Vstack srcVec size should be 2 or more, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (auto src : srcVec) {
        if (src.GetShape().size() == SCALAR_SHAPE_SIZE) {
            LogError << "Vstack shape only support HW/HWC/NHWC, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    if (srcVec[0].GetShape().size() == NHWC_SHAPE_SIZE) {
        heightDim = NHWC_DIMENSION_ONE;
    }
    for (auto src : srcVec) {
        for (size_t i = 0; i < src.GetShape().size(); ++i) {
            if (i == heightDim && src.GetShape()[i] > UINT32_MAX_VALUE - heightSum) {
                LogError << "CheckGeneralOpParams: Input Tensors width over max value, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            } else if (i == heightDim) {
                heightSum += src.GetShape()[heightDim];
            } else if (src.GetShape()[i] != srcShape[i]) {
                LogError << "CheckGeneralOpParams: Input Tensors have different shapes(except dimension of height)!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
    }
    return ret;
}

APP_ERROR Vstack(const std::vector<Tensor> &tv, Tensor &dst, AscendStream &stream)
{
    LogDebug << "Start to execute Vstack op.";
    std::string opType = "ConcatD";

    // 1. Check parameters.
    OpSupportDtype opSupportDtype;
    APP_ERROR ret = CheckGeneralOpParams(tv, opSupportDtype, true, false);
    if (ret != APP_ERR_OK) {
        LogError << "Vstack: Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }
    auto srcShape = tv[0].GetShape();
    size_t heightDim = HW_HWC_DIMENSION_ZERO;
    uint32_t heightSum = 0;
    ret = CheckVstackShape(tv, heightDim, heightSum);
    if (ret != APP_ERR_OK) {
        LogError << "Vstack: Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }

    // 2. OperatorImplicitMalloc
    srcShape[heightDim] = heightSum;
    ExpectedTensorInfo expectedTensorInfo = {srcShape, tv[0].GetDataType(), tv[0].GetDeviceId()};
    ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Vstack: Implicit malloc failed or dst tensor check failed." << GetErrorInfo(ret);
        return ret;
    }
    std::vector<Tensor> dstVec = {dst};

    // 3. Construct the op compile description
    std::vector<OpAttrDesc> opAttrDescs;
    auto srcSize = tv.size();
    opAttrDescs.push_back(OpAttrDesc(OpAttrType::INT, "N", static_cast<void *>(&srcSize)));
    opAttrDescs.push_back(OpAttrDesc(OpAttrType::INT, "concat_dim", static_cast<void *>(&heightDim)));

    // 4. Execute RunOp
    RunOpParam vstackParam{opType, tv, dstVec};
    vstackParam.fixedType = "dsl";
    vstackParam.dslParam.dslCompileOptValue = "enable";
    vstackParam.dslParam.opAttrDesc = opAttrDescs;
    ret = RunOp(vstackParam, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Vstack: Run op failed." << GetErrorInfo(ret);
    } else {
        LogDebug << "Vstack: Run op success.";
    }

    LogDebug << "Vstack: Run op finished.";
    return ret;
}

static APP_ERROR CheckTransposeAxes(const Tensor &src, std::vector<int>& axes)
{
    auto srcShape = src.GetShape();
    if (axes.size() == 0) {  // If axes is None, reverse the Tensor
        for (size_t i = 0; i < srcShape.size(); i++) {
            axes.push_back(srcShape.size() - i - 1);
        }
    }
    if (srcShape.size() != axes.size()) {
        LogError << "CheckTransposeAxes: src Tensor and axes config dimensions fail to match, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    auto axesTemp = axes;
    std::sort(axesTemp.begin(), axesTemp.end());
    for (size_t i = 0; i < axesTemp.size(); i++) {
        if (static_cast<size_t>(axesTemp[i]) != i) {
            LogError << "CheckTransposeAxes: axes vector has invalid number, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR Transpose(const Tensor &src, Tensor &dst, std::vector<int> axes, AscendStream &stream)
{
    LogDebug << "Start to execute Transpose op.";
    std::string opType = "Transpose";

    // 1. Check parameters.
    std::vector<Tensor> srcVec = {src};
    OpSupportDtype opSupportDtype;
    APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
    if (ret != APP_ERR_OK) {
        LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }

    ret = CheckTransposeAxes(src, axes);
    if (ret != APP_ERR_OK) {
        LogError << "Transpose: Fail to check Transpose axes." << GetErrorInfo(ret);
        return ret;
    }

    // 2. Create perm Tensor
    std::vector<uint32_t> srcShape2;
    srcShape2.push_back(static_cast<uint32_t>(axes.size()));
    int32_t srcData2[axes.size()];
    for (size_t i = 0; i < axes.size(); i++) {
        srcData2[i] = axes[i];
    }
    Tensor perm(&srcData2, srcShape2, TensorDType::INT32);
    ret = perm.ToDevice(src.GetDeviceId());
    if (ret != APP_ERR_OK) {
        LogError << opType << ": Tensor to device failed." << GetErrorInfo(ret);
        return ret;
    }

    // 3. OperatorImplicitMalloc
    std::vector<uint32_t> dstShape;
    for (size_t i = 0; i < axes.size(); i++) {
        dstShape.push_back(src.GetShape()[axes[i]]);
    }
    ExpectedTensorInfo expectedTensorInfo = {dstShape, src.GetDataType(), src.GetDeviceId()};
    ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
    if (ret != APP_ERR_OK) {
        LogError << opType << ": Implicit malloc failed or dst tensor check failed." << GetErrorInfo(ret);
        return ret;
    }
    std::vector<Tensor> dstVec = {dst};

    // 4. Execute RunOp
    CommonAclnnPara ascendcParam{axes, std::vector<float>{}};
    RunOpParam transposeParam{opType, srcVec, dstVec};
    transposeParam.ascendcParam = ascendcParam;
    transposeParam.dslParam.dslPropertyVec = {perm};
    ret = RunOp(transposeParam, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Transpose: Run op failed." << GetErrorInfo(ret);
    } else {
        LogDebug << "Transpose: Run op success.";
    }

    LogDebug << "Transpose: Run op finished.";
    return ret;
}

int SplitTensorDim(const Tensor &src)
{
    if (src.GetShape().size() == NHWC_SHAPE_SIZE) {
        return SCALAR_DIMENSION_THREE;
    }
    return SCALAR_DIMENSION_TWO;
}

void GetSplitOutputTensorShape(const Tensor& src, std::vector<uint32_t>& dstTensorShape)
{
    std::vector<uint32_t> shape = src.GetShape();
    for (size_t i = 0; i < shape.size() - 1; i++) {
        dstTensorShape.push_back(shape[i]);
    }
    dstTensorShape.push_back(SHAPE_DIMENSION_ONE);
}

APP_ERROR CheckSplitParams(const Tensor &src, const std::vector<Tensor> &, const AscendStream&)
{
    OpSupportDtype opSupportDtype;
    std::vector<Tensor> srcVec = {src};
    APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
    if (ret != APP_ERR_OK) {
        LogError << "CheckSplitParams: Failed to check split params." << GetErrorInfo(ret);
        return ret;
    }
    if ((src.GetShape().size() < HWC_SHAPE_SIZE) || (src.GetShape().size() > NHWC_SHAPE_SIZE)) {
        LogError << "CheckSplitParams: src dimension format should be HWC or NHWC, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    uint32_t srcChnNum = src.GetShape()[src.GetShape().size() - 1];
    if ((srcChnNum != HWC_SHAPE_SIZE) && (srcChnNum != NHWC_SHAPE_SIZE)) {
        LogError << "CheckSplitParams: The src channel number should be 3 or 4, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR Split(const Tensor &src, std::vector<Tensor> &tv, AscendStream &stream)
{
    LogDebug << "Start to execute Split op.";
    std::string opType = "SplitD";
    std::vector<Tensor> srcVec = {src};

    // 1. Check parameters.
    APP_ERROR ret = CheckSplitParams(src, tv, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Split: Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }

    // 2. OperatorImplicitMalloc
    std::vector<uint32_t> dstTensorShape;
    int splitDim = SplitTensorDim(src);
    int splitNum = static_cast<int>(src.GetShape()[splitDim]);
    GetSplitOutputTensorShape(src, dstTensorShape);
    ExpectedTensorInfo expectedTensorInfo = {dstTensorShape, src.GetDataType(), src.GetDeviceId()};
    ret = OperatorImplicitMallocVector(tv, splitNum, expectedTensorInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Split: Implicit malloc failed." << GetErrorInfo(ret);
        return ret;
    }

    // 3. Set attr
    OpAttrDesc dimOpAttrDesc = {OpAttrType::INT, "split_dim", &splitDim};
    OpAttrDesc numOpAttrDesc = {OpAttrType::INT, "num_split", &splitNum};
    std::vector<OpAttrDesc> opAttrDesc = {dimOpAttrDesc, numOpAttrDesc};

    // 4. Execute RunOp
    int splitSections = 1;
    CommonAclnnPara ascendcParam{std::vector<int>{splitDim, splitSections}, std::vector<float>{}};
    RunOpParam splitParam{opType, srcVec, tv};
    splitParam.ascendcParam = ascendcParam;
    splitParam.dslParam.dslCompileOptValue = "enable";
    splitParam.dslParam.opAttrDesc = opAttrDesc;
    ret = RunOp(splitParam, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Split: Run op failed." << GetErrorInfo(ret);
    } else {
        LogDebug << "Split: Run op success.";
    }

    LogDebug << "Split: Run op finished.";
    return ret;
}

static APP_ERROR CheckMergeSrcVecExtra(const std::vector<Tensor> &tv, uint32_t &sumSrcChannel)
{
    if (tv.size() <= 1) {
        LogError << "Tv vector should have more than 1 tensor." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::vector<uint32_t> standShape = tv[0].GetShape();
    if (standShape.size() == HWC_SHAPE_SIZE) {
        for (auto t : tv) {
            if (t.GetShape().size() != standShape.size() ||
                t.GetShape()[HW_HWC_DIMENSION_ZERO] != standShape[HW_HWC_DIMENSION_ZERO] ||
                t.GetShape()[HW_HWC_DIMENSION_ONE] != standShape[HW_HWC_DIMENSION_ONE]) {
                LogError << "CheckMergeSrcVecExtra: Input tensors shape don't match, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            sumSrcChannel += t.GetShape()[HW_HWC_DIMENSION_TWO];
        }
    } else if (standShape.size() == NHWC_SHAPE_SIZE) {
        for (auto t : tv) {
            if (t.GetShape().size() != standShape.size() ||
                t.GetShape()[NHWC_DIMENSION_ZERO] != standShape[NHWC_DIMENSION_ZERO] ||
                t.GetShape()[NHWC_DIMENSION_ONE] != standShape[NHWC_DIMENSION_ONE] ||
                t.GetShape()[NHWC_DIMENSION_TWO] != standShape[NHWC_DIMENSION_TWO]) {
                LogError << "CheckMergeSrcVecExtra: Input tensors shape don't match, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            sumSrcChannel += t.GetShape()[NHWC_DIMENSION_THREE];
        }
    } else {
        LogError << "CheckMergeSrcVecExtra: Input Tensor dimension format should be HWC or NHWC, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((sumSrcChannel != THREE_CHANNEL) && (sumSrcChannel != FOUR_CHANNEL)) {
        LogError << "CheckMergeSrcVecExtra: Sum of input tensors channel should be 3 or 4, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static int MergeTensorDim(const std::vector<Tensor> &tv)
{
    if (tv[0].GetShape().size() == NHWC_SHAPE_SIZE) {
        return SCALAR_DIMENSION_THREE;
    }
    return SCALAR_DIMENSION_TWO;
}

APP_ERROR Merge(const std::vector<Tensor> &tv, Tensor &dst, AscendStream &stream)
{
    LogDebug << "Start to execute Merge op.";
    std::string opType = "ConcatD";
    std::vector<Tensor> srcVec = tv;

    // 1. Check parameters.
    OpSupportDtype opSupportDtype;
    APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype, true, false);
    if (ret != APP_ERR_OK) {
        LogError << "Merge" << ": Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }
    uint32_t sumSrcChannel = 0;
    ret = CheckMergeSrcVecExtra(srcVec, sumSrcChannel);
    if (ret != APP_ERR_OK) {
        LogError << "Merge" << ": Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }

    // 2. OperatorImplicitMalloc
    std::vector<uint32_t> dstShape;
    for (size_t i = 0; i < srcVec[0].GetShape().size() - 1; ++i) {
        dstShape.push_back(srcVec[0].GetShape()[i]);
    }
    dstShape.push_back(sumSrcChannel);
    ExpectedTensorInfo expectedTensorInfo = {dstShape, srcVec[0].GetDataType(), srcVec[0].GetDeviceId()};
    ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Merge" << ": Implicit malloc failed or dst tensor check failed." << GetErrorInfo(ret);
        return ret;
    }
    std::vector<Tensor> dstVec = {dst};

    // 3. Set attr
    size_t inputSize = srcVec.size();
    int mergeSize = MergeTensorDim(srcVec);
    OpAttrDesc numOpAttrDesc = {OpAttrType::INT, "N", static_cast<void *>(&inputSize)};
    OpAttrDesc dimOpAttrDesc = {OpAttrType::INT, "concat_dim", static_cast<void *>(&mergeSize)};
    std::vector<OpAttrDesc> opAttrDesc = {dimOpAttrDesc, numOpAttrDesc};

    // 4. Execute RunOp
    CommonAclnnPara ascendcParam{std::vector<int>{mergeSize}, std::vector<float>{}};
    RunOpParam mergeParam{opType, srcVec, dstVec};
    mergeParam.ascendcParam = ascendcParam;
    mergeParam.dslParam.dslCompileOptValue = "enable";
    mergeParam.dslParam.opAttrDesc = opAttrDesc;
    ret = RunOp(mergeParam, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Merge: Run op failed." << GetErrorInfo(ret);
    } else {
        LogDebug << "Merge: Run op success.";
    }

    LogDebug << "Merge: Run op finished.";
    return ret;
}

APP_ERROR CheckTileOpParams(const Tensor &src, const std::vector <uint32_t> &multiples)
{
    std::vector<uint32_t> inputShape = src.GetShape();
    if (inputShape.size() != multiples.size()) {
        LogError << "CheckTileOpParams: The dimension of src is different from the size of multiples, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR GetMultiplesTensor(const Tensor &src, Tensor &multiplesTensor, const std::vector <uint32_t> &multiples)
{
    std::vector<uint32_t> multiplesShape = {static_cast<uint32_t>(multiples.size())};
    int32_t value[multiples.size()];
    for (size_t i = 0; i < multiples.size(); ++i) {
        value[i] = static_cast<int32_t>(multiples[i]);
    }
    Tensor TempMultiplesTensor(&value[0], multiplesShape, MxBase::TensorDType::INT32);
    multiplesTensor = TempMultiplesTensor;
    APP_ERROR ret = multiplesTensor.ToDevice(src.GetDeviceId());
    if (ret != APP_ERR_OK) {
        LogError <<"GetMultiplesTensor: Tensor to device failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR Tile(const Tensor &src, Tensor &dst, const std::vector <uint32_t> &multiples, AscendStream& stream)
{
    LogDebug << "Start to execute Tile op.";
    std::string opType = "Tile";
    std::vector<Tensor> srcVec = {src};

    // 1. Check parameters.
    OpSupportDtype opSupportDtype;
    APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
    if (ret != APP_ERR_OK) {
        LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckTileOpParams(src, multiples);
    if (ret != APP_ERR_OK) {
        LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }

    // 2. OperatorImplicitMalloc
    std::vector <uint32_t> dstShape;
    std::vector<uint32_t> inputShape = src.GetShape();
    for (size_t i = 0; i < inputShape.size(); ++i) {
        dstShape.push_back(inputShape[i] * multiples[i]);
    }
    ExpectedTensorInfo expectedTensorInfo = {dstShape, src.GetDataType(), src.GetDeviceId()};
    ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
    if (ret != APP_ERR_OK) {
        LogError << opType << ": Implicit malloc failed or dst tensor check failed." << GetErrorInfo(ret);
        return ret;
    }
    std::vector<Tensor> dstVec = {dst};

    // 3. Get the multiplesTensor
    Tensor multiplesTensor;
    ret = GetMultiplesTensor(src, multiplesTensor, multiples);
    if (ret != APP_ERR_OK) {
        LogError << opType << ": GetMultiplesTensor failed." << GetErrorInfo(ret);
        return ret;
    }

    // 4. Execute RunOp
    RunOpParam tileParam{opType, srcVec, dstVec};
    tileParam.fixedType = "dsl";
    tileParam.dslParam.dslPropertyVec = {multiplesTensor};
    ret = RunOp(tileParam, stream);
    if (ret != APP_ERR_OK) {
        LogError << opType << ": Run op failed." << GetErrorInfo(ret);
    } else {
        LogDebug << opType << ": Run op success.";
    }

    LogDebug << opType << ": Run op finished.";
    return ret;
}

APP_ERROR CheckErodeOpParams(const Tensor &src, const BlurConfig &blurconfig)
{
    std::vector<uint32_t> inputShape = src.GetShape();
    if (inputShape.size() != HWC_SHAPE_SIZE && inputShape.size() != HW_SHAPE_SIZE) {
        LogError << "CheckErodeOpParams: The dimension of src is not HWC or HW, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (inputShape[0] > MAX_ERODE_SHAPE || inputShape[0] < MIN_ERODE_SHAPE ||
       inputShape[1] > MAX_ERODE_SHAPE || inputShape[1] < MIN_ERODE_SHAPE) {
        LogError << "CheckErodeOpParams: The src shape is out of range, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (inputShape.size() == HWC_SHAPE_SIZE) {
        uint32_t channel = inputShape[HW_HWC_DIMENSION_TWO];
        if (channel != FOUR_CHANNEL && channel != THREE_CHANNEL && channel != ONE_CHANNEL) {
            LogError << "CheckErodeOpParams: The channel num of src should be 1, 3, 4, please check!"
                    << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }

    if (blurconfig.iterations > MAX_ERODE_ITER || blurconfig.iterations < MIN_ERODE_ITER) {
        LogError << "CheckErodeOpParams: The iterations of erode should in range [1, 100], please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (blurconfig.kernelSize.height > MAX_ERODE_KERNEL || blurconfig.kernelSize.height < MIN_ERODE_KERNEL ||
        blurconfig.kernelSize.height != blurconfig.kernelSize.width) {
        LogError << "CheckErodeOpParams: The kernel height should in range [3, 9]"
                 << " and kernel height should equal to width, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (blurconfig.morphShape != MorphShape::MORPH_RECT) {
        LogError << "CheckErodeOpParams: The morphShape of erode only support MORPH_RECT, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (blurconfig.borderType != BorderType::BORDER_REPLICATE) {
        LogError << "CheckErodeOpParams: The BorderType of erode only support BORDER_REPLICATE, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (blurconfig.anchor.first != DEFAULT_ANCHOR.first && blurconfig.anchor.second != DEFAULT_ANCHOR.second) {
        LogError << "CheckErodeOpParams: The anchor of erode only support (-1, -1), please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR RunErode(const Tensor &src, Tensor &dst, RunOpParam &erodeParam, const uint32_t &iter, AscendStream &stream)
{
    if (iter == 1) {
        erodeParam.srcVec = {src};
        erodeParam.dstVec = {dst};
        return RunOp(erodeParam, stream);
    }

    APP_ERROR ret = APP_ERR_OK;
    Tensor dstTmp = src.Clone(stream); // store erode intermediate results
    for (size_t i = 0; i < iter; i++) {
        if (i & 1) { // last iter's output will be next input
            erodeParam.srcVec = {dst};
            erodeParam.dstVec = {dstTmp};
        } else {
            erodeParam.srcVec = {dstTmp};
            erodeParam.dstVec = {dst};
        }
        ret = RunOp(erodeParam, stream);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    if (!(iter & 1)) { // if iter is an odd num, results store in dstTmp
        dst = dstTmp.Clone(stream);
    }
    return ret;
}

APP_ERROR Erode(const Tensor &src, Tensor &dst, const BlurConfig &blurconfig, AscendStream &stream)
{
    LogDebug << "Start to execute Erode op";
    std::string opType = "Erode";

    // 1. Check parameters.
    OpSupportDtype opSupportDtype;
    std::vector<Tensor> srcVec = {src};
    APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
    if (ret != APP_ERR_OK) {
        LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }

    ret = CheckErodeOpParams(src, blurconfig);
    if (ret != APP_ERR_OK) {
        LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }
    // 2. OperatorImplicitMalloc
    ExpectedTensorInfo expectedTensorInfo = {src.GetShape(), src.GetDataType(), src.GetDeviceId()};
    ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
    if (ret != APP_ERR_OK) {
        LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
        return ret;
    }

    // 3. construct erode attr
    std::vector<uint32_t> erodeShape = src.GetShape();
    uint32_t iter = blurconfig.iterations;
    int kernel = static_cast<int>(blurconfig.kernelSize.width);
    uint32_t height = erodeShape[HW_HWC_DIMENSION_ZERO];
    uint32_t width = erodeShape[HW_HWC_DIMENSION_ONE];
    RunOpParam erodeParam{opType};
    erodeParam.ascendcParam = {std::vector<int>{kernel, static_cast<int>(height), static_cast<int>(width)},
                               std::vector<float>{}};

    // 4. Execute RunOp
    if (erodeShape.size() == HWC_SHAPE_SIZE && erodeShape[HW_HWC_DIMENSION_TWO] > ONE_CHANNEL) {
        uint32_t channel = erodeShape[HW_HWC_DIMENSION_TWO];
        std::vector<MxBase::Tensor> erodeSrcVec = {};
        std::vector<MxBase::Tensor> erodeDstVec = {};
        ret = MxBase::Split(src, erodeSrcVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to Split tensor in Erode operation."  << GetErrorInfo(ret);
            return ret;
        }
        ret = AddStreamRef(erodeSrcVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Failed to add stream reference." << GetErrorInfo(ret);
            return ret;
        }
        for (size_t i = 0; i < channel; i++) {
            std::vector<uint32_t> dstShape = {height, width, 1};
            Tensor dstTmp;
            ExpectedTensorInfo expectedDstTensorInfo = {dstShape, src.GetDataType(), src.GetDeviceId()};
            ret = OperatorImplicitMallocTensor(dstTmp, expectedDstTensorInfo);
            if (ret != APP_ERR_OK) {
                LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
                return ret;
            }
            erodeDstVec.push_back(dstTmp);

            ret = RunErode(erodeSrcVec[i], erodeDstVec[i], erodeParam, iter, stream);
            if (ret != APP_ERR_OK) {
                LogError << opType << ": Run op failed." << GetErrorInfo(ret);
                return ret;
            }
        }

        ret = MxBase::Merge(erodeDstVec, dst, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to Merge tensor in Erode operation."  << GetErrorInfo(ret);
            return ret;
        }
    } else {
        ret = RunErode(src, dst, erodeParam, iter, stream);
    }

    if (ret != APP_ERR_OK) {
        LogError << opType <<": Run op failed." << GetErrorInfo(ret);
    } else {
        LogDebug << opType << ": Run op success.";
    }
    LogDebug << opType << ": Run op finished.";
    return ret;
}

bool IsCvtColorWidthAligned(const Tensor &inputTensor)
{
    if (inputTensor.IsEmpty()) {
        return false;
    }
    size_t inputWidth = inputTensor.GetShape()[SHAPE_DIMENSION_ONE];
    return inputWidth % DVPP_ALIGN_WIDTH == 0;
}

bool IsCvtColorMemoryReusable(const Tensor &tensor)
{
    return IsCvtColorWidthAligned(tensor) && tensor.GetMemoryType() == MemoryData::MemoryType::MEMORY_DVPP;
}

APP_ERROR GetChannelId(AscendStream& stream, hi_vpc_chn& chnId)
{
    APP_ERROR ret = stream.CreateChannel();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to create StreamChannel." << GetErrorInfo(ret);
        return ret;
    }
    ret = stream.GetChannel(&chnId);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to GetChannelId." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR CheckCvtColorTensorHW(const Tensor &tensor)
{
    if (tensor.GetShape().size() < MIN_TENSOR_DIM || tensor.GetShape().size() > HWC_SHAPE_SIZE) {
        LogError << "Invalid tensor shape, should be HWC or HW." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    size_t tensorWidth = tensor.GetShape()[SHAPE_DIMENSION_ONE];
    size_t tensorHeight = tensor.GetShape()[SHAPE_DIMENSION_ZERO];
    if (tensorWidth < MIN_TENSOR_WIDTH || tensorWidth > MAX_TENSOR_WIDTH) {
        LogError << "Get invalid tensor width(" << tensorWidth << "), " << "which should be in the range of ["
                 << MIN_TENSOR_WIDTH << ", " << MAX_TENSOR_WIDTH << "]. "
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (tensorHeight < MIN_TENSOR_HEIGHT || tensorHeight > MAX_TENSOR_HEIGHT) {
        LogError << "Get invalid tensor height(" << tensorHeight << "), " << "which should be in the range of ["
                 << MIN_TENSOR_HEIGHT << ", " << MAX_TENSOR_HEIGHT << "]. "
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    bool isGray = tensor.GetShape().size() == SHAPE_DIMENSION_TWO || (
        tensor.GetShape().size() == SHAPE_DIMENSION_THREE &&
        tensor.GetShape()[SHAPE_DIMENSION_TWO] == GRAY_WIDTH_EXTEND);
    if (isGray && tensorWidth < MIN_GRAY_TENSOR_WIDTH) {
        LogError << "Get invalid tensor width(" << tensorWidth << "), " << "which should be in the range of ["
                 << MIN_GRAY_TENSOR_WIDTH << ", " << MAX_TENSOR_WIDTH << "] when input format is YUV_400"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR CheckCvtColorInputParams(const Tensor &src, Tensor &dst, AscendStream &stream)
{
    if (src.IsEmpty()) {
        LogError << "Input tensor should not be empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetMemoryType() != MemoryData::MemoryType::MEMORY_DVPP &&
        src.GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE) {
        LogError << "Input tensor cannot be on the host, only support dvpp or device memory, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!dst.IsEmpty() && dst.GetMemoryType() != MemoryData::MemoryType::MEMORY_DVPP &&
        dst.GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE) {
        LogError << "Output tensor cannot be on the host if it is not empty, only support dvpp or device memory."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetDeviceId() != stream.GetDeviceId()) {
        LogError << "CheckCvtColorInputParams: Input Tensor Device(" << src.GetDeviceId() << ") and Stream Device("
                 << stream.GetDeviceId() << ") do not match, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!dst.IsEmpty() && dst.GetDeviceId() != stream.GetDeviceId()) {
        LogError << "CheckCvtColorInputParams: Output Tensor Device(" << dst.GetDeviceId() << ") and Stream Device("
                 << stream.GetDeviceId() << ") do not match, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetDataType() != TensorDType::UINT8 || (!dst.IsEmpty() && dst.GetDataType() != TensorDType::UINT8)) {
        LogError << "Unsupported src or dst Datatype, Only support uint8 TensorDType."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (CheckCvtColorTensorHW(src) != APP_ERR_OK) {
        LogError << "Check input tensor height and width failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!dst.IsEmpty() && dst.GetShape().size() != src.GetShape().size()) {
        LogError << "Input tensor shape size must same with the output tensor shape size."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

void FreeCvtColorDataIfNotReuse(hi_vpc_pic_info &inputDesc, hi_vpc_pic_info &outputDesc,
                                bool reuseInputTensor, bool reuseOutputTensor)
{
    if (!reuseInputTensor && inputDesc.picture_address != nullptr) {
        DVPPMemoryFreeFunc(inputDesc.picture_address);
    }
    if (!reuseOutputTensor && outputDesc.picture_address != nullptr) {
        DVPPMemoryFreeFunc(outputDesc.picture_address);
    }
}

static APP_ERROR ReallocInputDesc(hi_vpc_pic_info& inputDesc, const Tensor &inputTensor, uint32_t channel)
{
    auto ret = DVPPMemoryMallocFunc(inputTensor.GetDeviceId(), &inputDesc.picture_address,
                                    inputDesc.picture_buffer_size);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to malloc dvpp memory for input tensor." << GetErrorInfo(ret, "hi_mpi_dvpp_malloc");
        return APP_ERR_ACL_BAD_ALLOC;
    }
    if (IsCvtColorWidthAligned(inputTensor)) {
        // memory align, copy to dvpp directly.
        ret = aclrtMemcpy(inputDesc.picture_address, inputDesc.picture_buffer_size,
                          inputTensor.GetData(), inputDesc.picture_buffer_size, ACL_MEMCPY_DEVICE_TO_DEVICE);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to copy memory for input tensor." << GetErrorInfo(ret, "aclrtMemcpy");
            DVPPMemoryFreeFunc(inputDesc.picture_address);
            return APP_ERR_ACL_BAD_COPY;
        }
    } else {
        // memory not align, copy to dvpp line by line.
        for (size_t h = 0; h < inputDesc.picture_height; h++) {
            size_t offsetTo = h * inputDesc.picture_width_stride;
            size_t offsetFrom = h * inputTensor.GetShape()[1] * channel;
            ret = aclrtMemcpy(static_cast<void*>(static_cast<uint8_t*>(inputDesc.picture_address) + offsetTo),
                              inputDesc.picture_width * channel,
                              static_cast<void*>(static_cast<uint8_t*>(inputTensor.GetData()) + offsetFrom),
                              inputDesc.picture_width * channel, ACL_MEMCPY_DEVICE_TO_DEVICE);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to copy memory for input tensor." << GetErrorInfo(ret, "aclrtMemcpy");
                DVPPMemoryFreeFunc(inputDesc.picture_address);
                return APP_ERR_ACL_BAD_COPY ;
            }
        }
    }
    return ret;
}

void GetChannelByFormat(const ImageFormat fmt, uint32_t &channel)
{
    if (fmt <= ImageFormat::YVU_SP_420) {
        channel = 1;
    } else if (fmt == ImageFormat::RGB_888 || fmt == ImageFormat::BGR_888) {
        channel = RGB_WIDTH_EXTEND;
    } else if (fmt >= ImageFormat::ARGB_8888 && fmt <= ImageFormat::BGRA_8888) {
        channel = RGB_WIDTH_EXTEND + 1;
    }
}

APP_ERROR SetCvtColorInputDesc(hi_vpc_pic_info& inputDesc, const Tensor& inputTensor, const Tensor&,
                               InOutFormat& inOutFormat, bool& reuseInputTensor)
{
    ImageFormat outputFormat = inOutFormat.outputFormat;
    ImageFormat inputFormat = inOutFormat.inputFormat;
    bool isYuv420Output = (outputFormat == ImageFormat::YUV_SP_420 || outputFormat == ImageFormat::YVU_SP_420);
    bool isYuv420Input = (inputFormat == ImageFormat::YUV_SP_420 || inputFormat == ImageFormat::YVU_SP_420);
    bool isYuv420 = isYuv420Output || isYuv420Input;

    APP_ERROR ret = APP_ERR_OK;
    Rect validRoiRect = inputTensor.GetValidRoi();
    uint32_t width = validRoiRect.x1 == 0 ? inputTensor.GetShape()[1] : validRoiRect.x1;
    uint32_t height = validRoiRect.y1 == 0 ? inputTensor.GetShape()[0] : validRoiRect.y1;

    height = isYuv420Input ? height / YUV_HEIGHT_STRIDE * YUV_HEIGHT_DIV : height;

    uint32_t channel = 1;
    GetChannelByFormat(inputFormat, channel);
    inputDesc.picture_format = static_cast<hi_pixel_format>(inputFormat);
    inputDesc.picture_width = isYuv420 ? DVPP_ALIGN_UP(width, YUV_EVEN_ALIGN) : width;
    inputDesc.picture_height = isYuv420 ? DVPP_ALIGN_UP(height, YUV_EVEN_ALIGN) : height;
    inputDesc.picture_width_stride = DVPP_ALIGN_UP(inputTensor.GetShape()[1], DVPP_ALIGN_WIDTH) * channel;
    inputDesc.picture_height_stride = isYuv420Output ? DVPP_ALIGN_UP(inputTensor.GetShape()[0], YUV_EVEN_ALIGN)
                                                     : inputTensor.GetShape()[0];
    inputDesc.picture_height_stride = isYuv420Input ? inputDesc.picture_height_stride / YUV_HEIGHT_STRIDE *
                                                      YUV_HEIGHT_DIV : inputDesc.picture_height_stride;
    inputDesc.picture_buffer_size = isYuv420Input ? inputDesc.picture_width_stride * inputTensor.GetShape()[0] :
                                    inputDesc.picture_width_stride * inputDesc.picture_height_stride;
    if (IsCvtColorMemoryReusable(inputTensor)) {
        inputDesc.picture_address = inputTensor.GetData();
        reuseInputTensor = true;
        return ret;
    }
    // malloc dvpp memory for input tensor
    LogDebug << "Start realloc input picture address.";
    return ReallocInputDesc(inputDesc, inputTensor, channel);
}

APP_ERROR SetCvtColorOutputDesc(hi_vpc_pic_info& cvtOutputDesc, Tensor& outputTensor,
                                InOutFormat& inOutFormat, bool& reuseOutputTensor)
{
    APP_ERROR ret = APP_ERR_OK;
    auto inFormat = inOutFormat.inputFormat;
    auto outFormat = inOutFormat.outputFormat;
    bool isYuv420Input = (inFormat == ImageFormat::YUV_SP_420 || inFormat == ImageFormat::YVU_SP_420);
    bool isYuv420Output = (outFormat == ImageFormat::YUV_SP_420 || outFormat == ImageFormat::YVU_SP_420);
    bool isYuv420 = isYuv420Input || isYuv420Output;
    uint32_t channel = 1;
    GetChannelByFormat(outFormat, channel);
    Rect validRoiRect = outputTensor.GetValidRoi();
    uint32_t width = validRoiRect.x1 == 0 ? outputTensor.GetShape()[1] : validRoiRect.x1;
    uint32_t height = validRoiRect.y1 == 0 ? outputTensor.GetShape()[0] : validRoiRect.y1;
    if (validRoiRect.y1 != 0 && isYuv420Output) {
        height =  height * YUV_HEIGHT_DIV / YUV_HEIGHT_STRIDE;
    }
    cvtOutputDesc.picture_format = static_cast<hi_pixel_format>(outFormat);
    cvtOutputDesc.picture_width = isYuv420 ? DVPP_ALIGN_UP(width, YUV_EVEN_ALIGN) : width;
    cvtOutputDesc.picture_height = isYuv420 ?  DVPP_ALIGN_UP(height, YUV_EVEN_ALIGN) : height;
    cvtOutputDesc.picture_width_stride = DVPP_ALIGN_UP(outputTensor.GetShape()[1], DVPP_ALIGN_WIDTH) * channel;
    cvtOutputDesc.picture_height_stride = std::max(outputTensor.GetShape()[0], cvtOutputDesc.picture_height);
    cvtOutputDesc.picture_height_stride = isYuv420Output ? cvtOutputDesc.picture_height
                                                         : cvtOutputDesc.picture_height_stride;
    cvtOutputDesc.picture_buffer_size = cvtOutputDesc.picture_width_stride * cvtOutputDesc.picture_height_stride;
    cvtOutputDesc.picture_buffer_size = isYuv420Output ? cvtOutputDesc.picture_buffer_size * YUV_HEIGHT_STRIDE /
                                                         YUV_HEIGHT_DIV : cvtOutputDesc.picture_buffer_size;

    if (IsCvtColorMemoryReusable(outputTensor) &&
        (!isYuv420Input || cvtOutputDesc.picture_height_stride == outputTensor.GetShape()[0])) {
        cvtOutputDesc.picture_address = outputTensor.GetData();
        reuseOutputTensor = true;
    } else {
        LogDebug << "Start realloc output picture address.";
        ret = DVPPMemoryMallocFunc(outputTensor.GetDeviceId(), &cvtOutputDesc.picture_address,
                                   cvtOutputDesc.picture_buffer_size);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc dvpp memory for output tensor." << GetErrorInfo(ret);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    return ret;
}

APP_ERROR GenerateCvtColorNewOutputTensor(const Tensor &src, Tensor &dst, std::vector<uint32_t> &newShape,
                                          bool isDvpp, uint32_t widthExtend)
{
    if (!dst.IsEmpty()) {
        Tensor::TensorFree(dst);
    }
    bool shapeWithThreeDim = src.GetShape().size() == HWC_SHAPE_SIZE &&
                             src.GetShape()[SHAPE_DIMENSION_TWO] == GRAY_WIDTH_EXTEND;
    if (newShape.size() < SHAPE_DIMENSION_THREE && (widthExtend > GRAY_WIDTH_EXTEND || shapeWithThreeDim)) {
        newShape.push_back(widthExtend);
    }

    Tensor newOutputTensor(newShape, MxBase::TensorDType::UINT8, src.GetDeviceId(), isDvpp);
    APP_ERROR ret = newOutputTensor.Malloc();
    if (ret != APP_ERR_OK) {
        LogError << "Malloc dvpp memory of output tensor failed." << GetErrorInfo(ret);
        return ret;
    }
    dst = newOutputTensor;
    return APP_ERR_OK;
}

void SetCvtColorOutputProperties(CvtColorProperties &properties)
{
    MxBase::Tensor inputTensor = properties.inputTensor;
    MxBase::Tensor outputTensor = properties.outputTensor;
    bool isYuv420Input = properties.isYuv420Input;
    bool isYuv420Output = properties.isYuv420Output;
    bool keepMargin = properties.keepMargin;
    *properties.validRoiRect = inputTensor.GetValidRoi();
    *properties.width = properties.validRoiRect->x1 == 0 ? inputTensor.GetShape()[1] : properties.validRoiRect->x1;
    *properties.height = properties.validRoiRect->y1 == 0 ? inputTensor.GetShape()[0] : properties.validRoiRect->y1;
    properties.validRoiRect->x1 = isYuv420Output ? DVPP_ALIGN_UP(*properties.width, YUV_EVEN_ALIGN)
                                                 : *properties.width;
    properties.validRoiRect->y1 = isYuv420Output ? DVPP_ALIGN_UP(*properties.height, YUV_EVEN_ALIGN) *
                                                   YUV_HEIGHT_STRIDE / YUV_HEIGHT_DIV
                                                 : *properties.height;
    if (isYuv420Input) {
        *properties.height = *properties.height / YUV_HEIGHT_STRIDE * YUV_HEIGHT_DIV;
        properties.validRoiRect->y1 = properties.validRoiRect->y1 / YUV_HEIGHT_STRIDE * YUV_HEIGHT_DIV;
    }
    if (keepMargin) {
        *properties.height = isYuv420Input ? inputTensor.GetShape()[0] / YUV_HEIGHT_STRIDE * YUV_HEIGHT_DIV
                                           : inputTensor.GetShape()[0];
        *properties.width = DVPP_ALIGN_UP(inputTensor.GetShape()[1], DVPP_ALIGN_WIDTH);
    } else if (!outputTensor.IsEmpty()) {
        *properties.height =
            outputTensor.GetValidRoi().y1 == 0 ? *properties.height : std::min(outputTensor.GetValidRoi().y1,
                                                                               *properties.height);
        *properties.width =
            outputTensor.GetValidRoi().x1 == 0 ? *properties.width : std::min(outputTensor.GetValidRoi().x1,
                                                                              *properties.width);
    }
    *properties.width = isYuv420Output ? DVPP_ALIGN_UP(*properties.width, YUV_EVEN_ALIGN) : *properties.width;
    *properties.height = isYuv420Output ? DVPP_ALIGN_UP(*properties.height, YUV_EVEN_ALIGN) * YUV_HEIGHT_STRIDE /
                                          YUV_HEIGHT_DIV : *properties.height;
}

APP_ERROR SetCvtColorOutputTensor(const Tensor &inputTensor, Tensor &outputTensor, const CvtColorMode &mode,
                                  bool keepMargin)
{
    APP_ERROR ret = APP_ERR_OK;
    std::map<CvtColorMode, InOutFormat> map = CvtColorMap;
    auto iter = map.find(mode);
    InOutFormat inOutFormat = iter->second;
    bool isYuv420Input = mode <= CvtColorMode::COLOR_YVUSP4202BGR;
    bool isYuv420Output = (inOutFormat.outputFormat == ImageFormat::YUV_SP_420 ||
                           inOutFormat.outputFormat == ImageFormat::YVU_SP_420);
    uint32_t widthExtend = 1;
    GetChannelByFormat(inOutFormat.outputFormat, widthExtend);
    uint32_t width = 0;
    uint32_t height = 0;
    MxBase::Rect validRoiRect;
    CvtColorProperties properties = {inputTensor, outputTensor, isYuv420Input, isYuv420Output,
                                     keepMargin, &width, &height, &validRoiRect};
    SetCvtColorOutputProperties(properties);

    uint32_t expectSize = width * height * widthExtend;
    bool realloc = false;
    if (!IsCvtColorMemoryReusable(outputTensor) || expectSize != outputTensor.GetByteSize()) {
        if (keepMargin || width % DVPP_ALIGN_WIDTH == 0) {
            LogDebug << "Remalloc dvpp memory, expect " << expectSize << ", get " << outputTensor.GetByteSize();
            std::vector<uint32_t> newShape = {height, width, widthExtend};
            ret = GenerateCvtColorNewOutputTensor(inputTensor, outputTensor, newShape, true, widthExtend);
            realloc = true;
        } else if (outputTensor.GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE ||
                   expectSize != outputTensor.GetByteSize()) {
            LogDebug << "Remalloc device memory, expect " << expectSize << ", get " << outputTensor.GetByteSize();
            std::vector<uint32_t> newShape = {height, width, widthExtend};
            ret = GenerateCvtColorNewOutputTensor(inputTensor, outputTensor, newShape, false, widthExtend);
            realloc = true;
        }
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to GenerateCvtColorNewOutputTensor in SetCvtOutputTensor operation." << GetErrorInfo(ret);
        return ret;
    }
    if (realloc || (outputTensor.GetValidRoi().x1 == 0 && outputTensor.GetValidRoi().y1 == 0)) {
        ret = outputTensor.SetValidRoi(validRoiRect);
        LogDebug << "Vaild roi of output tensor is set as " << validRoiRect.x1 << ", " << validRoiRect.y1;
    }
    return ret;
}

APP_ERROR VpcConvertColor(const Tensor &inputTensor, hi_vpc_pic_info& cvtInputDesc,
                          hi_vpc_pic_info& cvtOutputDesc, AscendStream &stream)
{
    APP_ERROR ret = APP_ERR_OK;
    uint32_t taskID = 1;
    hi_vpc_chn chnId = 0;
    // Compatible with 310B.
    if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
        ret = DvppPool::GetInstance().GetChn(inputTensor.GetDeviceId(), chnId, DvppChnType::VPC);
    } else {
        ret = GetChannelId(stream, chnId);
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get Vpc channel in CvtColorProcess operation." << GetErrorInfo(ret);
        return ret;
    }
    ret = hi_mpi_vpc_convert_color(chnId, &cvtInputDesc, &cvtOutputDesc, &taskID, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute convert color." << GetErrorInfo(ret, "hi_mpi_vpc_convert_color");
        if (DvppPool::GetInstance().IsInited(inputTensor.GetDeviceId(), DvppChnType::VPC)) {
            ret = DvppPool::GetInstance().PutChn(inputTensor.GetDeviceId(), chnId, DvppChnType::VPC);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to put back VpcChannel to pool in VpcConvertColor operation." << GetErrorInfo(ret);
            }
        }
        return APP_ERR_DVPP_VPC_FAIL;
    }
    ret = hi_mpi_vpc_get_process_result(chnId, taskID, -1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get convert color result." << GetErrorInfo(ret, "hi_mpi_vpc_get_process_result");
    }
    if (DvppPool::GetInstance().IsInited(inputTensor.GetDeviceId(), DvppChnType::VPC)) {
        ret = DvppPool::GetInstance().PutChn(inputTensor.GetDeviceId(), chnId, DvppChnType::VPC);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to put back VpcChannel to pool in VpcConvertColor operation." << GetErrorInfo(ret);
        }
    }
    if (ret != APP_ERR_OK) {
        return APP_ERR_COMM_FAILURE;
    }
    return ret;
}

APP_ERROR CvtColorProcess(const Tensor &inputTensor, Tensor &outputTensor, const CvtColorMode mode, bool keepMargin,
                          AscendStream &stream)
{
    std::map<CvtColorMode, InOutFormat> map = CvtColorMap;
    auto iter = map.find(mode);
    InOutFormat inOutFormat = iter->second;
    bool reuseInputTensor = false;
    bool reuseOutputTensor = false;
    hi_vpc_pic_info cvtInputDesc;
    hi_vpc_pic_info cvtOutputDesc;
    if (SetCvtColorInputDesc(cvtInputDesc, inputTensor, outputTensor, inOutFormat, reuseInputTensor) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Set cvt inputDesc " << cvtInputDesc.picture_width << ", " << cvtInputDesc.picture_height;
    if (SetCvtColorOutputDesc(cvtOutputDesc, outputTensor, inOutFormat, reuseOutputTensor) != APP_ERR_OK) {
        if (!reuseInputTensor) {
            DVPPMemoryFreeFunc(cvtInputDesc.picture_address);
        }
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Set cvt outputDesc " << cvtOutputDesc.picture_width << ", " << cvtOutputDesc.picture_height;
    APP_ERROR ret = VpcConvertColor(inputTensor, cvtInputDesc, cvtOutputDesc, stream);
    if (ret != APP_ERR_OK) {
        FreeCvtColorDataIfNotReuse(cvtInputDesc, cvtOutputDesc, reuseInputTensor, reuseOutputTensor);
        return ret;
    }
    uint32_t channel = ONE_CHANNEL;
    if (outputTensor.GetShape().size() == HWC_SHAPE_SIZE) {
        channel = outputTensor.GetShape()[SHAPE_DIMENSION_TWO];
    }
    if (!keepMargin && !reuseOutputTensor) {
        uint32_t height = outputTensor.GetValidRoi().y1;
        if (inOutFormat.outputFormat == ImageFormat::YUV_SP_420 ||
            inOutFormat.outputFormat == ImageFormat::YVU_SP_420) {
            height = height * YUV_HEIGHT_STRIDE / YUV_HEIGHT_DIV;
        }
        for (size_t h = 0; h < height; h++) {
            size_t offsetFrom = h * DVPP_ALIGN_UP(cvtOutputDesc.picture_width, DVPP_ALIGN_WIDTH) * channel;
            size_t offsetTo = h * outputTensor.GetValidRoi().x1 * channel;
            ret = aclrtMemcpy(static_cast<void*>(static_cast<uint8_t*>(outputTensor.GetData()) + offsetTo),
                              outputTensor.GetValidRoi().x1 * channel,
                              static_cast<void*>(static_cast<uint8_t*>(cvtOutputDesc.picture_address) + offsetFrom),
                              outputTensor.GetValidRoi().x1 * channel, ACL_MEMCPY_DEVICE_TO_DEVICE);
            if (ret != APP_ERR_OK) {
                FreeCvtColorDataIfNotReuse(cvtInputDesc, cvtOutputDesc, reuseInputTensor, reuseOutputTensor);
                LogError << "Failed to copy dvpp memory to output tensor." << GetErrorInfo(ret, "aclrtMemcpy");
                return APP_ERR_ACL_BAD_COPY ;
            }
        }
        LogDebug << "Write to device memory finished.";
    }
    FreeCvtColorDataIfNotReuse(cvtInputDesc, cvtOutputDesc, reuseInputTensor, reuseOutputTensor);
    return APP_ERR_OK;
}

void CallBack::CvtColorCallBackFunc(void *arg)
{
    APP_ERROR ret = APP_ERR_OK;
    CvtColorCallbackParam* input = static_cast<CvtColorCallbackParam*>(arg);
    ret = CvtColorProcess(input->src, input->dst, input->mode, input->keepMargin, input->stream);
    delete input;
    if (ret != APP_ERR_OK) {
        LogError << "CvtColor failed." << GetErrorInfo(ret);
    }
}

APP_ERROR CallBack::CvtColorCallback(CvtColorCallbackParam* input, AscendStream &stream)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = aclrtLaunchCallback(CvtColorCallBackFunc, static_cast<void*>(input), ACL_CALLBACK_BLOCK, stream.stream);
    if (ret != APP_ERR_OK) {
        delete input;
        LogError << "Convert color failed." << GetErrorInfo(ret, "aclrtLaunchCallback");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR CheckInputTensorFormat(const Tensor &inputTensor, const CvtColorMode &mode)
{
    if (mode < CvtColorMode::COLOR_YUVSP4202GRAY || mode > CvtColorMode::COLOR_BGR2YVUSP420) {
        LogError << "The value of mode is not in the value range of CvtColorMode class."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::map<CvtColorMode, InOutFormat> map = CvtColorMap;
    auto iter = map.find(mode);
    InOutFormat inOutFormat = iter->second;
    uint32_t channel = 0;
    GetChannelByFormat(inOutFormat.inputFormat, channel);
    if (inputTensor.GetShape().size() == HWC_SHAPE_SIZE) {
        if (channel != inputTensor.GetShape()[SHAPE_DIMENSION_TWO]) {
            LogError << "Channel number of input tensor does not match the input format."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    if ((mode <= CvtColorMode::COLOR_BGR2GRAY || mode == CvtColorMode::COLOR_RGBA2GRAY ||
         (mode >= CvtColorMode::COLOR_BGR2YUVSP420 && mode <= CvtColorMode::COLOR_BGR2YVUSP420)) &&
        inputTensor.GetShape()[SHAPE_DIMENSION_ONE] < MIN_GRAY_TENSOR_WIDTH) {
        LogError << "Get invalid tensor width(" << inputTensor.GetShape()[SHAPE_DIMENSION_ONE] << "), "
                 << "which should be in the range of ["<< MIN_GRAY_TENSOR_WIDTH << ", " << MAX_TENSOR_WIDTH
                 << "] when input or output mode is one of YUV400, YUVSP420 and YVUSP420."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (mode <= CvtColorMode::COLOR_YVUSP4202BGR) {
        if (inputTensor.GetShape()[1] % DVPP_ALIGN_WIDTH != 0 || inputTensor.GetShape()[0] % YUV_HEIGHT_STRIDE != 0) {
            LogError << "The width of the input tensor should be mutiples of " << DVPP_ALIGN_WIDTH << " and the height"
                     << " should be mutiples of " << YUV_HEIGHT_STRIDE
                     << " if the input format is YUV_SP_420(nv12) or YVU_SP_420(nv21)."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (inputTensor.GetValidRoi().y1 % YUV_HEIGHT_STRIDE != 0) {
            LogError << "The height of validRoi in input tensor should be mutiples of " << YUV_HEIGHT_STRIDE
                     << " if the input format is YUV_SP_420(nv12) or YVU_SP_420(nv21)."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR CvtColorGray2RGB(const Tensor &src, Tensor &dst, AscendStream &stream)
{
    if (src.GetShape().size() == NHWC_SHAPE_SIZE) {
        LogError << "CvtColor inputFormat YUV_400 does not support NHWC." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // 1. expand dimension of src
    Tensor src1 = src;
    bool isHwShape = false;
    std::vector<uint32_t> srcShape = src.GetShape();
    if (src.GetShape().size() == HW_SHAPE_SIZE) {
        srcShape.push_back(1);
        src1.SetShape(srcShape);
        isHwShape = true;
    }

    // 2. use Merge op to expand the channel dimension
    std::vector<Tensor> srcVec = {src1, src1, src1};
    auto ret = Merge(srcVec, dst, stream);
    if (ret != APP_ERR_OK) {
        LogError << "CvtColorGray2RGB Merge op run failed." << GetErrorInfo(ret);
    }

    // 3. recover the Shape for src
    if (isHwShape) {
        srcShape.pop_back();
        src1.SetShape(srcShape);
    }

    return ret;
}

APP_ERROR CheckInputGrayParams(const Tensor &src, Tensor &dst)
{
    if (src.IsEmpty()) {
        LogError << "Input tensor should not be empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetDataType() != TensorDType::UINT8 || (!dst.IsEmpty() && dst.GetDataType() != TensorDType::UINT8)) {
        LogError << "Unsupported src or dst Datatype, Only support uint8 TensorDType."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (CheckCvtColorTensorHW(src) != APP_ERR_OK) {
        LogError << "Check input tensor height and width failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetMemoryType() != MemoryData::MemoryType::MEMORY_DVPP &&
        src.GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE) {
        LogError << "Input tensor cannot be on the host, only support dvpp or device memory, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!dst.IsEmpty() && dst.GetMemoryType() != MemoryData::MemoryType::MEMORY_DVPP &&
        dst.GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE) {
        LogError << "Output tensor cannot be on the host if it is not empty, only support dvpp or device memory."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetShape().size() == HWC_SHAPE_SIZE && src.GetShape()[SHAPE_DIMENSION_TWO] != 1) {
        LogError << "Channel number of input hwc gray tensor must be 1."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR CheckCvtRGBAParams(const Tensor &src, const Tensor &,  AscendStream &)
{
    std::vector<Tensor> srcVec = {src};
    OpSupportDtype opSupportDtype;
    opSupportDtype.isOnlyUint = true;

    APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype, true, true);
    if (ret != APP_ERR_OK) {
        LogError << "CheckCvtRGBAParams: failed to check general op parameters." << GetErrorInfo(ret);
        return ret;
    }
    if (src.GetShape().size() != HWC_SHAPE_SIZE) {
        LogError << "CheckCvtRGBAParams: Tensor is not HWC shape, please check." << GetErrorInfo(ret);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (src.GetShape()[SHAPE_DIMENSION_TWO] != RGBA_CHANNEL_NUM) {
        LogError << "CheckCvtRGBAParams: Tensor channel number is not 4, please check." << GetErrorInfo(ret);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR Mrgba(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream)
{
    std::string opType = "MRGBA";
    LogDebug << "Start Running MRGBA op...";

    // 1. Check parameters.
    std::vector<Tensor> srcVec = {src1, src2};

    // 2. OperatorImplicitMalloc
    ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
    APP_ERROR ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
    if (ret != APP_ERR_OK) {
        LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
        return ret;
    }
    std::vector<Tensor> dstVec = {dst};

    // 2. Execute RunOp
    RunOpParam mrgbaParam{opType, srcVec, dstVec};
    mrgbaParam.dslParam.dslCompileOptValue = "enable";
    ret = RunOp(mrgbaParam, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Run mRGBA op failed." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "Run MRGBA op success.";

    return ret;
}

APP_ERROR CvtRgba(const Tensor &src, Tensor &dst, AscendStream &stream)
{
    LogDebug << "Run CvtColor cvtRGBA...";
    APP_ERROR ret = CheckCvtRGBAParams(src, dst, stream);
    if (ret != APP_ERR_OK) {
        LogError << "CvtRgba: Fail to check parameters." << GetErrorInfo(ret);
        return ret;
    }

    // 1. Split src into RGB(3 channels) and A(1 channel)
    std::vector<Tensor> srcTensors;
    ret = Split(src, srcTensors, stream);
    if (ret != APP_ERR_OK) {
        LogError << "CvtRgba: Split failed." << GetErrorInfo(ret);
        return ret;
    }
    MxBase::Tensor srcRGB;
    std::vector<Tensor> srcRGBTensor = {srcTensors[RGBA_CHANNEL_R], srcTensors[RGBA_CHANNEL_G],
                                        srcTensors[RGBA_CHANNEL_B]};
    ret = Merge(srcRGBTensor, srcRGB, stream);
    if (ret != APP_ERR_OK) {
        LogError << "CvtRgba: Merge failed." << GetErrorInfo(ret);
        return ret;
    }

    Tensor dstRGB;
    ret = Mrgba(srcRGB, srcTensors[RGBA_CHANNEL_A], dstRGB, stream);
    if (ret != APP_ERR_OK) {
        LogError << "CvtRgba: cvtRGBA op MRGBA failed." << GetErrorInfo(ret);
        return ret;
    }

    // 2. Merge dstRGB and srcAlpha
    std::vector<Tensor> dstRGBA = {dstRGB,  srcTensors[RGBA_CHANNEL_A]};
    ret = Merge(dstRGBA, dst, stream);
    if (ret != APP_ERR_OK) {
        LogError << "CvtRgba: cvtColor op Merge failed." << GetErrorInfo(ret);
        return ret;
    }
    if (src.GetValidRoi().x1 > 0 && src.GetValidRoi().y1 > 0) {
        dst.SetValidRoi(src.GetValidRoi());
    }
    return APP_ERR_OK;
}

APP_ERROR DoCvtColorGray2RGB(const Tensor &inputTensor, Tensor &outputTensor,
                             const CvtColorMode &mode, AscendStream &stream)
{
    APP_ERROR ret = CheckInputGrayParams(inputTensor, outputTensor);
    if (ret != APP_ERR_OK) {
        LogError << "Check convert color gray to rgb input params failed. " << GetErrorInfo(ret);
        return ret;
    }
    ret = CvtColorGray2RGB(inputTensor, outputTensor, stream);
    if (ret != APP_ERR_OK) {
        LogError << "CvtColor failed, mode is " << static_cast<int>(mode) << ". " << GetErrorInfo(ret);
        return ret;
    }
    return ret;
}

APP_ERROR CvtColor(const Tensor &inputTensor, Tensor &outputTensor, const CvtColorMode &mode, bool keepMargin,
                   AscendStream &stream)
{
    if (mode == CvtColorMode::COLOR_GRAY2RGB) {
        if (DeviceManager::IsAtlas800IA2()) {
            LogError << "Mode COLOR_GRAY2RGB now is not support on Atlas800IA2."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return DoCvtColorGray2RGB(inputTensor, outputTensor, mode, stream);
    }
    APP_ERROR ret = CheckCvtColorInputParams(inputTensor, outputTensor, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Check convert color input params failed. " << GetErrorInfo(ret);
        return ret;
    }
    if (mode == CvtColorMode::COLOR_RGBA2mRGBA) {
        if (DeviceManager::IsAtlas800IA2()) {
            LogError << "Mode COLOR_RGBA2mRGBA now is not support on Atlas800IA2."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        ret = CvtRgba(inputTensor, outputTensor, stream);
        if (ret != APP_ERR_OK) {
            LogError << "CvtColor failed, mode is "  << static_cast<int>(mode) << ". " << GetErrorInfo(ret);
        }
        return ret;
    }
    ret = CheckInputTensorFormat(inputTensor, mode);
    if (ret != APP_ERR_OK) {
        LogError << "Check convert color input tensor channel failed. " << GetErrorInfo(ret);
        return ret;
    }
    ret = SetCvtColorOutputTensor(inputTensor, outputTensor, mode, keepMargin);
    if (ret != APP_ERR_OK) {
        LogError << "Pre-malloc convert color output tensor failed. " << GetErrorInfo(ret);
        return ret;
    }
    if (stream.isDefault_) {
        LogDebug << "AscendStream is default, ready to execute convert color.";
        ret = CvtColorProcess(inputTensor, outputTensor, mode, keepMargin, stream);
    } else {
        CallBack callback;
        CvtColorCallbackParam* cvtColorCallbackParam =
            new CvtColorCallbackParam{inputTensor, outputTensor, mode, keepMargin, stream};
        ret = callback.CvtColorCallback(cvtColorCallbackParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << "CvtColor failed. " << GetErrorInfo(ret);
        }
    }
    return ret;
}

}
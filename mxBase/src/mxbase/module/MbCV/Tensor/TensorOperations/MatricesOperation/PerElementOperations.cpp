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
* Description: Manage Tensor Per Element Operations.
* Author: MindX SDK
* Create: 2023
* History: NA
*/

#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/PerElementOperations.h"
#include <iostream>
#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/CoreOperationsOnTensors.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/DataTypeUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/CommonUtils.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/OpCall.h"

namespace {
    constexpr uint8_t UINT8_MAX_VALUE = 255;
    constexpr uint8_t MAX_SORT_DIM = 2;
    constexpr int REAL_SORT_DIM = 1;
    constexpr int THRESHOLD_BINARY = 0;
    constexpr int THRESHOLD_BINARY_INV = 1;
    constexpr size_t HW_SHAPE_SIZE = 2;
    constexpr size_t HWC_SHAPE_SIZE = 3;
    constexpr size_t NHWC_SHAPE_SIZE = 4;
    constexpr size_t SHAPE_DIMENSION_TWO = 2;
    constexpr size_t SHAPE_DIMENSION_HREE = 3;
    constexpr size_t BATCH_ONE = 1;
    const float MULDIV_SCALE = 1.f;
    const std::map<MxBase::CmpOp, std::string> toCmpOpMap = {
        {MxBase::CmpOp::CMP_EQ, "eq"},
        {MxBase::CmpOp::CMP_NE, "ne"},
        {MxBase::CmpOp::CMP_LT, "lt"},
        {MxBase::CmpOp::CMP_GT, "gt"},
        {MxBase::CmpOp::CMP_LE, "le"},
        {MxBase::CmpOp::CMP_GE, "ge"}
    };
}

namespace MxBase {
    APP_ERROR Add(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream)
    {
        LogDebug << "Start to execute Add op.";
        std::string opType = "Add";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};
        ret = CheckSrcsDstsRoiShape(srcVec, dstVec);
        if (ret != APP_ERR_OK) {
            LogError << "Operator " << opType << " fail to check roi shape." << GetErrorInfo(ret);
            return ret;
        }
        // 3. Execute RunOp
        int alpha = 1;
        CommonAclnnPara ascendcParam{std::vector<int>{alpha}, std::vector<float>{}};
        RunOpParam addParam{opType, srcVec, dstVec};
        addParam.ascendcParam = ascendcParam;
        ret = RunOp(addParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }

        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR Subtract(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream)
    {
        LogDebug << "Start Subtract...";
        std::string opType = "Sub";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        int alpha = 1;
        CommonAclnnPara ascendcParam{std::vector<int>{alpha}, std::vector<float>{}};
        RunOpParam subParam{opType, srcVec, dstVec};
        subParam.ascendcParam = ascendcParam;
        ret = RunOp(subParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Subtract: Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << "Subtract: Run op success.";
        }

        LogDebug << "Subtract: Run op finished.";
        return ret;
    }

    APP_ERROR Multiply(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream)
    {
        LogDebug << "Start to execute Multiply op.";
        std::string opType = "Multiply";
        // 0. Initial the lock
        static std::mutex g_mtx;

        // 1. Check parameters
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }

        // 3. Check parameters.
        std::vector<Tensor> dstVec = {dst};

        // 4. Set op attr
        float scale = MULDIV_SCALE;
        OpAttrDesc opAttrDesc(OpAttrType::FLOAT, "scale", static_cast<void *>(&scale));
        std::vector<OpAttrDesc> opAttrDescVec{opAttrDesc};

        // 5. Execute RunOp
        CommonAclnnPara ascendcParam{std::vector<int>{}, std::vector<float>{scale}};
        RunOpParam multiplyParam{opType, srcVec, dstVec};
        multiplyParam.ascendcParam = ascendcParam;
        multiplyParam.dslParam.opAttrDesc = opAttrDescVec;
        ret = RunOp(multiplyParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }

        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    void GetOutputTensorDType(const Tensor &src1, const Tensor &src2, MxBase::TensorDType& outputDType)
    {
        auto src1DType = src1.GetDataType();
        auto src2DType = src2.GetDataType();
        std::map<MxBase::TensorDType, size_t> dType = {
                {MxBase::TensorDType::UINT8,   0},
                {MxBase::TensorDType::FLOAT16, 1},
                {MxBase::TensorDType::FLOAT32, 2}
        };
        if (dType[src1DType] > dType[src2DType]) {
            outputDType = src1DType;
        } else {
            outputDType = src2DType;
        }
    }

    APP_ERROR Divide(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream)
    {
        LogDebug << "Start to execute Divide op.";
        std::string opType = "Divide";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype, false, true);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        MxBase::TensorDType expectDstDType;
        GetOutputTensorDType(src1, src2, expectDstDType);
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), expectDstDType, src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        CommonAclnnPara ascendcParam{std::vector<int>{}, std::vector<float>{MULDIV_SCALE}};
        RunOpParam divideParam{opType, srcVec, dstVec};
        divideParam.ascendcParam = ascendcParam;
        ret = RunOp(divideParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }

        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR Divide(const Tensor &src1, const Tensor &src2, Tensor &dst, float scale, AscendStream& stream)
    {
        LogDebug << "Start to execute Divide op.";
        std::string opType = "Divide";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype, false, true);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        MxBase::TensorDType expectDstDType;
        GetOutputTensorDType(src1, src2, expectDstDType);
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), expectDstDType, src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        CommonAclnnPara ascendcParam{std::vector<int>{}, std::vector<float>{scale}};
        RunOpParam divideParam{opType, srcVec, dstVec};
        divideParam.ascendcParam = ascendcParam;
        ret = RunOp(divideParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }

        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR BitwiseAnd(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream)
    {
        LogDebug << "Start to execute BitwiseAnd op.";
        std::string opType = "BitwiseAnd";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        opSupportDtype.isOnlyUint = true;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        RunOpParam bitwiseAndParam{opType, srcVec, dstVec};
        ret = RunOp(bitwiseAndParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }

        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR BitwiseOr(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream)
    {
        LogDebug << "Start to execute BitwiseOr op.";
        std::string opType = "BitwiseOr";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        opSupportDtype.isOnlyUint = true;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        RunOpParam bitwiseOrParam{opType, srcVec, dstVec};
        ret = RunOp(bitwiseOrParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }

        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR BitwiseXor(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream)
    {
        LogDebug << "Start to execute BitwiseXor op.";
        std::string opType = "BitwiseXor";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        opSupportDtype.isOnlyUint = true;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        RunOpParam bitwiseXorParam{opType, srcVec, dstVec};
        ret = RunOp(bitwiseXorParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }

        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR BitwiseNot(const Tensor &src, Tensor &dst, AscendStream& stream)
    {
        LogDebug << "Start to execute BitwiseNot op.";
        std::string opType = "BitwiseXor";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src};
        OpSupportDtype opSupportDtype;
        opSupportDtype.isOnlyUint = true;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << "BitwiseNot: Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src.GetShape(), src.GetDataType(), src.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << "BitwiseNot: Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};
        // 3. Execute RunOp
        RunOpParam bitwiseNotParam{opType, srcVec, dstVec};
        if (DeviceManager::IsAscend310P() && !stream.isDefault_) {
            bitwiseNotParam.opType = "BitwiseNot";
        } else {
            MxBase::Tensor inputTensor(src.GetShape(), src.GetDataType(), src.GetDeviceId());
            ret = inputTensor.Malloc();
            if (ret != APP_ERR_OK) {
                LogError << "BitwiseNot: Max value tensor malloc failed." << GetErrorInfo(ret);
                return ret;
            }
            ret = CheckStreamAndInplace("BitwiseNot", bitwiseNotParam.srcVec, dstVec, stream);
            if (ret != APP_ERR_OK) {
                LogError << "BitwiseNot: Fail to check parameters." << GetErrorInfo(ret);
                return ret;
            }
            ret = inputTensor.SetTensorValue(UINT8_MAX_VALUE, stream);
            if (ret != APP_ERR_OK) {
                LogError << "BitwiseNot: Fail to set max value." << GetErrorInfo(ret);
                return ret;
            }
            bitwiseNotParam.srcVec.push_back(inputTensor);
        }
        ret = RunOp(bitwiseNotParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << "BitwiseNot: Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << "BitwiseNot: Run op success.";
        }

        LogDebug << "BitwiseNot: Run op finished.";
        return ret;
    }

    APP_ERROR Pow(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream &stream)
    {
        LogDebug << "Start to execute Pow op.";
        std::string opType = "Pow";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        RunOpParam powRunOpParam{opType, srcVec, dstVec};
        ret = RunOp(powRunOpParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }
        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR Sqr(const Tensor &src, Tensor &dst, AscendStream &stream)
    {
        LogDebug << "Start to execute Sqr op.";
        std::string opType = "Sqr";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
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
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        RunOpParam sqrParam{opType, srcVec, dstVec};
        ret = RunOp(sqrParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }
        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR Sqrt(const Tensor &src, Tensor &dst, AscendStream &stream)
    {
        LogDebug << "Start to execute Sqrt op.";
        std::string opType = "Sqrt";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src};
        OpSupportDtype opSupportDtype;
        opSupportDtype.isNotUint = true;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
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
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        RunOpParam sqrtRunOpParam{opType, srcVec, dstVec};
        ret = RunOp(sqrtRunOpParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }
        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR Exp(const Tensor &src, Tensor &dst, AscendStream &stream)
    {
        LogDebug << "Start to execute Exp op.";
        std::string opType = "Exp";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src};
        OpSupportDtype opSupportDtype;
        opSupportDtype.isNotUint = true;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
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
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        RunOpParam expRunOpParam{opType, srcVec, dstVec};
        expRunOpParam.fixedType = "dsl";
        ret = RunOp(expRunOpParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }

        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR Log(const Tensor &src, Tensor &dst, AscendStream& stream)
    {
        LogDebug << "Start to execute Log op.";
        std::string opType = "Log";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src};
        OpSupportDtype opSupportDtype;
        opSupportDtype.isNotUint = true;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
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
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        RunOpParam logRunOpParam{opType, srcVec, dstVec};
        logRunOpParam.fixedType = "dsl";
        ret = RunOp(logRunOpParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }

        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    static APP_ERROR GetThreshHoldOperation(int &operation, const ThresholdType &thresholdType)
    {
        switch (thresholdType) {
            case ThresholdType::THRESHOLD_BINARY:
                operation = THRESHOLD_BINARY;
                break;
            case ThresholdType::THRESHOLD_BINARY_INV:
                if (!DeviceManager::IsAscend310P()) {
                    LogError << "Threshold binary inv now only support on 310P."
                             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                operation = THRESHOLD_BINARY_INV;
                break;
            default:
                LogError << "Invalid input of threshold type." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }

    static APP_ERROR ThresholdProcess(const Tensor &src, Tensor &dst, float thresh, float maxVal,
                                      const int &operation, AscendStream& stream)
    {
        std::string opType = "ThresholdBinary";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src.GetShape(), src.GetDataType(), src.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << "Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Set op attr
        OpAttrDesc opAttrDesc1(OpAttrType::FLOAT, "thresh", static_cast<void *>(&thresh));
        OpAttrDesc opAttrDesc2(OpAttrType::FLOAT, "maxVal", static_cast<void *>(&maxVal));
        std::vector<OpAttrDesc> opAttrDescVec{opAttrDesc1, opAttrDesc2};

        // 4. Execute RunOp
        CommonAclnnPara ascendcParam{std::vector<int>{operation}, std::vector<float>{thresh, maxVal}};
        RunOpParam thresholdBinaryParam{opType, srcVec, dstVec};
        thresholdBinaryParam.ascendcParam = ascendcParam;
        thresholdBinaryParam.dslParam.opAttrDesc = opAttrDescVec;
        thresholdBinaryParam.dslParam.dslCompileOptValue = "enable";
        return RunOp(thresholdBinaryParam, stream);
    }

    APP_ERROR ThresholdBinary(const Tensor &src, Tensor &dst, float thresh, float maxVal,
                              AscendStream& stream)
    {
        std::string opType = "ThresholdBinary";
        LogDebug << "Start to execute " << opType << " op. New interface Threshold is "
                                                    "recommended to replace the original ThresholdBinary interface.";
        APP_ERROR ret = ThresholdProcess(src, dst, thresh, maxVal, THRESHOLD_BINARY, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }
        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR Threshold(const Tensor &src, Tensor &dst, float thresh, float maxVal,
                        const ThresholdType &thresholdType, AscendStream& stream)
    {
        std::string opType = "Threshold";
        LogDebug << "Start to execute " << opType << " op.";
        int operation = 0;
        APP_ERROR ret = GetThreshHoldOperation(operation, thresholdType);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Get threshold operation failed." << GetErrorInfo(ret);
            return ret;
        }
        ret = ThresholdProcess(src, dst, thresh, maxVal, operation, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }
        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR AddWeighted(const Tensor &src1, float alpha, const Tensor &src2, float beta, float gamma, Tensor &dst,
                          AscendStream& stream)
    {
        LogDebug << "Start to execute AddWeighted op.";
        std::string opType = "AddWeighted";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Set op attr
        OpAttrDesc opAttrDesc1(OpAttrType::FLOAT, "alpha", static_cast<void *>(&alpha));
        OpAttrDesc opAttrDesc2(OpAttrType::FLOAT, "beta", static_cast<void *>(&beta));
        OpAttrDesc opAttrDesc3(OpAttrType::FLOAT, "gamma", static_cast<void *>(&gamma));
        std::vector<OpAttrDesc> opAttrDescVec{opAttrDesc1, opAttrDesc2, opAttrDesc3};

        // 4. Execute RunOp
        CommonAclnnPara ascendcParam{std::vector<int>{}, std::vector<float>{alpha, beta, gamma}};
        RunOpParam addweightedParam{opType, srcVec, dstVec};
        addweightedParam.ascendcParam = ascendcParam;
        addweightedParam.dslParam.opAttrDesc = opAttrDescVec;
        addweightedParam.dslParam.dslCompileOptValue = "enable";
        ret = RunOp(addweightedParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }
        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR AbsDiff(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream)
    {
        LogDebug << "Start to execute AbsDiff op.";
        std::string opType = "AbsDiff";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        RunOpParam minParam{opType, srcVec, dstVec};
        ret = RunOp(minParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }

        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR Abs(const Tensor &src, Tensor &dst, AscendStream &stream)
    {
        LogDebug << "Start to execute Abs op.";
        std::string opName = "Abs";
        std::string opType = "Absolute";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opName << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src.GetShape(), src.GetDataType(), src.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opName << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        RunOpParam minParam{opType, srcVec, dstVec};
        ret = RunOp(minParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opName << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opName << ": Run op success.";
        }
        LogDebug << opName << ": Run op finished.";
        return ret;
    }

    APP_ERROR ScaleAdd(const Tensor &src1, float scale, const Tensor &src2, Tensor &dst, AscendStream& stream)
    {
        LogDebug << "Start to execute ScaleAdd op.";
        std::string opType = "ScaleAdd";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Construct the op compile description
        OpAttrDesc opAttrDesc(OpAttrType::FLOAT, "scale", static_cast<void*>(&scale));
        std::vector<OpAttrDesc> opAttrDescVec{opAttrDesc};

        // 4. Execute RunOp
        CommonAclnnPara ascendcParam{std::vector<int>{}, std::vector<float>{scale}};
        RunOpParam scaleAddParam{opType, srcVec, dstVec};
        scaleAddParam.ascendcParam = ascendcParam;
        scaleAddParam.dslParam.opAttrDesc = opAttrDescVec;
        ret = RunOp(scaleAddParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }
        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    static APP_ERROR CheckConvertToParams(const Tensor &src, const MxBase::TensorDType &dataType)
    {
        if (!(DeviceManager::IsAscend310P() || DeviceManager::IsAscend310B())) {
            LogError << "CheckConvertToParams: current op only supported on device 310P now, current device is "
                     << DeviceManager::GetSocName() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if ((src.GetDataType() == TensorDType::UNDEFINED) || (dataType == TensorDType::UNDEFINED)) {
            LogError << "CheckConvertToParams: The tensor data type of src Tensor"
                     << " and target type should not be UNDEFINED, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (src.IsEmpty()) {
            LogError << "CheckConvertToParams: The src Tensor should not be empty, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (src.GetShape().empty()) {
            LogError << "CheckConvertToParams: Input tensor shape is empty, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (src.GetMemoryType() != MemoryData::MEMORY_DEVICE && src.GetMemoryType() != MemoryData::MEMORY_DVPP) {
            LogError << "CheckConvertToParams: The src memory type cannot be host, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (src.GetDataType() == dataType) {
            LogError << "CheckConvertToParams: The src and dst data types are the same, no need to ConvertTo."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }

    APP_ERROR IsConvertToWithRoiDataTypeValid(const Tensor &src, const MxBase::TensorDType &dataType)
    {
        auto srcDataType = src.GetDataType();
        if (srcDataType == TensorDType::UINT8 &&
            (dataType == TensorDType::FLOAT16 || dataType == TensorDType::FLOAT32)) {
            return APP_ERR_OK;
        } else if ((srcDataType == TensorDType::FLOAT16 || srcDataType == TensorDType::FLOAT32) &&
                   dataType == TensorDType::UINT8) {
            return APP_ERR_OK;
        } else {
            LogError << "ConvertTo: roi operation only support u8->fp16/fp32 and fp16/fp32->u8, please check.";
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }

    APP_ERROR ConvertToDstMalloc(const Tensor &src, Tensor &dst, const MxBase::TensorDType &dataType)
    {
        if (IsSetReferRect(dst)) {
            LogError << "ConvertTo: dst can not set refer rect, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        APP_ERROR ret = APP_ERR_OK;
        if (!IsSetReferRect(src)) {
            ExpectedTensorInfo expectedTensorInfo = {src.GetShape(), dataType, src.GetDeviceId()};
            ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
            if (ret != APP_ERR_OK) {
                LogError << "ConvertTo: Implicit malloc failed or dst tensor check failed." << GetErrorInfo(ret);
            }
            return ret;
        }
        // ConvertTo when src has roi
        ret = IsConvertToWithRoiDataTypeValid(src, dataType);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        auto rect = src.GetReferRect();
        uint32_t width = rect.x1 - rect.x0;
        uint32_t height = rect.y1 - rect.y0;
        std::vector<uint32_t> expectedDstShape;
        auto srcShape = src.GetShape();
        auto srcShapeSize = src.GetShape().size();
        if (srcShapeSize == HW_SHAPE_SIZE) {
            expectedDstShape = {height, width};
        } else if (srcShapeSize == HWC_SHAPE_SIZE) {
            expectedDstShape = {height, width, srcShape[SHAPE_DIMENSION_TWO]};
        } else if (srcShapeSize == NHWC_SHAPE_SIZE) {
            expectedDstShape = {BATCH_ONE, height, width, srcShape[SHAPE_DIMENSION_HREE]};
        } else {
            LogError << "ConvertTo: Unsupported tensor shape." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        ExpectedTensorInfo expectedTensorInfo = {expectedDstShape, dataType, src.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << "ConvertTo: Implicit malloc failed or dst tensor check failed." << GetErrorInfo(ret);
        }
        return ret;
    }

    APP_ERROR ConvertTo(const Tensor &src, Tensor &dst, const MxBase::TensorDType &dataType, AscendStream &stream)
    {
        LogDebug << "Start to execute ConvertTo op.";
        std::string opType = "Cast";
        std::vector<Tensor> srcVec = {src};

        // 1. Check parameters.
        APP_ERROR ret = CheckConvertToParams(src, dataType);
        if (ret != APP_ERR_OK) {
            LogError << "ConvertTo: Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ret = ConvertToDstMalloc(src, dst, dataType);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Set attr
        OpDataType dstDataType = static_cast<OpDataType>(dataType);
        OpAttrDesc opAttrDesc(OpAttrType::INT, "dst_type", static_cast<void*>(&dstDataType));
        std::vector<OpAttrDesc> opAttrDescVec{opAttrDesc};

        // 4. Execute RunOp
        CommonAclnnPara ascendcParam{std::vector<int>{}, std::vector<float>{}, dstDataType};
        RunOpParam castParam{opType, srcVec, dstVec};
        castParam.ascendcParam = ascendcParam;
        castParam.dslParam.opAttrDesc = opAttrDescVec;
        ret = RunOp(castParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << "ConvertTo" << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << "ConvertTo" << ": Run op success.";
        }

        LogDebug << "ConvertTo" << ": Run op finished.";
        return ret;
    }

    static APP_ERROR ClipValueAssign(Tensor &minValTensor, Tensor &maxValTensor, const Tensor &src,
                                     float minVal, float maxVal)
    {
        APP_ERROR ret = APP_ERR_OK;
        std::vector<uint32_t> valueShape = {1};
        OpDataType dataType = static_cast<OpDataType>(src.GetDataType());
        if (dataType == OP_UINT8) {
            std::vector<uint32_t> inputShape = src.GetShape();
            Tensor minValTensor2(inputShape, MxBase::TensorDType::UINT8);
            ret = minValTensor2.Malloc();
            if (ret != APP_ERR_OK) {
                LogError << "ClipValueAssign: Minimal Value Tensor malloc failed, infer interrupted."
                         << GetErrorInfo(ret);
                return ret;
            }
            minValTensor = minValTensor2;
            minValTensor.ToDevice(src.GetDeviceId());
            minValTensor.SetTensorValue(static_cast<uint8_t>(minVal));
 
            Tensor maxValTensor2(inputShape, MxBase::TensorDType::UINT8);
            ret = maxValTensor2.Malloc();
            if (ret != APP_ERR_OK) {
                LogError << "ClipValueAssign: Max Value Tensor malloc failed, infer interrupted." << GetErrorInfo(ret);
                return ret;
            }
            maxValTensor = maxValTensor2;
            maxValTensor.ToDevice(src.GetDeviceId());
            maxValTensor.SetTensorValue(static_cast<uint8_t>(maxVal));
        } else if (dataType == OP_FLOAT) {
            float value[1] = {minVal};
            Tensor minvalTensor2(&value[0], valueShape, MxBase::TensorDType::FLOAT32);
            minValTensor = minvalTensor2;
            minValTensor.ToDevice(src.GetDeviceId());
            value[0] = static_cast<float>(maxVal);
            Tensor maxValTensor2(&value[0], valueShape, MxBase::TensorDType::FLOAT32);
            maxValTensor = maxValTensor2;
            maxValTensor.ToDevice(src.GetDeviceId());
        } else {
            uint16_t uint16MinVal;
            DataTypeUtils::Float32ToFloat16(&uint16MinVal, minVal);
            uint16_t value[1] = { uint16MinVal };
            Tensor minValTensor2(&value[0], valueShape, MxBase::TensorDType::FLOAT16);
            minValTensor = minValTensor2;
            minValTensor.ToDevice(src.GetDeviceId());
 
            uint16_t uint16MaxVal;
            DataTypeUtils::Float32ToFloat16(&uint16MaxVal, maxVal);
            value[0] = uint16MaxVal;
            Tensor maxValTensor2(&value[0], valueShape, MxBase::TensorDType::FLOAT16);
            maxValTensor = maxValTensor2;
            maxValTensor.ToDevice(src.GetDeviceId());
        }
        return ret;
    }

    APP_ERROR Clip(const Tensor &src, Tensor &dst, float minVal, float maxVal, AscendStream &stream)
    {
        LogDebug << "Start to execute Clip op.";
        std::string opType = "ClipByValue";
        std::vector<Tensor> srcVec = {src};

        // 1. Check params
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << "Clip: Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }
        if (minVal > maxVal) {
            LogError << "Clip: The minVal must be less than or equal to the maxVal, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src.GetShape(), src.GetDataType(), src.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << "Clip: Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }

        // 3. Get min&max Tensor
        if (DeviceManager::IsAscend310B()) {
            Tensor minValTensor;
            Tensor maxValTensor;
            ret = ClipValueAssign(minValTensor, maxValTensor, src, minVal, maxVal);
            if (ret != APP_ERR_OK) {
                LogError << "Clip: Assign value failed." << GetErrorInfo(ret);
                return ret;
            }
            srcVec.push_back(minValTensor);
            srcVec.push_back(maxValTensor);
        }

        // 4. Execute RunOp
        std::vector<Tensor> dstVec = {dst};
        CommonAclnnPara ascendcParam{std::vector<int>{}, std::vector<float>{minVal, maxVal},
                                     static_cast<OpDataType>(src.GetDataType())};
        RunOpParam clipParam{opType, srcVec, dstVec};
        clipParam.ascendcParam = ascendcParam;
        ret = RunOp(clipParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Clip: Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << "Clip: Run op success.";
        }
        LogDebug << "Clip: Run op finished.";
        return ret;
    }

    APP_ERROR Min(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream)
    {
        LogDebug << "Start to execute Min op.";
        std::string opType = "Min";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        RunOpParam minParam{opType, srcVec, dstVec};
        ret = RunOp(minParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }

        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR Max(const Tensor &src1, const Tensor &src2, Tensor &dst, AscendStream& stream)
    {
        LogDebug << "Start to execute Max op.";
        std::string opType = "Max";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Execute RunOp
        RunOpParam maxParam{opType, srcVec, dstVec};
        ret = RunOp(maxParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }

        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    static APP_ERROR CheckSortOpParams(const Tensor &src, const int &axis, const OpSupportDtype &)
    {
        if (!(DeviceManager::IsAscend310P()|| DeviceManager::IsAscend310B())) {
            LogError << "CheckSortOpParams: current op only supported on device 310P now, current device is "
                     << DeviceManager::GetSocName() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (axis != 0 && axis != 1) {
            LogError << "CheckSortOpParams: The axis value is invalid, it must be [0/1], please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (src.IsEmpty()) {
            LogError << "CheckSortOpParams: The tensor is empty, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (src.GetMemoryType() != MemoryData::MEMORY_DEVICE && src.GetMemoryType() != MemoryData::MEMORY_DVPP) {
            LogError << "CheckSortOpParams: The tensor memory type cannot be host, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (src.GetShape().empty() || src.GetShape().size() != MAX_SORT_DIM) {
            LogError << "CheckSortOpParams: Tensor dimension(" << src.GetShape().size()
                     << ") invalid, not the operator’s valid dimension(2), please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (src.GetDataType() != TensorDType::UINT8 && src.GetDataType() != TensorDType::FLOAT16 &&
            src.GetDataType() != TensorDType::FLOAT32) {
            LogError << "CheckSortOpParams: Input Tensor type is not supported, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }

    static APP_ERROR SortCore(const Tensor &src, Tensor &sortDst, Tensor &sortIdxDst,
                              int axis, bool descending, AscendStream& stream)
    {
        LogDebug << "Start to execute Sort op.";
        std::string opType = "Sort";

        // 1. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src.GetShape(), src.GetDataType(), src.GetDeviceId()};
        APP_ERROR ret = OperatorImplicitMallocTensor(sortDst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }

        ExpectedTensorInfo expectedIdxTensorInfo = {src.GetShape(), MxBase::TensorDType::INT32,
                                                    src.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(sortIdxDst, expectedIdxTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dstIdx tensor check failed." << GetErrorInfo(ret);
            return ret;
        }

        std::vector<Tensor> dstVec = {sortDst, sortIdxDst};

        // 3. Set attr
        std::vector<OpAttrDesc> opAttrDescs;
        opAttrDescs.push_back(OpAttrDesc(OpAttrType::INT, "axis", static_cast<void *>(&axis)));
        opAttrDescs.push_back(OpAttrDesc(OpAttrType::BOOL, "descending", static_cast<void *>(&descending)));

        // 4. Execute RunOp
        std::vector<Tensor> srcVec = {src};
        RunOpParam sortRunOpParam{opType, srcVec, dstVec};
        sortRunOpParam.dslParam.opAttrDesc = opAttrDescs;
        sortRunOpParam.fixedType = "dsl";
        ret = RunOp(sortRunOpParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }

        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    static APP_ERROR SortMain(const Tensor &src, Tensor &sortDst, Tensor &sortIdxDst,
                              int axis, bool descending, AscendStream& stream)
    {
        APP_ERROR ret = APP_ERR_OK;
        if (axis == 0) {
            Tensor sortDstT;
            Tensor sortIdxDstT;
            Tensor srcT;
            ret = Transpose(src, srcT, std::vector<int>{1, 0}, stream);
            if (ret != APP_ERR_OK) {
                LogError << "Fail to transpose src tensor for SortCore function." << GetErrorInfo(ret);
                return ret;
            }

            ret = SortCore(srcT, sortDstT, sortIdxDstT, REAL_SORT_DIM, descending, stream);
            if (ret != APP_ERR_OK) {
                LogError << "Fail to execute Sort op for SortCore function." << GetErrorInfo(ret);
                return ret;
            }

            Tensor sortIdxDstTFP32;
            ret = ConvertTo(sortIdxDstT, sortIdxDstTFP32, MxBase::TensorDType::FLOAT32, stream);
            if (ret != APP_ERR_OK) {
                LogError << "Fail to convert to int32 sortIdx tensor to float32 for SortCore function."
                         << GetErrorInfo(ret);
                return ret;
            }

            Tensor fp32SortIdxDstT;
            ret = Transpose(sortIdxDstTFP32, fp32SortIdxDstT, std::vector<int>{1, 0}, stream);
            if (ret != APP_ERR_OK) {
                LogError << "Fail to transpose float32 dstIdx tensor for SortCore function." << GetErrorInfo(ret);
                return ret;
            }

            ret = ConvertTo(fp32SortIdxDstT, sortIdxDst, MxBase::TensorDType::INT32, stream);
            if (ret != APP_ERR_OK) {
                LogError << "Fail to ConvertTo float32 sortIdx tensor to int32 for SortCore function."
                         << GetErrorInfo(ret);
                return ret;
            }

            ret = Transpose(sortDstT, sortDst, std::vector<int>{1, 0}, stream);
            if (ret != APP_ERR_OK) {
                LogError << "Fail to transpose dst tensor for SortCore function." << GetErrorInfo(ret);
                return ret;
            }
        } else {
            Tensor dst;
            ret = SortCore(src, sortDst, sortIdxDst, axis, descending, stream);
            if (ret != APP_ERR_OK) {
                LogError << "Fail to execute Sort op for SortCore function." << GetErrorInfo(ret);
                return ret;
            }
        }
        return ret;
    }

    APP_ERROR SortIdx(const Tensor &src, Tensor &dstIdx, int axis, bool descending, AscendStream &stream)
    {
        LogDebug << "Start to execute SortIdx function.";
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckSortOpParams(src, axis, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to check input parameters for SortIdx function ." << GetErrorInfo(ret);
            return ret;
        }

        Tensor dst;
        ret = SortMain(src, dst, dstIdx, axis, descending, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to execute Sort op for SortIdx function." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> srcVec = {dst};
        ret = AddStreamRef(srcVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Add stream reference failed." << GetErrorInfo(ret);
        }
        LogDebug << "Success to execute SortIdx function.";
        return ret;
    }

    APP_ERROR Sort(const Tensor &src, Tensor &dst, int axis, bool descending, AscendStream &stream)
    {
        LogDebug << "Start to execute Sort function.";
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckSortOpParams(src, axis, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to check input parameters for Sort function ." << GetErrorInfo(ret);
            return ret;
        }

        Tensor dstIdx;
        ret = SortMain(src, dst, dstIdx, axis, descending, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to execute Sort op for Sort function." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> srcVec = {dstIdx};
        ret = AddStreamRef(srcVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Add stream reference failed." << GetErrorInfo(ret);
        }
        LogDebug << "Success to execute Sort function.";
        return ret;
    }

    APP_ERROR Compare(const Tensor &src1, const Tensor &src2, Tensor &dst,
                      const CmpOp cmpOp, AscendStream &stream)
    {
        LogDebug << "Start to execute Compare op.";
        std::string opType = "Compare";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        std::string operation;
        auto operationIter = toCmpOpMap.find(cmpOp);
        if (operationIter != toCmpOpMap.end()) {
            operation = operationIter->second;
        } else {
            LogError << "The operation must be [CMP_EQ/CMP_NE/CMP_LT/CMP_GT/CMP_LE/CMP_GE]."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        // 2. OperatorImplicitMalloc
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), src1.GetDataType(), src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Construct the Attr description
        OpAttrDesc attrOperation = {OpAttrType::STRING, "operation", (void*)(operation.c_str())};
        std::vector<OpAttrDesc> opAttrDescs = {attrOperation};

        // 4. Execute RunOp
        CommonAclnnPara ascendcParam{std::vector<int>{static_cast<int>(cmpOp)}, std::vector<float>{}};
        RunOpParam compareParam{opType, srcVec, dstVec};
        compareParam.ascendcParam = ascendcParam;
        compareParam.dslParam.opAttrDesc = opAttrDescs;
        ret = RunOp(compareParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }
        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR Rescale(const Tensor &src, Tensor &dst, float scale, float bias, AscendStream& stream)
    {
        LogDebug << "Start to execute Rescale op.";
        std::string opType = "Rescale";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype);
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
        std::vector<Tensor> dstVec = {dst};

        // 3. Set op attr
        OpAttrDesc opAttrDesc1(OpAttrType::FLOAT, "scale", static_cast<void *>(&scale));
        OpAttrDesc opAttrDesc2(OpAttrType::FLOAT, "bias", static_cast<void *>(&bias));
        std::vector<OpAttrDesc> opAttrDescVec{opAttrDesc1, opAttrDesc2};

        // 4. Execute RunOp
        CommonAclnnPara ascendcParam{std::vector<int>{}, std::vector<float>{scale, bias}};
        RunOpParam rescaleParam{opType, srcVec, dstVec};
        rescaleParam.ascendcParam = ascendcParam;
        rescaleParam.dslParam.opAttrDesc = opAttrDescVec;
        rescaleParam.dslParam.dslCompileOptValue = "enable";
        ret = RunOp(rescaleParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }
        LogDebug << opType << ": Run op finished.";
        return ret;
    }

    APP_ERROR Multiply(const Tensor &src1, const Tensor &src2, Tensor &dst, double scale, AscendStream& stream)
    {
        LogDebug << "Start to execute Multiply op.";
        std::string opType = "Multiply";

        // 1. Check parameters.
        std::vector<Tensor> srcVec = {src1, src2};
        OpSupportDtype opSupportDtype;
        APP_ERROR ret = CheckGeneralOpParams(srcVec, opSupportDtype, false, true);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Fail to check parameters." << GetErrorInfo(ret);
            return ret;
        }

        // 2. OperatorImplicitMalloc
        MxBase::TensorDType expectDstDType;
        GetOutputTensorDType(src1, src2, expectDstDType);
        ExpectedTensorInfo expectedTensorInfo = {src1.GetShape(), expectDstDType, src1.GetDeviceId()};
        ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Implicit malloc or dst tensor check failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<Tensor> dstVec = {dst};

        // 3. Set op attr
        auto scaleAttr = static_cast<float>(scale);
        OpAttrDesc opAttrDesc(OpAttrType::FLOAT, "scale", static_cast<void *>(&scaleAttr));
        std::vector<OpAttrDesc> opAttrDescVec{opAttrDesc};

        // 4. Execute RunOp
        CommonAclnnPara ascendcParam{std::vector<int>{}, std::vector<float>{scaleAttr}};
        RunOpParam multiplyParam{opType, srcVec, dstVec};
        multiplyParam.ascendcParam = ascendcParam;
        multiplyParam.dslParam.opAttrDesc = opAttrDescVec;
        ret = RunOp(multiplyParam, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Run op failed." << GetErrorInfo(ret);
        } else {
            LogDebug << opType << ": Run op success.";
        }
        LogDebug << opType << ": Run op finished.";
        return ret;
    }
}
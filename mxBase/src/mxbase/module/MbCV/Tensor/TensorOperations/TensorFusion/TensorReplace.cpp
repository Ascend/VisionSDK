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
 * Description: Tensor Replace implement file
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */
#include "MxBase/E2eInfer/TensorOperation/TensorReplace.h"
#include "MxBase/E2eInfer/DataType.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/TikFramework.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/OpCall.h"
#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/PerElementOperations.h"

namespace MxBase {
namespace {
constexpr int CHANNALS_ONE = 1;
constexpr int CHANNALS_THREE = 3;
constexpr int TILING_TENSOR_INDEX = 3;
}
static APP_ERROR TensorReplaceTillingFunc(int numInputs, const aclTensorDesc * const inputDesc[], int32_t numOutputs,
    const aclTensorDesc * const outputDesc[], const aclopAttr *opAttr, aclopKernelDesc *aclopKernelDesc);

static APP_ERROR TensorReplaceNormalizeTillingFunc(int numInputs, const aclTensorDesc * const inputDesc[],
    int32_t numOutputs, const aclTensorDesc * const outputDesc[], const aclopAttr *opAttr,
    aclopKernelDesc *aclopKernelDesc);

struct TensorReplaceParam {
    TensorImgParam src;
    TensorImgParam replace;
    TensorImgParam mask;
    TensorImgParam dst;

    uint32_t GetRoiH() const
    {
        uint32_t roiH = std::min(std::min(src.roiH, replace.roiH), mask.roiH);
        return dst.tensor_.IsEmpty() ? roiH : std::min(roiH, dst.roiH);
    }

    uint32_t GetRoiW() const
    {
        uint32_t roiW = std::min(std::min(src.roiW, replace.roiW), mask.roiW);
        return dst.tensor_.IsEmpty() ? roiW : std::min(roiW, dst.roiW);
    }

    bool ShouldUseAscendC() const
    {
        return src.height == replace.height && src.height == mask.height && src.height == dst.height &&
               src.height == GetRoiH() &&
               src.width == replace.width && src.width == mask.width && src.width == dst.width &&
               src.width == GetRoiW();
    }
};

class TensorReplaceParamChecker {
public:
    TensorReplaceParamChecker(TensorImgParamChecker srcChecker, TensorImgParamChecker replaceChecker,
        TensorImgParamChecker maskChecker, TensorImgParamChecker dstChecker)
        : srcChecker_(srcChecker), replaceChecker_(replaceChecker), maskChecker_(maskChecker), dstChecker_(dstChecker)
    {}

    bool CheckDst(const TensorReplaceParam &param) const
    {
        if (param.src.deviceId != param.dst.deviceId) {
            LogError << "TensorReplaceParamChecker: all tensors must be on the same device. Now "
                     << param.src.tensorName << " on device " << param.src.deviceId << ", "
                     << param.replace.tensorName << " on device " << param.replace.deviceId << ", "
                     << param.mask.tensorName << " on device " << param.mask.deviceId << ", "
                     << param.dst.tensorName << " on device " << param.dst.deviceId << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (param.src.dataType != param.dst.dataType) {
            LogError << "TensorReplaceParamChecker: The data type of "
                     << param.src.tensorName << "(" << TensorImgParamChecker::ToString(param.src.dataType) << ") and "
                     << param.dst.tensorName << "(" << TensorImgParamChecker::ToString(param.dst.dataType)
                     << ") are not equal." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (param.src.channels != param.dst.channels) {
            LogError << "TensorReplaceParamChecker: The channels of "
                     << param.src.tensorName << "(" << param.src.channels << ") and "
                     << param.dst.tensorName << "(" << param.dst.channels << ") are not equal."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        return true;
    }

    bool Check(const TensorReplaceParam &param, int32_t deviceId)
    {
        if (!(srcChecker_.Check(param.src) && replaceChecker_.Check(param.replace) && maskChecker_.Check(param.mask) &&
            dstChecker_.Check(param.dst))) {
            return false;
        }
        if (param.src.deviceId != param.replace.deviceId || param.src.deviceId != param.mask.deviceId) {
            LogError << "TensorReplaceParamChecker: all tensors must be on the same device. Now "
                     << param.src.tensorName << " on device " << param.src.deviceId << ", "
                     << param.replace.tensorName << " on device " << param.replace.deviceId << ", "
                     << param.mask.tensorName << " on device " << param.mask.deviceId << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (param.src.deviceId != deviceId) {
            LogError << "TensorReplaceParamChecker: src tensor and stream must be on the same device. Now "
                     << param.src.tensorName << " on device " << param.src.deviceId
                     << ", stream on device " << deviceId << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (param.src.dataType != param.replace.dataType) {
            LogError << "TensorReplaceParamChecker: The data type of "
                     << param.src.tensorName << "(" << TensorImgParamChecker::ToString(param.src.dataType) << ") and "
                     << param.replace.tensorName << "(" << TensorImgParamChecker::ToString(param.replace.dataType)
                     << ") are not equal." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (param.src.channels != param.replace.channels) {
            LogError << "TensorReplaceParamChecker: The channels of "
                     << param.src.tensorName << "(" << param.src.channels << ") and "
                     << param.replace.tensorName << "(" << param.replace.channels << ") are not equal."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if ((!param.dst.tensor_.IsEmpty()) && (!CheckDst(param))) {
            return false;
        }
        if (param.src.channels == CHANNALS_ONE && param.mask.channels == CHANNALS_THREE) {
            LogError << "TensorReplaceParamChecker: The channels of "
                     << param.src.tensorName << "(" << param.src.channels << ") and "
                     << param.mask.tensorName << "(" << param.mask.channels << ") are not supported."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        return true;
    }

private:
    TensorImgParamChecker srcChecker_;
    TensorImgParamChecker replaceChecker_;
    TensorImgParamChecker maskChecker_;
    TensorImgParamChecker dstChecker_;
};

namespace {
constexpr int TENSOR_REPLACE_AI_CORES_NUMS = 8;
constexpr int TENSOR_REPLACE_TILING_PARAMS_NUM = 17;
const aclopEngineType TENSOR_REPLACE_ENGINE_TYPE = ACL_ENGINE_AICORE;
const std::string BACKGROUND_REPLACE_OP_TYPE = "background_replace";
const SingleOperator BACKGROUND_REPLACE_OP_U8{
    BACKGROUND_REPLACE_OP_TYPE.c_str(), "background_replace_u8.o",  "background_replace_u8__kernel0",
    "background_replace_u8__kernel0",   TENSOR_REPLACE_ENGINE_TYPE, TensorReplaceTillingFunc,
};
const SingleOperator BACKGROUND_REPLACE_OP_F16{
    BACKGROUND_REPLACE_OP_TYPE.c_str(), "background_replace_f16.o", "background_replace_f16__kernel0",
    "background_replace_f16__kernel0",  TENSOR_REPLACE_ENGINE_TYPE, TensorReplaceTillingFunc,
};
std::vector<SingleOperator> BACKGROUND_REPLACE_OPS{ BACKGROUND_REPLACE_OP_F16, BACKGROUND_REPLACE_OP_U8 };

const std::string BACKGROUND_REPLACE_NORMALIZE_OP_TYPE = "background_replace_normalize";
const SingleOperator BACKGROUND_REPLACE_NORMALIZE_OP_U8{
    BACKGROUND_REPLACE_NORMALIZE_OP_TYPE.c_str(),
    "background_replace_normalize_u8.o",
    "background_replace_normalize_u8__kernel0",
    "background_replace_normalize_u8__kernel0",
    TENSOR_REPLACE_ENGINE_TYPE,
    TensorReplaceNormalizeTillingFunc,
};
const SingleOperator BACKGROUND_REPLACE_NORMALIZE_OP_F16{
    BACKGROUND_REPLACE_NORMALIZE_OP_TYPE.c_str(),
    "background_replace_normalize_f16.o",
    "background_replace_normalize_f16__kernel0",
    "background_replace_normalize_f16__kernel0",
    TENSOR_REPLACE_ENGINE_TYPE,
    TensorReplaceNormalizeTillingFunc,
};
std::vector<SingleOperator> BACKGROUND_REPLACE_NORMALIZE_OPS{ BACKGROUND_REPLACE_NORMALIZE_OP_F16,
    BACKGROUND_REPLACE_NORMALIZE_OP_U8 };
// check opts
constexpr uint32_t BR_MIN_WIDTH = 1;
constexpr uint32_t BR_MIN_HEIGHT = 1;
constexpr uint32_t BR_MAX_WIDTH = 4096;
constexpr uint32_t BR_MAX_HEIGHT = 4096;
std::vector<uint32_t> BR_HW_RANGE{ BR_MIN_WIDTH, BR_MIN_HEIGHT, BR_MAX_WIDTH, BR_MAX_HEIGHT };
std::vector<MemoryData::MemoryType> BR_MEM_TYPE_OPTS{ MemoryData::MEMORY_DEVICE, MemoryData::MEMORY_DVPP };
std::vector<TensorDType> BR_SRC_DATA_TYPE_OPTS{ TensorDType::FLOAT16, TensorDType::UINT8 };
std::vector<uint32_t> BR_SRC_CHANNEL_OPTS{ 1, 3 };
std::vector<uint32_t> BR_SRC_DIM_OPTS{ 2, 3 };
std::vector<TensorDType> BR_MASK_DATA_TYPE_OPTS{ TensorDType::FLOAT16 };
std::vector<uint32_t> BR_MASK_CHANNEL_OPTS{ 1, 3 };
std::vector<uint32_t> BR_MASK_DIM_OPTS{ 2, 3 };

auto TENSOR_REPLACE_PARAM_CHECKER = TensorReplaceParamChecker(TensorImgParamChecker("background", BR_HW_RANGE,
    BR_MEM_TYPE_OPTS, BR_SRC_DATA_TYPE_OPTS, BR_SRC_CHANNEL_OPTS, BR_SRC_DIM_OPTS),
    TensorImgParamChecker("replace", BR_HW_RANGE, BR_MEM_TYPE_OPTS, BR_SRC_DATA_TYPE_OPTS, BR_SRC_CHANNEL_OPTS,
    BR_SRC_DIM_OPTS),
    TensorImgParamChecker("mask", BR_HW_RANGE, BR_MEM_TYPE_OPTS, BR_MASK_DATA_TYPE_OPTS, BR_MASK_CHANNEL_OPTS,
    BR_MASK_DIM_OPTS),
    TensorImgParamChecker("dst", BR_HW_RANGE, BR_MEM_TYPE_OPTS, BR_SRC_DATA_TYPE_OPTS, BR_SRC_CHANNEL_OPTS,
    BR_SRC_DIM_OPTS, true));
}

static APP_ERROR SetTilingParam(const aclTensorDesc * const inputDesc[], uint32_t *param)
{
    APP_ERROR ret = APP_ERR_OK;
    for (int i = 0; i < TENSOR_REPLACE_TILING_PARAMS_NUM; i++) {
        int64_t tmp;
        if ((ret = aclGetTensorDescDimV2(inputDesc[TILING_TENSOR_INDEX], i + 1, &tmp)) != ACL_SUCCESS) {
            LogError << "Tensor tiling get desc failed." << GetErrorInfo(ret, "aclGetTensorDescDimV2");
            return ret;
        }
        param[i] = static_cast<uint32_t>(tmp);
    }
    return ret;
}

static APP_ERROR TensorReplaceTillingFunc(int, const aclTensorDesc * const inputDesc[], int32_t,
    const aclTensorDesc * const[], const aclopAttr *, aclopKernelDesc *aclOpKernelDesc)
{
    const char *kernelId;
    APP_ERROR ret = APP_ERR_OK;
    aclDataType srcDataType = aclGetTensorDescType(inputDesc[0]);
    switch (srcDataType) {
        case ACL_UINT8:
            kernelId = BACKGROUND_REPLACE_OP_U8.kernelId;
            break;
        case ACL_FLOAT16:
            kernelId = BACKGROUND_REPLACE_OP_F16.kernelId;
            break;
        default:
            LogError << "TensorReplaceTillingFunc: The src tensor data type only support fp16 or u8, please check!" <<
                GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
    }

    uint32_t param[TENSOR_REPLACE_TILING_PARAMS_NUM] = { 0 };
    ret = SetTilingParam(inputDesc, param);
    if (ret != APP_ERR_OK) {
        LogError << "TensorReplaceTillingFunc: SetTilingParam failed!" << GetErrorInfo(ret);
        return ret;
    }
    ret = aclopSetKernelArgs(aclOpKernelDesc, kernelId, TENSOR_REPLACE_AI_CORES_NUMS, &param, sizeof(param));
    if (ret != APP_ERR_OK) {
        LogError << "TensorReplaceTillingFunc: aclopSetKernelArgs " << kernelId << " failed, please check!" <<
            GetErrorInfo(ret, "aclopSetKernelArgs");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

static APP_ERROR TensorReplaceNormalizeTillingFunc(int, const aclTensorDesc * const inputDesc[],
    int32_t, const aclTensorDesc * const[], const aclopAttr *,
    aclopKernelDesc *aclOpKernelDesc)
{
    const char *kernelId;
    APP_ERROR ret = APP_ERR_OK;
    aclDataType srcDataType = aclGetTensorDescType(inputDesc[0]);
    switch (srcDataType) {
        case ACL_UINT8:
            kernelId = BACKGROUND_REPLACE_NORMALIZE_OP_U8.kernelId;
            break;
        case ACL_FLOAT16:
            kernelId = BACKGROUND_REPLACE_NORMALIZE_OP_F16.kernelId;
            break;
        default:
            LogError <<
                "TensorReplaceNormalizeTillingFunc: The src tensor data type only support fp16 or u8, please check!" <<
                GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
    }
    uint32_t param[TENSOR_REPLACE_TILING_PARAMS_NUM] = { 0 };
    ret = SetTilingParam(inputDesc, param);
    if (ret != APP_ERR_OK) {
        LogError << "TensorReplaceNormalizeTillingFunc: SetTilingParam failed!" << GetErrorInfo(ret);
        return ret;
    }
    ret = aclopSetKernelArgs(aclOpKernelDesc, kernelId, TENSOR_REPLACE_AI_CORES_NUMS, &param, sizeof(param));
    if (ret != APP_ERR_OK) {
        LogError << "TensorReplaceNormalizeTillingFunc: aclopSetKernelArgs " << kernelId << " failed, please check!" <<
            GetErrorInfo(ret, "aclopSetKernelArgs");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

static void CalcTensorReplaceTillingTensor(Tensor &tilingTensor, TensorReplaceParam &params)
{
    std::vector<uint32_t> tilingShape{0, params.src.roiStart, params.src.height, params.src.width, params.src.channels,
    params.replace.roiStart, params.replace.height, params.replace.width, params.replace.channels, params.mask.roiStart,
    params.mask.height, params.mask.width, params.mask.channels, params.GetRoiH(), params.GetRoiW(),
    params.dst.roiStart, params.dst.height, params.dst.width};
    Tensor tilingParamsTensor(nullptr, tilingShape, MxBase::TensorDType::UINT32);
    tilingTensor = tilingParamsTensor;
}

static APP_ERROR InitTensorReplaceOpKernels()
{
    static bool isInited = false;
    if (isInited) {
        return APP_ERR_OK;
    }
    APP_ERROR ret = RegistOp(BACKGROUND_REPLACE_OPS);
    if (ret != APP_ERR_OK) {
        LogError << "InitTensorReplaceOpKernels: RegistOp failed." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "InitTensorReplaceOpKernels: register ops success.";
    isInited = true;
    return APP_ERR_OK;
}

static APP_ERROR InitTensorReplaceNormalizeOpKernels()
{
    static bool isInitedNormalize = false;
    if (isInitedNormalize) {
        return APP_ERR_OK;
    }
    APP_ERROR ret = RegistOp(BACKGROUND_REPLACE_NORMALIZE_OPS);
    if (ret != APP_ERR_OK) {
        LogError << "InitTensorReplaceNormalizeOpKernels: RegistOp failed." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "InitTensorReplaceNormalizeOpKernels: register ops success.";
    isInitedNormalize = true;
    return APP_ERR_OK;
}

static APP_ERROR BackgroundReplaceAscendC(Tensor &background, const Tensor &replace, const Tensor &mask, Tensor &dst,
                                          AscendStream &stream)
{
    std::string opType = "BackgroundReplace";
    std::vector<Tensor> srcVec = {background, replace, mask};
    std::vector<Tensor> dstVec = {dst};

    RunOpParam bkgParam{opType, srcVec, dstVec};
    APP_ERROR ret = RunOp(bkgParam, stream);
    if (ret != APP_ERR_OK) {
        LogError << opType << "Run op failed." << GetErrorInfo(ret);
    }
    return ret;
}
APP_ERROR TensorReplace(Tensor &background, const Tensor &replace, const Tensor &mask, Tensor &dst,
    const bool normalizeFlag, AscendStream &stream)
{
    APP_ERROR ret = normalizeFlag ? InitTensorReplaceNormalizeOpKernels() : InitTensorReplaceOpKernels();
    if (ret != APP_ERR_OK) {
        LogError << "TensorReplace: RegistOp failed." << GetErrorInfo(ret);
        return ret;
    }

    TensorReplaceParam tmpParams{
        TensorImgParam{ "background", background },
        TensorImgParam{ "replace", replace }, TensorImgParam{ "mask", mask },
        TensorImgParam{ "dst", dst, true },
    };
    if (!TENSOR_REPLACE_PARAM_CHECKER.Check(tmpParams, stream.GetDeviceId())) {
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (dst.IsEmpty()) {
        std::vector<uint32_t> shape = background.GetShape();
        shape[0] = tmpParams.GetRoiH();
        shape[1] = tmpParams.GetRoiW();
        ExpectedTensorInfo expectedTensorInfo = { shape, background.GetDataType(), background.GetDeviceId() };
        APP_ERROR ret = OperatorImplicitMallocTensor(dst, expectedTensorInfo);
        if (ret != APP_ERR_OK) {
            LogError << "TensorReplace: Implicit malloc dst tensor failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    TensorReplaceParam params{ tmpParams.src, tmpParams.replace, tmpParams.mask, TensorImgParam{ "dst", dst } };

    if (params.ShouldUseAscendC() && !normalizeFlag) {
        return BackgroundReplaceAscendC(background, replace, mask, dst, stream);
    }
    Tensor tilingTensor;
    CalcTensorReplaceTillingTensor(tilingTensor, params);
    std::vector<Tensor> inputs{ background, replace, mask, tilingTensor };
    std::vector<Tensor> outputs{ dst };
    if (normalizeFlag) {
        ret = ExecuteOperator(BACKGROUND_REPLACE_NORMALIZE_OP_TYPE, inputs, outputs, stream);
    } else {
        ret = ExecuteOperator(BACKGROUND_REPLACE_OP_TYPE, inputs, outputs, stream);
    }
    if (ret != APP_ERR_OK) {
        LogError << "TensorReplace: execute op " << BACKGROUND_REPLACE_OP_TYPE << " failed." << GetErrorInfo(ret);
    }
    return ret;
}
}
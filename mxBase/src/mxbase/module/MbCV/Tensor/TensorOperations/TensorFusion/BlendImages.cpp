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
 * Description: Tensor BlendImages Operation
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */
#include "MxBase/Log/Log.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/CommonUtils.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/DslFramework.h"
#include "MxBase/E2eInfer/TensorOperation/TensorReplace.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/OpCall.h"
#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/PerElementOperations.h"
#include "MxBase/DeviceManager/DeviceManager.h"

namespace MxBase {
struct BlendImagesParam {
    TensorImgParam material;
    TensorImgParam frame;
    uint32_t GetRoiH()
    {
        return std::min(material.roiH, frame.roiH);
    }

    uint32_t GetRoiW()
    {
        return std::min(material.roiW, frame.roiW);
    }
};

class BlendImagesParamChecker {
public:
    BlendImagesParamChecker(TensorImgParamChecker materialChecker, TensorImgParamChecker frameChecker)
        : materialChecker_(materialChecker), frameChecker_(frameChecker)
    {}

    bool Check(const BlendImagesParam &param, int32_t deviceId)
    {
        if (!(materialChecker_.Check(param.material) && frameChecker_.Check(param.frame))) {
            return false;
        }

        if (param.material.deviceId != param.frame.deviceId) {
            LogError << "BlendImagesParamChecker: all tensors must be on the same device."
                     << "Now material on device " << param.material.deviceId << ", frame on device " <<
                param.frame.deviceId << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (param.frame.deviceId != deviceId) {
            LogError << "BlendImagesParamChecker: frame tensor and stream must be on the same device. Now "
                "frame on device " <<
                param.frame.deviceId << ", stream on device " << deviceId << "." <<
                GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (param.material.dataType != param.frame.dataType) {
            LogError << "BlendImagesParamChecker: The data type of material and frame are not equal." <<
                GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        return true;
    }

private:
    TensorImgParamChecker materialChecker_;
    TensorImgParamChecker frameChecker_;
};

namespace {
constexpr uint32_t BLEND_IMAGES_MIN_WIDTH = 1;
constexpr uint32_t BLEND_IMAGES_MIN_HEIGHT = 1;
constexpr uint32_t BLEND_IMAGES_MAX_WIDTH = 4096;
constexpr uint32_t BLEND_IMAGES_MAX_HEIGHT = 4096;
constexpr uint32_t BLEND_IMAGES_RGBA_ALPHA_CHANNEL = 3;
constexpr uint32_t RGB_C_CHNNEL = 3;
constexpr uint32_t ALPHA_C_CCHNNEL = 1;
constexpr uint32_t HWC_SHAPE_SIZE = 3;
constexpr uint32_t SHAPE_DIMENSION_ZERO = 0;
constexpr uint32_t SHAPE_DIMENSION_ONE = 1;
constexpr int MATERIAL_RGB_INDEX = 0;
constexpr int MATERIAL_ALPHA_INDEX = 1;
constexpr int SPLIT_SECTIONS = 3;
constexpr int SPLIT_DIMENSION = 2;
std::vector<uint32_t> BLEND_IMAGES_HW_RANGE{ BLEND_IMAGES_MIN_WIDTH, BLEND_IMAGES_MIN_HEIGHT, BLEND_IMAGES_MAX_WIDTH,
    BLEND_IMAGES_MAX_HEIGHT };
std::vector<MemoryData::MemoryType> BLEND_IMAGES_MEM_TYPE_OPTS{ MemoryData::MEMORY_DEVICE, MemoryData::MEMORY_DVPP };
std::vector<TensorDType> BLEND_IMAGES_MATERIAL_DATA_TYPE_OPTS{ TensorDType::UINT8 };
std::vector<uint32_t> BLEND_IMAGES_MATERIAL_CHANNEL_OPTS{ 4 };
std::vector<uint32_t> BLEND_IMAGES_FRAME_CHANNEL_OPTS{ 3 };
std::vector<uint32_t> BLEND_IMAGES_MATERIAL_DIM_OPTS{ 3 };

auto BLEND_IMAGES_PARAM_CHECKER =
    BlendImagesParamChecker(TensorImgParamChecker("material", BLEND_IMAGES_HW_RANGE, BLEND_IMAGES_MEM_TYPE_OPTS,
    BLEND_IMAGES_MATERIAL_DATA_TYPE_OPTS, BLEND_IMAGES_MATERIAL_CHANNEL_OPTS, BLEND_IMAGES_MATERIAL_DIM_OPTS),
    TensorImgParamChecker("frame", BLEND_IMAGES_HW_RANGE, BLEND_IMAGES_MEM_TYPE_OPTS,
    BLEND_IMAGES_MATERIAL_DATA_TYPE_OPTS, BLEND_IMAGES_FRAME_CHANNEL_OPTS, BLEND_IMAGES_MATERIAL_DIM_OPTS));
}

static APP_ERROR RunBlendImagesAclnnOp(const Tensor &rbg, const Tensor &alpha, Tensor &frame, AscendStream &stream)
{
    std::vector<Tensor> srcVec = { rbg, alpha, frame };
    bool isMemTypeDvpp = frame.GetMemoryType() == MemoryData::MEMORY_DVPP ? true : false;
    Tensor newOutputTensor(frame.GetShape(), TensorDType::UINT8, frame.GetDeviceId(), isMemTypeDvpp);
    APP_ERROR ret = newOutputTensor.Malloc();
    if (ret != APP_ERR_OK) {
        LogError << "Malloc memory of output tensor failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = aclrtMemcpyAsync(newOutputTensor.GetData(), newOutputTensor.GetByteSize(), frame.GetData(),
                           frame.GetByteSize(), ACL_MEMCPY_DEVICE_TO_DEVICE, stream.stream);
    if (ret != APP_ERR_OK) {
        LogError << "Call aclrtMemcpyAsync failed in blend images op." << GetErrorInfo(ret, "aclrtMemcpyAsync");
        return ret;
    }
    if (IsSetReferRect(frame)) {
        newOutputTensor.SetReferRect(frame.GetReferRect());
    }
    std::vector<Tensor> dstVec = { newOutputTensor };
    std::string opType = "BlendImages";
    RunOpParam tmpParam{opType, srcVec, dstVec};
    ret = RunOp(tmpParam, stream);
    if (ret != APP_ERR_OK) {
        LogError << opType << ": Run op failed." << GetErrorInfo(ret);
    } else {
        ret = aclrtMemcpyAsync(frame.GetData(), frame.GetByteSize(), newOutputTensor.GetData(),
                               newOutputTensor.GetByteSize(), ACL_MEMCPY_DEVICE_TO_DEVICE, stream.stream);
        if (ret != APP_ERR_OK) {
            LogError << "Call aclrtMemcpyAsync failed in blend images op." << GetErrorInfo(ret, "aclrtMemcpyAsync");
            return ret;
        }
        ret = AddStreamRef(dstVec, stream);
        if (ret != APP_ERR_OK) {
            LogError << opType << ": Failed to add stream reference." << GetErrorInfo(ret);
        }
        LogDebug << opType << ": Run op success.";
    }
    return ret;
}

static APP_ERROR RunBlendImagesTikOp(Tensor &materialRGB, const Tensor &materialAlphaTensor,
                                     Tensor &frame, const BlendImagesParam &tmpParams, AscendStream &stream)
{
    Tensor mask = Tensor(materialAlphaTensor.GetShape(), TensorDType::FLOAT16,
                         materialAlphaTensor.GetDeviceId());
    APP_ERROR ret = mask.Malloc();
    if (ret != APP_ERR_OK) {
        LogError << "RunBlendImagesTikOp: Mask tensor malloc failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = ConvertTo(materialAlphaTensor, mask, TensorDType::FLOAT16, stream);
    if (ret != APP_ERR_OK) {
        LogError << "RunBlendImagesTikOp: ConvertTo failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = materialRGB.SetReferRect(Rect(tmpParams.material.roiX0, tmpParams.material.roiY0,
                                        tmpParams.material.roiX1, tmpParams.material.roiY1));
    if (ret != APP_ERR_OK) {
        LogError << "RunBlendImagesTikOp: materialRGB SetReferRect failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = mask.SetReferRect(Rect(tmpParams.material.roiX0, tmpParams.material.roiY0,
                                 tmpParams.material.roiX1, tmpParams.material.roiY1));
    if (ret != APP_ERR_OK) {
        LogError << "RunBlendImagesTikOp: mask SetReferRect failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = TensorReplace(frame, materialRGB, mask, frame, true, stream);
    if (ret != APP_ERR_OK) {
        LogError << "RunBlendImagesTikOp: TensorReplace failed." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR SplitMaterial(const Tensor &material, std::vector<Tensor> &tv, AscendStream &stream)
{
    std::string opType = "SplitD";
    std::vector<Tensor> srcVec = {material};
    std::vector<uint32_t> shape = material.GetShape();
    std::vector<uint32_t> rgbShape = {shape[SHAPE_DIMENSION_ZERO], shape[SHAPE_DIMENSION_ONE], RGB_C_CHNNEL};
    std::vector<uint32_t> alphaShape = {shape[SHAPE_DIMENSION_ZERO], shape[SHAPE_DIMENSION_ONE], ALPHA_C_CCHNNEL};
    Tensor rgbTensor;
    ExpectedTensorInfo expectedRgbTensorInfo = {rgbShape, material.GetDataType(), material.GetDeviceId()};
    APP_ERROR ret = OperatorImplicitMallocTensor(rgbTensor, expectedRgbTensorInfo);
    if (ret != APP_ERR_OK) {
        LogError << "BlendImage: Implicit malloc rgb Tensor failed." << GetErrorInfo(ret);
        return ret;
    }
    tv.push_back(rgbTensor);
    Tensor alphaTensor;
    ExpectedTensorInfo expectedAlphaTensorInfo = {alphaShape, material.GetDataType(), material.GetDeviceId()};
    ret = OperatorImplicitMallocTensor(alphaTensor, expectedAlphaTensorInfo);
    if (ret != APP_ERR_OK) {
        LogError << "BlendImage: Implicit malloc alpha Tensor failed." << GetErrorInfo(ret);
        return ret;
    }
    tv.push_back(alphaTensor);
    CommonAclnnPara ascendcParam{std::vector<int>{SPLIT_DIMENSION, SPLIT_SECTIONS}, std::vector<float>{}};
    RunOpParam splitParam{opType, srcVec, tv};
    splitParam.ascendcParam = ascendcParam;
    return RunOp(splitParam, stream);
}

APP_ERROR BlendImages(const Tensor &material, Tensor &frame, AscendStream &stream)
{
    if (!DeviceManager::IsAscend310P()) {
        LogError << "BlendImages is only supported on 310P now." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    BlendImagesParam tmpParams{ TensorImgParam{ "material", material }, TensorImgParam{ "frame", frame, true } };
    if (!BLEND_IMAGES_PARAM_CHECKER.Check(tmpParams, stream.GetDeviceId())) {
        return APP_ERR_COMM_INVALID_PARAM;
    }

    std::vector<MxBase::Tensor> materialTensors;
    APP_ERROR ret = SplitMaterial(material, materialTensors, stream);
    if (ret != APP_ERR_OK) {
        LogError << "BlendImages: Split material failed." << GetErrorInfo(ret);
        return ret;
    }
    std::vector<uint32_t> mShape = material.GetShape();
    std::vector<uint32_t> fShape = frame.GetShape();
    if (mShape[1] == fShape[1] && fShape[1] == tmpParams.GetRoiW() && mShape[0] == tmpParams.GetRoiH()) {
        LogDebug << "RunBlendImagesAclnnOp begin.";
        ret = RunBlendImagesAclnnOp(materialTensors[MATERIAL_RGB_INDEX],
                                    materialTensors[MATERIAL_ALPHA_INDEX], frame, stream);
        if (ret != APP_ERR_OK) {
            LogError << "BlendImages: RunBlendImagesAclnnOp failed." << GetErrorInfo(ret);
        }
    } else {
        LogDebug << "RunBlendImagesTikOp begin.";
        ret = RunBlendImagesTikOp(materialTensors[MATERIAL_RGB_INDEX],
                                  materialTensors[MATERIAL_ALPHA_INDEX], frame, tmpParams, stream);
        if (ret != APP_ERR_OK) {
            LogError << "BlendImages: RunBlendImagesTikOp failed." << GetErrorInfo(ret);
        }
    }
    return ret;
}
}
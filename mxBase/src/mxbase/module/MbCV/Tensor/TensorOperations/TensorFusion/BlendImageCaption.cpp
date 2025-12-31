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
* Description: Manage Tensor Warp Operations.
* Author: MindX SDK
* Create: 2023
* History: NA
*/

#include "MxBase/Log/Log.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/CommonUtils.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/TikFramework.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace {
    static const size_t PARAMS_LEN_INT32 = 128;
    static const size_t MAX_RECT_WIDTH = 1920;
    static const size_t MAX_SIZE = 4096;
    static const size_t MIN_SIZE = 64;
    static const int CAPTION_A_IDX = 14;
    static const int CAPTION_BG_IDX = 21;
    static const int CHN_OFFSET = 2;
    static const int X0_OFFSET = 3;
    static const int Y0_OFFSET = 4;
    static const int X1_OFFSET = 5;
    static const int Y1_OFFSET = 6;
    static const int CAP_OFFSET = 7;
    static const int TILING_PARAMS_NUM = 29;
    static const int CAPTION_OPACITY = 28;
    static const int AI_CORE_NUMS_310P = 8;
    static const int PERCENT = 100;
    static const size_t NHWC_SIZE = 4;
    static const size_t HW_SIZE = 2;
    static const size_t RGB_CHANNEL = 3;
    static const int TILING_TENSOR_INDEX = 4;
    static const aclopEngineType ENGINE_TYPE = ACL_ENGINE_AICORE;
    const float EPSILON = 1e-6;
}

namespace MxBase {
static APP_ERROR SetTilingParam(const aclTensorDesc * const inputDesc[], int32_t *params)
{
    APP_ERROR ret = APP_ERR_OK;
    for (int i = 0; i < TILING_PARAMS_NUM; i++) {
        int64_t tmp = 0;
        /* Set tilingShape[1-29] to params[0-28]. */
        if ((ret = aclGetTensorDescDimV2(inputDesc[TILING_TENSOR_INDEX], i + 1, &tmp)) != ACL_SUCCESS) {
            LogError << "Tensor tiling get desc failed." << GetErrorInfo(ret, "aclGetTensorDescDimV2");
            return APP_ERR_ACL_FAILURE;
        }
        params[i] = static_cast<int32_t>(tmp);
    }
    return ret;
}

static APP_ERROR GetBlendTillingParam(int, const aclTensorDesc *const inputDesc[], int32_t,
                                      const aclTensorDesc *const[], const aclopAttr *,
                                      aclopKernelDesc *aclopKernelDesc)
{
    int32_t params[TILING_PARAMS_NUM] = { 0 };
    APP_ERROR ret = SetTilingParam(inputDesc, params);
    if (ret != APP_ERR_OK) {
        LogError << "GetBlendTillingParam: SetTilingParam failed!" << GetErrorInfo(ret);
        return ret;
    }
    ret = aclopSetKernelArgs(aclopKernelDesc, "blend_image_caption__kernel0", AI_CORE_NUMS_310P, &params,
                             sizeof(params));
    if (ret != APP_ERR_OK) {
        LogError << "Call aclopSetKernelArgs in tilling callback failed." << GetErrorInfo(ret, "aclopSetKernelArgs");
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR GetTensorHWC(const Tensor &tensor, size_t &width, size_t &height, size_t &channel)
{
    if (tensor.IsEmpty() || (tensor.GetMemoryType() != MemoryData::MemoryType::MEMORY_DEVICE &&
        tensor.GetMemoryType() != MemoryData::MemoryType::MEMORY_DVPP)) {
        LogError << "Tensor should not be empty and not be on host." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (tensor.GetDataType() != TensorDType::UINT8) {
        LogError << "Tensor should be uint8 in caption blending." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    auto temShape = tensor.GetShape();
    size_t shapeSize = temShape.size();
    if (shapeSize > NHWC_SIZE || shapeSize < HW_SIZE) {
        LogError << "Only tensor of NHWC/HWC/HW is available." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (shapeSize == NHWC_SIZE && temShape[0] != 1) {
        LogError << "Batch size should be 1." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    channel = shapeSize == CHN_OFFSET ? 1 :  temShape[shapeSize - 1];
    if (channel != 1 && channel != RGB_CHANNEL) {
        LogError << "Channel should be 1 or " << RGB_CHANNEL << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    size_t offset = shapeSize == NHWC_SIZE ? 1 : 0;
    height = temShape[offset];
    width = temShape[offset + 1];
    if (width < MIN_SIZE || width > MAX_SIZE || height < MIN_SIZE || height > MAX_SIZE) {
        LogError << "The height and width should be in [" << MIN_SIZE << ", " << MAX_SIZE << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

static APP_ERROR CheckAndSetRect(const Tensor &tensor, int startIdx, Size size, size_t expectChannel,
    std::vector<int32_t>& params)
{
    size_t expectWidth = size.width;
    size_t expectHeight = size.height;
    std::string name = "frame";
    name = startIdx == CAPTION_A_IDX ? "captionAlpha" : name;
    name = startIdx == CAPTION_BG_IDX ? "captionBg" : name;
    size_t tensorHeight = 0;
    size_t tensorWidth = 0;
    size_t tensorChannel = 0;
    if (GetTensorHWC(tensor, tensorWidth, tensorHeight, tensorChannel) != APP_ERR_OK) {
        LogError << "Check " << name << " tensor failed. " << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (startIdx == CAPTION_A_IDX) {
        if (tensorChannel != 1) {
            LogError << "Tensor captionAlpha should be one channel." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    } else if (tensorChannel != expectChannel) {
        LogError << "Tensor " << name << " should be " << expectChannel << " channel, which is same as caption tensor."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    params[startIdx] = static_cast<int32_t>(tensorWidth);
    params[startIdx + 1] = static_cast<int32_t>(tensorHeight);
    params[startIdx + CHN_OFFSET] = static_cast<int32_t>(tensorChannel);
    Rect tensorReferRect = tensor.GetReferRect();
    if (tensorReferRect.x1 == 0 && tensorReferRect.y1 == 0) {
        if (expectWidth != tensorWidth || expectHeight != tensorHeight) {
            LogError << "Tensor " << name << " do not have a refer rect, so it should be equal to the caption "
                     << "rect of [" << expectWidth << ", " << expectHeight << "]"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    } else if (tensorReferRect.x1 - tensorReferRect.x0 != static_cast<uint32_t>(expectWidth) ||
                tensorReferRect.y1 - tensorReferRect.y0 != static_cast<uint32_t>(expectHeight)) {
        LogError << "The referRect of Tensor " << name << " should have the same size as caption referRect ["
                 << expectWidth << ", " << expectHeight << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    params[startIdx + X0_OFFSET] = static_cast<int32_t>(tensorReferRect.x0);
    params[startIdx + Y0_OFFSET] = static_cast<int32_t>(tensorReferRect.y0);
    params[startIdx + X1_OFFSET] = tensorReferRect.x1 == 0 ? static_cast<int32_t>(expectWidth) :
        static_cast<int32_t>(tensorReferRect.x1);
    params[startIdx + Y1_OFFSET] = tensorReferRect.y1 == 0 ? static_cast<int32_t>(expectHeight) :
        static_cast<int32_t>(tensorReferRect.y1);
    return APP_ERR_OK;
}

static APP_ERROR ConstructParamsData(const Tensor &frame, const Tensor &caption, const Tensor &captionAlpha,
    const Tensor &captionBg, std::vector<int32_t>& params)
{
    size_t expectHeight = 0;
    size_t expectWidth = 0;
    size_t capChn = 0;
    int deviceId = caption.GetDeviceId();
    if (GetTensorHWC(caption, expectWidth, expectHeight, capChn) != APP_ERR_OK) {
        LogError << "Check caption tensor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    Rect captionReferRect = caption.GetReferRect();
    params[CAP_OFFSET] = static_cast<int32_t>(expectWidth);
    params[CAP_OFFSET + 1] = static_cast<int32_t>(expectHeight);
    params[CAP_OFFSET + CHN_OFFSET] = static_cast<int32_t>(capChn);
    if (captionReferRect.x1 != 0 && captionReferRect.y1 != 0) {
        expectWidth = captionReferRect.x1 - captionReferRect.x0;
        expectHeight = captionReferRect.y1 - captionReferRect.y0;
    } else {
        LogDebug << "Caption does not contain the referRect, the total caption will be set as paste rect.";
    }
    if (expectWidth > MAX_RECT_WIDTH) {
        LogError << "The width of the overlay area exceed " << MAX_RECT_WIDTH
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (CheckAndSetRect(frame, 0, Size(expectWidth, expectHeight), capChn, params) != APP_ERR_OK ||
        CheckAndSetRect(captionAlpha, CAPTION_A_IDX, Size(expectWidth, expectHeight), capChn, params) != APP_ERR_OK ||
        CheckAndSetRect(captionBg, CAPTION_BG_IDX, Size(expectWidth, expectHeight), capChn, params) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (frame.GetDeviceId() != deviceId || captionAlpha.GetDeviceId() != deviceId ||
        captionBg.GetDeviceId() != deviceId) {
        LogError << "Tensor frame device("<< frame.GetDeviceId() << "), captionAlpha device("
                 << captionAlpha.GetDeviceId() << "), captionBg device("<< captionBg.GetDeviceId() << ") and tensor "
                 << "caption device("<< deviceId << ") should be same. " << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    params[CAP_OFFSET + X0_OFFSET] = static_cast<int32_t>(captionReferRect.x0);
    params[CAP_OFFSET + Y0_OFFSET] = static_cast<int32_t>(captionReferRect.y0);
    params[CAP_OFFSET + X1_OFFSET] = captionReferRect.x1 == 0 ? static_cast<int32_t>(caption.GetShape()[1]) :
        static_cast<int32_t>(captionReferRect.x1);
    params[CAP_OFFSET + Y1_OFFSET] = captionReferRect.y1 == 0 ? static_cast<int32_t>(caption.GetShape()[0]) :
        static_cast<int32_t>(captionReferRect.y1);
    return APP_ERR_OK;
}

APP_ERROR CheckCaptionBgOpacity(float captionBgOpacity)
{
    if (captionBgOpacity < 0 - EPSILON || captionBgOpacity > 1 + EPSILON) {
        LogError << "Float captionBgOpacity should be in [0, 1]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR BlendImageCaption(Tensor &frame, const Tensor &caption, const Tensor &captionAlpha, const Tensor &captionBg,
                            float captionBgOpacity, AscendStream& stream)
{
    std::vector<SingleOperator> op{{"blend_image_caption", "blend_image_caption.o", "blend_image_caption__kernel0",
        "blend_image_caption__kernel0", ENGINE_TYPE, GetBlendTillingParam}};
    APP_ERROR ret = CheckCaptionBgOpacity(captionBgOpacity);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (caption.GetDeviceId() != -1 && stream.GetDeviceId() != caption.GetDeviceId()) {
        LogError << "The input caption tensor device(" << caption.GetDeviceId() << ") and stream device("
                 << stream.GetDeviceId() << ") should be same." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::vector<int32_t> params(TILING_PARAMS_NUM);
    ret = ConstructParamsData(frame, caption, captionAlpha, captionBg, params);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    params[CAPTION_OPACITY] = static_cast<int32_t>(captionBgOpacity * PERCENT);
    std::vector<uint32_t> tilingShape(TILING_PARAMS_NUM + 1);
    /* Put params[0-28] to tilingShape[1-29], tilingShape[0] is 0, to avoid malloc memory. */
    for (int i = 1; i < TILING_PARAMS_NUM + 1; i++) {
        tilingShape[i] = static_cast<uint32_t>(params[i-1]);
    }
    Tensor tilling(nullptr, tilingShape, TensorDType::UINT32);
    std::vector<Tensor> inputs = {frame, caption, captionAlpha, captionBg, tilling};
    std::vector<Tensor> outputs = {frame};
    if (RegistOp(op) != APP_ERR_OK) {
        LogError << "Init single operator failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ret = ExecuteOperator(op[0].opType, inputs, outputs, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute operator for blend image caption " << op[0].opType
                 << GetErrorInfo(ret);
    }
    return ret;
}

}
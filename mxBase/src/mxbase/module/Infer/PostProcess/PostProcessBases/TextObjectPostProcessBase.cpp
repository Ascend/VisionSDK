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
 * Description: Accepts the model inference output tensor and outputs the target detection result.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/PostProcessBases/TextObjectPostProcessBase.h"

namespace MxBase {
TextObjectPostProcessBase &TextObjectPostProcessBase::operator = (const TextObjectPostProcessBase &other)
{
    if (this == &other) {
        return *this;
    }
    PostProcessBase::operator = (other);
    return *this;
}

APP_ERROR TextObjectPostProcessBase::Init(const std::map<std::string, std::string> &postConfig)
{
    LogInfo << "Begin to initialize TextObjectPostProcessBase. ";
    APP_ERROR ret = LoadConfigData(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "LoadConfigData failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = configData_.GetFileValue<bool>("CHECK_MODEL_FLAG", checkModelFlag_);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read CHECK_MODEL_FLAG from config, default is true";
    }
    LogInfo << "End to initialize TextObjectPostProcessBase.";
    return APP_ERR_OK;
}

APP_ERROR TextObjectPostProcessBase::DeInit()
{
    LogInfo << "Begin to deinitialize TextObjectPostProcessBase.";
    LogInfo << "End to deinitialize TextObjectPostProcessBase.";
    return APP_ERR_OK;
}

APP_ERROR TextObjectPostProcessBase::Process(const std::vector<TensorBase> &,
    std::vector<std::vector<TextObjectInfo>> &, const std::vector<ResizedImageInfo> &,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogError << "Process() has not been overridden in subclass!" << GetErrorInfo(APP_ERR_COMM_UNREALIZED);
    return APP_ERR_COMM_UNREALIZED;
}

void TextObjectPostProcessBase::FixCoords(uint32_t scrData, float &desData)
{
    if (desData > scrData) {
        desData = scrData;
    }
    desData = std::max(0.0f, std::min((float)scrData, desData));
}

void TextObjectPostProcessBase::ResizeReduction(const ResizedImageInfo &resizedImageInfo, TextObjectInfo &textObjInfo)
{
    if (resizedImageInfo.resizeType == RESIZER_STRETCHING) {
        textObjInfo.x0 *= resizedImageInfo.widthOriginal;
        textObjInfo.x1 *= resizedImageInfo.widthOriginal;
        textObjInfo.x2 *= resizedImageInfo.widthOriginal;
        textObjInfo.x3 *= resizedImageInfo.widthOriginal;
        textObjInfo.y0 *= resizedImageInfo.heightOriginal;
        textObjInfo.y1 *= resizedImageInfo.heightOriginal;
        textObjInfo.y2 *= resizedImageInfo.heightOriginal;
        textObjInfo.y3 *= resizedImageInfo.heightOriginal;
    } else if (JudgeResizeType(resizedImageInfo)) {
        float ratio = resizedImageInfo.keepAspectRatioScaling;
        LogDebug << "ctpn resizedImageInfo.keepAspectRatioScaling " << ratio;
        if (IsDenominatorZero(ratio)) {
            LogError << "The value of ratio must not equal to 0!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        textObjInfo.x0 *= resizedImageInfo.widthResize / ratio;
        textObjInfo.x1 *= resizedImageInfo.widthResize / ratio;
        textObjInfo.x2 *= resizedImageInfo.widthResize / ratio;
        textObjInfo.x3 *= resizedImageInfo.widthResize / ratio;
        textObjInfo.y0 *= resizedImageInfo.heightResize / ratio;
        textObjInfo.y1 *= resizedImageInfo.heightResize / ratio;
        textObjInfo.y2 *= resizedImageInfo.heightResize / ratio;
        textObjInfo.y3 *= resizedImageInfo.heightResize / ratio;
    }
    FixCoords(resizedImageInfo.widthOriginal, textObjInfo.x0);
    FixCoords(resizedImageInfo.widthOriginal, textObjInfo.x1);
    FixCoords(resizedImageInfo.widthOriginal, textObjInfo.x2);
    FixCoords(resizedImageInfo.widthOriginal, textObjInfo.x3);
    FixCoords(resizedImageInfo.heightOriginal, textObjInfo.y0);
    FixCoords(resizedImageInfo.heightOriginal, textObjInfo.y1);
    FixCoords(resizedImageInfo.heightOriginal, textObjInfo.y2);
    FixCoords(resizedImageInfo.heightOriginal, textObjInfo.y3);
}
}

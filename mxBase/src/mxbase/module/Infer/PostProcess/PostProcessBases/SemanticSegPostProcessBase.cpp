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
 * Description: Base class for semantic segmentation task post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/PostProcessBases/SemanticSegPostProcessBase.h"
#include "MxBase/ConfigUtil/ConfigUtil.h"
#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"

namespace MxBase {
const int MAX_IMAGE_EDGE = 8192;
const int MAX_RATIO = 16;
SemanticSegPostProcessBase& SemanticSegPostProcessBase::operator=(const SemanticSegPostProcessBase &other)
{
    if (this == &other) {
        return *this;
    }
    PostProcessBase::operator=(other);

    cropRoiBoxes_ = other.cropRoiBoxes_;
    classNum_ = other.classNum_;
    modelType_ = other.modelType_;

    return *this;
}

APP_ERROR SemanticSegPostProcessBase::GetSemanticSegConfigData()
{
    APP_ERROR ret = configData_.GetFileValue<uint32_t>("CLASS_NUM", classNum_, (uint32_t)0x0, (uint32_t)0x3e8);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to read CLASS_NUM from config, default value(" << classNum_
                 << ") will be used as classNum_." << GetErrorInfo(ret);
        return ret;
    }
    ret = configData_.GetFileValue<int>("MODEL_TYPE", modelType_, 0x0, 0x64);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read MODEL_TYPE from config, default value(" << modelType_
                << ") will be used as modelType_.";
    }

    for (uint32_t i = 0; i < classNum_; i++) {
        labelMap_.push_back(configData_.GetClassName(i));
    }
    return APP_ERR_OK;
}

APP_ERROR SemanticSegPostProcessBase::Init(const std::map<std::string, std::string> &postConfig)
{
    LogDebug << "Start to init SemanticSegPostProcessBase.";
    APP_ERROR ret = LoadConfigData(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "LoadConfigData failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = GetSemanticSegConfigData();
    if (ret != APP_ERR_OK) {
        LogError << "GetSemanticSegConfigData failed." << GetErrorInfo(ret);
    }

    LogDebug << "End to init SemanticSegPostProcessBase.";
    return APP_ERR_OK;
}

APP_ERROR SemanticSegPostProcessBase::DeInit()
{
    return APP_ERR_OK;
}

void SemanticSegPostProcessBase::CoordinatesReduction(const ResizedImageInfo& resizedImageInfo,
                                                      SemanticSegInfo& semanticSegInfo)
{
    // Use Nearest interpolation to resize to original image size.
    uint32_t imgHeight = resizedImageInfo.heightOriginal;
    uint32_t imgWidth = resizedImageInfo.widthOriginal;
    uint32_t resizedHeight = resizedImageInfo.heightResize;
    uint32_t resizedWidth = resizedImageInfo.widthResize;

    if ((resizedHeight == imgHeight) && (resizedWidth == imgWidth)) {
        LogDebug << "Same width and height, no need to resize back.";
        return;
    }
    if (imgHeight > MAX_IMAGE_EDGE || imgWidth > MAX_IMAGE_EDGE) {
        LogError << "ImgHeight or imageWidth is out of range." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    if (resizedHeight > MAX_IMAGE_EDGE || resizedWidth > MAX_IMAGE_EDGE) {
        LogError << "ResizedHeight or resizedWidth is out of range." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    if ((int)resizedImageInfo.keepAspectRatioScaling < 0 || (int)resizedImageInfo.keepAspectRatioScaling > MAX_RATIO) {
        LogError << "KeepAspectRatioScaling is out of range." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    if (IsDenominatorZero(imgHeight) || IsDenominatorZero(imgWidth)) {
        LogError << "The value of imgHeight or imgWidth must not equal to 0!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM) ;
        return;
    }
    if (resizedImageInfo.resizeType == RESIZER_TF_KEEP_ASPECT_RATIO ||
        resizedImageInfo.resizeType == RESIZER_MS_KEEP_ASPECT_RATIO) {
        float keepAspectRatioScaling = resizedImageInfo.keepAspectRatioScaling;
        resizedHeight = (uint32_t)(imgHeight * keepAspectRatioScaling);
        resizedWidth = (uint32_t)(imgWidth * keepAspectRatioScaling);
    }
    std::vector<std::vector<int>> resultPixels(imgHeight, std::vector<int>(imgWidth));
    for (uint32_t y = 0; y < imgHeight; y++) {
        for (uint32_t x = 0; x < imgWidth; x++) {
            auto i = y * resizedHeight / imgHeight;
            auto j = x * resizedWidth / imgWidth;
            if (i < semanticSegInfo.pixels.size() && j < semanticSegInfo.pixels[0].size()) {
                resultPixels[y][x] = semanticSegInfo.pixels[i][j];
            }
        }
    }
    semanticSegInfo.pixels = resultPixels;
}

APP_ERROR SemanticSegPostProcessBase::Process(const std::vector<TensorBase>&,
                                              std::vector<SemanticSegInfo>&,
                                              const std::vector<ResizedImageInfo>&,
                                              const std::map<std::string, std::shared_ptr<void>> &)
{
    LogError << "Process() has not been overridden in subclass!" << GetErrorInfo(APP_ERR_COMM_UNREALIZED);
    return APP_ERR_COMM_UNREALIZED;
}
}
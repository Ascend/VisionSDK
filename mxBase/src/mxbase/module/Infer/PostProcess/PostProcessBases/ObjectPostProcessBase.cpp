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
 * Description: Target detection post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/PostProcessBases/ObjectPostProcessBase.h"
#include "MxBase/ConfigUtil/ConfigUtil.h"
#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/Utils/StringUtils.h"

namespace MxBase {
ObjectPostProcessBase& ObjectPostProcessBase::operator=(const ObjectPostProcessBase &other)
{
    if (this == &other) {
        return *this;
    }
    PostProcessBase::operator=(other);

    cropRoiBoxes_ = other.cropRoiBoxes_;
    separateScoreThresh_ = other.separateScoreThresh_;
    scoreThresh_ = other.scoreThresh_;
    classNum_ = other.classNum_;

    return *this;
}

APP_ERROR ObjectPostProcessBase::GetSeparateScoreThresh(std::string& strSeparateScoreThresh)
{
    if (classNum_ == 0) {
        LogError << "Failed to get classNum_ (" << classNum_ << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    separateScoreThresh_ = StringUtils::SplitAndCastToFloat(strSeparateScoreThresh, ',');
    if (separateScoreThresh_.size() != classNum_) {
        LogError << "ClassNum (" << classNum_
                 << ") is not equal to total number" <<separateScoreThresh_.size()
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR ObjectPostProcessBase::GetObjectConfigData()
{
    APP_ERROR ret = configData_.GetFileValue<uint32_t>("CLASS_NUM", classNum_, (uint32_t)0x0, (uint32_t)0x3e8);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to read CLASS_NUM from config." << GetErrorInfo(ret);
        return ret;
    }
    ret = configData_.GetFileValue<float>("SCORE_THRESH", scoreThresh_, 0.0f, 1.0f);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read SCORE_THRESH from config, default value(0) will be used as scoreThresh_.";
    }

    std::string str;
    ret = configData_.GetFileValue<std::string>("SEPARATE_SCORE_THRESH", str);
    if (ret != APP_ERR_OK) {
        LogInfo << GetErrorInfo(ret) << "Fail to read SEPARATE_SCORE_THRESH from config, default value(" << scoreThresh_
                << ") will be used as separateScoreThresh_.";
        for (uint32_t i = 0; i < classNum_; i++) {
            separateScoreThresh_.push_back(scoreThresh_);
        }
    } else {
        ret = GetSeparateScoreThresh(str);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to get separateScoreThresh_." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR ObjectPostProcessBase::Init(const std::map<std::string, std::string> &postConfig)
{
    LogDebug << "Start to init ObjectPostProcessBase.";
    APP_ERROR ret = LoadConfigData(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "LoadConfigData failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = GetObjectConfigData();
    if (ret != APP_ERR_OK) {
        LogError << "GetObjectConfigData failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR ObjectPostProcessBase::DeInit()
{
    return APP_ERR_OK;
}

void ObjectPostProcessBase::ResizeReduction(const ResizedImageInfo& resizedImageInfo, const int imgWidth,
    const int imgHeight, std::vector<ObjectInfo>& objInfos)
{
    if (resizedImageInfo.resizeType == RESIZER_STRETCHING || resizedImageInfo.resizeType == RESIZER_MS_YOLOV4) {
        for (auto& objInfo : objInfos) {
            objInfo.x0 *= imgWidth;
            objInfo.x1 *= imgWidth;
            objInfo.y0 *= imgHeight;
            objInfo.y1 *= imgHeight;
        }
    } else if (JudgeResizeType(resizedImageInfo)) {
        float ratio = resizedImageInfo.keepAspectRatioScaling;
        if (IsDenominatorZero(ratio)) {
            LogError << "Ratio (denominator) must not be equal to 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        for (auto objInfo = objInfos.begin(); objInfo != objInfos.end();) {
            objInfo->x0 *= resizedImageInfo.widthResize / ratio;
            objInfo->y0 *= resizedImageInfo.heightResize / ratio;
            objInfo->x1 *= resizedImageInfo.widthResize / ratio;
            objInfo->y1 *= resizedImageInfo.heightResize / ratio;

            if (objInfo->x0 > imgWidth || objInfo->y0 > imgHeight) {
                objInfo = objInfos.erase(objInfo);
                continue;
            }
            if (objInfo->x1 > imgWidth) {
                objInfo->x1 = imgWidth;
            }
            if (objInfo->y1 > imgHeight) {
                objInfo->y1 = imgHeight;
            }
            ++objInfo;
        }
    } else {
        LogWarn << "Unknown resize type is used. Therefor ResizeReduction will not be implemented.";
    }
}

void ObjectPostProcessBase::CoordinatesReduction(const uint32_t index, const ResizedImageInfo& resizedImageInfo,
    std::vector<ObjectInfo>& objInfos, bool normalizedFlag)
{
    int imgWidth = resizedImageInfo.widthOriginal;
    int imgHeight = resizedImageInfo.heightOriginal;
    if (index < cropRoiBoxes_.size()) {
        imgWidth = (int)(cropRoiBoxes_[index].x1 - cropRoiBoxes_[index].x0);
        imgHeight = (int)(cropRoiBoxes_[index].y1 - cropRoiBoxes_[index].y0);
    }

    if (!normalizedFlag && resizedImageInfo.widthResize > 0 && resizedImageInfo.heightResize > 0) {
        for (auto& objInfo : objInfos) {
            objInfo.x0 /= resizedImageInfo.widthResize;
            objInfo.x1 /= resizedImageInfo.widthResize;
            objInfo.y0 /= resizedImageInfo.heightResize;
            objInfo.y1 /= resizedImageInfo.heightResize;
        }
    }
    ObjectPostProcessBase::ResizeReduction(resizedImageInfo, imgWidth, imgHeight, objInfos);

    if (index < cropRoiBoxes_.size()) {
        for (auto& objInfo : objInfos) {
            objInfo.x0 += cropRoiBoxes_[index].x0;
            objInfo.x1 += cropRoiBoxes_[index].x0;
            objInfo.y0 += cropRoiBoxes_[index].y0;
            objInfo.y1 += cropRoiBoxes_[index].y0;
        }
    }
}

void ObjectPostProcessBase::CoordinatesReduction(std::vector<std::vector<ObjectInfo>> &objInfos,
                                                 const std::vector<MxBase::ImagePreProcessInfo> &imagePreProcessInfos,
                                                 bool normalizedFlag)
{
    for (size_t i = 0; i < imagePreProcessInfos.size(); i++) {
        // multiply image size if normalized.
        auto info = imagePreProcessInfos[i];
        if (IsDenominatorZero(info.xRatio) || IsDenominatorZero(info.yRatio)) {
            LogError << "The value of info.xRatio or info.yRatio must not be equal to 0."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            continue;
        }
        if (normalizedFlag) {
            for (auto& objInfo : objInfos[i]) {
                objInfo.x0 *= info.imageWidth;
                objInfo.x1 *= info.imageWidth;
                objInfo.y0 *= info.imageHeight;
                objInfo.y1 *= info.imageHeight;
            }
        }

        // filter outside box and crop partly-outside box.
        for (auto objInfo = objInfos[i].begin(); objInfo != objInfos[i].end();) {
            if (objInfo->x0 > info.x1Valid || objInfo->y0 > info.y1Valid ||
                objInfo->x1 < info.x0Valid || objInfo->y1 < info.y0Valid) {
                LogDebug << "Filtered objectBox [(" << objInfo->x0 << "," << objInfo->x1 << "), (" << objInfo->y0 << ","
                         << objInfo->y1 << ")] is outside valid region:" << "ObjectBox [(" << info.x0Valid << ","
                         << info.y0Valid << "), (" << info.x0Valid << "," << info.y1Valid << ")]";
                objInfo = objInfos[i].erase(objInfo);
                continue;
            }
            objInfo->x0 = std::max(objInfo->x0, info.x0Valid);
            objInfo->x1 = std::min(objInfo->x1, info.x1Valid);
            objInfo->y0 = std::max(objInfo->y0, info.y0Valid);
            objInfo->y1 = std::min(objInfo->y1, info.y1Valid);
            ++objInfo;
        }

        // map back to original image.
        for (auto objInfo = objInfos[i].begin(); objInfo != objInfos[i].end();) {
            objInfo->x0 = (objInfo->x0 - info.xBias) / info.xRatio;
            objInfo->x1 = (objInfo->x1 - info.xBias) / info.xRatio;
            objInfo->y0 = (objInfo->y0 - info.yBias) / info.yRatio;
            objInfo->y1 = (objInfo->y1 - info.yBias) / info.yRatio;
            ++objInfo;
        }
    }
}

void ObjectPostProcessBase::LogObjectInfos(const std::vector<std::vector<ObjectInfo>>& objectInfos)
{
    for (size_t i = 0; i < objectInfos.size(); i++) {
        LogDebug << "Objects in Image No." << i << " are listed as followed.";
        for (auto &objInfo : objectInfos[i]) {
            auto foo = (separateScoreThresh_.size() > objInfo.classId) ?
                       separateScoreThresh_[(int)objInfo.classId] : scoreThresh_;
            LogDebug << "Find object: classId(" << objInfo.classId << "), confidence(" << objInfo.confidence <<
                     "), preset scoreThresh(" << foo <<
                     "), Coordinates (x0,y0)=(" << objInfo.x0 << ", " << objInfo.y0 << "); (x1, y1)=(" << objInfo.x1
                     << ", " << objInfo.y1 << ").";
        }
    }
}

APP_ERROR ObjectPostProcessBase::Process(const std::vector<TensorBase>&,
    std::vector<std::vector<ObjectInfo>>&,
    const std::vector<ResizedImageInfo>&,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogError << "Process() has not been overridden in subclass!" << GetErrorInfo(APP_ERR_COMM_UNREALIZED);
    return APP_ERR_COMM_UNREALIZED;
}
}
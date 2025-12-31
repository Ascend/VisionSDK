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
 * Description: Inference post-processing of target detection.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/ModelPostProcessors/ModelPostProcessorBase/ObjectPostProcessorBase.h"
#include "MxBase/GlobalManager/GlobalManager.h"

namespace MxBase {
const int MAX_IMAGE_EDGE = 8192;
APP_ERROR ObjectPostProcessorBase::GetSeparateScoreThresh(std::string& strSeparateScoreThresh)
{
    if (classNum_ <= 0) {
        LogError << "Failed to get classNum_ (" << classNum_ << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    separateScoreThresh_.clear();
    int index = 0;
    float value = 0;
    size_t num = strSeparateScoreThresh.find(",");
    while (num != std::string::npos && index < classNum_) {
        if (!(std::stringstream(strSeparateScoreThresh.substr(0, num)) >> value)) {
            LogError << "Failed to get SeparateScoreThresh."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            separateScoreThresh_.clear();
            return APP_ERR_COMM_INVALID_PARAM;
        }
        num++;
        strSeparateScoreThresh = strSeparateScoreThresh.substr(num, strSeparateScoreThresh.size());
        separateScoreThresh_.push_back(value);
        index++;
        num = strSeparateScoreThresh.find(",");
    }
    if (index != classNum_ - 1 || strSeparateScoreThresh.size() == 0) {
        LogError << "classNum_ (" << classNum_ << ") is not equal to total number of SeparateScoreThresh."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        separateScoreThresh_.clear();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!(std::stringstream(strSeparateScoreThresh) >> value)) {
        LogError << "Failed to get SeparateScoreThresh."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        separateScoreThresh_.clear();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    separateScoreThresh_.push_back(value);
    return APP_ERR_OK;
}

APP_ERROR ObjectPostProcessorBase::Init(const std::string& configPath, const std::string& labelPath,
    MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize ObjectPostProcessor. ";
    APP_ERROR ret = LoadConfigDataAndLabelMap(configPath, labelPath);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to LoadConfigDataAndLabelMap in ObjectPostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    ret = configData_.GetFileValue<int>("CLASS_NUM", classNum_, 0x0, 0x3e8);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read CLASS_NUM from config, default value(" << DEFAULT_CLASS_NUM
                << ") will be used as classNum_.";
    }
    ret = configData_.GetFileValue<float>("SCORE_THRESH", scoreThresh_, 0.0f, 1.0f);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read SCORE_THRESH from config, default value(" << DEFAULT_SCORE_THRESH
                << ") will be used as scoreThresh_.";
    }

    std::string str;
    ret = configData_.GetFileValue<std::string>("SEPARATE_SCORE_THRESH", str);
    if (ret != APP_ERR_OK) {
        LogInfo << GetErrorInfo(ret) << "Fail to read SEPARATE_SCORE_THRESH from config, default value(" << scoreThresh_
                << ") will be used as separateScoreThresh_.";
        for (int i = 0; i < classNum_; i++) {
            separateScoreThresh_.push_back(scoreThresh_);
        }
    } else {
        ret = GetSeparateScoreThresh(str);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to get separateScoreThresh_, configPath." << GetErrorInfo(ret);
            return ret;
        }
    }

    ret = GetModelTensorsShape(modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to get model tensors shape" << GetErrorInfo(ret);
        return ret;
    }

    LogInfo << "End to initialize ObjectPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR ObjectPostProcessorBase::DeInit()
{
    LogInfo << "Begin to deinitialize ObjectPostProcessor.";
    LogInfo << "End to deinitialize ObjectPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR CheckObjectInfoValid(ObjDetectInfo& objInfo)
{
    if ((int)objInfo.x0 > MAX_IMAGE_EDGE || (int)objInfo.x1 > MAX_IMAGE_EDGE) {
        LogError << "Invalid objInfo coordinates" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((int)objInfo.y0 > MAX_IMAGE_EDGE || (int)objInfo.y1 > MAX_IMAGE_EDGE) {
        LogError << "Invalid objInfo coordinates" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((int)objInfo.x0 < 0 || (int)objInfo.x1 < 0) {
        LogError << "Invalid objInfo coordinates" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((int)objInfo.y0 < 0 || (int)objInfo.y1 < 0) {
        LogError << "Invalid objInfo coordinates" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (objInfo.classId < 0) {
        LogError << "Invalid objInfo classId." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

void ObjectPostProcessorBase::CoordinatesReduction(ImageInfo& imgInfo, std::vector<ObjDetectInfo>& objInfos)
{
    if (postProcessorImageInfo_.resizeType == AspectRatioPostImageInfo::RESIZER_STRETCHING ||
        postProcessorImageInfo_.resizeType == AspectRatioPostImageInfo::RESIZER_MS_YOLOV4) {
        for (auto& objInfo : objInfos) {
            objInfo.x0 *= imgInfo.imgWidth;
            objInfo.x1 *= imgInfo.imgWidth;
            objInfo.y0 *= imgInfo.imgHeight;
            objInfo.y1 *= imgInfo.imgHeight;
        }
    } else if (postProcessorImageInfo_.resizeType == AspectRatioPostImageInfo::RESIZER_TF_KEEP_ASPECT_RATIO ||
        postProcessorImageInfo_.resizeType == AspectRatioPostImageInfo::RESIZER_MS_KEEP_ASPECT_RATIO) {
        float ratio = postProcessorImageInfo_.keepAspectRatioScaling;
        if (IsDenominatorZero(ratio)) {
            LogError << "The value of ratio must not equal to 0!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        for (auto objInfo = objInfos.begin(); objInfo != objInfos.end();) {
            objInfo->x0 *= imgInfo.modelWidth / ratio;
            objInfo->y0 *= imgInfo.modelWidth / ratio;
            objInfo->x1 *= imgInfo.modelHeight / ratio;
            objInfo->y1 *= imgInfo.modelHeight / ratio;

            if (objInfo->x0 > imgInfo.imgWidth || objInfo->y0 > imgInfo.imgHeight) {
                objInfo = objInfos.erase(objInfo);
                continue;
            }
            if (objInfo->x1 > imgInfo.imgWidth) {
                objInfo->x1 = imgInfo.imgWidth;
            }
            if (objInfo->y1 > imgInfo.imgHeight) {
                objInfo->y1 = imgInfo.imgHeight;
            }
            ++objInfo;
        }
    }
}

APP_ERROR CheckImageParamsValid(std::vector<ObjDetectInfo> &objInfos, MxBase::PostImageInfo &postImageInfo)
{
    if ((int)postImageInfo.widthResize > MAX_IMAGE_EDGE || (int)postImageInfo.heightResize > MAX_IMAGE_EDGE) {
        LogError << "Invalid postImageInfo resize width or height." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((int)postImageInfo.widthOriginal > MAX_IMAGE_EDGE || (int)postImageInfo.heightOriginal > MAX_IMAGE_EDGE) {
        LogError << "Invalid postImageInfo original width or height." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((int)postImageInfo.x0 > MAX_IMAGE_EDGE || (int)postImageInfo.x1 > MAX_IMAGE_EDGE) {
        LogError << "Invalid postImageInfo x coordinates" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((int)postImageInfo.y0 > MAX_IMAGE_EDGE || (int)postImageInfo.y1 > MAX_IMAGE_EDGE) {
        LogError << "Invalid postImageInfo y coordinates." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((int)postImageInfo.x0 < 0 || (int)postImageInfo.x1 < 0) {
        LogError << "Invalid postImageInfo x coordinates." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((int)postImageInfo.y0 < 0 || (int)postImageInfo.y1 < 0) {
        LogError << "Invalid postImageInfo y coordinates." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (auto &objInfo : objInfos) {
        if (CheckObjectInfoValid(objInfo) != APP_ERR_OK) {
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR ObjectPostProcessorBase::Process(std::vector<std::shared_ptr<void>>& featLayerData,
    std::vector<ObjDetectInfo>& objInfos, const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo)
{
    if (CheckImageParamsValid(objInfos, postImageInfo) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ImageInfo imgInfo;
    int xOffset = 0;
    int yOffset = 0;
    imgInfo.modelWidth = (int)postImageInfo.widthResize;
    imgInfo.modelHeight = (int)postImageInfo.heightResize;
    if (useMpPictureCrop) {
        imgInfo.imgWidth = (int)(postImageInfo.x1 - postImageInfo.x0);
        imgInfo.imgHeight = (int)(postImageInfo.y1 - postImageInfo.y0);
        xOffset = (int)postImageInfo.x0;
        yOffset = (int)postImageInfo.y0;
    } else {
        imgInfo.imgWidth = (int)postImageInfo.widthOriginal;
        imgInfo.imgHeight = (int)postImageInfo.heightOriginal;
    }
    ObjectDetectionOutput(featLayerData, objInfos, imgInfo);

    // coordinates reduction according to resize type.
    CoordinatesReduction(imgInfo, objInfos);

    // coordinates reduction according if the picture has been cropped.
    if (useMpPictureCrop) {
        for (auto& objInfo : objInfos) {
            objInfo.x0 += xOffset;
            objInfo.x1 += xOffset;
            objInfo.y0 += yOffset;
            objInfo.y1 += yOffset;
        }
    }

    // Log the final bboxes.
    for (auto& objInfo : objInfos) {
        auto foo = (separateScoreThresh_.size() > (size_t)objInfo.classId) ?
                separateScoreThresh_[objInfo.classId] : scoreThresh_;
        LogDebug << "Find object: classId(" << objInfo.classId << "), confidence(" << objInfo.confidence <<
                 "), preset scoreThresh(" << foo <<
                 "), Coordinates (x0,y0)=(" << objInfo.x0 << ", " << objInfo.y0 << "); (x1, y1)=(" << objInfo.x1 << ", "
                 << objInfo.y1 << ").";
    }
    return APP_ERR_OK;
}

void ObjectPostProcessorBase::SetAspectRatioImageInfo(const MxBase::AspectRatioPostImageInfo& postProcessorImageInfo)
{
    postProcessorImageInfo_ = postProcessorImageInfo;
}

void ObjectPostProcessorBase::ObjectDetectionOutput(std::vector<std::shared_ptr<void>>&,
                                                    std::vector<ObjDetectInfo>&, ImageInfo&)
{
}

void ObjectPostProcessorBase::NmsSort(std::vector<ObjDetectInfo>& objInfos, float iouThresh, IOUMethod method)
{
    std::vector<MxBase::DetectBox> detBoxes;
    for (auto& objInfo : objInfos) {
        if (CheckObjectInfoValid(objInfo) != APP_ERR_OK) {
            return;
        }
        MxBase::DetectBox det;
        det.classID = objInfo.classId;
        det.prob = objInfo.confidence;
        det.x = (objInfo.x0 + objInfo.x1) / COORDINATE_PARAM;
        det.y = (objInfo.y0 + objInfo.y1) / COORDINATE_PARAM;
        det.width = (objInfo.x1 - objInfo.x0 > std::numeric_limits<float>::epsilon()) ?
                (objInfo.x1 - objInfo.x0) : (objInfo.x0 - objInfo.x1);
        det.height = (objInfo.y1 - objInfo.y0 > std::numeric_limits<float>::epsilon()) ?
                (objInfo.y1 - objInfo.y0) : (objInfo.y0 - objInfo.y1);
        detBoxes.push_back(det);
    }
    MxBase::NmsSort(detBoxes, iouThresh, method);
    objInfos.clear();
    for (auto& detBox : detBoxes) {
        ObjDetectInfo objInfo;
        objInfo.classId = detBox.classID;
        objInfo.confidence = detBox.prob;
        objInfo.x0 = (detBox.x - detBox.width / COORDINATE_PARAM > std::numeric_limits<float>::epsilon()) ?
                     (float)(detBox.x - detBox.width / COORDINATE_PARAM) : 0;
        objInfo.y0 = (detBox.y - detBox.height / COORDINATE_PARAM > std::numeric_limits<float>::epsilon()) ?
                     (float)(detBox.y - detBox.height / COORDINATE_PARAM) : 0;
        objInfo.x1 = ((detBox.x + detBox.width / COORDINATE_PARAM) <= 1) ?
                     (float)(detBox.x + detBox.width / COORDINATE_PARAM) : 1;
        objInfo.y1 = ((detBox.y + detBox.height / COORDINATE_PARAM) <= 1) ?
                     (float)(detBox.y + detBox.height / COORDINATE_PARAM) : 1;
        objInfos.push_back(objInfo);
    }
}
}
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
 * Description: Used for post-processing of Ssdvgg models.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/ModelPostProcessors/ObjectPostProcessors/SsdvggPostProcessor.h"

namespace {
const int MIN_TENSOR_LENGTH = 2;
const int MIN_INFO_POOLSIZE = 3;
}

namespace {
const int CLASSID  = 1;
const int CONFIDENCE  = 2;
const int LEFTTOPX  = 3;
const int LEFTTOPY  = 4;
const int RIGHTBOTX = 5;
const int RIGHTBOTY = 6;
const int INFONUM = 8;
}

namespace MxBase {
APP_ERROR SsdvggPostProcessor::Init(const std::string& configPath, const std::string& labelPath,
    MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize SsdvggPostProcessor.";
    APP_ERROR ret = ObjectPostProcessorBase::Init(configPath, labelPath, modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in ObjectPostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    configData_.GetFileValueWarn<uint32_t>("OBJECT_NUM_TENSOR", objectNumTensor_, 0x0, 0x3e8);
    configData_.GetFileValueWarn<uint32_t>("OBJECT_INFO_TENSOR", objectInfoTensor_, 0x0, 0x3e8);

    if (checkModelFlag_) {
        ret = CheckModelCompatibility();
        if (ret != APP_ERR_OK) {
            LogError << "Fail to CheckModelCompatibility in SsdvggPostProcessor."
                     << "Please check the compatibility between model and postprocessor." << GetErrorInfo(ret);
            return ret;
        }
    } else {
        LogWarn << "Compatibility check for model is skipped as CHECK_MODEL is set as false, please ensure your model "
                << "is correct before running.";
    }
    LogInfo << "End to initialize SsdvggPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR SsdvggPostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize SsdvggPostProcessor.";
    LogInfo << "End to deinitialize SsdvggPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR SsdvggPostProcessor::Process(std::vector<std::shared_ptr<void>>& featLayerData,
    std::vector<ObjDetectInfo>& objInfos, const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo)
{
    LogDebug << "Begin to process SsdvggPostProcessor.";
    ObjectPostProcessorBase::Process(featLayerData, objInfos, useMpPictureCrop, postImageInfo);
    LogDebug << "End to process SsdvggPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR SsdvggPostProcessor::CheckModelCompatibility()
{
    if (outputTensorShapes_.size() < MIN_TENSOR_LENGTH) {
        LogError << "The outputTensorShapes_.size() < 2." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[0][0] != (int)modelDesc_.batchSizes.back()) {
        LogError << "The outputTensorShapes_[0][0] != modelDesc_.batchSizes.back()."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (objectInfoTensor_ >= MIN_TENSOR_LENGTH) {
        LogError << "TENSOR(" << objectInfoTensor_ << ") must be less than tensors'size(" << MIN_TENSOR_LENGTH
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[objectInfoTensor_].size() != MIN_INFO_POOLSIZE) {
        LogError << "The outputTensorShapes_[" << objectInfoTensor_ << "].size() != " << MIN_INFO_POOLSIZE << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[objectInfoTensor_][0x2] != INFONUM) {
        LogError << "The outputTensorShapes_[" << objectInfoTensor_ << "][" << 0x2 << "] != " << INFONUM << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

/*
 * @description: Get the info of detected object from output and resize to original coordinates.
 * @param featLayerData  Vector of 4 output feature data.
 * @param objInfos  Address of output object infos.
 * @param imgInfo  Original size of images.
 * @return: void
 */
void SsdvggPostProcessor::ObjectDetectionOutput(std::vector<std::shared_ptr<void>>& featLayerData,
    std::vector<ObjDetectInfo>& objInfos, ImageInfo&)
{
    LogDebug << "SsdvggPostProcessor start to write results.";
    for (auto num : {objectNumTensor_, objectInfoTensor_}) {
        if (num >= featLayerData.size()) {
            LogError << "TENSOR(" << num << ") must be less than tensors'size(" << featLayerData.size()
                     << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
    }
    if (featLayerData[objectNumTensor_] == nullptr || featLayerData[objectInfoTensor_] == nullptr) {
        LogError << "ObjectDetectionOutput: featLayerData contains nullptr."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    auto *objectNum = static_cast<int *>(featLayerData[objectNumTensor_].get());
    float *objectInfo = static_cast<float *>(featLayerData[objectInfoTensor_].get());
    int64_t objectInfoSize = 1;
    for (size_t i = 0; i < outputTensorShapes_[objectInfoTensor_].size(); i++) {
        objectInfoSize *= outputTensorShapes_[objectInfoTensor_][i];
    }
    if (objectInfoSize <= ((int)*objectNum - 1) * INFONUM + RIGHTBOTY) {
        LogError << "Check object info size failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    for (int i = 0; i < (int)*objectNum; i++) {
        uint32_t classId = static_cast<uint32_t>(objectInfo[i * INFONUM + CLASSID]);
        float confidence = objectInfo[i * INFONUM + CONFIDENCE];
        if ((classId >= separateScoreThresh_.size()) || (confidence < separateScoreThresh_[classId])) {
            continue;
        }
        ObjDetectInfo objInfo;
        objInfo.classId = static_cast<int>(classId);
        objInfo.confidence = confidence;
        objInfo.x0  = objectInfo[i * INFONUM + LEFTTOPX];
        objInfo.y0  = objectInfo[i * INFONUM + LEFTTOPY];
        objInfo.x1 = objectInfo[i * INFONUM + RIGHTBOTX];
        objInfo.y1 = objectInfo[i * INFONUM + RIGHTBOTY];
        objInfos.push_back(objInfo);
    }
    LogDebug << "SsdvggPostProcessor write results successed.";
}
}
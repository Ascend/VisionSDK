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
 * Description: Used for post-processing of Ssdmobilenetfpn models.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/ModelPostProcessors/ObjectPostProcessors/SsdmobilenetfpnPostProcessor.h"

namespace {
const int BBOX_POOLSIZE = 3;
const int MIN_TENSOR_LENGTH = 4;
}

namespace {
const int LEFTTOPY  = 0;
const int LEFTTOPX  = 1;
const int RIGHTBOTY = 2;
const int RIGHTBOTX = 3;
}

namespace MxBase {
/*
 * @description Load the labels from the file
 * @param labelPath classification label path
 * @return APP_ERROR error code
 */
APP_ERROR SsdmobilenetfpnPostProcessor::Init(const std::string& configPath, const std::string& labelPath,
                                             MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize SsdmobilenetfpnPostProcessor.";
    APP_ERROR ret = ObjectPostProcessorBase::Init(configPath, labelPath, modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in ObjectPostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    configData_.GetFileValueWarn<uint32_t>("OBJECT_NUM_TENSOR", objectNumTensor_, 0x0, 0x3e8);
    configData_.GetFileValueWarn<uint32_t>("CONFIDENCE_TENSOR", confidenceTensor_, 0x0, 0x3e8);
    configData_.GetFileValueWarn<uint32_t>("BBOX_TENSOR", bboxTensor_, 0x0, 0x3e8);
    configData_.GetFileValueWarn<uint32_t>("CLASSID_TENSOR", classIdTensor_, 0x0, 0x3e8);

    if (checkModelFlag_) {
        ret = CheckModelCompatibility();
        if (ret != APP_ERR_OK) {
            LogError << "Fail to CheckModelCompatibility in SsdmobilenetfpnPostProcessor."
                     << "Please check the compatibility between model and postprocessor." << GetErrorInfo(ret);
            return ret;
        }
    } else {
        LogWarn << "Compatibility check for model is skipped as CHECK_MODEL is set as false, please ensure your model "
                << "is correct before running.";
    }
    LogInfo << "End to initialize SsdmobilenetfpnPostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Do nothing temporarily.
 * @return APP_ERROR error code.
 */
APP_ERROR SsdmobilenetfpnPostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize SsdmobilenetfpnPostProcessor.";
    LogInfo << "End to deialize SsdmobilenetfpnPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR SsdmobilenetfpnPostProcessor::Process(std::vector<std::shared_ptr<void>>& featLayerData,
    std::vector<ObjDetectInfo>& objInfos, const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo)
{
    LogDebug << "Begin to process SsdmobilenetfpnPostProcessor.";
    ObjectPostProcessorBase::Process(featLayerData, objInfos, useMpPictureCrop, postImageInfo);
    LogDebug << "End to process SsdmobilenetfpnPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR SsdmobilenetfpnPostProcessor::CheckModelCompatibility()
{
    if ((int)outputTensorShapes_.size() < MIN_TENSOR_LENGTH) {
        LogError << "The outputTensorShapes_.size() < " << MIN_TENSOR_LENGTH
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (bboxTensor_ >= MIN_TENSOR_LENGTH) {
        LogError << "TENSOR(" << bboxTensor_ << ") must be less than tensors'size(" << MIN_TENSOR_LENGTH
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (confidenceTensor_ >= MIN_TENSOR_LENGTH) {
        LogError << "TENSOR(" << confidenceTensor_ << ") must be less than tensors'size(" << MIN_TENSOR_LENGTH
                 << ") and not less than 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (classIdTensor_ >= MIN_TENSOR_LENGTH) {
        LogError << "TENSOR(" << classIdTensor_ << ") must be less than tensors'size(" << MIN_TENSOR_LENGTH
                 << ") and not less than 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[0][0] != (int)modelDesc_.batchSizes.back()) {
        LogError << "The outputTensorShapes_[0][0] != modelDesc_.batchSizes.back()."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[bboxTensor_].size() != BBOX_POOLSIZE) {
        LogError << "The outputTensorShapes_[" << bboxTensor_ << "].size() != " << BBOX_POOLSIZE << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[bboxTensor_][0x2] != BOX_DIM) {
        LogError << "The outputTensorShapes_[" << bboxTensor_ <<"][" << 0x2 << "] != " << BOX_DIM << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

bool SsdmobilenetfpnPostProcessor::CheckFeatLayerData(int64_t constrain)
{
    int64_t confidenceTensorSize = 1;
    for (size_t i = 0; i < outputTensorShapes_[confidenceTensor_].size(); i++) {
        confidenceTensorSize *= outputTensorShapes_[confidenceTensor_][i];
    }
    int64_t bboxTensorSize = 1;
    for (size_t i = 0; i < outputTensorShapes_[bboxTensor_].size(); i++) {
        bboxTensorSize *= outputTensorShapes_[bboxTensor_][i];
    }
    int64_t classIdTensorSize = 1;
    for (size_t i = 0; i < outputTensorShapes_[classIdTensor_].size(); i++) {
        classIdTensorSize *= outputTensorShapes_[classIdTensor_][i];
    }
    if (classIdTensorSize <= constrain - 1 || confidenceTensorSize <= constrain - 1 ||
        bboxTensorSize <= (constrain - 1) * BOX_DIM + RIGHTBOTX) {
        LogError << "Check model output data size failed. Constrain is " << constrain <<
                 GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

/*
 * @description: Get the info of detected object from output and resize to original coordinates.
 * @param: featLayerData  Vector of 4 output feature data.
 * @param: objInfos  Address of output object infos.
 * @param: imgInfo  Original size of images.
 * @return: void
 */
void SsdmobilenetfpnPostProcessor::ObjectDetectionOutput(std::vector<std::shared_ptr<void>>& featLayerData,
                                                         std::vector<ObjDetectInfo>& objInfos, ImageInfo&)
{
    for (auto num : {objectNumTensor_, confidenceTensor_, bboxTensor_, classIdTensor_}) {
        if (num >= featLayerData.size()) {
            LogError << "TENSOR(" << num << ") must be less than tensors'size(" << featLayerData.size()
                     << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        if (featLayerData[num] == nullptr) {
            LogError << "FeatLayerData contains nullptr." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
            return;
        }
    }
    LogDebug << "SsdmobilenetfpnPostProcessor start to write results.";
    float *detectedNumber = static_cast<float *>(featLayerData[objectNumTensor_].get());
    float *confidencePtr = static_cast<float *>(featLayerData[confidenceTensor_].get());
    float *bboxPtr = static_cast<float *>(featLayerData[bboxTensor_].get());
    float *classIdPtr = static_cast<float *>(featLayerData[classIdTensor_].get());
    auto constrain = static_cast<int64_t>(*detectedNumber - 1);
    if (!CheckFeatLayerData(constrain)) {
        return;
    }
    for (int i = 0; i < (int)*detectedNumber; i++) {
        uint32_t classId = static_cast<uint32_t>(classIdPtr[i]);
        if ((classId >= separateScoreThresh_.size()) || (confidencePtr[i] < separateScoreThresh_[classId])) {
            continue;
        }
        ObjDetectInfo objInfo;
        objInfo.classId = static_cast<int>(classId);
        objInfo.confidence = confidencePtr[i];
        objInfo.x0  = bboxPtr[i * BOX_DIM + LEFTTOPX];
        objInfo.y0  = bboxPtr[i * BOX_DIM + LEFTTOPY];
        objInfo.x1 = bboxPtr[i * BOX_DIM + RIGHTBOTX];
        objInfo.y1 = bboxPtr[i * BOX_DIM + RIGHTBOTY];

        objInfos.push_back(objInfo);
    }
    LogDebug << "SsdmobilenetfpnPostProcessor write results successed.";
}
}
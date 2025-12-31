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
 * Description: Used for FasterRcnn model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/ModelPostProcessors/ObjectPostProcessors/FasterRcnnPostProcessor.h"
#include "MxBase/GlobalManager/GlobalManager.h"

namespace {
const int BBOX_POOLSIZE = 4;
const int BBOX_POOLSIZE_UNCUT = 3;
const int MIN_TENSOR_LENGTH = 3;
const int FPN_MIN_TENSOR_LENGTH = 2;
const int MIN_CLASSID_VAL = 0;
const int MAX_CLASSID_VAL = 80;
}

namespace {
const int LEFTTOPY  = 0;
const int LEFTTOPX  = 1;
const int RIGHTBOTY = 2;
const int RIGHTBOTX = 3;

template<typename T>
void GetConfigDataValue(const MxBase::ConfigData &configData, const std::string& key, T& value)
{
    APP_ERROR ret = configData.GetFileValue<T>(key, value);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read " << key << "from config, default value(" << value << ") will be used.";
    }
}
}

namespace MxBase {
/*
 * @description Load the configs and labels from the file.
 * @param labelPath config path and label path.
 * @return APP_ERROR error code.
 */
APP_ERROR FasterRcnnPostProcessor::Init(const std::string &configPath, const std::string &labelPath,
                                        MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize FasterRcnnPostProcessor.";
    APP_ERROR ret = ObjectPostProcessorBase::Init(configPath, labelPath, modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in ObjectPostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    iouThresh_ = DEFAULT_IOU_THRESH;
    GetConfigDataValue<float>(configData_, "IOU_THRESH", iouThresh_);
    GetConfigDataValue<bool>(configData_, "NMS_FINISHED", nmsFinished_);
    GetConfigDataValue<bool>(configData_, "FPN_SWITCH", fpnSwitch_);
    classNum_ -= (int)!nmsFinished_;
    GetConfigDataValue<uint32_t>(configData_, "BBOX_TENSOR", bboxCutTensor_);
    GetConfigDataValue<uint32_t>(configData_, "CONFIDENCE_TENSOR", confidenceCutTensor_);
    GetConfigDataValue<uint32_t>(configData_, "OBJECT_NUM_TENSOR", objectNumTensor_);
    GetConfigDataValue<uint32_t>(configData_, "CONFIDENCE_UNCUT_TENSOR", confidenceTensor_);
    GetConfigDataValue<uint32_t>(configData_, "BBOX_UNCUT_TENSOR", bboxTensor_);
    GetConfigDataValue<uint32_t>(configData_, "CLASSID_UNCUT_TENSOR", classIdTensor_);
    ret = CheckModel();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    LogInfo << "End to initialize FasterRcnnPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR FasterRcnnPostProcessor::Process(std::vector<std::shared_ptr<void>>& featLayerData,
    std::vector<ObjDetectInfo>& objInfos, const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo)
{
    LogDebug << "Begin to process FasterRcnnPostProcessor.";
    APP_ERROR ret = ObjectPostProcessorBase::Process(featLayerData, objInfos, useMpPictureCrop, postImageInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superProcess in ObjectPostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "End to process FasterRcnnPostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Do nothing temporarily.
 * @return: APP_ERROR error code.
 */
APP_ERROR FasterRcnnPostProcessor::DeInit()
{
    LogInfo << "Begin to deinit FasterRcnnPostProcessor.";
    LogInfo << "End to deinit FasterRcnnPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR FasterRcnnPostProcessor::CheckModel()
{
    APP_ERROR ret = APP_ERR_OK;
    if (fpnSwitch_) {
        ret = CheckModelCompatibilityForFPN();
        if (ret != APP_ERR_OK) {
            LogError << "Fail to CheckModelCompatibility in FasterRcnnFpnPostProcessor."
                     << "Please check the compatibility between model and postprocessor." << GetErrorInfo(ret);
        }
    } else {
        if (checkModelFlag_) {
            ret = nmsFinished_ ? CheckNmsFinishedModelCompatibility() : CheckModelCompatibility();
            if (ret != APP_ERR_OK) {
                LogError << "Fail to CheckModelCompatibility in FasterRcnnPostProcessor."
                         << "Please check the compatibility between model and postprocessor." << GetErrorInfo(ret);
            }
        } else {
            LogWarn << "Compatibility check for model is skipped as CHECK_MODEL is set as false, "
                       "please ensure your model " << "is correct before running.";
        }
    }
    return ret;
}

APP_ERROR FasterRcnnPostProcessor::CheckModelCompatibility()
{
    if (outputTensorShapes_.size() < MIN_TENSOR_LENGTH) {
        LogError << "The outputTensorShapes_.size() < " << MIN_TENSOR_LENGTH
                 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (bboxCutTensor_ >= MIN_TENSOR_LENGTH) {
        LogError << "TENSOR(" << bboxCutTensor_ << ") must be less than tensors'size(" << MIN_TENSOR_LENGTH << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[bboxCutTensor_].size() != BBOX_POOLSIZE) {
        LogError << "outputTensorShapes_[" << bboxCutTensor_ << "].size() != " << BBOX_POOLSIZE
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[bboxCutTensor_][0x2] != classNum_) {
        LogError << "outputTensorShapes_[" << bboxCutTensor_ << "][" << 0x2 << "] != " << classNum_
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[bboxCutTensor_][0x3] != BOX_DIM) {
        LogError << "outputTensorShapes_[" << bboxCutTensor_ << "][" << 0x3 << "] != " << BOX_DIM
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR FasterRcnnPostProcessor::CheckModelCompatibilityForFPN()
{
    // check model output tensor
    if (outputTensorShapes_.size() != FPN_MIN_TENSOR_LENGTH) {
        LogWarn << "outputTensorShapes_.size() != " << FPN_MIN_TENSOR_LENGTH;
        return APP_ERR_COMM_FAILURE;
    }
    if (objectNumTensor_ >= FPN_MIN_TENSOR_LENGTH) {
        LogError << "TENSOR(" << objectNumTensor_
                 << ") must be less than tensors'size(" << FPN_MIN_TENSOR_LENGTH << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[objectNumTensor_].size() != FPN_MIN_TENSOR_LENGTH) {
        LogWarn << "outputTensorShapes_[" << objectNumTensor_ << "].size() != " << FPN_MIN_TENSOR_LENGTH;
        return APP_ERR_COMM_FAILURE;
    }
    if (outputTensorShapes_[objectNumTensor_][0x1] != FPN_BOX_DIM) {
        LogWarn << "outputTensorShapes_[" << objectNumTensor_ << "][" << 0x1 << "] != " << FPN_BOX_DIM;
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR FasterRcnnPostProcessor::CheckNmsFinishedModelCompatibility()
{
    if (outputTensorShapes_.size() <= MIN_TENSOR_LENGTH) {
        LogError << "outputTensorShapes_.size() <= " << MIN_TENSOR_LENGTH << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (bboxTensor_ >= MIN_TENSOR_LENGTH) {
        LogError << "TENSOR(" << bboxTensor_ << ") must be less than tensors'size(" << MIN_TENSOR_LENGTH
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[bboxTensor_].size() != BBOX_POOLSIZE_UNCUT) {
        LogError << "outputTensorShapes_[" << bboxTensor_ << "].size() != " << BBOX_POOLSIZE_UNCUT
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[bboxTensor_][0x2] != BOX_DIM) {
        LogError << "outputTensorShapes_[" << bboxTensor_ << "][" << 0x2 << "] != " << BOX_DIM
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

bool FasterRcnnPostProcessor::CheckNmsObjectFeatLayerData(int64_t constrain)
{
    auto outputTensorShapesSize = outputTensorShapes_.size();
    if (bboxCutTensor_ >= outputTensorShapesSize || confidenceCutTensor_ >= outputTensorShapesSize) {
        LogError << "Check outputTensorShape failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    int64_t bboxCutTensorSize = 1;
    for (size_t i = 0; i < outputTensorShapes_[bboxCutTensor_].size(); i++) {
        bboxCutTensorSize *= outputTensorShapes_[bboxCutTensor_][i];
    }
    int64_t confidenceCutTensorSize = 1;
    for (size_t i = 0; i < outputTensorShapes_[confidenceCutTensor_].size(); i++) {
        confidenceCutTensorSize *= outputTensorShapes_[confidenceCutTensor_][i];
    }
    if (confidenceCutTensorSize <= (constrain - 1) * classNum_ + classNum_ - 1 ||
        bboxCutTensorSize <= (constrain - 1) * BOX_DIM + (classNum_ - 1) * BOX_DIM + RIGHTBOTX) {
        LogError << "Check featLayerData failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

void FasterRcnnPostProcessor::NmsObjectDetectionOutput(std::vector<ObjDetectInfo>& objInfos,
    std::vector<std::shared_ptr<void>>& featLayerData, int stride, ImageInfo& imgInfo)
{
    if (IsDenominatorZero(imgInfo.modelWidth) || IsDenominatorZero(imgInfo.modelHeight)) {
        LogError << "The value of imgInfo.modelWidth or imgInfo.modelHeight must not equal to 0!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    for (auto num : {bboxCutTensor_, confidenceCutTensor_}) {
        if (num >= featLayerData.size()) {
            LogError << "TENSOR(" << num << ") must be less than tensors'size(" << featLayerData.size()
                     << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
    }
    float *bboxPtr = static_cast<float *>(featLayerData[bboxCutTensor_].get());
    float *confidencePtr = static_cast<float *>(featLayerData[confidenceCutTensor_].get());
    if (bboxPtr == nullptr || confidencePtr == nullptr) {
        LogError << "BboxPtr or confidencePtr is null." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    if (!CheckNmsObjectFeatLayerData(stride - 1)) {
        LogError << "Check featLayerData failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    for (int j = 0; j < stride; j++) {
        float maxProb = 0.0;
        int maxIndex = 0;
        for (int k = 0; k < classNum_; k++) {
            if (maxProb < confidencePtr[k + j * classNum_]) {
                maxProb = confidencePtr[k + j * classNum_];
                maxIndex = k;
            }
        }
        if (maxProb > separateScoreThresh_[maxIndex]) {
            ObjDetectInfo objInfo;
            objInfo.classId = maxIndex + 1;
            objInfo.confidence = maxProb;
            objInfo.x0 = bboxPtr[j * classNum_ * BOX_DIM + maxIndex * BOX_DIM + LEFTTOPX] / imgInfo.modelWidth;
            objInfo.y0 = bboxPtr[j * classNum_ * BOX_DIM + maxIndex * BOX_DIM + LEFTTOPY] / imgInfo.modelHeight;
            objInfo.x1 = bboxPtr[j * classNum_ * BOX_DIM + maxIndex * BOX_DIM + RIGHTBOTX] / imgInfo.modelWidth;
            objInfo.y1 = bboxPtr[j * classNum_ * BOX_DIM + maxIndex * BOX_DIM + RIGHTBOTY] / imgInfo.modelHeight;
            objInfos.push_back(objInfo);
        }
    }
    NmsSort(objInfos, iouThresh_);
}

/*
 * @description: Get the info of detected object from output and resize to original coordinates.
 * @param featLayerData  Vector of output feature data.
 * @param objInfos  Address of output object infos.
 * @param imgInfo  Info of model/image width and height.
 * @return: void
 */
void FasterRcnnPostProcessor::ObjectDetectionOutput(std::vector<std::shared_ptr<void>> &featLayerData,
    std::vector<ObjDetectInfo> &objInfos, ImageInfo &imgInfo)
{
    LogDebug << "FasterRcnnPostProcessor start to write results.";
    for (auto data : featLayerData) {
        if (data == nullptr) {
            LogError << "ObjectDetectionOutput: featLayerData contains nullptr."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
    }
    if (fpnSwitch_) {
        if (featLayerData.size() < 0x2) {
            LogError << "Invalid featLayerData." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        FPNObjectDetectionOutput(featLayerData, objInfos, imgInfo);
        return;
    }
    if (nmsFinished_) {
        if (featLayerData.size() < 0x4) {
            LogError << "Invalid featLayerData." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        NmsFinishedObjectDetectionOutput(featLayerData, objInfos, imgInfo);
        return;
    }
    if (featLayerData.size() < 0x4) {
        LogError << "Invalid featLayerData." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    if (objectNumTensor_ >= featLayerData.size()) {
        LogError << "TENSOR(" << objectNumTensor_ << ") must be less than tensors'size(" << FPN_MIN_TENSOR_LENGTH
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    float *stride = static_cast<float *>(featLayerData[objectNumTensor_].get());
    NmsObjectDetectionOutput(objInfos, featLayerData, (int) *stride, imgInfo);
    LogDebug << "FasterRcnnPostProcessor write results successed.";
}

bool FasterRcnnPostProcessor::CheckNmsFinishedFeatLayerData(int64_t constrain)
{
    auto outputTensorShapesSize = outputTensorShapes_.size();
    if (confidenceTensor_ >= outputTensorShapesSize || bboxTensor_ >= outputTensorShapesSize ||
        classIdTensor_ >= outputTensorShapesSize) {
        LogError << "Check outputTensorShape failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
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
    if (bboxTensorSize <= (constrain - 1) * BOX_DIM + RIGHTBOTX ||
        classIdTensorSize <= constrain- 1 || confidenceTensorSize <= constrain - 1) {
        LogError << "Check model output tensor data size failed. bboxTensorSize is " << bboxTensorSize
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

void FasterRcnnPostProcessor::NmsFinishedObjectDetectionOutput(std::vector<std::shared_ptr<void>> &featLayerData,
                                                               std::vector<ObjDetectInfo> &objInfos, ImageInfo &)
{
    LogDebug << "FasterRcnnPostProcessor has skipped NMS.";
    for (auto num : {objectNumTensor_, confidenceTensor_, bboxTensor_, classIdTensor_}) {
        if (num >= featLayerData.size()) {
            LogError << "TENSOR(" << num << ") must be less than tensors'size(" << featLayerData.size()
                     << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
    }
    float *detectedNumber = static_cast<float *>(featLayerData[objectNumTensor_].get());
    float *confidencePtr = static_cast<float *>(featLayerData[confidenceTensor_].get());
    float *bboxPtr = static_cast<float *>(featLayerData[bboxTensor_].get());
    float *classIdPtr = static_cast<float *>(featLayerData[classIdTensor_].get());
    if (detectedNumber == nullptr || confidencePtr == nullptr || bboxPtr == nullptr || classIdPtr == nullptr) {
        LogError << "FeatLayerData ptr check failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    if (!CheckNmsFinishedFeatLayerData(static_cast<int64_t>(*detectedNumber) - 1)) {
        LogError << "Check featlayerData failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    for (int i = 0; i < (int)(*detectedNumber); i++) {
        uint32_t classId = static_cast<uint32_t>(classIdPtr[i]);
        if ((classId >= separateScoreThresh_.size()) || (confidencePtr[i] < separateScoreThresh_[classId]) ||
            (classIdPtr[i] <= 0)) {
            continue;
        }
        ObjDetectInfo objInfo;
        objInfo.classId = static_cast<int>(classId);
        objInfo.confidence = confidencePtr[i];
        objInfo.x0 = bboxPtr[i * BOX_DIM + LEFTTOPX];
        objInfo.y0 = bboxPtr[i * BOX_DIM + LEFTTOPY];
        objInfo.x1 = bboxPtr[i * BOX_DIM + RIGHTBOTX];
        objInfo.y1 = bboxPtr[i * BOX_DIM + RIGHTBOTY];

        objInfos.push_back(objInfo);
    }
    LogDebug << "FasterRcnnPostProcessor write results successed.";
}

bool FasterRcnnPostProcessor::CheckFPNFeatLayerData(int64_t constrain)
{
    auto outputTensorShapesSize = outputTensorShapes_.size();
    if (objectNumTensor_ >= outputTensorShapesSize || bboxCutTensor_ >= outputTensorShapesSize) {
        LogError << "Check outputTensorShapes size failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    int64_t objectNumTensorSize = 1;
    for (size_t i = 0; i < outputTensorShapes_[objectNumTensor_].size(); i++) {
        objectNumTensorSize *= outputTensorShapes_[objectNumTensor_][i];
    }
    int64_t bboxCutTensorSize = 1;
    for (size_t i = 0; i < outputTensorShapes_[bboxCutTensor_].size(); i++) {
        bboxCutTensorSize *= outputTensorShapes_[bboxCutTensor_][i];
    }
    if (objectNumTensorSize <= (constrain - 1) * FPN_BOX_DIM + BBOX_POOLSIZE ||
        bboxCutTensorSize <= constrain - 1) {
        LogError << "Check model output tensor data size failed. objectNumTensorSize is " << objectNumTensorSize
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

/*
 * @description: Get the info of detected object from output and resize to original coordinates.
 * @param: featLayerData  Vector of four output feature data.
 * @param: objInfos  Address of output object infos.
 * @param: imgInfo  Original size of images.
 * @return: void
 */
void FasterRcnnPostProcessor::FPNObjectDetectionOutput(std::vector<std::shared_ptr<void>> &featLayerData,
                                                       std::vector<ObjDetectInfo> &objInfos, ImageInfo &imgInfo)
{
    LogDebug << "Start to enter fasterrcnn-fpn FasterRcnnPostProcessor.";
    for (auto num : {objectNumTensor_, bboxCutTensor_}) {
        if (num >= featLayerData.size()) {
            LogError << "TENSOR(" << num << ") must be less than tensors'size(" << featLayerData.size()
                     << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
    }
    int detectedNumber = outputTensorShapes_[objectNumTensor_][0x0];
    auto *bboxPtr = static_cast<float *>(featLayerData[objectNumTensor_].get());
    auto *classIdPtr = static_cast<int64_t *>(featLayerData[bboxCutTensor_].get());
    if (bboxPtr == nullptr || classIdPtr == nullptr) {
        LogError << "BboxPtr or classIdPtr is null." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    if (IsDenominatorZero(imgInfo.modelWidth) || IsDenominatorZero(imgInfo.modelHeight)) {
        LogError << "The value of imgInfo.modelWidth or imgInfo.modelHeight must not equal to 0!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    if (CheckFPNFeatLayerData(detectedNumber - 1)) {
        LogError << "Check featLayerData failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    for (int i = 0; i < detectedNumber; i++) {
        auto classId = static_cast<int64_t>(classIdPtr[i]);
        ObjDetectInfo objInfo {};
        objInfo.classId = classId;
        objInfo.x0 = bboxPtr[i * FPN_BOX_DIM + LEFTTOPY] / imgInfo.modelWidth;
        objInfo.y0 = bboxPtr[i * FPN_BOX_DIM + LEFTTOPX] / imgInfo.modelHeight;
        objInfo.x1 = bboxPtr[i * FPN_BOX_DIM + RIGHTBOTY] / imgInfo.modelWidth;
        objInfo.y1 = bboxPtr[i * FPN_BOX_DIM + RIGHTBOTX] / imgInfo.modelHeight;
        objInfo.confidence = bboxPtr[i * FPN_BOX_DIM + BBOX_POOLSIZE];
        if (objInfo.confidence < separateScoreThresh_[classId]) {
            continue;
        }
        if (classId < MIN_CLASSID_VAL || classId > MAX_CLASSID_VAL) {
            continue;
        }
        objInfos.push_back(objInfo);
    }
    LogDebug << "FasterRcnnPostProcessor write results successed.";
}
}
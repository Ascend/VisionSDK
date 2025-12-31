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
 * Description: SsdMobilenetFpn_Mindspore model post-processing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MINDXSDK_RETINANETPOSTPROCESS_DPTR_H
#define MINDXSDK_RETINANETPOSTPROCESS_DPTR_H
#include <algorithm>
#include <vector>
#include <map>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/CV/Core/DataType.h"
#include "MxBase/PostProcessBases/ObjectPostProcessBase.h"
#include "ObjectPostProcessors/RetinaNetPostProcess.h"
#include "MxBase/Log/Log.h"
#include "MxBase/CV/ObjectDetection/Nms/Nms.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "acl/acl.h"

namespace MxBase {
const int OUTPUT_BBOX_INDEX = 0;
const int OUTPUT_SCORE_INDEX = 1;
const int OUTPUT_CLASS_INDEX = 2;
const int OUTPUT_OBJ_NUM_INDEX = 3;

const int OUTPUT_BBOX_TENSOR_DIM = 3;
const int OUTPUT_SCORE_TENSOR_DIM = 2;
const int OUTPUT_CLASS_TENSOR_DIM = 2;
const int OUTPUT_OBJ_NUM_TENSOR_DIM = 1;

const int OUTPUT_TENSOR_SIZE = 4;
const int OUTPUT_NUM_INDEX = 1;
const int MAX_OUTPUT_NUM = 300;
const int MAX_DETECT_NUM = 100;

const int TF_BBOX_INDEX_LX = 0;
const int TF_BBOX_INDEX_LY = 1;
const int TF_BBOX_INDEX_RX = 2;
const int TF_BBOX_INDEX_RY = 3;
}

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER RetinaNetPostProcessDptr {
public:
    explicit RetinaNetPostProcessDptr(RetinaNetPostProcess *pRetinaNetPostProcess);
    RetinaNetPostProcessDptr(const RetinaNetPostProcessDptr &other);
    ~RetinaNetPostProcessDptr() = default;
    RetinaNetPostProcessDptr &operator = (const RetinaNetPostProcessDptr &other);

    APP_ERROR GetConfigValue();
    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);
    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;
    bool IsValidTensorsForTF(const std::vector<TensorBase> &tensors) const;
    APP_ERROR TensorflowOutput(const std::vector<TensorBase> &tensors,
                               std::vector<std::vector<ObjectInfo>> &objectInfos,
                               const std::vector<ResizedImageInfo> &resizedImageInfos);
    void TFHandleOneObject(uint32_t j, std::vector<ObjectInfo> &objInfos,
                           const ResizedImageInfo &resizedImageInfos, const uint32_t bboxLen);
    APP_ERROR TFHandleOneBatch(const std::vector<TensorBase> &tensors,
                               std::vector<std::vector<ObjectInfo>> &objectInfos,
                               const ResizedImageInfo &resizedImageInfos, uint32_t i);
    enum ModelType {
        TENSORFLOW = 0
    };
    int modelType_ = TENSORFLOW;
private:
    RetinaNetPostProcess *qPtr_ = nullptr;
    aclFloat16 *bboxPtr_ = nullptr;
    aclFloat16 *scorePtr_ = nullptr;
    aclFloat16 *labelPtr_ = nullptr;
    int *detectedNumberPtr_ = nullptr;
};

RetinaNetPostProcessDptr::RetinaNetPostProcessDptr(RetinaNetPostProcess *pRetinaNetPostProcess)
    : qPtr_(pRetinaNetPostProcess)
{}

RetinaNetPostProcessDptr::RetinaNetPostProcessDptr(const RetinaNetPostProcessDptr &other)
{
    *this = other;
}

RetinaNetPostProcessDptr &RetinaNetPostProcessDptr::operator = (const RetinaNetPostProcessDptr &other)
{
    if (this == &other) {
        return *this;
    }
    modelType_ = other.modelType_;
    qPtr_ = other.qPtr_;
    bboxPtr_ = other.bboxPtr_;
    scorePtr_ = other.scorePtr_;
    labelPtr_ = other.labelPtr_;
    detectedNumberPtr_ = other.detectedNumberPtr_;
    return *this;
}

APP_ERROR RetinaNetPostProcessDptr::GetConfigValue()
{
    APP_ERROR ret = qPtr_->configData_.GetFileValue<int>("MODEL_TYPE", modelType_, 0x0, 0x64);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) <<
            "Fail to read MODEL_TYPE from config, default Tensorflow(0) will be used as modelType_.";
        return ret;
    }

    LogInfo << "The config parameters of post process are as follows: \n"
            << "MODEL_TYPE:" << modelType_ << ",CLASS_NUM: " << qPtr_->classNum_ << " \n"
            << ",SCORE_THRESH: " << qPtr_->scoreThresh_ << " \n";
    return APP_ERR_OK;
}

APP_ERROR RetinaNetPostProcessDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "Input tensors are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return qPtr_->CheckAndMoveTensors(tensors);
}

bool RetinaNetPostProcessDptr::IsValidTensorsForTF(const std::vector<TensorBase> &tensors) const
{
    if (tensors.size() != OUTPUT_TENSOR_SIZE) {
        LogError << "Ouput tensors size (" << tensors.size() << ") "
                 << "is not equal to expected value (" << OUTPUT_TENSOR_SIZE << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto bboxShape = tensors[OUTPUT_BBOX_INDEX].GetShape();
    if (bboxShape.size() != OUTPUT_BBOX_TENSOR_DIM) {
        LogError << "Output tensor[" << OUTPUT_BBOX_INDEX << "] dimensions (" << bboxShape.size() << ") "
                 << "is not equal to expected value (" << OUTPUT_BBOX_TENSOR_DIM << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }

    auto scoreShape = tensors[OUTPUT_SCORE_INDEX].GetShape();
    if (scoreShape.size() != OUTPUT_SCORE_TENSOR_DIM) {
        LogError << "Output tensor[" << OUTPUT_SCORE_INDEX << "] dimensions (" << scoreShape.size() << ") "
                 << "is not equal to expected value (" << OUTPUT_SCORE_TENSOR_DIM << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }

    auto classShape = tensors[OUTPUT_CLASS_INDEX].GetShape();
    if (classShape.size() != OUTPUT_CLASS_TENSOR_DIM) {
        LogError << "Output tensor[" << OUTPUT_CLASS_INDEX << "] dimensions (" << classShape.size() << ") "
                 << "is not equal to expected value (" << OUTPUT_CLASS_TENSOR_DIM << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }

    if (bboxShape[OUTPUT_OBJ_NUM_TENSOR_DIM] != MAX_OUTPUT_NUM ||
        scoreShape[OUTPUT_OBJ_NUM_TENSOR_DIM] != MAX_OUTPUT_NUM ||
        classShape[OUTPUT_OBJ_NUM_TENSOR_DIM] != MAX_OUTPUT_NUM) {
        LogError << "Output tensor object num " << "is not equal to expected value (" << MAX_OUTPUT_NUM << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }

    auto objNumape = tensors[OUTPUT_OBJ_NUM_INDEX].GetShape();
    if (objNumape.size() != OUTPUT_OBJ_NUM_TENSOR_DIM) {
        LogError << "Output tensor[" << OUTPUT_OBJ_NUM_INDEX << "] dimensions (" << objNumape.size() << ") "
                 << "is not equal to expected value (" << OUTPUT_OBJ_NUM_TENSOR_DIM << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool RetinaNetPostProcessDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    switch (modelType_) {
        case static_cast<int>(ModelType::TENSORFLOW):
            if (!IsValidTensorsForTF(tensors)) {
                LogError << "Tensorflow model's output tensor is invalid!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return false;
            }
            break;
        default:
            LogError << "The modelType_(" << modelType_ << ") is not supported."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
    }
    return true;
}

void RetinaNetPostProcessDptr::TFHandleOneObject(uint32_t j, std::vector<ObjectInfo> &objInfos,
    const ResizedImageInfo &resizedImageInfo, const uint32_t bboxLen)
{
    ObjectInfo objInfo {};
    uint32_t classId = static_cast<uint32_t>(aclFloat16ToFloat(labelPtr_[j]));
    if (classId >= qPtr_->classNum_) {
        LogDebug << "classId"
                 << "(" << classId << ")"
                 << "is out of range (0," << qPtr_->classNum_ << ").";
        return;
    }
    objInfo.classId = classId;
    objInfo.className = qPtr_->configData_.GetClassName(static_cast<size_t>(objInfo.classId));
    objInfo.confidence = aclFloat16ToFloat(scorePtr_[j]);
    if (objInfo.confidence < qPtr_->separateScoreThresh_[classId]) {
        return;
    }
    if (IsDenominatorZero(resizedImageInfo.widthResize) || IsDenominatorZero(resizedImageInfo.heightResize)) {
        LogError << "The resizedImageInfo.widthResize is " << resizedImageInfo.widthResize
                 << ", resizedImageInfo.heightResize is " << resizedImageInfo.heightResize
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return;
    }
    float widthScale = (float)resizedImageInfo.widthOriginal / (float)resizedImageInfo.widthResize;
    float heightScale = (float)resizedImageInfo.heightOriginal / (float)resizedImageInfo.heightResize;
    if (j * OUTPUT_TENSOR_SIZE + TF_BBOX_INDEX_RY >= bboxLen) {
        return;
    }
    objInfo.x0 = aclFloat16ToFloat(bboxPtr_[j * OUTPUT_TENSOR_SIZE + TF_BBOX_INDEX_LX]) * widthScale;
    objInfo.y0 = aclFloat16ToFloat(bboxPtr_[j * OUTPUT_TENSOR_SIZE + TF_BBOX_INDEX_LY]) * heightScale;
    objInfo.x1 = aclFloat16ToFloat(bboxPtr_[j * OUTPUT_TENSOR_SIZE + TF_BBOX_INDEX_RX]) * widthScale;
    objInfo.y1 = aclFloat16ToFloat(bboxPtr_[j * OUTPUT_TENSOR_SIZE + TF_BBOX_INDEX_RY]) * heightScale;
    LogInfo << "(x0, y0, x1, y1, score, classId) : " << objInfo.x0 << ", " << objInfo.y0 << ", "
            << ", " << objInfo.x1 << ", " << objInfo.y1 << ", " << objInfo.confidence << "," << objInfo.classId;
    objInfos.push_back(objInfo);
}

APP_ERROR RetinaNetPostProcessDptr::TFHandleOneBatch(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<ObjectInfo>> &objectInfos, const ResizedImageInfo &resizedImageInfo, uint32_t i)
{
    std::vector<ObjectInfo> objInfos;
    detectedNumberPtr_ = (int *)qPtr_->GetBuffer(tensors[OUTPUT_OBJ_NUM_INDEX], i); // 1
    if (detectedNumberPtr_ == nullptr) {
        LogError << "The detectedNumberPtr is nullptr!" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    int detectedNumber = detectedNumberPtr_[0];
    if (detectedNumber > MAX_DETECT_NUM) {
        LogWarn << "Detected objects' number is more than 100, check ScoreTresh value.";
    }

    bboxPtr_ = (aclFloat16 *)qPtr_->GetBuffer(tensors[OUTPUT_BBOX_INDEX], i); // 1 * 300 * 4
    if (bboxPtr_ == nullptr) {
        LogError << "The bboxPtr is nullptr!" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    scorePtr_ = (aclFloat16 *)qPtr_->GetBuffer(tensors[OUTPUT_SCORE_INDEX], i); // 1 * 300
    if (scorePtr_ == nullptr) {
        LogError << "The scorePtr is nullptr!" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }

    labelPtr_ = (aclFloat16 *)qPtr_->GetBuffer(tensors[OUTPUT_CLASS_INDEX], i); // 1 * 300
    if (labelPtr_ == nullptr) {
        LogError << "The labelPtr is nullptr!" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    uint32_t batchSize = tensors[OUTPUT_BBOX_INDEX].GetShape()[0];
    uint32_t bboxLen = tensors[OUTPUT_BBOX_INDEX].GetSize() / batchSize;
    for (uint32_t j = 0; j < (uint32_t)detectedNumber && j < (uint32_t)MAX_DETECT_NUM; j++) {
        TFHandleOneObject(j, objInfos, resizedImageInfo, bboxLen);
    }
    objectInfos.push_back(objInfos);
    return APP_ERR_OK;
}

APP_ERROR RetinaNetPostProcessDptr::TensorflowOutput(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<ObjectInfo>> &objectInfos, const std::vector<ResizedImageInfo> &resizedImageInfos)
{
    LogInfo << "Begin to handle TensorFlow-RetinaNet Output.";
    APP_ERROR ret = APP_ERR_OK;
    auto shape = tensors[OUTPUT_BBOX_INDEX].GetShape();
    uint32_t batchSize = shape[0];
    LogInfo << "BatchSize=" << batchSize;
    for (uint32_t i = 0; i < batchSize; ++i) {
        ret = TFHandleOneBatch(tensors, objectInfos, resizedImageInfos[i], i);
        if (ret != APP_ERR_OK) {
            LogError << "Handle TensorFlow-RetinaNet Output failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    LogInfo << "End to handle TensorFlow-RetinaNet Output.";
    return ret;
}
}
#endif // MINDXSDK_RETINANETPOSTPROCESS_DPTR_H
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
 * Description: MaskRcnnMindsporePost is a proprietary interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MINDXSDK_MASKRCNNMINDSPOREPOST_DPTR_H
#define MINDXSDK_MASKRCNNMINDSPOREPOST_DPTR_H
#include <algorithm>
#include <vector>
#include <map>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include "ObjectPostProcessors/MaskRcnnMindsporePost.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/CV/Core/DataType.h"
#include "MxBase/PostProcessBases/ObjectPostProcessBase.h"
#include "MxBase/Log/Log.h"
#include "MxBase/CV/ObjectDetection/Nms/Nms.h"
#include "acl/acl.h"

namespace MxBase {
    const int OUTPUT_TENSOR_SIZE = 4;
    const int OUTPUT_BBOX_SIZE = 3;
    const int OUTPUT_BBOX_TWO_INDEX_SHAPE = 5;
    const int OUTPUT_BBOX_INDEX = 0;
    const int OUTPUT_CLASS_INDEX = 1;
    const int OUTPUT_MASK_INDEX = 2;
    const int OUTPUT_MASK_AREA_INDEX = 3;
    const int BBOX_INDEX_SCALE_NUM = 5;
    const int BBOX_COORD_IDX = 1;
    const int BBOX_COORD_NUM = 4;
    const int BBOX_DETECT_IDX = 2;
    const int DETECT_NUM = 100;
    const int BBOX_INFO_IDX = 3;
    const int BBOX_OUTPUT_SHAPE = 3;
    const int BBOX_INFO_SHAPE = 4;
    const unsigned int BBOX_SHAPE_SIZE = 3;
    const unsigned int CLASS_SHAPE_SIZE = 2;
    const unsigned int MASK_SHAPE_SIZE = 2;
    const unsigned int MASK_AREA_SHAPE_SIZE = 4;
    const int CLASS_DETECT_IDX = 1;
    const int MASK_DETECT_IDX = 1;

    const int BBOX_INDEX_LX = 0;
    const int BBOX_INDEX_LY = 1;
    const int BBOX_INDEX_RX = 2;
    const int BBOX_INDEX_RY = 3;
    const int BBOX_INDEX_PROB = 4;
}

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER MaskRcnnMindsporePostDptr {
public:
    explicit MaskRcnnMindsporePostDptr(MaskRcnnMindsporePost* pMaskRcnnMindsporePost);
    MaskRcnnMindsporePostDptr(const MaskRcnnMindsporePostDptr &other);
    ~MaskRcnnMindsporePostDptr() = default;
    MaskRcnnMindsporePostDptr &operator=(const MaskRcnnMindsporePostDptr &other);
    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;
    APP_ERROR PytorchOutput(const std::vector<TensorBase> &tensors,
        std::vector<std::vector<ObjectInfo>> &objectInfos, const std::vector<ResizedImageInfo> &resizedImageInfos);
    void GetConfigValue();
    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);

    bool IsValidTensorsMS(const std::vector<TensorBase> &tensors) const;

    APP_ERROR MsObjectDetectionOutput(const std::vector<TensorBase> &tensors,
        std::vector<std::vector<ObjectInfo>> &objectInfos, const std::vector<ResizedImageInfo> &resizedImageInfos);

    bool IsValidTensorsPT(const std::vector<TensorBase> &tensors) const;

    APP_ERROR GetValidDetBoxes(const std::vector<TensorBase> &tensors, std::vector<DetectBox> &detBoxes,
        const uint32_t batchNum);

    APP_ERROR MaskPostProcess(ObjectInfo &objInfo, void *maskPtr, const ResizedImageInfo &imgInfo);

    void ConvertObjInfoFromDetectBox(std::vector<DetectBox> &detBoxes, std::vector<ObjectInfo> &objectInfos,
                                     const ResizedImageInfo &resizedImageInfos);

    APP_ERROR GetMaskSize(const ObjectInfo &objInfo, const ResizedImageInfo &imgInfo, cv::Size &maskSize);

    int modelType_ = 0;
    enum ModelType {
        MINDSPORE = 0,
        PYTORCH = 1
    };
    const uint32_t MIN_CLASS_NUM_MS_MASK = 0;
    const uint32_t DEFAULT_CLASS_NUM_MS_MASK = 80;
    const float DEFAULT_SCORE_THRESH_MS_MASK = 0.7;
    const float DEFAULT_IOU_THRESH_MS_MASK = 0.5;
    const uint32_t DEFAULT_RPN_MAX_NUM_MS_MASK = 1000;
    const uint32_t DEFAULT_MAX_PER_IMG_MS_MASK = 128;
    const float DEFAULT_THR_BINARY_MASK = 0.5;
    const uint32_t DEFAULT_MASK_SIZE_MS_MASK = 28;

public:
    uint32_t classNum_ = DEFAULT_CLASS_NUM_MS_MASK;
    float scoreThresh_ = DEFAULT_SCORE_THRESH_MS_MASK;
    float iouThresh_ = DEFAULT_IOU_THRESH_MS_MASK;
    uint32_t rpnMaxNum_ = DEFAULT_RPN_MAX_NUM_MS_MASK;
    uint32_t maxPerImg_ = DEFAULT_MAX_PER_IMG_MS_MASK;
    float maskThrBinary_ = DEFAULT_THR_BINARY_MASK;
    uint32_t maskSize_ = DEFAULT_MASK_SIZE_MS_MASK;

public:
    MaskRcnnMindsporePost* qPtr_ = nullptr;

private:
    aclFloat16* detectedNumberPtr_ = nullptr;
    aclFloat16* scorePtr_ = nullptr;
    aclFloat16* bboxPtr_ = nullptr;
    aclFloat16* labelPtr_ = nullptr;
    aclFloat16* maskPtr_ = nullptr;
};

MaskRcnnMindsporePostDptr::MaskRcnnMindsporePostDptr(MaskRcnnMindsporePost* pMaskRcnnMindsporePost)
    : qPtr_(pMaskRcnnMindsporePost)
{}

MaskRcnnMindsporePostDptr::MaskRcnnMindsporePostDptr(const MaskRcnnMindsporePostDptr &other)
{
    *this = other;
}

MaskRcnnMindsporePostDptr &MaskRcnnMindsporePostDptr::operator=(const MaskRcnnMindsporePostDptr &other)
{
    if (this == &other) {
        return *this;
    }
    classNum_ = other.classNum_;
    scoreThresh_ = other.scoreThresh_;
    iouThresh_ = other.iouThresh_;
    rpnMaxNum_ = other.rpnMaxNum_;
    maxPerImg_ = other.maxPerImg_;
    maskThrBinary_ = other.maskThrBinary_;
    maskSize_ = other.maxPerImg_;
    return *this;
}

void MaskRcnnMindsporePostDptr::GetConfigValue()
{
    APP_ERROR ret = APP_ERR_OK;
    ret = qPtr_->configData_.GetFileValue<uint32_t>("CLASS_NUM", classNum_, (uint32_t)0x0, (uint32_t)0x64);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "No CLASS_NUM in config file, default value(" << classNum_ << ").";
    }
    ret = qPtr_->configData_.GetFileValue<float>("SCORE_THRESH", scoreThresh_, 0.0f, 1.0f);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "No SCORE_THRESH in config file, default value(" << scoreThresh_ << ").";
    }
    ret = qPtr_->configData_.GetFileValue<float>("IOU_THRESH", iouThresh_, 0.0f, 1.0f);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "No IOU_THRESH in config file, default value(" << iouThresh_ << ").";
    }
    ret = qPtr_->configData_.GetFileValue<uint32_t>("RPN_MAX_NUM", rpnMaxNum_, (uint32_t)0x0,
        DEFAULT_RPN_MAX_NUM_MS_MASK);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "No RPN_MAX_NUM in config file, default value(" << rpnMaxNum_ << ").";
    }
    ret = qPtr_->configData_.GetFileValue<uint32_t>("MAX_PER_IMG", maxPerImg_, (uint32_t)0x0, (uint32_t)0x96);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "No MAX_PER_IMG in config file, default value(" << maxPerImg_ << ").";
    }
    ret = qPtr_->configData_.GetFileValue<float>("MASK_THREAD_BINARY", maskThrBinary_, 0.0f, 1.0f);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "No MASK_THREAD_BINARY in config file, default value(" << maskThrBinary_ << ").";
    }
    ret = qPtr_->configData_.GetFileValue<uint32_t>("MASK_SHAPE_SIZE", maskSize_, (uint32_t)0x0, (uint32_t)0x64);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "No MASK_SHAPE_SIZE in config file, default value(" << maskSize_ << ").";
    }
    ret = qPtr_->configData_.GetFileValue<int>("MODEL_TYPE", modelType_);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read MODEL_TYPE from config, default MindSpore(0) will be used as modelType_.";
    }
    LogInfo << "The config parameters of post process are as follows: \n"
            << "  CLASS_NUM: " << classNum_ << " \n" << "  SCORE_THRESH: " << scoreThresh_ << " \n"
            << "  IOU_THRESH: " << iouThresh_ << " \n" << "  RPN_MAX_NUM: "
            << rpnMaxNum_ << " \n" << "  MAX_PER_IMG: "
            << maxPerImg_ << " \n" << "  MASK_THREAD_BINARY: "
            << maskThrBinary_ << " \n" << "  MASK_SHAPE_SIZE: "
            << maskSize_ << " \n" << "  MODEL_TYPE: "
            << modelType_ << " \n";
}

APP_ERROR MaskRcnnMindsporePostDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "The input tensors are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return qPtr_->CheckAndMoveTensors(tensors);
}

bool MaskRcnnMindsporePostDptr::IsValidTensorsPT(const std::vector<TensorBase> &tensors) const
{
    if (tensors.size() < OUTPUT_TENSOR_SIZE) {
        LogError << "The number of tensor (" << tensors.size() << ") is less than required (" << OUTPUT_TENSOR_SIZE
                 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto bboxShape = tensors[OUTPUT_BBOX_INDEX].GetShape();
    auto classShape = tensors[OUTPUT_CLASS_INDEX].GetShape();
    auto maskShape = tensors[OUTPUT_MASK_INDEX].GetShape();
    if (bboxShape.size() != BBOX_SHAPE_SIZE || classShape.size() != CLASS_SHAPE_SIZE ||
        maskShape.size() != MASK_SHAPE_SIZE || bboxShape[0] != classShape[0] || bboxShape[0] != maskShape[0]) {
        LogError << "The tensors shape is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (bboxShape[BBOX_COORD_IDX] != BBOX_COORD_NUM) {
        LogError << "The number of coord (" << bboxShape[BBOX_COORD_IDX] << ") is less than required ("
                 << BBOX_COORD_NUM << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (bboxShape[BBOX_DETECT_IDX] != DETECT_NUM) {
        LogError << "The number of bbox detection (" << bboxShape[BBOX_DETECT_IDX] << ") is not equal to required ("
                 << DETECT_NUM << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (classShape[CLASS_DETECT_IDX] != DETECT_NUM) {
        LogError << "The number of class detection (" << classShape[CLASS_DETECT_IDX] << ") is not equal to required ("
                 << DETECT_NUM << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (maskShape[MASK_DETECT_IDX] != DETECT_NUM) {
        LogError << "The number of mask detection (" << maskShape[MASK_DETECT_IDX] << ") is not equal to required ("
                 << DETECT_NUM << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto infoShape = tensors[BBOX_INFO_IDX].GetShape();
    if (infoShape.size() != BBOX_INFO_SHAPE) {
        LogError << "No." << BBOX_INFO_IDX << " tensorShape.size(" << infoShape.size() << ") is not equal to required ("
                 << BBOX_INFO_SHAPE << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (infoShape[OUTPUT_CLASS_INDEX] != classNum_) {
        LogError << "classNum_(" << classNum_ << ") is not equal to model's output(" << infoShape[OUTPUT_CLASS_INDEX]
                 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool MaskRcnnMindsporePostDptr::IsValidTensorsMS(const std::vector<TensorBase> &tensors) const
{
    if (tensors.size() < OUTPUT_TENSOR_SIZE) {
        LogError << "The number of tensor (" << tensors.size() << ") is less than required (" << OUTPUT_TENSOR_SIZE
                 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto bboxShape = tensors[OUTPUT_BBOX_INDEX].GetShape();
    if (bboxShape.size() != OUTPUT_BBOX_SIZE) {
        LogError << "The number of tensor[" << OUTPUT_BBOX_INDEX << "] dimensions (" << bboxShape.size()
                 << ") is not equal to (" << OUTPUT_BBOX_SIZE << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    uint32_t totalNum = classNum_ * rpnMaxNum_;
    if (bboxShape[BBOX_COORD_IDX] != totalNum) {
        LogError << "The output tensor is mismatched: " << totalNum << "/" << bboxShape[BBOX_COORD_IDX] << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
    }
    if (bboxShape[BBOX_DETECT_IDX] != OUTPUT_BBOX_TWO_INDEX_SHAPE) {
        LogError << "The number of bbox[" << BBOX_DETECT_IDX << "] dimensions (" << bboxShape[BBOX_DETECT_IDX]
                 << ") is not equal to (" << OUTPUT_BBOX_TWO_INDEX_SHAPE << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto classShape = tensors[OUTPUT_CLASS_INDEX].GetShape();
    auto maskShape = tensors[OUTPUT_MASK_INDEX].GetShape();
    auto maskAreaShape = tensors[OUTPUT_MASK_AREA_INDEX].GetShape();
    if (classShape.size() != CLASS_SHAPE_SIZE || maskShape.size() != MASK_SHAPE_SIZE ||
        maskAreaShape.size() != MASK_AREA_SHAPE_SIZE || classShape[0] != maskShape[0] ||
        classShape[0] != bboxShape[0] || classShape[0] != maskAreaShape[0]) {
        LogError << "The tensors shape is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (classShape[0x1] != totalNum) {
        LogError << "The output tensor is mismatched: (" << totalNum << "/" << classShape[0x1]
                 << "). " << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }

    if (maskShape[0x1] != totalNum) {
        LogError << "The output tensor is mismatched: (" << totalNum << "/" << maskShape[0x1] << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }

    if (maskAreaShape[0x1] != totalNum) {
        LogError << "The output tensor is mismatched: (" << totalNum << "/" << maskAreaShape[0x1]
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (maskAreaShape[0x2] != maskSize_) {
        LogError << "The output tensor of mask is mismatched: (" << maskAreaShape[0x2] << "/"
                 << maskSize_ << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool MaskRcnnMindsporePostDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    switch (static_cast<ModelType>(modelType_)) {
        case PYTORCH:
            return IsValidTensorsPT(tensors);
        case MINDSPORE:
            return IsValidTensorsMS(tensors);
        default:
            LogError << "The modelType_(" << modelType_ << ") is not supported." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
    }
    return true;
}

APP_ERROR MaskRcnnMindsporePostDptr::PytorchOutput(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<ObjectInfo>> &objectInfos, const std::vector<ResizedImageInfo> &)
{
    auto shape = tensors[OUTPUT_BBOX_INDEX].GetShape();
    uint32_t batchSize = shape[0];
    for (uint32_t i = 0; i < batchSize; ++i) {
        std::vector<ObjectInfo> objInfos;
        auto *bboxPtr = (float *) qPtr_->GetBuffer(tensors[OUTPUT_BBOX_INDEX], i); // 100 * 4
        auto *scorePtr = (float *) qPtr_->GetBuffer(tensors[OUTPUT_CLASS_INDEX], i); // 100 * 1
        auto *labelPtr = (int64_t *) qPtr_->GetBuffer(tensors[OUTPUT_MASK_INDEX], i); // 100 * 1
        if (labelPtr == nullptr || scorePtr == nullptr || bboxPtr == nullptr) {
            LogError << "The labelPtr or bboxPtr or scorePtr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        int detectedNumber = DETECT_NUM;
        for (int j = 0; j < detectedNumber; j++) {
            ObjectInfo objInfo {};
            auto classId = static_cast<int64_t>(labelPtr[j]);
            objInfo.classId = (float)classId;
            objInfo.className = qPtr_->configData_.GetClassName(static_cast<size_t>(objInfo.classId));
            objInfo.confidence = scorePtr[j];
            objInfo.x0 = bboxPtr[j * OUTPUT_TENSOR_SIZE + BBOX_INDEX_LX];
            objInfo.y0 = bboxPtr[j * OUTPUT_TENSOR_SIZE + BBOX_INDEX_LY];
            objInfo.x1 = bboxPtr[j * OUTPUT_TENSOR_SIZE + BBOX_INDEX_RX];
            objInfo.y1 = bboxPtr[j * OUTPUT_TENSOR_SIZE + BBOX_INDEX_RY];
            if (objInfo.confidence < qPtr_->separateScoreThresh_[classId]) {
                continue;
            }
            if (classId < MIN_CLASS_NUM_MS_MASK || classId > DEFAULT_CLASS_NUM_MS_MASK) {
                continue;
            }
            LogDebug << "(x0, y0, x1, y1, score) : " << objInfo.x0 << ", " << objInfo.y0 << ", " << objInfo.y0 <<
                     ", " << objInfo.x1 << ", " << objInfo.y1 << ", " << objInfo.confidence;
            objInfos.push_back(objInfo);
        }
        objectInfos.push_back(objInfos);
    }
    return APP_ERR_OK;
}

static bool CompareDetectBoxes(const MxBase::DetectBox &box1, const MxBase::DetectBox &box2)
{
    return box1.prob > box2.prob;
}

static void GetDetectBoxesTopK(std::vector<MxBase::DetectBox> &detBoxes, size_t kVal)
{
    std::sort(detBoxes.begin(), detBoxes.end(), CompareDetectBoxes);
    if (detBoxes.size() <= kVal) {
        return;
    }

    LogDebug << "Total detect boxes: " << detBoxes.size() << ", kVal: " << kVal;
    detBoxes.erase(detBoxes.begin() + kVal, detBoxes.end());
}

APP_ERROR MaskRcnnMindsporePostDptr::GetValidDetBoxes(const std::vector<TensorBase> &tensors,
    std::vector<DetectBox> &detBoxes, const uint32_t batchNum)
{
    LogInfo << "Begin to GetValidDetBoxes Mask GetValidDetBoxes.";
    auto *bboxPtr = (aclFloat16 *)qPtr_->GetBuffer(tensors[OUTPUT_BBOX_INDEX], batchNum);
    auto *labelPtr = (int32_t *)qPtr_->GetBuffer(tensors[OUTPUT_CLASS_INDEX], batchNum);
    auto *maskPtr = (bool *)qPtr_->GetBuffer(tensors[OUTPUT_MASK_INDEX], batchNum);
    auto *maskAreaPtr = (aclFloat16 *)qPtr_->GetBuffer(tensors[OUTPUT_MASK_AREA_INDEX], batchNum);
    if (bboxPtr == nullptr || labelPtr == nullptr || maskPtr == nullptr || maskAreaPtr == nullptr) {
        LogError << "bboxPtr or labelPtr or maskPtr or " << "maskAreaPtr is nullptr"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    // mask filter
    float prob = 0;
    size_t total = rpnMaxNum_ * classNum_;
    for (size_t index = 0; index < total; ++index) {
        if (!maskPtr[index]) {
            continue;
        }
        size_t startIndex = index * BBOX_INDEX_SCALE_NUM;
        prob = aclFloat16ToFloat(bboxPtr[startIndex + BBOX_INDEX_PROB]);
        if (prob <= scoreThresh_) {
            continue;
        }

        MxBase::DetectBox detBox;
        float x1 = aclFloat16ToFloat(bboxPtr[startIndex + BBOX_INDEX_LX]);
        float y1 = aclFloat16ToFloat(bboxPtr[startIndex + BBOX_INDEX_LY]);
        float x2 = aclFloat16ToFloat(bboxPtr[startIndex + BBOX_INDEX_RX]);
        float y2 = aclFloat16ToFloat(bboxPtr[startIndex + BBOX_INDEX_RY]);
        detBox.x = (x1 + x2) / COORDINATE_PARAM;
        detBox.y = (y1 + y2) / COORDINATE_PARAM;
        detBox.width = x2 - x1;
        detBox.height = y2 - y1;
        detBox.prob = prob;
        detBox.classID = labelPtr[index];
        detBox.maskPtr = maskAreaPtr + index * maskSize_ * maskSize_;
        detBoxes.push_back(detBox);
    }
    GetDetectBoxesTopK(detBoxes, maxPerImg_);
    return APP_ERR_OK;
}

APP_ERROR MaskRcnnMindsporePostDptr::GetMaskSize(const ObjectInfo &objInfo, const ResizedImageInfo &,
    cv::Size &maskSize)
{
    int width = static_cast<int>(objInfo.x1 - objInfo.x0 + 1);
    int height = static_cast<int>(objInfo.y1 - objInfo.y0 + 1);
    if (width < 1 || height < 1) {
        LogError << "The mask bbox is invalid, will be ignored, bboxWidth: " << width << ", bboxHeight: " << height
                 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_FAILURE;
    }

    maskSize.width = std::max(width, 1);
    maskSize.height = std::max(height, 1);
    return APP_ERR_OK;
}

APP_ERROR MaskRcnnMindsporePostDptr::MaskPostProcess(ObjectInfo &objInfo, void *maskPtr,
    const ResizedImageInfo &imgInfo)
{
    // resize
    cv::Mat maskMat(maskSize_, maskSize_, CV_32FC1);
    // maskPtr aclFloat16 to float
    aclFloat16 *maskTempPtr;
    auto *maskAclPtr = reinterpret_cast<aclFloat16 *>(maskPtr);
    for (int row = 0; row < maskMat.rows; ++row) {
        maskTempPtr = maskAclPtr + row * maskMat.cols;
        for (int col = 0; col < maskMat.cols; ++col) {
            maskMat.at<float>(row, col) = aclFloat16ToFloat(*(maskTempPtr + col));
        }
    }

    cv::Size maskSize;
    APP_ERROR ret = GetMaskSize(objInfo, imgInfo, maskSize);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    size_t bboxWidth = static_cast<size_t>(maskSize.width);
    size_t bboxHeight = static_cast<size_t>(maskSize.height);

    cv::Mat maskDst;
    cv::resize(maskMat, maskDst, cv::Size(bboxWidth, bboxHeight));
    std::vector<std::vector<uint8_t>> maskResult(bboxHeight, std::vector<uint8_t>(bboxWidth));

    for (size_t row = 0; row < bboxHeight; ++row) {
        for (size_t col = 0; col < bboxWidth; ++col) {
            if (maskDst.at<float>(row, col) <= maskThrBinary_) {
                continue;
            }
            maskResult[row][col] = 1;
        }
    }

    objInfo.mask = maskResult;
    return APP_ERR_OK;
}

void MaskRcnnMindsporePostDptr::ConvertObjInfoFromDetectBox(std::vector<DetectBox> &detBoxes,
    std::vector<ObjectInfo> &objectInfos, const ResizedImageInfo &resizedImageInfo)
{
    APP_ERROR ret = APP_ERR_OK;
    for (auto &detBoxe : detBoxes) {
        if (detBoxe.classID < 0) {
            continue;
        }
        ObjectInfo objInfo = {};
        objInfo.classId = (float)detBoxe.classID;
        objInfo.className = qPtr_->configData_.GetClassName(detBoxe.classID);
        objInfo.confidence = detBoxe.prob;

        objInfo.x0 = std::max<float>(detBoxe.x - detBoxe.width / COORDINATE_PARAM, 0);
        objInfo.y0 = std::max<float>(detBoxe.y - detBoxe.height / COORDINATE_PARAM, 0);
        objInfo.x1 = std::max<float>(detBoxe.x + detBoxe.width / COORDINATE_PARAM, 0);
        objInfo.y1 = std::max<float>(detBoxe.y + detBoxe.height / COORDINATE_PARAM, 0);

        objInfo.x0 = std::min<float>(objInfo.x0, resizedImageInfo.widthOriginal - 1);
        objInfo.y0 = std::min<float>(objInfo.y0, resizedImageInfo.heightOriginal - 1);
        objInfo.x1 = std::min<float>(objInfo.x1, resizedImageInfo.widthOriginal - 1);
        objInfo.y1 = std::min<float>(objInfo.y1, resizedImageInfo.heightOriginal - 1);

        LogDebug << "Find object: "
                 << "classId(" << objInfo.classId << "), confidence(" << objInfo.confidence << "), Coordinates("
                 << objInfo.x0 << ", " << objInfo.y0 << "; " << objInfo.x1 << ", " << objInfo.y1 << ").";

        ret = MaskPostProcess(objInfo, detBoxe.maskPtr, resizedImageInfo);
        if (ret == APP_ERR_COMM_FAILURE) {
            continue;
        } else if (ret != APP_ERR_OK) {
            break;
        }

        objectInfos.push_back(objInfo);
    }

    if (ret != APP_ERR_OK && ret != APP_ERR_COMM_FAILURE) {
        LogError << "Convert obj info failed." << GetErrorInfo(ret);
    }
}

APP_ERROR MaskRcnnMindsporePostDptr::MsObjectDetectionOutput(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<ObjectInfo>> &objectInfos, const std::vector<ResizedImageInfo> &resizedImageInfos)
{
    LogDebug << "MaskRcnnMindsporePost start to write results.";
    auto shape = tensors[OUTPUT_BBOX_INDEX].GetShape();
    uint32_t batchSize = shape[0];
    for (uint32_t i = 0; i < batchSize; ++i) {
        std::vector<MxBase::DetectBox> detBoxes;
        std::vector<ObjectInfo> objectInfo;
        APP_ERROR ret = GetValidDetBoxes(tensors, detBoxes, i);
        if (ret != APP_ERR_OK) {
            LogError << "GetValidDetBoxes failed." << GetErrorInfo(ret);
            return ret;
        }
        NmsSort(detBoxes, iouThresh_, MxBase::MAX);
        ConvertObjInfoFromDetectBox(detBoxes, objectInfo, resizedImageInfos[i]);
        objectInfos.push_back(objectInfo);
    }
    LogDebug << "MaskRcnnMindsporePost write results successed.";
    return APP_ERR_OK;
}
}
#endif // MINDXSDK_MASKRCNNMINDSPOREPOST_DPTR_H
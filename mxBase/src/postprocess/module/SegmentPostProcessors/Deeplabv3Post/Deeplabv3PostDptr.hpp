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
 * Description: Deeplabv3PostDptr private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef DEEPLAB_V3_POST_DPTR_H
#define DEEPLAB_V3_POST_DPTR_H

#include "MxBase/GlobalManager/GlobalManager.h"

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER Deeplabv3PostDptr {
public:
    explicit Deeplabv3PostDptr(Deeplabv3Post *qPtr);
    ~Deeplabv3PostDptr();
    Deeplabv3PostDptr(const Deeplabv3PostDptr &other);
    Deeplabv3PostDptr &operator=(const Deeplabv3PostDptr &other);

    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);

    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;

    APP_ERROR TensorflowFwOutput(const std::vector<TensorBase>& tensors,
                                 const std::vector<ResizedImageInfo>& resizedImageInfos,
                                 std::vector<SemanticSegInfo> &semanticSegInfos);
    void PytorchFwOutput(const std::vector<TensorBase>& tensors,
                         const std::vector<ResizedImageInfo>& resizedImageInfos,
                         std::vector<SemanticSegInfo> &semanticSegInfos);
    void MindsporeFwOutput(const std::vector<TensorBase>& tensors,
                           const std::vector<ResizedImageInfo>& resizedImageInfos,
                           std::vector<SemanticSegInfo> &semanticSegInfos);
    cv::Mat NCHWToNHWC(TensorBase &tensor, uint32_t batchNum, uint32_t batchSize);
    void GetImageInfoShape(uint32_t batchNum,
                           const std::vector<TensorBase> &tensors,
                           const std::vector<ResizedImageInfo> &resizedImageInfos);
    std::vector<std::vector<int>> CalArgmaxRes(cv::Mat OriginalMat, uint32_t& label);
    std::vector<std::vector<int>> OriginalSizeOutput(
            const std::vector<ResizedImageInfo>& resizedImageInfos,
            uint32_t bathNum, std::vector<std::vector<int>> argmaxResults);

public:
    int frameworkType_ = 0;
    uint32_t originalHeight_ = 0;
    uint32_t originalWidth_ = 0;
    uint32_t outputModelHeight_ = 0;
    uint32_t outputModelWidth_ = 0;
    uint32_t SlicedHeight_ = 0;
    uint32_t SlicedWidth_ = 0;
    uint32_t imgHeight_ = 0;
    uint32_t imgWidth_ = 0;
    Deeplabv3Post* qPtr_ = nullptr;
};

Deeplabv3PostDptr::Deeplabv3PostDptr(Deeplabv3Post *qPtr):qPtr_(qPtr) {}
Deeplabv3PostDptr::~Deeplabv3PostDptr() {}
Deeplabv3PostDptr::Deeplabv3PostDptr(const Deeplabv3PostDptr &other)
{
    *this = other;
}

Deeplabv3PostDptr& Deeplabv3PostDptr::operator=(const Deeplabv3PostDptr &other)
{
    if (this == &other) {
        return *this;
    }
    originalHeight_ = other.originalHeight_;
    originalWidth_ = other.originalWidth_;
    outputModelHeight_ = other.outputModelHeight_;
    outputModelWidth_ = other.outputModelWidth_;
    SlicedHeight_ = other.SlicedHeight_;
    SlicedWidth_ = other.SlicedWidth_;
    imgHeight_ = other.imgHeight_;
    imgWidth_ = other.imgWidth_;
    return *this;
}

APP_ERROR Deeplabv3PostDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "Input tensors are invalid" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return qPtr_->CheckAndMoveTensors(tensors);
}

bool Deeplabv3PostDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    if (tensors.size() != 1) {
        LogError << "The number of tensors (" << tensors.size() << ") " << "is not equal to required (" << 1 << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto shape = tensors[0].GetShape();
    if (shape.size() != 0x4) {
        LogError << "The number of tensor dimensions (" << shape.size() << ") " << "is not equal to ("
                 << 0x4 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (shape[0x0] != 0x1) {
        LogError << "The value of tensor batch num not equal to 1." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (qPtr_->modelType_ == TYPE_NHWC) {
        if (shape[0x3] != qPtr_->classNum_) {
            return false;
        }
    } else if (qPtr_->modelType_ == TYPE_NCHW) {
        if (shape[0x1] != qPtr_->classNum_) {
            return false;
        }
    }
    return true;
}

APP_ERROR Deeplabv3PostDptr::TensorflowFwOutput(const std::vector<TensorBase> &tensors,
                                                const std::vector<ResizedImageInfo> &resizedImageInfos,
                                                std::vector<SemanticSegInfo> &semanticSegInfos)
{
    LogDebug << "TensorflowFwOutput start to write results.";
    auto tensor = tensors[0];
    auto shape = tensor.GetShape();
    uint32_t batchSize = shape[0];

    for (uint32_t i = 0; i < batchSize; i++) {
        GetImageInfoShape(i, tensors, resizedImageInfos);
        SemanticSegInfo semanticSegInfo;
        auto tensorPtr = (float*)(qPtr_->GetBuffer)(tensor, i);
        if (tensorPtr == nullptr) {
            LogError << "The tensorPtr is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        // Argmax between classes.
        std::vector<std::vector<int>> results(outputModelHeight_, std::vector<int>(outputModelWidth_));
        for (uint32_t y = 0; y < outputModelHeight_; y++) {
            for (uint32_t x = 0; x < outputModelWidth_; x++) {
                float* begin = tensorPtr +  y * outputModelWidth_ * qPtr_->classNum_
                        + x * qPtr_->classNum_;
                results[y][x] = std::max_element(begin, begin + qPtr_->classNum_) - begin;
            }
        }
        std::vector<std::vector<int>> resultPixels = OriginalSizeOutput(resizedImageInfos, i, results);
        semanticSegInfo.pixels = resultPixels;
        semanticSegInfo.labelMap = qPtr_->labelMap_;
        semanticSegInfos.push_back(semanticSegInfo);
    }
    LogDebug << "TensorflowFwOutput write results successed.";
    return APP_ERR_OK;
}

void Deeplabv3PostDptr::PytorchFwOutput(const std::vector<TensorBase> &tensors,
                                        const std::vector<ResizedImageInfo> &resizedImageInfos,
                                        std::vector<SemanticSegInfo> &semanticSegInfos)
{
    LogDebug << "PytorchFwOutput start to write results.";
    auto tensor = tensors[0];
    auto shape = tensor.GetShape();
    uint32_t batchSize = shape[0];

    for (uint32_t i = 0; i < batchSize; i++) {
        GetImageInfoShape(i, tensors, resizedImageInfos);
        SemanticSegInfo semanticSegInfo;
        cv::Mat hwcMat = NCHWToNHWC(tensor, i, batchSize);
        // Argmax
        std::vector<std::vector<int>> results(outputModelHeight_, std::vector<int>(outputModelWidth_));
        cv::Mat& hwcMatOut = hwcMat;
        float* hwcMatData = (float*)hwcMatOut.data;
        for (uint32_t y = 0; y < outputModelHeight_; ++y) {
            for (uint32_t x = 0; x < outputModelWidth_; ++x) {
                float* begin = hwcMatData + y * (outputModelWidth_ * qPtr_->classNum_)
                        + x * qPtr_->classNum_;
                results[y][x] = std::max_element(begin, begin + qPtr_->classNum_) - begin;
            }
        }
        semanticSegInfo.pixels = results;
        semanticSegInfo.labelMap = qPtr_->labelMap_;
        semanticSegInfos.push_back(semanticSegInfo);
    }
    LogDebug << "PytorchFwOutput write results successed.";
}

void Deeplabv3PostDptr::MindsporeFwOutput(const std::vector<TensorBase> &tensors,
                                          const std::vector<ResizedImageInfo> &resizedImageInfos,
                                          std::vector<SemanticSegInfo> &semanticSegInfos)
{
    LogDebug << "MindsporeFwOutput start to write results.";
    auto tensor = tensors[0];
    auto shape = tensor.GetShape();
    uint32_t batchSize = shape[0];

    for (uint32_t i = 0; i < batchSize; i++) {
        GetImageInfoShape(i, tensors, resizedImageInfos);
        SemanticSegInfo semanticSegInfo;
        cv::Mat hwcMat = NCHWToNHWC(tensor, i, batchSize);
        cv::Mat slicedMat = cv::Mat(SlicedHeight_, SlicedWidth_, CV_32FC(qPtr_->classNum_));
        LogDebug << "End to Process SliceToResizedImg.";
        cv::Mat originalMat = cv::Mat(originalHeight_, originalWidth_, CV_32FC(qPtr_->classNum_));
        LogDebug << "End to Process ResizeToOriginalImg.";
        slicedMat = hwcMat(cv::Range(0, SlicedHeight_), cv::Range(0, SlicedWidth_));
        cv::resize(slicedMat,
                   originalMat,
                   cv::Size(static_cast<int>(originalWidth_), static_cast<int>(originalHeight_)),
                   cv::INTER_LINEAR);
        uint32_t label = 0;
        std::vector<std::vector<int>> results = CalArgmaxRes(originalMat, label);
        if (label) {
            continue;
        }
        semanticSegInfo.pixels = results;
        semanticSegInfo.labelMap = qPtr_->labelMap_;
        semanticSegInfos.push_back(semanticSegInfo);
    }
    LogDebug << "MindsporeFwOutput write results successed.";
}

cv::Mat Deeplabv3PostDptr::NCHWToNHWC(TensorBase &tensor, uint32_t batchNum, uint32_t batchSize)
{
    LogDebug << "Start to Process NCHWToNHWC.";
    cv::Mat hwcMat = cv::Mat::zeros(outputModelHeight_, outputModelWidth_,
                                    CV_32FC(qPtr_->classNum_));
    cv::Mat& hwcMatOut = hwcMat;
    float* hwcMatData = (float*)hwcMatOut.data;
    if (hwcMatData == nullptr) {
        LogError << "The hwcMatData are invalid pointer. " << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return cv::Mat::zeros(0, 0, CV_32FC(0));
    }
    if (batchSize == 0) {
        return cv::Mat::zeros(0, 0, CV_32FC(0));
    }
    if (tensor.GetBuffer() == nullptr) {
        LogError << "The tensor.GetBuffer() is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return cv::Mat::zeros(0, 0, CV_32FC(0));
    }
    float *outputInfo = (float *)(tensor.GetBuffer()) + batchNum * tensor.GetByteSize() / batchSize;
    if (outputInfo == nullptr) {
        LogError << "The hwcMatData are invalid pointer." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return cv::Mat::zeros(0, 0, CV_32FC(0));
    }
    for (uint32_t c = 0; c < qPtr_->classNum_; ++c) {
        for (uint32_t y = 0; y < outputModelHeight_; ++y) {
            for (uint32_t x = 0; x < outputModelWidth_; ++x) {
                float* begin = outputInfo +  c * outputModelHeight_ * outputModelWidth_ +  y * outputModelWidth_;
                hwcMatData[y * (outputModelWidth_ * qPtr_->classNum_)
                + x * qPtr_->classNum_ + c] = *(begin + x);
            }
        }
    }
    LogDebug << "End to Process NCHWToNHWC.";
    return hwcMat;
}

void Deeplabv3PostDptr::GetImageInfoShape(uint32_t batchNum,
                                          const std::vector<TensorBase> &tensors,
                                          const std::vector<ResizedImageInfo> &resizedImageInfos)
{
    LogDebug << "Start to process GetImageInfoShape.";
    originalHeight_ = resizedImageInfos[batchNum].heightOriginal;
    originalWidth_ = resizedImageInfos[batchNum].widthOriginal;
    imgHeight_ = resizedImageInfos[batchNum].heightOriginal;
    imgWidth_ = resizedImageInfos[batchNum].widthOriginal;
    outputModelHeight_ = tensors[batchNum].GetShape()[qPtr_->modelType_ ?
                                                      0x2 : 0x1];
    outputModelWidth_ = tensors[batchNum].GetShape()[qPtr_->modelType_ ?
                                                     0x3 : 0x2];
    if (IsDenominatorZero(originalHeight_) || IsDenominatorZero(originalWidth_)) {
        LogWarn << "originalHeight_ is " << originalHeight_ << ", originalWidth_ is " << originalWidth_;
        return;
    }
    if (originalHeight_ > originalWidth_) {
        SlicedWidth_ = outputModelHeight_ * originalWidth_ / originalHeight_;
        SlicedHeight_ = outputModelHeight_;
    } else {
        SlicedHeight_ = outputModelWidth_ * originalHeight_ / originalWidth_;
        SlicedWidth_ = outputModelWidth_;
    }
    LogDebug << "End to process GetImageInfoShape.";
}

std::vector<std::vector<int>> Deeplabv3PostDptr::OriginalSizeOutput(
    const std::vector<ResizedImageInfo>& resizedImageInfos,
    uint32_t bathNum, std::vector<std::vector<int>> argmaxResults)
{
    LogDebug << "Start to Process OriginalSizeOutput.";
    std::vector<std::vector<int>> resultPixels(imgHeight_, std::vector<int>(imgWidth_));
    for (uint32_t y = 0; y < imgHeight_; y++) {
        for (uint32_t x = 0; x < imgWidth_; x++) {
            // Use Slice on results to original image size.
            if (resizedImageInfos[bathNum].resizeType == RESIZER_ONLY_PADDING) {
                resultPixels[y][x] = argmaxResults[y][x];
            }  else {
                // Resize to original image size.
                resultPixels[y][x] = argmaxResults[y * outputModelHeight_ / imgHeight_] \
                [x * outputModelWidth_ / imgWidth_];
            }
        }
    }
    LogDebug << "End to Process OriginalSizeOutput.";
    return resultPixels;
}

std::vector<std::vector<int>> Deeplabv3PostDptr::CalArgmaxRes(cv::Mat originalMat, uint32_t& label)
{
    LogDebug << "Start to Process CalArgmaxRes.";
    uint32_t step0 = originalMat.step[0];
    uint32_t step1 = originalMat.step[1];
    std::vector<std::vector<int>> results(originalHeight_, std::vector<int>(originalWidth_));
    float *originalData = (float*)(originalMat.data);
    if (originalData == nullptr) {
        LogError << "The pointer are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        label = 1;
        return results;
    }
    for (uint32_t y = 0; y < originalHeight_; y++) {
        for (uint32_t x = 0; x < originalWidth_; x++) {
            float *pre_ptr = originalData +
                step0 / sizeof(float) * y + step1 / sizeof(float) * x;
            results[y][x] = std::max_element(pre_ptr, pre_ptr + qPtr_->classNum_) - pre_ptr;
        }
    }
    LogDebug << "End to Process CalArgmaxRes.";
    return results;
}
}

#endif
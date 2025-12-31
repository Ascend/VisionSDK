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
 * Description: CrnnPostProcess private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef CRNN_POST_PROCESS_DPTR_H
#define CRNN_POST_PROCESS_DPTR_H

#include "TextGenerationPostProcessors/CrnnPostProcess.h"
#include "MxBase/Log/Log.h"

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER CrnnPostProcessDptr {
public:
    explicit CrnnPostProcessDptr(CrnnPostProcess *pCrnnPostProcess);

    CrnnPostProcessDptr(const CrnnPostProcessDptr &others);

    ~CrnnPostProcessDptr() = default;

    void TextGenerationOutput(const std::vector<TensorBase> &tensors, std::vector<TextsInfo> &textsInfos);

    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);

    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;

    std::string CalcOutputArgmax(TensorBase &tensor, uint32_t batchNum);

    std::string CalcOutputIndex(TensorBase &tensor, uint32_t batchNum);

    CrnnPostProcessDptr &operator=(const CrnnPostProcessDptr &other);

public:
    uint32_t objectNum_ = 0;
    uint32_t blankIdx_ = 0;
    bool withArgmax_ = false;
    CrnnPostProcess* qPtr_ = nullptr;
};

CrnnPostProcessDptr::CrnnPostProcessDptr(CrnnPostProcess *pCrnnPostProcess)
    : qPtr_(pCrnnPostProcess)
{}

CrnnPostProcessDptr::CrnnPostProcessDptr(const CrnnPostProcessDptr &others)
{
    *this = others;
}

CrnnPostProcessDptr& CrnnPostProcessDptr::operator=(const CrnnPostProcessDptr &other)
{
    if (this == &other) {
        return *this;
    }
    objectNum_ = other.objectNum_;
    blankIdx_ = other.blankIdx_;
    withArgmax_ = other.withArgmax_;
    return *this;
}

void CrnnPostProcessDptr::TextGenerationOutput(const std::vector<TensorBase> &tensors,
    std::vector<TextsInfo> &textsInfos)
{
    LogDebug << "CrnnPostProcess start to write results.";
    auto shape = tensors[0].GetShape();
    auto tensor = tensors[0];
    uint32_t batchSize = shape[0];

    for (uint32_t i = 0; i < batchSize; i++) {
        TextsInfo textsInfo;
        std::string result = "";
        if (!withArgmax_) {
            result = CalcOutputArgmax(tensor, i);
        } else {
            result = CalcOutputIndex(tensor, i);
        }
        textsInfo.text.push_back(result);
        textsInfos.push_back(textsInfo);
        LogDebug << "CrnnPostProcessor output string(" << textsInfos[i].text[0] << ").";
    }
    LogDebug << "CrnnPostProcess end to write results.";
}

bool CrnnPostProcessDptr::IsValidTensors(const std::vector <TensorBase> &tensors) const
{
    if (tensors.size() != 1) {
        LogError << "The number of tensors (" << tensors.size() << ") " << "is not equal to required (" << 1 << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto tensor = tensors[0];
    auto shape = tensor.GetShape();
    // The output tensors of model with no argmax layer have one more dimension.
    if (!withArgmax_) {
        for (size_t i = 0; i < tensors.size(); i++) {
            if (tensors[i].GetDataTypeSize() != qPtr_->FOUR_BYTE) {
                LogError << "The tensor type(" << TensorDataTypeStr[tensors[i].GetTensorType()]
                         << ") mismatched. requires(" << qPtr_->FOUR_BYTE << ") bytes tensortype."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return false;
            }
        }
        if (shape.size() != 0x3 && shape.size() != 0x4) {
            LogError << "The input size of tensor0(" << shape.size() << ") " << "mismatch the size:"
                     << 0x3 << " or " << 0x4 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (shape.size() == 0x3 && shape[shape.size() - 0x2] != objectNum_) {
            LogError << "The input size(" << shape[shape.size() - 0x2] << ") not equal to objectNum("
                     << objectNum_ << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        } else if (shape.size() == 0x4 && shape[shape.size() - 0x3] != objectNum_) {
            LogError << "The input size(" << shape[shape.size() - 0x3] << ") not equal to objectNum("
                     << objectNum_ << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (shape[shape.size() - 0x1] != qPtr_->classNum_) {
            LogError << "The input size(" << shape[shape.size() - 0x1] << ") " << "not equal to classNum("
                     << qPtr_->classNum_ << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    } else {
        if (shape.size() != 0x2) {
            LogError << "The input size of tensor0(" << shape.size() << ") " << "mismatch the size:"
                     << 0x2 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (shape[1] != objectNum_) {
            LogError << "The input size(" << shape[1] << ") " << "not equal to objectNum(" << objectNum_ << ")"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
    return true;
}

APP_ERROR CrnnPostProcessDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "Input tensors are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return qPtr_->CheckAndMoveTensors(tensors);
}

std::string CrnnPostProcessDptr::CalcOutputArgmax(TensorBase &tensor, uint32_t batchNum)
{
    LogDebug << "Start to Process CalcOutputArgmax.";
    float *outputInfo = (float *)(qPtr_->GetBuffer)(tensor, batchNum);
    if (outputInfo == nullptr) {
        LogError << "The outputInfo is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return "";
    }
    std::vector<float> logits;
    uint32_t previousIdx = blankIdx_;
    std::string result = "";
    for (uint32_t i = 0; i < objectNum_; i++) {
        for (uint32_t j = 0; j < (qPtr_->classNum_); j++) {
            logits.push_back(outputInfo[i * (qPtr_->classNum_) + j]);
        }
        auto maxElement = std::max_element(std::begin(logits), std::end(logits));
        uint32_t argmaxIndex = (uint32_t)(maxElement - std::begin(logits));
        if (argmaxIndex != blankIdx_ && argmaxIndex != previousIdx) {
            result += (qPtr_->configData_).GetClassName(argmaxIndex);
        }
        previousIdx = argmaxIndex;
        logits.clear();
    }
    LogDebug << "End to Process CalcOutputArgmax.";
    return result;
}

std::string CrnnPostProcessDptr::CalcOutputIndex(TensorBase &tensor, uint32_t batchNum)
{
    LogDebug << "Start to Process CalcOutputIndex.";
    long long int *objectInfo = (long long int *)(qPtr_->GetBuffer)(tensor, batchNum);
    if (objectInfo == nullptr) {
        LogError << "The objectInfo is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return "";
    }
    long long int previousIdx = blankIdx_;
    std::string result = "";
    for (uint32_t i = 0; i < objectNum_; i++) {
        if (objectInfo[i] != blankIdx_ && objectInfo[i] != previousIdx) {
            result += (qPtr_->configData_).GetClassName(objectInfo[i]);
        }
        previousIdx = objectInfo[i];
    }
    LogDebug << "End to Process CalcOutputIndex.";
    return result;
}
}
#endif

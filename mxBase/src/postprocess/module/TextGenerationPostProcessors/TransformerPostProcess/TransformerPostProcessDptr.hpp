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
 * Description: TransformerPostProcess private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef TRANSFORMER_POST_PROCESS_DPTR_H
#define TRANSFORMER_POST_PROCESS_DPTR_H

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER TransformerPostProcessDptr {
public:
    void TextGenerationOutput(const std::vector<TensorBase> &tensors, std::vector<TextsInfo>& textsInfos);

    std::string CalcOutputIndex(TensorBase &tensor, uint32_t batchNum, uint32_t batchSize);

    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;

    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);

    explicit TransformerPostProcessDptr(TransformerPostProcess* pTransformerPostProcess);

    TransformerPostProcessDptr(const TransformerPostProcessDptr &other);

    ~TransformerPostProcessDptr() = default;

    TransformerPostProcessDptr &operator=(const TransformerPostProcessDptr &other);

public:
    TransformerPostProcess* qPtr_ = nullptr;
    uint32_t breakIdx_ {};
};

TransformerPostProcessDptr& TransformerPostProcessDptr::operator=(const TransformerPostProcessDptr &other)
{
    if (this == &other) {
        return *this;
    }
    breakIdx_ = other.breakIdx_;
    return *this;
}

TransformerPostProcessDptr::TransformerPostProcessDptr(TransformerPostProcess* pTransformerPostProcess)
    : qPtr_(pTransformerPostProcess)
{}

TransformerPostProcessDptr::TransformerPostProcessDptr(const TransformerPostProcessDptr &other)
{
    *this = other;
}

void TransformerPostProcessDptr::TextGenerationOutput(const std::vector<TensorBase> &tensors,
    std::vector<TextsInfo>& textsInfos)
{
    LogDebug << "TransformerPostProcess start to write results.";
    if (tensors.empty()) {
        LogError << "Input tensor is empty, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    auto shape = tensors[0].GetShape();
    if (shape.empty()) {
        LogError << "Input tensor shape is invalid, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    auto tensor = tensors[0];
    uint32_t batchSize = shape[0];

    for (uint32_t i = 0; i < batchSize; i++) {
        TextsInfo textsInfo;
        std::string result = "";
        result = CalcOutputIndex(tensor, i, batchSize);
        textsInfo.text.push_back(result);
        textsInfos.push_back(textsInfo);
    }
    LogDebug << "TransformerPostProcess end to write results.";
}

bool TransformerPostProcessDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    if (tensors.size() == 0 || tensors.size() > qPtr_->MAX_TENSOR_VEC_SIZE) {
        LogError << "The number of tensors (" << tensors.size() << ") " << "is not equal to required."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto tensor = tensors[0];
    auto shape = tensor.GetShape();
    for (size_t i = 0; i < tensors.size(); i++) {
        if (tensors[i].GetDataTypeSize() != qPtr_->FOUR_BYTE) {
            LogError << "The tensor type(" << TensorDataTypeStr[tensors[i].GetTensorType()]
                     << ") mismatched. requires(" << qPtr_->FOUR_BYTE << ") bytes tensortype."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
    return true;
}

APP_ERROR TransformerPostProcessDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "Input tensors are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return qPtr_->CheckAndMoveTensors(tensors);
}

std::string TransformerPostProcessDptr::CalcOutputIndex(TensorBase &tensor, uint32_t batchNum, uint32_t batchSize)
{
    LogDebug << "Start to Process CalcOutputIndex.";
    std::string result = "";
    if (batchSize == 0) {
        LogError << "The batchSize is 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return result;
    }
    if (tensor.GetBuffer() == nullptr) {
        LogError << "tensor.GetBuffer() is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return result;
    }
    uint32_t *objectInfo = (uint32_t *)(tensor.GetBuffer()) + batchNum * tensor.GetByteSize() / batchSize;
    size_t outputSize = 0;
    if (tensor.GetDataTypeSize() == qPtr_->FOUR_BYTE) {
        outputSize = tensor.GetByteSize() / batchSize / qPtr_->FOUR_BYTE;
    } else {
        LogError << "The output tensor data type is wrong." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
    }
    for (uint32_t i = 0; i < (uint32_t)outputSize; i++) {
        if (objectInfo[i] == breakIdx_) {
            break;
        }
        result += (qPtr_->configData_).GetClassName(objectInfo[i]) + " ";
    }
    LogDebug << "End to Process CalcOutputIndex.";
    return result;
}
}
#endif
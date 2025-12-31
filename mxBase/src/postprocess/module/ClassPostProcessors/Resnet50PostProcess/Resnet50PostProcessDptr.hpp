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
 * Description: Resnet50PostProcess private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef RESNET50_POST_PROCESS_DPTR_H
#define RESNET50_POST_PROCESS_DPTR_H
#include "MxBase/PostProcessBases/ClassPostProcessBase.h"

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER Resnet50PostProcessDptr {
public:
    explicit Resnet50PostProcessDptr(Resnet50PostProcess *pResnet50PostProcess);
    Resnet50PostProcessDptr(const Resnet50PostProcessDptr &other);
    ~Resnet50PostProcessDptr() = default;
    Resnet50PostProcessDptr &operator=(const Resnet50PostProcessDptr &other);
    uint32_t classNum_ = 0;
    bool softmax_ = false;
    uint32_t topK_ = 1;
    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);
    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;
public:
    Resnet50PostProcess *qPtr_ = nullptr;
};

Resnet50PostProcessDptr::Resnet50PostProcessDptr(Resnet50PostProcess *pResnet50PostProcess)
    : qPtr_(pResnet50PostProcess)
{}

Resnet50PostProcessDptr::Resnet50PostProcessDptr(const Resnet50PostProcessDptr &other)
{
    *this = other;
}

Resnet50PostProcessDptr& Resnet50PostProcessDptr::operator=(const Resnet50PostProcessDptr &other)
{
    if (this == &other) {
        return *this;
    }
    softmax_ = other.softmax_;
    classNum_ = other.classNum_;
    topK_ = other.topK_;
    return *this;
}

bool Resnet50PostProcessDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    const uint32_t softmaxTensorIndex = 0;
    if (tensors.empty()) {
        LogError << "Tensor size is: " << tensors.size() << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto softmaxTensor = tensors[softmaxTensorIndex];
    if (softmaxTensor.GetDataTypeSize() != qPtr_->FOUR_BYTE) {
        LogError << "Tensor type(" << TensorDataTypeStr[softmaxTensor.GetTensorType()]
                 << ") mismatched. required(" << qPtr_->FOUR_BYTE << ") bytes tensortype."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto softmaxShape = softmaxTensor.GetShape();
    if (softmaxShape.size() < 0x2) {
        LogError << "Input size of tensor0(" << softmaxShape.size() << ") " << "< " << 0x2 << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (softmaxShape[1] != classNum_) {
        LogError << "Input size(" << softmaxShape[1] << ") "
                 << "classNumber(" << classNum_ << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

APP_ERROR Resnet50PostProcessDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "Input tensors are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return qPtr_->CheckAndMoveTensors(tensors);
}
}
#endif
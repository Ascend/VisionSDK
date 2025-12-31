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
 * Description: Ssdvgg16PostProcess private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef SSDVGG16_POST_PROCESS_DPTR_H
#define SSDVGG16_POST_PROCESS_DPTR_H
#include "MxBase/PostProcessBases/ObjectPostProcessBase.h"

namespace MxBase {
    const int MIN_TENSOR_LENGTH = 2;
    const int MIN_INFO_POOLSIZE = 3;
    const int MAX_DETECT_NUM = 100;

    const int CLASSID  = 1;
    const int CONFIDENCE  = 2;
    const int LEFTTOPX  = 3;
    const int LEFTTOPY  = 4;
    const int RIGHTBOTX = 5;
    const int RIGHTBOTY = 6;
    const int INFONUM = 8;
}

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER Ssdvgg16PostProcessDptr {
public:
    explicit Ssdvgg16PostProcessDptr(Ssdvgg16PostProcess *pSsdvgg16PostProcess);

    Ssdvgg16PostProcessDptr(const Ssdvgg16PostProcessDptr &other);

    ~Ssdvgg16PostProcessDptr() = default;

    Ssdvgg16PostProcessDptr &operator=(const Ssdvgg16PostProcessDptr &other);

    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);

    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;

    APP_ERROR ObjectDetectionOutput(const std::vector<TensorBase> &tensors,
                                    std::vector<std::vector<ObjectInfo>> &objectInfos);

    const int DEFAULT_OBJECT_NUM_TENSOR = 0;
    const int DEFAULT_OBJECT_INFO_TENSOR = 1;

    uint32_t objectNumTensor_ = DEFAULT_OBJECT_NUM_TENSOR;
    uint32_t objectInfoTensor_ = DEFAULT_OBJECT_INFO_TENSOR;

    Ssdvgg16PostProcess* qPtr_ = nullptr;
};

Ssdvgg16PostProcessDptr::Ssdvgg16PostProcessDptr(Ssdvgg16PostProcess *pSsdvgg16PostProcess)
    : qPtr_(pSsdvgg16PostProcess)
{}

Ssdvgg16PostProcessDptr::Ssdvgg16PostProcessDptr(const Ssdvgg16PostProcessDptr &other)
{
    *this = other;
}

Ssdvgg16PostProcessDptr& Ssdvgg16PostProcessDptr::operator=(const Ssdvgg16PostProcessDptr &other)
{
    if (this == &other) {
        return *this;
    }
    return *this;
}

APP_ERROR Ssdvgg16PostProcessDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "Input tensors are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return qPtr_->CheckAndMoveTensors(tensors);
}

bool Ssdvgg16PostProcessDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    for (size_t i = 0; i < tensors.size(); i++) {
        if (tensors[i].GetDataTypeSize() != qPtr_->FOUR_BYTE) {
            LogError << "The tensor type(" << TensorDataTypeStr[tensors[i].GetTensorType()]
                     << ") mismatched. requires(" << qPtr_->FOUR_BYTE << ") bytes tensortype."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
    if (tensors.size() < 0x2) {
        LogError << "The number of tensors (" << tensors.size() << ") " << "is less than required ("
                 << 0x2 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto firstTensorShape = tensors[0].GetShape();
    auto secondTensorShape = tensors[1].GetShape();
    if (firstTensorShape.size() < 0x1 || secondTensorShape.size() != 0x3 ||
        firstTensorShape[0] != secondTensorShape[0]) {
        LogError << "The tensors shape is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (secondTensorShape[0x2] != INFONUM) {
        LogError << "The dimension of tensor[1][2] (" << secondTensorShape[0x2] << ") " << "is not equal to ("
                 << INFONUM << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

APP_ERROR Ssdvgg16PostProcessDptr::ObjectDetectionOutput(const std::vector<TensorBase>& tensors,
    std::vector<std::vector<ObjectInfo>>& objectInfos)
{
    LogDebug << "Ssdvgg16PostProcessor start to write results.";
    for (auto num : {objectNumTensor_, objectInfoTensor_}) {
        if (num >= tensors.size()) {
            LogError << "TENSOR(" << num << ") must be less than tensors'size(" << tensors.size()
                     << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    uint32_t batchSize = tensors[objectNumTensor_].GetShape()[0];
    for (uint32_t i = 0; i < batchSize; i++) {
        int* objectNumPtr = (int*) qPtr_->GetBuffer(tensors[objectNumTensor_], i);
        float *objectInfoPtr = (float*) qPtr_->GetBuffer(tensors[objectInfoTensor_], i);
        if (objectNumPtr == nullptr || objectInfoPtr == nullptr) {
            LogError << "The objectNumPtr or objectInfoPtr is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        if (*objectNumPtr > MAX_DETECT_NUM) {
            LogWarn << "Detected objects' number is more than 100, check ScoreTresh value.";
        }
        std::vector<ObjectInfo> objectInfo = {};
        for (int j = 0; j < *objectNumPtr && j < MAX_DETECT_NUM; j++) {
            int classId = (int)objectInfoPtr[j * INFONUM + CLASSID];
            if (classId < 0 || classId >= (int)qPtr_->classNum_) {
                continue;
            }
            float confidence = objectInfoPtr[j * INFONUM + CONFIDENCE];
            if (confidence < qPtr_->separateScoreThresh_[classId]) {
                continue;
            }
            ObjectInfo objInfo;
            objInfo.classId = classId;
            objInfo.confidence = confidence;
            objInfo.className = qPtr_->configData_.GetClassName((size_t)objInfo.classId);
            objInfo.x0 = objectInfoPtr[j * INFONUM + LEFTTOPX];
            objInfo.y0 = objectInfoPtr[j * INFONUM + LEFTTOPY];
            objInfo.x1 = objectInfoPtr[j * INFONUM + RIGHTBOTX];
            objInfo.y1 = objectInfoPtr[j * INFONUM + RIGHTBOTY];
            objectInfo.push_back(objInfo);
        }
        objectInfos.push_back(objectInfo);
    }
    LogDebug << "Ssdvgg16PostProcessor write results successed.";
    return APP_ERR_OK;
}
}
#endif
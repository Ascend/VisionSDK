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
 * Description: SsdMobilenetv1FpnPostProcess private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef SSDMOBILENETV1FPN_POST_PROCESS_DPTR_H
#define SSDMOBILENETV1FPN_POST_PROCESS_DPTR_H
#include "MxBase/PostProcessBases/ObjectPostProcessBase.h"
#include "ObjectPostProcessors/SsdMobilenetv1FpnPostProcess.h"

namespace MxBase {
    const int BBOX_POOLSIZE = 3;
    const int MIN_TENSOR_LENGTH = 4;
    const int MAX_DETECT_NUM = 100;

    const int LEFTTOPY  = 0;
    const int LEFTTOPX  = 1;
    const int RIGHTBOTY = 2;
    const int RIGHTBOTX = 3;
}

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER SsdMobilenetv1FpnPostProcessDptr {
public:
    explicit SsdMobilenetv1FpnPostProcessDptr(SsdMobilenetv1FpnPostProcess* pSsdMobilenetv1FpnPostProcess);

    ~SsdMobilenetv1FpnPostProcessDptr() = default;

    SsdMobilenetv1FpnPostProcessDptr(const SsdMobilenetv1FpnPostProcessDptr &other);

    SsdMobilenetv1FpnPostProcessDptr &operator=(const SsdMobilenetv1FpnPostProcessDptr &other);

    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);

    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;

    APP_ERROR ObjectDetectionOutput(const std::vector<TensorBase> &tensors,
                                    std::vector<std::vector<ObjectInfo>> &objectInfos);

    uint32_t GetTensorStrides(const TensorBase& tensor);

    const int DEFAULT_OBJECT_NUM_TENSOR = 0;
    const int DEFAULT_CONFIDENCE_TENSOR = 1;
    const int DEFAULT_BBOX_TENSOR = 2;
    const int DEFAULT_CLASSID_TENSOR = 3;

    uint32_t objectNumTensor_ = DEFAULT_OBJECT_NUM_TENSOR;
    uint32_t confidenceTensor_ = DEFAULT_CONFIDENCE_TENSOR;
    uint32_t bboxTensor_ = DEFAULT_BBOX_TENSOR;
    uint32_t classIdTensor_ = DEFAULT_CLASSID_TENSOR;

    SsdMobilenetv1FpnPostProcess* qPtr_ = nullptr;
};

SsdMobilenetv1FpnPostProcessDptr::SsdMobilenetv1FpnPostProcessDptr(
    SsdMobilenetv1FpnPostProcess *pSsdMobilenetv1FpnPostProcess)
    : qPtr_(pSsdMobilenetv1FpnPostProcess)
{}

SsdMobilenetv1FpnPostProcessDptr::SsdMobilenetv1FpnPostProcessDptr(const SsdMobilenetv1FpnPostProcessDptr &other)
{
    *this = other;
}

SsdMobilenetv1FpnPostProcessDptr& SsdMobilenetv1FpnPostProcessDptr::operator=(
    const SsdMobilenetv1FpnPostProcessDptr &other)
{
    if (this == &other) {
        return *this;
    }
    objectNumTensor_ = other.objectNumTensor_;
    confidenceTensor_ = other.confidenceTensor_;
    bboxTensor_ = other.bboxTensor_;
    classIdTensor_ = other.classIdTensor_;
    return *this;
}

APP_ERROR SsdMobilenetv1FpnPostProcessDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "Input tensors are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return qPtr_->CheckAndMoveTensors(tensors);
}

bool SsdMobilenetv1FpnPostProcessDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    for (size_t i = 0; i < tensors.size(); i++) {
        if (tensors[i].GetDataTypeSize() != qPtr_->FOUR_BYTE) {
            LogError << "The tensor type(" << TensorDataTypeStr[tensors[i].GetTensorType()]
                     << ") mismatched. requires(" << qPtr_->FOUR_BYTE << ") bytes tensortype."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
    if (tensors.size() < 0x4) {
        LogError << "The number of tensors (" << tensors.size() << ") " << "is less than required ("
                 << 0x4 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto bboxTensorShape = tensors[bboxTensor_].GetShape();
    if (bboxTensorShape.size() != BBOX_POOLSIZE) {
        LogError << "The number of tensor[bboxTensor_] dimensions (" << bboxTensorShape.size() << ") " << "is not equal to ("
                 << BBOX_POOLSIZE << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (bboxTensorShape[0x2] != BOX_DIM) {
        LogError << "The dimension of tensor[bboxTensor_][2] (" << bboxTensorShape[0x2] << ") " << "is not equal to ("
                 << BOX_DIM << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto objectNumTensorShape = tensors[objectNumTensor_].GetShape();
    auto confidenceTensorShape = tensors[confidenceTensor_].GetShape();
    auto classIdTensorShape = tensors[classIdTensor_].GetShape();
    if (objectNumTensorShape.size() < 1 || confidenceTensorShape.size() < 1 || classIdTensorShape.size() < 1 ||
        objectNumTensorShape[0] != confidenceTensorShape[0] || objectNumTensorShape[0] != classIdTensorShape[0] ||
        objectNumTensorShape[0] != bboxTensorShape[0]) {
        LogError << "The tensors shape is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

uint32_t SsdMobilenetv1FpnPostProcessDptr::GetTensorStrides(const TensorBase& tensor)
{
    auto shape = tensor.GetShape();
    uint32_t length = 1;
    for (size_t i = 1; i < shape.size(); i++) {
        length *= shape[i];
    }
    return length;
}

APP_ERROR SsdMobilenetv1FpnPostProcessDptr::ObjectDetectionOutput(const std::vector<TensorBase>& tensors,
    std::vector<std::vector<ObjectInfo>>& objectInfos)
{
    LogDebug << "SsdMobilenetv1FpnPostProcessor start to write results.";
    for (auto num : {objectNumTensor_, confidenceTensor_, bboxTensor_, classIdTensor_}) {
        if (num >= tensors.size()) {
            LogError << "TENSOR(" << num << ") must be less than tensors'size(" << tensors.size()
                     << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    uint32_t batchSize = tensors[objectNumTensor_].GetShape()[0];
    uint32_t bboxLen = GetTensorStrides(tensors[bboxTensor_]);
    uint32_t classIdLen =  GetTensorStrides(tensors[classIdTensor_]);
    uint32_t confidenceLen = GetTensorStrides(tensors[confidenceTensor_]);
    for (uint32_t i = 0; i < batchSize; i++) {
        std::vector<ObjectInfo> objectInfo;
        float *detectedNumber = (float*) qPtr_->GetBuffer(tensors[objectNumTensor_], i);
        float *confidencePtr = (float*) qPtr_->GetBuffer(tensors[confidenceTensor_], i);
        float *bboxPtr = (float*) qPtr_->GetBuffer(tensors[bboxTensor_], i);
        float *classIdPtr = (float*) qPtr_->GetBuffer(tensors[classIdTensor_], i);
        if (detectedNumber == nullptr || confidencePtr == nullptr || bboxPtr == nullptr || classIdPtr == nullptr) {
            LogError << "The detectedNumber or confidencePtr or bboxPtr "
                     << "or classIdPtr is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        if ((int)*detectedNumber > MAX_DETECT_NUM) {
            LogWarn << "Detected objects' number is more than 100, check ScoreTresh value.";
        }
        for (size_t j = 0; j < (size_t)*detectedNumber && j < MAX_DETECT_NUM; j++) {
            if (j >= (batchSize - i) * classIdLen || j >= (batchSize - i) * confidenceLen) {
                continue;
            }
            int classId = static_cast<int>(classIdPtr[j]);
            if (classId < 0 || classId >= (int)qPtr_->classNum_) {
                continue;
            }
            if (confidencePtr[j] < qPtr_->separateScoreThresh_[classId]) {
                continue;
            }
            ObjectInfo objInfo;
            objInfo.classId = classId;
            objInfo.confidence = confidencePtr[j];
            objInfo.className = qPtr_->configData_.GetClassName((size_t)objInfo.classId);
            if (j * BOX_DIM + RIGHTBOTX >= (batchSize - i) * bboxLen) {
                continue;
            }
            objInfo.x0  = bboxPtr[j * BOX_DIM + LEFTTOPX];
            objInfo.y0  = bboxPtr[j * BOX_DIM + LEFTTOPY];
            objInfo.x1 = bboxPtr[j * BOX_DIM + RIGHTBOTX];
            objInfo.y1 = bboxPtr[j * BOX_DIM + RIGHTBOTY];
            objectInfo.push_back(objInfo);
        }
        objectInfos.push_back(objectInfo);
    }
    LogDebug << "SsdMobilenetv1FpnPostProcessor write results successed.";
    return APP_ERR_OK;
}
}
#endif
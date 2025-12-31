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
 * Description: SsdMobilenetFpnMindsporePost private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXVISION_SSDMOBILENETFPN_MINDSPOREPOST_DPTR_H
#define MXVISION_SSDMOBILENETFPN_MINDSPOREPOST_DPTR_H

#include <algorithm>
#include <vector>
#include <map>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/CV/Core/DataType.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "ObjectPostProcessors/SsdMobilenetFpnMindsporePost.h"

namespace MxBase {
    const int LEFTTOPY  = 0;
    const int LEFTTOPX  = 1;
    const int RIGHTBOTY = 2;
    const int RIGHTBOTX = 3;
    const float FLOAT_AVERAGE = 2.f;
    const int MAX_BOX_PARAMETER = 8192;
}

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER SsdMobilenetFpnMindsporePostDptr {
public:
    explicit SsdMobilenetFpnMindsporePostDptr(SsdMobilenetFpnMindsporePost* pThis);

    SsdMobilenetFpnMindsporePostDptr(const SsdMobilenetFpnMindsporePostDptr &other);

    ~SsdMobilenetFpnMindsporePostDptr() = default;

    SsdMobilenetFpnMindsporePostDptr &operator=(const SsdMobilenetFpnMindsporePostDptr &other);

    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;

    APP_ERROR ObjectDetectionOutput(const std::vector<TensorBase> &tensors,
                                    std::vector<std::vector<ObjectInfo>> &objectInfos,
                                    const std::vector<ResizedImageInfo> &resizedImageInfos,
                                    std::vector<MxBase::CropRoiBox> cropRoiBoxes);
    APP_ERROR NonMaxSuppression(std::vector<MxBase::DetectBox>& detBoxes,
        const std::vector<TensorBase> &tensors, const ResizedImageInfo &imgInfo, uint32_t batchNum);
    void NmsSort(std::vector<DetectBox>& detBoxes, float iouThresh, IOUMethod method);
    void FilterByIou(std::vector<DetectBox> dets, std::vector<DetectBox>& sortBoxes, float iouThresh, IOUMethod method);
    float CalcIou(const DetectBox& boxA, const DetectBox& boxB, IOUMethod method);
    bool GetArea(const DetectBox& boxA, const DetectBox& boxB, float& area);
    const float DEFAULT_IOU_THRESH = 0.6;
    const int DEFAULT_OBJECT_BBOX_TENSOR = 0;
    const int DEFAULT_OBJECT_CONFIDENCE_TENSOR = 1;
    const int DEFAULT_MAX_BBOX_PER_CLASS = 100;

    float iouThresh_ = DEFAULT_IOU_THRESH;
    int objectBboxTensor_ = DEFAULT_OBJECT_BBOX_TENSOR;
    int objectConfidenceTensor_ = DEFAULT_OBJECT_CONFIDENCE_TENSOR;
    int maxBboxPerClass_ = DEFAULT_MAX_BBOX_PER_CLASS;
    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);
    SsdMobilenetFpnMindsporePost* qPtr_ = nullptr;
};

SsdMobilenetFpnMindsporePostDptr::SsdMobilenetFpnMindsporePostDptr(
    SsdMobilenetFpnMindsporePost *pThis)
    : qPtr_(pThis)
{}

SsdMobilenetFpnMindsporePostDptr::SsdMobilenetFpnMindsporePostDptr(const SsdMobilenetFpnMindsporePostDptr &other)
{
    *this = other;
}

SsdMobilenetFpnMindsporePostDptr& SsdMobilenetFpnMindsporePostDptr::operator=(
    const SsdMobilenetFpnMindsporePostDptr &other)
{
    if (this == &other) {
        return *this;
    }
    iouThresh_ =  other.iouThresh_;
    objectBboxTensor_ = other.objectBboxTensor_;
    objectConfidenceTensor_ = other.objectConfidenceTensor_;
    maxBboxPerClass_ = other.maxBboxPerClass_;
    return *this;
}


bool SsdMobilenetFpnMindsporePostDptr::GetArea(const DetectBox& boxA, const DetectBox& boxB, float& area)
{
    if ((int)boxA.x < 0 || (int)boxA.x > MAX_BOX_PARAMETER ||
        (int)boxB.y < 0 || (int)boxB.y > MAX_BOX_PARAMETER ||
        (int)boxA.width < 0 || (int)boxA.width > MAX_BOX_PARAMETER ||
        (int)boxA.height < 0 || (int)boxA.height > MAX_BOX_PARAMETER) {
        return false;
    }
    float left = std::max(boxA.x - boxA.width / FLOAT_AVERAGE, boxB.x - boxB.width / FLOAT_AVERAGE);
    float right = std::min(boxA.x + boxA.width / FLOAT_AVERAGE, boxB.x + boxB.width / FLOAT_AVERAGE);
    float top = std::max(boxA.y - boxA.height / FLOAT_AVERAGE, boxB.y - boxB.height / FLOAT_AVERAGE);
    float bottom = std::min(boxA.y + boxA.height / FLOAT_AVERAGE, boxB.y + boxB.height / FLOAT_AVERAGE);
    if (top > bottom || left > right) { // If no intersection
        return false;
    }
    area = (right - left) * (bottom - top);
    return true;
}

float SsdMobilenetFpnMindsporePostDptr::CalcIou(const DetectBox& boxA, const DetectBox& boxB, IOUMethod method)
{
    // intersection / union
    float area = 0.0f;
    if (!GetArea(boxA, boxB, area)) {
        return 0.0f;
    }
    if (method == IOUMethod::MAX) {
        auto denominator = std::max(boxA.width * boxA.height, boxB.width * boxB.height);
        if (IsDenominatorZero(denominator)) {
            return 0.0f;
        }
        return area / denominator;
    }
    if (method == IOUMethod::MIN) {
        auto denominator = std::min(boxA.width * boxA.height, boxB.width * boxB.height);
        if (IsDenominatorZero(denominator)) {
            return 0.0f;
        }
        return area / denominator;
    }
    float checkValue = ((boxA.width + 1) * (boxA.height + 1)  + (boxB.width + 1) * (boxB.height + 1) - area);
    if (IsDenominatorZero(checkValue)) {
        LogError << "The checkValue is " << checkValue << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return 0.0f;
    }
    return area / ((boxA.width + 1) * (boxA.height + 1)  + (boxB.width + 1) * (boxB.height + 1) - area);
}

void SsdMobilenetFpnMindsporePostDptr::FilterByIou(std::vector<DetectBox> dets,
    std::vector<DetectBox>& sortBoxes, float iouThresh, IOUMethod method)
{
    int count = 0;
    for (size_t m = 0; m < dets.size(); ++m) {
        auto& item = dets[m];
        sortBoxes.push_back(item);
        count++;
        if (count >= maxBboxPerClass_) {
            break;
        }
        for (size_t n = m + 1; n < dets.size(); ++n) {
            if (CalcIou(item, dets[n], method) > iouThresh) {
                dets.erase(dets.begin() + n);
                --n;
            }
        }
    }
}

void SsdMobilenetFpnMindsporePostDptr::NmsSort(std::vector<DetectBox>& detBoxes, float iouThresh, IOUMethod method)
{
    std::vector<DetectBox> sortBoxes;
    std::map<int, std::vector<DetectBox>> resClassMap;
    for (const auto& item : detBoxes) {
        resClassMap[item.classID].push_back(item);
    }

    std::map<int, std::vector<DetectBox>>::iterator iter;
    for (iter = resClassMap.begin(); iter != resClassMap.end(); ++iter) {
        std::sort(iter->second.begin(), iter->second.end(), [=](const DetectBox& a, const DetectBox& b) {
            return a.prob < b.prob;
        });
        std::reverse(iter->second.begin(), iter->second.end());
        FilterByIou(iter->second, sortBoxes, iouThresh, method);
    }
    detBoxes = std::move(sortBoxes);
}

APP_ERROR SsdMobilenetFpnMindsporePostDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "Input tensors are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return qPtr_->CheckAndMoveTensors(tensors);
}

bool SsdMobilenetFpnMindsporePostDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    // model : ssd_mobilenet_fpn_mindspore.om
    for (size_t i = 0; i < tensors.size(); i++) {
        if (tensors[i].GetDataTypeSize() != qPtr_->FOUR_BYTE) {
            LogError << "The tensor type(" << TensorDataTypeStr[tensors[i].GetTensorType()]
                     << ") mismatched. requires(" << qPtr_->FOUR_BYTE
                     << ") bytes tensortype." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
    if (tensors.size() < 0x2) {
        LogError << "The number of tensors (" << tensors.size() << ") " << "is less than required ("
                 << 0x2 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto shape = tensors[0].GetShape();
    if (shape.size() != 0x3) {
        LogError << "The number of tensor[0] dimensions (" << shape.size() << ") " << "is not equal to ("
                 << 0x3 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (shape[0x2] != BOX_DIM) {
        LogError << "The dimension of tensor[0][2] (" << shape[0x2] << ") " << "is not equal to ("
                 << BOX_DIM << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    shape = tensors[1].GetShape();
    if (shape.size() != 0x3) {
        LogError << "The number of tensor[1] dimensions (" << shape.size() << ") " << "is not equal to ("
                 << 0x3 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (shape[0x2] != (uint32_t)qPtr_->classNum_) {
        LogError << "The dimension of tensor[1][2] (" << shape[0x2] << ") " << "is not equal to ("
                 << qPtr_->classNum_ << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (tensors[objectBboxTensor_].GetShape()[0] != shape[0]) {
        LogError << "The batchSize of tensor[1] (" << tensors[objectBboxTensor_].GetShape()[0] << ") "
                 << "is not equal to tensor[0] (" << shape[0]<< ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

APP_ERROR SsdMobilenetFpnMindsporePostDptr::NonMaxSuppression(std::vector<MxBase::DetectBox>& detBoxes,
    const std::vector<TensorBase> &tensors, const ResizedImageInfo &imgInfo, uint32_t batchNum)
{
    // Find the detection boxes with confidence scores greater than the score threshold.

    auto shape = tensors[objectConfidenceTensor_].GetShape();
    auto bboxTensor = tensors[objectBboxTensor_];
    auto confidenceTensor = tensors[objectConfidenceTensor_];
    int stride = (int)shape[0x1];

    float *objectBboxPtr = (float *) qPtr_->GetBuffer(bboxTensor, batchNum);
    float *objectConfidencePtr = (float *) qPtr_->GetBuffer(confidenceTensor, batchNum);
    if (objectBboxPtr == nullptr || objectConfidencePtr == nullptr) {
        LogError << "The objectBboxPtr or objectConfidencePtr is nullptr."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    uint32_t batchSize = shape[0];
    uint32_t objectConfLen =  confidenceTensor.GetSize() / batchSize;
    uint32_t objectBboxLen = bboxTensor.GetSize() / batchSize;
    for (size_t k = 1; k < static_cast<size_t>(qPtr_->classNum_); k++) {
        for (size_t j = 0; j < static_cast<size_t>(stride); j++) {
            if (j * qPtr_->classNum_ + k >= objectConfLen || j * BOX_DIM + RIGHTBOTX >= objectBboxLen) {
                continue;
            }
            if (objectConfidencePtr[j * qPtr_->classNum_ + k]
                <= qPtr_->separateScoreThresh_[k]) {
                continue;
            }
            MxBase::DetectBox det;
            det.classID = static_cast<int>(k);
            det.prob = objectConfidencePtr[j * qPtr_->classNum_ + k];
            float x1 = objectBboxPtr[j * BOX_DIM + LEFTTOPX] * imgInfo.widthOriginal;
            float y1 = objectBboxPtr[j * BOX_DIM + LEFTTOPY] * imgInfo.heightOriginal;
            float x2 = objectBboxPtr[j * BOX_DIM + RIGHTBOTX] * imgInfo.widthOriginal;
            float y2 = objectBboxPtr[j * BOX_DIM + RIGHTBOTY] * imgInfo.heightOriginal;
            det.x = (x1 + x2) / COORDINATE_PARAM;
            det.y = (y1 + y2) / COORDINATE_PARAM;
            det.width = (x2 - x1 > std::numeric_limits<float>::epsilon()) ? (x2 - x1) : (x1 - x2);
            det.height = (y2 - y1 > std::numeric_limits<float>::epsilon()) ? (y2 - y1) : (y1 - y2);
            detBoxes.emplace_back(det);
        }
    }
    NmsSort(detBoxes, iouThresh_, UNION);
    return APP_ERR_OK;
}

APP_ERROR SsdMobilenetFpnMindsporePostDptr::ObjectDetectionOutput(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<ObjectInfo>> &objectInfos, const std::vector<ResizedImageInfo> &resizedImageInfos,
    std::vector<MxBase::CropRoiBox> cropRoiBoxes)
{
    LogDebug << "SsdMobilenetFpnMindsporePost start to write results.";
    std::vector<MxBase::DetectBox> detBoxes;
    auto shape = tensors[objectConfidenceTensor_].GetShape();
    uint32_t batchSize = shape[0x0];

    for (uint32_t i = 0; i < batchSize; i++) {
        ResizedImageInfo imageInfo = resizedImageInfos[i];
        if (i < cropRoiBoxes.size()) {
            imageInfo.widthOriginal = (uint32_t)(cropRoiBoxes[i].x1 - cropRoiBoxes[i].x0);
            imageInfo.heightOriginal = (uint32_t)(cropRoiBoxes[i].y1 - cropRoiBoxes[i].y0);
        }
        std::vector<MxBase::DetectBox> detBoxes;
        APP_ERROR ret = NonMaxSuppression(detBoxes, tensors, imageInfo, i);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        std::vector<ObjectInfo> objectInfos1;
        for (size_t k = 0; k < detBoxes.size(); k++) {
            ObjectInfo objInfo;
            objInfo.classId = detBoxes[k].classID;
            objInfo.confidence = detBoxes[k].prob;
            objInfo.x0 = ((float) (detBoxes[k].x - detBoxes[k].width / COORDINATE_PARAM)
                         > std::numeric_limits<float>::epsilon()) ?
                         (float) ((detBoxes[k].x - detBoxes[k].width / COORDINATE_PARAM)) : 0;

            objInfo.y0 = ((float) (detBoxes[k].y - detBoxes[k].height / COORDINATE_PARAM)
                         > std::numeric_limits<float>::epsilon()) ?
                         (float) ((detBoxes[k].y - detBoxes[k].height / COORDINATE_PARAM)) : 0;

            objInfo.x1 = ((detBoxes[k].x + detBoxes[k].width / COORDINATE_PARAM) <=
                          resizedImageInfos[i].widthOriginal) ?
                         (float) ((detBoxes[k].x + detBoxes[k].width / COORDINATE_PARAM)) :
                         resizedImageInfos[i].widthOriginal;

            objInfo.y1 = ((detBoxes[k].y + detBoxes[k].height / COORDINATE_PARAM) <=
                          resizedImageInfos[i].heightOriginal) ?
                         (float) ((detBoxes[k].y + detBoxes[k].height / COORDINATE_PARAM)) :
                         resizedImageInfos[i].heightOriginal;
            if (i < cropRoiBoxes.size()) {
                objInfo.x0 += cropRoiBoxes[i].x0;
                objInfo.x1 += cropRoiBoxes[i].x0;
                objInfo.y0 += cropRoiBoxes[i].y0;
                objInfo.y1 += cropRoiBoxes[i].y0;
            }
            objInfo.className = qPtr_->configData_.GetClassName((size_t)objInfo.classId);

            objectInfos1.push_back(objInfo);
        }
        objectInfos.push_back(objectInfos1);
    }
    LogDebug << "SsdMobilenetFpnMindsporePost write results successed.";
    return APP_ERR_OK;
}
}
#endif // MXVISION_SsdMobilenetFpnMindsporePost_H

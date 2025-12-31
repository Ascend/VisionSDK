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
 * Description: FasterRcnnPostProcess private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef FASTER_RCNN_POST_PROCESS_DPTR_H
#define FASTER_RCNN_POST_PROCESS_DPTR_H

#include "ObjectPostProcessors/FasterRcnnPostProcess.h"
#include "MxBase/CV/ObjectDetection/Nms/Nms.h"
#include "MxBase/Log/Log.h"
#include "acl/acl.h"

namespace MxBase {
    const uint32_t BBOX_POOLSIZE = 3;
    const int DETECT_NUM = 100;
    const int OUTPUT_TENSOR_SIZE = 3;
    const int OUTPUT_BBOX_SIZE = 3;
    const int OUTPUT_BBOX_TWO_INDEX_SHAPE = 5;
    const int OUTPUT_BBOX_INDEX = 0;
    const int OUTPUT_CLASS_INDEX = 1;
    const int OUTPUT_MASK_INDEX = 2;
    const uint32_t DEFAULT_RPN_MAX_NUM_MS = 1000;
    const uint32_t DEFAULT_MAX_PER_IMG_MS = 128;

    const int LEFTTOPY  = 0;
    const int LEFTTOPX  = 1;
    const int RIGHTBOTY = 2;
    const int RIGHTBOTX = 3;
    const int CONFINDENCE = 4;

    const int BBOX_INDEX_LX = 0;
    const int BBOX_INDEX_LY = 1;
    const int BBOX_INDEX_RX = 2;
    const int BBOX_INDEX_RY = 3;
    const int BBOX_INDEX_SCALE_NUM = 5;

    const int HWC_H_DIM = 0;
    const int HWC_W_DIM = 1;
    const int HWC_C_DIM = 2;
    const int NHWC_H_DIM = 1;
    const int NHWC_W_DIM = 2;
    const int NHWC_C_DIM = 3;
}

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER FasterRcnnPostProcessDptr {
public:
    explicit FasterRcnnPostProcessDptr(FasterRcnnPostProcess *pFasterRcnnPostProcess);

    FasterRcnnPostProcessDptr(const FasterRcnnPostProcessDptr &other);

    ~FasterRcnnPostProcessDptr() = default;

    FasterRcnnPostProcessDptr &operator=(const FasterRcnnPostProcessDptr &other);

    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;

    bool CheckPrivateTensors(const std::vector<TensorBase> &tensors) const;

    bool IsValidTensorsForOriginal(const std::vector<TensorBase> &tensors) const;

    bool IsValidTensorsForFPN(const std::vector<TensorBase> &tensors) const;

    bool CheckTensors(const std::vector<TensorBase> &tensors) const;

    bool IsValidTensorsForNmsCut(const std::vector<TensorBase> &tensors) const;

    bool IsValidTensorsForMS(const std::vector<TensorBase> &tensors) const;

    bool CheckBboxTensorsForOriginal(const std::vector<uint32_t> &bboxTensorShape) const;

    bool CheckTensorIndex(const std::vector<TensorBase> &tensors) const;

    APP_ERROR ObjectDetectionOutput(const std::vector<TensorBase> &tensors,
                                    std::vector<std::vector<ObjectInfo>> &objectInfos);
    APP_ERROR OriginalOutput(const std::vector<TensorBase>& inputs, std::vector<std::vector<ObjectInfo>>& objectInfos);

    APP_ERROR NmsCutOutput(const std::vector<TensorBase>& inputs, std::vector<std::vector<ObjectInfo>>& objectInfos);

    APP_ERROR FpnOutput(const std::vector<TensorBase>& inputs, std::vector<std::vector<ObjectInfo>>& objectInfos);

    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);

    APP_ERROR GetObjectInfo(const std::vector<TensorBase> &tensors, uint32_t i, std::vector<ObjectInfo> &objectInfo);

    APP_ERROR ObjectDetectionOutputForMS(const std::vector<TensorBase> &tensors,
        std::vector<std::vector<ObjectInfo>> &objectInfos, const std::vector<ResizedImageInfo> &resizedImageInfos);

    APP_ERROR GetValidDetBoxes(const std::vector<TensorBase> &tensors, std::vector<DetectBox> &detBoxes,
        uint32_t batchNum);

    void ConvertObjInfoFromDetectBox(std::vector<DetectBox> &detBoxes, std::vector<ObjectInfo> &objectInfos,
        const ResizedImageInfo &resizedImageInfos);

    APP_ERROR GetFasterRcnnConfig();

    APP_ERROR GetTensorsObjectInfos(const std::vector<TensorBase>& inputs,
                                    std::vector<std::vector<ObjectInfo>>& objectInfos,
                                    const std::vector<ResizedImageInfo>& resizedImageInfos, bool& normalizedFlag);
    const uint32_t DEFAULT_OBJECT_NUM_TENSOR = 0;
    const uint32_t DEFAULT_BBOX_TENSOR = 1;
    const uint32_t DEFAULT_CONFIDENCE_TENSOR = 2;
    const uint32_t DEFAULT_CONFIDENCE_UNCUT_TENSOR = 1;
    const uint32_t DEFAULT_BBOX_UNCUT_TENSOR = 2;
    const uint32_t DEFAULT_CLASSID_UNCUT_TENSOR = 3;
    const uint32_t FPN_BOX_DIM = 5;

    enum ModelType {
        ORIGINAL = 0,
        NMS_CUT = 1,
        FPN = 2
    };

public:
    int modelType_ = 0;
    float iouThresh_ = 1.0;
    uint32_t bboxCutTensor_ = DEFAULT_BBOX_TENSOR;
    uint32_t confidenceCutTensor_ = DEFAULT_CONFIDENCE_TENSOR;
    uint32_t objectNumTensor_ = DEFAULT_OBJECT_NUM_TENSOR;
    uint32_t confidenceTensor_ = DEFAULT_CONFIDENCE_UNCUT_TENSOR;
    uint32_t bboxTensor_ = DEFAULT_BBOX_UNCUT_TENSOR;
    uint32_t classIdTensor_ = DEFAULT_CLASSID_UNCUT_TENSOR;
    uint32_t rpnMaxNum_ = DEFAULT_RPN_MAX_NUM_MS;
    uint32_t maxPerImg_ = DEFAULT_MAX_PER_IMG_MS;
    std::string framework_ = "pytorch";

public:
    FasterRcnnPostProcess* qPtr_ = nullptr;
};

FasterRcnnPostProcessDptr::FasterRcnnPostProcessDptr(FasterRcnnPostProcess *pFasterRcnnPostProcess)
    : qPtr_(pFasterRcnnPostProcess)
{}

FasterRcnnPostProcessDptr::FasterRcnnPostProcessDptr(const FasterRcnnPostProcessDptr &other)
{
    *this = other;
}

FasterRcnnPostProcessDptr &FasterRcnnPostProcessDptr::operator=(const FasterRcnnPostProcessDptr &other)
{
    if (this == &other) {
        return *this;
    }
    iouThresh_ = other.iouThresh_;
    bboxCutTensor_ = other.bboxCutTensor_;
    confidenceCutTensor_ = other.confidenceCutTensor_;
    objectNumTensor_ = other.objectNumTensor_;
    confidenceTensor_ = other.confidenceCutTensor_;
    bboxTensor_ = other.bboxTensor_;
    classIdTensor_ = other.classIdTensor_;
    return *this;
}

bool FasterRcnnPostProcessDptr::CheckBboxTensorsForOriginal(const std::vector<uint32_t> &bboxTensorShape) const
{
    if (bboxTensorShape.size() != BBOX_POOLSIZE) {
        LogError << "The number of tensor[" << bboxTensor_ << "] dimensions (" << bboxTensorShape.size()
                 << ") " << "is not equal to (" << BBOX_POOLSIZE << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (bboxTensorShape[HWC_W_DIM] != DETECT_NUM) {
        LogError << "The dimension of tensor[" << bboxTensor_ << "][" << HWC_W_DIM << "] ("
                 << bboxTensorShape[HWC_W_DIM] << ") " << "is not equal to (" << DETECT_NUM << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (bboxTensorShape[HWC_C_DIM] != BOX_DIM) {
        LogError << "The dimension of tensor[" << bboxTensor_ << "][" << HWC_C_DIM << "] ("
                 << bboxTensorShape[HWC_C_DIM] << ") " << "is not equal to (" << BOX_DIM << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool FasterRcnnPostProcessDptr::CheckPrivateTensors(const std::vector<TensorBase> &tensors) const
{
    if (tensors.size() < 0x4) {
        LogError << "The number of tensors (" << tensors.size() << ") " << "is less than required ("
                 << 0x4 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (objectNumTensor_ >= tensors.size()) {
        LogError << "TENSOR(" << objectNumTensor_ << ") must be less than tensors'size(" << 0x4
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (confidenceTensor_ >= tensors.size()) {
        LogError << "TENSOR(" << confidenceTensor_ << ") must be less than tensors'size(" << 0x4
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (bboxTensor_ >= tensors.size()) {
        LogError << "TENSOR(" << bboxTensor_ << ") must be less than tensors'size(" << 0x4
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (classIdTensor_ >= tensors.size()) {
        LogError << "TENSOR(" << classIdTensor_ << ") must be less than tensors'size(" << 0x4
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool FasterRcnnPostProcessDptr::IsValidTensorsForOriginal(const std::vector<TensorBase> &tensors) const
{
    // model: faster_rcnn_coco_uncut.om
    for (size_t i = 0; i < tensors.size(); i++) {
        if (tensors[i].GetDataTypeSize() != qPtr_->FOUR_BYTE) {
            LogError << "The tensor type(" << TensorDataTypeStr[tensors[i].GetTensorType()]
                     << ") mismatched. required(" << qPtr_->FOUR_BYTE << ") bytes tensortype."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        } else if (tensors[i].GetShape().size() <= 0) {
            LogError << "The tensors shape is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
    if (!CheckPrivateTensors(tensors)) {
        return false;
    }
    auto objectNumTensorShape = tensors[objectNumTensor_].GetShape();
    auto confidenceTensorShape = tensors[confidenceTensor_].GetShape();
    auto bboxTensorShape = tensors[bboxTensor_].GetShape();
    auto classIdTensorShape = tensors[classIdTensor_].GetShape();
    if (objectNumTensorShape[0] != confidenceTensorShape[0] || objectNumTensorShape[0] != bboxTensorShape[0]
        || objectNumTensorShape[0] != classIdTensorShape[0]) {
        LogError << "The tensors' batchSize is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return CheckBboxTensorsForOriginal(bboxTensorShape);
}

APP_ERROR FasterRcnnPostProcessDptr::GetFasterRcnnConfig()
{
    qPtr_->configData_.GetFileValueWarn<std::string>("FRAMEWORK", framework_);
    std::transform(framework_.begin(), framework_.end(), framework_.begin(), ::tolower);
    APP_ERROR ret = qPtr_->configData_.GetFileValue<float>("IOU_THRESH", iouThresh_, 0.0f, 1.0f);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read IOU_THRESH from config, default value(1.0) will be used as iouThresh_.";
    }
    ret = qPtr_->configData_.GetFileValue<int>("MODEL_TYPE", modelType_, 0x0, 0x3e8);
    if (ret != APP_ERR_OK) {
        if (framework_ == "mindspore") {
            modelType_ = NMS_CUT;
        }
        LogWarn << GetErrorInfo(ret)
                << "Fail to read MODEL_TYPE from config, default value(0) will be used as modelType_.";
    }
    qPtr_->classNum_ -= (modelType_ == NMS_CUT);
    qPtr_->configData_.GetFileValueWarn<uint32_t>("BBOX_TENSOR", bboxCutTensor_, 0x0, 0x1388);
    qPtr_->configData_.GetFileValueWarn<uint32_t>("CONFIDENCE_TENSOR", confidenceCutTensor_, 0x0, 0x1388);
    qPtr_->configData_.GetFileValueWarn<uint32_t>("OBJECT_NUM_TENSOR", objectNumTensor_, 0x0, 0x1388);
    qPtr_->configData_.GetFileValueWarn<uint32_t>("CONFIDENCE_UNCUT_TENSOR", confidenceTensor_, 0x0, 0x1388);
    qPtr_->configData_.GetFileValueWarn<uint32_t>("BBOX_UNCUT_TENSOR", bboxTensor_, 0x0, 0x1388);
    qPtr_->configData_.GetFileValueWarn<uint32_t>("CLASSID_UNCUT_TENSOR", classIdTensor_, 0x0, 0x1388);
    qPtr_->configData_.GetFileValueWarn<uint32_t>("RPN_MAX_NUM", rpnMaxNum_, 0x0, 0x1388);
    qPtr_->configData_.GetFileValueWarn<uint32_t>("MAX_PER_IMG", maxPerImg_, 0x0, 0x1388);
    return APP_ERR_OK;
}

bool FasterRcnnPostProcessDptr::CheckTensors(const std::vector<TensorBase> &tensors) const
{
    if (objectNumTensor_ >= tensors.size()) {
        LogError << "TENSOR(" << objectNumTensor_ << ") must be less than tensors'size(" << 0x4
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (bboxCutTensor_ >= tensors.size()) {
        LogError << "TENSOR(" << bboxCutTensor_ << ") must be less than tensors'size(" << 0x4
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (confidenceCutTensor_ >= tensors.size()) {
        LogError << "TENSOR(" << confidenceCutTensor_ << ") must be less than tensors'size(" << 0x4
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool FasterRcnnPostProcessDptr::IsValidTensorsForNmsCut(const std::vector<TensorBase> &tensors) const
{
    // model : faster_rcnn_coco_cut.om
    if (tensors.size() < 0x3) {
        LogError << "The number of tensors(" << tensors.size() << ") " << "is less than required ("
                 << 0x3 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    for (size_t i = 0; i < tensors.size(); i++) {
        if (tensors[i].GetDataTypeSize() != qPtr_->FOUR_BYTE) {
            LogError << "The tensor type(" << TensorDataTypeStr[tensors[i].GetTensorType()]
                     << ") mismatched. required(" << qPtr_->FOUR_BYTE << ") bytes tensortype."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
    if (!CheckTensors(tensors)) {
        return false;
    }

    auto objectNumTensorShape = tensors[objectNumTensor_].GetShape();
    auto bboxCutTensorShape = tensors[bboxCutTensor_].GetShape();
    if (bboxCutTensorShape.size() != 0x4) {
        LogError << "The number of tensor[" << bboxCutTensor_ << "] dimensions (" << bboxCutTensorShape.size() <<  ") "
                 << "is not equal to (" << 0x4 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (bboxCutTensorShape[NHWC_H_DIM] != DETECT_NUM) {
        LogError << "The tensor[" << bboxCutTensor_ << "][" << NHWC_H_DIM << "] != "
                 << DETECT_NUM << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (bboxCutTensorShape[NHWC_W_DIM] != qPtr_->classNum_) {
        LogError << "The tensor[" << bboxCutTensor_ << "][" << NHWC_W_DIM << "] != "
                 << qPtr_->classNum_ << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (bboxCutTensorShape[NHWC_C_DIM] != BOX_DIM) {
        LogError << "The dimension of tensor[" << bboxCutTensor_ << "][" << NHWC_C_DIM << "] ("
                 << bboxCutTensorShape[NHWC_C_DIM] << ") " << "is not equal to (" << BOX_DIM << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto confidenceCutTensorShape = tensors[confidenceCutTensor_].GetShape();
    if (confidenceCutTensorShape[NHWC_H_DIM] != DETECT_NUM) {
        LogError << "The tensor[" << confidenceCutTensor_ << "][" << NHWC_H_DIM << "] != "
                 << DETECT_NUM << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (confidenceCutTensorShape[NHWC_W_DIM] != qPtr_->classNum_) {
        LogError << "The tensor[" << confidenceCutTensor_ << "][" << NHWC_W_DIM << "] != "
                 << qPtr_->classNum_ << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (objectNumTensorShape.size() < 1 || confidenceCutTensorShape.size() < 1 ||
        objectNumTensorShape[0] != bboxCutTensorShape[0] || objectNumTensorShape[0] != confidenceCutTensorShape[0]) {
        LogError << "The tensors shape is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool FasterRcnnPostProcessDptr::IsValidTensorsForFPN(const std::vector<TensorBase> &tensors) const
{
    // model : faster_rcnn_227_go_finally.om
    if (tensors.size() < 0x2) {
        LogError << "The number of tensors(" << tensors.size() << ") " << "is less than required ("
                 << 0x2 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    // check model output tensor
    if (objectNumTensor_ >= tensors.size()) {
        LogError << "TENSOR(" << objectNumTensor_ << ") must be less than tensors'size(" << 0x2
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (bboxCutTensor_ >= tensors.size()) {
        LogError << "TENSOR(" << bboxCutTensor_ << ") must be less than tensors'size(" << 0x2
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto shape = tensors[objectNumTensor_].GetShape();
    auto shape2 = tensors[bboxCutTensor_].GetShape();
    if (shape.size() != DEFAULT_CONFIDENCE_TENSOR) {
        LogError << "The tensors[0].GetShape().size() == " << shape.size() << ", but not equal to "
                 << DEFAULT_CONFIDENCE_TENSOR << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (shape[1] != FPN_BOX_DIM) {
        LogError << "The tensors[0][" << 1 << "] == " << shape[1]
                 << ", but not equal to " << FPN_BOX_DIM << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (shape2.size() != DEFAULT_BBOX_TENSOR) {
        LogError << "The tensors[1].GetShape().size() !== " << shape2.size() << ", but not equal to "
                 << DEFAULT_BBOX_TENSOR << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool FasterRcnnPostProcessDptr::CheckTensorIndex(const std::vector<TensorBase> &tensors) const
{
    if (tensors.size() < OUTPUT_TENSOR_SIZE) {
        LogError << "The number of tensor (" << tensors.size() << ") is less than required (" << OUTPUT_TENSOR_SIZE
                 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if ((OUTPUT_BBOX_INDEX >= tensors.size()) || (OUTPUT_BBOX_INDEX < 0)) {
        LogError << "TENSOR(" << OUTPUT_BBOX_INDEX << ") must be less than tensors'size(" << 0x2
                 << ") and not less than 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if ((OUTPUT_CLASS_INDEX >= tensors.size()) || (OUTPUT_CLASS_INDEX < 0)) {
        LogError << "TENSOR(" << OUTPUT_CLASS_INDEX << ") must be less than tensors'size(" << 0x2
                 << ") and not less than 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if ((OUTPUT_MASK_INDEX >= tensors.size()) || (OUTPUT_MASK_INDEX < 0)) {
        LogError << "TENSOR(" << OUTPUT_MASK_INDEX << ") must be less than tensors'size(" << 0x2
                 << ") and not less than 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool FasterRcnnPostProcessDptr::IsValidTensorsForMS(const std::vector<TensorBase> &tensors) const
{
    if (!CheckTensorIndex(tensors)) {
        return false;
    }
    auto bboxShape = tensors[OUTPUT_BBOX_INDEX].GetShape();
    if (bboxShape.size() != OUTPUT_BBOX_SIZE) {
        LogError << "The number of tensor[" << OUTPUT_BBOX_INDEX << "] dimensions (" << bboxShape.size()
                 << ") is not equal to (" << OUTPUT_BBOX_SIZE << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    uint32_t totalNum = qPtr_->classNum_ * rpnMaxNum_;
    if (bboxShape[0x1] != totalNum) {
        LogError << "The output tensor is mismatched: " << totalNum << "/" << bboxShape[0x1] << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (bboxShape[0x2] != OUTPUT_BBOX_TWO_INDEX_SHAPE) {
        LogError << "The number of bbox[" << 0x2 << "] dimensions (" << bboxShape[0x2]
                 << ") is not equal to (" << OUTPUT_BBOX_TWO_INDEX_SHAPE << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto classShape = tensors[OUTPUT_CLASS_INDEX].GetShape();
    auto maskShape = tensors[OUTPUT_MASK_INDEX].GetShape();
    if (classShape.size() < 2 || maskShape.size() < 2 || classShape[0] != maskShape[0] ||
        classShape[0] != bboxShape[0]) {
        LogError << "The tensors shape is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
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
    return true;
}

APP_ERROR FasterRcnnPostProcessDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "The input tensors are invalid."<< GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return qPtr_->CheckAndMoveTensors(tensors);
}


bool FasterRcnnPostProcessDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    if (framework_ == "mindspore") {
        return IsValidTensorsForMS(tensors);
    } else {
        if (modelType_ == FasterRcnnPostProcessDptr::ORIGINAL) {
            return IsValidTensorsForOriginal(tensors);
        } else if (modelType_ == NMS_CUT) {
            return IsValidTensorsForNmsCut(tensors);
        } else if (modelType_ == FasterRcnnPostProcessDptr::FPN) {
            return IsValidTensorsForFPN(tensors);
        } else {
            LogError << "The modelType_(" << modelType_ << ") is not supported."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
}

APP_ERROR FasterRcnnPostProcessDptr::ObjectDetectionOutput(const std::vector<TensorBase>&,
    std::vector<std::vector<ObjectInfo>>&)
{
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

void FasterRcnnPostProcessDptr::ConvertObjInfoFromDetectBox(std::vector<DetectBox> &detBoxes,
    std::vector<ObjectInfo> &objectInfos, const ResizedImageInfo &resizedImageInfo)
{
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

        objectInfos.push_back(objInfo);
    }
}

APP_ERROR FasterRcnnPostProcessDptr::GetValidDetBoxes(const std::vector<TensorBase> &tensors,
    std::vector<DetectBox> &detBoxes, uint32_t batchNum)
{
    LogInfo << "Begin to GetValidDetBoxes.";
    auto *bboxPtr = (aclFloat16 *)qPtr_->GetBuffer(tensors[OUTPUT_BBOX_INDEX], batchNum);
    auto *labelPtr = (int32_t *)qPtr_->GetBuffer(tensors[OUTPUT_CLASS_INDEX], batchNum);
    auto *maskPtr = (bool *)qPtr_->GetBuffer(tensors[OUTPUT_MASK_INDEX], batchNum);
    if (bboxPtr == nullptr || labelPtr == nullptr || maskPtr == nullptr) {
        LogError << "The bboxPtr or labelPtr or maskPtr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    // mask filter
    float prob = 0;
    uint32_t batchSize = tensors[bboxCutTensor_].GetShape()[0];
    uint32_t bboxLen = tensors[bboxCutTensor_].GetSize() / batchSize;
    size_t total = (size_t)rpnMaxNum_ * (size_t)qPtr_->classNum_;
    for (size_t index = 0; index < total; ++index) {
        if (!maskPtr[index]) {
            continue;
        }
        size_t startIndex = index * BBOX_INDEX_SCALE_NUM;
        auto classId = labelPtr[index];
        prob = aclFloat16ToFloat(bboxPtr[startIndex + CONFINDENCE]);
        if (prob < qPtr_->separateScoreThresh_[classId]) {
            continue;
        }
        MxBase::DetectBox detBox;
        if (startIndex + BBOX_INDEX_RY >= bboxLen) {
            continue;
        }
        float x1 = aclFloat16ToFloat(bboxPtr[startIndex + BBOX_INDEX_LX]);
        float y1 = aclFloat16ToFloat(bboxPtr[startIndex + BBOX_INDEX_LY]);
        float x2 = aclFloat16ToFloat(bboxPtr[startIndex + BBOX_INDEX_RX]);
        float y2 = aclFloat16ToFloat(bboxPtr[startIndex + BBOX_INDEX_RY]);
        detBox.x = (x1 + x2) / COORDINATE_PARAM;
        detBox.y = (y1 + y2) / COORDINATE_PARAM;
        detBox.width = x2 - x1;
        detBox.height = y2 - y1;
        detBox.prob = prob;
    
        detBox.classID = classId;
        detBoxes.push_back(detBox);
    }
    GetDetectBoxesTopK(detBoxes, maxPerImg_);
    return APP_ERR_OK;
}

APP_ERROR FasterRcnnPostProcessDptr::ObjectDetectionOutputForMS(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<ObjectInfo>> &objectInfos, const std::vector<ResizedImageInfo> &resizedImageInfos)
{
    LogDebug << "FasterRcnnMindsporePost start to write results.";
    auto shape = tensors[OUTPUT_BBOX_INDEX].GetShape();
    uint32_t batchSize = shape[0];
    if (resizedImageInfos.size() < batchSize) {
        LogError << "Check resizedImageInfos size failed, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
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
    LogDebug << "FasterRcnnMindsporePost write results successed.";
    return APP_ERR_OK;
}

APP_ERROR FasterRcnnPostProcessDptr::OriginalOutput(const std::vector<TensorBase>& tensors,
    std::vector<std::vector<ObjectInfo>>& objectInfos)
{
    LogDebug << "OriginalOutput start to write results.";
    for (auto num : {objectNumTensor_, confidenceTensor_, bboxTensor_, classIdTensor_}) {
        if (num >= tensors.size()) {
            LogError << "TENSOR(" << num << ") must be less than tensors'size(" << tensors.size()
                     << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_FAILURE;
        }
    }
    uint32_t batchSize = tensors[objectNumTensor_].GetShape()[0];
    uint32_t bboxLen = tensors[bboxTensor_].GetSize() / batchSize;
    for (uint32_t i = 0; i < batchSize; i++) {
        std::vector<ObjectInfo> objectInfo;
        float *detectedNumber = (float*) qPtr_->GetBuffer(tensors[objectNumTensor_], i);
        float *confidencePtr = (float*) qPtr_->GetBuffer(tensors[confidenceTensor_], i);
        float *bboxPtr = (float*) qPtr_->GetBuffer(tensors[bboxTensor_], i);
        float *classIdPtr = (float*) qPtr_->GetBuffer(tensors[classIdTensor_], i);
        if (detectedNumber == nullptr || confidencePtr == nullptr || bboxPtr == nullptr || classIdPtr == nullptr) {
            LogError << "detectedNumber or confidencePtr or classIdPtr "
                     << "or bboxPtr is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        if ((int)*detectedNumber > DETECT_NUM) {
            LogWarn << "Detected objects' number is more than 100, check ScoreTresh value.";
        }
        for (size_t j = 0; j < (size_t)*detectedNumber && j < DETECT_NUM; j++) {
            int classId = static_cast<int>(classIdPtr[j]);
            if (classId >= (int)qPtr_->classNum_ || classId < 0) {
                continue;
            }
            if (confidencePtr[j] < qPtr_->separateScoreThresh_[classId]) {
                continue;
            }
            ObjectInfo objInfo;
            objInfo.classId = classId;
            objInfo.confidence = confidencePtr[j];
            objInfo.className = qPtr_->configData_.GetClassName((size_t)objInfo.classId);
            if (j * BOX_DIM + RIGHTBOTX >= bboxLen) {
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
    LogDebug << "OriginalOutput write results successed.";
    return APP_ERR_OK;
}

APP_ERROR FasterRcnnPostProcessDptr::NmsCutOutput(const std::vector<TensorBase>& tensors,
    std::vector<std::vector<ObjectInfo>>& objectInfos)
{
    LogDebug << "NmsCutOutput start to write results.";
    for (auto num : {bboxCutTensor_, confidenceCutTensor_}) {
        if (num >= tensors.size()) {
            LogError << "TENSOR(" << num << ") must be less than tensors'size(" << tensors.size()
                     << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_FAILURE;
        }
    }
    uint32_t batchSize = tensors[objectNumTensor_].GetShape()[0];
    for (uint32_t i = 0; i < batchSize; i++) {
        std::vector<ObjectInfo> objectInfo;
        APP_ERROR ret = GetObjectInfo(tensors, i, objectInfo);
        if (ret != APP_ERR_OK) {
            LogError << "GetObjectInfo failed." << GetErrorInfo(ret);
            return ret;
        }
        NmsSort(objectInfo, iouThresh_);
        objectInfos.push_back(objectInfo);
    }
    LogDebug << "NmsCutOutput write results successed.";
    return APP_ERR_OK;
}

APP_ERROR FasterRcnnPostProcessDptr::GetObjectInfo(const std::vector<TensorBase> &tensors, uint32_t i,
    std::vector<ObjectInfo> &objectInfo)
{
    float *detectedNumber = (float*) qPtr_->GetBuffer(tensors[objectNumTensor_], i);
    float *bboxPtr = (float*) qPtr_->GetBuffer(tensors[bboxCutTensor_], i);
    float *confidencePtr = (float*) qPtr_->GetBuffer(tensors[confidenceCutTensor_], i);
    uint32_t batchSize = tensors[bboxTensor_].GetShape()[0];
    uint32_t bboxLen = tensors[bboxTensor_].GetSize() / batchSize;
    uint32_t confidenceLen = tensors[confidenceCutTensor_].GetSize() / batchSize;

    if (detectedNumber == nullptr || confidencePtr == nullptr || bboxPtr == nullptr) {
        LogError << "The detectedNumber or confidencePtr "
                 << "or bboxPtr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER) ;
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if ((int)*detectedNumber > DETECT_NUM) {
        LogWarn << "Detected objects' number is more than 100, check ScoreTresh value.";
    }
    std::vector<int> maxIndexs = {};
    std::vector<float> maxProbs = {};
    for (uint32_t j = 0; j < static_cast<uint32_t>(*detectedNumber) && j < DETECT_NUM; j++) {
        std::vector<float> confidences;
        for (uint32_t k = 0; k < qPtr_->classNum_; ++k) {
            if (k + j * qPtr_->classNum_ >= confidenceLen) {
                continue;
            }
            confidences.push_back(confidencePtr[k + j * qPtr_->classNum_]);
        }
        std::vector<float>::iterator maxElement = std::max_element(std::begin(confidences), std::end(confidences));
        int argmaxIndex = maxElement - std::begin(confidences);
        maxIndexs.push_back(argmaxIndex);
        maxProbs.push_back(confidencePtr[static_cast<uint32_t>(argmaxIndex) + j * qPtr_->classNum_]);
    }
    for (size_t j = 0; j < static_cast<size_t>(*detectedNumber); j++) {
        if (maxIndexs[j] < 0 || maxIndexs[j] >= static_cast<int>(qPtr_->classNum_)) {
            continue;
        }
        if (maxProbs[j] > qPtr_->separateScoreThresh_[maxIndexs[j]]) {
            ObjectInfo objInfo;
            objInfo.classId = maxIndexs[j] + 1;
            objInfo.className = qPtr_->configData_.GetClassName((size_t)objInfo.classId);
            objInfo.confidence = maxProbs[j];
            int detectClassNum = static_cast<int>(j * qPtr_->classNum_);
            if (detectClassNum * BOX_DIM + maxIndexs[j] * BOX_DIM + RIGHTBOTX >=
                static_cast<int>(bboxLen)) {
                continue;
            }
            objInfo.x0 = bboxPtr[detectClassNum * BOX_DIM + maxIndexs[j] * BOX_DIM + LEFTTOPX];
            objInfo.y0 = bboxPtr[detectClassNum * BOX_DIM + maxIndexs[j] * BOX_DIM + LEFTTOPY];
            objInfo.x1 = bboxPtr[detectClassNum * BOX_DIM + maxIndexs[j] * BOX_DIM + RIGHTBOTX];
            objInfo.y1 = bboxPtr[detectClassNum * BOX_DIM + maxIndexs[j] * BOX_DIM + RIGHTBOTY];
            objectInfo.push_back(objInfo);
        }
    }
    return APP_ERR_OK;
}

APP_ERROR FasterRcnnPostProcessDptr::FpnOutput(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<ObjectInfo>> &objectInfos)
{
    LogDebug << "FpnOutput start to write results.";
    for (auto num : {objectNumTensor_, bboxCutTensor_}) {
        if (num >= tensors.size()) {
            LogError << "TENSOR(" << num << ") must be less than tensors'size(" << tensors.size()
                     << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_FAILURE;
        }
    }
    auto shape = tensors[objectNumTensor_].GetShape();
    uint32_t batchSize = shape[0];
    uint32_t bboxLen = tensors[bboxCutTensor_].GetSize() / batchSize;
    for (uint32_t i = 0; i < batchSize; i++) {
        int detectedNumber = DETECT_NUM;
        auto *bboxPtr = (float *)qPtr_->GetBuffer(tensors[objectNumTensor_], i);
        auto *classIdPtr = (int64_t *)qPtr_->GetBuffer(tensors[bboxCutTensor_], i);
        if (bboxPtr == nullptr || classIdPtr == nullptr) {
            LogError << "The bboxPtr is nullptr or classIdPtr is nullptr."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        std::vector<ObjectInfo> objInfos;
        for (int j = 0; j < detectedNumber; j++) {
            auto classId = static_cast<int64_t>(classIdPtr[j]);
            if (classId < 0 || classId >= qPtr_->classNum_) {
                continue;
            }
            ObjectInfo objInfo {};
            objInfo.classId = (float)classId;
            objInfo.className = qPtr_->configData_.GetClassName((size_t)objInfo.classId);
            if (j * static_cast<int>(FPN_BOX_DIM) + CONFINDENCE >= static_cast<int>(bboxLen)) {
                continue;
            }
            objInfo.x0 = bboxPtr[j * FPN_BOX_DIM + LEFTTOPY];
            objInfo.y0 = bboxPtr[j * FPN_BOX_DIM + LEFTTOPX];
            objInfo.x1 = bboxPtr[j * FPN_BOX_DIM + RIGHTBOTY];
            objInfo.y1 = bboxPtr[j * FPN_BOX_DIM + RIGHTBOTX];
            objInfo.confidence = bboxPtr[j * FPN_BOX_DIM + CONFINDENCE];
            if (objInfo.confidence < qPtr_->separateScoreThresh_[classId]) {
                continue;
            }
            LogDebug << "(x0, y0, x1, y1, score) : " << objInfo.x0 << ", " << objInfo.y0 << ", " << objInfo.y0
                     << ", " << objInfo.x1 << ", " << objInfo.y1 << ", " << objInfo.confidence;
            objInfos.push_back(objInfo);
        }
        objectInfos.push_back(objInfos);
    }
    LogDebug << "FpnOutput write results successed.";
    return APP_ERR_OK;
}

APP_ERROR FasterRcnnPostProcessDptr::GetTensorsObjectInfos(const std::vector<TensorBase>& inputs,
    std::vector<std::vector<ObjectInfo>>& objectInfos, const std::vector<ResizedImageInfo>& resizedImageInfos,
    bool& normalizedFlag)
{
    APP_ERROR ret = APP_ERR_OK;
    switch ((FasterRcnnPostProcessDptr::ModelType)modelType_) {
        case FasterRcnnPostProcessDptr::ORIGINAL:
            ret = OriginalOutput(inputs, objectInfos);
            break;
        case FasterRcnnPostProcessDptr::NMS_CUT:
            if (framework_ == "mindspore") {
                ret = ObjectDetectionOutputForMS(inputs, objectInfos, resizedImageInfos);
            } else {
                ret = NmsCutOutput(inputs, objectInfos);
                if (!resizedImageInfos.empty())
                    normalizedFlag = false;
            }
            break;
        case FasterRcnnPostProcessDptr::FPN:
            ret = FpnOutput(inputs, objectInfos);
            if (!resizedImageInfos.empty())
                normalizedFlag = false;
            break;
        default:
            LogError << "The modelType_(" << modelType_ << ") is not supported." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
    }
    return ret;
}
}
#endif

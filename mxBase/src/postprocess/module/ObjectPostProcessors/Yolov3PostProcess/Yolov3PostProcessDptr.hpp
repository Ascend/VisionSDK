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
 * Description: Private interface of Yolov3PostProcess, for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef YOLOV3_POST_PROCESS_DPTR_H
#define YOLOV3_POST_PROCESS_DPTR_H

#include <algorithm>
#include "ObjectPostProcessors/Yolov3PostProcess.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/GlobalManager/GlobalManager.h"

namespace MxBase {
const int SCALE = 32;
const int BIASESDIM = 2;
const int OFFSETWIDTH = 2;
const int OFFSETHEIGHT = 3;
const int OFFSETBIASES = 1;
const int OFFSETOBJECTNESS = 1;

const int NHWC_HEIGHTINDEX = 1;
const int NHWC_WIDTHINDEX = 2;
const int NCHW_HEIGHTINDEX = 2;
const int NCHW_WIDTHINDEX = 3;
const int YOLO_INFO_DIM = 5;
const int BOX_NUM = 1000;

const std::vector<std::string> FRAMEWORKS = {
    "Yolov3-tensorflow-NHWC", "Yolov3-caffe-NCHW", "Yolov4-mindspore-NHWC",
    "Yolov4-pytorch-NCHW", "Yolov5-pytorch-NCHWC"
};
}

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER Yolov3PostProcessDptr {
public:
    explicit Yolov3PostProcessDptr(Yolov3PostProcess *pYolov3PostProcess);
    ~Yolov3PostProcessDptr() {}
    Yolov3PostProcessDptr &operator=(const Yolov3PostProcessDptr &other);
    Yolov3PostProcessDptr(const Yolov3PostProcessDptr &other);
    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;
    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);
    APP_ERROR ObjectDetectionOutput(const std::vector<TensorBase> &tensors,
                                    std::vector<std::vector<ObjectInfo>> &objectInfos,
                                    const std::vector<ResizedImageInfo> &resizedImageInfos);
    APP_ERROR ObjectDetectionOutput(const std::vector<TensorBase> &tensors,
                                    std::vector<std::vector<ObjectInfo>> &objectInfos,
                                    std::vector<uint32_t>& widths, std::vector<uint32_t>& heights);
    void CompareProb(int& classID, float& maxProb, float classProb, int classNum);
    void SelectClassNCHW(std::shared_ptr<void> netout, NetInfo info, std::vector<MxBase::ObjectInfo>& detBoxes,
                         int stride, OutputLayer layer);
    void SelectClassNHWC(std::shared_ptr<void> netout, NetInfo info, std::vector<MxBase::ObjectInfo>& detBoxes,
                         int stride, OutputLayer layer);
    void SelectClassNCHWC(std::shared_ptr<void> netout, NetInfo info, std::vector<MxBase::ObjectInfo>& detBoxes,
                          int stride, OutputLayer layer);
    void SelectClassYoloV4MS(std::shared_ptr<void> netout, NetInfo info, std::vector<MxBase::ObjectInfo>& detBoxes,
                             int stride, OutputLayer layer);
    void GenerateBbox(std::vector<std::shared_ptr<void>> featLayerData, std::vector<MxBase::ObjectInfo> &detBoxes,
                      const std::vector<std::vector<size_t>>& featLayerShapes, const int netWidth, const int netHeight);
    ObjectInfo &GetInfo(ObjectInfo &det, float x, float y, float width, float height) const;
    APP_ERROR GetBiases(std::string& strBiases);

    void GetConfigValue();
    APP_ERROR CheckFrameWork();

public:
    float objectnessThresh_ = DEFAULT_OBJECTNESS_THRESH; // Threshold of objectness value
    float iouThresh_ = DEFAULT_IOU_THRESH; // Non-Maximum Suppression threshold
    int anchorDim_ = DEFAULT_ANCHOR_DIM;
    uint32_t biasesNum_ = DEFAULT_BIASES_NUM; // Yolov3 anchors, generate from train data, coco dataset
    int yoloType_ = DEFAULT_YOLO_TYPE;
    int modelType_ = 0;
    enum ModelType {
        NHWC = 0,
        NCHW = 1,
        NCHWC = 2
    };
    int yoloVersion_ = DEFAULT_YOLO_VERSION;
    std::string framework_ = "TensorFlow";
    std::vector<float> biases_ = {};
    Yolov3PostProcess* qPtr_ = nullptr;
};

Yolov3PostProcessDptr::Yolov3PostProcessDptr(Yolov3PostProcess *pYolov3PostProcess)
    : qPtr_(pYolov3PostProcess)
{}

Yolov3PostProcessDptr::Yolov3PostProcessDptr(const Yolov3PostProcessDptr &other)
{
    *this = other;
}

Yolov3PostProcessDptr& Yolov3PostProcessDptr::operator=(const Yolov3PostProcessDptr &other)
{
    if (this == &other) {
        return *this;
    }
    objectnessThresh_ = other.objectnessThresh_;
    iouThresh_ = other.iouThresh_;
    anchorDim_ = other.anchorDim_;
    biasesNum_ = other.biasesNum_;
    yoloType_ = other.yoloType_;
    modelType_ = other.modelType_;
    yoloType_ = other.yoloType_;
    biases_ = other.biases_;
    framework_ = other.framework_;
    return *this;
}
APP_ERROR Yolov3PostProcessDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "Input tensors are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return qPtr_->CheckAndMoveTensors(tensors);
}
bool Yolov3PostProcessDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    if (tensors.size() != (size_t)yoloType_) {
        LogError << "The number of tensors (" << tensors.size() << ") " << "is unequal to yoloType_("
                 << yoloType_ << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    for (size_t i = 0; i < tensors.size(); i++) {
        if (tensors[i].GetDataTypeSize() != qPtr_->FOUR_BYTE) {
            LogError << "The tensor type(" << TensorDataTypeStr[tensors[i].GetTensorType()]
                     << ") mismatched. requires(" << qPtr_->FOUR_BYTE << ") bytes tensortype."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
    for (size_t i = 0; i < tensors.size(); i++) {
        auto shape = tensors[i].GetShape();
        if (shape.size() < 0x4) {
            LogError << "Dimensions of tensor [" << i << "] is less than " << 0x4 << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        uint32_t channelNumber = 1;
        if (ModelType(modelType_) == NHWC || ModelType(modelType_) == NCHW) {
            size_t startIndex = modelType_ ? 0x1 : 0x3;
            size_t endIndex = modelType_ ? (shape.size() - 0x2) : shape.size();
            for (size_t j = startIndex; j < endIndex; j++) {
                channelNumber *= shape[j];
            }
        } else {
            if (shape.size() == 0x4) {
                LogError << "The dimensions of tensor [" << i << "] cannot be equal to " << 0x4 << "."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return false;
            }
            channelNumber = shape[0x1] * shape[0x4];
        }
        if (channelNumber != anchorDim_ * (qPtr_->classNum_ + YOLO_INFO_DIM)) {
            LogError << "The channelNumber(" << channelNumber << ") != anchorDim_ * (classNum_ + 5)."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
    return true;
}

APP_ERROR Yolov3PostProcessDptr::ObjectDetectionOutput(const std::vector<TensorBase>& tensors,
    std::vector<std::vector<ObjectInfo>>& objectInfos, const std::vector<ResizedImageInfo>& resizedImageInfos)
{
    LogDebug << "Yolov3PostProcess start to write results.";
    if (tensors.size() == 0) {
        LogError << "Empty tensors!" << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    auto shape = tensors[0].GetShape();
    if (shape.size() == 0) {
        LogError << "Empty tensor shape!" << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    uint32_t batchSize = shape[0];
    if (resizedImageInfos.size() != batchSize) {
        LogError << "The size of resizedImageInfos(" << resizedImageInfos.size() << ") is not equal to batchSize("
                 << batchSize << "), please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (uint32_t i = 0; i < batchSize; i++) {
        std::vector<std::shared_ptr<void>> featLayerData = {};
        std::vector<std::vector<size_t>> featLayerShapes = {};
        for (uint32_t j = 0; j < tensors.size(); j++) {
            auto dataPtr = (uint8_t *) qPtr_->GetBuffer(tensors[j], i);
            if (dataPtr == nullptr) {
                LogError << "The dataPtr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
                return APP_ERR_COMM_INVALID_POINTER;
            }
            std::shared_ptr<void> tmpPointer;
            auto uint8Deleter = [] (uint8_t*) { };
            tmpPointer.reset(dataPtr, uint8Deleter);
            featLayerData.push_back(tmpPointer);
            shape = tensors[j].GetShape();
            std::vector<size_t> featLayerShape = {};
            for (auto s : shape) {
                featLayerShape.push_back((size_t)s);
            }
            featLayerShapes.push_back(featLayerShape);
        }
        std::vector<ObjectInfo> objectInfo;
        GenerateBbox(featLayerData, objectInfo, featLayerShapes, resizedImageInfos[i].widthResize,
                     resizedImageInfos[i].heightResize);
        MxBase::NmsSort(objectInfo, iouThresh_);
        objectInfos.push_back(objectInfo);
    }
    LogDebug << "Yolov3PostProcess write results successed.";
    return APP_ERR_OK;
}

/*
* @description: Compare the confidences between 2 classes and get the larger one
*/
void Yolov3PostProcessDptr::CompareProb(int& classID, float& maxProb, float classProb, int classNum)
{
    if (classProb > maxProb) {
        maxProb = classProb;
        classID = classNum;
    }
}

/*
* @description: Select the highest confidence class name for each predicted box
* @param netout  The feature data which contains box coordinates, objectness value and confidence of each class
* @param info  Yolo layer info which contains class number, box dim and so on
* @param detBoxes  ObjectInfo vector where all ObjectInfoes's confidences are greater than threshold
* @param stride  Stride of output feature data
* @param layer  Yolo output layer
*/
void Yolov3PostProcessDptr::SelectClassNCHW(std::shared_ptr<void> netout, NetInfo info,
    std::vector<MxBase::ObjectInfo>& detBoxes, int stride, OutputLayer layer)
{
    if (layer.width == 0 || layer.height == 0 || IsDenominatorZero(info.netWidth)
        || IsDenominatorZero(info.netHeight)) {
        LogError << "The divided value is invalid parameter, please check it first. "
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    for (int64_t j = 0; j < stride; ++j) {
        for (int64_t k = 0; k < info.anchorDim; ++k) {
            int64_t bIdx = (info.bboxDim + 1 + info.classNum) * stride * k + j; // begin index
            int64_t oIdx = bIdx + info.bboxDim * stride; // objectness index
            // check obj
            float objectness = fastmath::sigmoid(static_cast<float *>(netout.get())[oIdx]);
            if (objectness <= objectnessThresh_) {
                continue;
            }
            int classID = -1;
            float maxProb = qPtr_->scoreThresh_;
            float classProb;
            // Compare the confidence of the 3 anchors, select the largest one
            for (int c = 0; c < info.classNum; ++c) {
                classProb = fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx +
                    (info.bboxDim + OFFSETOBJECTNESS + c) * stride]) * objectness;
                CompareProb(classID, maxProb, classProb, c);
            }
            if (classID < 0) continue;
            MxBase::ObjectInfo det;
            int64_t row = j / static_cast<int64_t>(layer.width);
            int64_t col = j % static_cast<int64_t>(layer.width);
            float x = (col + fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx])) / layer.width;
            float y = (row + fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx + stride])) / layer.height;
            float width = fastmath::exp(static_cast<float *>(netout.get())[bIdx + OFFSETWIDTH * stride]) *
                          layer.anchors[BIASESDIM * k] / info.netWidth;
            float height = fastmath::exp(static_cast<float *>(netout.get())[bIdx + OFFSETHEIGHT * stride]) *
                           layer.anchors[BIASESDIM * k + OFFSETBIASES] / info.netHeight;
            det.x0 = std::max(0.0f, x - width / COORDINATE_PARAM);
            det.x1 = std::min(1.0f, x + width / COORDINATE_PARAM);
            det.y0 = std::max(0.0f, y - height / COORDINATE_PARAM);
            det.y1 = std::min(1.0f, y + height / COORDINATE_PARAM);
            det.classId = classID;
            det.className = qPtr_->configData_.GetClassName(classID);
            det.confidence = maxProb;
            bool ret = det.confidence < qPtr_->separateScoreThresh_[classID];
            if (ret) {
                continue;
            }
            detBoxes.emplace_back(det);
        }
    }
}

/*
* @description: Select the highest confidence class label for each predicted box and save into detBoxes
* @param netout  The feature data which contains box coordinates, objectness value and confidence of each class
* @param info  Yolo layer info which contains class number, box dim and so on
* @param detBoxes  ObjectInfo vector where all ObjectInfoes's confidences are greater than threshold
* @param stride  Stride of output feature data
* @param layer  Yolo output layer
*/
void Yolov3PostProcessDptr::SelectClassNHWC(std::shared_ptr<void> netout, NetInfo info,
    std::vector<MxBase::ObjectInfo>& detBoxes, int stride, OutputLayer layer)
{
    LogDebug << " out size " << sizeof(netout.get());
    if (layer.width == 0 || layer.height == 0 || IsDenominatorZero(info.netWidth)
        || IsDenominatorZero(info.netHeight)) {
        LogError << "The divided value is an invalid parameter." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    const int offsetY = 1;
    for (int64_t j = 0; j < stride; ++j) {
        for (int64_t k = 0; k < info.anchorDim; ++k) {
            int64_t bIdx = (info.bboxDim + 1 + info.classNum) * info.anchorDim * j +
                       k * (info.bboxDim + 1 + info.classNum);
            int64_t oIdx = bIdx + info.bboxDim; // objectness index
            // check obj
            float objectness = fastmath::sigmoid(static_cast<float *>(netout.get())[oIdx]);
            if (objectness <= objectnessThresh_) {
                continue;
            }
            int classID = -1;
            float maxProb = qPtr_->scoreThresh_;
            float classProb;
            // Compare the confidence of the 3 anchors, select the largest one
            for (int c = 0; c < info.classNum; ++c) {
                classProb = fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx +
                            (info.bboxDim + OFFSETOBJECTNESS + c)]) * objectness;
                CompareProb(classID, maxProb, classProb, c);
            }
            if (classID < 0) continue;
            MxBase::ObjectInfo det;
            int64_t row = j / static_cast<int64_t>(layer.width);
            int64_t col = j % static_cast<int64_t>(layer.width);
            float x = (col + fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx])) / layer.width;
            float y = (row + fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx + offsetY])) / layer.height;
            float width = fastmath::exp(static_cast<float *>(netout.get())[bIdx + OFFSETWIDTH]) *
                          layer.anchors[BIASESDIM * k] / info.netWidth;
            float height = fastmath::exp(static_cast<float *>(netout.get())[bIdx + OFFSETHEIGHT]) *
                           layer.anchors[BIASESDIM * k + OFFSETBIASES] / info.netHeight;
            det.x0 = std::max(0.0f, x - width / COORDINATE_PARAM);
            det.x1 = std::min(1.0f, x + width / COORDINATE_PARAM);
            det.y0 = std::max(0.0f, y - height / COORDINATE_PARAM);
            det.y1 = std::min(1.0f, y + height / COORDINATE_PARAM);
            det.classId = classID;
            det.className = qPtr_->configData_.GetClassName(classID);
            det.confidence = maxProb;
            if (det.confidence < qPtr_->separateScoreThresh_[classID]) {
                continue;
            }
            detBoxes.emplace_back(det);
        }
    }
}

void Yolov3PostProcessDptr::SelectClassNCHWC(std::shared_ptr<void> netout, NetInfo info,
    std::vector<MxBase::ObjectInfo>& detBoxes, int stride, OutputLayer layer)
{
    if (layer.width == 0 || layer.height == 0 || IsDenominatorZero(info.netWidth)
        || IsDenominatorZero(info.netHeight)) {
        LogError << "The divided value is an invalid parameter. " << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    const int offsetY = 1;
    for (int64_t j = 0; j < stride; ++j) {
        for (int64_t k = 0; k < info.anchorDim; ++k) {
            int64_t bIdx = (info.bboxDim + 1 + info.classNum) * stride * k + j * (info.bboxDim + 1 + info.classNum);
            int64_t oIdx = bIdx + info.bboxDim; // objectness index
            // check obj
            float objectness = fastmath::sigmoid(static_cast<float *>(netout.get())[oIdx]);
            if (objectness <= objectnessThresh_) {
                continue;
            }
            int classID = -1;
            float maxProb = qPtr_->scoreThresh_;
            float classProb;
            // Compare the confidence of the 3 anchors, select the largest one
            for (int c = 0; c < info.classNum; ++c) {
                classProb = fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx +
                    (info.bboxDim + OFFSETOBJECTNESS + c)]) * objectness;
                CompareProb(classID, maxProb, classProb, c);
            }
            if (classID < 0) continue;
            MxBase::ObjectInfo det;
            int64_t row = j / static_cast<int64_t>(layer.width);
            int64_t col = j % static_cast<int64_t>(layer.width);
            float x = (col + fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx]) * COORDINATE_PARAM -
                       MEAN_PARAM) / layer.width;
            float y = (row + fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx + offsetY]) *
                             COORDINATE_PARAM - MEAN_PARAM) / layer.height;
            float width = (fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx + OFFSETWIDTH]) *
                           COORDINATE_PARAM) * (fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx +
                           OFFSETWIDTH]) * COORDINATE_PARAM) * layer.anchors[BIASESDIM * k] / info.netWidth;
            float height = (fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx + OFFSETHEIGHT]) *
                            COORDINATE_PARAM) * (fastmath::sigmoid(static_cast<float *>(netout.get())[bIdx +
                            OFFSETHEIGHT]) * COORDINATE_PARAM) * layer.anchors[BIASESDIM * k + OFFSETBIASES] /
                            info.netHeight;
            det = GetInfo(det, x, y, width, height);
            det.classId = classID;
            det.className = qPtr_->configData_.GetClassName(classID);
            det.confidence = maxProb;
            if (det.confidence < qPtr_->separateScoreThresh_[classID]) {
                continue;
            }
            detBoxes.emplace_back(det);
        }
    }
}

ObjectInfo &Yolov3PostProcessDptr::GetInfo(ObjectInfo &det, float x, float y,
    float width, float height) const
{
    det.x0 = std::max(0.0f, x - width / COORDINATE_PARAM);
    det.x1 = std::min(1.0f, x + width / COORDINATE_PARAM);
    det.y0 = std::max(0.0f, y - height / COORDINATE_PARAM);
    det.y1 = std::min(1.0f, y + height / COORDINATE_PARAM);
    return det;
}

void Yolov3PostProcessDptr::SelectClassYoloV4MS(std::shared_ptr<void> netout, NetInfo info,
    std::vector<MxBase::ObjectInfo>& detBoxes, int stride, OutputLayer)
{
    float minObjConf =
        *std::min_element(std::begin(qPtr_->separateScoreThresh_), std::end(qPtr_->separateScoreThresh_));
    const int offsetY = 1;
    for (int64_t j = 0; j < stride; ++j) {
        for (int64_t k = 0; k < info.anchorDim; ++k) {
            int64_t bIdx = (info.bboxDim + 1 + info.classNum) * info.anchorDim * j +
                       k * (info.bboxDim + 1 + info.classNum);
            int64_t oIdx = bIdx + info.bboxDim; // objectness index
            // check obj
            float objectness = static_cast<float *>(netout.get())[oIdx];
            if (objectness < minObjConf) continue;
            int classID = -1;
            float maxProb = qPtr_->scoreThresh_;
            float classProb;
            // Compare the confidence of the 3 anchors, select the largest one
            for (int c = 0; c < info.classNum; ++c) {
                classProb =
                    static_cast<float *>(netout.get())[bIdx + (info.bboxDim + OFFSETOBJECTNESS + c)] * objectness;
                CompareProb(classID, maxProb, classProb, c);
            }
            if (classID < 0) continue;
            if (maxProb < qPtr_->separateScoreThresh_[classID]) continue;
            MxBase::ObjectInfo det;
            float x = static_cast<float *>(netout.get())[bIdx];
            float y = static_cast<float *>(netout.get())[bIdx + offsetY];
            float width = static_cast<float *>(netout.get())[bIdx + OFFSETWIDTH];
            float height = static_cast<float *>(netout.get())[bIdx + OFFSETHEIGHT];
            det.x0 = std::max(0.0f, x - width / COORDINATE_PARAM);
            det.x1 = std::min(1.0f, x + width / COORDINATE_PARAM);
            det.y0 = std::max(0.0f, y - height / COORDINATE_PARAM);
            det.y1 = std::min(1.0f, y + height / COORDINATE_PARAM);
            det.classId = classID;
            det.className = qPtr_->configData_.GetClassName(classID);
            det.confidence = maxProb;
            bool ret = det.confidence < qPtr_->separateScoreThresh_[classID];
            if (ret) {
                continue;
            }
            detBoxes.emplace_back(det);
        }
    }
}

/*
* @description: According to the yolo layer structure, encapsulate the anchor box data of each feature into detBoxes
* @param featLayerData  Vector of 3 output feature data
* @param info  Yolo layer info which contains anchors dim, bbox dim, class number, net width, net height and
            3 outputlayer(eg. 13*13, 26*26, 52*52)
* @param detBoxes  ObjectInfo vector where all ObjectInfoes's confidences are greater than threshold
*/
void Yolov3PostProcessDptr::GenerateBbox(std::vector<std::shared_ptr<void>> featLayerData,
    std::vector<MxBase::ObjectInfo> &detBoxes, const std::vector<std::vector<size_t>>& featLayerShapes,
    const int netWidth, const int netHeight)
{
    NetInfo netInfo;
    netInfo.anchorDim = anchorDim_;
    netInfo.bboxDim = BOX_DIM;
    netInfo.classNum = (int)qPtr_->classNum_;
    netInfo.netWidth = netWidth;
    netInfo.netHeight = netHeight;
    for (int i = 0; i < yoloType_; ++i) {
        int widthIndex_ = modelType_ ? NCHW_WIDTHINDEX : NHWC_WIDTHINDEX;
        int heightIndex_ = modelType_ ? NCHW_HEIGHTINDEX : NHWC_HEIGHTINDEX;
        OutputLayer layer = {};
        layer.width = featLayerShapes[i][widthIndex_];
        layer.height = featLayerShapes[i][heightIndex_];
        int logOrder = (int)(
            log(static_cast<double>(featLayerShapes[i][widthIndex_] * SCALE) / static_cast<double>(netWidth))
            / log(BIASESDIM));
        int64_t startIdx = (int64_t)(yoloType_ - 1 - logOrder) * netInfo.anchorDim * BIASESDIM;
        int64_t endIdx = startIdx + (int64_t)netInfo.anchorDim * BIASESDIM;
        int64_t idx = 0;
        int64_t biasSize = static_cast<int64_t>(biases_.size());
        for (int64_t j = startIdx; j < endIdx; ++j) {
            if (idx >= ANCHOR_NUM || j < 0 || j >= biasSize) {
                 LogError << "GenenrateBox failed, Please check the Model's config and output."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                 return;
            }
            layer.anchors[idx++] = biases_[j];
        }
        if (IsDenominatorZero(netWidth) || IsDenominatorZero(netHeight) ||
            IsDenominatorZero((long)layer.height) || IsDenominatorZero((long)layer.width)) {
            LogError << "Model's output width and height must not be equal to 0."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        int stride = (int)(layer.width * layer.height);
        std::shared_ptr<void> netout = featLayerData[i];
        if (ModelType(modelType_) == NHWC) {
            if (yoloVersion_ == YOLOV4_VERSION && framework_ == "mindspore") {
                SelectClassYoloV4MS(netout, netInfo, detBoxes, stride, layer);
            } else {
                SelectClassNHWC(netout, netInfo, detBoxes, stride, layer);
            }
        } else if (ModelType(modelType_) == NCHW) {
            SelectClassNCHW(netout, netInfo, detBoxes, stride, layer);
        } else {
            SelectClassNCHWC(netout, netInfo, detBoxes, stride, layer);
        }
    }
}

APP_ERROR Yolov3PostProcessDptr::GetBiases(std::string& strBiases)
{
    biases_ = StringUtils::SplitAndCastToFloat(strBiases, ',');
    if (biases_.size() != biasesNum_) {
        if (MxBase::StringUtils::HasInvalidChar(strBiases)) {
            LogError << "Invalid strBiases." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        } else {
            LogError << "The biasesNum (" << biasesNum_ << ") is not equal to total number(" << biases_.size()
                     << ") of biases (" << strBiases <<")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM) ;
        }
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

void Yolov3PostProcessDptr::GetConfigValue()
{
    APP_ERROR ret = qPtr_->configData_.GetFileValue<uint32_t>("BIASES_NUM", biasesNum_, (uint32_t)0x0, (uint32_t)0x64);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read BIASES_NUM from config, default is: " << biasesNum_;
    }
    ret = qPtr_->configData_.GetFileValue<float>("OBJECTNESS_THRESH", objectnessThresh_, 0.0f, 1.0f);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read OBJECTNESS_THRESH from config, default is: " << objectnessThresh_;
    }
    ret = qPtr_->configData_.GetFileValue<float>("IOU_THRESH", iouThresh_, 0.0f, 1.0f);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read IOU_THRESH from config, default is: " << iouThresh_;
    }
    ret = qPtr_->configData_.GetFileValue<int>("YOLO_TYPE", yoloType_, 0x0, 0x10);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read YOLO_TYPE from config, default is: " << yoloType_;
    }
    ret = qPtr_->configData_.GetFileValue<int>("YOLO_VERSION", yoloVersion_, 0x0, 0x10);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read YOLO_VERSION from config, default is: " << yoloVersion_;
    }
    ret = qPtr_->configData_.GetFileValue<int>("MODEL_TYPE", modelType_);
    if (ret != APP_ERR_OK) {
        if (yoloVersion_ == YOLOV5_VERSION) {
            modelType_ = NCHWC;
        }
        LogWarn << GetErrorInfo(ret) << "Fail to read MODEL_TYPE from config, default is: " << modelType_;
    }
    ret = qPtr_->configData_.GetFileValue<int>("ANCHOR_DIM", anchorDim_, 0x0, 0x10);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read ANCHOR_DIM from config, default is: " << anchorDim_;
    }
    ret = qPtr_->configData_.GetFileValue<std::string>("FRAMEWORK", framework_);
    if (ret != APP_ERR_OK) {
        if (yoloVersion_ == YOLOV4_VERSION) {
            framework_ = "MindSpore";
        } else if (yoloVersion_ == YOLOV5_VERSION) {
            framework_ = "PyTorch";
        } else if (yoloVersion_ == YOLOV3_VERSION && modelType_ == 1) {
            framework_ = "Caffe";
        }
        LogWarn << GetErrorInfo(ret) << "Fail to read FRAMEWORK from config, default is: " << framework_;
    }
    std::transform(framework_.begin(), framework_.end(), framework_.begin(), ::tolower);
}

APP_ERROR Yolov3PostProcessDptr::CheckFrameWork()
{
    std::string versionStr = "Yolov" + std::to_string(yoloVersion_);
    std::string frameWorkStr = framework_;
    std::string modelTypeStr;
    switch (ModelType(modelType_)) {
        case NHWC:
            modelTypeStr = "NHWC";
            break;
        case NCHW:
            modelTypeStr = "NCHW";
            break;
        case NCHWC:
            modelTypeStr = "NCHWC";
            break;
        default:
            LogError << "The modelType_(" << modelType_ << ") is not supported."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
    }
    std::string supportedFramework = versionStr + "-" + frameWorkStr + "-" + modelTypeStr;
    LogInfo << "Your model postprocess setting is: " << versionStr << ", " << frameWorkStr << ", " << modelTypeStr;
    if (std::find(FRAMEWORKS.begin(), FRAMEWORKS.end(), supportedFramework) == FRAMEWORKS.end()) {
        LogError << "Sorry, we now support: \n"
                 << "  YOLO_VERSION  |   FRAMEWORK  |  MODEL_TYPE (model's output type)  \n"
                 << "----------------------------------------------------------------  \n"
                 << "     Yolov3     |  TensorFlow  |         NHWC (0)                 \n"
                 << "     Yolov3     |    Caffe     |         NCHW (1)                 \n"
                 << "     Yolov4     |  MindSpore   |         NHWC (0)                 \n"
                 << "     Yolov4     |   PyTorch    |         NCHW (1)                 \n"
                 << "     Yolov5     |   PyTorch    |         NCHWC(2)                 \n"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}
}
#endif // YOLOV3_POST_PROCESS_DPTR_H
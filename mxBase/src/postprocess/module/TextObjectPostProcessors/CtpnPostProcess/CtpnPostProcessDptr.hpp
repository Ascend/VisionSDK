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
 * Description: CtpnPostProcess private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef CTPNPOSTPROCESSOR_DPTR_H
#define CTPNPOSTPROCESSOR_DPTR_H

#include "MxBase/GlobalManager/GlobalManager.h"

namespace MxBase {
    const int BBOX_PRED = 0;
    const int CLS_PRED = 1;

    const int LEFTTOPX = 0;
    const int LEFTTOPY = 1;
    const int RIGHTBOTX = 2;
    const int RIGHTBOTY = 3;
    const int SCORE_DIM = 4;
    const int RESULT_DIM = 5;
    const int CONF_OBJECT = 1;

    const int OBJ_NUM_MINDSPORE = 1000;
    const float HALF_SCALE = 0.5;
}

namespace MxBase {
struct CtpnPostProcessInput {
    const std::vector<TensorBase> *tensors = nullptr;
    const ResizedImageInfo *resizedImageInfos = nullptr;
    const std::vector<std::vector<int>> *wholeImgAnchors = nullptr;
};

class SDK_UNAVAILABLE_FOR_OTHER CtpnPostProcessDptr {
public:
    explicit CtpnPostProcessDptr(CtpnPostProcess* pCtpnPostProcess);

    CtpnPostProcessDptr(const CtpnPostProcessDptr &other);

    ~CtpnPostProcessDptr() = default;

    CtpnPostProcessDptr &operator=(const CtpnPostProcessDptr &other);

    void ObjectDetectionOutput(const std::vector<TensorBase> &tensors,
        std::vector<std::vector<TextObjectInfo>> &textObjInfos, const std::vector<ResizedImageInfo> &resizedImageInfos);

    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;

    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);

    void GetConfigValue();

public:
    CtpnPostProcess *qPtr_ = nullptr;
    int minSize_ = 0;
    int featStride_ = 0;
    int anchorScales_ = 0;
    int featBoxLayer_ = 0;
    int featConfLayer_ = 0;
    int anchorNum_ = 0;
    int maxHorizontalGap_ = 0;
    float minOverLaps_ = 0;
    float minSizeSim_ = 0;
    bool isOriented_ = false;
    float boxIouThresh_ = 0;
    float textIouThresh_ = 0;
    float minRatio_ = 0;
    float textProposalsMinScore_ = 0;
    float lineMinScore_ = 0;
    int textProposalsWidth_ = 0;
    int minNumProposals_ = 0;
    int rpnPreNmsTopN_ = 0;
    int rpnPostNmsTopN_ = 0;
    bool is_mindspore_ = false;
    bool order_by_py_ = false;

private:
    int DecimalToInteger(double decimal);

    void CreateAnchors(std::vector<std::vector<int>> &anchors);

    void GetWholeImgAnchors(const std::vector<std::vector<int>> &anchors, const ResizedImageInfo &resizedImageInfos,
        std::vector<std::vector<int>> &wholeImgAnchors);

    void GetRealBoxesOfImg(const std::vector<int> &wholeImgAnchor, float &x1, float &y1, float &x2, float &y2);

    void ClipBoxes(const ResizedImageInfo &resizedImageInfos, float &x0, float &y0, float &x1, float &y1);

    bool FilterBoxes(float &x0, float &y0, float &x1, float &y1);

    void NonMaxSuppression(const CtpnPostProcessInput &input, int index,
        std::vector<std::vector<float>> &textProposals, std::vector<float> &scores);

    bool CalcOverLaps(const int &index1, const int &index2, const std::vector<std::vector<float>> &textProposals);

    void BuildGraph(const std::vector<std::vector<float>> &textProposals, const std::vector<float> &scores,
        const ResizedImageInfo &resizedImageInfo, std::vector<std::vector<bool>> &graph);

    void GetSuccessions(const std::vector<std::vector<float>> &textProposals,
        const std::vector<std::vector<int>> &boxesTable, const ResizedImageInfo &resizedImageInfo,
        const int &index, std::vector<int> &successions);

    void GetPrecursors(const std::vector<std::vector<float>> &textProposals,
        const std::vector<std::vector<int>> &boxesTable, const int &successionIdx, const int &index,
        std::vector<int> &precursors);

    void GetTextLines(const std::vector<std::vector<float>> &textProposals, const std::vector<float> &scores,
        const std::vector<std::vector<int>> &subGraphs, const ResizedImageInfo &resizedImageInfos,
        std::vector<TextObjectInfo> &textRecs);

    bool FilterBoxesFinal(TextObjectInfo textObjDetectInfo);

    APP_ERROR ReadSrcData(const CtpnPostProcessInput &input, int index,
        std::vector<MxBase::DetectBox> &detBoxes, std::vector<TextObjectInfo> &textObjectInfos);

    APP_ERROR FilterBoxesForReadSrcData(const CtpnPostProcessInput &input, CoorDim coorDim,
        std::vector<MxBase::DetectBox> &detBoxes, std::vector<TextObjectInfo> &textObjectInfos);

    APP_ERROR TraverseAnchor(CoorDim &coorDim, std::vector<MxBase::DetectBox> &detBoxes,
        const CtpnPostProcessInput &input, std::vector<TextObjectInfo> &textObjectInfos);

    APP_ERROR GetSrcDataAndNmsForMindSpore(const std::vector<TensorBase> &tensors, const int &index,
        std::vector<std::vector<float>> &textProposals, std::vector<float> &scores);

    void SelectTopKBoxesByScore(unsigned int num, std::vector<MxBase::DetectBox> &detBoxes);

    void GetTextObjInfoResult(const ResizedImageInfo &resizedImageInfo, std::vector<TextObjectInfo> &textObjInfo,
        uint32_t batchIdx);

    // reverse is false show the num from small to large.
    void SortIndexesByNpy(const std::vector<TextObjectInfo> &textObjectInfos, std::vector<int> &order,
        bool reverse = false);

    void SortIndexesByCpp(const std::vector<TextObjectInfo> &v, std::vector<int> &order);

    bool JudgeObjInfo(double widths, double heights, float confidence);

    void GetInfo(std::vector<std::vector<float>> &textLineBoxes, std::vector<float> &textLineBoxesX0,
                 std::vector<float> &textLineBoxesX1, std::vector<float> &textLineBoxesY0,
                 std::vector<float> &textLineBoxesY1) const;
};

CtpnPostProcessDptr::CtpnPostProcessDptr(CtpnPostProcess* pCtpnPostProcess)
    : qPtr_(pCtpnPostProcess)
{}

CtpnPostProcessDptr::CtpnPostProcessDptr(const CtpnPostProcessDptr &other)
{
    *this = other;
}

CtpnPostProcessDptr& CtpnPostProcessDptr::operator=(const CtpnPostProcessDptr &other)
{
    if (this == &other) {
        return *this;
    }
    minSize_ = other.minSize_;
    featStride_ = other.featStride_;
    anchorScales_ = other.anchorScales_;
    featBoxLayer_ = other.featBoxLayer_;
    featConfLayer_ = other.featConfLayer_;
    anchorNum_ = other.anchorNum_;
    maxHorizontalGap_ = other.maxHorizontalGap_;
    minOverLaps_ = other.minOverLaps_;
    minSizeSim_ = other.minSizeSim_;
    isOriented_ = other.isOriented_;
    boxIouThresh_ = other.boxIouThresh_;
    textIouThresh_ = other.textIouThresh_;
    minRatio_ = other.minRatio_;
    textProposalsMinScore_ = other.textProposalsMinScore_;
    lineMinScore_ = other.lineMinScore_;
    textProposalsWidth_ = other.textProposalsWidth_;
    minNumProposals_ = other.minNumProposals_;
    rpnPreNmsTopN_ = other.rpnPreNmsTopN_;
    rpnPostNmsTopN_ = other.rpnPostNmsTopN_;
    is_mindspore_ = other.is_mindspore_;
    order_by_py_ = other.order_by_py_;
    return *this;
}

bool GetConncectY(const std::vector<std::vector<bool>> &graph, int connectX, int &connectY)
{
    for (size_t y = 0; y < graph[0].size(); y++) {
        if (graph[connectX][y]) {
            connectY = int(y);
            return true;
        }
    }
    return false;
}

bool CtpnPostProcessDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    LogDebug << "Start to check the output tensor of model";
    if (tensors.size() < RIGHTBOTX) {
        LogError << "The number of tensors (" << tensors.size() << ") "
                 << "is less than required (" << RIGHTBOTX << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto bboxShape = tensors[BBOX_PRED].GetShape();
    auto classShape = tensors[CLS_PRED].GetShape();
    if (is_mindspore_) {
        if (tensors[BBOX_PRED].GetDataTypeSize() != qPtr_->TWO_BYTE ||
            tensors[CLS_PRED].GetDataTypeSize() != qPtr_->ONE_BYTE) {
            LogError << "Postprocess failed. Please check model output dataType!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        if (bboxShape.size() < RIGHTBOTX || classShape.size() < LEFTTOPY) {
            LogError << "The number of tensor[0] dimensions (" << bboxShape.size() << ") "
                     << "is not equal to (" << RIGHTBOTX << "); or number of tensor[0] dimensions (" <<
                     classShape.size() << ") "
                     << "is not equal to (" << LEFTTOPY << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    } else {
        if (tensors[BBOX_PRED].GetDataTypeSize() != qPtr_->FOUR_BYTE ||
            tensors[CLS_PRED].GetDataTypeSize() != qPtr_->FOUR_BYTE) {
            LogError << "Postprocess failed. Please check model output dataType!"
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
        if (bboxShape.size() != SCORE_DIM || classShape.size() != SCORE_DIM) {
            LogError << "The number of tensor[0] dimensions (" << bboxShape.size() << ") "
                     << "is not equal to (" << SCORE_DIM << "); or number of tensor[0] dimensions (" <<
                     classShape.size() << ") "
                     << "is not equal to (" << SCORE_DIM << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
    LogDebug << "End to check the output tensor of model";
    return true;
}

APP_ERROR CtpnPostProcessDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "Input tensors are invalid" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return qPtr_->CheckAndMoveTensors(tensors);
}

void GetSubGraph(std::vector<std::vector<bool>> &graph, std::vector<std::vector<int>> &subGraphs)
{
    std::vector<int> nonByConnects;
    for (size_t j = 0; j < graph.size(); j++) {
        bool nonByConnectLabel = true;
        for (size_t i = 0; i < graph[0].size(); i++) {
            if (graph[i][j]) {
                nonByConnectLabel = false;
                break;
            }
        }
        if (nonByConnectLabel) {
            nonByConnects.push_back(j);
        }
    }
    // Get connect left index
    std::vector<int> connects;
    for (auto nonByConnectIdx : nonByConnects) {
        for (size_t k = 0; k < graph[0].size(); k++) {
            if (graph[nonByConnectIdx][k]) {
                connects.push_back(nonByConnectIdx);
                break;
            }
        }
    }
    // Get connect right index
    for (auto connectX : connects) {
        std::vector<int> connectTemp = { connectX };
        subGraphs.push_back(connectTemp);
        int connectY;
        while (GetConncectY(graph, connectX, connectY)) {
            subGraphs.back().push_back(connectY);
            connectX = connectY;
        }
    }
}

void CtpnPostProcessDptr::ObjectDetectionOutput(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<TextObjectInfo>> &textObjInfos, const std::vector<ResizedImageInfo> &resizedImageInfos)
{
    LogDebug << "CtpnPostProcess start to write results.";
    auto shape = tensors[CLS_PRED].GetShape();
    uint32_t batchSize = shape[0];
    std::vector<std::vector<float>> textProposals;
    std::vector<float> scores;
    if (resizedImageInfos.size() < batchSize) {
        LogError << "Size of resizedImageInfos is invalid, size: " << resizedImageInfos.size()
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    for (uint32_t i = 0; i < batchSize; i++) {
        std::vector<TextObjectInfo> textObjInfo;
        ResizedImageInfo resizedImageInfo = resizedImageInfos[i];
        if (i < qPtr_->cropRoiBoxes_.size()) {
            resizedImageInfo.widthOriginal =
                static_cast<uint32_t>(qPtr_->cropRoiBoxes_[i].x1 - qPtr_->cropRoiBoxes_[i].x0);
            resizedImageInfo.heightOriginal =
                static_cast<uint32_t>(qPtr_->cropRoiBoxes_[i].y1 - qPtr_->cropRoiBoxes_[i].y0);
        }
        if (is_mindspore_) {
            LogDebug << "Welcome to enter mindspore version!";
            if (GetSrcDataAndNmsForMindSpore(tensors, i, textProposals, scores) != APP_ERR_OK) {
                return;
            }
        } else {
            LogDebug << "Welcome to enter tensorflow version!";
            std::vector<std::vector<int>> anchors;
            std::vector<std::vector<int>> wholeImgAnchors;
            // 1. Creat anchors (x0, y0, x1, y1)
            CreateAnchors(anchors);
            // 2. Get the real anchors of the whole img
            GetWholeImgAnchors(anchors, resizedImageInfo, wholeImgAnchors);
            // 3. Do nms and keep 100 proposals
            CtpnPostProcessInput inputInfo;
            inputInfo.tensors = &tensors;
            inputInfo.resizedImageInfos = &resizedImageInfo;
            inputInfo.wholeImgAnchors = &wholeImgAnchors;
            NonMaxSuppression(inputInfo, i, textProposals, scores);
        }
        // 4. Create graph, the value is bool. The value true show the two index box connect each other.
        size_t proposalSize = textProposals.size();
        std::vector<std::vector<bool>> graph(proposalSize, std::vector<bool>(proposalSize, false));
        BuildGraph(textProposals, scores, resizedImageInfo, graph);
        // 5. Get connective proposals from graph
        std::vector<std::vector<int>> subGraphs;
        GetSubGraph(graph, subGraphs);
        // 6. Fit box's y and get final box location
        GetTextLines(textProposals, scores, subGraphs, resizedImageInfo, textObjInfo);
        LogDebug << "Number of objects found : " << textObjInfo.size();
        GetTextObjInfoResult(resizedImageInfo, textObjInfo, i);
        textObjInfos.push_back(textObjInfo);
    }
    LogDebug << "CtpnPostProcess write results successed.";
}

int CtpnPostProcessDptr::DecimalToInteger(double decimal)
{
    if (decimal >= 0) {
        return floor(decimal);
    } else {
        return ceil(decimal);
    }
}

void CtpnPostProcessDptr::CreateAnchors(std::vector<std::vector<int>> &anchors)
{
    std::vector<int> anchorHeights { 11, 16, 23, 33, 48, 68, 97, 139, 198, 283 };
    const int anchorWeight = ANCHOR_SCALES;
    std::vector<float> baseAnchor = {
        0, 0, static_cast<float>(anchorScales_ - 1), static_cast<float>(anchorScales_ - 1)
    };
    for (const auto &anchorHeight : anchorHeights) {
        double xBase = (baseAnchor[LEFTTOPX] + baseAnchor[RIGHTBOTX]) * HALF_SCALE;
        double yBase = (baseAnchor[LEFTTOPY] + baseAnchor[RIGHTBOTY]) * HALF_SCALE;
        double scaledAnchorXMin = xBase - anchorWeight * HALF_SCALE;
        double scaledAnchorXMax = xBase + anchorWeight * HALF_SCALE;
        double scaledAnchorYMin = yBase - anchorHeight * HALF_SCALE;
        double scaledAnchorYMax = yBase + anchorHeight * HALF_SCALE;
        std::vector<int> anchor {
            DecimalToInteger(scaledAnchorXMin), DecimalToInteger(scaledAnchorYMin),
            DecimalToInteger(scaledAnchorXMax), DecimalToInteger(scaledAnchorYMax)
        };
        anchors.push_back(anchor);
    }
}

void CtpnPostProcessDptr::GetWholeImgAnchors(const std::vector<std::vector<int>> &anchors,
    const ResizedImageInfo &resizedImageInfos, std::vector<std::vector<int>> &wholeImgAnchors)
{
    std::vector<int> shiftX;
    std::vector<int> shiftY;
    int64_t featWidth = resizedImageInfos.widthResize / featStride_;   // 67
    int64_t featHeight = resizedImageInfos.heightResize / featStride_; // 38
    for (int64_t y = 0; y < featHeight; y++) {
        for (int64_t x = 0; x < featWidth; x++) {
            shiftX.push_back(x * featStride_);
            shiftY.push_back(y * featStride_);
        }
    }
    const int64_t totalSize = featHeight * featWidth;
    std::vector<std::vector<int>> shifts;
    shifts.push_back(shiftX);
    shifts.push_back(shiftY);
    shifts.push_back(shiftX);
    shifts.push_back(shiftY);
    int shiftsSize = (int)shifts.size();
    for (int64_t shiftValue = 0; shiftValue < totalSize; shiftValue++) {
        for (auto anchorValue = 0; anchorValue < anchorNum_; anchorValue++) {
            std::vector<int> wholeImgAnchor;
            for (auto location = 0; location < shiftsSize; location++) {
                wholeImgAnchor.push_back(anchors[anchorValue][location] + shifts[location][shiftValue]);
            }
            wholeImgAnchors.push_back(wholeImgAnchor);
        }
    }
}

void CtpnPostProcessDptr::GetRealBoxesOfImg(const std::vector<int> &wholeImgAnchor, float &x1, float &y1, float &x2,
    float &y2)
{
    if (wholeImgAnchor.size() == 0) {
        LogWarn << "All img anchor vector is empty, need to check";
        return;
    }
    // count x1 x2
    double anchorWeight = wholeImgAnchor[RIGHTBOTX] - wholeImgAnchor[LEFTTOPX] + 1;
    double anchorWeightMid = wholeImgAnchor[LEFTTOPX] + anchorWeight * HALF_SCALE;
    x1 = static_cast<float>(anchorWeightMid - HALF_SCALE * anchorWeight);
    x2 = static_cast<float>(anchorWeightMid + HALF_SCALE * anchorWeight);
    // count y1 y2
    double anchorHeight = wholeImgAnchor[RIGHTBOTY] - wholeImgAnchor[LEFTTOPY] + 1;
    double anchorHeightMid = wholeImgAnchor[LEFTTOPY] + anchorHeight * HALF_SCALE;
    double predY = y1 * anchorHeight + anchorHeightMid;
    double predHeight = std::exp(y2) * anchorHeight;
    y1 = static_cast<float>(predY - HALF_SCALE * predHeight);
    y2 = static_cast<float>(predY + HALF_SCALE * predHeight);
}

void CtpnPostProcessDptr::ClipBoxes(const ResizedImageInfo &resizedImageInfos,
    float &x0, float &y0, float &x1, float &y1)
{
    x0 = std::max(std::min(x0, static_cast<float>(resizedImageInfos.widthResize) - 1), 0.0f);
    y0 = std::max(std::min(y0, static_cast<float>(resizedImageInfos.heightResize) - 1), 0.0f);
    x1 = std::max(std::min(x1, static_cast<float>(resizedImageInfos.widthResize) - 1), 0.0f);
    y1 = std::max(std::min(y1, static_cast<float>(resizedImageInfos.heightResize) - 1), 0.0f);
}

bool CtpnPostProcessDptr::FilterBoxes(float &x0, float &y0, float &x1, float &y1)
{
    return !((x1 - x0 + 1 >= static_cast<float>(minSize_)) && (y1 - y0 + 1 >= static_cast<float>(minSize_)));
}

void CtpnPostProcessDptr::SelectTopKBoxesByScore(unsigned int num, std::vector<MxBase::DetectBox> &detBoxes)
{
    num = (num < detBoxes.size()) ? num : detBoxes.size();
    std::vector<DetectBox> sortBoxes;
    for (size_t i = 0; i != num; i++) {
        sortBoxes.push_back(detBoxes[i]);
    }
    detBoxes = std::move(sortBoxes);
}

APP_ERROR CtpnPostProcessDptr::FilterBoxesForReadSrcData(const CtpnPostProcessInput &input, CoorDim coorDim,
    std::vector<MxBase::DetectBox> &detBoxes, std::vector<TextObjectInfo> &textObjectInfos)
{
    const auto tensors = *(input.tensors);
    const auto resizedImageInfos = *(input.resizedImageInfos);
    const auto wholeImgAnchors = *(input.wholeImgAnchors);
    auto bboxPtr = (float *)qPtr_->GetBuffer(tensors[BBOX_PRED], coorDim.index);
    auto confidencePtr = (float *)qPtr_->GetBuffer(tensors[CLS_PRED], coorDim.index);
    if (bboxPtr == nullptr || confidencePtr == nullptr) {
        LogError << "The bboxPtr is nullptr or confidencePtr is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    int64_t featWidth = resizedImageInfos.widthResize / featStride_; // 67
    int64_t boxAnchorSize = (int64_t)featBoxLayer_ * anchorNum_;              // 40
    int64_t confAnchorSize = (int64_t)featConfLayer_ * anchorNum_;            // 20
    int64_t count = featWidth * anchorNum_ * coorDim.i + anchorNum_ * coorDim.j + coorDim.k;
    MxBase::DetectBox det;
    TextObjectInfo textObjectInfo;
    float x1 = bboxPtr[boxAnchorSize * coorDim.j + coorDim.k * featBoxLayer_ + LEFTTOPX +
                       boxAnchorSize * featWidth * coorDim.i];
    float y1 = bboxPtr[boxAnchorSize * coorDim.j + coorDim.k * featBoxLayer_ + LEFTTOPY +
                       boxAnchorSize * featWidth * coorDim.i];
    float x2 = bboxPtr[boxAnchorSize * coorDim.j + coorDim.k * featBoxLayer_ + RIGHTBOTX +
                       boxAnchorSize * featWidth * coorDim.i];
    float y2 = bboxPtr[boxAnchorSize * coorDim.j + coorDim.k * featBoxLayer_ + RIGHTBOTY +
                       boxAnchorSize * featWidth * coorDim.i];
    det.prob = confidencePtr[confAnchorSize * coorDim.j + coorDim.k * featConfLayer_ + CONF_OBJECT +
                             confAnchorSize * featWidth * coorDim.i];
    textObjectInfo.x0 = x1;
    textObjectInfo.y0 = y1;
    textObjectInfo.x1 = x2;
    textObjectInfo.x1 = x2;
    textObjectInfo.confidence = det.prob;
    // Get real box location
    GetRealBoxesOfImg(wholeImgAnchors[count], x1, y1, x2, y2);
    // Fix proposal, cut the part that more than image's size.
    ClipBoxes(resizedImageInfos, x1, y1, x2, y2);
    // Filter proposal of that size < minSize
    if (FilterBoxes(x1, y1, x2, y2) || det.prob <= textProposalsMinScore_) {
        return APP_ERR_OK;
    }
    textObjectInfos.push_back(textObjectInfo);
    LogDebug << "x1: " << x1 << ", y1: " << y1 << ", x2: " << x2 << ", y2: " << y2 << ", score: " << det.prob << ")";
    det.x = (x1 + x2) / COORDINATE_PARAM;
    det.y = (y1 + y2) / COORDINATE_PARAM;
    det.width = (x2 - x1 > std::numeric_limits<float>::epsilon()) ? (x2 - x1) : (x1 - x2);
    det.height = (y2 - y1 > std::numeric_limits<float>::epsilon()) ? (y2 - y1) : (y1 - y2);
    det.classID = 0;
    detBoxes.emplace_back(det);
    return APP_ERR_OK;
}

APP_ERROR CtpnPostProcessDptr::TraverseAnchor(CoorDim &coorDim, std::vector<MxBase::DetectBox> &detBoxes,
    const CtpnPostProcessInput &input, std::vector<TextObjectInfo> &textObjectInfos)
{
    for (int k = 0; k < anchorNum_; k++) {
        coorDim.k = k;
        APP_ERROR ret = FilterBoxesForReadSrcData(input, coorDim, detBoxes, textObjectInfos);
        if (ret != APP_ERR_OK) {
            LogError << "Filter boxes for read src data fialed."  << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR CtpnPostProcessDptr::ReadSrcData(const CtpnPostProcessInput &input, int index,
    std::vector<MxBase::DetectBox> &detBoxes, std::vector<TextObjectInfo> &textObjectInfos)
{
    const auto resizedImageInfos = *(input.resizedImageInfos);
    int64_t featWidth = resizedImageInfos.widthResize / featStride_;   // 67
    int64_t featHeight = resizedImageInfos.heightResize / featStride_; // 38
    for (int64_t i = 0; i < featHeight; i++) {
        for (int64_t j = 0; j < featWidth; j++) {
            CoorDim coorDim(i, j, 0, index);
            APP_ERROR ret = TraverseAnchor(coorDim, detBoxes, input, textObjectInfos);
            if (ret != APP_ERR_OK) {
                LogError << "TraverseAnchor fialed." << GetErrorInfo(ret);
                return ret;
            }
        }
    }
    return APP_ERR_OK;
}

void CtpnPostProcessDptr::NonMaxSuppression(const CtpnPostProcessInput &input, int index,
    std::vector<std::vector<float>> &textProposals, std::vector<float> &scores)
{
    std::vector<MxBase::DetectBox> detBoxes;
    std::vector<TextObjectInfo> textObjectInfos;
    ReadSrcData(input, index, detBoxes, textObjectInfos);
    // Order
    std::vector<int> order(textObjectInfos.size());
    if (order_by_py_) {
        SortIndexesByNpy(textObjectInfos, order, true);
    } else {
        SortIndexesByCpp(textObjectInfos, order);
    }
    std::vector<MxBase::DetectBox> textObjectInfosAfterOrder;
    for (auto i : order) {
        textObjectInfosAfterOrder.push_back(detBoxes[i]);
    }

    // Keep 12000 boxes
    SelectTopKBoxesByScore(rpnPreNmsTopN_, textObjectInfosAfterOrder);
    std::vector<DetectBox> sortBoxes;
    FilterByIou(textObjectInfosAfterOrder, sortBoxes, boxIouThresh_);
    SelectTopKBoxesByScore(rpnPostNmsTopN_, sortBoxes);

    LogDebug << "After first nms image, detBoxes2 size: " << sortBoxes.size();
    std::vector<MxBase::DetectBox> detBoxes2;
    for (const auto &detBox : sortBoxes) {
        if (detBox.prob > textProposalsMinScore_) {
            detBoxes2.push_back(detBox);
        }
    }
    std::vector<DetectBox> sortBoxes1;
    FilterByIou(detBoxes2, sortBoxes1, textIouThresh_);

    LogDebug << "After twice nms image, detBoxes2 size: " << sortBoxes1.size();
    for (auto &sortBox : sortBoxes1) {
        float score = sortBox.prob;
        float x1 = sortBox.x - sortBox.width / COORDINATE_PARAM;
        float y1 = sortBox.y - sortBox.height / COORDINATE_PARAM;
        float x2 = sortBox.x + sortBox.width / COORDINATE_PARAM;
        float y2 = sortBox.y + sortBox.height / COORDINATE_PARAM;
        std::vector<float> textProposal = { x1, y1, x2, y2 };
        textProposals.push_back(textProposal);
        scores.push_back(score);
    }
}

bool CtpnPostProcessDptr::CalcOverLaps(const int &index1, const int &index2,
    const std::vector<std::vector<float>> &textProposals)
{
    // cal over laps
    float h1 = textProposals[index1][RIGHTBOTY] - textProposals[index1][LEFTTOPY] + 1;
    float h2 = textProposals[index2][RIGHTBOTY] - textProposals[index2][LEFTTOPY] + 1;
    float y0 = std::max(textProposals[index2][LEFTTOPY], textProposals[index1][LEFTTOPY]);
    float y1 = std::min(textProposals[index2][RIGHTBOTY], textProposals[index1][RIGHTBOTY]);
    if (IsDenominatorZero(h1) || IsDenominatorZero(h2)) {
        LogWarn << "h1 is zero: " << h1 << "or h2 is zero: " << h2 << ", when calculate overlaps.";
        return false;
    }
    float overLaps = std::max(0.0f, y1 - y0 + 1) / std::min(h1, h2);
    // cal similarity
    float similarity = std::min(h1, h2) / std::max(h1, h2);
    return (overLaps >= minOverLaps_ && similarity >= minSizeSim_);
}

void CtpnPostProcessDptr::GetSuccessions(const std::vector<std::vector<float>> &textProposals,
    const std::vector<std::vector<int>> &boxesTable, const ResizedImageInfo &resizedImageInfo,
    const int &index, std::vector<int> &successions)
{
    // Get positive connective anchors
    std::vector<float> box = textProposals[index];
    int leftMaxIdx = std::min((uint32_t)box[LEFTTOPX] + maxHorizontalGap_ + 1, resizedImageInfo.widthResize);
    for (auto left = static_cast<int>(box[LEFTTOPX]) + 1; left >=0 && left < leftMaxIdx; left++) {
        std::vector<int> adjBoxIndices = boxesTable[left];
        for (auto adjBoxIndex : adjBoxIndices) {
            if (CalcOverLaps(adjBoxIndex, index, textProposals)) {
                successions.push_back(adjBoxIndex);
            }
        }
        if (!successions.empty()) {
            return;
        }
    }
}

void CtpnPostProcessDptr::GetPrecursors(const std::vector<std::vector<float>> &textProposals,
    const std::vector<std::vector<int>> &boxesTable, const int &successionIdx, const int &index,
    std::vector<int> &precursors)
{
    // Get negative connective anchors
    std::vector<float> box = textProposals[successionIdx];
    int leftMaxNegIdx = std::max((int)box[LEFTTOPX] - maxHorizontalGap_, 0) - 1;
    for (auto left = (int)box[LEFTTOPX] - 1; left > leftMaxNegIdx; left--) {
        std::vector<int> adjBoxIndices = boxesTable[left];
        for (auto adjBoxIndex : adjBoxIndices) {
            if (CalcOverLaps(adjBoxIndex, index, textProposals)) {
                precursors.push_back(adjBoxIndex);
            }
        }
        if (!precursors.empty()) {
            return;
        }
    }
}

void CtpnPostProcessDptr::BuildGraph(const std::vector<std::vector<float>> &textProposals,
    const std::vector<float> &scores, const ResizedImageInfo &resizedImageInfo, std::vector<std::vector<bool>> &graph)
{
    // Put index into boxes table of x0 value of proposal, it's like histogram.
    std::vector<std::vector<int>> boxesTable(resizedImageInfo.widthResize, std::vector<int>());
    int boxesTableIdx = 0;
    for (const auto &textProposal : textProposals) {
        auto tableIndex = static_cast<uint32_t>(textProposal[LEFTTOPX]);
        if (tableIndex < resizedImageInfo.widthResize) {
            boxesTable[tableIndex].push_back(boxesTableIdx);
        }
        boxesTableIdx++;
    }
    int boxesSize = (int)textProposals.size();
    for (auto index = 0; index < boxesSize; index++) {
        std::vector<int> successions;
        // Get succession connective anchors
        GetSuccessions(textProposals, boxesTable, resizedImageInfo, index, successions);
        if (successions.empty()) {
            continue;
        }
        // Get the index of max score from successions
        int successionIdx = 0;
        float successionScoreMax = INTMAX_MIN;
        for (auto succession : successions) {
            if (scores[succession] > successionScoreMax) {
                successionIdx = succession;
                successionScoreMax = scores[succession];
            }
        }

        // Get precursors connective anchors
        std::vector<int> precursors;
        GetPrecursors(textProposals, boxesTable, successionIdx, index, precursors);
        if (precursors.empty()) {
            continue;
        }
        // Get the index of max score from negative successions
        float precursorScoreMax = INTMAX_MIN;
        for (auto precursor : precursors) {
            if (scores[precursor] > precursorScoreMax) {
                precursorScoreMax = scores[precursor];
            }
        }
        if (scores[index] >= precursorScoreMax) {
            graph[index][successionIdx] = true;
        }
    }
}

void setTextLineBoxesValue(const std::vector<std::vector<float>> &textLineBoxes,
                           std::vector<float> &textLineBoxesMidX,
                           std::vector<float> &textLineBoxesMidY,
                           std::vector<float> &textLineBoxesHeight)
{
    for (auto textLineBox : textLineBoxes) {
        textLineBoxesMidX.push_back(float((textLineBox[LEFTTOPX] + textLineBox[RIGHTBOTX]) * HALF_SCALE));
        textLineBoxesMidY.push_back(float((textLineBox[LEFTTOPY] + textLineBox[RIGHTBOTY]) * HALF_SCALE));
        textLineBoxesHeight.push_back(float(textLineBox[RIGHTBOTY] - textLineBox[LEFTTOPY]));
    }
}

void GetTextLinesOriented(const std::vector<std::vector<float>> &textLineBoxes, const float &index0,
    const float &index2, const float &score, TextObjectInfo &textRec)
{
    std::vector<float> textLineBoxesMidX;
    std::vector<float> textLineBoxesMidY;
    std::vector<float> textLineBoxesHeight;
    setTextLineBoxesValue(textLineBoxes, textLineBoxesMidX, textLineBoxesMidY, textLineBoxesHeight);
    LineRegressionFit lineRegressionFitMid;
    lineRegressionFitMid.SetAlphaAndBeta(textLineBoxesMidX, textLineBoxesMidY);
    if (textLineBoxesHeight.empty()) {
        return;
    }
    double heightOffsize = std::accumulate(textLineBoxesHeight.begin(), textLineBoxesHeight.end(), 0.0) /
            textLineBoxesHeight.size() + HEIGHT_OFFSIZE;
    double y1 = lineRegressionFitMid.alpha_ * index0 + (lineRegressionFitMid.beta_ - heightOffsize * HALF_SCALE);
    double y2 = lineRegressionFitMid.alpha_ * index2 + (lineRegressionFitMid.beta_ - heightOffsize * HALF_SCALE);
    double y3 = lineRegressionFitMid.alpha_ * index0 + (lineRegressionFitMid.beta_ + heightOffsize * HALF_SCALE);
    double y4 = lineRegressionFitMid.alpha_ * index2 + (lineRegressionFitMid.beta_ + heightOffsize * HALF_SCALE);
    double disX = std::fabs(index2 - index0); // right up - left up
    double disY = std::fabs(y2 - y1);
    double width = std::sqrt(disX * disX + disY * disY); // text line width
    if (IsDenominatorZero(width)) {
        LogError << "The value of width must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    double fTmp1 = (y3 - y1) * disY / width;
    double x = std::fabs(fTmp1 * disX / width);
    double y = std::fabs(fTmp1 * disY / width);
    if (lineRegressionFitMid.alpha_ < 0) {
        textRec.x0 = index0 - x;
        textRec.y0 = y1 + y;
        textRec.x1 = index2;
        textRec.y1 = y2;
        textRec.x2 = index2 + x;
        textRec.y2 = y4 - y;
        textRec.x3 = index0;
        textRec.y3 = y3;
    } else {
        textRec.x0 = index0;
        textRec.y0 = y1;
        textRec.x1 = index2 + x;
        textRec.y1 = y2 + y;
        textRec.x2 = index2;
        textRec.y2 = y4;
        textRec.x3 = index0 - x;
        textRec.y3 = y3 - y;
    }
    textRec.confidence = score;
}

bool CtpnPostProcessDptr::JudgeObjInfo(double widths, double heights, float confidence)
{
    if (IsDenominatorZero(heights)) {
        LogError << "The heights is invalid parameter, please check it first. "
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return (widths / heights > minRatio_) && (confidence > lineMinScore_) &&
           (widths > textProposalsWidth_ * minNumProposals_);
}

bool CtpnPostProcessDptr::FilterBoxesFinal(TextObjectInfo textObjDetectInfo)
{
    if (is_mindspore_) {
        double heights = std::fabs(textObjDetectInfo.y2 - textObjDetectInfo.y0);
        if (IsDenominatorZero(heights)) {
            LogWarn << "is_mindspore_ is true: " << is_mindspore_ << ", heights is zero: " << heights;
            return false;
        }
        double widths = std::fabs(textObjDetectInfo.x2 - textObjDetectInfo.x0);
        return JudgeObjInfo(widths, heights, textObjDetectInfo.confidence);
    } else {
        double heights = (std::fabs(textObjDetectInfo.y2 - textObjDetectInfo.y0) +
                         std::fabs(textObjDetectInfo.y3 - textObjDetectInfo.y1)) *
                         HALF_SCALE + 1;
        if (IsDenominatorZero(heights)) {
            LogWarn << "heights is zero: " << heights;
            return false;
        }
        double widths = (std::fabs(textObjDetectInfo.x1 - textObjDetectInfo.x0) +
                        std::fabs(textObjDetectInfo.x3 - textObjDetectInfo.x2)) *
                        HALF_SCALE + 1;
        return JudgeObjInfo(widths, heights, textObjDetectInfo.confidence);
    }
}

void CtpnPostProcessDptr::GetTextLines(const std::vector<std::vector<float>> &textProposals,
    const std::vector<float> &scores, const std::vector<std::vector<int>> &subGraphs,
    const ResizedImageInfo &resizedImageInfos, std::vector<TextObjectInfo> &textRecs)
{
    for (const auto &subGraph : subGraphs) {
        std::vector<std::vector<float>> textLineBoxes;
        std::vector<float> textBoxesScores;
        for (auto index : subGraph) {
            textLineBoxes.push_back(textProposals[index]);
            textBoxesScores.push_back(scores[index]);
        }
        std::vector<float> textLineBoxesX0;
        std::vector<float> textLineBoxesX1;
        std::vector<float> textLineBoxesY0;
        std::vector<float> textLineBoxesY1;
        GetInfo(textLineBoxes, textLineBoxesX0, textLineBoxesX1, textLineBoxesY0, textLineBoxesY1);
        float textLineX0 = *std::min_element(textLineBoxesX0.begin(), textLineBoxesX0.end());
        float textLineX1 = *std::max_element(textLineBoxesX1.begin(), textLineBoxesX1.end());
        float offset = (textLineBoxes[LEFTTOPX][RIGHTBOTX] - textLineBoxes[LEFTTOPX][LEFTTOPX]) * HALF_SCALE;
        LineRegressionFit lrFitTop;
        lrFitTop.SetAlphaAndBeta(textLineBoxesX0, textLineBoxesY0);
        LineRegressionFit lrFitBottom;
        lrFitBottom.SetAlphaAndBeta(textLineBoxesX0, textLineBoxesY1);
        float y0 = std::min(lrFitTop.LRFunction(textLineX0 + offset), lrFitTop.LRFunction(textLineX1 - offset));
        float y1 = std::max(lrFitBottom.LRFunction(textLineX0 + offset), lrFitBottom.LRFunction(textLineX1 - offset));
        if (textBoxesScores.empty()) {
            LogError << "The divided value is invalid parameter, please check it first."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        float score = std::accumulate(textBoxesScores.begin(), textBoxesScores.end(), 0.0) / textBoxesScores.size();
        // Direct is left down/right down/right up/left up
        TextObjectInfo textObjDetectInfo {};
        if (isOriented_) {
            GetTextLinesOriented(textLineBoxes, textLineX0, textLineX1, score, textObjDetectInfo);
        } else {
            ClipBoxes(resizedImageInfos, textLineX0, y0, textLineX1, y1);
            textObjDetectInfo.x0 = textLineX0;
            textObjDetectInfo.y0 = y0;
            textObjDetectInfo.x1 = textLineX1;
            textObjDetectInfo.y1 = y0;
            textObjDetectInfo.x2 = textLineX1;
            textObjDetectInfo.y2 = y1;
            textObjDetectInfo.x3 = textLineX0;
            textObjDetectInfo.y3 = y1;
            textObjDetectInfo.confidence = score;
        }
        if (!FilterBoxesFinal(textObjDetectInfo)) {
            continue;
        }
        textRecs.push_back(textObjDetectInfo);
    }
}

void CtpnPostProcessDptr::GetInfo(std::vector<std::vector<float>> &textLineBoxes, std::vector<float> &textLineBoxesX0,
                                  std::vector<float> &textLineBoxesX1, std::vector<float> &textLineBoxesY0,
                                  std::vector<float> &textLineBoxesY1) const
{
    if (textLineBoxes.empty()) {
        LogError << "The textLineBoxes is empty, please check it first." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    for (auto textLineBox : textLineBoxes) {
        textLineBoxesX0.push_back(textLineBox[LEFTTOPX]);
        textLineBoxesY0.push_back(textLineBox[LEFTTOPY]);
        textLineBoxesX1.push_back(textLineBox[RIGHTBOTX]);
        textLineBoxesY1.push_back(textLineBox[RIGHTBOTY]);
    }
}

void NmsForCtpn(const float &iouThresh, std::vector<TextObjectInfo> &textObjectInfos,
    std::vector<std::vector<float>> &textProposals, std::vector<float> &scores, std::vector<int> order)
{
    std::vector<int> suppressed(textObjectInfos.size(), 0);
    for (size_t _i = 0; _i < textObjectInfos.size(); _i++) {
        int i = order[_i];
        float area =
            (textObjectInfos[i].x1 - textObjectInfos[i].x0 + 1) * (textObjectInfos[i].y1 - textObjectInfos[i].y0 + 1);
        if (suppressed[i] == 1) {
            continue;
        }
        std::vector<float> textProposal = {
            textObjectInfos[i].x0, textObjectInfos[i].y0,
            textObjectInfos[i].x1, textObjectInfos[i].y1
        };
        textProposals.push_back(textProposal);
        scores.push_back(textObjectInfos[i].confidence);
        float x0Temp = textObjectInfos[i].x0;
        float y0Temp = textObjectInfos[i].y0;
        float x1Temp = textObjectInfos[i].x1;
        float y1Temp = textObjectInfos[i].y1;
        for (size_t _j = _i + 1; _j < textObjectInfos.size(); _j++) {
            int j = order[_j];
            if (suppressed[j] == 1) {
                continue;
            }
            float areaJ = (textObjectInfos[j].x1 - textObjectInfos[j].x0 + 1) *
                    (textObjectInfos[j].y1 - textObjectInfos[j].y0 + 1);
            float x0TempJ = std::max(x0Temp, textObjectInfos[j].x0);
            float y0TempJ = std::max(y0Temp, textObjectInfos[j].y0);
            float x1TempJ = std::min(x1Temp, textObjectInfos[j].x1);
            float y1TempJ = std::min(y1Temp, textObjectInfos[j].y1);
            float w = std::max(0.0f, (x1TempJ - x0TempJ + 1));
            float h = std::max(0.0f, (y1TempJ - y0TempJ + 1));
            float inter = w * h;
            if (IsDenominatorZero((area + areaJ - inter))) {
                LogError << "The divided value is invalid parameter, please check it first. "
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return;
            }
            float overLap = inter / (area + areaJ - inter);
            if (overLap >= iouThresh) {
                suppressed[j] = 1;
            }
        }
    }
}

void CtpnPostProcessDptr::SortIndexesByCpp(const std::vector<TextObjectInfo> &v, std::vector<int> &order)
{
    // initialize original index locations
    iota(order.begin(), order.end(), 0);
    // sort indexes based on comparing values in v
    sort(order.begin(), order.end(), [&v](int i1, int i2) { return v[i1].confidence > v[i2].confidence; });
}

void CtpnPostProcessDptr::SortIndexesByNpy(const std::vector<TextObjectInfo> &textObjectInfos,
    std::vector<int> &order, bool reverse)
{
    iota(order.begin(), order.end(), 0);
    std::vector<float> value;
    for (const auto &textConf : textObjectInfos) {
        value.push_back(textConf.confidence);
    }
    NpySort npySort(value, order);
    npySort.NpyArgQuickSort(reverse);
    order = npySort.GetSortIdx();
}

APP_ERROR CtpnPostProcessDptr::GetSrcDataAndNmsForMindSpore(const std::vector<TensorBase> &tensors, const int &index,
    std::vector<std::vector<float>> &textProposals, std::vector<float> &scores)
{
    auto bboxPtr = (aclFloat16 *)qPtr_->GetBuffer(tensors[BBOX_PRED], index);
    auto classIdPtr = (int8_t *)qPtr_->GetBuffer(tensors[CLS_PRED], index);
    if (bboxPtr == nullptr || classIdPtr == nullptr) {
        LogError << "The bboxPtr is nullptr or classIdPtr is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    std::vector<MxBase::DetectBox> detBoxes;
    std::vector<TextObjectInfo> textObjectInfos;
    for (int i = 0; i < OBJ_NUM_MINDSPORE * RESULT_DIM; i += RESULT_DIM) {
        TextObjectInfo textObjectInfo;
        textObjectInfo.x0 = aclFloat16ToFloat(bboxPtr[i + LEFTTOPX]) * classIdPtr[i / RESULT_DIM];
        textObjectInfo.y0 = aclFloat16ToFloat(bboxPtr[i + LEFTTOPY]) * classIdPtr[i / RESULT_DIM];
        textObjectInfo.x1 = aclFloat16ToFloat(bboxPtr[i + RIGHTBOTX]) * classIdPtr[i / RESULT_DIM];
        textObjectInfo.y1 = aclFloat16ToFloat(bboxPtr[i + RIGHTBOTY]) * classIdPtr[i / RESULT_DIM];
        textObjectInfo.confidence = aclFloat16ToFloat(bboxPtr[i + SCORE_DIM]) * classIdPtr[i / RESULT_DIM];
        LogDebug << "get value for model:(x0: " << textObjectInfo.x0 << ", y0: " << textObjectInfo.y0 << ", x1: " <<
                 textObjectInfo.x1 << ", y1: " << textObjectInfo.y1 << ", score: " << textObjectInfo.confidence << ")"
                 << " || No." << i / RESULT_DIM;
        if (textObjectInfo.confidence <= textProposalsMinScore_) {
            continue;
        }
        textObjectInfos.emplace_back(textObjectInfo);
    }
    std::vector<int> order(textObjectInfos.size());
    if (order_by_py_) {
        SortIndexesByNpy(textObjectInfos, order, true);
    } else {
        SortIndexesByCpp(textObjectInfos, order);
    }
    std::vector<TextObjectInfo> textObjectInfosAfterOrder;
    LogDebug << "textObjectInfos size: " << textObjectInfos.size() << ", order size: " << order.size();
    if (!order.empty() && *std::max_element(order.begin(), order.end()) >= (int)textObjectInfos.size()) {
        LogError << "Please check your order result of text object information."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    for (auto i : order) {
        textObjectInfosAfterOrder.push_back(textObjectInfos[i]);
    }
    NmsForCtpn(textIouThresh_, textObjectInfosAfterOrder, textProposals, scores, order);
    if (!textObjectInfosAfterOrder.empty() && textProposals.empty()) {
        LogWarn << "text object is empty after num processing, please turn up TEXT_IOU_THRESH.";
    }
    return APP_ERR_OK;
}

void CtpnPostProcessDptr::GetTextObjInfoResult(const ResizedImageInfo &resizedImageInfo,
    std::vector<TextObjectInfo> &textObjInfo, uint32_t batchIdx)
{
    if (IsDenominatorZero(resizedImageInfo.widthResize) || IsDenominatorZero(resizedImageInfo.heightResize)) {
        LogWarn << "resizedImageInfo.widthResize is " << resizedImageInfo.widthResize
                << ", resizedImageInfo.heightResize is " << resizedImageInfo.heightResize;
        return;
    }
    for (auto &textRec : textObjInfo) {
        textRec.x0 = textRec.x0 / resizedImageInfo.widthResize;
        textRec.x1 = textRec.x1 / resizedImageInfo.widthResize;
        textRec.x2 = textRec.x2 / resizedImageInfo.widthResize;
        textRec.x3 = textRec.x3 / resizedImageInfo.widthResize;
        textRec.y0 = textRec.y0 / resizedImageInfo.heightResize;
        textRec.y1 = textRec.y1 / resizedImageInfo.heightResize;
        textRec.y2 = textRec.y2 / resizedImageInfo.heightResize;
        textRec.y3 = textRec.y3 / resizedImageInfo.heightResize;
        qPtr_->ResizeReduction(resizedImageInfo, textRec);
        if (batchIdx < qPtr_->cropRoiBoxes_.size()) {
            textRec.x0 += qPtr_->cropRoiBoxes_[batchIdx].x0;
            textRec.x1 += qPtr_->cropRoiBoxes_[batchIdx].x0;
            textRec.x2 += qPtr_->cropRoiBoxes_[batchIdx].x0;
            textRec.x3 += qPtr_->cropRoiBoxes_[batchIdx].x0;
            textRec.y0 += qPtr_->cropRoiBoxes_[batchIdx].y0;
            textRec.y1 += qPtr_->cropRoiBoxes_[batchIdx].y0;
            textRec.y2 += qPtr_->cropRoiBoxes_[batchIdx].y0;
            textRec.y3 += qPtr_->cropRoiBoxes_[batchIdx].y0;
        }
        LogDebug << "Find object: "
                 << "confidence(" << textRec.confidence << "), Coordinates(" << textRec.x0 << ", " << textRec.y0 <<
                 "; " << textRec.x1 << ", " << textRec.y1 << "; " << textRec.x2 << ", " << textRec.y2 << "; " <<
                 textRec.x3 << ", " << textRec.y3 << ").";
    }
}

void CtpnPostProcessDptr::GetConfigValue()
{
    APP_ERROR ret = qPtr_->configData_.GetFileValue<int>("MAX_HORIZONTAL_GAP", maxHorizontalGap_, 0x0, 0x3e8);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read MAX_HORIZONTAL_GAP from config, default is: "
                << maxHorizontalGap_;
    }
    ret = qPtr_->configData_.GetFileValue<float>("BOX_IOU_THRESH", boxIouThresh_, 0.0f, 1.0f);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read BOX_IOU_THRESH from config, default is: "
                << boxIouThresh_;
    }
    ret = qPtr_->configData_.GetFileValue<float>("TEXT_IOU_THRESH", textIouThresh_, 0.0f, 1.0f);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read TEXT_IOU_THRESH from config, default is: "
                << textIouThresh_;
    }
    ret = qPtr_->configData_.GetFileValue<float>("TEXT_PROPOSALS_MIN_SCORE", textProposalsMinScore_, 0.0f, 1.0f);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read TEXT_PROPOSALS_MIN_SCORE from config, default is: "
                << textProposalsMinScore_;
    }
    ret = qPtr_->configData_.GetFileValue<float>("LINE_MIN_SCORE", lineMinScore_, 0.0f, 1.0f);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read LINE_MIN_SCORE from config, default is: " << lineMinScore_;
    }
    ret = qPtr_->configData_.GetFileValue<bool>("IS_ORIENTED", isOriented_);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read IS_ORIENTED from config, default is: "
                << isOriented_;
    }
    ret = qPtr_->configData_.GetFileValue<bool>("IS_MINDSPORE", is_mindspore_);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read IS_MINDSPORE from config, default is: "
                << is_mindspore_;
    }
    ret = qPtr_->configData_.GetFileValue<bool>("ORDER_BY_PY", order_by_py_);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read ORDER_BY_PY from config, default is: "
                << order_by_py_;
    }
}
}
#endif
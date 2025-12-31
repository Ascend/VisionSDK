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
 * Description: Used for post-processing of Ctpn models.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/ModelPostProcessors/OcrPostProcessors/CtpnPostProcessor.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include "MxBase/Maths/MathFunction.h"
#include "MxBase/GlobalManager/GlobalManager.h"

namespace {
const int BBOX_PRED = 0;
const int CLS_PRED = 1;
}

namespace {
const int LEFTTOPX = 0;
const int LEFTTOPY = 1;
const int RIGHTBOTX = 2;
const int RIGHTBOTY = 3;
const int CONF_OBJECT = 1;
}

namespace MxBase {
const int MAX_IMAGE_EDGE = 8192;
const float HALF_SCALE = 0.5;
void CtpnPostProcessor::ReadParamConfig(const std::string &configValue, bool &keyValue)
{
    APP_ERROR ret = configData_.GetFileValue<bool>(configValue, keyValue);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Failed to read from config, default value: " << keyValue;
    }
    LogDebug << "Success to read from config, default value: " << keyValue;
}

void CtpnPostProcessor::ReadParamConfig(const std::string &configValue, int &keyValue)
{
    APP_ERROR ret = configData_.GetFileValue<int>(configValue, keyValue, 0x0, 0x3e8);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Failed to read from config, default value: " << keyValue;
    }
    LogDebug << "Success to read from config, default value: " << keyValue;
}

void CtpnPostProcessor::ReadParamConfig(const std::string &configValue, float &keyValue)
{
    APP_ERROR ret = configData_.GetFileValue<float>(configValue, keyValue, 0x0, 0x3e8);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Failed to read from config, default value: " << keyValue;
    }
    LogDebug << "Success to read from config, default value: " << keyValue;
}

CtpnPostProcessor::CtpnPostProcessor()
    : minSize_(MIN_SIZE),
      featStride_(FEAT_STRIDE),
      anchorScales_(ANCHOR_SCALES),
      featBoxLayer_(FEAT_BOX_LAYER),
      featConfLayer_(FEAT_CONF_LAYER),
      anchorNum_(ANCHORNUM),
      maxHorizontalGap_(MAX_HORIZONTAL_GAP),
      minOverLaps_(MIN_OVER_LAPS),
      minSizeSim_(MIN_SIZE_SIM),
      isOriented_(IS_ORIENTED),
      boxIouThresh_(BOX_IOU_THRESH),
      textIouThresh_(TEXT_IOU_THRESH),
      minRatio_(MIN_RATIO),
      textProposalsMinScore_(TEXT_PROPOSALS_MIN_SCORE),
      lineMinScore_(LINE_MIN_SCORE),
      textProposalsWidth_(TEXT_PROPOSALS_WIDTH),
      minNumProposals_(MIN_NUM_PROPOSALS),
      rpnPreNmsTopN_(RPN_PRE_NMS_TOP_N),
      rpnPostNmsTopN_(RPN_POST_NMS_TOP_N)
{}

CtpnPostProcessor::~CtpnPostProcessor() {}

/*
 * @description Load the configs and labels from the file.
 * @param labelPath config path and label path.
 * @return APP_ERROR error code.
 */
APP_ERROR CtpnPostProcessor::Init(const std::string &configPath, const std::string &,
    MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize CtpnPostProcessor.";
    // Open config file
    MxBase::ConfigUtil util;
    APP_ERROR ret = util.LoadConfiguration(configPath, configData_, MxBase::CONFIGFILE);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to load configuration, config path invalidate." << GetErrorInfo(ret);
        return ret;
    }

    // read param
    ReadParamConfig("MIN_SIZE", minSize_);
    ReadParamConfig("FEAT_STRIDE", featStride_);
    ReadParamConfig("ANCHOR_SCALES", anchorScales_);
    ReadParamConfig("FEAT_BOX_LAYER", featBoxLayer_);
    ReadParamConfig("FEAT_CONF_LAYER", featConfLayer_);
    ReadParamConfig("ANCHORNUM", anchorNum_);
    ReadParamConfig("MAX_HORIZONTAL_GAP", maxHorizontalGap_);
    ReadParamConfig("MIN_OVER_LAPS", minOverLaps_);
    ReadParamConfig("MIN_SIZE_SIM", minSizeSim_);
    ReadParamConfig("IS_ORIENTED", isOriented_);
    ReadParamConfig("BOX_IOU_THRESH", boxIouThresh_);
    ReadParamConfig("TEXT_IOU_THRESH", textIouThresh_);
    ReadParamConfig("MIN_RATIO", minRatio_);
    ReadParamConfig("TEXT_PROPOSALS_MIN_SCORE", textProposalsMinScore_);
    ReadParamConfig("LINE_MIN_SCORE", lineMinScore_);
    ReadParamConfig("TEXT_PROPOSALS_WIDTH", textProposalsWidth_);
    ReadParamConfig("MIN_NUM_PROPOSALS", minNumProposals_);
    ReadParamConfig("RPN_PRE_NMS_TOP_N", rpnPreNmsTopN_);
    ReadParamConfig("RPN_POST_NMS_TOP_N", rpnPostNmsTopN_);

    ret = GetModelTensorsShape(modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to get model tensors shape" << GetErrorInfo(ret);
        return ret;
    }

    LogInfo << "End to initialize CtpnPostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Do nothing temporarily.
 * @return: APP_ERROR error code.
 */
APP_ERROR CtpnPostProcessor::DeInit()
{
    LogInfo << "Begin to deinit CtpnPostProcessor.";
    LogInfo << "End to deinit CtpnPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR CheckImageParamsValid(MxBase::PostImageInfo &postImageInfo)
{
    if ((int)postImageInfo.widthResize > MAX_IMAGE_EDGE || (int)postImageInfo.heightResize > MAX_IMAGE_EDGE) {
        LogError << "Invalid postImageInfo resize width or height." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((int)postImageInfo.widthOriginal > MAX_IMAGE_EDGE || (int)postImageInfo.heightOriginal > MAX_IMAGE_EDGE) {
        LogError << "Invalid postImageInfo original width or height." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((int)postImageInfo.x0 > MAX_IMAGE_EDGE || (int)postImageInfo.x1 > MAX_IMAGE_EDGE) {
        LogError << "Invalid postImageInfo x coordinates" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((int)postImageInfo.y0 > MAX_IMAGE_EDGE || (int)postImageInfo.y1 > MAX_IMAGE_EDGE) {
        LogError << "Invalid postImageInfo y coordinates." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((int)postImageInfo.x0 < 0 || (int)postImageInfo.x1 < 0) {
        LogError << "Invalid postImageInfo x coordinates." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((int)postImageInfo.y0 < 0 || (int)postImageInfo.y1 < 0) {
        LogError << "Invalid postImageInfo y coordinates." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR CtpnPostProcessor::Process(std::vector<std::shared_ptr<void>> &featLayerData,
    std::vector<TextObjDetectInfo> &textObjInfos, const bool useMpPictureCrop, MxBase::PostImageInfo postImageInfo)
{
    LogDebug << "Begin to process CtpnPostProcessor.";
    if (CheckImageParamsValid(postImageInfo) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ImageInfo imgInfo;
    int xOffset = 0;
    int yOffset = 0;
    imgInfo.modelWidth = (int)postImageInfo.widthResize;
    imgInfo.modelHeight = (int)postImageInfo.heightResize;
    if (useMpPictureCrop) {
        imgInfo.imgWidth = (int)(postImageInfo.x1 - postImageInfo.x0);
        imgInfo.imgHeight = (int)(postImageInfo.y1 - postImageInfo.y0);
        xOffset = (int)postImageInfo.x0;
        yOffset = (int)postImageInfo.y0;
    } else {
        imgInfo.imgWidth = (int)postImageInfo.widthOriginal;
        imgInfo.imgHeight = (int)postImageInfo.heightOriginal;
    }
    if (imgInfo.imgWidth < 0 || imgInfo.imgHeight < 0) {
        LogError << "Invalid crop parameters." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = ObjectDetectionOutput(featLayerData, textObjInfos, imgInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to executed ObjectDetectionOutput" << GetErrorInfo(ret);
        return ret;
    }
    if (useMpPictureCrop) {
        for (auto &textObjInfo : textObjInfos) {
            textObjInfo.x0 += xOffset;
            textObjInfo.x1 += xOffset;
            textObjInfo.x2 += xOffset;
            textObjInfo.x3 += xOffset;
            textObjInfo.y0 += yOffset;
            textObjInfo.y1 += yOffset;
            textObjInfo.y2 += yOffset;
            textObjInfo.y3 += yOffset;
        }
    }
    LogDebug << "End to process CtpnPostProcessor.";
    return APP_ERR_OK;
}

int CtpnPostProcessor::DecimalToInteger(double decimal)
{
    if (decimal >= 0) {
        return floor(decimal);
    } else {
        return ceil(decimal);
    }
}

void CtpnPostProcessor::CreateAnchors(std::vector<std::vector<int>> &anchors)
{
    std::vector<int> anchorHeights { 11, 16, 23, 33, 48, 68, 97, 139, 198, 283 };
    const int anchorWeight = ANCHOR_SCALES;
    std::vector<float> baseAnchor {
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

APP_ERROR CtpnPostProcessor::GetWholeImgAnchors(const std::vector<std::vector<int>> &anchors, const ImageInfo &imgInfo,
    std::vector<std::vector<int>> &wholeImgAnchors)
{
    std::vector<int> shiftX;
    std::vector<int> shiftY;
    if (IsDenominatorZero(featStride_)) {
        LogError << "The value of featStride_ of must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (anchors.size() < static_cast<uint32_t>(anchorNum_)) {
        LogError << "Check size of anchors failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    int featWidth = imgInfo.modelWidth / featStride_;   // 67
    int featHeight = imgInfo.modelHeight / featStride_; // 38
    for (auto y = 0; y < featHeight; y++) {
        for (auto x = 0; x < featWidth; x++) {
            shiftX.push_back(x * featStride_);
            shiftY.push_back(y * featStride_);
        }
    }
    const int totalSize = featHeight * featWidth;
    std::vector<std::vector<int>> shifts;
    shifts.push_back(shiftX);
    shifts.push_back(shiftY);
    shifts.push_back(shiftX);
    shifts.push_back(shiftY);
    int shiftsSize = (int)shifts.size();
    for (auto shiftValue = 0; shiftValue < totalSize; shiftValue++) {
        for (auto anchorValue = 0; anchorValue < anchorNum_; anchorValue++) {
            std::vector<int> wholeImgAnchor;
            for (auto location = 0; location < shiftsSize; location++) {
                wholeImgAnchor.push_back(anchors[anchorValue][location] + shifts[location][shiftValue]);
            }
            wholeImgAnchors.push_back(wholeImgAnchor);
        }
    }
    return APP_ERR_OK;
}

void CtpnPostProcessor::GetRealBoxesOfImg(const std::vector<int> &wholeImgAnchor, float &x1, float &y1, float &x2,
    float &y2)
{
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

void CtpnPostProcessor::ClipBoxes(const ImageInfo &imgInfo, float &x0, float &y0, float &x1, float &y1)
{
    x0 = std::max(std::min(x0, static_cast<float>(imgInfo.modelWidth) - 1), 0.0f);
    y0 = std::max(std::min(y0, static_cast<float>(imgInfo.modelHeight) - 1), 0.0f);
    x1 = std::max(std::min(x1, static_cast<float>(imgInfo.modelWidth) - 1), 0.0f);
    y1 = std::max(std::min(y1, static_cast<float>(imgInfo.modelHeight) - 1), 0.0f);
}

bool CtpnPostProcessor::FilterBoxes(float &x0, float &y0, float &x1, float &y1)
{
    return !((x1 - x0 + 1 >= static_cast<float>(minSize_)) && (y1 - y0 + 1 >= static_cast<float>(minSize_)));
}

void SelectTopKBoxesByScore(unsigned int num, std::vector<MxBase::DetectBox> &detBoxes)
{
    num = (num < detBoxes.size()) ? num : detBoxes.size();
    std::vector<DetectBox> sortBoxes;
    for (size_t i = 0; i != num; i++) {
        sortBoxes.push_back(detBoxes[i]);
    }
    detBoxes = std::move(sortBoxes);
}

bool CtpnPostProcessor::CheckFeatLayerData(int i, int j, int k, int boxAnchorSize, int confAnchorSize, int featWidth)
{
    auto outputTensorShapesSize = outputTensorShapes_.size();
    if (CLS_PRED >= outputTensorShapesSize || outputTensorShapes_.empty()) {
        LogError << "Check outputTensorShapes size failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    int64_t bboxTensorSize = 1;
    for (size_t t = 0; t < outputTensorShapes_[BBOX_PRED].size(); t++) {
        bboxTensorSize *= outputTensorShapes_[BBOX_PRED][t];
    }
    int64_t confidenceTensorSize = 1;
    for (size_t t = 0; t < outputTensorShapes_[CLS_PRED].size(); t++) {
        confidenceTensorSize *= outputTensorShapes_[CLS_PRED][t];
    }
    if (bboxTensorSize <= boxAnchorSize * j + k * featBoxLayer_ + RIGHTBOTY + boxAnchorSize * featWidth * i) {
        LogError << "Check bbox tensor size failed, bboxTensorSize is " << bboxTensorSize
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (confidenceTensorSize <= confAnchorSize * j + k * featConfLayer_ + CONF_OBJECT +
                                confAnchorSize * featWidth * i) {
        LogError << "Check confidence tensor size failed, confidenceTensorSize is " << confidenceTensorSize
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

APP_ERROR CtpnPostProcessor::FilterBoxesForReadSrcData(const std::vector<std::shared_ptr<void>> &featLayerData,
    const ImageInfo &imgInfo, int i, int j, int k, const std::vector<std::vector<int>> &wholeImgAnchors,
    std::vector<MxBase::DetectBox> &detBoxes)
{
    if (IsDenominatorZero(featStride_)) {
        LogError << "The value of featStride_ of must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (featLayerData.size() <= CLS_PRED) {
        LogError << "The size of featLayerData is not enough." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    auto *bboxPtr = static_cast<float *>(featLayerData[BBOX_PRED].get());
    auto *confidencePtr = static_cast<float *>(featLayerData[CLS_PRED].get());
    if (bboxPtr == nullptr || confidencePtr == nullptr) {
        LogError << "The bboxPtr is nullptr or confidencePtr is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    int featWidth = imgInfo.modelWidth / featStride_;   // 67
    int boxAnchorSize = featBoxLayer_ * anchorNum_;     // 40
    int confAnchorSize = featConfLayer_ * anchorNum_;   // 20
    int count = featWidth * anchorNum_ * i + anchorNum_ * j + k;
    if (!CheckFeatLayerData(i, j, k, boxAnchorSize, confAnchorSize, featWidth)) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxBase::DetectBox det;
    float x1 = bboxPtr[boxAnchorSize * j + k * featBoxLayer_ + LEFTTOPX + boxAnchorSize * featWidth * i];
    float y1 = bboxPtr[boxAnchorSize * j + k * featBoxLayer_ + LEFTTOPY + boxAnchorSize * featWidth * i];
    float w = bboxPtr[boxAnchorSize * j + k * featBoxLayer_ + RIGHTBOTX + boxAnchorSize * featWidth * i];
    float h = bboxPtr[boxAnchorSize * j + k * featBoxLayer_ + RIGHTBOTY + boxAnchorSize * featWidth * i];
    det.prob = confidencePtr[confAnchorSize * j + k * featConfLayer_ + CONF_OBJECT + confAnchorSize * featWidth * i];
    // Get real box location
    float x2 = w;
    float y2 = h;
    if (wholeImgAnchors.size() < static_cast<uint32_t >(count)) {
        LogError << "Size of wholeImgAnchors check failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    GetRealBoxesOfImg(wholeImgAnchors[count], x1, y1, x2, y2);
    // Fix proposal, cut the part that more than image's size.
    ClipBoxes(imgInfo, x1, y1, x2, y2);
    // Filter proposal of that size < minSize
    if (FilterBoxes(x1, y1, x2, y2) || det.prob <= textProposalsMinScore_) {
        return APP_ERR_OK;
    }
    LogDebug << "get value for model:(x1: " << x1 << ", y1: " << y1 << ", x2: " << x2 << ", y2: " << y2 <<
             ", score: " << det.prob << ")";
    det.x = (x1 + x2) / COORDINATE_PARAM;
    det.y = (y1 + y2) / COORDINATE_PARAM;
    det.width = (x2 - x1 > 0) ? (x2 - x1) : (x1 - x2);
    det.height = (y2 - y1 > 0) ? (y2 - y1) : (y1 - y2);
    det.classID = 0;
    detBoxes.emplace_back(det);
    return APP_ERR_OK;
}

APP_ERROR CtpnPostProcessor::TraverseAnchor(const std::vector<std::shared_ptr<void>> &featLayerData,
    const ImageInfo &imgInfo, CoorDim &coorDim, const std::vector<std::vector<int>> &wholeImgAnchors,
    std::vector<MxBase::DetectBox> &detBoxes)
{
    for (int k = 0; k < anchorNum_; k++) {
        APP_ERROR ret = FilterBoxesForReadSrcData(featLayerData, imgInfo, coorDim.i, coorDim.j, k, wholeImgAnchors,
            detBoxes);
        if (ret != APP_ERR_OK) {
            LogError << "FilterBoxesForReadSrcData failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR CtpnPostProcessor::ReadSrcData(const std::vector<std::shared_ptr<void>> &featLayerData,
    const ImageInfo &imgInfo, const std::vector<std::vector<int>> &wholeImgAnchors,
    std::vector<MxBase::DetectBox> &detBoxes)
{
    if (IsDenominatorZero(featStride_)) {
        LogError << "The value of featStride_ of must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    int featWidth = imgInfo.modelWidth / featStride_;   // 67
    int featHeight = imgInfo.modelHeight / featStride_; // 38
    for (int i = 0; i < featHeight; i++) {
        for (int j = 0; j < featWidth; j++) {
            CoorDim coorDim(i, j);
            APP_ERROR ret = TraverseAnchor(featLayerData, imgInfo, coorDim, wholeImgAnchors, detBoxes);
            if (ret != APP_ERR_OK) {
                LogError << "FilterBoxesForReadSrcData failed." << GetErrorInfo(ret);
                return ret;
            }
        }
    }
    return APP_ERR_OK;
}

APP_ERROR CtpnPostProcessor::NonMaxSuppression(const std::vector<std::shared_ptr<void>> &featLayerData,
    const ImageInfo &imgInfo, const std::vector<std::vector<int>> &wholeImgAnchors,
    std::vector<std::vector<float>> &textProposals, std::vector<float> &scores)
{
    std::vector<MxBase::DetectBox> detBoxes;
    APP_ERROR ret = ReadSrcData(featLayerData, imgInfo, wholeImgAnchors, detBoxes);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to executed ReadSrcData." << GetErrorInfo(ret);
        return ret;
    }
    // Order
    std::sort(detBoxes.begin(), detBoxes.end(),
        [=](const DetectBox &a, const DetectBox &b) { return a.prob > b.prob; });
    // Keep 12000 boxes
    SelectTopKBoxesByScore(rpnPreNmsTopN_, detBoxes);
    std::vector<DetectBox> sortBoxes;
    FilterByIou(detBoxes, sortBoxes, boxIouThresh_);
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
        textProposals.push_back({ x1, y1, x2, y2 });
        scores.push_back(score);
    }
    return APP_ERR_OK;
}

bool CtpnPostProcessor::CalcOverLaps(const int &index1, const int &index2,
    const std::vector<std::vector<float>> &textProposals)
{
    // cal over laps
    float h1 = textProposals[index1][RIGHTBOTY] - textProposals[index1][LEFTTOPY] + 1;
    float h2 = textProposals[index2][RIGHTBOTY] - textProposals[index2][LEFTTOPY] + 1;
    float y0 = std::max(textProposals[index2][LEFTTOPY], textProposals[index1][LEFTTOPY]);
    float y1 = std::min(textProposals[index2][RIGHTBOTY], textProposals[index1][RIGHTBOTY]);
    if (IsDenominatorZero(h1) || IsDenominatorZero(h2)) {
        LogError << "The value of h1 or h2 must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    float overLaps = std::max(0.0f, y1 - y0 + 1) / std::min(h1, h2);
    // cal similarity
    float similarity = std::min(h1, h2) / std::max(h1, h2);
    return (overLaps >= minOverLaps_ && similarity >= minSizeSim_);
}

void CtpnPostProcessor::GetSuccessions(const std::vector<std::vector<float>> &textProposals,
    const std::vector<std::vector<int>> &boxesTable, const ImageInfo &imgInfo, const int &index,
    std::vector<int> &successions)
{
    // Get positive connective anchors
    std::vector<float> box = textProposals[index];
    int leftMaxIdx = std::min((int)box[LEFTTOPX] + maxHorizontalGap_ + 1, imgInfo.modelWidth);
    for (auto left = (int)box[LEFTTOPX] + 1; left < leftMaxIdx; left++) {
        if (static_cast<uint32_t>(left) >= boxesTable.size()) {
            LogError << "Size of boxesTable check failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
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

void CtpnPostProcessor::GetPrecursors(const std::vector<std::vector<float>> &textProposals,
    const std::vector<std::vector<int>> &boxesTable, const int &successionIdx, std::vector<int> &precursors)
{
    // Get negative connective anchors
    std::vector<float> box = textProposals[successionIdx];
    int leftMaxNegIdx = std::max((int)box[LEFTTOPX] - maxHorizontalGap_, 0) - 1;
    for (auto left = (int)box[LEFTTOPX] - 1; left > leftMaxNegIdx; left--) {
        if (static_cast<uint32_t>(left) >= boxesTable.size()) {
            LogError << "Size of boxesTable check failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        std::vector<int> adjBoxIndices = boxesTable[left];
        for (auto adjBoxIndex : adjBoxIndices) {
            if (CalcOverLaps(adjBoxIndex, successionIdx, textProposals)) {
                precursors.push_back(adjBoxIndex);
            }
        }
        if (!precursors.empty()) {
            return;
        }
    }
}

void GetComponents(int proposalSize, std::vector<std::unordered_set<int>> graph,
    std::vector<std::vector<int>> &subGraphs)
{
    std::vector<bool> visited(proposalSize, false);
    for (auto i = 0; i < proposalSize; i++) {
        std::vector<int> subGraph;
        std::queue<int> q { { i } };
        if (visited[i]) {
            continue;
        }
        while (!q.empty()) {
            auto cur = q.front();
            q.pop();
            if (visited[cur]) {
                continue;
            }
            subGraph.push_back(cur);
            visited[cur] = true;
            for (const auto &neighbor : graph[cur]) {
                q.push(neighbor);
            }
        }
        subGraphs.push_back(subGraph);
    }
}

void CtpnPostProcessor::BuildGraph(const std::vector<std::vector<float>> &textProposals,
    const std::vector<float> &scores, const ImageInfo &imgInfo, std::vector<std::unordered_set<int>> &graph)
{
    // Put index into boxes table of x0 value of proposal, it's like histogram.
    std::vector<std::vector<int>> boxesTable(imgInfo.modelWidth, std::vector<int>());
    int boxesTableIdx = 0;
    for (const auto &textProposal : textProposals) {
        if (textProposal.empty()) {
            LogError << "TextProposal is empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        if (static_cast<uint32_t>(textProposal[LEFTTOPX]) >= boxesTable.size()) {
            LogError << "Check size of boxesTable failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return;
        }
        boxesTable[static_cast<int>(textProposal[LEFTTOPX])].push_back(boxesTableIdx);
        boxesTableIdx++;
    }
    int boxesSize = (int)textProposals.size();
    for (auto index = 0; index < boxesSize; index++) {
        std::vector<int> successions;
        // Get succession connective anchors
        GetSuccessions(textProposals, boxesTable, imgInfo, index, successions);
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
        GetPrecursors(textProposals, boxesTable, successionIdx, precursors);
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
            graph[index].insert(successionIdx);
            graph[successionIdx].insert(index);
        }
    }
}

void SetTextLineBoxesValue(const std::vector<std::vector<float>> &textLineBoxes,
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
    const float &index2, const float &score, TextObjDetectInfo &textRec)
{
    std::vector<float> textLineBoxesMidX;
    std::vector<float> textLineBoxesMidY;
    std::vector<float> textLineBoxesHeight;
    SetTextLineBoxesValue(textLineBoxes, textLineBoxesMidX, textLineBoxesMidY, textLineBoxesHeight);
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
    double disX = index2 - index0; // right up - left up
    double disY = y2 - y1;
    double width = std::sqrt(disX * disX + disY * disY); // text line width
    if (IsDenominatorZero(width)) {
        LogError << "The width: " << width << "must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
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

bool CtpnPostProcessor::FilterBoxes(TextObjDetectInfo textObjDetectInfo)
{
    double heights = (std::fabs(textObjDetectInfo.y2 - textObjDetectInfo.y0) +
        std::fabs(textObjDetectInfo.y3 - textObjDetectInfo.y1)) *
        HALF_SCALE + 1;
    double widths = (std::fabs(textObjDetectInfo.x1 - textObjDetectInfo.x0) +
        std::fabs(textObjDetectInfo.x3 - textObjDetectInfo.x2)) *
        HALF_SCALE + 1;
    if (IsDenominatorZero(heights)) {
        LogError << "The value of heights must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return (widths / heights > minRatio_) && (textObjDetectInfo.confidence > lineMinScore_) &&
        (widths > textProposalsWidth_ * minNumProposals_);
}

void CtpnPostProcessor::GetTextLines(const std::vector<std::vector<float>> &textProposals,
    const std::vector<float> &scores, const std::vector<std::vector<int>> &subGraphs, const ImageInfo &imgInfo,
    std::vector<TextObjDetectInfo> &textRecs)
{
    for (auto subGraph : subGraphs) {
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
        for (auto textLineBox : textLineBoxes) {
            textLineBoxesX0.push_back(textLineBox[LEFTTOPX]);
            textLineBoxesY0.push_back(textLineBox[LEFTTOPY]);
            textLineBoxesX1.push_back(textLineBox[RIGHTBOTX]);
            textLineBoxesY1.push_back(textLineBox[RIGHTBOTY]);
        }
        InfoTextLineValue info;
        if (textBoxesScores.empty() || textLineBoxesX0.empty() || textLineBoxesX1.empty()) {
            LogError << "The vector of textBoxesScores or textLineBoxesX0 or textLineBoxesX1 must not be empty!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            continue;
        }
        info.x0 = *std::min_element(textLineBoxesX0.begin(), textLineBoxesX0.end());
        info.x1 = *std::max_element(textLineBoxesX1.begin(), textLineBoxesX1.end());
        info.offset = (textLineBoxes[LEFTTOPX][RIGHTBOTX] - textLineBoxes[LEFTTOPX][LEFTTOPX]) * HALF_SCALE;
        LineRegressionFit lrFitTop;
        lrFitTop.SetAlphaAndBeta(textLineBoxesX0, textLineBoxesY0);
        LineRegressionFit lrFitBottom;
        lrFitBottom.SetAlphaAndBeta(textLineBoxesX0, textLineBoxesY1);
        info.y0 = std::min(lrFitTop.LRFunction(info.x0 + info.offset), lrFitTop.LRFunction(info.x1 - info.offset));
        info.y1 = std::max(lrFitBottom.LRFunction(info.x0 + info.offset),
            lrFitBottom.LRFunction(info.x1 - info.offset));
        info.score = std::accumulate(textBoxesScores.begin(), textBoxesScores.end(), 0.0) / textBoxesScores.size();
        // Direct is left down/right down/right up/left up
        TextObjDetectInfo textObjDetectInfo {};
        if (isOriented_) {
            GetTextLinesOriented(textLineBoxes, info.x0, info.x1, info.score, textObjDetectInfo);
        } else {
            ClipBoxes(imgInfo, info.x0, info.y0, info.x1, info.y1);
            textObjDetectInfo = {
                info.x0, info.y0, info.x1, info.y0, info.x1, info.y1, info.x0, info.y1, info.score
            };
        }
        if (!FilterBoxes(textObjDetectInfo)) {
            continue;
        }
        textRecs.push_back(textObjDetectInfo);
    }
}

APP_ERROR CtpnPostProcessor::ObjectDetectionOutput(std::vector<std::shared_ptr<void>> &featLayerData,
    std::vector<TextObjDetectInfo> &textObjInfos, ImageInfo &imgInfo)
{
    LogDebug << "CtpnPostProcessor start to write results.";
    std::vector<std::vector<int>> anchors;
    std::vector<std::vector<int>> wholeImgAnchors;
    // 1. Creat anchors (x0, y0, x1, y1)
    CreateAnchors(anchors);
    // 2. Get the real anchors of the whole img
    APP_ERROR ret = GetWholeImgAnchors(anchors, imgInfo, wholeImgAnchors);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to executed GetWholeImgAnchors" << GetErrorInfo(ret);
        return ret;
    }
    // 3. Do nms and keep 100 proposals
    std::vector<std::vector<float>> textProposals;
    std::vector<float> scores;
    ret = NonMaxSuppression(featLayerData, imgInfo, wholeImgAnchors, textProposals, scores);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to executed NonMaxSuppression" << GetErrorInfo(ret);
        return ret;
    }
    // 4. Create graph, the value is bool. The value true show the two index box connect each other.
    int proposalSize = (int)textProposals.size();
    std::vector<std::unordered_set<int>> graph(proposalSize);
    BuildGraph(textProposals, scores, imgInfo, graph);
    // 5. Get connective proposals from graph
    std::vector<std::vector<int>> subGraphs;
    GetComponents(proposalSize, graph, subGraphs);
    // 6. Fit box's y and get final box location
    GetTextLines(textProposals, scores, subGraphs, imgInfo, textObjInfos);
    LogDebug << "Number of objects found : " << textObjInfos.size();
    if (IsDenominatorZero(imgInfo.modelWidth) || IsDenominatorZero(imgInfo.modelHeight)) {
        LogError << "The value of imgInfo.modelWidth or imgInfo.modelHeight must not equal to zero!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (auto &textRec : textObjInfos) {
        textRec.x0 = (textRec.x0 / imgInfo.modelWidth) * imgInfo.imgWidth;
        textRec.x1 = (textRec.x1 / imgInfo.modelWidth) * imgInfo.imgWidth;
        textRec.x2 = (textRec.x2 / imgInfo.modelWidth) * imgInfo.imgWidth;
        textRec.x3 = (textRec.x3 / imgInfo.modelWidth) * imgInfo.imgWidth;
        textRec.y0 = (textRec.y0 / imgInfo.modelHeight) * imgInfo.imgHeight;
        textRec.y1 = (textRec.y1 / imgInfo.modelHeight) * imgInfo.imgHeight;
        textRec.y2 = (textRec.y2 / imgInfo.modelHeight) * imgInfo.imgHeight;
        textRec.y3 = (textRec.y3 / imgInfo.modelHeight) * imgInfo.imgHeight;
        LogDebug << "Find object: "
                 << "confidence(" << textRec.confidence << "), Coordinates(" << textRec.x0 << ", " << textRec.y0 <<
            "; " << textRec.x1 << ", " << textRec.y1 << "; " << textRec.x2 << ", " << textRec.y2 << "; " <<
            textRec.x3 << ", " << textRec.y3 << ").";
    }
    LogDebug << "CtpnPostProcessor write results successed.";
    return APP_ERR_OK;
}
}
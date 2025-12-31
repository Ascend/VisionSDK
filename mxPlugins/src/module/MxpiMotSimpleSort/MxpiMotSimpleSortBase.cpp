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
 * Description: Implements multi-objective path logging and adjusts the input port of the plug-in.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"
#include "MxPlugins/MxpiMotSimpleSort/MxpiMotSimpleSortBase.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const float EPSILON = 1e-6;
const float NORM_EPS = 1e-10;
const int FLOAT_TO_INT = 1000;
const float WIDTH_RATE_THRESH = 1.f;
const float HEIGHT_RATE_THRESH = 1.f;
const float X_DIST_RATE_THRESH = 1.3f;
const float Y_DIST_RATE_THRESH = 1.f;
const double SIMILARITY_THRESHOLD = 0.66;
const int MULTIPLE_IOU = 6;
const int HITS_THRESHOLD = 2;
}

namespace MxPlugins {
APP_ERROR MxpiMotSimpleSortBase::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiMotSimpleSort(" << elementName_ << ").";
    std::vector<std::string> parameterNamesPtr = {"dataSourceDetection", "dataSourceFeature",
                                                  "trackThreshold", "lostThreshold"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    // Set previous object detection plugin name
    dataSourceDetection_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceDetection"]);
    LogInfo << "The detectionDataSource is " << dataSourceDetection_ << "(" << elementName_ << ").";
    // Set previous embedding plugin name
    dataSourceFeature_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceFeature"]);
    LogInfo << "The featureDataSource is " << dataSourceFeature_ << "(" << elementName_ << ").";

    if (!dataSourceFeature_.empty()) {
        withFeature_ = true;
    }
    // Set threshold of tracking
    trackThreshold_ = *std::static_pointer_cast<float>(configParamMap["trackThreshold"]);
    // Set threshold of losing object
    lostThreshold_ = *std::static_pointer_cast<uint>(configParamMap["lostThreshold"]);

    LogInfo << "End to initialize MxpiMotSimpleSort(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiMotSimpleSortBase::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiMotSimpleSort(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiMotSimpleSort(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiMotSimpleSortBase::ErrorProcessing(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, APP_ERROR ret)
{
    LogError << errorInfo_.str();
    SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
    return ret;
}

APP_ERROR MxpiMotSimpleSortBase::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiMotSimpleSort(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // Get mxpiObjectList from mxpiBuffer
    MxpiBuffer* buffer = mxpiBuffer[0];
    MxpiMetadataManager mxpiMetadataManager(*buffer);
    errorInfo_.str("");

    // check data source
    ret = CheckDataSource(mxpiMetadataManager);
    if (ret != APP_ERR_OK) {
        SendData(0, *buffer);
        return ret;
    }
    std::vector<DetectObject> detectObjectList;
    ret = GetModelInferResult(*buffer, detectObjectList);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    std::vector<DetectObject> unmatchedObjectQueue; // Lost tracking object collection
    std::vector<cv::Point> matchedTrackedDetected; // Matched tracking object collection
    if (!detectObjectList.empty()) {
        ret = MatchProcess(detectObjectList, matchedTrackedDetected, unmatchedObjectQueue);
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
            return ret;
        }
    } else {
        for (auto &trackLet : trackLetList_) {
            trackLet.trackInfo.trackFlag = LOST_OBJECT;
        }
    }
    // Update all the trackLet in the trackLet list per frame
    UpdateTrackLet(matchedTrackedDetected, detectObjectList, unmatchedObjectQueue);
    // Add metadata of trackLetList to the buffer
    auto mxpiTrackLetList = MemoryHelper::MakeShared<MxpiTrackLetList>();
    if (mxpiTrackLetList == nullptr) {
        errorInfo_ << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return ErrorProcessing(mxpiBuffer, APP_ERR_COMM_ALLOC_MEM);
    }
    ret = GetTrackingResult(mxpiTrackLetList);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to get tracking result." << GetErrorInfo(ret);
        return ErrorProcessing(mxpiBuffer, ret);
    }
    ret = mxpiMetadataManager.AddProtoMetadata(elementName_, std::static_pointer_cast<void>(mxpiTrackLetList));
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to add metadata." << GetErrorInfo(ret);
        return ErrorProcessing(mxpiBuffer, ret);
    }
    SendData(0, *buffer); // Send the data to downstream plugin
    LogDebug << "End to process MxpiMotSimpleSort(" << elementName_ << ").";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiMotSimpleSortBase::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto dataSourceDetection = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceDetection", "dataSourceDetection", "the name of object detection data source", "", "", ""
    });
    auto dataSourceFeature = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceFeature", "dataSourceFeature", "the name of feature data source", "", "", ""
    });
    auto trackThreshold = std::make_shared<ElementProperty<float>>(ElementProperty<float> {
        FLOAT, "trackThreshold", "trackThreshold", "Threshold of tracking", 0.5, 0.f, 1.f
    });
    auto lostThreshold = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
        UINT, "lostThreshold", "lostThreshold", "Threshold of losing object", 5, 0, 10
    });

    properties.push_back(dataSourceDetection);
    properties.push_back(dataSourceFeature);
    properties.push_back(trackThreshold);
    properties.push_back(lostThreshold);
    return properties;
}

APP_ERROR MxpiMotSimpleSortBase::CheckDataSource(MxTools::MxpiMetadataManager& mxpiMetadataManager)
{
    if (mxpiMetadataManager.GetMetadata(dataSourceDetection_) == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << "metadata is null. please check"
                 << " your property dataSourceDetection(" << dataSourceDetection_ << ").";
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
    }
    if (withFeature_ && mxpiMetadataManager.GetMetadata(dataSourceFeature_) == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << "metadata is null. please check"
                 << " your property dataSourceFeature(" << dataSourceFeature_ << ").";
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiMotSimpleSortBase::CheckDataStructure(MxTools::MxpiMetadataManager &mxpiMetadataManager)
{
    auto objMetadata = mxpiMetadataManager.GetMetadata(dataSourceDetection_);
    auto objDesc = ((google::protobuf::Message*)objMetadata.get())->GetDescriptor();
    if (!objDesc) {
        errorInfo_ << "Get Metadata descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (objDesc->name() != "MxpiObjectList") {
        errorInfo_ << "Not a MxpiObjectList." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    if (!withFeature_) {
        return APP_ERR_OK;
    }
    auto feaMetadata = mxpiMetadataManager.GetMetadata(dataSourceFeature_);
    auto feaDesc = ((google::protobuf::Message*)feaMetadata.get())->GetDescriptor();
    if (!feaDesc) {
        errorInfo_ << "Get Metadata descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (feaDesc->name() != "MxpiFeatureVectorList") {
        errorInfo_ << "Not a MxpiFeatureVectorList object." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiMotSimpleSortBase::GetModelInferResult(MxpiBuffer &buffer, std::vector<DetectObject> &detectObjectList)
{
    MxpiMetadataManager mxpiMetadataManager(buffer);
    APP_ERROR ret = CheckDataStructure(mxpiMetadataManager);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    std::shared_ptr<MxpiObjectList> objectList = std::static_pointer_cast<MxpiObjectList>(
        mxpiMetadataManager.GetMetadata(dataSourceDetection_));
    if (objectList == nullptr) {
        LogError << "Fail to get meta data." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    std::shared_ptr<MxpiFeatureVectorList> featureList;
    if (objectList->objectvec_size() == 0) {
        LogDebug << "Object detection result of model infer is null.";
        return APP_ERR_OK;
    }
    if (withFeature_) {
        featureList = std::static_pointer_cast<MxpiFeatureVectorList>(
            mxpiMetadataManager.GetMetadata(dataSourceFeature_));
        if (featureList == nullptr) {
            LogError << "Fail to get meta data." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        if (featureList->featurevec_size() == 0) {
            ret = APP_ERR_COMM_FAILURE;
            errorInfo_ << "Face short feature result of model infer is null." << GetErrorInfo(ret);
            return ret;
        }
    }
    for (int i = 0; i < objectList->objectvec_size(); ++i) {
        DetectObject detectObject {};
        detectObject.detectInfo = objectList->objectvec(i);
        detectObject.memberId = static_cast<uint32_t>(i);
        if (withFeature_) {
            GetFeatureVector(featureList, i, detectObject);
        }
        detectObjectList.push_back(detectObject);
    }
    return APP_ERR_OK;
}

void MxpiMotSimpleSortBase::GetFeatureVector(const std::shared_ptr<MxTools::MxpiFeatureVectorList> &featureList,
    const int32_t &memberId, DetectObject &detectObject)
{
    if (featureList == nullptr) {
        LogError << "FeatureList is null when getting feature vector." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return;
    }
    for (int i = 0; i < featureList->featurevec_size(); ++i) {
        if (featureList->featurevec(i).headervec_size() == 0) {
            LogError << "Protobuf message vector is invalid." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
            return;
        }
        if (featureList->featurevec(i).headervec(0).memberid() == memberId) {
            detectObject.featureVector = featureList->featurevec(i);
        }
    }
}

float MxpiMotSimpleSortBase::CalcIOU(const MxpiObject &preDetectionBox, const MxpiObject &curDetectionBox)
{
    // Calculate two detection box IOU
    float preWidth = preDetectionBox.x1() - preDetectionBox.x0();
    float preHeight = preDetectionBox.y1() - preDetectionBox.y0();
    float curWidth = curDetectionBox.x1() - curDetectionBox.x0();
    float curHeight = curDetectionBox.y1() - curDetectionBox.y0();
    cv::Rect_<float> preBbox(preDetectionBox.x0(), preDetectionBox.y0(), preWidth, preHeight);
    cv::Rect_<float> curBbox(curDetectionBox.x0(), curDetectionBox.y0(), curWidth, curHeight);
    float intersectionArea = (preBbox & curBbox).area();
    float unionArea = preBbox.area() + curBbox.area() - intersectionArea;
    if (std::fabs(unionArea) < EPSILON) {
        LogDebug << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM, elementName_)
                 << "the value of unionArea must not equal to zero!";
        return 0.f;
    }
    return intersectionArea / unionArea;
}

float MxpiMotSimpleSortBase::CalcFeatureNorm(const MxTools::MxpiFeatureVector &feature)
{
    // calculate norm
    float norm = 0.f;
    for (int i = 0; i < feature.featurevalues_size(); i++) {
        norm += feature.featurevalues(i) * feature.featurevalues(i);
    }
    norm = std::sqrt(norm);
    return norm;
}

float MxpiMotSimpleSortBase::CalcDot(const MxpiFeatureVector &preFeature, const MxpiFeatureVector &curFeature)
{
    if (preFeature.featurevalues_size() != curFeature.featurevalues_size()) {
        LogWarn << "[" + elementName_ + "] Feature length is not equal between previous and current feature.";
        return 0.f;
    }
    if (preFeature.featurevalues_size() == 0) {
        LogError << "[" + elementName_ + "] Data of previous or current feature is null."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return 0.f;
    }
    float dotSum = 0.f;
    for (int i = 0; i < preFeature.featurevalues_size(); i++) {
        dotSum += preFeature.featurevalues(i) * curFeature.featurevalues(i);
    }
    return dotSum;
}

float MxpiMotSimpleSortBase::CalcShortFeatureCosine(const MxpiFeatureVector &preFeature,
    const MxpiFeatureVector &curFeature)
{
    float shortFeatureCosine = CalcDot(preFeature, curFeature);
    float preFeatureNorm = CalcFeatureNorm(preFeature);
    float curFeatureNorm = CalcFeatureNorm(curFeature);
    if (IsDenominatorZero(preFeatureNorm) || IsDenominatorZero(curFeatureNorm)) {
        LogError << "The value of preFeatureNorm or curFeatureNorm must not equal to zero!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return 0.f;
    }
    if (preFeatureNorm > NORM_EPS) {
        shortFeatureCosine /= preFeatureNorm;
    } else {
        shortFeatureCosine = 0.f;
    }
    if (curFeatureNorm > NORM_EPS) {
        shortFeatureCosine /= curFeatureNorm;
    } else {
        shortFeatureCosine = 0.f;
    }
    return shortFeatureCosine;
}

float MxpiMotSimpleSortBase::CalcDistSimilarity(const MxpiObject &preDetectionBox, const MxpiObject &curDetectionBox)
{
    float preWidth = preDetectionBox.x1() - preDetectionBox.x0();
    float preHeight = preDetectionBox.y1() - preDetectionBox.y0();
    float curWidth = curDetectionBox.x1() - curDetectionBox.x0();
    float curHeight = curDetectionBox.y1() - curDetectionBox.y0();
    float preCenterX = preDetectionBox.x0() + preWidth / 2.f;
    float preCenterY = preDetectionBox.y0() + preHeight / 2.f;
    float curCenterX = curDetectionBox.x0() + curWidth / 2.f;
    float curCenterY = curDetectionBox.y0() + curHeight / 2.f;
    float xDistance = preCenterX - curCenterX;
    float yDistance = preCenterY - curCenterY;
    float minWidth = std::min(preWidth, curWidth);
    float minHeight = std::min(preHeight, curHeight);
    if (IsDenominatorZero(minWidth) || IsDenominatorZero(minHeight)) {
        LogError << "The value of minWidth or minHeight must not equal to zero!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return 0.f;
    }
    return (1.f - xDistance / minWidth) * (1.f - yDistance / minHeight);
}

float MxpiMotSimpleSortBase::CalcMixed(const TrackLet &trackLet, const DetectObject &detectObject)
{
    // Mixed method is weighted sum of distance similarity and feature cosine
    float detWidth = detectObject.detectInfo.x1() - detectObject.detectInfo.x0();
    float detHeight = detectObject.detectInfo.y1() - detectObject.detectInfo.y0();
    float trackWidth = trackLet.detectInfo.x1() - trackLet.detectInfo.x0();
    float trackHeight = trackLet.detectInfo.y1() - trackLet.detectInfo.y0();
    if (IsDenominatorZero(trackWidth) || IsDenominatorZero(trackHeight)) {
        LogError << "The value of trackWidth or trackHeight must not equal to zero!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return 0.f;
    }
    float widthRate = std::abs(detWidth - trackWidth) / trackWidth;
    float heightRate = std::abs(detHeight - trackHeight) / trackHeight;
    float xDistanceRate = std::abs(detectObject.detectInfo.x0() - trackLet.detectInfo.x0() +
                                   detWidth / MULTIPLE - trackWidth / MULTIPLE) / trackWidth;
    float yDistanceRate = std::abs(detectObject.detectInfo.y0() - trackLet.detectInfo.y0() +
                                   detHeight / MULTIPLE - trackHeight / MULTIPLE) / trackHeight;
    if (widthRate > WIDTH_RATE_THRESH || heightRate > HEIGHT_RATE_THRESH) {
        return 0.f;
    }
    if (xDistanceRate > X_DIST_RATE_THRESH || yDistanceRate > Y_DIST_RATE_THRESH) {
        return 0.f;
    }
    float distSim = CalcDistSimilarity(trackLet.detectInfo, detectObject.detectInfo);
    float cosine = CalcShortFeatureCosine(trackLet.featureVector, detectObject.featureVector);
    return (cosine > SIMILARITY_THRESHOLD) ? std::max(distSim * cosine * MULTIPLE_IOU, 1.f) : distSim;
}

float MxpiMotSimpleSortBase::CalcSimilarity(const TrackLet &trackLet, const DetectObject &detectObject,
    uint32_t &method)
{
    switch (method) {
        case IOU:
            return CalcIOU(trackLet.detectInfo, detectObject.detectInfo);
        case COSINE:
            return CalcShortFeatureCosine(trackLet.featureVector, detectObject.featureVector);
        case MIXED:
            return CalcMixed(trackLet, detectObject);
        default:
            LogError << "Failed to calculate similarity matrix for the method: " << method << " is invalid."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return 0.f;
    }
}

void MxpiMotSimpleSortBase::TrackObjectPredict()
{
    // Every traceLet should do kalman predict
    for (auto &traceLet : trackLetList_) {
        DetectBox detectBox = traceLet.kalman.Predict();
        traceLet.detectInfo.set_x0(detectBox.x);
        traceLet.detectInfo.set_x1(detectBox.x + detectBox.width);
        traceLet.detectInfo.set_y0(detectBox.y);
        traceLet.detectInfo.set_y1(detectBox.y + detectBox.height);
    }
}

APP_ERROR MxpiMotSimpleSortBase::TrackObjectUpdate(std::vector<DetectObject> &detectObjectList,
    std::vector<cv::Point> &matchedTrackedDetected, std::vector<DetectObject> &unmatchedObjectQueue)
{
    // Init detected object tracking flag
    std::vector<bool> detectObjectFlagVec;
    for (size_t i = 0; i < detectObjectList.size(); ++i) {
        detectObjectFlagVec.push_back(false);
    }
    // Calculate the similarity matrix
    std::vector<std::vector<int>> simMatrix;
    simMatrix.clear();
    simMatrix.resize(trackLetList_.size(), std::vector<int>(detectObjectList.size(), 0));
    if (withFeature_) {
        method_ = MIXED;
    } else {
        method_ = IOU;
    }
    for (size_t j = 0; j < detectObjectList.size(); ++j) {
        for (size_t i = 0; i < trackLetList_.size(); ++i) {
            float similarity = CalcSimilarity(trackLetList_[i], detectObjectList[j], method_);
            simMatrix[i][j] = (int) (similarity * FLOAT_TO_INT);
        }
    }
    // Solve the assignment problem using hungarian
    HungarianHandle hungarianHandleObj;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, trackLetList_.size(), detectObjectList.size());
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Init HungarianHandle failed." << GetErrorInfo(ret);
        return ret;
    }
    HungarianSolve(hungarianHandleObj, simMatrix, trackLetList_.size(), detectObjectList.size());
    // Filter out targets below the threshold
    FilterLowThreshold(hungarianHandleObj, simMatrix, matchedTrackedDetected, detectObjectFlagVec);
    // Fill unmatched object queue
    for (size_t i = 0; i < detectObjectFlagVec.size(); ++i) {
        if (!detectObjectFlagVec[i]) {
            unmatchedObjectQueue.push_back(detectObjectList[i]);
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiMotSimpleSortBase::MatchProcess(std::vector<DetectObject> &detectObjectList,
    std::vector<cv::Point> &matchedTrackedDetected, std::vector<DetectObject> &unmatchedObjectQueue)
{
    APP_ERROR ret = APP_ERR_OK;
    if (!trackLetList_.empty()) {
        // Every trackLet do kalman predict
        TrackObjectPredict();
        // Update tracked object
        ret = TrackObjectUpdate(detectObjectList, matchedTrackedDetected, unmatchedObjectQueue);
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str();
            return ret;
        }
    } else {
        if (!detectObjectList.empty()) {
            for (size_t i = 0; i < detectObjectList.size(); ++i) {
                unmatchedObjectQueue.push_back(detectObjectList[i]);
            }
        }
    }
    return APP_ERR_OK;
}

void MxpiMotSimpleSortBase::FilterLowThreshold(const HungarianHandle &hungarianHandleObj,
    const std::vector<std::vector<int>> &disMatrix, std::vector<cv::Point> &matchedTracedDetected,
    std::vector<bool> &detectObjectFlagVec)
{
    for (size_t i = 0; i < trackLetList_.size(); ++i) {
        // Matched if above the threshold
        if (hungarianHandleObj.resX[i] != -1 &&
            disMatrix[i][hungarianHandleObj.resX[i]] >= (trackThreshold_ * FLOAT_TO_INT)) {
            matchedTracedDetected.push_back(cv::Point(i, hungarianHandleObj.resX[i]));
            detectObjectFlagVec[hungarianHandleObj.resX[i]] = true;
        } else {
            trackLetList_[i].trackInfo.trackFlag = LOST_OBJECT;
        }
    }
}

void MxpiMotSimpleSortBase::UpdateTrackLet(const std::vector<cv::Point> &matchedTrackedDetected,
                                           std::vector<DetectObject> &detectObjectList,
                                           std::vector<DetectObject> &unmatchedObjectQueue)
{
    UpdateMatchedTrackLet(matchedTrackedDetected, detectObjectList);
    AddNewDetectedObject(unmatchedObjectQueue);
    UpdateUnmatchedTrackLet();
}

void MxpiMotSimpleSortBase::UpdateMatchedTrackLet(const std::vector<cv::Point> &matchedTrackedDetected,
                                                  std::vector<DetectObject> &detectObjectList)
{
    for (size_t i = 0; i < matchedTrackedDetected.size(); ++i) {
        int traceIndex = matchedTrackedDetected[i].x;
        int detectIndex = matchedTrackedDetected[i].y;
        if (static_cast<size_t>(traceIndex) >= trackLetList_.size() ||
            static_cast<size_t>(detectIndex) >= detectObjectList.size()) {
            continue;
        }
        // Update matched object in trackLet list
        trackLetList_[traceIndex].trackInfo.age++;
        trackLetList_[traceIndex].trackInfo.hits++;
        if (trackLetList_[traceIndex].trackInfo.hits >= HITS_THRESHOLD) {
            trackLetList_[traceIndex].trackInfo.trackFlag = TRACKED_OBJECT;
        }
        trackLetList_[traceIndex].lostAge = 0;
        trackLetList_[traceIndex].detectInfo = detectObjectList[detectIndex].detectInfo;
        DetectBox detectBox = ConvertToDetectBox(detectObjectList[detectIndex].detectInfo);
        trackLetList_[traceIndex].kalman.Update(detectBox);
        if (withFeature_) {
            trackLetList_[traceIndex].featureVector = detectObjectList[detectIndex].featureVector;
        }
        trackLetList_[traceIndex].parentName = dataSourceDetection_;
        trackLetList_[traceIndex].memberId = detectObjectList[detectIndex].memberId;
    }
}

void MxpiMotSimpleSortBase::AddNewDetectedObject(std::vector<DetectObject> &unmatchedObjectQueue)
{
    for (auto &detectObject : unmatchedObjectQueue) {
        // Add new detected info into trackLet list
        TrackLet trackLet {};
        generatedId_++;
        trackLet.trackInfo.trackId = generatedId_;
        trackLet.trackInfo.age = 1;
        trackLet.trackInfo.hits = 1;
        trackLet.lostAge = 0;
        trackLet.trackInfo.trackFlag = NEW_OBJECT;
        trackLet.detectInfo = detectObject.detectInfo;
        DetectBox detectBox = ConvertToDetectBox(detectObject.detectInfo);
        trackLet.kalman.CvKalmanInit(detectBox);
        if (withFeature_) {
            trackLet.featureVector = detectObject.featureVector;
        }
        trackLet.parentName = dataSourceDetection_;
        trackLet.memberId = detectObject.memberId;
        trackLetList_.push_back(trackLet);
    }
}

void MxpiMotSimpleSortBase::UpdateUnmatchedTrackLet()
{
    // Update unmatched object in trackLet list
    for (auto &trackLet : trackLetList_) {
        if (trackLet.trackInfo.trackFlag == LOST_OBJECT) {
            trackLet.lostAge++;
            trackLet.trackInfo.age++;
        }
    }
}

APP_ERROR MxpiMotSimpleSortBase::GetTrackingResult(std::shared_ptr<MxpiTrackLetList> &mxpiTrackLetList)
{
    APP_ERROR ret = APP_ERR_OK;
    for (auto itr = trackLetList_.begin(); itr != trackLetList_.end();) {
        if (itr->trackInfo.trackFlag != LOST_OBJECT) {
            ret = AddTrackLet(mxpiTrackLetList, itr);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to add tracklet." << GetErrorInfo(ret);
                return ret;
            }
            ++itr;
        } else if (itr->lostAge > lostThreshold_) {
            ret = AddTrackLet(mxpiTrackLetList, itr);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to add tracklet." << GetErrorInfo(ret);
                return ret;
            }
            itr = trackLetList_.erase(itr);
        } else {
            ++itr;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiMotSimpleSortBase::AddTrackLet(std::shared_ptr<MxTools::MxpiTrackLetList> &mxpiTrackLetList,
                                             std::vector<TrackLet>::iterator &iter)
{
    MxpiTrackLet *mxpiTrackLet = mxpiTrackLetList->add_trackletvec();
    if (CheckPtrIsNullptr(mxpiTrackLet, "mxpiTrackLet"))  return APP_ERR_COMM_ALLOC_MEM;
    mxpiTrackLet->set_trackid(iter->trackInfo.trackId);
    mxpiTrackLet->set_hits(iter->trackInfo.hits);
    mxpiTrackLet->set_age(iter->trackInfo.age);
    mxpiTrackLet->set_trackflag(iter->trackInfo.trackFlag);
    if (iter->trackInfo.trackFlag != LOST_OBJECT) {
        MxpiMetaHeader* header = mxpiTrackLet->add_headervec();
        if (CheckPtrIsNullptr(header, "header"))  return APP_ERR_COMM_ALLOC_MEM;
        header->set_datasource(iter->parentName);
        header->set_memberid(iter->memberId);
    }
    return APP_ERR_OK;
}

DetectBox MxpiMotSimpleSortBase::ConvertToDetectBox(const MxpiObject& mxpiObject)
{
    DetectBox detectBox {};
    float height = mxpiObject.y1() - mxpiObject.y0();
    float width = mxpiObject.x1() - mxpiObject.x0();
    detectBox.x = mxpiObject.x0();
    detectBox.y = mxpiObject.y0();
    detectBox.height = height;
    detectBox.width = width;
    detectBox.classID = mxpiObject.classvec(0).classid();
    detectBox.prob = mxpiObject.classvec(0).confidence();
    return detectBox;
}
}
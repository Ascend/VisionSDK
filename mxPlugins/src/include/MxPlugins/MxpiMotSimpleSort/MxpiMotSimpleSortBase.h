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

#ifndef MXPLUGINS__MOTSIMPLESORTBASE_H
#define MXPLUGINS__MOTSIMPLESORTBASE_H

#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Log/Log.h"
#include "MxBase/CV/Core/DataType.h"
#include "MxBase/CV/MultipleObjectTracking/Huangarian.h"
#include "MxBase/CV/MultipleObjectTracking/KalmanTracker.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include <cmath>

namespace MxPlugins {
enum MethodFlag {
    IOU = 0,
    COSINE,
    MIXED
};

struct TrackInfo {
    uint32_t trackId;
    uint32_t age;
    uint32_t hits;
    MxBase::TrackFlag trackFlag;
};

struct TrackLet {
    std::string parentName;
    uint32_t memberId;
    TrackInfo trackInfo;
    MxBase::KalmanTracker kalman;
    MxTools::MxpiObject detectInfo;
    uint32_t lostAge;
    MxTools::MxpiFeatureVector featureVector;
};

struct DetectObject {
    uint32_t memberId;
    MxTools::MxpiObject detectInfo;
    MxTools::MxpiFeatureVector featureVector;
};

class MxpiMotSimpleSortBase : public MxTools::MxPluginBase {
public:
    /**
     * @api
     * @param configParamMap
     * @return
     */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap) override;

    /**
     * @api
     * @return
     */
    APP_ERROR DeInit() override;

    /**
     * @api
     * @param mxpiBuffer
     * @return
     */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

    /**
     * @api
     * @brief Definition the parameter of configure properties.
     * @return std::vector<std::shared_ptr<void>>
     */
    static std::vector<std::shared_ptr<void>> DefineProperties();

protected:
    APP_ERROR ErrorProcessing(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, APP_ERROR ret);

    APP_ERROR CheckDataSource(MxTools::MxpiMetadataManager &mxpiMetadataManager);

    APP_ERROR GetModelInferResult(MxTools::MxpiBuffer &buffer, std::vector<DetectObject> &detectObjectList);

    void GetFeatureVector(const std::shared_ptr<MxTools::MxpiFeatureVectorList> &featureList,
                          const int32_t &memberId, DetectObject &detectObject);

    float CalcIOU(const MxTools::MxpiObject &preDetectionBox, const MxTools::MxpiObject &curDetectionBox);

    float CalcFeatureNorm(const MxTools::MxpiFeatureVector &feature);

    float CalcDot(const MxTools::MxpiFeatureVector &preFeature, const MxTools::MxpiFeatureVector &curFeature);

    float CalcShortFeatureCosine(const MxTools::MxpiFeatureVector &preFeature,
                                 const MxTools::MxpiFeatureVector &curFeature);

    float CalcDistSimilarity(const MxTools::MxpiObject &preDetectionBox, const MxTools::MxpiObject &curDetectionBox);

    float CalcSimilarity(const TrackLet &trackLet, const DetectObject &detectObject, uint32_t &method);

    float CalcMixed(const TrackLet &trackLet, const DetectObject &detectObject);

    void TrackObjectPredict();

    APP_ERROR TrackObjectUpdate(std::vector<DetectObject> &detectObjectList,
                                std::vector<cv::Point> &matchedTrackedDetected,
                                std::vector<DetectObject> &unmatchedObjectQueue);

    APP_ERROR MatchProcess(std::vector<DetectObject> &detectObjectList,
                           std::vector<cv::Point> &matchedTrackedDetected,
                           std::vector<DetectObject> &unmatchedObjectQueue);

    void FilterLowThreshold(const MxBase::HungarianHandle &hungarianHandleObj,
                            const std::vector<std::vector<int>> &disMatrix,
                            std::vector<cv::Point> &matchedTracedDetected, std::vector<bool> &detectObjectFlagVec);

    void UpdateTrackLet(const std::vector<cv::Point> &matchedTrackedDetected,
                        std::vector<DetectObject> &detectObjectList,
                        std::vector<DetectObject> &unmatchedObjectQueue);

    void UpdateMatchedTrackLet(const std::vector<cv::Point> &matchedTrackedDetected,
                               std::vector<DetectObject> &detectObjectList);

    void AddNewDetectedObject(std::vector<DetectObject> &unmatchedObjectQueue);

    void UpdateUnmatchedTrackLet();

    APP_ERROR GetTrackingResult(std::shared_ptr<MxTools::MxpiTrackLetList> &mxpiTrackLetList);

    APP_ERROR AddTrackLet(std::shared_ptr<MxTools::MxpiTrackLetList> &mxpiTrackLetList,
                     std::vector<TrackLet>::iterator &iter);

    MxBase::DetectBox ConvertToDetectBox(const MxTools::MxpiObject &mxpiObject);

    APP_ERROR CheckDataStructure(MxTools::MxpiMetadataManager &mxpiMetadataManager);

protected:

    std::string dataSourceDetection_ = "";  // previous object detection plugin name
    std::string dataSourceFeature_ = "";  // previous embedding plugin name
    bool withFeature_ = false; // whether to use object embedding for multiple object tracking
    uint32_t generatedId_ = 0;
    uint32_t lostThreshold_ = 0;
    uint32_t method_ = 0;
    float trackThreshold_ = 0.f;
    std::ostringstream errorInfo_;
    std::vector<TrackLet> trackLetList_ = {};
};
}

#endif

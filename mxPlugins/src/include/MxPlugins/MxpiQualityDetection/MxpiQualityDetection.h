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
 * Description: Analyze and detect the quality of the decoded image.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXVISION_QUALITYDETECTION_H
#define MXVISION_QUALITYDETECTION_H

#include <map>
#include <deque>

#include "opencv4/opencv2/highgui.hpp"
#include "opencv4/opencv2/imgcodecs.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "opencv4/opencv2/core/types_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"

#include "acl/acl.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/base/MxPluginBase.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/ConfigUtil/ConfigUtil.h"

struct FrameInfo {
    uint32_t frameIdPlugin;
    uint32_t frameIdVdec;
    uint32_t channelIdVdec;
    cv::Mat rgbData;
};

namespace MxPlugins {
class MxpiQualityDetection : public MxTools::MxPluginBase {
public:
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap) override;
    APP_ERROR DeInit() override;
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer) override;
    static std::vector<std::shared_ptr<void>> DefineProperties();
    static MxTools::MxpiPortInfo DefineInputPorts();
    static MxTools::MxpiPortInfo DefineOutputPorts();

private:
    APP_ERROR InitParams(void);
    bool IsValidInterval(uint32_t frameInterval);
    APP_ERROR CheckImageDectParams(void);
    APP_ERROR CheckVideoDectParams(void);
    APP_ERROR LoadConfig(std::string configPath);
    bool FrameSkip(bool switchDetection, uint32_t frameInterval);
    APP_ERROR ImageFormatConversion(MxTools::MxpiVision visionItem);
    APP_ERROR ImageProcessAndDetection(MxTools::MxpiBuffer &buffer);
    APP_ERROR DetectionProcess(void);

    APP_ERROR ImageBrightnessDetection(void);
    APP_ERROR ImageOcclusionDetection(void);
    APP_ERROR ImageBlurDetection(void);
    APP_ERROR ImageNoiseDetection(void);
    APP_ERROR CheckValuePixelSum(int pixelSum, int noiseNum);
    APP_ERROR ImageColorCastDetection(void);
    double StripeRateCalculation(const cv::Mat &imgGray);
    APP_ERROR ImageStripeDetection(void);
    APP_ERROR BlackScreenDetection(void);
    APP_ERROR VideoFreezeDetection(void);
    APP_ERROR ViewShakeOffsetCal(const cv::Mat &imageGray, std::vector<int> &sumRowVec, std::vector<int> &sumColVec);
    int ViewShakeCalMin(std::vector<int> &sumVecImage1, std::vector<int> &sumVecImage2, const int searchLen);
    APP_ERROR ViewShakeDetection(void);
    APP_ERROR SceneMutationDetection(void);
    float PTZMovementHistSim(const cv::Mat &imageGrayAvg1, const cv::Mat &imageGrayAvg2);
    APP_ERROR PTZMovementDetection(void);

    typedef APP_ERROR (MxpiQualityDetection::*DoDetection)(void);
    std::map<int, DoDetection> keyToHandle;
    APP_ERROR DetectionWithKey(int detectionType);
    void MapKeyToHandle(void);

private:
    std::ostringstream errorInfo_;
    std::uint32_t frameIdCur_ = 0;
    std::uint32_t frameIdVdec_;
    std::uint32_t channelIdVdec_;
    std::deque<FrameInfo> frameList_ = {};
    MxBase::ConfigData configData_;

    uint32_t frameListMaxLen_ = 20;
    bool switchBrightnessDetection_ = false;
    uint32_t frameIntervalBrightnessDetection_ = 10;
    float thresholdBrightnessFactor_ = 1;
    bool switchOcclusionDetection_ = false;
    uint32_t frameIntervalOcclusionDetection_ = 10;
    float thresholdOcclusion_ = 0.32;
    bool switchBlurDetection_ = false;
    uint32_t frameIntervalBlurDetection_ = 10;
    float thresholdBlur_ = 2000;
    bool switchNoiseDetection_ = false;
    uint32_t frameIntervalNoiseDetection_ = 10;
    float thresholdNoiseRate_ = 0.005;
    bool switchColorCastDetection_ = false;
    uint32_t frameIntervalColorCastDetection_ = 10;
    float thresholdcolorCastFactor_ = 1.5;
    bool switchStripeDetection_ = false;
    uint32_t frameIntervalStripeDetection_ = 10;
    float thresholdStripe_ = 0.0015;
    bool switchScreenDetection_ = false;
    uint32_t frameIntervalScreenDetection_ = 10;
    float thresholdDarkProp_ = 0.72;
    bool switchVideoFreezeDetection_ = false;
    uint32_t frameIntervalVideoFreezeDetection_ = 10;
    float thresholdVideoFreezeDetection_ = 0.1;
    bool switchViewShakeDetection_ = false;
    uint32_t frameIntervalViewShakeDetection_ = 10;
    float thresholdViewShakeDetection_ = 20;
    bool switchSceneMutationDetection_ = false;
    uint32_t frameIntervalSceneMutationDetection_ = 10;
    float thresholdSceneMutationDetection_ = 0.5;
    bool switchPTZMovementDetection_ = false;
    uint32_t frameIntervalPTZMovementDetection_ = 10;
    float thresholdPTZMovementDetection_ = 0.95;
};
}

#endif // MXVISION_QUALITYDETECTION_H
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

#include "MxPlugins/MxpiQualityDetection/MxpiQualityDetection.h"

#include <cmath>
#include <limits>
#include <fstream>
#include "nlohmann/json.hpp"
#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxPlugins;
using namespace MxTools;
using namespace MxBase;
using namespace std;
using namespace cv;

namespace {
const string QUALITY_DETECTION_KEY = "MxpiVisionList";
const int PIXEL_VALUE_NUM = 256;
const float NV12_COEF = 1.5;
const int OCCLUSION_GRAY_THRES = 25;
const int OCCLUSION_LAP_THRES = 10;
const float INT_TO_FLOAT = 1.0;
const int POW_AREA = 2;
const int NUM_2 = 2;

enum class DetectionType {
    IMAGE_BRIGHTNESS_DETECTION = 0,
    IMAGE_OCCLUSION_DETECTION,
    IMAGE_BLUR_DETECTION,
    IMAGE_NOISE_DETECTION,
    IMAGE_COLOR_CAST_DETECTION,
    IMAGE_STRIPE_DETECTION,
    BLACK_SCREEN_DETECTION,
    SCENE_MUTATION_DETECTION,
    PTZ_MOVEMENT_DETECTION,
    VIEW_SHAKE_DETECTION,
    VIDEO_FREEZE_DETECTION,
};

int AlignToEven(int num)
{
    if (num % NUM_2 == 0) {
        return num;
    }
    return num - 1;
}
} // namespace

APP_ERROR MxpiQualityDetection::InitParams(void)
{
    configData_.GetFileValueWarn<uint32_t>("FRAME_LIST_LEN", frameListMaxLen_);

    configData_.GetFileValueWarn<bool>("BRIGHTNESS_SWITCH", switchBrightnessDetection_);
    configData_.GetFileValueWarn<uint32_t>("BRIGHTNESS_FRAME_INTERVAL", frameIntervalBrightnessDetection_);
    configData_.GetFileValueWarn<float>("BRIGHTNESS_THRESHOLD", thresholdBrightnessFactor_);

    configData_.GetFileValueWarn<bool>("OCCLUSION_SWITCH", switchOcclusionDetection_);
    configData_.GetFileValueWarn<uint32_t>("OCCLUSION_FRAME_INTERVAL", frameIntervalOcclusionDetection_);
    configData_.GetFileValueWarn<float>("OCCLUSION_THRESHOLD", thresholdOcclusion_);

    configData_.GetFileValueWarn<bool>("BLUR_SWITCH", switchBlurDetection_);
    configData_.GetFileValueWarn<uint32_t>("BLUR_FRAME_INTERVAL", frameIntervalBlurDetection_);
    configData_.GetFileValueWarn<float>("BLUR_THRESHOLD", thresholdBlur_);

    configData_.GetFileValueWarn<bool>("NOISE_SWITCH", switchNoiseDetection_);
    configData_.GetFileValueWarn<uint32_t>("NOISE_FRAME_INTERVAL", frameIntervalNoiseDetection_);
    configData_.GetFileValueWarn<float>("NOISE_THRESHOLD", thresholdNoiseRate_);

    configData_.GetFileValueWarn<bool>("COLOR_CAST_SWITCH", switchColorCastDetection_);
    configData_.GetFileValueWarn<uint32_t>("COLOR_CAST_FRAME_INTERVAL", frameIntervalColorCastDetection_);
    configData_.GetFileValueWarn<float>("COLOR_CAST_THRESHOLD", thresholdcolorCastFactor_);

    configData_.GetFileValueWarn<bool>("STRIPE_SWITCH", switchStripeDetection_);
    configData_.GetFileValueWarn<uint32_t>("STRIPE_FRAME_INTERVAL", frameIntervalStripeDetection_);
    configData_.GetFileValueWarn<float>("STRIPE_THRESHOLD", thresholdStripe_);

    configData_.GetFileValueWarn<bool>("DARK_SWITCH", switchScreenDetection_);
    configData_.GetFileValueWarn<uint32_t>("DARK_FRAME_INTERVAL", frameIntervalScreenDetection_);
    configData_.GetFileValueWarn<float>("DARK_THRESHOLD", thresholdDarkProp_);

    configData_.GetFileValueWarn<bool>("VIDEO_FREEZE_SWITCH", switchVideoFreezeDetection_);
    configData_.GetFileValueWarn<uint32_t>("VIDEO_FREEZE_FRAME_INTERVAL", frameIntervalVideoFreezeDetection_);
    configData_.GetFileValueWarn<float>("VIDEO_FREEZE_THRESHOLD", thresholdVideoFreezeDetection_);

    configData_.GetFileValueWarn<bool>("VIEW_SHAKE_SWITCH", switchViewShakeDetection_);
    configData_.GetFileValueWarn<uint32_t>("VIEW_SHAKE_FRAME_INTERVAL", frameIntervalViewShakeDetection_);
    configData_.GetFileValueWarn<float>("VIEW_SHAKE_THRESHOLD", thresholdViewShakeDetection_);

    configData_.GetFileValueWarn<bool>("SCENE_MUTATION_SWITCH", switchSceneMutationDetection_);
    configData_.GetFileValueWarn<uint32_t>("SCENE_MUTATION_FRAME_INTERVAL", frameIntervalSceneMutationDetection_);
    configData_.GetFileValueWarn<float>("SCENE_MUTATION_THRESHOLD", thresholdSceneMutationDetection_);

    configData_.GetFileValueWarn<bool>("PTZ_MOVEMENT_SWITCH", switchPTZMovementDetection_);
    configData_.GetFileValueWarn<uint32_t>("PTZ_MOVEMENT_FRAME_INTERVAL", frameIntervalPTZMovementDetection_);
    configData_.GetFileValueWarn<float>("PTZ_MOVEMENT_THRESHOLD", thresholdPTZMovementDetection_);

    return APP_ERR_OK;
}

bool MxpiQualityDetection::IsValidInterval(uint32_t frameInterval)
{
    if (frameInterval == 0 || frameInterval >= frameListMaxLen_) {
        return false;
    }
    return true;
}

APP_ERROR MxpiQualityDetection::CheckImageDectParams(void)
{
    const uint32_t minFrameListLen = 2;
    if (frameListMaxLen_ < minFrameListLen) {
        errorInfo_ << "Invalid frame list length, please check it." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!IsValidInterval(frameIntervalBrightnessDetection_) || !IsValidInterval(frameIntervalOcclusionDetection_) ||
        !IsValidInterval(frameIntervalBlurDetection_) || !IsValidInterval(frameIntervalNoiseDetection_) ||
        !IsValidInterval(frameIntervalColorCastDetection_) || !IsValidInterval(frameIntervalStripeDetection_) ||
        !IsValidInterval(frameIntervalScreenDetection_)) {
        errorInfo_ << "Invalid frame interval, please check it." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::CheckVideoDectParams(void)
{
    const float thresShakeDetectionMin = 10.0;
    const float thresShakeDetectionMax = 100.0;
    if (!IsValidInterval(frameIntervalVideoFreezeDetection_) || !IsValidInterval(frameIntervalViewShakeDetection_) ||
        !IsValidInterval(frameIntervalSceneMutationDetection_) ||
        !IsValidInterval(frameIntervalPTZMovementDetection_)) {
        errorInfo_ << "Invalid frame interval, please check it." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (thresholdViewShakeDetection_ < thresShakeDetectionMin ||
        thresholdViewShakeDetection_ > thresShakeDetectionMax) {
        errorInfo_ << "Invalid threshold of Video Shake Detection, please check it."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::LoadConfig(std::string configPath)
{
    nlohmann::json config;
    MxBase::ConfigUtil util;
    MxBase::ConfigMode configMode;
    try {
        config = nlohmann::json::parse(configPath);
        LogDebug << "Parse config content from content";
        configMode = MxBase::CONFIGCONTENT;
    } catch (const std::exception &ex) {
        LogDebug << "Parse config content from file";
        configMode = MxBase::CONFIGFILE;
    }
    APP_ERROR ret = util.LoadConfiguration(configPath, configData_, configMode);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to load configuration, config path invalidate." << GetErrorInfo(ret);
        return ret;
    }
    ret = InitParams();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to initialize the parameters." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckImageDectParams();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = CheckVideoDectParams();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiQualityDetection(" << elementName_ << ").";
    dataSource_ = MxPluginsAutoDataSource(elementName_, 0, "dataSource", dataSource_, dataSourceKeys_);
    if (dataSource_.empty()) {
        LogError << "Get data source fail." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::vector<std::string> parameterConfigContentPtr = {"qualityDetectionConfigContent"};
    auto ret = CheckConfigParamMapIsValid(parameterConfigContentPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }

    std::string config = *std::static_pointer_cast<std::string>(configParamMap["qualityDetectionConfigContent"]);
    if (config == "") {
        LogDebug << "element(" << elementName_ <<
            ") not set property(qualityDetectionConfigContent), try to read content from cfg file.";
        std::vector<std::string> parameterConfigPathPtr = {"qualityDetectionConfigPath"};
        ret = CheckConfigParamMapIsValid(parameterConfigPathPtr, configParamMap);
        if (ret != APP_ERR_OK) {
            LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
            return ret;
        }
        config = *std::static_pointer_cast<std::string>(configParamMap["qualityDetectionConfigPath"]);
        if (!MxBase::FileUtils::CheckFileExists(config)) {
            LogWarn << GetErrorInfo(APP_ERR_COMM_NO_EXIST, elementName_)
                    << "The Configuration file of qualityDetection does not exist.";
        }
    }
    ret = LoadConfig(config);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to load config file." << GetErrorInfo(ret);
        return ret;
    }
    MapKeyToHandle(); // Initialize detection func map
    LogInfo << "End to initialize MxpiQualityDetection(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiQualityDetection(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiQualityDetection(" << elementName_ << ").";
    return APP_ERR_OK;
}

bool MxpiQualityDetection::FrameSkip(bool switchDetection, uint32_t frameInterval)
{
    bool ifSkip = false;
    if (!switchDetection) {
        LogDebug << "Switch Off, skip frame " << frameIdVdec_ << ", channel " << channelIdVdec_;
        ifSkip = true;
    } else if (frameIdCur_ % frameInterval != 0) {
        LogDebug << "Invalid Frame Id, skip frame " << frameIdVdec_ << ", channel " << channelIdVdec_;
        ifSkip = true;
    }
    return ifSkip;
}

APP_ERROR MxpiQualityDetection::ImageBrightnessDetection(void)
{
    LogDebug << "Begin to process ImageBrightnessDetection.";
    if (FrameSkip(switchBrightnessDetection_, frameIntervalBrightnessDetection_)) {
        return APP_ERR_OK;
    }
    float diffSum = 0;
    float deviation = 0;
    const int brightnessMean = 128;
    int hist[PIXEL_VALUE_NUM] = { 0 };
    Mat imgGray;
    Mat imgRgb = frameList_.back().rgbData;
    cvtColor(imgRgb, imgGray, COLOR_RGB2GRAY);
    for (int i = 0; i < imgGray.rows; i++) {
        for (int j = 0; j < imgGray.cols; j++) {
            diffSum += float(imgGray.at<uchar>(i, j) - brightnessMean);
            hist[imgGray.at<uchar>(i, j)]++;
        }
    }
    if (IsDenominatorZero(float(imgGray.rows * imgGray.cols))) {
        LogError << "The multiplication of rows and cols must not equal to 0!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    float diffAvg = diffSum / float(imgGray.rows * imgGray.cols);
    for (int i = 0; i < PIXEL_VALUE_NUM; i++) {
        deviation += abs(i - brightnessMean - diffAvg) * hist[i];
    }
    deviation /= float((imgGray.rows * imgGray.cols));
    if (IsDenominatorZero(deviation)) {
        LogError << "The value of deviation must not equal to 0!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    float brightnessFactor = abs(diffAvg / deviation);
    if (brightnessFactor > thresholdBrightnessFactor_) {
        if (diffAvg > std::numeric_limits<float>::epsilon()) {
            LogWarn << "Video Lightness Detection: Too bright, Brightness Rate = " << brightnessFactor <<
                ", Frame ID = " << frameIdVdec_ << ", Channel ID = " << channelIdVdec_;
        } else {
            LogWarn << "Video Lightness Detection: Too dark, Brightness Rate = " << brightnessFactor <<
                ", Frame ID = " << frameIdVdec_ << ", Channel ID = " << channelIdVdec_;
        }
    } else {
        LogDebug << "Video Lightness Detection: Normal, Frame ID = " << frameIdVdec_ << ", Channel ID = " <<
            channelIdVdec_;
    }
    LogDebug << "End to process ImageBrightnessDetection.";
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::ImageOcclusionDetection(void)
{
    LogDebug << "Begin to process ImageOcclusionDetection.";
    if (FrameSkip(switchOcclusionDetection_, frameIntervalOcclusionDetection_)) {
        return APP_ERR_OK;
    }
    int coverBlock = 0;
    const int blockNum = 8;
    const int sumBlock = static_cast<int>(pow(blockNum, POW_AREA));
    Mat imgGray;
    Mat imgLap;
    Mat imgRgb = frameList_.back().rgbData;
    cvtColor(imgRgb, imgGray, COLOR_RGB2GRAY);
    Laplacian(imgGray, imgLap, imgGray.depth());
    for (int i = 0; i < imgGray.rows / blockNum * blockNum; i += imgGray.rows / blockNum) {
        for (int j = 0; j < imgGray.cols / blockNum * blockNum; j += imgGray.cols / blockNum) {
            float sigmaGray = 0;
            float sigmaLap = 0;
            cv::Rect rec = cv::Rect(j, i, imgGray.cols / blockNum, imgGray.rows / blockNum);
            Mat subImgGray = imgGray(rec);
            Mat subImgLap = imgLap(rec);
            Mat means, stdDevGray, stdDevLap;
            meanStdDev(subImgGray, means, stdDevGray);
            meanStdDev(subImgLap, means, stdDevLap);
            sigmaGray = stdDevGray.at<double>(0);
            sigmaLap = stdDevLap.at<double>(0);
            if (sigmaGray < OCCLUSION_GRAY_THRES && sigmaLap < OCCLUSION_LAP_THRES)
                coverBlock++;
        }
    }
    double coveredRate = 0.0;
    if (!IsDenominatorZero(sumBlock)) {
        coveredRate = coverBlock * INT_TO_FLOAT / sumBlock;
    }
    if (coveredRate > thresholdOcclusion_) {
        LogWarn << "Image Occlusion Detection: Occlusion, Covered Rate = " << coveredRate << ", Frame ID = " <<
            frameIdVdec_ << ", Channel ID = " << channelIdVdec_;
    } else {
        LogDebug << "Image Occlusion Detection: Normal, Frame ID = " << frameIdVdec_ << ", Channel ID = " <<
            channelIdVdec_;
    }
    LogDebug << "End to process ImageOcclusionDetection.";
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::ImageBlurDetection(void)
{
    LogDebug << "Begin to process ImageBlurDetection.";
    if (FrameSkip(switchBlurDetection_, frameIntervalBlurDetection_)) {
        return APP_ERR_OK;
    }
    Mat imgGray;
    Mat imgRgb = frameList_.back().rgbData;
    cvtColor(imgRgb, imgGray, COLOR_RGB2GRAY);
    Mat meansMat, stdDevGrayMat;
    double stdDevGray = 0;
    int kernelSize = 3;
    Laplacian(imgGray, imgGray, imgGray.depth(), kernelSize);
    convertScaleAbs(imgGray, imgGray);
    meanStdDev(imgGray, meansMat, stdDevGrayMat);
    stdDevGray = stdDevGrayMat.at<double>(0, 0);
    double var = pow(stdDevGray, POW_AREA);
    if (var < thresholdBlur_) {
        LogWarn << "Image Blur Detection: Blur, Blur Rate = " << var << ", Frame ID = " << frameIdVdec_ <<
            ", Channel ID = " << channelIdVdec_;
    } else {
        LogDebug << "Image Blur Detection: Normal, Frame ID = " << frameIdVdec_ << ", Channel ID = " << channelIdVdec_;
    }
    LogDebug << "End to process ImageBlurDetection.";
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::ImageNoiseDetection(void)
{
    LogDebug << "Begin to process ImageNoiseDetection.";
    if (FrameSkip(switchNoiseDetection_, frameIntervalNoiseDetection_)) {
        return APP_ERR_OK;
    }
    Mat imgGray;
    Mat imgRgb = frameList_.back().rgbData;
    cvtColor(imgRgb, imgGray, COLOR_RGB2GRAY);
    const int matRowSize = 3;
    const int matColSize = 3;
    // initialize convolution kernel1 (-1, 0, 1, -2, 0, 2, -1, 0, 1)
    Mat kern1 = (Mat_<char>(matRowSize, matColSize) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
    // initialize convolution kernel2 (-1, -2, -1, 0, 0, 0, 1, 2, 1)
    Mat kern2 = (Mat_<char>(matRowSize, matColSize) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
    // initialize convolution kernel3 (0, 1, 2, -1, 0, 1, -2, -1, 0)
    Mat kern3 = (Mat_<char>(matRowSize, matColSize) << 0, 1, 2, -1, 0, 1, -2, -1, 0);
    // initialize convolution kernel4 (-2, -1, 0, -1, 0, 1, 0, 1, 2)
    Mat kern4 = (Mat_<char>(matRowSize, matColSize) << -2, -1, 0, -1, 0, 1, 0, 1, 2);
    int pixelSum = imgGray.cols * imgGray.rows;
    const float thresSpbe = 0.1;
    const int maxPixelValue = 255;
    const int kernSize = 3;
    Mat imgSpbel1, imgSpbel2, imgSpbel3, imgSpbel4, imgMBlur;
    filter2D(imgGray, imgSpbel1, imgGray.depth(), kern1);
    filter2D(imgGray, imgSpbel2, imgGray.depth(), kern2);
    filter2D(imgGray, imgSpbel3, imgGray.depth(), kern3);
    filter2D(imgGray, imgSpbel4, imgGray.depth(), kern4);
    Mat imgSpbel1Locate = (abs(imgSpbel1) > thresSpbe) / maxPixelValue;
    Mat imgSpbel2Locate = (abs(imgSpbel2) > thresSpbe) / maxPixelValue;
    Mat imgSpbel3Locate = (abs(imgSpbel3) > thresSpbe) / maxPixelValue;
    Mat imgSpbel4Locate = (abs(imgSpbel4) > thresSpbe) / maxPixelValue;
    Mat countLocate = imgSpbel1Locate.mul(imgSpbel2Locate).mul(imgSpbel3Locate).mul(imgSpbel4Locate);
    medianBlur(imgGray, imgMBlur, kernSize);
    Mat reduce = abs(imgMBlur - imgGray).mul(countLocate);
    int noiseNum = 0;
    const int noiseValue = 20;
    for (int i = 0; i < reduce.rows; i++) {
        for (int j = 0; j < reduce.cols; j++) {
            if (reduce.at<uchar>(i, j) > noiseValue) {
                noiseNum++;
            }
        }
    }
    APP_ERROR ret = CheckValuePixelSum(pixelSum, noiseNum);
    if (ret != APP_ERR_OK) {
        LogError << "The value of pixelSum must not equal to 0!" << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "End to process ImageNoiseDetection.";
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::CheckValuePixelSum(int pixelSum, int noiseNum)
{
    if (IsDenominatorZero(pixelSum)) {
        LogError << "The value of pixelSum must not equal to 0!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    float noiseRate = noiseNum * INT_TO_FLOAT / pixelSum;
    if (noiseRate > thresholdNoiseRate_) {
        LogWarn << "Image Noise Detection: Noise, Noise Rate = " << noiseRate << ", Frame ID = " << frameIdVdec_
            <<", Channel ID = " << channelIdVdec_;
    } else {
        LogDebug << "Image Noise Detection: Normal, Frame ID = " << frameIdVdec_ << ", Channel ID = " << channelIdVdec_;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::ImageColorCastDetection(void)
{
    LogDebug << "Begin to process ImageColorCastDetection.";
    if (FrameSkip(switchColorCastDetection_, frameIntervalColorCastDetection_)) {
        return APP_ERR_OK;
    }
    Mat imgRgb = frameList_.back().rgbData;
    Mat_<Vec3b>::iterator begin = imgRgb.begin<Vec3b>();
    Mat_<Vec3b>::iterator end = imgRgb.end<Vec3b>();
    float sumA = 0;
    float sumB = 0;
    const int idxA = 1;
    const int idxB = 2;
    for (; begin != end; begin++) {
        sumA += (*begin)[idxA];
        sumB += (*begin)[idxB];
    }
    int pixelSum = imgRgb.rows * imgRgb.cols;
    const int normValue = 128;
    if (IsDenominatorZero(pixelSum - normValue) || IsDenominatorZero(pixelSum)) {
        LogError << "The subtraction of pixelSum and normValuevalue or the value of pixelSum must not equal to 0!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    double normA = sumA / pixelSum - normValue;
    double normB = sumB / pixelSum - normValue;
    double chrAvg = sqrt(pow(normA, POW_AREA) + pow(normB, POW_AREA));
    begin = imgRgb.begin<Vec3b>();
    double centerDisA = 0;
    double centerDisB = 0;
    for (; begin != end; begin++) {
        centerDisA += abs((*begin)[idxA] - normValue - normA);
        centerDisB += abs((*begin)[idxB] - normValue - normB);
    }
    centerDisA = centerDisA / pixelSum;
    centerDisB = centerDisB / pixelSum;
    double centerDisAvg = sqrt(pow(centerDisA, POW_AREA) + pow(centerDisB, POW_AREA));
    float colorCastFactor = static_cast<float>(chrAvg / centerDisAvg);
    if (colorCastFactor >= thresholdcolorCastFactor_) {
        LogWarn << "Image Color Cast Detection: Color Cast, Cast Rate = " << colorCastFactor << ", Frame ID = " <<
            frameIdVdec_ << ", Channel ID = " << channelIdVdec_;
    } else {
        LogDebug << "Image Color Cast Detection: Normal, Frame ID = " << frameIdVdec_ << ", Channel ID = " <<
            channelIdVdec_;
    }
    LogDebug << "End to process ImageColorCastDetection.";
    return APP_ERR_OK;
}

double MxpiQualityDetection::StripeRateCalculation(const Mat &imgGray)
{
    int dftSizeCol = getOptimalDFTSize(imgGray.cols);
    int dftSizeRow = getOptimalDFTSize(imgGray.rows);
    Mat imgPadded;
    copyMakeBorder(imgGray, imgPadded, 0, dftSizeRow - imgGray.rows, 0, dftSizeCol - imgGray.cols, BORDER_CONSTANT,
        Scalar::all(0));
    Mat plane[] = {Mat_<float>(imgPadded), Mat::zeros(imgPadded.size(), CV_32F)};
    Mat complexImg;
    size_t count = 2;
    merge(plane, count, complexImg);
    dft(complexImg, complexImg);
    split(complexImg, plane);
    magnitude(plane[0], plane[1], plane[0]);
    Mat magnitudeImage = plane[0];
    magnitudeImage += Scalar::all(1);
    log(magnitudeImage, magnitudeImage);
    const int coefMagnitude = 20;
    const int coefMagnitudeStddev = 3;
    const int coefMaxV = 2;
    magnitudeImage = magnitudeImage(cv::Rect(0, 0, AlignToEven(magnitudeImage.cols), AlignToEven(magnitudeImage.rows)));
    Mat magnitudeSpectrum = coefMagnitude * magnitudeImage;
    Mat matMeanMat, matStddevMat;
    meanStdDev(magnitudeSpectrum, matMeanMat, matStddevMat);
    double matMean = matMeanMat.at<double>(0, 0);
    double matStddev = matStddevMat.at<double>(0, 0);
    double minV = 0.0;
    double maxV = 0.0;
    double *minP = &minV;
    double *maxP = &maxV;
    minMaxIdx(magnitudeSpectrum, minP, maxP);
    double threshold = max(matMean + coefMagnitudeStddev * matStddev, maxV / coefMaxV);
    int noiseNum = 0;
    int magSpecPixelSum = magnitudeSpectrum.rows * magnitudeSpectrum.cols;
    for (int i = 0; i < magnitudeSpectrum.rows; i++) {
        for (int j = 0; j < magnitudeSpectrum.cols; j++) {
            if (magnitudeSpectrum.at<uchar>(i, j) > threshold) {
                noiseNum++;
            }
        }
    }
    if (IsDenominatorZero(magSpecPixelSum)) {
        LogError << "The value of magSpecPixelSum must not equal to 0!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return 0.f;
    }
    double stripeRate = noiseNum * INT_TO_FLOAT / magSpecPixelSum;
    return stripeRate;
}

APP_ERROR MxpiQualityDetection::ImageStripeDetection(void)
{
    LogDebug << "Begin to process ImageStripeDetection.";
    if (FrameSkip(switchStripeDetection_, frameIntervalStripeDetection_)) {
        return APP_ERR_OK;
    }
    Mat imgGray;
    Mat imgRgb = frameList_.back().rgbData;
    cvtColor(imgRgb, imgGray, COLOR_RGB2GRAY);

    double stripeRate = StripeRateCalculation(imgGray);

    Mat meansMat, stdDevGrayMat;
    double stdDevGray = 0;
    const int kernelSize = 3;
    const double varThreshould = 3000;
    Laplacian(imgGray, imgGray, imgGray.depth(), kernelSize);
    convertScaleAbs(imgGray, imgGray);
    meanStdDev(imgGray, meansMat, stdDevGrayMat);
    stdDevGray = stdDevGrayMat.at<double>(0, 0);
    double var = pow(stdDevGray, POW_AREA);
    if (stripeRate > thresholdStripe_ && var > varThreshould) {
        LogWarn << "Image Stripe Detection：Stripe Noise, Stripe Rate = " << stripeRate << ", Frame ID = " <<
            frameIdVdec_ << ", Channel ID = " << channelIdVdec_;
    } else {
        LogDebug << "Image Stripe Detection：Normal, Frame ID = " << frameIdVdec_ << ", Channel ID = " <<
            channelIdVdec_;
    }
    LogDebug << "End to process ImageStripeDetection.";
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::BlackScreenDetection(void)
{
    LogDebug << "Begin to process BlackScreenDetection.";
    if (FrameSkip(switchScreenDetection_, frameIntervalScreenDetection_)) {
        return APP_ERR_OK;
    }
    Mat imgGray;
    Mat imgRgb = frameList_.back().rgbData;
    cvtColor(imgRgb, imgGray, COLOR_RGB2GRAY);

    int pixelSum = imgGray.rows * imgGray.cols;
    int darkSum = 0;
    const int darknessValue = 20;
    for (int i = 0; i < imgGray.rows; i++) {
        for (int j = 0; j < imgGray.cols; j++) {
            if (imgGray.at<uchar>(i, j) < darknessValue) {
                darkSum++;
            }
        }
    }
    if (IsDenominatorZero(pixelSum)) {
        LogError << "The value of pixelSum must not equal to 0!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    double darkProp = darkSum * INT_TO_FLOAT / pixelSum;
    if (darkProp >= thresholdDarkProp_) {
        LogWarn << "Black Screen Detection: Black screen, Dark Rate = " << darkProp << ", Frame ID = " <<
            frameIdVdec_ << ", Channel ID = " << channelIdVdec_;
    } else {
        LogDebug << "Black Screen Detection: Normal, Frame ID = " << frameIdVdec_ << ", Channel ID = " <<
            channelIdVdec_;
    }
    LogDebug << "End to process BlackScreenDetection.";
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::VideoFreezeDetection(void)
{
    LogDebug << "Begin to process VideoFreezeDetection.";
    if (FrameSkip(switchVideoFreezeDetection_, frameIntervalVideoFreezeDetection_)) {
        return APP_ERR_OK;
    }
    FrameInfo image1 = frameList_[frameListMaxLen_ - 1 - frameIntervalVideoFreezeDetection_];
    FrameInfo image2 = frameList_[frameListMaxLen_ - 1];
    cv::Mat image1Gray, image2GRAY;
    cvtColor(image1.rgbData, image1Gray, cv::COLOR_RGB2GRAY);
    cvtColor(image2.rgbData, image2GRAY, cv::COLOR_RGB2GRAY);
    int diffPixelNum = 0;
    const int pixelDiffValue = 10;
    for (int i = 0; i < image1Gray.rows; i++) {
        for (int j = 0; j < image1Gray.cols; j++) {
            if (abs(image1Gray.at<uchar>(i, j) - image2GRAY.at<uchar>(i, j)) > pixelDiffValue)
                diffPixelNum++;
        }
    }
    auto imageRowsCols = image1Gray.rows * image1Gray.cols;
    if (IsDenominatorZero(imageRowsCols)) {
        LogError << "The value of imageRowsCols must not equal to 0!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    float freezeFactor = diffPixelNum * INT_TO_FLOAT / (image1Gray.rows * image1Gray.cols);
    if (freezeFactor > thresholdVideoFreezeDetection_) {
        LogDebug << "Video Freeze Detection: Normal, Frame ID = " << frameIdVdec_ << ", Channel ID = " <<
            channelIdVdec_;
    } else {
        LogWarn << "Video Freeze Detection: Freeze, Freeze Rate = " << freezeFactor << ", Frame ID = " <<
            frameIdVdec_ << ", Channel ID = " << channelIdVdec_;
    }
    LogDebug << "End to process VideoFreezeDetection.";
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::ViewShakeOffsetCal(const Mat &imageGray, vector<int> &sumRowVec, vector<int> &sumColVec)
{
    int sumRow, sumCol;
    float meanRow, meanCol;
    float sumRC = 0;
    for (int i = 0; i < imageGray.rows; i++) {
        sumRow = 0;
        for (int j = 0; j < imageGray.cols; j++) {
            sumRow += imageGray.at<uchar>(i, j);
        }
        sumRC += sumRow;
        sumRowVec.push_back(sumRow);
    }
    for (int i = 0; i < imageGray.cols; i++) {
        sumCol = 0;
        for (int j = 0; j < imageGray.rows; j++) {
            sumCol += imageGray.at<uchar>(j, i);
        }
        sumColVec.push_back(sumCol);
    }
    if (IsDenominatorZero(imageGray.rows) || IsDenominatorZero(imageGray.cols)) {
        LogError << "ImageGray.rows: " << imageGray.rows << ", imageGray.cols: " << imageGray.cols
                 <<  "must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    meanRow = sumRC / imageGray.rows;
    meanCol = sumRC / imageGray.cols;
    for (size_t i = 0; i < sumRowVec.size(); i++) {
        sumRowVec[i] = static_cast<int>(sumRowVec[i] - meanRow);
    }
    for (size_t i = 0; i < sumColVec.size(); i++) {
        sumColVec[i] = static_cast<int>(sumColVec[i] - meanCol);
    }
    return APP_ERR_OK;
}

int MxpiQualityDetection::ViewShakeCalMin(std::vector<int> &sumVecImage1, std::vector<int> &sumVecImage2,
    const int searchLen)
{
    int minCor;
    int idx = 0;
    const int offsetProduct = 2;
    int doubleLen = offsetProduct * searchLen + 1;
    float sumDiff;
    float minDiff = numeric_limits<float>::max();
    if (static_cast<int>(sumVecImage1.size()) < doubleLen) {
        doubleLen = static_cast<int>(sumVecImage1.size());
    }
    for (int i = 1; i <= doubleLen; i++) {
        sumDiff = 0;
        for (size_t j = 0; j < sumVecImage1.size() - offsetProduct * searchLen; j++) {
            sumDiff += pow((sumVecImage1[i + j - 1] - sumVecImage2[j + searchLen]), POW_AREA);
        }
        if (sumDiff < minDiff) {
            minDiff = sumDiff;
            idx = i;
        }
    }
    minCor = searchLen + 1 - idx;
    return minCor;
}

APP_ERROR MxpiQualityDetection::ViewShakeDetection(void)
{
    LogDebug << "Begin to process ViewShakeDetection.";
    if (FrameSkip(switchViewShakeDetection_, frameIntervalViewShakeDetection_)) {
        return APP_ERR_OK;
    }
    FrameInfo image1 = frameList_[frameListMaxLen_ - 1 - frameIntervalViewShakeDetection_];
    FrameInfo image2 = frameList_[frameListMaxLen_ - 1];
    cv::Mat image1Gray, image2Gray;
    cvtColor(image1.rgbData, image1Gray, cv::COLOR_RGB2GRAY);
    cvtColor(image2.rgbData, image2Gray, cv::COLOR_RGB2GRAY);
    vector<int> sumRowVecImage1, sumColVecImage1, sumRowVecImage2, sumColVecImage2;
    APP_ERROR ret = ViewShakeOffsetCal(image1Gray, sumRowVecImage1, sumColVecImage1);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to executed first function of ViewShakeOffsetCal." << GetErrorInfo(ret);
        return ret;
    }
    ret = ViewShakeOffsetCal(image2Gray, sumRowVecImage2, sumColVecImage2);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to executed first function of ViewShakeOffsetCal." << GetErrorInfo(ret);
        return ret;
    }
    int searchLen = static_cast<int>(thresholdViewShakeDetection_);
    int offsetPixelRow = ViewShakeCalMin(sumRowVecImage1, sumRowVecImage2, searchLen);
    int offsetPixelCol = ViewShakeCalMin(sumColVecImage1, sumColVecImage2, searchLen);
    if (offsetPixelRow < 0 || offsetPixelRow > thresholdViewShakeDetection_ || offsetPixelCol < 0 ||
        offsetPixelCol > thresholdViewShakeDetection_) {
        LogWarn << "View Shake Detection: Shake, Row Shake Rate = " << offsetPixelRow << ", Col Shake Rate = " <<
            offsetPixelCol << ", Frame ID = " << frameIdVdec_ << ", Channel ID = " << channelIdVdec_;
    } else {
        LogDebug << "View Shake Detection: Normal, Frame ID = " << frameIdVdec_ << ", Channel ID = " << channelIdVdec_;
    }
    LogDebug << "End to process ViewShakeDetection.";
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::SceneMutationDetection(void)
{
    LogDebug << "Begin to process SceneMutationDetection.";
    if (FrameSkip(switchSceneMutationDetection_, frameIntervalSceneMutationDetection_)) {
        return APP_ERR_OK;
    }
    FrameInfo image1 = frameList_[frameListMaxLen_ - 1 - frameIntervalSceneMutationDetection_];
    FrameInfo image2 = frameList_[frameListMaxLen_ - 1];
    Mat image1Gray, image2Gray;
    cvtColor(image1.rgbData, image1Gray, cv::COLOR_RGB2GRAY);
    cvtColor(image2.rgbData, image2Gray, cv::COLOR_RGB2GRAY);
    int diffSum = 0;
    const int pixelDiffValue = 50;
    int totalPixelNum = image1Gray.rows * image1Gray.cols;
    for (int i = 0; i < image1Gray.rows; i++) {
        for (int j = 0; j < image1Gray.cols; j++) {
            if (abs(image1Gray.at<uchar>(i, j) - image2Gray.at<uchar>(i, j)) > pixelDiffValue) {
                diffSum++;
            }
        }
    }
    if (IsDenominatorZero(totalPixelNum)) {
        LogError << "The value of totalPixelNum must not equal to 0!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    float diffAvg = diffSum * INT_TO_FLOAT / totalPixelNum;
    if (diffAvg < thresholdSceneMutationDetection_) {
        LogDebug << "Scene Mutation Detection: Normal, Frame ID = " << frameIdVdec_ << ", Channel ID = " <<
            channelIdVdec_;
    } else {
        LogWarn << "Scene Mutation Detection: Mutation, Mutation Rate = " << diffAvg << ", Frame ID = " <<
            frameIdVdec_ << ", Channel ID = " << channelIdVdec_;
    }
    LogDebug << "End to process SceneMutationDetection.";
    return APP_ERR_OK;
}

float MxpiQualityDetection::PTZMovementHistSim(const Mat &imageGrayAvg1, const Mat &imageGrayAvg2)
{
    const int histSize = 256;
    const float range[] = { 0, 256 };
    const float *histRange = { range };
    bool uniform = true;
    bool accumulate = false;
    Mat hist1, hist2;
    calcHist(&imageGrayAvg1, 1, 0, Mat(), hist1, 1, &histSize, &histRange, uniform, accumulate);
    normalize(hist1, hist1, 0, 1, NORM_MINMAX, -1, Mat());
    calcHist(&imageGrayAvg2, 1, 0, Mat(), hist2, 1, &histSize, &histRange, uniform, accumulate);
    normalize(hist2, hist2, 0, 1, NORM_MINMAX, -1, Mat());
    float histSimilarity = compareHist(hist1, hist2, HISTCMP_CORREL);
    return histSimilarity;
}

APP_ERROR MxpiQualityDetection::PTZMovementDetection(void)
{
    LogDebug << "Begin to process PTZMovementDetection.";
    if (FrameSkip(switchPTZMovementDetection_, frameIntervalPTZMovementDetection_)) {
        return APP_ERR_OK;
    }
    FrameInfo image = frameList_[frameListMaxLen_ - 1];
    Mat imageGray;
    cvtColor(image.rgbData, imageGray, cv::COLOR_RGB2GRAY);
    const uint32_t defaultScalarValue = 255;
    Mat imageGraySum1(imageGray.rows, imageGray.cols, CV_32FC1, Scalar(defaultScalarValue));
    Mat imageGraySum2(imageGray.rows, imageGray.cols, CV_32FC1, Scalar(defaultScalarValue));
    const uint32_t medianDivisor = 2;
    uint32_t startFrameIdx = frameListMaxLen_ - frameIntervalPTZMovementDetection_ - 1;
    uint32_t midFrameIdx = startFrameIdx + (frameIntervalPTZMovementDetection_ / medianDivisor);
    for (size_t i = startFrameIdx; i < midFrameIdx; i++) {
        cvtColor(frameList_[i].rgbData, imageGray, cv::COLOR_RGB2GRAY);
        accumulate(imageGray, imageGraySum1);
    }
    for (size_t j = midFrameIdx; j < frameListMaxLen_; j++) {
        cvtColor(frameList_[j].rgbData, imageGray, cv::COLOR_RGB2GRAY);
        accumulate(imageGray, imageGraySum2);
    }
    auto frameIdxDifference = midFrameIdx - startFrameIdx;
    auto listMaxLenIdxDifference = frameListMaxLen_ - midFrameIdx;
    if (IsDenominatorZero(frameIdxDifference) || IsDenominatorZero(listMaxLenIdxDifference)) {
        LogError << "The value of frameIdxDifference or listMaxLenIdxDifference must not equal to 0!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    imageGraySum1 /= frameIdxDifference;
    imageGraySum2 /= listMaxLenIdxDifference;
    Mat imageGrayAvg1, imageGrayAvg2;
    imageGraySum1.convertTo(imageGrayAvg1, CV_8UC1, 1, 0);
    imageGraySum2.convertTo(imageGrayAvg2, CV_8UC1, 1, 0);
    float simHist = PTZMovementHistSim(imageGrayAvg1, imageGrayAvg2);
    if (simHist > thresholdPTZMovementDetection_) {
        LogWarn << "PTZ Movement Detection: Abnormal, Movement Rate = " << simHist << ", Frame ID = " << frameIdVdec_ <<
            ", Channel ID = " << channelIdVdec_;
    } else {
        LogDebug << "PTZ Movement Detection: Normal, Frame ID = " << frameIdVdec_ << ", Channel ID = " <<
            channelIdVdec_;
    }
    LogDebug << "End to process PTZMovementDetection.";
    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::ImageFormatConversion(MxpiVision visionItem)
{
    MxpiVisionInfo visionItemInfo = visionItem.visioninfo();
    MxpiVisionData visionItemData = visionItem.visiondata();

    auto inputData = visionItemData.dataptr();
    auto dataSize = (uint32_t)visionItemData.datasize();
    uint32_t dstImageWidth = visionItemInfo.widthaligned();
    uint32_t dstImageHeigh = visionItemInfo.heightaligned();
    uint32_t srcImageWidth = dstImageWidth;
    uint32_t srcImageHeigh = static_cast<uint32_t>(dstImageHeigh * NV12_COEF);

    MemoryData dataDevice((void *)inputData, dataSize, MxBase::MemoryData::MEMORY_DEVICE);
    MemoryData dataHost(dataSize, MxBase::MemoryData::MEMORY_HOST);

    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(dataHost, dataDevice);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Failed to create destination memory, type(" << dataHost.type
                   << "), size(" << dataHost.size << ")." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }

    Mat srcNV12Mat(srcImageHeigh, srcImageWidth, CV_8UC1, dataHost.ptrData);
    Mat dstRGB888(dstImageHeigh, dstImageWidth, CV_8UC3);
    cvtColor(srcNV12Mat, dstRGB888, COLOR_YUV2RGB_NV12); // COLOR_YUV2RGB_NV12
    ret = MxBase::MemoryHelper::MxbsFree(dataHost);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to free host memory." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }

    if (frameList_.size() >= frameListMaxLen_) {
        frameList_.pop_front();
    }
    FrameInfo imageInfo = { frameIdCur_, frameIdVdec_, channelIdVdec_, dstRGB888 };
    frameList_.push_back(imageInfo);

    return APP_ERR_OK;
}


APP_ERROR MxpiQualityDetection::DetectionProcess(void)
{
    if (frameList_.size() < frameListMaxLen_) {
        LogInfo << "The list size of frameList_ is less than " << frameListMaxLen_ << ", Channel ID = " <<
            channelIdVdec_;
        return APP_ERR_OK;
    }
    DetectionWithKey(static_cast<int>(DetectionType::IMAGE_BRIGHTNESS_DETECTION));
    DetectionWithKey(static_cast<int>(DetectionType::IMAGE_OCCLUSION_DETECTION));
    DetectionWithKey(static_cast<int>(DetectionType::IMAGE_BLUR_DETECTION));
    DetectionWithKey(static_cast<int>(DetectionType::IMAGE_NOISE_DETECTION));
    DetectionWithKey(static_cast<int>(DetectionType::IMAGE_COLOR_CAST_DETECTION));
    DetectionWithKey(static_cast<int>(DetectionType::IMAGE_STRIPE_DETECTION));
    DetectionWithKey(static_cast<int>(DetectionType::BLACK_SCREEN_DETECTION));
    DetectionWithKey(static_cast<int>(DetectionType::SCENE_MUTATION_DETECTION));
    DetectionWithKey(static_cast<int>(DetectionType::PTZ_MOVEMENT_DETECTION));
    DetectionWithKey(static_cast<int>(DetectionType::VIEW_SHAKE_DETECTION));
    DetectionWithKey(static_cast<int>(DetectionType::VIDEO_FREEZE_DETECTION));

    return APP_ERR_OK;
}

APP_ERROR MxpiQualityDetection::ImageProcessAndDetection(MxpiBuffer &buffer)
{
    APP_ERROR ret = APP_ERR_OK;
    MxpiMetadataManager mxpiMetadataManager(buffer);
    shared_ptr<void> metadata = mxpiMetadataManager.GetMetadata(dataSource_);
    if (metadata == nullptr) {
        errorInfo_ << "Metadata is null." << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL; // self define the error code
    }
    auto message = (google::protobuf::Message *)metadata.get();
    const google::protobuf::Descriptor *desc = message->GetDescriptor();
    if (!desc) {
        errorInfo_ << "Get input data's descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM; // self define the error code
    }
    if (desc->name() != QUALITY_DETECTION_KEY) {
        errorInfo_ << "Not a MxpiVisionList." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH; // self define the error code
    }
    shared_ptr<void> metadataFrameInfo = mxpiMetadataManager.GetMetadata("ReservedFrameInfo");
    if (metadataFrameInfo == nullptr) {
        LogError << "Fail to get meta data." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    shared_ptr<MxpiFrameInfo> mxpiFrameInfo = static_pointer_cast<MxpiFrameInfo>(metadataFrameInfo);
    frameIdVdec_ = mxpiFrameInfo->frameid();
    channelIdVdec_ = mxpiFrameInfo->channelid();
    shared_ptr<MxpiVisionList> mxpiVisionList = static_pointer_cast<MxpiVisionList>(metadata);

    for (int i = 0; i < mxpiVisionList->visionvec_size(); i++) {
        MxpiVision visionItem = mxpiVisionList->visionvec(i);
        ret = ImageFormatConversion(visionItem);
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            return ret;
        }
        ret = DetectionProcess();
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Failed to finish detection process." << GetErrorInfo(ret);
            return APP_ERR_COMM_FAILURE;
        }
        frameIdCur_++;
    }
    return ret;
}

APP_ERROR MxpiQualityDetection::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiQualityDetection(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxpiBuffer *buffer = mxpiBuffer[0];
    errorInfo_.str("");

    ret = ImageProcessAndDetection(*buffer);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }

    SendData(0, *buffer); // Send the data to downstream plugin
    LogDebug << "End to process MxpiQualityDetection(" << elementName_ << ").";
    return ret;
}

std::vector<std::shared_ptr<void>> MxpiQualityDetection::DefineProperties()
{
    // Define an A to store properties
    std::vector<std::shared_ptr<void>> properties;
    // Set the type and related information of the properties, and the key is the name
    auto configContent = std::make_shared<ElementProperty<string>>(ElementProperty<string>{
        STRING, "qualityDetectionConfigContent", "config", "config content, its type is [json::object]", "", "", ""});
    auto configPath = std::make_shared<ElementProperty<string>>(
        ElementProperty<string>{STRING, "qualityDetectionConfigPath", "config", "config path", "", "", ""});

    properties.push_back(configContent);
    properties.push_back(configPath);
    return properties;
}

MxpiPortInfo MxpiQualityDetection::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = { { "image/yuv", "metadata/object" } };
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxpiPortInfo MxpiQualityDetection::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = { { "ANY" } };
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

APP_ERROR MxpiQualityDetection::DetectionWithKey(int detectionType)
{
    using detectionMap = map<int, DoDetection>::const_iterator;
    detectionMap iter = keyToHandle.find(detectionType);
    DoDetection detectionFunc = iter->second;
    return (this->*detectionFunc)();
}

void MxpiQualityDetection::MapKeyToHandle(void)
{
    keyToHandle[static_cast<int>(DetectionType::IMAGE_BRIGHTNESS_DETECTION)] =
        &MxpiQualityDetection::ImageBrightnessDetection;
    keyToHandle[static_cast<int>(DetectionType::IMAGE_OCCLUSION_DETECTION)] =
        &MxpiQualityDetection::ImageOcclusionDetection;
    keyToHandle[static_cast<int>(DetectionType::IMAGE_BLUR_DETECTION)] =
        &MxpiQualityDetection::ImageBlurDetection;
    keyToHandle[static_cast<int>(DetectionType::IMAGE_NOISE_DETECTION)] =
        &MxpiQualityDetection::ImageNoiseDetection;
    keyToHandle[static_cast<int>(DetectionType::IMAGE_COLOR_CAST_DETECTION)] =
        &MxpiQualityDetection::ImageColorCastDetection;
    keyToHandle[static_cast<int>(DetectionType::IMAGE_STRIPE_DETECTION)] =
        &MxpiQualityDetection::ImageStripeDetection;
    keyToHandle[static_cast<int>(DetectionType::BLACK_SCREEN_DETECTION)] =
        &MxpiQualityDetection::BlackScreenDetection;
    keyToHandle[static_cast<int>(DetectionType::VIDEO_FREEZE_DETECTION)] =
        &MxpiQualityDetection::VideoFreezeDetection;
    keyToHandle[static_cast<int>(DetectionType::VIEW_SHAKE_DETECTION)] =
        &MxpiQualityDetection::ViewShakeDetection;
    keyToHandle[static_cast<int>(DetectionType::SCENE_MUTATION_DETECTION)] =
        &MxpiQualityDetection::SceneMutationDetection;
    keyToHandle[static_cast<int>(DetectionType::PTZ_MOVEMENT_DETECTION)] =
        &MxpiQualityDetection::PTZMovementDetection;
}

// Register the MxpiQualityDetection plugin through macro
MX_PLUGIN_GENERATE(MxpiQualityDetection)
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
 * Description: Checker of the Video Encode.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef MXBASE_VIDEOENCODERCHECK_H
#define MXBASE_VIDEOENCODERCHECK_H
#include "MxBase/E2eInfer/VideoEncoder/VideoEncoder.h"
namespace MxBase {
    constexpr uint32_t MAX_FIRST_FRAME_START_QP = 51;
    constexpr uint32_t CVBR_MAX_QP = 47;
    constexpr uint32_t CVBR_MODE_VALUE = 5;
    constexpr uint32_t MAX_CACHE_COUNT = 256;

    struct Config310 {
        size_t inputWidthMin = 128;
        size_t inputWidthMax = 1920;
        size_t inputHeightMin = 128;
        size_t inputHeightMax = 1920;
        size_t rcModeMin = 0;
        size_t rcModeMax = 2;
        size_t ipPropMin = 1;
        size_t ipPropMax = 100;
        size_t keyFrameIntervalMin = 1;
        size_t keyFrameIntervalMax = 2000;
        size_t srcRateMin = 1;
        size_t srcRateMax = 120;
        size_t maxBitRateMin = 10;
        size_t maxBitRateMax = 30000;
        size_t widthStride = 16;
        size_t heightStride = 2;
        std::vector<ImageFormat> formats = {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420};
    };

    struct Config310B {
        size_t inputWidthMin = 114;
        size_t inputWidthMax = 8192;
        size_t inputHeightMin = 114;
        size_t inputHeightMax = 8192;
        size_t rcModeMin = 0;
        size_t rcModeMax = 2;
        size_t ipPropMin = 1;
        size_t ipPropMax = 100;
        size_t keyFrameIntervalMin = 1;
        size_t keyFrameIntervalMax = 65536;
        size_t srcRateMin = 1;
        size_t srcRateMax = 240;
        size_t maxBitRateMin = 2;
        size_t maxBitRateMax = 614400;
        size_t widthStride = 2;
        size_t heightStride = 2;
        std::vector<ImageFormat> formats = {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420};
    };

    struct Config310P {
        size_t channelIdMax = 127;
        size_t inputWidthMin = 128;
        size_t inputWidthMax = 4096;
        size_t inputHeightMin = 128;
        size_t inputHeightMax = 4096;
        size_t maxArea = 4096 * 2304;
        size_t displayRateMin = 1;
        size_t displayRateMax = 120;
        size_t rcModeMin = 0;
        size_t rcModeMax = 5;
        size_t ipPropMin = 1;
        size_t ipPropMax = 100;
        size_t sceneModeMin = 0;
        size_t sceneModeMax = 1;
        size_t keyFrameIntervalMin = 1;
        size_t keyFrameIntervalMax = 65536;
        size_t srcRateMin = 1;
        size_t srcRateMax = 240;
        size_t maxBitRateMin = 2;
        size_t maxBitRateMax = 614400;
        size_t statsTimeMin = 1;
        size_t statsTimeMax = 60;
        size_t directionMin = 0;
        size_t directionMax = 16;
        size_t rowQpDeltaMin = 0;
        size_t rowQpDeltaMax = 10;
        size_t thresholdMin = 0;
        size_t thresholdMax = 255;
        size_t thresholdSize = 16;
        size_t shortTermStatsTimeMin = 1;
        size_t shortTermStatsTimeMax = 120;
        size_t longTermStatsTimeMin = 1;
        size_t longTermStatsTimeMax = 1440;
        size_t longTermMaxBitRateMin = 2;
        size_t longTermMinBitRateMin = 0;
        size_t widthStride = 2;
        size_t heightStride = 2;
        const std::map<uint32_t, uint32_t> MIN_QP_MAP = {
            {0, 10},  // key: rcMode  value: minQpValue
            {1, 10},
            {2, 24},
            {3, 24},
            {4, 16},
            {5, 20}
        };
        std::vector<ImageFormat> formats = {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420, ImageFormat::RGB_888,
                                            ImageFormat::BGR_888};
    };

    class Checker {
    public:
        Checker() = default;
        virtual ~Checker() = default;
        APP_ERROR InputImageCheck(const VideoEncodeConfig& input, const Image &inputImage, int32_t deviceId)
        {
            if (inputImage.GetDeviceId() == -1) {
                LogError << "Input image cannot be on the host, please check."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            } else if (inputImage.GetDeviceId() != deviceId) {
                LogError << "Input image get wrong deviceId(" << inputImage.GetDeviceId()
                         << "), which should be equal to VideoEncoder(" << deviceId << ")."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (inputImage.GetFormat() != input.inputImageFormat) {
                LogError << "input image get wrong format(" << IMAGE_FORMAT_STRING.at(inputImage.GetFormat())
                         << "), which should be " << IMAGE_FORMAT_STRING.at(input.inputImageFormat) << "."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (inputImage.GetData() == nullptr) {
                LogError << "input image data is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            return APP_ERR_OK;
        }
    protected:
        template<class ConfigType>
        APP_ERROR BaseCheck(const ConfigType& config, const VideoEncodeConfig& input)
        {
            if (input.callbackFunc == nullptr) {
                LogError << "Input param(callbackFunc) in VideoEncodeConfig is invalid, which should not be nullptr"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("width", input.width, config.inputWidthMin, config.inputWidthMax) ||
                (input.width % config.widthStride != 0)) {
                LogError << "Or width should be stride with " << config.widthStride << "."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("height", input.height, config.inputHeightMin, config.inputHeightMax) ||
                (input.height % config.heightStride != 0)) {
                LogError << "Or height should be stride with " << config.heightStride << "."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("srcRate", input.srcRate, config.srcRateMin, config.srcRateMax)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("maxBitRate", input.maxBitRate, config.maxBitRateMin, config.maxBitRateMax)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("rcMode", input.rcMode, config.rcModeMin, config.rcModeMax)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("ipProp", input.ipProp, config.ipPropMin, config.ipPropMax)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("keyFrameInterval", input.keyFrameInterval, config.keyFrameIntervalMin,
                           config.keyFrameIntervalMax)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            for (const auto& iter: config.formats) {
                if (iter == input.inputImageFormat) {
                    return APP_ERR_OK;
                }
            }
            LogError << "Input param(inputImageFormat) in VideoEncodeConfig is invalid("
                     << IMAGE_FORMAT_STRING.at(input.inputImageFormat)
                     << "), which should be YUV_SP_420, YVU_SP_420, RGB_888, BGR_888 for 310P or YUV_SP_420, "
                        "YVU_SP_420 for 310 or 310B."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        bool CheckRange(const std::string paramName, uint32_t input, uint32_t min, uint32_t max)
        {
            if (input < min || input > max) {
                LogError << "Input param(" << paramName << ") is invalid(" << input
                         << "), which should be in range[" << min << ", " << max << "]."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return true;
            }
            return false;
        }
    };

    class Checker310 : public Checker {
    public:
        APP_ERROR Check(const Config310& config, const VideoEncodeConfig& input)
        {
            return BaseCheck(config, input);
        }
    };

    class Checker310B : public Checker {
    public:
        APP_ERROR Check(const Config310B& config, const VideoEncodeConfig& input)
        {
            return BaseCheck(config, input);
        }
    };

    class Checker310P : public Checker {
    public:
        APP_ERROR Check(const Config310P& config, const VideoEncodeConfig& input, uint32_t channelId)
        {
            APP_ERROR ret = BaseCheck(config, input);
            if (ret != APP_ERR_OK) {
                return ret;
            }
            if (CheckRange("channelId", channelId, 0, config.channelIdMax)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckHeightAndWidth(config, input) != APP_ERR_OK) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("displayRate", input.displayRate, config.displayRateMin, config.displayRateMax)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("sceneMode", input.sceneMode, config.sceneModeMin, config.sceneModeMax)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("statsTime", input.statsTime, config.statsTimeMin, config.statsTimeMax)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("direction", input.direction, config.directionMin, config.directionMax)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("rowQpDelta", input.rowQpDelta, config.rowQpDeltaMin, config.rowQpDeltaMax)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (input.rcMode == CVBR_MODE_VALUE) {
                if (CheckRange("shortTermStatsTime", input.shortTermStatsTime, config.shortTermStatsTimeMin,
                               config.shortTermStatsTimeMax)) {
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                if (CheckRange("longTermStatsTime", input.longTermStatsTime, config.longTermStatsTimeMin,
                               config.longTermStatsTimeMax)) {
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                if (CheckRange("longTermMaxBitRate", input.longTermMaxBitRate, config.longTermMaxBitRateMin,
                               input.maxBitRate)) {
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                if (CheckRange("longTermMinBitRate", input.longTermMinBitRate, config.longTermMinBitRateMin,
                               input.longTermMaxBitRate)) {
                    return APP_ERR_COMM_INVALID_PARAM;
                }
            }
            if (CheckThresholdIBP(config, input)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            return CheckFirstFrameQpParam(config, input);
        }
    private:
        APP_ERROR CheckThresholdIBP(const Config310P& config, const VideoEncodeConfig& input)
        {
            if (input.thresholdI.size() != config.thresholdSize || input.thresholdP.size() != config.thresholdSize ||
                input.thresholdB.size() != config.thresholdSize) {
                LogError << "The size of [thresholdI/thresholdP/thresholdB] must equal to"
                         << config.thresholdSize << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            for (uint32_t i = 0; i < config.thresholdSize; i++) {
                if (CheckRange("thresholdI[" + std::to_string(i) + "]", input.thresholdI[i], config.thresholdMin,
                               config.thresholdMax) ||
                    CheckRange("thresholdP[" + std::to_string(i) + "]", input.thresholdP[i], config.thresholdMin,
                               config.thresholdMax) ||
                    CheckRange("thresholdB[" + std::to_string(i) + "]", input.thresholdB[i], config.thresholdMin,
                               config.thresholdMax)) {
                    return APP_ERR_COMM_INVALID_PARAM;
                }
            }
            return APP_ERR_OK;
        }
        APP_ERROR CheckFirstFrameQpParam(const Config310P& config, const VideoEncodeConfig& input)
        {
            uint32_t maxQpValue = MAX_FIRST_FRAME_START_QP;
            if (input.rcMode == CVBR_MODE_VALUE) {
                maxQpValue = CVBR_MAX_QP;
            }
            auto iter = config.MIN_QP_MAP.find(input.rcMode);
            if (iter == config.MIN_QP_MAP.end()) {
                LogError << "Wrong rcMode, support range is [" << config.rcModeMin << ", " << config.rcModeMax
                         << "], please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("firstFrameStartQp", input.firstFrameStartQp, iter->second, maxQpValue)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            return APP_ERR_OK;
        }

        APP_ERROR CheckHeightAndWidth(const Config310P& config, const VideoEncodeConfig& input)
        {
            if (input.maxPicHeight < input.height ||  input.maxPicWidth < input.width) {
                LogError << "The maxPicHeight should be large than or equal to the height and "
                         << "the maxPicWidth should be large than or equal to the width."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if ((input.width * input.height) > config.maxArea) {
                LogError << "Input width * height in VideoEncodeConfig should not be bigger than " << config.maxArea
                         << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("maxPicWidth", input.maxPicWidth, config.inputWidthMin, config.inputWidthMax)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            if (CheckRange("maxPicHeight", input.maxPicHeight, config.inputHeightMin, config.inputHeightMax)) {
                return APP_ERR_COMM_INVALID_PARAM;
            }
            return APP_ERR_OK;
        }
    };
}
#endif

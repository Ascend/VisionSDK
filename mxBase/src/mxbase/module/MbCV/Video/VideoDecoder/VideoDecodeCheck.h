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
 * Description: Checker of the Video decode process.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_VIDEODECODECHECK_H
#define MXBASE_VIDEODECODECHECK_H
#include "MxBase/E2eInfer/VideoDecoder/VideoDecoder.h"
namespace MxBase {
struct BaseConfig {
    uint32_t videoWidthMin = 128;
    uint32_t videoWidthMax = 4096;
    uint32_t videoHeightMin = 128;
    uint32_t videoHeightMax = 4096;
    uint32_t widthAlign = 2;
    uint32_t heightAlign = 2;
    uint32_t skipInterval = 250;
};
struct Config310 : public BaseConfig {
    uint32_t channelIdMax = 31;
    std::vector<ImageFormat> formats = {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420};
};
struct Config310B : public BaseConfig {
    uint32_t channelIdMax = 127;
    std::vector<ImageFormat> formats = {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420, ImageFormat::RGB_888,
                                        ImageFormat::BGR_888};
};

struct Config310P: public BaseConfig {
    uint32_t channelIdMax = 255;
    std::vector<ImageFormat> formats = {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420, ImageFormat::RGB_888,
                                        ImageFormat::BGR_888};
    uint32_t cscMatrixMin = 0;
    uint32_t cscMatrixMax = 5;
};

struct ConfigAtlas800IA2: public BaseConfig {
    uint32_t channelIdMax = 255;
    std::vector<ImageFormat> formats = {ImageFormat::YUV_SP_420, ImageFormat::YVU_SP_420, ImageFormat::RGB_888,
                                        ImageFormat::BGR_888};
    uint32_t cscMatrixMin = 0;
    uint32_t cscMatrixMax = 5;
};

template<class ConfigType>
APP_ERROR BaseCheck(const VideoDecodeConfig& input, uint32_t chId, const ConfigType& config)
{
    if (input.callbackFunc == nullptr) {
        LogError << "Input param(callbackFunc) in VideoDecodeConfig is invalid, which should not be nullptr."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (input.width < config.videoWidthMin || input.width > config.videoWidthMax ||
        (input.width % config.widthAlign != 0)) {
        LogError << "Input param(width) in VideoDecodeConfig is invalid, which should be in range["
                 << config.videoWidthMin << ", " << config.videoWidthMax << "], and stride with "
                 << config.widthAlign << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (input.height < config.videoHeightMin || input.height > config.videoHeightMax ||
        (input.height % config.heightAlign != 0)) {
        LogError << "Input param(height) in VideoDecodeConfig is invalid, which should be in range["
                 << config.videoHeightMin << ", " << config.videoHeightMax << "], and stride with "
                 << config.heightAlign << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (input.skipInterval > config.skipInterval) {
        LogError << "Input param(skipInterval) in VideoDecodeConfig is invalid, which should be in range[0"
                 << ", " << config.skipInterval << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (chId > config.channelIdMax) {
        LogError << "Input param(channelId) in VideoDecodeConfig is invalid(" << chId
                 << "), which should be in range[0, "
                 << config.channelIdMax << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (const auto& iter: config.formats) {
        if (iter == input.outputImageFormat) {
            return APP_ERR_OK;
        }
    }
    LogError << "Input param(outputImageFormat) in VideoDecodeConfig is invalid("
             << IMAGE_FORMAT_STRING.at(input.outputImageFormat) << "), which should be YUV_SP_420"
             << "or YVU_SP_420 for 310 or YUV_SP_420, YVU_SP_420, RGB_888, BGR_888 for 310P, 310B and Atlas800IA2."
             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
    return APP_ERR_COMM_INVALID_PARAM;
}

APP_ERROR Check310(const VideoDecodeConfig& input, uint32_t chId, const Config310& config)
{
    return BaseCheck(input, chId, config);
}

APP_ERROR Check310B(const VideoDecodeConfig& input, uint32_t chId, const Config310B& config)
{
    return BaseCheck(input, chId, config);
}

APP_ERROR Check310P(const VideoDecodeConfig& input, uint32_t chId, const Config310P& config)
{
    if (input.cscMatrix > config.cscMatrixMax) {
        LogError << "Input param(cscMatrix) in VideoDecodeConfig is invalid, expect ["
                 << config.cscMatrixMin << ", " << config.cscMatrixMax << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return BaseCheck(input, chId, config);
}

APP_ERROR CheckAtlas800IA2(const VideoDecodeConfig& input, uint32_t chId, const ConfigAtlas800IA2& config)
{
    if (input.cscMatrix > config.cscMatrixMax) {
        LogError << "Input param(cscMatrix) in VideoDecodeConfig is invalid, expect ["
                 << config.cscMatrixMin << ", " << config.cscMatrixMax << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return BaseCheck(input, chId, config);
}
}
#endif

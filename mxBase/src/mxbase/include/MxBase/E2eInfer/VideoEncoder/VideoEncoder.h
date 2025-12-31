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
 * Description: Processing of the Video Encode Function.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef MX_VIDEOENCODER_H
#define MX_VIDEOENCODER_H

#include "MxBase/E2eInfer/DataType.h"
#include "MxBase/E2eInfer/Image/Image.h"
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxBase {
typedef APP_ERROR (*VideoEncodeCallBack)(std::shared_ptr<uint8_t>& outDataPtr, uint32_t& outDataSize,
                                         uint32_t& channelId, uint32_t& frameId, void* userData);

struct VideoEncodeConfig {
    uint32_t maxPicWidth = 4096;
    uint32_t maxPicHeight = 4096;
    uint32_t width = 1920;
    uint32_t height = 1080;
    StreamFormat outputVideoFormat = StreamFormat::H264_MAIN_LEVEL;
    ImageFormat inputImageFormat = ImageFormat::YUV_SP_420;
    VideoEncodeCallBack callbackFunc = nullptr;
    uint32_t keyFrameInterval = 30;
    uint32_t srcRate = 30;
    uint32_t rcMode = 0;
    uint32_t shortTermStatsTime = 60;
    uint32_t longTermStatsTime = 120;
    uint32_t longTermMaxBitRate = 300;
    uint32_t longTermMinBitRate = 0;
    uint32_t maxBitRate = 300;
    uint32_t ipProp = 70;
    uint32_t sceneMode = 0;
    uint32_t displayRate = 30;
    uint32_t statsTime = 1;
    uint32_t firstFrameStartQp = 32;
    std::vector<uint32_t> thresholdI = {0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255};
    std::vector<uint32_t> thresholdP = {0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255};
    std::vector<uint32_t> thresholdB  = {0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255};
    uint32_t direction = 8;
    uint32_t rowQpDelta = 1;
};

class VideoEncoderDptr;

class VideoEncoder {
public:
    VideoEncoder(const VideoEncodeConfig& vEncodeConfig, const int32_t deviceId = 0, const uint32_t channelId = 0);
    ~VideoEncoder();

    APP_ERROR Encode(const Image &inputImage, const uint32_t frameId, void* userData);

private:
    std::shared_ptr<MxBase::VideoEncoderDptr> videoEncoderDptr_;
};
}
#endif
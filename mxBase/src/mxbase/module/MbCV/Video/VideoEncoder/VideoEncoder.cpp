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

#include "VideoEncoderDptr.hpp"

namespace MxBase {
static bool g_existEncoderAcl = false;
VideoEncoder::VideoEncoder(const VideoEncodeConfig &vEncodeConfig, const int32_t deviceId, const uint32_t channelId)
{
    if (g_existEncoderAcl) {
        LogError << "VideoEncoder already has been inited." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    videoEncoderDptr_ = MemoryHelper::MakeShared<MxBase::VideoEncoderDptr>(vEncodeConfig, deviceId, channelId);
    if (videoEncoderDptr_ == nullptr) {
        LogError << "Failed to create VideoEncoder object." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    if (DeviceManager::IsAscend310() || DeviceManager::IsAscend310B()) {
        g_existEncoderAcl = true;
    }
}

VideoEncoder::~VideoEncoder()
{
    g_existEncoderAcl = false;
    videoEncoderDptr_.reset();
}

APP_ERROR VideoEncoder::Encode(const Image &inputImage, const uint32_t frameId, void* userData)
{
    return videoEncoderDptr_->Encode(inputImage, frameId, userData);
}
}
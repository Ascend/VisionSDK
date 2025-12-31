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
 * Description: Processing of the Video Decode Function.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#include "VideoDecoderDptr.hpp"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxBase {
VideoDecoder::VideoDecoder(const VideoDecodeConfig& vDecodeConfig, const int32_t deviceId, const uint32_t channelId)
{
    videoDecoderDptr_ = MemoryHelper::MakeShared<MxBase::VideoDecoderDptr>(vDecodeConfig, deviceId, channelId, this);
    if (videoDecoderDptr_ == nullptr) {
        LogError << "Failed to create VideoDecoder object." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

VideoDecoder::~VideoDecoder()
{
    videoDecoderDptr_.reset();
}

APP_ERROR VideoDecoder::Decode(const std::shared_ptr<uint8_t> data, const uint32_t dataSize,
                               const uint32_t frameId, void* userData)
{
    return videoDecoderDptr_->Decode(data, dataSize, frameId, userData);
}

APP_ERROR VideoDecoder::Decode(const std::shared_ptr<uint8_t> data, const uint32_t dataSize, const uint32_t frameId,
                               Image& preMallocData, void* userData)
{
    return videoDecoderDptr_->Decode(data, dataSize, frameId, preMallocData, userData);
}

APP_ERROR VideoDecoder::Flush()
{
    return videoDecoderDptr_->Flush();
}

void VideoDecoder::SetImageWH(Image& inputImage, const Size size)
{
    inputImage.SetOriginalSize(size);
}
}
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
 * Description: Private interface of the Video encode process.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef VIDEOENCODERDPTR_HPP
#define VIDEOENCODERDPTR_HPP

#include <unordered_map>
#include "MxBase/E2eInfer/VideoEncoder/VideoEncoder.h"
#include "MxBase/Log/Log.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "VideoEncoderCheck.h"

namespace MxBase {

class VideoEncoderDptr {
public:
    VideoEncoderDptr(const VideoEncodeConfig& vEncodeConfig, const int32_t deviceId, const uint32_t channelId);
    VideoEncoderDptr(const VideoEncoderDptr&) = delete;
    VideoEncoderDptr& operator=(const VideoEncoderDptr&) = delete;
    ~VideoEncoderDptr();

    APP_ERROR Encode(const Image &inputImage, const uint32_t frameId, void* userData);

public:
    int32_t deviceId_ = 0;
    uint32_t deviceCount_ = 0;
    std::shared_ptr<DvppWrapper> dvppWrapper_;
    VideoEncodeConfig vEncodeConfig_;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)> callbackHandle_ = {};
    void *userData_;
    uint32_t handleFrameId_ = 0;
    uint32_t channelId_ = 0;
    pthread_mutex_t releaseVencMutex_ = PTHREAD_MUTEX_INITIALIZER;
    std::unordered_map<void*, Image> processCache_ = {};
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**, void*)> callbackHandleFor310P_ = {};

private:
    APP_ERROR SetDevice();
    APP_ERROR CheckVencConfig();
    APP_ERROR InitDvppWrapperVenc(VencConfig& vencConfig);
    void GetVencConfig(VencConfig& vencConfig);
    APP_ERROR CreateEncodeCallBack();
    APP_ERROR EncodeCallBackCommon(std::shared_ptr<uint8_t> data, uint32_t streamSize, void** inputAddr);
};

APP_ERROR VideoEncoderDptr::SetDevice()
{
    DeviceContext context = {};
    context.devId = static_cast<int>(deviceId_);
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "VideoEncoder setDevice failed, please check the state of device." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR VideoEncoderDptr::EncodeCallBackCommon(std::shared_ptr<uint8_t> data, uint32_t streamSize, void** inputAddr)
{
    pthread_mutex_lock(&releaseVencMutex_);
    processCache_.erase(*inputAddr);
    pthread_mutex_unlock(&releaseVencMutex_);
    if (data.get() == nullptr) {
        LogError << "data is invalid" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (streamSize == 0) {
        LogError << "data size is equal to 0" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    handleFrameId_++;
    return APP_ERR_OK;
}

APP_ERROR VideoEncoderDptr::CreateEncodeCallBack()
{
    if (DeviceManager::IsAscend310P()) {
        callbackHandleFor310P_ = [this](std::shared_ptr<uint8_t> data, uint32_t streamSize, void** inputAddr,
            void* userData) {
            if (EncodeCallBackCommon(data, streamSize, inputAddr) != APP_ERR_OK) {
                return;
            }
            VideoEncodeCallBack callPtr = vEncodeConfig_.callbackFunc;
            callPtr(data, streamSize, channelId_, handleFrameId_, userData);
        };
    } else {
        callbackHandle_ = [this](std::shared_ptr<uint8_t> data, uint32_t streamSize, void** inputAddr) {
            if (EncodeCallBackCommon(data, streamSize, inputAddr) != APP_ERR_OK) {
                return;
            }
            VideoEncodeCallBack callPtr = vEncodeConfig_.callbackFunc;

            callPtr(data, streamSize, channelId_, handleFrameId_, userData_);
        };
    }
    return APP_ERR_OK;
}

VideoEncoderDptr::VideoEncoderDptr(const VideoEncodeConfig &vEncodeConfig, const int32_t deviceId,
                                   const uint32_t channelId)
{
    vEncodeConfig_ = vEncodeConfig;
    deviceId_ = deviceId;
    channelId_ = channelId;

    APP_ERROR ret = SetDevice();
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }

    MxBase::DeviceManager::GetInstance()->GetDevicesCount(deviceCount_);
    if (deviceCount_ == 0) {
        LogError << "Cannot recognize the count of chip." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }

    ret = CheckVencConfig();
    if (ret != APP_ERR_OK) {
        LogError << "Checking VideoEncodeConfig failed in VideoEncoder." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }

    VencConfig vencConfig;
    GetVencConfig(vencConfig);
    if (CreateEncodeCallBack() != APP_ERR_OK) {
        LogError << "Fail to create encode callback in VideoEncoder." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }

    if (DeviceManager::IsAscend310P()) {
        vencConfig.userDataWithInputFor310P = &callbackHandleFor310P_;
    } else {
        vencConfig.userDataWithInput = &callbackHandle_;
    }

    ret = InitDvppWrapperVenc(vencConfig);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    LogInfo << "VideoEncoder init successfully.";
}

APP_ERROR VideoEncoderDptr::InitDvppWrapperVenc(VencConfig& vencConfig)
{
    dvppWrapper_ = MemoryHelper::MakeShared<DvppWrapper>();
    if (dvppWrapper_ == nullptr) {
        LogError << "Failed to make shared dvppWrapper_ object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    APP_ERROR ret = dvppWrapper_->InitVenc(vencConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to initialize dvppWrapper_ object." << GetErrorInfo(ret);
    }
    return ret;
}

VideoEncoderDptr::~VideoEncoderDptr()
{
    pthread_mutex_destroy(&releaseVencMutex_);

    SetDevice();

    APP_ERROR ret = dvppWrapper_->DeInitVenc();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to deinitialize dvppWrapper_." << GetErrorInfo(ret);
    }
    dvppWrapper_.reset();
}

APP_ERROR VideoEncoderDptr::Encode(const Image &inputImage, const uint32_t frameId, void* userData)
{
    // (1) Check input Image.
    APP_ERROR ret = Checker().InputImageCheck(vEncodeConfig_, inputImage, deviceId_);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to check encode input image." << GetErrorInfo(ret);
        return ret;
    }

    ret = SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }

    // (2) Construct input image data.
    DvppDataInfo inputDataInfo;
    inputDataInfo.width = inputImage.GetOriginalSize().width;
    inputDataInfo.height = inputImage.GetOriginalSize().height;
    inputDataInfo.widthStride = inputImage.GetSize().width;
    inputDataInfo.heightStride = inputImage.GetSize().height;
    inputDataInfo.device = inputImage.GetDeviceId();
    inputDataInfo.format = static_cast<MxbasePixelFormat>(inputImage.GetFormat());
    inputDataInfo.data = inputImage.GetData().get();
    inputDataInfo.dataSize = inputImage.GetDataSize();
    inputDataInfo.frameId = frameId;
    inputDataInfo.channelId = channelId_;
    inputDataInfo.outData = (uint8_t*)userData;

    userData_ = userData;

    pthread_mutex_lock(&releaseVencMutex_);
    if (processCache_.size() > MAX_CACHE_COUNT) {
        LogError << "The number of the frame waiting for encoding is too large, cannot send more data now."
                 << GetErrorInfo(APP_ERR_COMM_FULL);
        pthread_mutex_unlock(&releaseVencMutex_);
        return APP_ERR_COMM_FULL;
    }
    processCache_.insert({static_cast<void*>(inputDataInfo.data), inputImage});
    pthread_mutex_unlock(&releaseVencMutex_);
    if (DeviceManager::IsAscend310P()) {
        static std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* fakeHandleFunc = nullptr;
        ret = dvppWrapper_->DvppVenc(inputDataInfo, fakeHandleFunc);
    } else {
        ret = dvppWrapper_->DvppVenc(inputDataInfo, &callbackHandle_);
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to dvppWrapper_ DvppVenc." << GetErrorInfo(ret);
        pthread_mutex_lock(&releaseVencMutex_);
        processCache_.erase(inputDataInfo.data);
        pthread_mutex_unlock(&releaseVencMutex_);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR VideoEncoderDptr::CheckVencConfig()
{
    APP_ERROR ret = APP_ERR_OK;
    if (DeviceManager::IsAscend310()) {
        Config310 config;
        ret = Checker310().Check(config, vEncodeConfig_);
    } else if (DeviceManager::IsAscend310B()) {
        Config310B config;
        ret = Checker310B().Check(config, vEncodeConfig_);
    } else if (DeviceManager::IsAscend310P()) {
        Config310P config;
        ret = Checker310P().Check(config, vEncodeConfig_, channelId_);
    } else {
        LogError << "Could not recognize Ascend chip." << GetErrorInfo(ret);
        return APP_ERR_COMM_FULL;
    }
    return ret;
}

void VideoEncoderDptr::GetVencConfig(VencConfig& vencConfig)
{
    vencConfig.width = vEncodeConfig_.width;
    vencConfig.height = vEncodeConfig_.height;
    vencConfig.maxPicWidth = vEncodeConfig_.maxPicWidth;
    vencConfig.maxPicHeight = vEncodeConfig_.maxPicHeight;
    vencConfig.outputVideoFormat = static_cast<MxbaseStreamFormat>(vEncodeConfig_.outputVideoFormat);
    vencConfig.inputImageFormat = static_cast<MxbasePixelFormat>(vEncodeConfig_.inputImageFormat);
    vencConfig.keyFrameInterval = vEncodeConfig_.keyFrameInterval;
    vencConfig.deviceId = (uint32_t)deviceId_;
    vencConfig.srcRate = vEncodeConfig_.srcRate;
    vencConfig.rcMode = vEncodeConfig_.rcMode;
    vencConfig.maxBitRate = vEncodeConfig_.maxBitRate;
    vencConfig.ipProp = vEncodeConfig_.ipProp;
    vencConfig.channelId = channelId_;
    vencConfig.sceneMode = vEncodeConfig_.sceneMode;
    vencConfig.statsTime = vEncodeConfig_.statsTime;
    vencConfig.firstFrameStartQp = vEncodeConfig_.firstFrameStartQp;
    vencConfig.displayRate = vEncodeConfig_.displayRate;
    vencConfig.shortTermStatsTime = vEncodeConfig_.shortTermStatsTime;
    vencConfig.longTermStatsTime = vEncodeConfig_.longTermStatsTime;
    vencConfig.longTermMaxBitRate = vEncodeConfig_.longTermMaxBitRate;
    vencConfig.longTermMinBitRate = vEncodeConfig_.longTermMinBitRate;
    for (uint32_t i = 0; i < THRESHOLD_OF_ENCODE_RATE_VECTOR_LEN; i++) {
        vencConfig.thresholdI[i] = vEncodeConfig_.thresholdI[i];
        vencConfig.thresholdP[i] = vEncodeConfig_.thresholdI[i];
        vencConfig.thresholdB[i] = vEncodeConfig_.thresholdI[i];
    }
    vencConfig.direction = vEncodeConfig_.direction;
    vencConfig.rowQpDelta = vEncodeConfig_.rowQpDelta;
}
}
#endif
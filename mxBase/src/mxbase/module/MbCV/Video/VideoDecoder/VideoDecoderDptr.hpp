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
 * Description: Private interface of the Video decode process.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef VIDEODECODERDPTR_HPP
#define VIDEODECODERDPTR_HPP

#include "acl/acl.h"
#include "MxBase/E2eInfer/VideoDecoder/VideoDecoder.h"
#include "MxBase/Log/Log.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "VideoDecodeCheck.h"

namespace MxBase {
constexpr uint32_t RGB_CHANNEL = 3;
constexpr uint32_t RGB_CONVERT_YUV_NUM = 2;

enum class MemoryMode {
    NONE_MALLOC_MODE = 0,
    SYS_MALLOC,
    USER_MALLOC,
};

struct VideoDecodeH26x {
    VideoDecodeH26x(VideoDecodeCallBack pFunction, void *pVoid, uint32_t skipNum)
        : callbackFunc(pFunction), userData(pVoid), skipInterval(skipNum) {}
    VideoDecodeCallBack callbackFunc = nullptr;
    void* userData = nullptr;
    uint32_t skipInterval = 0;
};

class VideoDecoderDptr {
public:
    VideoDecoderDptr(const VideoDecodeConfig& vDecodeConfig, const int32_t deviceId,
                     const uint32_t channelId, VideoDecoder* pVideoDecoder);
    VideoDecoderDptr(const VideoDecoderDptr&) = delete;
    VideoDecoderDptr& operator=(const VideoDecoderDptr&) = delete;
    ~VideoDecoderDptr();

    APP_ERROR SetDevice();

    APP_ERROR Decode(const std::shared_ptr<uint8_t> data, const uint32_t dataSize,
                     const uint32_t frameId, void* userData);
    APP_ERROR Decode(const std::shared_ptr<uint8_t> data, const uint32_t dataSize,
                     const uint32_t frameId, Image& preMallocData, void* userData);
    APP_ERROR Flush();

private:
    int32_t deviceId_ = 0;
    uint32_t channelId_ = 0;
    std::shared_ptr<DvppWrapper> dvppWrapper_;
    VideoDecodeConfig vDecodeConfig_;
    VdecConfig vdecConfig_;
    aclrtRunMode runMode_;
    MemoryMode memoryMode_ = MemoryMode::NONE_MALLOC_MODE;
    VideoDecoder* qPtr_ = nullptr;

private:
    APP_ERROR GetRunMode();
    APP_ERROR CheckVdecConfig();
    void GetVdecConfig();
    APP_ERROR ConstructDvppDataInfo(const std::shared_ptr<uint8_t> data, const uint32_t dataSize,
                                    const uint32_t frameId, DvppDataInfo& inputDataInfo);
    static APP_ERROR CallBack(std::shared_ptr<void> buffer, DvppDataInfo &dvppDataInfo, void *userData);
};

APP_ERROR VideoDecoderDptr::SetDevice()
{
    DeviceContext context = {};
    context.devId = static_cast<int>(deviceId_);
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "VideoDecoder setDevice failed, please check the state of device." << GetErrorInfo(ret);
    }
    return ret;
}

void convertToOutPtr(std::shared_ptr<void>& inPtr, std::shared_ptr<uint8_t>& outPtr, bool userMalloc)
{
    auto dvppDeleter = [](void *dataPtr) -> void {
        if (dataPtr != nullptr) {
            MemoryData data;
            data.type = MemoryData::MEMORY_DVPP;
            data.ptrData = (void*)dataPtr;
            MemoryHelper::MxbsFree(data);
            data.ptrData = nullptr;
        }
    };
    auto nullDeleter = [](void*) -> void {};

    if (userMalloc) {
        outPtr.reset((uint8_t*)inPtr.get(), nullDeleter);
    } else {
        outPtr.reset((uint8_t*)inPtr.get(), dvppDeleter);
    }
}

APP_ERROR VideoDecoderDptr::CallBack(std::shared_ptr<void> buffer, DvppDataInfo &dvppDataInfo, void *userData)
{
    VideoDecoderDptr* videoDecodeDptr = (VideoDecoderDptr*) userData;
    if (videoDecodeDptr == nullptr) {
        LogError << "videoDecodeDptr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    ImageFormat imgFormat = static_cast<ImageFormat>(dvppDataInfo.format);

    std::shared_ptr<uint8_t> imgData;
    convertToOutPtr(buffer, imgData, (videoDecodeDptr->memoryMode_ == MemoryMode::USER_MALLOC));

    if (videoDecodeDptr->qPtr_ == nullptr) {
        LogError << "VideoDecoder get wrong init state." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (imgData == nullptr) {
        LogError << "VideoDecoder internal callback input buffer is nullptr."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    uint32_t dataSize = 0;
    // Recalculate dataSize, for vdec without resize.
    if (imgFormat == ImageFormat::YUV_SP_420 || imgFormat == ImageFormat::YVU_SP_420) {
        dataSize = dvppDataInfo.widthStride * dvppDataInfo.heightStride * RGB_CHANNEL / RGB_CONVERT_YUV_NUM;
    } else if (imgFormat == ImageFormat::RGB_888 || imgFormat == ImageFormat::BGR_888) {
        dataSize = dvppDataInfo.widthStride * dvppDataInfo.heightStride;
        dvppDataInfo.widthStride /= RGB_CHANNEL;
    }

    try {
        Image decodedImage(imgData, dataSize, videoDecodeDptr->deviceId_,
                           Size(dvppDataInfo.widthStride, dvppDataInfo.heightStride), imgFormat);
        videoDecodeDptr->qPtr_->SetImageWH(decodedImage, Size(dvppDataInfo.width, dvppDataInfo.height));

        VideoDecodeCallBack callPtr = videoDecodeDptr->vDecodeConfig_.callbackFunc;
        callPtr(decodedImage, dvppDataInfo.channelId, dvppDataInfo.frameId, videoDecodeDptr->vdecConfig_.userData);
    } catch (const std::exception& ex) {
        LogError << "(1) Catch the exception of user defined callback function, or"
                 << "(2) Failed to construct output image." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }

    return APP_ERR_OK;
}

APP_ERROR VideoDecoderDptr::GetRunMode()
{
    APP_ERROR ret = aclrtGetRunMode(&runMode_);
    if (ret != APP_ERR_OK) {
        LogError << "VideoDecoder: fail to get run mode of device." << GetErrorInfo(ret, "aclrtGetRunMode");
        return APP_ERR_ACL_FAILURE;
    }
    if (runMode_ == ACL_DEVICE) {
        LogInfo << "Software runMode in ACL_DEVICE.";
    } else {
        LogInfo << "Software runMode in ACL_HOST.";
    }
    return APP_ERR_OK;
}

VideoDecoderDptr::VideoDecoderDptr(const VideoDecodeConfig& vDecodeConfig, const int32_t deviceId,
                                   const uint32_t channelId, VideoDecoder* pVideoDecoder)
{
    qPtr_ = pVideoDecoder;
    vDecodeConfig_ = vDecodeConfig;
    deviceId_ = deviceId;
    channelId_ = channelId;

    APP_ERROR ret = CheckVdecConfig();
    if (ret != APP_ERR_OK) {
        LogError << "VideoDecoder check VideoDecodeConfig failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }

    GetVdecConfig();

    ret = SetDevice();
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }

    dvppWrapper_ = MemoryHelper::MakeShared<DvppWrapper>();
    if (dvppWrapper_ == nullptr) {
        LogError << "Failed to make shared dvppWrapper_ object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    ret = dvppWrapper_->InitVdec(vdecConfig_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to initialize dvppWrapper_ object." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }

    ret = GetRunMode();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to initialize dvppWrapper_ object." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    LogInfo << "VideoDecoder init successfully.";
}

VideoDecoderDptr::~VideoDecoderDptr()
{
    SetDevice();

    APP_ERROR ret = dvppWrapper_->DeInitVdec();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to deinitialize dvppWrapper_." << GetErrorInfo(ret);
    }
    dvppWrapper_.reset();
    qPtr_ = nullptr;
}

APP_ERROR VideoDecoderDptr::Flush()
{
    return dvppWrapper_->DvppVdecFlush();
}

APP_ERROR VideoDecoderDptr::Decode(const std::shared_ptr<uint8_t> data, const uint32_t dataSize,
                                   const uint32_t frameId, void* userData)
{
    Image image;

    return Decode(data, dataSize, frameId, image, userData);
}

APP_ERROR VideoDecoderDptr::Decode(const std::shared_ptr<uint8_t> data, const uint32_t dataSize,
                                   const uint32_t frameId, Image& preMallocData, void* userData)
{
    APP_ERROR ret = SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }

    if (memoryMode_ == MemoryMode::NONE_MALLOC_MODE) {
        memoryMode_ = (preMallocData.GetData() == nullptr) ? MemoryMode::SYS_MALLOC : MemoryMode::USER_MALLOC;
    } else if (memoryMode_ == MemoryMode::SYS_MALLOC && preMallocData.GetData() != nullptr) {
        LogError << "Please use decode function without pre-malloc data buffer."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    } else if (memoryMode_ == MemoryMode::USER_MALLOC && preMallocData.GetData() == nullptr) {
        LogError << " Please use the second decode function with pre-malloc data buffer."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (data == nullptr) {
        LogError << "Input data is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    vdecConfig_.userData = userData;

    // (2) Construct input image data.
    DvppDataInfo inputDataInfo;
    ret = ConstructDvppDataInfo(data, dataSize, frameId, inputDataInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    if (memoryMode_ == MemoryMode::USER_MALLOC) {
        inputDataInfo.outData = preMallocData.GetData().get();
        inputDataInfo.outDataSize = preMallocData.GetDataSize();
    }
    ret = dvppWrapper_->DvppVdec(inputDataInfo, this);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to dvppWrapper_ DvppVdec." << GetErrorInfo(ret);
        MemoryData dvppData((void *)inputDataInfo.data, (size_t)inputDataInfo.dataSize, MemoryData::MEMORY_DVPP,
                            deviceId_);
        MemoryHelper::MxbsFree(dvppData);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR VideoDecoderDptr::ConstructDvppDataInfo(const std::shared_ptr<uint8_t> data, const uint32_t dataSize,
                                                  const uint32_t frameId, DvppDataInfo& inputDataInfo)
{
    MemoryData hostMemory(static_cast<void*>(data.get()),
                          static_cast<size_t>(dataSize), MemoryData::MEMORY_HOST, deviceId_);
    MemoryData dvppMemory(nullptr, static_cast<size_t>(dataSize),
                          MemoryData::MEMORY_DVPP, deviceId_);

    if (runMode_ == ACL_DEVICE && DeviceManager::IsAscend310P()) {
        dvppMemory.type = MemoryData::MEMORY_HOST_MALLOC;
    }

    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(dvppMemory, hostMemory);
    if (ret != APP_ERR_OK) {
        ret = APP_ERR_ACL_BAD_COPY;
        LogError << "Fail to malloc and copy dvpp memory." << GetErrorInfo(ret);
        return ret;
    }

    inputDataInfo.width = vdecConfig_.width;
    inputDataInfo.height = vdecConfig_.height;
    inputDataInfo.widthStride = vdecConfig_.width;
    inputDataInfo.heightStride = vdecConfig_.height;
    inputDataInfo.device = deviceId_;
    inputDataInfo.format = static_cast<MxbasePixelFormat>(vDecodeConfig_.outputImageFormat);
    inputDataInfo.data = (uint8_t*)dvppMemory.ptrData;
    inputDataInfo.dataSize = dvppMemory.size;
    inputDataInfo.frameId = frameId;
    inputDataInfo.channelId = vdecConfig_.channelId;

    return APP_ERR_OK;
}

APP_ERROR VideoDecoderDptr::CheckVdecConfig()
{
    APP_ERROR ret = APP_ERR_OK;;
    if (DeviceManager::IsAscend310()) {
        ret = Check310(vDecodeConfig_, channelId_, Config310());
    } else if (DeviceManager::IsAscend310B()) {
        ret = Check310B(vDecodeConfig_, channelId_, Config310B());
    } else if (DeviceManager::IsAscend310P()) {
        ret = Check310P(vDecodeConfig_, channelId_, Config310P());
    } else if (DeviceManager::IsAtlas800IA2()) {
        ret = CheckAtlas800IA2(vDecodeConfig_, channelId_, ConfigAtlas800IA2());
    } else {
        LogError << "Could not recognize Ascend chip." << GetErrorInfo(ret);
        return APP_ERR_COMM_FULL;
    }
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return APP_ERR_OK;
}

void VideoDecoderDptr::GetVdecConfig()
{
    DecodeCallBackFunction callPtr = CallBack;
    vdecConfig_.deviceId = (uint32_t)deviceId_;
    vdecConfig_.videoChannel = channelId_;
    vdecConfig_.channelId = channelId_;
    vdecConfig_.callbackFunc = callPtr;
    vdecConfig_.skipInterval = vDecodeConfig_.skipInterval;
    vdecConfig_.inputVideoFormat = static_cast<MxbaseStreamFormat>(vDecodeConfig_.inputVideoFormat);
    vdecConfig_.outputImageFormat = static_cast<MxbasePixelFormat>(vDecodeConfig_.outputImageFormat);
    vdecConfig_.width = vDecodeConfig_.width;
    vdecConfig_.height = vDecodeConfig_.height;
    vdecConfig_.cscMatrix = vDecodeConfig_.cscMatrix;
    vdecConfig_.outMode = 0;
}
}
#endif
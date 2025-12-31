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
 * Description: Plugin for video encoding.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 * Attention: The video-encoder plugin can only be called by a single process, and only supports a single channel
 */

#include "MxPlugins/MxpiVideoEncoder/MxpiVideoEncoder.h"

#include <thread>
#include <chrono>

#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/DeviceManager/DeviceManager.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;
using namespace std;

namespace {
    const uint32_t MIN_HEIGHT_VALUE = 128;
    const uint32_t MIN_WIDTH_VALUE = 128;
    const uint32_t MAX_HEIGHT_VALUE_310 = 1920;
    const uint32_t MAX_WIDTH_VALUE_310 = 1920;
    const uint32_t MAX_HEIGHT_VALUE_310P = 4096;
    const uint32_t MAX_WIDTH_VALUE_310P = 4096;
    const uint32_t MAX_SIZE_VALUE_310P = 4096 * 2304;
    const uint32_t MAX_QUEUE_LENGHT = 1000;
    const uint32_t RC_MODE_VBR = 1;
    const uint32_t RC_MODE_CBR = 2;
    const uint32_t DEFAULT_SRC_RATE = 30;
    const uint32_t MIN_BITRATE = 10;
    const uint32_t DEFAULT_BITRATE = 30;
    const uint32_t IP_PROP_VBR = 80;
    const uint32_t IP_PROP_CBR = 70;
    const uint32_t MIN_TIMEOUT_VALUE = 0;
    const uint32_t MAX_TIMEOUT_VALUE = 10;
    const uint32_t DEFAULT_TIMEOUT_VALUE = 5;
}

MxpiPortInfo MxpiVideoEncoder::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = { { "ANY" } };
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiVideoEncoder::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = { { "ANY" } };
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

std::vector<std::shared_ptr<void>> MxpiVideoEncoder::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    std::shared_ptr<void> channelId = std::make_shared<ElementProperty<uint>>(
        ElementProperty<uint> { UINT, "vencChannelId", "channel id", "video encoder channel id", 0, 0, 127 });
    std::shared_ptr<void> iFrameInterval = std::make_shared<ElementProperty<uint>>(
        ElementProperty<uint> { UINT, "iFrameInterval", "interval", "I frame Interval time", 30, 1, 2000 });
    std::shared_ptr<void> imageHeight = std::make_shared<ElementProperty<uint>>(
        ElementProperty<uint> { UINT, "imageHeight", "height", "input image height", 1080, 128, 4096 });
    std::shared_ptr<void> imageWidth = std::make_shared<ElementProperty<uint>>(
        ElementProperty<uint> { UINT, "imageWidth", "width", "input image height", 1920, 128, 4096 });
    std::shared_ptr<void> fps = std::make_shared<ElementProperty<uint>>(
        ElementProperty<uint> { UINT, "fps", "print fps", "show the frames per second", 0, 0, 1 });
    std::shared_ptr<void> rcMode = std::make_shared<ElementProperty<uint>>(
        ElementProperty<uint> { UINT, "rcMode", "my rcMode", "print rcMode", 2, 0, 2 });
    std::shared_ptr<void> srcRate = std::make_shared<ElementProperty<uint>>(
        ElementProperty<uint> { UINT, "srcRate", "my srcRate", "print srcRate", 30, 1, 120 });
    std::shared_ptr<void> maxBitRate = std::make_shared<ElementProperty<uint>>(
        ElementProperty<uint> { UINT, "maxBitRate", "my maxBitRate", "print maxBitRate", 300, 10, 30000 });
    std::shared_ptr<void> ipProp = std::make_shared<ElementProperty<uint>>(
        ElementProperty<uint> { UINT, "ipProp", "my ipProp", "print ipProp", 70, 0, 100 });
    std::shared_ptr<void> inputFormat = std::make_shared<ElementProperty<string>>(ElementProperty<string> {
        STRING, "inputFormat", "input format", "input image format", "YUV420SP_NV12", "YUV420SP_NV12", "YUV420SP_NV21"
    });
    std::shared_ptr<void> outputFormat = std::make_shared<ElementProperty<string>>(ElementProperty<string> {
        STRING, "outputFormat", "output format", "output image format", "H264", "H264", "H265"
    });
    std::shared_ptr<void> frameReleaseTimeOut = std::make_shared<ElementProperty<uint>>(
            ElementProperty<uint>{UINT, "frameReleaseTimeOut", "frame release timeout", "frame release timeout",
                                  DEFAULT_TIMEOUT_VALUE, MIN_TIMEOUT_VALUE, MAX_TIMEOUT_VALUE});

    properties.push_back(std::move(channelId));
    properties.push_back(std::move(iFrameInterval));
    properties.push_back(std::move(imageHeight));
    properties.push_back(std::move(imageWidth));
    properties.push_back(std::move(fps));
    properties.push_back(std::move(inputFormat));
    properties.push_back(std::move(outputFormat));
    properties.push_back(std::move(rcMode));
    properties.push_back(std::move(srcRate));
    properties.push_back(std::move(maxBitRate));
    properties.push_back(std::move(ipProp));
    properties.push_back(std::move(frameReleaseTimeOut));
    return properties;
}

bool MxpiVideoEncoder::CheckParam()
{
    if (DeviceManager::IsAscend310() || DeviceManager::IsAscend310B()) {
        if (imageHeight_ < MIN_HEIGHT_VALUE || imageHeight_ > MAX_HEIGHT_VALUE_310) {
        LogError << "\"imageHeight\" is out of range. It must be between [128, 1920], but now it is " << imageHeight_
                 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
        }
        if (imageWidth_ < MIN_WIDTH_VALUE || imageWidth_ > MAX_WIDTH_VALUE_310) {
            LogError << "\"imageWidth\" is out of range. It must be between [128, 1920], but now it is " << imageWidth_
                     << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    } else if (DeviceManager::IsAscend310P()) {
        if (imageHeight_ < MIN_HEIGHT_VALUE || imageHeight_ > MAX_HEIGHT_VALUE_310P) {
        LogError << "\"imageHeight\" is out of range. It must be between [128, 4096], but now it is " << imageHeight_
                 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
        }
        if (imageWidth_ < MIN_WIDTH_VALUE || imageWidth_ > MAX_WIDTH_VALUE_310P) {
            LogError << "\"imageWidth\" is out of range. It must be between [128, 4096], but now it is " << imageWidth_
                     << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (outputFormat_ == "H264" && imageHeight_ * imageWidth_ > MAX_SIZE_VALUE_310P) {
            LogError << "On Ascend310P, h264 video max image size must below 4096x2304."
                     << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    } else {
        LogError << "Unsupported device." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    if (enableFpsMode_ != 0 && enableFpsMode_ != 1) {
        LogError << "\"fps\" is out of range. It must be 0 or 1, but now it is " << enableFpsMode_
                 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (inputFormat_ != "YUV420SP_NV12" && inputFormat_ != "YUV420SP_NV21") {
        LogError << "\"inputFormat\" is out of range. It must be \"YUV420SP_NV12\" or \"YUV420SP_NV21\"."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (outputFormat_ != "H264" && outputFormat_ != "H265") {
        LogError << "\"inputFormat\" is out of range. It must be \"H264\" or \"H265\"."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (releaseTimeout_ > MAX_TIMEOUT_VALUE) {
        LogError << "\"releaseTimeOut\" is out of range. It must be between [" << MIN_TIMEOUT_VALUE << ","
                 << MAX_TIMEOUT_VALUE << "], but now it is " << releaseTimeout_
                 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

APP_ERROR MxpiVideoEncoder::GetChannelDescParam(std::map<std::string, std::shared_ptr<void>> &configParamMap,
                                                MxBase::VencConfig &vencConfig)
{
    std::vector<std::string> parameterNamesPtr = {"iFrameInterval", "imageHeight", "imageWidth", "frameReleaseTimeOut",
                                                  "fps", "inputFormat", "outputFormat", "vencChannelId", "rcMode",
                                                  "srcRate", "maxBitRate", "ipProp"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    keyFrameInterval_ = *std::static_pointer_cast<uint>(configParamMap["iFrameInterval"]);
    imageHeight_ = *std::static_pointer_cast<uint>(configParamMap["imageHeight"]);
    imageWidth_ = *std::static_pointer_cast<uint>(configParamMap["imageWidth"]);
    releaseTimeout_ = *std::static_pointer_cast<uint>(configParamMap["frameReleaseTimeOut"]);
    enableFpsMode_ = *std::static_pointer_cast<uint>(configParamMap["fps"]);
    parentName_ = dataSource_;
    parentName_ = MxPluginsAutoDataSource(elementName_, 0, "dataSource", parentName_, dataSourceKeys_);
    inputFormat_ = *std::static_pointer_cast<string>(configParamMap["inputFormat"]);
    outputFormat_ = *std::static_pointer_cast<string>(configParamMap["outputFormat"]);
    channelId_ = *std::static_pointer_cast<uint>(configParamMap["vencChannelId"]);
    vencConfig.rcMode = *std::static_pointer_cast<uint>(configParamMap["rcMode"]);
    vencConfig.rcMode = (vencConfig.rcMode == 0) ? RC_MODE_CBR : vencConfig.rcMode;
    vencConfig.srcRate = *std::static_pointer_cast<uint>(configParamMap["srcRate"]);
    vencConfig.srcRate = (vencConfig.srcRate == 0) ? DEFAULT_SRC_RATE : vencConfig.srcRate;
    vencConfig.maxBitRate = *std::static_pointer_cast<uint>(configParamMap["maxBitRate"]);
    vencConfig.maxBitRate = (vencConfig.maxBitRate < MIN_BITRATE) ? DEFAULT_BITRATE : vencConfig.maxBitRate;
    vencConfig.ipProp = *std::static_pointer_cast<uint>(configParamMap["ipProp"]);
    if (vencConfig.ipProp == 0) {
        vencConfig.ipProp = (vencConfig.rcMode == RC_MODE_VBR) ? IP_PROP_VBR : IP_PROP_CBR;
    }
    return APP_ERR_OK;
}

void MxpiVideoEncoder::VencConfigInit(MxBase::VencConfig &vencConfig)
{
    vencConfig.width = imageWidth_;
    vencConfig.height = imageHeight_;
    vencConfig.outputVideoFormat =
    (outputFormat_ == "H264") ? MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL : MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL;
    vencConfig.inputImageFormat = (inputFormat_ == "YUV420SP_NV12") ? MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420 :
                                  MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vencConfig.keyFrameInterval = keyFrameInterval_;
    vencConfig.deviceId = static_cast<unsigned int>(deviceId_);
    vencConfig.channelId = channelId_;
    vencConfig.stopEncoderThread = false;
    vencConfig.userData = nullptr;
    vencConfig.rcMode = (vencConfig.rcMode == 0) ? RC_MODE_CBR : RC_MODE_VBR;

    callbackHandle_ = [this] (std::shared_ptr<uint8_t> data, uint32_t streamSize) {
        if (data.get() == nullptr) {
            LogError << "Data is invalid." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return;
        }
        if (streamSize == 0) {
            LogError << "Data size is equal to 0." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return;
        }
        handleFrameId_++;
        auto item = std::make_tuple(data, handleFrameId_, streamSize);
        queue_->Push(item, true);
    };
    vencConfig.userData = &callbackHandle_;

    callbackHandleV2_ = [this] (std::shared_ptr<uint8_t> data, uint32_t streamSize, void**) {
        callbackHandle_(data, streamSize);
    };

    callbackHandleV2For310P_ = [this] (std::shared_ptr<uint8_t> data, uint32_t streamSize, void**,
        void*) {
        callbackHandle_(data, streamSize);
    };

    if (DeviceManager::IsAscend310() || DeviceManager::IsAscend310B()) {
        vencConfig.userDataWithInput = &callbackHandleV2_;
    } else if (DeviceManager::IsAscend310P()) {
        vencConfig.userDataWithInputFor310P = &callbackHandleV2For310P_;
    }
}

APP_ERROR MxpiVideoEncoder::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiVideoEncoder(" << elementName_ << ").";
    VencConfig vencConfig = {};
    APP_ERROR ret = GetChannelDescParam(configParamMap, vencConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Get channel description failed." << GetErrorInfo(ret);
        return ret;
    }
    if (parentName_.empty()) {
        LogError << "Property dataSource is \"\", please check element(" << elementName_ << ")'s previous element."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (!CheckParam()) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // start calculate fps
    if (enableFpsMode_ == 1) {
        threadFps_ = std::unique_ptr<std::thread>(new(std::nothrow) std::thread(&MxpiVideoEncoder::CalFps, this));
        if (threadFps_ == nullptr) {
            LogError << "Memory alloc failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    // venc init
    VencConfigInit(vencConfig);
    dvppWrapper_ = MemoryHelper::MakeShared<DvppWrapper>();
    if (dvppWrapper_ == nullptr) {
        LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }

    ret = dvppWrapper_->InitVenc(vencConfig);
    if (ret != APP_ERR_OK) {
        LogError << "MxpiVideoEncoder DvppWrapper Init failed." << GetErrorInfo(ret);
        return ret;
    }
    using BlockingQueueType = BlockingQueue<std::tuple<std::shared_ptr<uint8_t>, uint32_t, uint32_t>>;
    queue_ = MemoryHelper::MakeShared<BlockingQueueType>(MAX_QUEUE_LENGHT);
    if (queue_ == nullptr) {
        LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    threadSend_ = std::unique_ptr<std::thread>(new(std::nothrow) std::thread(&MxpiVideoEncoder::SendDataThread, this));
    if (threadSend_ == nullptr) {
        LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }

    LogInfo << "End to initialize MxpiVideoEncoder(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiVideoEncoder::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiVideoEncoder(" << elementName_ << ").";
    // venc deinit

    APP_ERROR ret = dvppWrapper_->DeInitVenc();
    if (ret != APP_ERR_OK) {
        LogError << "MxpiVideoEncoder DvppWrapper DeInit failed." << GetErrorInfo(ret);
        return ret;
    }

    if (enableFpsMode_) {
        fpsMutex_.lock();
        fpsStopFlag_ = true;
        fpsMutex_.unlock();
        fpsCv_.notify_one();
        if (threadFps_->joinable()) {
            threadFps_->join();
        }
        threadFps_.reset();
    }

    sendStopFlag_ = true;
    if (threadSend_->joinable()) {
        threadSend_->join();
    }
    threadSend_.reset();
    queue_->Stop();
    queue_->Clear();

    callbackHandle_ = {};

    LogInfo << "End to deinitialize MxpiVideoEncoder(" << elementName_ << ").";
    return APP_ERR_OK;
}

static APP_ERROR CreateDvppDataInfo(std::shared_ptr<MxpiVisionList> mxpiVisionList, uint64_t frameId,
    DvppDataInfo &dvppDataInfo)
{
    if (mxpiVisionList->visionvec_size() != 1) {
        LogError << "The vision size is " << mxpiVisionList->visionvec_size() << ", not equal to 1."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    auto srcMxpiVision = mxpiVisionList->visionvec(0);
    dvppDataInfo.width = srcMxpiVision.visioninfo().width();
    dvppDataInfo.height = srcMxpiVision.visioninfo().height();
    dvppDataInfo.widthStride = srcMxpiVision.visioninfo().widthaligned();
    dvppDataInfo.heightStride = srcMxpiVision.visioninfo().heightaligned();
    dvppDataInfo.format = static_cast<MxbasePixelFormat>(srcMxpiVision.visioninfo().format());
    dvppDataInfo.data = (uint8_t *)srcMxpiVision.visiondata().dataptr();
    dvppDataInfo.dataSize = static_cast<unsigned int>(srcMxpiVision.visiondata().datasize());
    dvppDataInfo.frameId = frameId;
    return APP_ERR_OK;
}

APP_ERROR MxpiVideoEncoder::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiVideoEncoder(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxpiBuffer *inputMxpiBuffer = mxpiBuffer[0];
    MxpiMetadataManager mxpiMetadataManager(*inputMxpiBuffer);
    auto metadata = mxpiMetadataManager.GetMetadata(parentName_);
    if (metadata == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << "metadata is null.";
        SendData(0, *inputMxpiBuffer);
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL; // self define the error code
    }
    // check target data structure
    auto res = mxpiMetadataManager.GetMetadataWithType(parentName_, "MxpiVisionList");
    if (res == nullptr) {
        LogError << "Not a MxpiVisionList object." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        SendMxpiErrorInfo(*inputMxpiBuffer, elementName_, APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH,
                          "Not a MxpiVisionList object.");
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    auto mxpiVisionList = std::static_pointer_cast<MxpiVisionList>(metadata);
    // frameId need to be consistent with handleFrameId_ which is push in queue in callbackHandleV2_
    uint64_t frameId = ++sendFrameId_;
    DvppDataInfo inputDataInfo = {};
    ret = CreateDvppDataInfo(mxpiVisionList, frameId, inputDataInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Create DvppDataInfo fail." << GetErrorInfo(APP_ERR_DVPP_H26X_ENCODE_FAIL);
        SendMxpiErrorInfo(*inputMxpiBuffer, elementName_, APP_ERR_DVPP_H26X_ENCODE_FAIL, "Create DvppDataInfo fail");
        return APP_ERR_DVPP_H26X_ENCODE_FAIL;
    }
    // add cache before callbackHandleV2_
    struct BufferCache cache = { inputMxpiBuffer, time(NULL) };
    cacheMutex_.lock();
    processCache_.insert(std::pair<uint64_t, BufferCache>(frameId, cache));
    cacheMutex_.unlock();
    ret = dvppWrapper_->DvppVenc(inputDataInfo, &callbackHandleV2_);
    if (ret != APP_ERR_OK) {
        LogError << "Encode fail." << GetErrorInfo(APP_ERR_DVPP_H26X_ENCODE_FAIL);
        SendMxpiErrorInfo(*inputMxpiBuffer, elementName_, APP_ERR_DVPP_H26X_ENCODE_FAIL, "Encode fail");
        return APP_ERR_DVPP_H26X_ENCODE_FAIL;
    }
    LogDebug << "End to process MxpiVideoEncoder(" << elementName_ << ").";
    return APP_ERR_OK;
}

void MxpiVideoEncoder::CalFps()
{
    std::unique_lock<std::mutex> lck(fpsMutex_);
    while (!fpsStopFlag_) {
        fpsCv_.wait_for(lck, std::chrono::seconds(1));
        LogInfo << "Plugin(" << elementName_ << ") fps (" << fpsCount_ << ").";
        fpsCount_ = 0;
    }
}

void MxpiVideoEncoder::DeleteCacheByFrameId(uint64_t frameId)
{
    time_t now = time(NULL);
    std::unordered_map<uint64_t, BufferCache>::iterator it = processCache_.find(frameId);
    for (it = processCache_.begin(); it != processCache_.end();) {
        if (it->first == frameId || static_cast<uint32_t>(now - it->second.addTime) > releaseTimeout_) {
            MxpiBufferManager::DestroyBuffer(it->second.buffer);
            processCache_.erase(it++);
        } else {
            ++it;
        }
    }
}

void MxpiVideoEncoder::SendDataThread()
{
    const uint32_t tenMillisecond = 10;
    while (!sendStopFlag_) {
        std::tuple<std::shared_ptr<uint8_t>, uint32_t, uint32_t> item = {};
        APP_ERROR ret = queue_->Pop(item, tenMillisecond);
        if (ret != APP_ERR_OK) {
            continue;
        }
        const uint32_t streamDataIndex = 0;
        const uint32_t frameIdIndex = 1;
        const uint32_t streamSizeIndex = 2;
        auto streamData = std::get<streamDataIndex>(item);
        auto frameId = std::get<frameIdIndex>(item);
        auto streamSize = std::get<streamSizeIndex>(item);
        InputParam inputParam = {};
        LogDebug << "Plugin(" << elementName_ << ") encode frameId(" << frameId
                 << ") stream size(" << streamSize << ").";
        if (enableFpsMode_) {
            std::unique_lock<std::mutex> lck(fpsMutex_);
            fpsCount_++;
        }
        inputParam.deviceId = deviceId_;
        inputParam.ptrData = (void *)streamData.get();
        inputParam.dataSize = static_cast<int>(streamSize);
        inputParam.mxpiFrameInfo.set_channelid(channelId_);
        inputParam.mxpiFrameInfo.set_frameid(frameId);
        inputParam.mxpiVisionInfo.set_width(imageWidth_);
        inputParam.mxpiVisionInfo.set_height(imageHeight_);
        inputParam.key = elementName_;

        cacheMutex_.lock();
        DeleteCacheByFrameId(frameId);
        cacheMutex_.unlock();

        auto *mxpiBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
        SendData(0, *mxpiBuffer);
    }
    return;
}

namespace {
    MX_PLUGIN_GENERATE(MxpiVideoEncoder)
}
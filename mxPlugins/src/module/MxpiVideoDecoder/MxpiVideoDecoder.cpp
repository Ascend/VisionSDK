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
 * Description: Plugin for video decoding.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"
#include "MxPlugins/MxpiVideoDecoder/MxpiVideoDecoder.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const std::string FORMAT_H264 = "H264";
const std::string FORMAT_H265 = "H265";
const std::string FORMAT_YUV420SP_NV12 = "YUV420SP_NV12";
const std::string FORMAT_YUV420SP_NV21 = "YUV420SP_NV21";
const std::string FORMAT_RGB_888 = "RGB_888";
const std::string FORMAT_BGR_888 = "BGR_888";
const size_t CHANNEL_ID_310_MAX = 31;
const size_t CHANNEL_ID_310P_MAX = 255;
}

APP_ERROR MxpiVideoDecoder::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiVideoDecoder(" << elementName_ << ").";
    // get parameters from website.
    std::vector<std::string> parameterNamesPtr = {"inputVideoFormat", "outputImageFormat", "vdecChannelId", "outMode",
                                                  "outPicHeightMax", "outPicWidthMax", "skipFrame", "vdecResizeWidth",
                                                  "vdecResizeHeight"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    paramVideoFormat_ = *std::static_pointer_cast<std::string>(configParamMap["inputVideoFormat"]);
    LogInfo << "element(" << elementName_ << ") property inputVideoFormat(" << paramVideoFormat_ << ").";
    paramImageFormat_ = *std::static_pointer_cast<std::string>(configParamMap["outputImageFormat"]);
    LogInfo << "element(" << elementName_ << ") property outputImageFormat(" << paramImageFormat_ << ").";
    paramVdecChannelId_ = *std::static_pointer_cast<uint>(configParamMap["vdecChannelId"]);
    LogInfo << "element(" << elementName_ << ") property vdecChannelId(" << paramVdecChannelId_ << ").";
    outMode_ = *std::static_pointer_cast<uint>(configParamMap["outMode"]);
    LogInfo << "element(" << elementName_ << ") property outMode(" << outMode_ << ").";
    outPicHeightMax_ = *std::static_pointer_cast<uint>(configParamMap["outPicHeightMax"]);
    LogInfo << "element(" << elementName_ << ") property outPicHeightMax(" << outPicHeightMax_ << ").";
    outPicWidthMax_ = *std::static_pointer_cast<uint>(configParamMap["outPicWidthMax"]);
    LogInfo << "element(" << elementName_ << ") property outPicWidthMax(" << outPicWidthMax_ << ").";
    skipFrame_ = *std::static_pointer_cast<uint>(configParamMap["skipFrame"]);
    LogInfo << "element(" << elementName_ << ") property skipFrame_(" << skipFrame_ << ").";
    vdecResizeWidth_ = *std::static_pointer_cast<uint>(configParamMap["vdecResizeWidth"]);
    LogInfo << "element(" << elementName_ << ") property skipFrame_(" << vdecResizeWidth_ << ").";
    vdecResizeHeight_ = *std::static_pointer_cast<uint>(configParamMap["vdecResizeHeight"]);
    LogInfo << "element(" << elementName_ << ") property skipFrame_(" << vdecResizeHeight_ << ").";
    // dvpp initialization.
    dvppWrapper_ = MemoryHelper::MakeShared<DvppWrapper>();
    if (dvppWrapper_ == nullptr) {
        LogError << "Failed to create dvppWrapper_ object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    // prepare vdecConfig
    ret = PrepareVdecConfig();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to prepare vdec config." << GetErrorInfo(ret);
        return ret;
    }
    PerformanceStatisticsManager::GetInstance()->VideoDecodeStatisticsRegister(streamName_, elementName_);
    LogInfo << "End to initialize MxpiVideoDecoder(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiVideoDecoder::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiVideoDecoder(" << elementName_ << ").";
    if (isInitVdec_) {
        // vdec deinit
        APP_ERROR ret = dvppWrapper_->DeInitVdec();
        if (ret != APP_ERR_OK) {
            LogError << "Failed to deinitialize dvppWrapper_." << GetErrorInfo(ret);
            return ret;
        }
        dvppWrapper_.reset();
    }
    LogInfo << "End to deinitialize MxpiVideoDecoder(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiVideoDecoder::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiVideoDecoder(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxpiBuffer *inputMxpiBuffer = mxpiBuffer[0];
    errorInfo_.str("");
    MxpiFrame inputMxpiFrame = MxpiBufferManager::GetHostDataInfo(*inputMxpiBuffer);
    if (inputMxpiFrame.visionlist().visionvec_size() == 0) {
        errorInfo_ << "Empty vision in frame. use SendData rather than SendProtobuf."
                   << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*inputMxpiBuffer, elementName_, ret, errorInfo_.str());
        return APP_ERR_COMM_FAILURE;
    }
    // prepare input data information
    DvppDataInfo inputDataInfo;
    ret = PrepareInputDataInfo(inputMxpiFrame, inputDataInfo);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*inputMxpiBuffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    if (isInitVdec_ == false) {
        auto format = (StreamFormat)inputMxpiFrame.visionlist().visionvec(0).visioninfo().format();
        if ((ret = SetDvppVideoFormat(format)) != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            SendMxpiErrorInfo(*inputMxpiBuffer, elementName_, ret, errorInfo_.str());
            return ret;
        }
        // vdec init
        ret = dvppWrapper_->InitVdec(vdecConfig_);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Failed to initialize dvppWrapper_ object.";
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            SendMxpiErrorInfo(*inputMxpiBuffer, elementName_, ret, errorInfo_.str());
            return ret;
        }
        isInitVdec_ = true;
    }
    PerformanceStatisticsManager::GetInstance()->VideoDecodeStatisticsSetStartTime(streamName_, elementName_);
    // decode video
    ret = DvppVideoDecode(inputDataInfo, (void*)this);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*inputMxpiBuffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    MxpiBufferManager::DestroyBuffer(inputMxpiBuffer);
    LogDebug << "End to process MxpiVideoDecoder(" << elementName_ << ").";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiVideoDecoder::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto inputVideoFormat = (std::make_shared<ElementProperty<std::string>>)(ElementProperty<std::string> {
        STRING,
        "inputVideoFormat",
        "format",
        "the format of input video before decoding, only support H264, H265",
        "H264",
        "NULL",
        "NULL"
    });
    auto outputImageFormat = (std::make_shared<ElementProperty<std::string>>)(ElementProperty<std::string> {
        STRING,
        "outputImageFormat",
        "format",
        "the format of output image after decoding, only support YUV420SP_NV12, YUV420SP_NV21",
        "YUV420SP_NV12",
        "NULL",
        "NULL"
    });
    auto vdecChannelId = (std::make_shared<ElementProperty<uint>>)(ElementProperty<uint> {
        UINT,
        "vdecChannelId",
        "channelId",
        "the channel id for vdec channel description",
        0, 0, 255
    });
    auto outMode = (std::make_shared<ElementProperty<uint>>)(ElementProperty<uint> {
            UINT,
            "outMode",
            "out mode",
            "the outMode for callback, 0: delay mode -> default mode, 1: real time mode",
            0, 0, 1
    });

    auto outPicWidthMax = (std::make_shared<ElementProperty<uint>>)(
        ElementProperty<uint> { UINT, "outPicWidthMax", "outPicWidthMax", "output picture max width", 0, 0, 4096 });

    auto outPicHeightMax = (std::make_shared<ElementProperty<uint>>)(
        ElementProperty<uint> { UINT, "outPicHeightMax", "outPicHeightMax", "output picture max height", 0, 0, 4096 });

    auto skipFrame = (std::make_shared<ElementProperty<uint>>)(
        ElementProperty<uint> { UINT, "skipFrame", "skipFrame", "skip frame value of video decode", 0, 0, 100 });
    auto vdecResizeWidth = (std::make_shared<ElementProperty<uint>>)(
        ElementProperty<uint> { UINT, "vdecResizeWidth", "vdecResizeWidth",
                                "output picture width after resize", 0, 0, 4096 });
    auto vdecResizeHeight = (std::make_shared<ElementProperty<uint>>)(
        ElementProperty<uint> { UINT, "vdecResizeHeight", "vdecResizeHeight",
                                "output picture height after resize", 0, 0, 4096 });

    properties = { inputVideoFormat, outputImageFormat, vdecChannelId, outMode, outPicWidthMax, outPicHeightMax,
        skipFrame, vdecResizeWidth, vdecResizeHeight };

    return properties;
}

MxpiPortInfo MxpiVideoDecoder::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"video/x-h264"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxpiPortInfo MxpiVideoDecoder::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/yuv"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

APP_ERROR MxpiVideoDecoder::SetDvppVideoFormat(StreamFormat format)
{
    // use transmission stream format first, such as rtspsrc.
    if (format == StreamFormat::H264) {
        vdecConfig_.inputVideoFormat = MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL;
    } else if (format == StreamFormat::H265) {
        vdecConfig_.inputVideoFormat = MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL;
    } else {
        // check input video format
        if (paramVideoFormat_.compare(FORMAT_H264) == 0) {
            vdecConfig_.inputVideoFormat = MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL;
        } else if (paramVideoFormat_.compare(FORMAT_H265) == 0) {
            vdecConfig_.inputVideoFormat = MXBASE_STREAM_FORMAT_H265_MAIN_LEVEL;
        } else {
            errorInfo_ << "Input video format for video decoding, only support H264 or H265. "
                       << "Current format is " << paramVideoFormat_ << "."
                       << GetErrorInfo(APP_ERR_DVPP_H26X_DECODE_FAIL);
            return APP_ERR_DVPP_H26X_DECODE_FAIL;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiVideoDecoder::PrepareVdecConfig()
{
    // check output image format
    if (paramImageFormat_.compare(FORMAT_YUV420SP_NV12) == 0) {
        vdecConfig_.outputImageFormat = MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    } else if (paramImageFormat_.compare(FORMAT_YUV420SP_NV21) == 0) {
        vdecConfig_.outputImageFormat = MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    } else if (paramImageFormat_.compare(FORMAT_RGB_888) == 0) {
        vdecConfig_.outputImageFormat = MXBASE_PIXEL_FORMAT_RGB_888;
    } else if (paramImageFormat_.compare(FORMAT_BGR_888) == 0) {
        vdecConfig_.outputImageFormat = MXBASE_PIXEL_FORMAT_BGR_888;
    } else {
        LogError << "Output image format is invalid." << "Current format is " << paramImageFormat_ << "."
                 << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
        return APP_ERR_DVPP_INVALID_FORMAT;
    }

    if (DeviceManager::IsAscend310() || DeviceManager::IsAscend310B()) {
        if (paramVdecChannelId_ > CHANNEL_ID_310_MAX) {
            LogError << "Vdec channel id is out of range," << " current channel id is " << paramVdecChannelId_ << "."
                     << GetErrorInfo(APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE);
            return APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE;
        }
    } else if (DeviceManager::IsAscend310P()) {
        if (paramVdecChannelId_ > CHANNEL_ID_310P_MAX) {
            LogError << "Vdec channel id is out of range," << " current channel id is " << paramVdecChannelId_ << "."
                     << GetErrorInfo(APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE);
            return APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE;
        }
    }

    vdecConfig_.deviceId = static_cast<unsigned int>(deviceId_);
    vdecConfig_.channelId = paramVdecChannelId_;
    DecodeCallBackFunction callPtr = CallBack;
    vdecConfig_.callbackFunc = callPtr;
    vdecConfig_.outMode = outMode_;
    vdecConfig_.width = outPicWidthMax_;
    vdecConfig_.height = outPicHeightMax_;
    vdecConfig_.skipInterval = skipFrame_;
    return APP_ERR_OK;
}

APP_ERROR MxpiVideoDecoder::PrepareInputDataInfo(MxpiFrame &inputMxpiFrame, DvppDataInfo &inputDataInfo)
{
    // get video related information(channelId, frameId, width, height, dataSize, data)
    inputDataInfo.channelId = inputMxpiFrame.frameinfo().channelid();
    inputDataInfo.frameId = inputMxpiFrame.frameinfo().frameid();
    inputDataInfo.width = inputMxpiFrame.visionlist().visionvec(0).visioninfo().width();
    inputDataInfo.height = inputMxpiFrame.visionlist().visionvec(0).visioninfo().height();
    inputDataInfo.dataSize =
        static_cast<unsigned int>(inputMxpiFrame.visionlist().visionvec(0).visiondata().datasize());
    inputDataInfo.data = (uint8_t *)inputMxpiFrame.visionlist().visionvec(0).visiondata().dataptr();

    auto format = (StreamFormat)inputMxpiFrame.visionlist().visionvec(0).visioninfo().format();
    LogDebug << "element(" << elementName_ << "): " << "stage(sendframe),"
             << "channelId(" << inputDataInfo.channelId << "),"
             << "frameId(" << inputDataInfo.frameId << "),"
             << "dataSize(" << inputDataInfo.dataSize << "),"
             << "format(" << format << "),"
             << "width(" << inputDataInfo.width << "),"
             << "height(" << inputDataInfo.height << ").";

    if (format != StreamFormat::H264 && format != StreamFormat::H265) {
        if (paramVideoFormat_.compare(FORMAT_H264) != 0 && paramVideoFormat_.compare(FORMAT_H265) != 0) {
            errorInfo_ << "Input video format for video decoding, only support H264 or H265. "
                       << "Current format is " << paramVideoFormat_ << "."
                       << GetErrorInfo(APP_ERR_DVPP_H26X_DECODE_FAIL);
            return APP_ERR_DVPP_H26X_DECODE_FAIL;
        }
    }

    // video width and height verification.
    if ((inputDataInfo.width < MIN_VDEC_WIDTH) || (inputDataInfo.width > MAX_VDEC_WIDTH)) {
        errorInfo_ << "The width of video is out of range [" << MIN_VDEC_WIDTH << "," << MAX_VDEC_WIDTH
                   << "], actual width is " << inputDataInfo.width << "." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    if ((inputDataInfo.height < MIN_VDEC_HEIGHT) || (inputDataInfo.height > MAX_VDEC_HEIGHT)) {
        errorInfo_ << "The height of video is out of range [" << MIN_VDEC_HEIGHT << "," << MAX_VDEC_HEIGHT
                   << "], actual height is " << inputDataInfo.height << "." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiVideoDecoder::DvppVideoDecode(DvppDataInfo& inputDataInfo, void* userData)
{
    // copy data from host to device
    MemoryData hostMemory((void *)inputDataInfo.data, (size_t)inputDataInfo.dataSize,
                          MemoryData::MEMORY_HOST, deviceId_);
    MemoryData dvppMemory(nullptr, (size_t)inputDataInfo.dataSize, MemoryData::MEMORY_DVPP, deviceId_);

    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(dvppMemory, hostMemory);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Malloc and copy data from host to dvpp failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }

    inputDataInfo.dataSize = dvppMemory.size;
    inputDataInfo.data = (uint8_t *)dvppMemory.ptrData;
    if (DeviceManager::IsAscend310P()) {
        inputDataInfo.resizeWidth = vdecResizeWidth_;
        inputDataInfo.resizeHeight = vdecResizeHeight_;
    }

    // video decode
    ret = dvppWrapper_->DvppVdec(inputDataInfo, userData);
    if (ret != APP_ERR_OK) {
        MemoryData dvppData((void *)inputDataInfo.data, (size_t)inputDataInfo.dataSize, MemoryData::MEMORY_DVPP,
            deviceId_);
        MemoryHelper::MxbsFree(dvppData);
        errorInfo_ << "Decode video failed." << GetErrorInfo(APP_ERR_DVPP_H26X_DECODE_FAIL);
        return APP_ERR_DVPP_H26X_DECODE_FAIL;
    }

    return APP_ERR_OK;
}

APP_ERROR MxpiVideoDecoder::CallBack(std::shared_ptr<void> buffer, DvppDataInfo &dvppDataInfo, void *userData)
{
    InputParam bufferParam;
    MxpiVisionInfo *outputVisionInfo = &(bufferParam.mxpiVisionInfo);
    MxpiFrameInfo *outputFrameInfo = &(bufferParam.mxpiFrameInfo);
    outputVisionInfo->set_format(dvppDataInfo.format);
    outputVisionInfo->set_width(dvppDataInfo.width);
    outputVisionInfo->set_height(dvppDataInfo.height);
    outputVisionInfo->set_widthaligned(dvppDataInfo.widthStride);
    outputVisionInfo->set_heightaligned(dvppDataInfo.heightStride);
    outputFrameInfo->set_frameid(dvppDataInfo.frameId);
    outputFrameInfo->set_channelid(dvppDataInfo.channelId);
    bufferParam.key = ((MxpiVideoDecoder*)userData)->elementName_;
    bufferParam.deviceId = ((MxpiVideoDecoder*)userData)->deviceId_;
    bufferParam.dataSize = static_cast<int>(dvppDataInfo.dataSize);
    bufferParam.ptrData = buffer.get();
    bufferParam.mxpiMemoryType = MXPI_MEMORY_DVPP;
    MxpiBuffer* outputMxpiBuffer = MxpiBufferManager::CreateDeviceBufferWithMemory(bufferParam);
    if (outputMxpiBuffer == nullptr) {
        MemoryData dvppData((void *)bufferParam.ptrData, (size_t)bufferParam.dataSize, MemoryData::MEMORY_DVPP,
                bufferParam.deviceId);
        MemoryHelper::MxbsFree(dvppData);
        LogError << "Create device buffer and copy data failed." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }

    LogDebug << "element(" << ((MxpiVideoDecoder*)userData)->elementName_ << "): " << "stage(callback),"
             << "channelId(" << dvppDataInfo.channelId << "),"
             << "frameId(" << dvppDataInfo.frameId << "),"
             << "dataSize(" << dvppDataInfo.dataSize << "),"
             << "width(" << dvppDataInfo.width << "),"
             << "height(" << dvppDataInfo.height << "),"
             << "widthStride(" << dvppDataInfo.widthStride << "),"
             << "heightStride(" << dvppDataInfo.heightStride << ").";
    PerformanceStatisticsManager::GetInstance()->VideoDecodeStatisticsSetEndTime(
        ((MxpiVideoDecoder*)userData)->streamName_, ((MxpiVideoDecoder*)userData)->elementName_);
    ((MxpiVideoDecoder*)userData)->SendData(0, *outputMxpiBuffer);
    return APP_ERR_OK;
}

namespace {
    MX_PLUGIN_GENERATE(MxpiVideoDecoder)
}
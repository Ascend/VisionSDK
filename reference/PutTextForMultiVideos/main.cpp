/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Put text for 1080P videos.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include <sys/time.h>

#include <algorithm>
#include <chrono>
#include <csignal>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <sstream>
#include <thread>

#include "BlockingQueue.h"
#include "ConfigParser/ConfigParser.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/E2eInfer/VideoDecoder/VideoDecoder.h"
#include "MxBase/E2eInfer/VideoEncoder/VideoEncoder.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MxBase.h"
#include "PutText/CaptionImpl.h"
#include "acl/acl.h"
#include "acl/acl_rt.h"
#include "unistd.h"
extern "C"
{
#include <libavformat/avformat.h>
}

namespace
{
using namespace std::chrono;
using namespace std;
using namespace MxBase;
bool g_saveVIDEO = true;
static bool g_signalReceived = false;
const int QUEUE_SIZE = 1000;
const float YUV420_RATIO = 1.5;
const float DEFAULT_OPACITY = 0.3;
const int TIME_OUT = 3000;
const int INIT_INTERVAL = 100;
const int DEFAULT_SRC_RATE = 25;
const int DEFAULT_MAX_BIT_RATE = 30000;
const uint32_t CHANNEL_COUNT = 25;
const int YEAR_OFFSET = 1900;
const int MONTH_OFFSET = 1;
const int FULL_HD_WIDTH = 1920;
const int FULL_HD_HEIGHT = 1080;
const int CIF_WIDTH = 352;
const int CIF_HEIGHT = 288;
const int SIGNAL_CHECK_TIMESTEP = 1000;
const std::string CONFIG_FILE_NAME = "../setup.config";
const MxBase::Size CIF_IMAGE_SIZE(CIF_WIDTH, CIF_HEIGHT);
const MxBase::Size RESIZE_MIDDLE_IMAGE_SIZE(960, 540);
const MxBase::Point leftTop{60, 60};
const MxBase::Point rightTop{1400, 60};
const MxBase::Point leftButtom{60, 850};

struct DecodedFrame
{
    MxBase::Image image;
    uint32_t frameId = 0;
    uint32_t channelId = 0;
};

struct EncodedFrame
{
    std::shared_ptr<uint8_t> data;
    uint32_t dataSize = 0;
    uint32_t frameId = 0;
    uint32_t channelId = 0;
};

struct ThreadPools
{
    std::map<int, std::vector<std::thread>> threadStreamPullerPool;
    std::map<int, std::vector<std::thread>> threadVdecPool;
    std::map<int, std::vector<std::thread>> threadFrameProcessPool;
    std::map<int, std::vector<std::thread>> threadVenc1080PPool;
    std::map<int, std::vector<std::thread>> threadVencCIFPool;
    std::map<int, std::vector<std::thread>> threadCIFSavePool;
    std::map<int, std::vector<std::thread>> thread1080PSavePool;
};

std::map<int, std::vector<std::shared_ptr<MxBase::ImageProcessor>>> g_imageProcessorMap = {};
std::map<int, std::vector<std::shared_ptr<BlockingQueue<EncodedFrame>>>> g_pullerToVdecQueueMap = {};
std::map<int, std::vector<std::shared_ptr<BlockingQueue<DecodedFrame>>>> g_vdecToCaptionQueueMap = {};
std::map<std::string, std::map<int, std::vector<std::shared_ptr<BlockingQueue<DecodedFrame>>>>>
    g_captionToVencQueueMap = {};
std::map<std::string, std::map<int, std::vector<std::shared_ptr<BlockingQueue<EncodedFrame>>>>> g_vencToFileQueueMap =
    {};
std::map<int, std::vector<std::shared_ptr<CaptionImpl>>> g_captionImplVecForTimeMap = {};
std::map<int, std::vector<std::shared_ptr<CaptionImpl>>> g_captionImplVecForTextMap = {};
std::map<int, std::vector<std::shared_ptr<VideoDecoder>>> g_videoDecoderMap = {};
std::map<std::string, std::map<int, std::vector<std::shared_ptr<VideoEncoder>>>> g_videoEncoderMap = {};
}  // namespace

AVFormatContext *CreateFormatContext(const std::string &filePath)
{
    AVFormatContext *formatContext = nullptr;
    AVDictionary *options = nullptr;

    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    av_dict_set(&options, "stimeout", "3000000", 0);
    int ret = avformat_open_input(&formatContext, filePath.c_str(), nullptr, &options);
    if (options != nullptr)
    {
        av_dict_free(&options);
    }
    if (ret != 0)
    {
        LogError << "Couldn't open input stream " << filePath.c_str() << " ret=" << ret;
        return nullptr;
    }
    ret = avformat_find_stream_info(formatContext, nullptr);
    if (ret != 0)
    {
        LogError << "Couldn't find stream information";
        return nullptr;
    }
    return formatContext;
}

void GetFrame(AVPacket &pkt, EncodedFrame &encodedFrame, AVFormatContext *pFormatCtx)
{
    av_init_packet(&pkt);
    int avRet = av_read_frame(pFormatCtx, &pkt);
    if (avRet != 0)
    {
        LogError << "[StreamPuller] Channel read frame failed, continue!";
        if (avRet == AVERROR_EOF)
        {
            LogError << "[StreamPuller] Channel streamPuller is EOF, over!";
        }
        return;
    }
    else
    {
        if (pkt.size <= 0)
        {
            LogError << "Invalid pkt.size: " << pkt.size;
            return;
        }
        auto hostDeleter = [](void *dataPtr) -> void { aclrtFreeHost(dataPtr); };
        MemoryData data(pkt.size, MemoryData::MEMORY_HOST);
        MemoryData src((void *)(pkt.data), pkt.size, MemoryData::MEMORY_HOST_MALLOC);
        APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(data, src);
        if (ret != APP_ERR_OK)
        {
            LogError << "MxbsMallocAndCopy failed!" << std::endl;
        }
        std::shared_ptr<uint8_t> imageData((uint8_t *)data.ptrData, hostDeleter);
        encodedFrame.data = imageData;
        encodedFrame.dataSize = pkt.size;
        av_packet_unref(&pkt);
    }
}

// 拉流线程
void StreamPullerThread(const std::string filePath, AVFormatContext *pFormatCtx, int deviceId, int channelId)
{
    uint32_t streamHeight = 0;
    uint32_t streamWidth = 0;
    AVPacket pkt;
    uint32_t frameId = 0;
    pFormatCtx = CreateFormatContext(filePath);
    if (pFormatCtx == nullptr)
    {
        return;
    }
    av_dump_format(pFormatCtx, 0, filePath.c_str(), 0);
    DeviceContext context = {};
    context.devId = deviceId;
    DeviceManager::GetInstance()->SetDevice(context);
    // 获取视频流真实宽高并检查是否符合要求
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; ++i)
    {
        AVStream *inStream = pFormatCtx->streams[i];
        if (inStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            streamHeight = inStream->codecpar->height;
            streamWidth = inStream->codecpar->width;
            break;
        }
    }
    if (streamWidth != FULL_HD_WIDTH || streamHeight != FULL_HD_HEIGHT)
    {
        LogError << "VideoDecoder[" << "deviceId: " << deviceId << " channelId: " << channelId
                 << "]: Video size is not 1080P.";
        return;
    }
    // 循环拉流
    while (!g_signalReceived)
    {
        EncodedFrame encodedFrame;
        encodedFrame.channelId = channelId;
        encodedFrame.frameId = frameId;
        GetFrame(pkt, encodedFrame, pFormatCtx);
        g_pullerToVdecQueueMap[deviceId][channelId]->Push(encodedFrame, true);
        frameId += 1;
    }
}

// 视频帧处理线程
void FrameProcessThread(const uint32_t deviceId, const int &channelId)
{
    while (!g_signalReceived)
    {
        DecodedFrame decodedFrame;
        APP_ERROR ret = g_vdecToCaptionQueueMap[deviceId][channelId]->Pop(decodedFrame, TIME_OUT);
        if (ret != APP_ERR_OK)
        {
            continue;
        }
        MxBase::Image imageRGB;
        ret = g_imageProcessorMap[deviceId][channelId]->ConvertFormat(decodedFrame.image, MxBase::ImageFormat::RGB_888,
                                                                      imageRGB);
        if (ret != 0)
        {
            LogError << "Fail to convert format for decoded image into RGB format";
        }
        auto imageTensor = imageRGB.ConvertToTensor(true, false);
        ret = g_captionImplVecForTextMap[deviceId][channelId]->putText(imageTensor, "位置信息1", "位置信息2", leftTop,
                                                                       DEFAULT_OPACITY);
        if (ret != APP_ERR_OK)
        {
            LogError << "Fail to put the address text in the top left corner.";
        }
        // 获取当前时间
        std::time_t now = std::time(nullptr);
        std::tm *t = std::localtime(&now);
        // 将信息输出到字符串流
        std::stringstream ss;
        ss << t->tm_year + YEAR_OFFSET << "." << t->tm_mon + MONTH_OFFSET << "." << t->tm_mday << " " << t->tm_hour
           << ":" << t->tm_min << ":" << t->tm_sec;
        ret = g_captionImplVecForTimeMap[deviceId][channelId]->putText(imageTensor, ss.str(), "", rightTop,
                                                                       DEFAULT_OPACITY);
        if (ret != APP_ERR_OK)
        {
            LogError << "Fail to put the time text in the top right corner.";
        }
        ret = g_captionImplVecForTextMap[deviceId][channelId]->putText(imageTensor, "", "预留信息", leftButtom,
                                                                       DEFAULT_OPACITY);
        if (ret != APP_ERR_OK)
        {
            LogError << "Fail to put the reserved text in the left button corner.";
        }

        MxBase::Image imageYuv;
        ret = g_imageProcessorMap[deviceId][channelId]->ConvertFormat(imageRGB, MxBase::ImageFormat::YUV_SP_420,
                                                                      imageYuv);
        if (ret != 0)
        {
            LogError << "Fail to convert format for image";
        }
        decodedFrame.image = imageYuv;

        // 一路变两路
        ret = g_captionToVencQueueMap["CIF"][deviceId][channelId]->Push(decodedFrame, TIME_OUT);
        if (ret != APP_ERR_OK)
        {
            std::cout << "Fail to push frame into vencQueue";
        }
        ret = g_captionToVencQueueMap["1080P"][deviceId][channelId]->Push(decodedFrame, TIME_OUT);
        if (ret != APP_ERR_OK)
        {
            std::cout << "Fail to push frame into vencQueue";
        }
    }
}

// 视频帧解码线程
void VdecThread(std::shared_ptr<VideoDecoder> videoDecoder, int deviceId, int channelId)
{
    uint32_t frameId = 0;
    MxBase::DeviceContext context = {};
    context.devId = deviceId;
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->SetDevice(context);
    if (ret != 0)
    {
        LogError << "Fail to set device for vdecThread.";
        return;
    }
    MxBase::MemoryData imgData(FULL_HD_WIDTH * FULL_HD_HEIGHT * YUV420_RATIO,
                               MxBase::MemoryData::MemoryType::MEMORY_DVPP, deviceId);
    ret = MxBase::MemoryHelper::MxbsMalloc(imgData);
    if (ret != 0)
    {
        LogError << "malloc error" << std::endl;
        return;
    }
    std::shared_ptr<uint8_t> pastedData((uint8_t *)imgData.ptrData, imgData.free);
    MxBase::Size imgSize(FULL_HD_WIDTH, FULL_HD_HEIGHT);
    MxBase::Image psatedImgTmp(pastedData, FULL_HD_WIDTH * FULL_HD_HEIGHT * YUV420_RATIO, deviceId, imgSize,
                               MxBase::ImageFormat::YUV_SP_420);

    while (!g_signalReceived)
    {
        EncodedFrame encodedFrame;
        ret = g_pullerToVdecQueueMap[deviceId][channelId]->Pop(encodedFrame, TIME_OUT);
        if (ret != APP_ERR_OK)
        {
            continue;
        }
        ret = videoDecoder->Decode(encodedFrame.data, encodedFrame.dataSize, frameId, psatedImgTmp,
                                   (void *)&(*g_vdecToCaptionQueueMap[deviceId][channelId]));
        if (ret != APP_ERR_OK)
        {
            LogError << "VideoDecoder decode failed. Ret is: " << ret;
        }
        frameId += 1;
    }
}

APP_ERROR SetConfigValue(ConfigParser &configParser, uint32_t &deviceNum, std::vector<std::string> &streamNames)
{
    APP_ERROR ret = configParser.ParseConfig(CONFIG_FILE_NAME);
    if (ret != APP_ERR_OK)
    {
        LogError << "Cannot parse file.";
        return APP_ERR_COMM_FAILURE;
    }

    ret = configParser.GetUnsignedIntValue("deviceNum", deviceNum);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to get config variable named deviceNum.";
        return APP_ERR_COMM_FAILURE;
    }

    unsigned int saveVideo = 0;
    ret = configParser.GetUnsignedIntValue("saveVideo", saveVideo);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to get config variable named saveVideo.";
        return APP_ERR_COMM_FAILURE;
    }
    if (saveVideo != 0 && saveVideo != 1)
    {
        LogError << "saveVideo must be set in the range [0, 1].";
        return APP_ERR_COMM_FAILURE;
    }
    if (saveVideo == 0)
    {
        g_saveVIDEO = false;
    }
    streamNames = std::vector<std::string>(CHANNEL_COUNT * deviceNum);
    for (unsigned int i = 0; i < deviceNum * CHANNEL_COUNT; i++)
    {
        std::string itemCfgStr = std::string("stream.ch") + std::to_string(i);
        ret = configParser.GetStringValue(itemCfgStr, streamNames[i]);
        if (ret != APP_ERR_OK)
        {
            LogError << "Get StreamNames from config file fail.";
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR VencCallBack(std::shared_ptr<uint8_t> &outDataPtr, uint32_t &outDataSize, uint32_t &channelId,
                       uint32_t &frameId, void *userData)
{
    EncodedFrame encodedFrame{outDataPtr, outDataSize, frameId, channelId};
    auto encodedQueue = static_cast<BlockingQueue<EncodedFrame> *>(userData);
    if (encodedQueue == nullptr)
    {
        LogError << "EncodedQueue has been released." << std::endl;
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    if (g_saveVIDEO)
    {
        encodedQueue->Push(encodedFrame);
    }
    return APP_ERR_OK;
}

// 视频帧编码线程
void VencThread(std::shared_ptr<VideoEncoder> videoEncoder, std::shared_ptr<ImageProcessor> imageProcessor,
                int deviceId, int channelId, std::string videoType)
{
    uint32_t frameId = 0;
    while (!g_signalReceived)
    {
        if (videoType == "CIF")
        {
            DecodedFrame decodedFrame;
            APP_ERROR ret = g_captionToVencQueueMap[videoType][deviceId][channelId]->Pop(decodedFrame, TIME_OUT);
            if (ret != APP_ERR_OK)
            {
                continue;
            }
            Image resizedImage;
            Image resizedMidImage;
            ret = g_imageProcessorMap[deviceId][channelId]->Resize(decodedFrame.image, RESIZE_MIDDLE_IMAGE_SIZE,
                                                                   resizedMidImage,
                                                                   MxBase::Interpolation::HUAWEI_HIGH_ORDER_FILTER);
            if (ret != APP_ERR_OK)
            {
                LogError << "Fail to resize middle image." << " [DeviceId: " << deviceId << "  channelId: " << channelId
                         << "].";
            }

            ret = g_imageProcessorMap[deviceId][channelId]->Resize(resizedMidImage, CIF_IMAGE_SIZE, resizedImage,
                                                                   (MxBase::Interpolation)0);
            if (ret != APP_ERR_OK)
            {
                LogError << "Fail to resize final image" << " [DeviceId: " << deviceId << "  channelId: " << channelId
                         << "].";
            }
            decodedFrame.image = resizedImage;
            ret = videoEncoder->Encode(decodedFrame.image, decodedFrame.frameId,
                                       static_cast<void *>(&(*g_vencToFileQueueMap[videoType][deviceId][channelId])));
            if (ret != APP_ERR_OK)
            {
                LogError << "Fail to encode 1080P image." << " [DeviceId: " << deviceId << "  channelId: " << channelId
                         << "].";
            }
        }
        else
        {
            DecodedFrame decodedFrame;
            APP_ERROR ret = g_captionToVencQueueMap[videoType][deviceId][channelId]->Pop(decodedFrame, TIME_OUT);
            if (ret != APP_ERR_OK)
            {
                continue;
            }
            ret = videoEncoder->Encode(decodedFrame.image, decodedFrame.frameId,
                                       static_cast<void *>(&(*g_vencToFileQueueMap[videoType][deviceId][channelId])));
            if (ret != APP_ERR_OK)
            {
                LogError << "Fail to encode 1080P image." << " [DeviceId: " << deviceId << "  channelId: " << channelId
                         << "].";
            }
        }
        frameId += 1;
    }
}

// 编码后视频帧保存线程
void SaveFrameThread(int deviceId, int channelId, std::string videoType)
{
    string savePath =
        "../output/deviceId_" + to_string(deviceId) + " _channelId_" + to_string(channelId) + "_" + videoType + ".h264";
    FILE *fp = fopen(savePath.c_str(), "wb");
    if (fp == nullptr)
    {
        LogError << "Failed to open file.";
        return;
    }

    bool mbFoundFirstIDR = false;
    bool bIsIDR = false;
    while (!g_signalReceived)
    {
        EncodedFrame encodedFrame;
        APP_ERROR ret = g_vencToFileQueueMap[videoType][deviceId][channelId]->Pop(encodedFrame, TIME_OUT);
        if (ret != APP_ERR_OK)
        {
            continue;
        }
        bIsIDR = (encodedFrame.dataSize > 1);
        if (!mbFoundFirstIDR)
        {
            if (!bIsIDR)
            {
                LogWarn << "Not bIsIDR!!!";
                continue;
            }
            else
            {
                mbFoundFirstIDR = true;
            }
        }
        if (fwrite(encodedFrame.data.get(), encodedFrame.dataSize, 1, fp) != 1)
        {
            LogError << "write frame to file fail" << std::endl;
        }
    }
    if (fclose(fp) != 0)
    {
        LogError << "Failed to close file for device " << deviceId << "  channel " << channelId << ".";
    };
}

APP_ERROR InitCaptionResource(CaptionImpl &captionImpl, int deviceId)
{
    std::string fontSize = "60px";
    float fontScale = 1;
    MxBase::Color textColor = MxBase::Color(255, 255, 255);
    MxBase::Color backgroundColor = MxBase::Color(0, 0, 0);
    APP_ERROR ret = captionImpl.init("simsun", fontSize, "times", fontSize, deviceId);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to init captionImpl.";
        return APP_ERR_COMM_FAILURE;
    }
    auto length = captionImpl.getLength("位置信息，时间信息，预留信息");
    ret = captionImpl.initRectAndColor(textColor, backgroundColor, fontScale, length);
    if (ret != APP_ERR_OK)
    {
        LogError << "Fail to init the intermediate tensor of captionImpl1.";
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR VdecCallBack(MxBase::Image &decodedImage, uint32_t channelId, uint32_t frameId, void *userData)
{
    DecodedFrame decodedFrame{decodedImage, frameId, channelId};
    BlockingQueue<DecodedFrame> *decodedVec = static_cast<BlockingQueue<DecodedFrame> *>(userData);
    if (decodedVec == nullptr)
    {
        LogError << "VideoDecoderCallback: decodedVec has been released.";
        return APP_ERR_DVPP_INVALID_FORMAT;
    }

    decodedVec->Push(decodedFrame, true);
    return APP_ERR_OK;
}

APP_ERROR GeneratePairEncoderAndDecoder(int deviceId, int channelId)
{
    VideoDecodeConfig vDecodeConfig;
    vDecodeConfig.width = FULL_HD_WIDTH;
    vDecodeConfig.height = FULL_HD_HEIGHT;
    vDecodeConfig.callbackFunc = VdecCallBack;
    std::shared_ptr<VideoDecoder> videoDecoder = std::make_shared<VideoDecoder>(vDecodeConfig, deviceId, channelId);
    g_videoDecoderMap[deviceId].push_back(videoDecoder);

    VideoEncodeConfig vEncodeConfig;
    vEncodeConfig.callbackFunc = VencCallBack;
    vEncodeConfig.height = FULL_HD_HEIGHT;
    vEncodeConfig.width = FULL_HD_WIDTH;
    vEncodeConfig.keyFrameInterval = 1;
    vEncodeConfig.srcRate = DEFAULT_SRC_RATE;
    vEncodeConfig.maxBitRate = DEFAULT_MAX_BIT_RATE;
    vEncodeConfig.maxPicHeight = FULL_HD_HEIGHT;
    vEncodeConfig.maxPicWidth = FULL_HD_WIDTH;
    std::shared_ptr<VideoEncoder> videoEncoder1080p =
        std::make_shared<VideoEncoder>(vEncodeConfig, deviceId, channelId);
    g_videoEncoderMap["1080P"][deviceId].push_back(videoEncoder1080p);

    vEncodeConfig.height = CIF_HEIGHT;
    vEncodeConfig.width = CIF_WIDTH;
    vEncodeConfig.maxPicHeight = CIF_HEIGHT;
    vEncodeConfig.maxPicWidth = CIF_WIDTH;
    std::shared_ptr<VideoEncoder> videoEncoderCIF = std::make_shared<VideoEncoder>(vEncodeConfig, deviceId, channelId);
    g_videoEncoderMap["CIF"][deviceId].push_back(videoEncoderCIF);

    return APP_ERR_OK;
}

APP_ERROR GenerateResourcesForDevices(int deviceNum, int channelCount, const ConfigParser &configParser)
{
    for (int deviceId = 0; deviceId < deviceNum; deviceId++)
    {
        // 初始化线程通信队列
        for (int channelId = 0; channelId < channelCount; channelId++)
        {
            g_pullerToVdecQueueMap[deviceId].push_back(std::make_shared<BlockingQueue<EncodedFrame>>(QUEUE_SIZE));
            g_vdecToCaptionQueueMap[deviceId].push_back(std::make_shared<BlockingQueue<DecodedFrame>>(QUEUE_SIZE));
            g_captionToVencQueueMap["CIF"][deviceId].push_back(
                std::make_shared<BlockingQueue<DecodedFrame>>(QUEUE_SIZE));
            g_captionToVencQueueMap["1080P"][deviceId].push_back(
                std::make_shared<BlockingQueue<DecodedFrame>>(QUEUE_SIZE));
            g_vencToFileQueueMap["CIF"][deviceId].push_back(std::make_shared<BlockingQueue<EncodedFrame>>(QUEUE_SIZE));
            g_vencToFileQueueMap["1080P"][deviceId].push_back(
                std::make_shared<BlockingQueue<EncodedFrame>>(QUEUE_SIZE));
        }

        // 初始化字幕贴图相关资源
        for (int channelId = 0; channelId < channelCount; channelId++)
        {
            g_imageProcessorMap[deviceId].push_back(std::make_shared<MxBase::ImageProcessor>(deviceId));
            auto captionImplForTime = std::make_shared<CaptionImpl>();
            auto captionImplForText = std::make_shared<CaptionImpl>();
            APP_ERROR ret = InitCaptionResource(*captionImplForTime, deviceId);
            if (ret != APP_ERR_OK)
            {
                LogError << "Fail to init caption resource for time";
                return APP_ERR_COMM_FAILURE;
            }
            ret = InitCaptionResource(*captionImplForText, deviceId);
            if (ret != APP_ERR_OK)
            {
                LogError << "Fail to init caption resource for text";
                return APP_ERR_COMM_FAILURE;
            }
            g_captionImplVecForTimeMap[deviceId].push_back(captionImplForTime);
            g_captionImplVecForTextMap[deviceId].push_back(captionImplForText);
        }

        // 初始化视频编解码器资源
        for (int channelId = 0; channelId < channelCount; channelId++)
        {
            APP_ERROR ret = GeneratePairEncoderAndDecoder(deviceId, channelId);
            if (ret != APP_ERR_OK)
            {
                LogError << "Fail to init videoDecoder and  resource for text";
                return APP_ERR_COMM_FAILURE;
            }
        }
    }
    return APP_ERR_OK;
}

void InitializeThreadPools(ThreadPools &threadPools, int deviceNum, int channelCount)
{
    for (int deviceId = 0; deviceId < deviceNum; deviceId++)
    {
        std::vector<AVFormatContext *> pFormatCtxs(channelCount, nullptr);
        std::vector<std::thread> threadStreamPuller(channelCount);
        std::vector<std::thread> threadVdec(channelCount);
        std::vector<std::thread> threadFrameProcess(channelCount);
        std::vector<std::thread> threadVenc1080P(channelCount);
        std::vector<std::thread> threadVencCIF(channelCount);
        std::vector<std::thread> threadCIFSave(channelCount);
        std::vector<std::thread> thread1080PSave(channelCount);
        threadPools.threadStreamPullerPool.emplace(deviceId, std::move(threadStreamPuller));
        threadPools.threadVdecPool.emplace(deviceId, std::move(threadVdec));
        threadPools.threadFrameProcessPool.emplace(deviceId, std::move(threadFrameProcess));
        threadPools.threadVenc1080PPool.emplace(deviceId, std::move(threadVenc1080P));
        threadPools.threadVencCIFPool.emplace(deviceId, std::move(threadVencCIF));
        if (g_saveVIDEO)
        {
            threadPools.threadCIFSavePool.emplace(deviceId, std::move(threadCIFSave));
            threadPools.thread1080PSavePool.emplace(deviceId, std::move(thread1080PSave));
        }
    }
}

void StartThreads(ThreadPools &threadPools, std::map<int, std::vector<AVFormatContext *>> &pFormatCtxsMap,
                  std::vector<std::string> &streamNames, int deviceNum, int channelCount)
{
    for (int deviceId = 0; deviceId < deviceNum; deviceId++)
    {
        for (int channelId = 0; channelId < channelCount; channelId++)
        {
            // 启动拉流线程
            threadPools.threadStreamPullerPool[deviceId][channelId] =
                std::thread(StreamPullerThread, streamNames[deviceId * channelCount + channelId],
                            pFormatCtxsMap[deviceId][channelId], deviceId, channelId);
            // 启动解码线程
            threadPools.threadVdecPool[deviceId][channelId] =
                std::thread(VdecThread, g_videoDecoderMap[deviceId][channelId], deviceId, channelId);
            // 启动视频帧处理线程
            threadPools.threadFrameProcessPool[deviceId][channelId] =
                std::thread(FrameProcessThread, deviceId, channelId);
            // 启动CIF视频编码线程
            threadPools.threadVencCIFPool[deviceId][channelId] =
                std::thread(VencThread, g_videoEncoderMap["CIF"][deviceId][channelId],
                            g_imageProcessorMap[deviceId][channelId], deviceId, channelId, "CIF");
            // 启动1080P视频编码线程
            threadPools.threadVenc1080PPool[deviceId][channelId] =
                std::thread(VencThread, g_videoEncoderMap["1080P"][deviceId][channelId],
                            g_imageProcessorMap[deviceId][channelId], deviceId, channelId, "1080P");
            if (g_saveVIDEO)
            {
                // 启动CIF视频保存线程
                threadPools.threadCIFSavePool[deviceId][channelId] =
                    std::thread(SaveFrameThread, deviceId, channelId, "CIF");
                // 启动1080P视频保存线程
                threadPools.thread1080PSavePool[deviceId][channelId] =
                    std::thread(SaveFrameThread, deviceId, channelId, "1080P");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(INIT_INTERVAL));
            std::cout << "Succeed to start working threads for device " << deviceId << ", channelId " << channelId
                      << std::endl;
        }
    }
}

void JoinThreads(ThreadPools &threadPools, int deviceNum, int channelCount)
{
    for (int deviceId = 0; deviceId < deviceNum; deviceId++)
    {
        for (int channelId = 0; channelId < channelCount; channelId++)
        {
            threadPools.threadStreamPullerPool[deviceId][channelId].join();
            threadPools.threadVdecPool[deviceId][channelId].join();
            threadPools.threadFrameProcessPool[deviceId][channelId].join();
            threadPools.threadVencCIFPool[deviceId][channelId].join();
            threadPools.threadVenc1080PPool[deviceId][channelId].join();
            if (g_saveVIDEO)
            {
                threadPools.threadCIFSavePool[deviceId][channelId].join();
                threadPools.thread1080PSavePool[deviceId][channelId].join();
            }
            std::cout << "Succeed to join working threads for device " << deviceId << ", channelId " << channelId << "."
                      << std::endl;
        }
    }
}
APP_ERROR StartServices(int deviceNum, int channelCount, std::vector<std::string> &streamNames)
{
    // 创建线程池
    ThreadPools threadPools;
    InitializeThreadPools(threadPools, deviceNum, channelCount);
    std::map<int, std::vector<AVFormatContext *>> pFormatCtxsMap;
    for (int deviceId = 0; deviceId < deviceNum; deviceId++)
    {
        std::vector<AVFormatContext *> pFormatCtxs(channelCount, nullptr);
        pFormatCtxsMap.emplace(deviceId, std::move(pFormatCtxs));
    }

    // 启动线程
    StartThreads(threadPools, pFormatCtxsMap, streamNames, deviceNum, channelCount);
    std::cout << "**************All working threads are started.**************" << std::endl;

    // 循环等待
    while (!g_signalReceived)
    {
        usleep(SIGNAL_CHECK_TIMESTEP);
    }

    // 销毁线程
    JoinThreads(threadPools, deviceNum, channelCount);
    std::cout << "**************All working threads are joined.**************" << std::endl;
    return APP_ERR_OK;
}

static void SignalHandler(int signal)
{
    if (signal == SIGINT)
    {
        g_signalReceived = true;
    }
}

void ClearGlobalContainers()
{
    g_imageProcessorMap.clear();
    g_pullerToVdecQueueMap.clear();
    g_vdecToCaptionQueueMap.clear();
    g_captionToVencQueueMap.clear();
    g_vencToFileQueueMap.clear();
    g_captionImplVecForTimeMap.clear();
    g_captionImplVecForTextMap.clear();
    g_videoDecoderMap.clear();
    g_videoEncoderMap.clear();
}

int main(int argc, char *argv[])
{
    // 初始化全局资源
    avformat_network_init();
    if (MxInit() != APP_ERR_OK)
    {
        LogError << "Fail to conduct MxInit.";
        return APP_ERR_COMM_FAILURE;
    }
    if (!CaptionGenManager::getInstance().Init())
    {
        LogError << "Fail to init CaptionGenManager.";
        return APP_ERR_COMM_FAILURE;
    }
    if (signal(SIGINT, SignalHandler) == SIG_ERR)
    {
        LogError << "Fail to register SignalHandler.";
        return APP_ERR_COMM_FAILURE;
    }

    {
        // 解析配置文件
        uint32_t deviceNum = 0;
        std::vector<std::string> streamNames;
        ConfigParser configParser;
        APP_ERROR ret = SetConfigValue(configParser, deviceNum, streamNames);
        if (ret != APP_ERR_OK)
        {
            LogError << "Fail to parse config file";
            return APP_ERR_COMM_FAILURE;
        }

        // 生成设备资源
        ret = GenerateResourcesForDevices(deviceNum, CHANNEL_COUNT, configParser);
        if (ret != APP_ERR_OK)
        {
            LogError << "Fail to generate resources for devices";
            return APP_ERR_COMM_FAILURE;
        }

        // 拉起服务
        ret = StartServices(deviceNum, CHANNEL_COUNT, streamNames);
        if (ret != APP_ERR_OK)
        {
            LogError << "Fail to start services";
            return APP_ERR_COMM_FAILURE;
        }
        // 清理全局容器资源
        std::cout << "**************Begin clear global NPU resources.**************" << std::endl;
        ClearGlobalContainers();
    }
    // 销毁资源
    CaptionGenManager::getInstance().DeInit();
    CaptionGeneration::getAscendStream().DestroyAscendStream();
    MxDeInit();
    std::cout << "**************PutTextForMultiVideos end.**************" << std::endl;
    return 0;
}

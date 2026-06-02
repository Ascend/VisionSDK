/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: VideoDecoder and videoEncoder tutorials.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include <sys/time.h>

#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include "BlockingQueue.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/E2eInfer/Image/Image.h"
#include "MxBase/E2eInfer/VideoDecoder/VideoDecoder.h"
#include "MxBase/E2eInfer/VideoEncoder/VideoEncoder.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MxBase.h"
#include "acl/acl.h"
#include "acl/acl_rt.h"
extern "C"
{
#include <libavformat/avformat.h>
}

using namespace std;
using namespace MxBase;

const string VIDEO_PATH = "path";
const uint32_t VIDEO_WIDTH = 1920;
const uint32_t VIDEO_HEIGHT = 1080;
const uint32_t VIDEO_FPS = 25;

const int TIME_OUT = 3000;
const int QUEUE_SIZE = 1000;
const int FRAME_WAIT_TIME = 15;
const int DEFAULT_DEVICE_ID = 0;
const int DEFAULT_CHANNEL_ID = 0;
const std::string DEFAULT_SAVED_FILE_PATH = "./output";
static std::atomic<bool> g_sendSignal{false};
static std::atomic<bool> g_readVideoEnded{false};
static std::atomic<bool> g_vdecEnded{false};
static std::atomic<bool> g_vencEnded{false};

static void SigHandler(int signal)
{
    if (signal == SIGINT)
    {
        g_sendSignal = true;
    }
}

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

// 创建用于拉流线程、解码线程、编码线程、视频文件保存线程之间通信的全局队列
BlockingQueue<EncodedFrame> g_pullerToVdecQueue(QUEUE_SIZE);
BlockingQueue<DecodedFrame> g_vdecToVencQueue(QUEUE_SIZE);
BlockingQueue<EncodedFrame> g_vencToFileSaveQueue(QUEUE_SIZE);

AVFormatContext* CreateFormatContext(const std::string& filePath)
{
    AVFormatContext* formatContext = nullptr;
    AVDictionary* options = nullptr;
    int ret = avformat_open_input(&formatContext, filePath.c_str(), nullptr, nullptr);
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

void GetFrame(AVPacket& pkt, EncodedFrame& encodedFrame, AVFormatContext* pFormatCtx)
{
    av_init_packet(&pkt);
    int avRet = av_read_frame(pFormatCtx, &pkt);
    if (avRet != 0)
    {
        LogWarn << "[StreamPuller] Channel read frame failed, continue!";
        if (avRet == AVERROR_EOF)
        {
            g_readVideoEnded = true;
            LogWarn << "[StreamPuller] Channel streamPuller is EOF, over!";
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
        auto hostDeleter = [](void* dataPtr) -> void { aclrtFreeHost(dataPtr); };
        MemoryData data(pkt.size, MemoryData::MEMORY_HOST);
        MemoryData src((void*)(pkt.data), pkt.size, MemoryData::MEMORY_HOST_MALLOC);
        APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(data, src);
        if (ret != APP_ERR_OK)
        {
            LogError << "MxbsMallocAndCopy failed!" << std::endl;
        }
        std::shared_ptr<uint8_t> imageData((uint8_t*)data.ptrData, hostDeleter);
        encodedFrame.data = imageData;
        encodedFrame.dataSize = pkt.size;
        av_packet_unref(&pkt);
    }
}

// 线程1：用于拉流
void StreamPullerThread(const std::string filePath, const uint32_t width, const uint32_t height)
{
    uint32_t streamHeight = 0;
    uint32_t streamWidth = 0;
    AVPacket pkt;
    uint32_t frameId = 0;
    uint32_t channelId = 0;
    AVFormatContext* pFormatCtx = CreateFormatContext(filePath);  // create context
    if (pFormatCtx == nullptr)
    {
        return;
    }
    av_dump_format(pFormatCtx, 0, filePath.c_str(), 0);
    DeviceContext context = {};
    context.devId = 0;
    DeviceManager::GetInstance()->SetDevice(context);
    // get the real width and height of the stream
    for (unsigned int i = 0; i < pFormatCtx->nb_streams; ++i)
    {
        AVStream* inStream = pFormatCtx->streams[i];
        if (inStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            streamHeight = inStream->codecpar->height;
            streamWidth = inStream->codecpar->width;
            if (streamHeight != height)
            {
                LogError << "Video height " << streamHeight << " is not equal to the configuration height " << height
                         << ".";
                g_readVideoEnded = true;
                return;
            }
            if (streamWidth != width)
            {
                LogError << "Video width " << streamWidth << " is not equal to the configuration width " << width
                         << ".";
                g_readVideoEnded = true;
                return;
            }
        }
    }
    while (!g_sendSignal && !g_readVideoEnded)
    {
        EncodedFrame encodedFrame;
        encodedFrame.channelId = channelId;
        encodedFrame.frameId = frameId;
        GetFrame(pkt, encodedFrame, pFormatCtx);
        g_pullerToVdecQueue.Push(encodedFrame, true);
        frameId += 1;
        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_WAIT_TIME));
    }
    std::cout << "*********************StreamPullThread end*********************" << std::endl;
}

//                                      |拉流|
//                                        |
//                                        |
//                                        V
//                                   |下发解码指令|
// 线程2：用于下发解码指令
void VdecThread(VideoDecoder& videoDecoder)
{
    while (!g_sendSignal)
    {
        if (g_readVideoEnded && g_pullerToVdecQueue.GetSize() == 0)
        {
            break;
        }
        // 获取待解码的视频帧数据
        EncodedFrame encodedFrame;
        APP_ERROR ret = g_pullerToVdecQueue.Pop(encodedFrame, TIME_OUT);
        if (ret != APP_ERR_OK || encodedFrame.data == nullptr)
        {
            continue;
        }
        // 下发解码指令
        ret = videoDecoder.Decode(encodedFrame.data, encodedFrame.dataSize, encodedFrame.frameId,
                                  static_cast<void*>(&g_vdecToVencQueue));
        if (ret != APP_ERR_OK)
        {
            LogError << "Decode failed.";
        }
        // 控制调用Decode接口的频率
        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_WAIT_TIME));
    }
    g_vdecEnded = true;
    std::cout << "*********************VdecThread end*********************" << std::endl;
}

//                                   |下发解码指令|
//                                        |
//                                        |
//                                        V
//                                   |获取解码结果|
// 线程3：用于获取解码结果（获取解码结果的线程由Vision
// SDK内部创建，用户仅需自定义回调函数、用于由该线程调用、获取解码结果）
APP_ERROR VdecCallBack(MxBase::Image& decodedImage, uint32_t channelId, uint32_t frameId, void* userData)
{
    DecodedFrame decodedFrame{decodedImage, frameId, channelId};
    BlockingQueue<DecodedFrame>* vdecToVencQueuePtr = static_cast<BlockingQueue<DecodedFrame>*>(userData);
    if (vdecToVencQueuePtr == nullptr)
    {
        LogError << "VideoDecoderCallback: g_vdecToVencQueue has been released.";
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    vdecToVencQueuePtr->Push(decodedFrame, true);
    return APP_ERR_OK;
}

//                                   |获取解码结果|
//                                        |
//                                        |
//                                        V
//                                   |下发编码指令|
// 线程4：用于下发编码指令
void VencThread(VideoEncoder& videoEncoder)
{
    while (!g_sendSignal)
    {
        if (g_vdecEnded && g_vdecToVencQueue.GetSize() == 0)
        {
            break;
        }
        // 获取解码后的视频帧
        DecodedFrame decodedFrame;
        APP_ERROR ret = g_vdecToVencQueue.Pop(decodedFrame, TIME_OUT);
        if (ret != APP_ERR_OK)
        {
            continue;
        }
        // 下发编码指令
        ret = videoEncoder.Encode(decodedFrame.image, decodedFrame.frameId, static_cast<void*>(&g_vencToFileSaveQueue));
        if (ret != APP_ERR_OK)
        {
            LogError << "Encode failed.";
        }
        // 控制调用Encode接口的频率
        std::this_thread::sleep_for(std::chrono::milliseconds(FRAME_WAIT_TIME));
    }
    g_vencEnded = true;
    std::cout << "*********************VencThread end*********************" << std::endl;
}

//                                   |下发编码指令|
//                                        |
//                                        |
//                                        V
//                                   |获取编码结果|
// 线程5：用于获取编码结果（用于获取编码结果的线程由Vision
// SDK内部创建，用户仅需自定义回调函数、用于由该线程调用、获取编码结果）
APP_ERROR VencCallBack(std::shared_ptr<uint8_t>& outDataPtr, uint32_t& outDataSize, uint32_t& channelId,
                       uint32_t& frameId, void* userData)
{
    EncodedFrame encodedFrame{outDataPtr, outDataSize, frameId, channelId};
    auto vencToFileSaveQueuePtr = static_cast<BlockingQueue<EncodedFrame>*>(userData);
    if (vencToFileSaveQueuePtr == nullptr)
    {
        LogError << "g_vencToFileSaveQueue has been released." << std::endl;
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    vencToFileSaveQueuePtr->Push(encodedFrame, true);
    return APP_ERR_OK;
}

//                                   |获取编码结果|
//                                        |
//                                        |
//                                        V
//                                   |保存编码结果|
// 线程6：用于保存编码结果
void SaveFrameThread(StreamFormat streamFormat)
{
    string savePath = DEFAULT_SAVED_FILE_PATH;
    if (streamFormat == StreamFormat::H265_MAIN_LEVEL)
    {
        savePath = savePath + ".h265";
    }
    else
    {
        savePath = savePath + ".h264";
    }

    FILE* fp = fopen(savePath.c_str(), "wb");
    if (fp == nullptr)
    {
        LogError << "Failed to open file.";
        return;
    }

    bool mbFoundFirstIDR = false;
    bool bIsIDR = false;
    while (!g_sendSignal)
    {
        if (g_vencEnded && g_vencToFileSaveQueue.GetSize() == 0)
        {
            break;
        }
        // 获取编码后的视频帧
        EncodedFrame encodedFrame;
        APP_ERROR ret = g_vencToFileSaveQueue.Pop(encodedFrame, TIME_OUT);
        if (ret != APP_ERR_OK)
        {
            continue;
        }
        // 保存编码后的视频帧
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
            LogError << "write frame to file fail";
        }
    }
    if (fclose(fp) != 0)
    {
        LogError << "Failed to close file.";
    }
    std::cout << "*********************Save frame thread end*********************" << std::endl;
}

int main(int argc, char* argv[])
{
    // 初始化全局资源
    avformat_network_init();
    if (MxInit() != APP_ERR_OK)
    {
        LogError << "Fail to conduct MxInit.";
        return APP_ERR_COMM_FAILURE;
    }
    if (signal(SIGINT, SigHandler) == SIG_ERR)
    {
        LogError << "Fail to register SigHandler.";
        return APP_ERR_COMM_FAILURE;
    }
    {
        // 设置输入视频路径和该视频宽、高
        std::string filePath = VIDEO_PATH;
        int width = VIDEO_WIDTH;
        int height = VIDEO_HEIGHT;
        // 设置解码器主要配置项，根据配置项初始化解码器
        VideoDecodeConfig vDecodeConfig;
        vDecodeConfig.width = width;                                     // 指定视频宽
        vDecodeConfig.height = height;                                   // 指定视频高
        vDecodeConfig.inputVideoFormat = StreamFormat::H264_MAIN_LEVEL;  // 指定待解码的输入视频格式
        vDecodeConfig.outputImageFormat = ImageFormat::YUV_SP_420;       // 指定解码后的输出图片格式
        vDecodeConfig.callbackFunc = VdecCallBack;  // 指定解码后、用于取解码结果的回调函数
        VideoDecoder videoDecoder = VideoDecoder(vDecodeConfig, DEFAULT_DEVICE_ID, DEFAULT_CHANNEL_ID);  // 初始化解码器

        // 设置编码器主要配置项，根据配置项初始化编码器
        VideoEncodeConfig vEncodeConfig;
        vEncodeConfig.width = width;                                      // 指定视频宽
        vEncodeConfig.height = height;                                    // 指定视频高
        vEncodeConfig.inputImageFormat = ImageFormat::YUV_SP_420;         // 指定待编码的输入图片格式
        vEncodeConfig.srcRate = VIDEO_FPS;                                // 指定待编码的输入图片帧率
        vEncodeConfig.outputVideoFormat = StreamFormat::H264_MAIN_LEVEL;  // 指定编码后的输出视频格式
        vEncodeConfig.displayRate = VIDEO_FPS;                            // 指定编码后的输出视频帧率
        vEncodeConfig.callbackFunc = VencCallBack;  // 指定编码后，用于取编码结果的回调函数
        VideoEncoder videoEncoder = VideoEncoder(vEncodeConfig, DEFAULT_DEVICE_ID, DEFAULT_CHANNEL_ID);  // 初始化编码器

        // 启动拉流线程
        std::thread streamPullerThread = std::thread(StreamPullerThread, filePath, width, height);
        std::cout << "*********************StreamPullerThread start*********************" << std::endl;
        // 启动视频解码线程
        std::thread vdecThread = std::thread(VdecThread, std::ref(videoDecoder));
        std::cout << "*********************VdecThread start*********************" << std::endl;
        // 启动视频编码线程
        std::thread vencThread = std::thread(VencThread, std::ref(videoEncoder));
        std::cout << "*********************VencThread start*********************" << std::endl;
        // 启动视频文件保存线程
        std::thread saveFrameThread = std::thread(SaveFrameThread, vEncodeConfig.outputVideoFormat);
        std::cout << "*********************SaveFrameThread start*********************" << std::endl;

        // 等待执行完毕
        streamPullerThread.join();
        vdecThread.join();
        vencThread.join();
        saveFrameThread.join();

        // 销毁全局资源
        g_pullerToVdecQueue.Clear();
        g_vdecToVencQueue.Clear();
        g_vencToFileSaveQueue.Clear();
    }
    // 去初始化
    MxDeInit();
    return 0;
}

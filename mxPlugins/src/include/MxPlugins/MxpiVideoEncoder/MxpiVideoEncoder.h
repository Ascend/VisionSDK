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

#ifndef MXPLUGINS_MXPIVIDEOENCODER_H
#define MXPLUGINS_MXPIVIDEOENCODER_H

#include <atomic>
#include <tuple>
#include <map>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/BlockingQueue/BlockingQueue.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
/**
 * This plugin is used for video decoding. Temporarily only h264/h265 format is supported.
 */
namespace MxPlugins {
class MxpiVideoEncoder : public MxTools::MxPluginBase {
public:
    /**
    * @description: store Buffer info and time
    */
    struct BufferCache {
        MxTools::MxpiBuffer* buffer;
        time_t               addTime;
    };

    /**
    * @description: Init configs, device and dvpp.
    * @param configParamMap: config.
    * @return: Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap) override;

    /**
    * @description: DeInit device and dvpp.
    * @return: Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @description: MxpiVideoEncoder plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

    /**
    * @description: MxpiVideoEncoder plugin define properties.
    * @return: properties.
    */
    static std::vector<std::shared_ptr<void>> DefineProperties();

    /**
    * @api
    * @brief Define the number and data type of input ports.
    * @return MxTools::MxpiPortInfo.
    */
    static MxTools::MxpiPortInfo DefineInputPorts();

    /**
    * @api
    * @brief Define the number and data type of output ports.
    * @return MxTools::MxpiPortInfo.
    */
    static MxTools::MxpiPortInfo DefineOutputPorts();

private:
    void CalFps();
    void SendDataThread();
    bool CheckParam();
    APP_ERROR GetChannelDescParam(std::map<std::string, std::shared_ptr<void>> &configParamMap,
        MxBase::VencConfig &vencConfig);
    void VencConfigInit(MxBase::VencConfig &vencConfig);
    void DeleteCacheByFrameId(uint64_t frameId);

private:
    uint32_t imageWidth_ = 1920;
    uint32_t imageHeight_ = 1080;
    uint32_t keyFrameInterval_ = 200;
    std::string outputFormat_ = "H264";
    std::string inputFormat_ = "YUV420SP_NV12";
    std::string parentName_ = {};
    std::atomic<uint32_t> fpsCount_ = {};
    uint32_t enableFpsMode_ = 0; // 0: print no fps. 1: print fps
    bool fpsStopFlag_ = false;
    std::unique_ptr<std::thread> threadFps_ = nullptr;

    bool sendStopFlag_ = false;
    std::shared_ptr<MxBase::BlockingQueue<std::tuple<std::shared_ptr<uint8_t>, uint32_t, uint32_t>>> queue_ = nullptr;
    std::unique_ptr<std::thread> threadSend_ = nullptr;

    /* store mxpiBuffer for async free */
    std::unordered_map<uint64_t, BufferCache> processCache_ = {};

    uint32_t channelId_ = 0;
    uint32_t releaseTimeout_ = 0;
    uint64_t sendFrameId_ = 0;
    uint64_t handleFrameId_ = 0;
    std::shared_ptr<MxBase::DvppWrapper> dvppWrapper_ = nullptr;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t)> callbackHandle_ = {};
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)> callbackHandleV2_ = {};
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**, void*)> callbackHandleV2For310P_ = {};
    std::mutex fpsMutex_ = {};
    std::mutex cacheMutex_ = {};
    std::condition_variable fpsCv_ = {};
};
}
#endif // MXPLUGINS_DVPPVIDEOENCODER_H
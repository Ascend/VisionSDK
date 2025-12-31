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
 * Description: Automatic sending of empty buffer upon timeout.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "MxPlugins/MxpiBufferStablizer/MxpiBufferStablizer.h"
#include <unistd.h>
#include <sys/time.h>
#include <functional>
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

namespace {
const int INIT_DEVICEID = -1;
const int INIT_DATASIZE = 1;
const int CHECKFREQUENCY = 10;
const float SEC2MS = 1000.0;
const uint32_t MAX_INTERVAL = 100000;
}

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

APP_ERROR MxpiBufferStablizer::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiBufferStablizer(" << elementName_ << ").";
    // get config param map
    configParamMap_ = &configParamMap;
    // init props
    auto ret = InitAndRefreshProps();
    if (ret != APP_ERR_OK) {
        LogError << "MxpiBufferStablizer init and refresh props failed." << GetErrorInfo(ret);
        return ret;
    }
    // create threadi
    CreateThread();
    LogInfo << "End to initialize MxpiBufferStablizer(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiBufferStablizer::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiBufferStablizer(" << elementName_ << ").";
    runningFlag_ = false;
    realCount_ = 0;
    condition_.notify_all();
    if (thread_.joinable()) {
        thread_.join();
    }

    LogInfo << "End to deinitialize MxpiBufferStablizer(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiBufferStablizer::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    APP_ERROR ret = APP_ERR_OK;
    LogDebug << "Begin to process MxpiBufferStablizer(" << elementName_ << ").";
    ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (dataSourceKeys_.size() < 1) {
        LogError << "Invalid dataSourceKeys_, size must not be equal to 0!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // refresh props
    ret = InitAndRefreshProps();
    if (ret != APP_ERR_OK) {
        LogError << "MxpiBufferStablizer init and refresh props failed." << GetErrorInfo(ret);
        return ret;
    }

    realCount_++;
    mtx_.lock();
    MxpiFrame inputMxpiFrame = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer[0]);
    frameId_ = inputMxpiFrame.frameinfo().frameid();
    channelId_ = inputMxpiFrame.frameinfo().channelid();
    LogDebug << "Current buffer with channelId(" << channelId_ << ") frameId(" << frameId_
             << ") is sent to next plugin in element(" << elementName_ << ").";
    MxpiMetadataManager manager(*mxpiBuffer[0]);
    if (!dataSourceKeys_[0].empty()) {
        auto lastMetadata = manager.GetMetadata(dataSourceKeys_[0]);
        if (lastMetadata != nullptr) {
            ret = manager.AddProtoMetadata(elementName_, lastMetadata);
            if (ret != APP_ERR_OK) {
                errorInfo_ << "Add proto metadata failed in Process." << GetErrorInfo(ret);
                SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
                mtx_.unlock();
                condition_.notify_all();
                return ret;
            }
        }
    }
    if (allowDelay_) {
        SendData(0, *mxpiBuffer[0]);
    } else {
        if (numSent_ > 0) {
            MxpiBufferManager::DestroyBuffer(mxpiBuffer[0]);
            numSent_--;
        } else {
            SendData(0, *mxpiBuffer[0]);
        }
    }
    mtx_.unlock();
    condition_.notify_all();
    LogDebug << "End to process MxpiBufferStablizer(" << elementName_ << ").";
    return ret;
}

std::vector<std::shared_ptr<void>> MxpiBufferStablizer::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto tolerance = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
            UINT, "tolerance", "tolerance", "tolerance time for input buffer, unit:ms", 10000, 1, UINT32_MAX
    });
    auto frequency = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
            UINT, "frequency", "frequency", "time interval of auto buffer after tolerance, unit:ms", 100, 1, UINT32_MAX
    });
    auto allowDelay = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
            UINT, "allowDelay", "allowDelay", "delayed buffer is allowed to be sent, 0:false, 1:true", 0, 0, 1
    });
    properties = { tolerance, frequency, allowDelay };
    return properties;
}

MxpiPortInfo MxpiBufferStablizer::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiBufferStablizer::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

APP_ERROR MxpiBufferStablizer::InitAndRefreshProps()
{
    ConfigParamLock();
    std::vector<std::string> parameterNamesPtr = {"tolerance", "frequency", "allowDelay"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, *configParamMap_);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        ConfigParamUnlock();
        return ret;
    }
    tolerance_ = *std::static_pointer_cast<uint32_t>((*configParamMap_)["tolerance"]);
    frequency_ = *std::static_pointer_cast<uint32_t>((*configParamMap_)["frequency"]);
    allowDelay_ = *std::static_pointer_cast<bool>((*configParamMap_)["allowDelay"]);
    ConfigParamUnlock();
    return APP_ERR_OK;
}

void MxpiBufferStablizer::CreateThread()
{
    std::function<void()> timeCall = std::bind(&MxpiBufferStablizer::TimeCall, this);
    thread_ = std::thread(timeCall);
}

void MxpiBufferStablizer::TimeCall(void)
{
    MxBase::DeviceManager *deviceManager = MxBase::DeviceManager::GetInstance();
    MxBase::DeviceContext deviceContext;
    deviceContext.devId = deviceId_;
    while (runningFlag_) {
        std::unique_lock<std::mutex> lock(mtx_);
        condition_.wait(lock);
        lock.unlock();
        auto interval = std::min(MAX_INTERVAL, tolerance_ / CHECKFREQUENCY);
        uint64_t lastCount = realCount_;
        auto startTime = std::chrono::high_resolution_clock::now();
        while (runningFlag_) {
            usleep(interval);
            auto endTime = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration<double, std::milli>(endTime - startTime).count() <= tolerance_) {
                continue;
            }
            if (realCount_ == lastCount) {
                break;
            }
            lastCount = realCount_;
            startTime = std::chrono::high_resolution_clock::now();
        }

        while (realCount_ == lastCount && runningFlag_) {
            LogWarn << "Null buffer with channelId(" << channelId_ << ") frameId(" << frameId_
                    << ") is sent to next plugin in element(" << elementName_ << ").";
            usleep(static_cast<int>(frequency_ * SEC2MS));
            mtx_.lock();
            APP_ERROR ret = deviceManager->SetDevice(deviceContext);
            if (ret != APP_ERR_OK) {
                LogError << "Element(" << elementName_ << ") Failed to set deviceId."
                         << GetErrorInfo(ret);
            }
            numSent_++;
            InputParam bufferParam = {};
            bufferParam.key = "";
            bufferParam.deviceId = INIT_DEVICEID;
            bufferParam.dataSize = INIT_DATASIZE;
            MxpiFrameInfo *outputFrameInfo = &(bufferParam.mxpiFrameInfo);
            outputFrameInfo->set_frameid(++frameId_);
            outputFrameInfo->set_channelid(channelId_);
            MxpiBuffer* outputMxpiBuffer = MxpiBufferManager::CreateHostBuffer(bufferParam);
            if (outputMxpiBuffer == nullptr) {
                LogError << "Create null buffer failed." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
                mtx_.unlock();
                break;
            }
            mtx_.unlock();
            SendData(0, *outputMxpiBuffer);
        }
    }
}

namespace {
MX_PLUGIN_GENERATE(MxpiBufferStablizer)
}

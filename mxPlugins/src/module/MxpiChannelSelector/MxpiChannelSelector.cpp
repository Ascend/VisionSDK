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
 * Description: Transparently transmits the buffer of the specified channel ID.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiChannelSelector/MxpiChannelSelector.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const int INIT_DEVICEID = -1;
const int INIT_DATASIZE = 1;
const int MAX_CHANNELID = 31;
}

APP_ERROR MxpiChannelSelector::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiChannelselector(" << elementName_ << ").";
    // get parameters from configParamMap.
    std::vector<std::string> parameterNamesPtr = {"channelIds"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    channelIdsString_ = *std::static_pointer_cast<std::string>(configParamMap["channelIds"]);
    channelIds_ = StringUtils::SplitAndCastToInt(channelIdsString_, ',');
    if (!IsValidChannelIds(channelIds_)) {
        LogError << "ChannelIdsString SplitAndCastToInt failed. Check your config."
                 << GetErrorInfo(APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR);
        return APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR;
    }
    configParamMap_ = &configParamMap;
    LogInfo << "End to initialize MxpiChannelselector(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelSelector::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiChannelselector(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiChannelselector(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiChannelSelector::RefreshChannelIds()
{
    APP_ERROR ret = APP_ERR_OK;
    ConfigParamLock();
    std::vector<std::string> parameterNamesPtr = {"channelIds"};
    ret = CheckConfigParamMapIsValid(parameterNamesPtr, *configParamMap_);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        ConfigParamUnlock();
        return ret;
    }
    std::string channelIdsString = *std::static_pointer_cast<std::string>((*configParamMap_)["channelIds"]);
    ConfigParamUnlock();
    if (channelIdsString_ != channelIdsString) {
        auto channelIds = StringUtils::SplitAndCastToInt(channelIdsString, ',');
        if (!IsValidChannelIds(channelIds)) {
            ret = APP_ERR_STREAM_INVALID_CONFIG;
            errorInfo_ << "The channelIdsStrin SplitAndCastToInt failed. Check your config.\
                        The channelIds_ will not be changed." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        } else {
            channelIds_ = channelIds;
            channelIdsString_ = channelIdsString;
        }
    }
    return APP_ERR_OK;
}

bool MxpiChannelSelector::IsValidChannelIds(const std::vector<int>& channelIds)
{
    if (channelIds.empty()) {
        LogError << "The channelIdsString SplitAndCastToInt failed. Check your config."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    std::set<int> validIds = {};
    for (auto id : channelIds) {
        if (id < 0 || id > MAX_CHANNELID) {
            LogError << "The channel id(" << id << ") must be 0 ~ 31. Check your config!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (validIds.find(id) != validIds.end()) {
            LogError << "The channelIds has repeated channel id(" << id << "). Check your config!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        } else {
            validIds.insert(id);
        }
    }
    return true;
}

APP_ERROR MxpiChannelSelector::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiChannelselector(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // 1.refresh channelIds;
    ret = RefreshChannelIds();
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        return ret;
    }

    MxpiFrame inputMxpiFrame = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer[0]);
    uint32_t channelId = inputMxpiFrame.frameinfo().channelid();

    auto iter = std::find(channelIds_.begin(), channelIds_.end(), channelId);
    if (iter != channelIds_.end()) {
        LogDebug << "Current buffer with channelId(" << channelId << ") is sent to next plugin.";
        MxpiMetadataManager manager(*mxpiBuffer[0]);
        if (dataSourceKeys_.size() < 1) {
            errorInfo_ << "Invalid dataSourceKeys_, size must not be equal to 0!"
                       << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            LogError << errorInfo_.str();
            SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto lastMetadata = manager.GetMetadata(dataSourceKeys_[0]);
        if (lastMetadata != nullptr) {
            ret = manager.AddProtoMetadata(elementName_, lastMetadata);
            if (ret != APP_ERR_OK) {
                errorInfo_ << "Add proto metadata failed in Process." << GetErrorInfo(ret);
                SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
                return ret;
            }
        }
        SendData(0, *mxpiBuffer[0]);
    } else {
        LogDebug << "Current buffer with channelId(" << channelId
                 << ") is filtered.";
        InputParam bufferParam = {};
        bufferParam.key = "";
        bufferParam.deviceId = INIT_DEVICEID;
        bufferParam.dataSize = INIT_DATASIZE;
        MxpiFrameInfo *outputFrameInfo = &(bufferParam.mxpiFrameInfo);
        outputFrameInfo->CopyFrom(inputMxpiFrame.frameinfo());
        MxpiBuffer* outputMxpiBuffer = MxpiBufferManager::CreateHostBuffer(bufferParam);
        if (outputMxpiBuffer == nullptr) {
            errorInfo_ << "Create device buffer failed." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            LogError << errorInfo_.str();
            SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, APP_ERR_COMM_INVALID_POINTER, errorInfo_.str());
            return APP_ERR_COMM_INVALID_POINTER;
        }
        MxpiBufferManager::DestroyBuffer(mxpiBuffer[0]);
        SendData(0, *outputMxpiBuffer);
    }
    LogDebug << "End to process MxpiChannelselector(" << elementName_ << ").";
    return ret;
}

std::vector<std::shared_ptr<void>> MxpiChannelSelector::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    std::shared_ptr<void> channelIds = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "channelIds", "channel id", "rule of distribute", "0", "NULL", "NULL"
    });
    properties = {channelIds};
    return properties;
}

MxpiPortInfo MxpiChannelSelector::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiChannelSelector::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

namespace {
MX_PLUGIN_GENERATE(MxpiChannelSelector)
}
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
 * Description: Inference flow management interface.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxStream/StreamManager/MxStreamManager.h"
#include <functional>
#include <cstdlib>
#include <unistd.h>
#include <gst/gst.h>
#include "acl/acl.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxStream/StreamManager/MxsmStream.h"
#include "MxBase/ConfigUtil/ConfigUtil.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsLog.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"
#include "MxStream/Packet/DataTransform.h"
#include "MxStreamManagerDptr.h"

using MxTools::PSE2ELog;
using MxTools::PSPluginLog;
using MxTools::PSTPRLog;
using MxTools::PSQueueLog;

namespace {
static int g_initCounts = 0;
static std::mutex g_threadsMutex;
static std::timed_mutex g_createSingleStreamMutex;
const int MAX_INIT_COUNTS = 1024;
const size_t MAX_PIPELINE_STRING = 10 * 1024 * 1024;
const size_t MAX_PIPELINE_SIZE = 256;
static bool g_isInited = false;
static bool g_isDestroyed = false;
}

namespace MxStream {
MxStreamManager::MxStreamManager()
{
    /*
     * The constructor needs to throw an exception if it fails for the std::make_shared function.
     * Don't use MxBase::MemoryHelper::MakeShared function
     */
    dPtr_ = std::make_shared<MxStream::MxStreamManagerDptr>(this);
}

MxStreamManager::~MxStreamManager()
{
    LogDebug << "MxStreamManager destroy.";
    // destroy management threads if applicable
    if (!dPtr_->isThreadsDestroyed_) {
        DestroyManagementThreads();
    }

    MxBase::Log::UpdateFileMode();
    LogDebug << "log rotate thread exits successful.";
}

void MxStreamManager::DestroyManagementThreads()
{
    std::unique_lock<std::mutex> threadManagementLock(g_threadsMutex);
    if (g_initCounts >= 0) {
        g_initCounts--;
    }
    if (g_initCounts == 0 && g_isDestroyed == false) {
        g_isDestroyed = true;
        LogInfo << "Begin to destroy management threads.";
        dPtr_->DestroyManagementThreads();
    }
}

APP_ERROR MxStreamManager::InitManager(const std::vector<std::string>& argStrings)
{
    std::unique_lock<std::mutex> threadManagementLock(g_threadsMutex);
    if (g_initCounts == 0 && g_isInited == false) {
        g_isInited = true;
        if (MxBase::Log::Init() != APP_ERR_OK) {
            LogError << "Failed to initialize log." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
        }
        for (size_t i = 0; i < argStrings.size(); i++) {
            if (MxBase::StringUtils::HasInvalidChar(argStrings[i])) {
                LogError << "InitManager: argStrings[" << i << "] contains invalid char, please check.";
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        APP_ERROR ret = dPtr_->HandleSDKEnv();
        if (ret != APP_ERR_OK) {
            LogError << "Before creating a pipeline, please set related environment variables.\n"
                     << "The following two methods are available:\n"
                     << "(1) Permanent method: set the environment variable in the ~/.bashrc file of "
                     << "the current user, and run the \"source ~/.bashrc\" command manually in the current window.\n"
                     << "(2) Temporary method: run the export command to import the required "
                     << "environment variables in the current window." << GetErrorInfo(ret);
            return ret;
        }
        ret = dPtr_->ParseSDKConfig(true);
        if (ret != APP_ERR_OK) {
            LogWarn << GetErrorInfo(ret) << "Failed to load configuration.";
        }
        PSE2ELog.Init(dPtr_->cfgPSLogDir_, dPtr_->cfgPSLogFileName_, dPtr_->cfgPSMaxLogSize_);
        PSPluginLog.Init(dPtr_->cfgPSLogDir_, dPtr_->cfgPSLogFileName_, dPtr_->cfgPSMaxLogSize_);
        PSTPRLog.Init(dPtr_->cfgPSLogDir_, dPtr_->cfgPSLogFileName_, dPtr_->cfgPSMaxLogSize_);
        PSQueueLog.Init(dPtr_->cfgPSLogDir_, dPtr_->cfgPSLogFileName_, dPtr_->cfgPSMaxLogSize_);
        ret = dPtr_->CreateManagementThreads();
        if (ret != APP_ERR_OK) {
            LogError << "Failed to create management threads." << GetErrorInfo(ret);
            return ret;
        }
        dPtr_->GstreamerInit(argStrings);
    }
    if (!dPtr_->isInitialized_ && g_initCounts >= 0 && g_initCounts < MAX_INIT_COUNTS) {
        g_initCounts++;
    }
    dPtr_->isInitialized_ = true;
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->InitDevices();
    if (ret != APP_ERR_OK) {
        LogError << "Initialize devices failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR MxStreamManager::InitManager(const MxBase::AppGlobalCfgExtra &globalCfgExtra,
                                       const std::vector<std::string>& argStrings)
{
    if (MxBase::DeviceManager::IsAscend310() || MxBase::DeviceManager::IsAscend310B()) {
        return InitManager(argStrings);
    } else if (!MxBase::DeviceManager::IsAscend310P()) {
        LogError << "MxStreamManager is only supported on device 310/310B/310P now, current device is "
                 << MxBase::DeviceManager::GetSocName() << ".";
        return APP_ERR_COMM_INIT_FAIL;
    }

    std::unique_lock<std::mutex> threadManagementLock(g_threadsMutex);
    if (g_initCounts == 0 && g_isInited == false) {
        g_isInited = true;
        if (MxBase::MxInit(globalCfgExtra) != APP_ERR_OK) {
            LogError << "Failed to initialize resource." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
        }
        for (size_t i = 0; i < argStrings.size(); i++) {
            if (MxBase::StringUtils::HasInvalidChar(argStrings[i])) {
                LogError << "InitManager: argStrings[" << i << "] contains invalid char, please check.";
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        APP_ERROR ret = dPtr_->HandleSDKEnv();
        if (ret != APP_ERR_OK) {
            LogError << "Before creating a pipeline, please set related environment variables.\n"
                     << "The following two methods are available:\n"
                     << "(1) Permanent method: set the environment variable in the ~/.bashrc file of "
                     << "the current user, and run the \"source ~/.bashrc\" command manually in the current window.\n"
                     << "(2) Temporary method: run the export command to import the required "
                     << "environment variables in the current window." << GetErrorInfo(ret);
            return ret;
        }
        ret = dPtr_->ParseSDKConfig(true);
        if (ret != APP_ERR_OK) {
            LogWarn << GetErrorInfo(ret) << "Failed to load configuration.";
        }
        PSE2ELog.Init(dPtr_->cfgPSLogDir_, dPtr_->cfgPSLogFileName_, dPtr_->cfgPSMaxLogSize_);
        PSPluginLog.Init(dPtr_->cfgPSLogDir_, dPtr_->cfgPSLogFileName_, dPtr_->cfgPSMaxLogSize_);
        PSTPRLog.Init(dPtr_->cfgPSLogDir_, dPtr_->cfgPSLogFileName_, dPtr_->cfgPSMaxLogSize_);
        PSQueueLog.Init(dPtr_->cfgPSLogDir_, dPtr_->cfgPSLogFileName_, dPtr_->cfgPSMaxLogSize_);
        ret = dPtr_->CreateManagementThreads();
        if (ret != APP_ERR_OK) {
            LogError << "Failed to create management threads." << GetErrorInfo(ret);
            return ret;
        }
        dPtr_->GstreamerInit(argStrings);
    }
    if (!dPtr_->isInitialized_ && g_initCounts >= 0 && g_initCounts < MAX_INIT_COUNTS) {
        g_initCounts++;
    }
    dPtr_->isInitialized_ = true;
    return APP_ERR_OK;
}

static APP_ERROR CheckStreamsConfig(const std::string& streamsConfig)
{
    size_t streamLength = streamsConfig.size();
    if (streamLength == 0 || streamLength >= MAX_PIPELINE_STRING) {
        LogError << "The streamsConfig is empty or too large." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxStreamManager::CreateMultipleStreams(const std::string& streamsConfig)
{
    if (CheckStreamsConfig(streamsConfig) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = APP_ERR_OK;
    if (!dPtr_->isInitialized_) {
        ret = InitManager();
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    std::unique_lock<std::timed_mutex> singleStreamLock(g_createSingleStreamMutex);
    // verify json string
    nlohmann::json streamsJson;
    try {
        streamsJson = nlohmann::json::parse(streamsConfig);
        if (!streamsJson.is_object() || streamsJson.empty()) {
            LogError << "Json value is empty or not an valid object, or json value is empty"
                     << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
            return APP_ERR_STREAM_INVALID_CONFIG;
        }
        if (MxBase::StringUtils::HasInvalidChar(streamsJson.dump())) {
            LogError << "Pipeline has invalid char." << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
            return APP_ERR_STREAM_INVALID_CONFIG;
        }
    } catch (const std::exception& ex) {
        LogError << "pipeline is not a valid json. Parse json value of stream failed. Error message: (" << ex.what()
                 << ")." << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
        return APP_ERR_STREAM_INVALID_CONFIG;
    }
    try {
        nlohmann::json streamObject = streamsJson;
        if (streamObject.size() > MAX_PIPELINE_SIZE) {
            LogError << "Too many pipelines in streamsConfig." << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
            return APP_ERR_STREAM_INVALID_CONFIG;
        }
        for (auto& iter : streamObject.items()) {
            ret = dPtr_->CreateSingleStream(iter.key(), iter.value());
            if (ret != APP_ERR_OK) {
                return ret;
            }
        }
    } catch (const std::exception& ex) {
        LogError << "Invalid stream config. Error message: (" << ex.what() << ")."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
        return APP_ERR_STREAM_INVALID_CONFIG;
    }

    MxTools::PerformanceStatisticsManager::GetInstance()->enablePs_ = dPtr_->cfgEnablePS_;
    MxTools::PerformanceStatisticsManager::GetInstance()->SetQueueSizeWarnPercent(dPtr_->cfgPSQueueSizeWarnPercent_);
    MxTools::PerformanceStatisticsManager::GetInstance()->SetQueueSizeTimes(dPtr_->cfgPSQueueSizeTimes_);
    LogInfo << "Creates streams successfully.";
    return APP_ERR_OK;
}

APP_ERROR MxStreamManager::CreateMultipleStreamsFromFile(const std::string& streamsFilePath)
{
    std::string streamsFileRealPath;
    if (!MxBase::FileUtils::RegularFilePath(streamsFilePath, streamsFileRealPath)) {
        LogError << "Failed to regular FilePath." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return APP_ERR_COMM_INVALID_PATH;
    }
    if (!MxBase::FileUtils::IsFileValid(streamsFileRealPath, false)) {
        LogError << "Invalid streamsFilePath, please check" << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::ifstream file(streamsFileRealPath.c_str(), std::ifstream::binary);
    if (!file) {
        LogError << "File is not exists." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    file.seekg(0, std::ifstream::end);
    uint32_t fileSize = static_cast<uint32_t>(file.tellg());
    if (!dPtr_->JudgePipelineFileSize(fileSize)) {
        LogError << "The pipeline file size exceed 100(M)." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        file.close();
        return APP_ERR_COMM_FAILURE;
    }
    file.seekg(0);
    std::unique_ptr<char[]> data(new (std::nothrow) char[fileSize + 1]);
    if (data == nullptr) {
        LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        file.close();
        return APP_ERR_COMM_INIT_FAIL;
    }
    file.read(data.get(), fileSize);
    file.close();
    std::string pipelineConfig(data.get(), fileSize);
    APP_ERROR ret = CreateMultipleStreams(pipelineConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to create Stream from streams file." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR MxStreamManager::StopStream(const std::string& streamName)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName)) {
        LogError << "StopStream: the streamName contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        return ret;
    }

    const auto& streamInstance = dPtr_->streamMap_[streamName];
    ret = streamInstance->DestroyStream();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to destroy stream:" << streamName << "." << GetErrorInfo(ret);
        return ret;
    }
    std::unique_lock<decltype(dPtr_->streamMapMutex_)> streamLock(dPtr_->streamMapMutex_);
    dPtr_->streamMap_.erase(streamName);
    return APP_ERR_OK;
}

APP_ERROR MxStreamManager::DestroyAllStreams()
{
    std::unique_lock<decltype(dPtr_->streamMapMutex_)> streamLock(dPtr_->streamMapMutex_);
    for (auto& iter : dPtr_->streamMap_) {
        APP_ERROR ret = iter.second->DestroyStream();
        if (ret != APP_ERR_OK) {
            LogError << "Failed to destroy stream:" << iter.first << "." << GetErrorInfo(ret);
            return ret;
        }
    }

    dPtr_->streamMap_.clear();

    // destroy management threads
    if (!dPtr_->isThreadsDestroyed_) {
        DestroyManagementThreads();
    }
    dPtr_->isThreadsDestroyed_ = true;

    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->DestroyDevices();
    if (ret != APP_ERR_OK) {
        LogError << "DeviceManager DestroyDevices failed." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "Destroy streams successfully.";
    return APP_ERR_OK;
}

APP_ERROR MxStreamManager::SendProtobuf(const std::string& streamName, int inPluginId,
    std::vector<MxstProtobufIn>& protoVec)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName)) {
        LogError << "SendProtobuf: the streamName contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < protoVec.size(); i++) {
        if (MxBase::StringUtils::HasInvalidChar(protoVec[i].key)) {
            LogError << "SendProtobuf: protoVec[" << i << "].key contains invalid char, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    if (inPluginId < 0) {
        LogError << "The inPluginId cannot be a negative number." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        return ret;
    }
    const auto& streamInstance = dPtr_->streamMap_[streamName];
    ret = streamInstance->SendProtobuf(inPluginId, protoVec);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to send data to input plugin(" << inPluginId << ")." << GetErrorInfo(ret);
        return ret;
    }

    LogDebug << "Sends data to stream(" << streamName.c_str() << ") and input(" << inPluginId << ") successfully.";
    return APP_ERR_OK;
}

APP_ERROR MxStreamManager::SendProtobuf(const std::string& streamName, const std::string& elementName,
    std::vector<MxstProtobufIn>& protoVec)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName) || MxBase::StringUtils::HasInvalidChar(elementName)) {
        LogError << "SendProtobuf: the streamName or elementName contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < protoVec.size(); i++) {
        if (MxBase::StringUtils::HasInvalidChar(protoVec[i].key)) {
            LogError << "SendProtobuf: protoVec[" << i << "].key contains invalid char, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        return ret;
    }
    const auto& streamInstance = dPtr_->streamMap_[streamName];
    ret = streamInstance->SendProtobuf(elementName, protoVec);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to send data to input plugin(" << elementName << ")." << GetErrorInfo(ret);
        return ret;
    }

    LogDebug << "Sends data to stream(" << streamName.c_str() << ") and input(" << elementName << ") successfully.";
    return APP_ERR_OK;
}

APP_ERROR MxStreamManager::SendData(const std::string& streamName,
    const std::string& elementName, std::vector<MxstMetadataInput>& metadataVec,
    MxstBufferInput& bufferInput)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName) || MxBase::StringUtils::HasInvalidChar(elementName)) {
        LogError << "SendData: the streamName or elementName contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < metadataVec.size(); i++) {
        if (MxBase::StringUtils::HasInvalidChar(metadataVec[i].dataSource)) {
            LogError << "SendData: metadataVec["<< i <<"].dataSource contains invalid char, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        return ret;
    }
    const auto& streamInstance = dPtr_->streamMap_[streamName];
    ret = streamInstance->SendData(elementName, metadataVec, bufferInput);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to SendProtoAndBuffer to input plugin(" << elementName << ")."
                 << GetErrorInfo(ret);
        return ret;
    }

    LogDebug << "SendProtoAndBuffer to stream(" << streamName.c_str()
        << ") and input(" << elementName << ") successfully.";
    return APP_ERR_OK;
}

APP_ERROR MxStreamManager::SendData(const std::string& streamName, int inPluginId, MxstDataInput& dataBuffer)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName) ||
        MxBase::StringUtils::HasInvalidChar(dataBuffer.serviceInfo.customParam)) {
        LogError << "SendData: the streamName or dataBuffer contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (inPluginId < 0) {
        LogError << "The inPluginId cannot be a negative number." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        return ret;
    }

    const auto& streamInstance = dPtr_->streamMap_[streamName];
    ret = streamInstance->SendData(inPluginId, dataBuffer);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to send data to input plugin(" << inPluginId << ")." << GetErrorInfo(ret);
        return ret;
    }

    LogDebug << "Sends data to stream(" << streamName.c_str() << ") and input(" << inPluginId << ") successfully.";
    return APP_ERR_OK;
}

APP_ERROR MxStreamManager::SendData(const std::string& streamName,
    const std::string& elementName, MxstDataInput& dataBuffer)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName) || MxBase::StringUtils::HasInvalidChar(elementName)
        || MxBase::StringUtils::HasInvalidChar(dataBuffer.serviceInfo.customParam)) {
        LogError << "SendData: the streamName or elementName or dataBuffer contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        return ret;
    }

    const auto& streamInstance = dPtr_->streamMap_[streamName];
    ret = streamInstance->SendData(elementName, dataBuffer);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to send data to input plugin(" << elementName << ")." << GetErrorInfo(ret);
        return ret;
    }

    LogDebug << "Sends data to stream(" << streamName.c_str() << ") and input(" << elementName << ") successfully.";
    return APP_ERR_OK;
}

MxstBufferAndMetadataOutput MxStreamManager::GetResult(const std::string& streamName, const std::string& elementName,
    const std::vector<std::string>& dataSourceVec, const uint32_t& msTimeOut)
{
    MxstBufferAndMetadataOutput bufferAndMetaOut;
    bufferAndMetaOut.metadataVec.clear();
    if (MxBase::StringUtils::HasInvalidChar(streamName) || MxBase::StringUtils::HasInvalidChar(elementName)) {
        std::string errMsg = "GetResult: the streamName or elementName contains invalid char, please check.";
        LogError << errMsg << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        bufferAndMetaOut.SetErrorInfo(APP_ERR_COMM_INVALID_PARAM, errMsg);
        return bufferAndMetaOut;
    }
    for (size_t i = 0; i < dataSourceVec.size(); i++) {
        if (MxBase::StringUtils::HasInvalidChar(dataSourceVec[i])) {
            LogError << "GetResult: dataSourceVec[" << i << "] contains invalid char, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return bufferAndMetaOut;
        }
    }
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        std::string errorMsg = "Stream(" + streamName + ") not exist.";
        LogError << errorMsg << GetErrorInfo(ret);
        bufferAndMetaOut.SetErrorInfo(APP_ERR_STREAM_NOT_EXIST, errorMsg);
        return bufferAndMetaOut;
    }
    if (dataSourceVec.empty()) {
        std::string errorMsg = GetErrorInfo(APP_ERR_COMM_INVALID_PARAM) + "keyVec size is 0.";
    }
    const auto& streamInstance = dPtr_->streamMap_[streamName];
    bufferAndMetaOut = streamInstance->GetResult(elementName, dataSourceVec, msTimeOut);
    return bufferAndMetaOut;
}

std::vector<MxstProtobufOut> MxStreamManager::GetProtobuf(const std::string& streamName, int outPluginId,
    const std::vector<std::string>& keyVec)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName)) {
        LogError << "GetProtobuf: the streamName contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return {};
    }
    std::vector<MxstProtobufOut> dataBufferVec;
    dataBufferVec.clear();
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        MxstProtobufOut outPut(APP_ERR_STREAM_NOT_EXIST);
        dataBufferVec.push_back(outPut);
        return dataBufferVec;
    }
    if (outPluginId < 0) {
        LogError << "outPluginId is negative number." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        MxstProtobufOut outPut(APP_ERR_COMM_INVALID_PARAM);
        dataBufferVec.push_back(outPut);
        return dataBufferVec;
    }
    if (keyVec.empty()) {
        LogError << "keyVec size is 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        MxstProtobufOut outPut(APP_ERR_COMM_INVALID_PARAM);
        dataBufferVec.push_back(outPut);
        return dataBufferVec;
    }
    for (size_t i = 0; i < keyVec.size(); i++) {
        if (MxBase::StringUtils::HasInvalidChar(keyVec[i])) {
            LogError << "GetProtobuf: keyVec[" << i << "] contains invalid char, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            MxstProtobufOut outPut(APP_ERR_COMM_INVALID_PARAM);
            dataBufferVec.push_back(outPut);
            return dataBufferVec;
        }
    }
    const auto& streamInstance = dPtr_->streamMap_[streamName];
    dataBufferVec = streamInstance->GetProtobuf(outPluginId, keyVec);
    return dataBufferVec;
}

MxstDataOutput* MxStreamManager::GetResult(const std::string& streamName, int outPluginId, const uint32_t& msTimeOut)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName)) {
        LogError << "GetResult: the streamName contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return nullptr;
    }
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        return dPtr_->SetErrorCode(APP_ERR_STREAM_NOT_EXIST, GetErrorInfo(ret));
    }
    if (outPluginId < 0) {
        LogError << "outPluginId is negative number." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return dPtr_->SetErrorCode(APP_ERR_COMM_INVALID_PARAM, GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    const auto& streamInstance = dPtr_->streamMap_[streamName];
    MxstDataOutput* dataBuffer = streamInstance->GetResult(outPluginId, msTimeOut);
    if (dataBuffer == nullptr) {
        LogError << "streamInstance GetResult return nullptr." << "stream(" << streamName << ")"
                 << GetErrorInfo(APP_ERR_COMM_INNER);
        return dPtr_->SetErrorCode(APP_ERR_COMM_INNER, GetErrorInfo(APP_ERR_COMM_INNER));
    }

    LogDebug << "Gets data from stream(" << streamName.c_str() << ") and output(" << outPluginId << ") successfully.";
    return dataBuffer;
}

APP_ERROR MxStreamManager::SendDataWithUniqueId(const std::string& streamName, int inPluginId,
    MxstDataInput& dataBuffer, uint64_t& uniqueId)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName) ||
        MxBase::StringUtils::HasInvalidChar(dataBuffer.serviceInfo.customParam)) {
        LogError << "SendDataWithUniqueId: the streamName or dataBuffer contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (inPluginId < 0) {
        LogError << "The inPluginId cannot be a negative number." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret) ;
        return ret;
    }

    const auto& streamInstance = dPtr_->streamMap_[streamName];
    ret = streamInstance->SendDataWithUniqueId(inPluginId, dataBuffer, uniqueId);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to send data to input plugin(" << inPluginId << ")." << GetErrorInfo(ret);
        return ret;
    }

    LogDebug << "Sends data to stream(" << streamName.c_str() << ") successfully: input plugin id(" <<
        inPluginId << ") " << "unique id(" << inPluginId << ").";
    return APP_ERR_OK;
}

// sendDataWithUniqueId support multi-input
APP_ERROR MxStreamManager::SendMultiDataWithUniqueId(const std::string& streamName, std::vector<int> inPluginIdVec,
    std::vector<MxstDataInput>& dataBufferVec, uint64_t& uniqueId)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName)) {
        LogError << "SendMultiDataWithUniqueId: the streamName contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < dataBufferVec.size(); i++) {
        if (MxBase::StringUtils::HasInvalidChar(dataBufferVec[i].serviceInfo.customParam)) {
            LogError << "SendMultiDataWithUniqueId: the dataBufferVec contains invalid char, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        return ret;
    }

    for (size_t i = 0; i < inPluginIdVec.size(); i++) {
        if (inPluginIdVec[i] < 0) {
            LogError << "InPluginIdVec[" << i << "] is " << inPluginIdVec[i]
                    << ", The inPluginId cannot be a negative number." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    const auto& streamInstance = dPtr_->streamMap_[streamName];
    ret = streamInstance->SendMultiDataWithUniqueId(inPluginIdVec, dataBufferVec, uniqueId);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to send data to input plugin." << GetErrorInfo(ret);
        return ret;
    }

    LogDebug << "Sends data to stream(" << streamName.c_str() << ") successfully.";
    return APP_ERR_OK;
}

APP_ERROR MxStreamManager::SendDataWithUniqueId(const std::string& streamName, const std::string& elementName,
    MxstDataInput& dataBuffer, uint64_t& uniqueId)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName) || MxBase::StringUtils::HasInvalidChar(elementName)
        || MxBase::StringUtils::HasInvalidChar(dataBuffer.serviceInfo.customParam)) {
        LogError << "SendDataWithUniqueId: the streamName or elementName or dataBuffer contains invalid char"
                 << ", please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        return ret;
    }

    const auto& streamInstance = dPtr_->streamMap_[streamName];
    ret = streamInstance->SendDataWithUniqueId(elementName, dataBuffer, uniqueId);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to send data to input plugin(" << elementName << ")." << GetErrorInfo(ret);
        return ret;
    }

    LogDebug << "Sends data to stream(" << streamName.c_str() << ") successfully: input plugin id(" <<
        elementName << ") " << "unique id(" << uniqueId << ").";
    return APP_ERR_OK;
}

MxstDataOutput* MxStreamManager::GetResultWithUniqueId(const std::string& streamName,
    uint64_t uniqueId, unsigned int timeOutInMs)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName)) {
        LogError << "GetResultWithUniqueId: the streamName contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return nullptr;
    }
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        return dPtr_->SetErrorCode(APP_ERR_STREAM_NOT_EXIST, GetErrorInfo(APP_ERR_STREAM_NOT_EXIST));
    }

    const auto& streamInstance1 = dPtr_->streamMap_[streamName];
    MxstDataOutput* dataBuffer = streamInstance1->GetResultWithUniqueId(uniqueId, timeOutInMs);
    if ((dataBuffer == nullptr)) {
        dataBuffer = new (std::nothrow) MxstDataOutput;
        if (dataBuffer == nullptr) {
            LogError << "Allocate memory with \"new MxstDataOutput\" failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return nullptr;
        }
        dataBuffer->errorCode = APP_ERR_STREAM_TIMEOUT;
        std::string errorInfo = "Time out, can not get result in time.";
        dataBuffer->dataSize = static_cast<int>(errorInfo.size());
        dataBuffer->dataPtr = (uint32_t *)malloc(dataBuffer->dataSize);
        if (dataBuffer->dataPtr == nullptr) {
            LogError << "Allocate memory with \"malloc function\" failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            delete dataBuffer;
            dataBuffer = nullptr;
            return nullptr;
        }
        std::copy(errorInfo.begin(), errorInfo.end(), (char *)dataBuffer->dataPtr);
        if (dPtr_->IsStreamExist(streamName) == APP_ERR_STREAM_NOT_EXIST) {
            LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(APP_ERR_STREAM_NOT_EXIST);
            delete dataBuffer;
            dataBuffer = nullptr;
            return dPtr_->SetErrorCode(APP_ERR_STREAM_NOT_EXIST, GetErrorInfo(APP_ERR_STREAM_NOT_EXIST));
        }
        const auto& streamInstance3 = dPtr_->streamMap_[streamName];
        streamInstance3->DropUniqueId(uniqueId);
        LogDebug << "data from stream(" << streamName.c_str() << ")  droped: unique id(" << uniqueId << ").";
    } else {
        LogDebug << "Gets data from stream(" << streamName.c_str() << ") successfully: unique id(" << uniqueId << ").";
    }

    return dataBuffer;
}

// getResultWithUniqueId with multi-output
std::vector<MxstDataOutput*> MxStreamManager::GetMultiResultWithUniqueId(const std::string& streamName,
    uint64_t uniqueId, unsigned int timeOutInMs)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName)) {
        LogError << "GetMultiResultWithUniqueId: the streamName contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return {};
    }
    std::vector<MxstDataOutput*> errorVec;
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        errorVec.push_back(dPtr_->SetErrorCode(APP_ERR_STREAM_NOT_EXIST, GetErrorInfo(APP_ERR_STREAM_NOT_EXIST)));
        return errorVec;
    }

    const auto& streamInstance1 = dPtr_->streamMap_[streamName];
    auto dataBufferVec = streamInstance1->GetMultiResultWithUniqueId(uniqueId, timeOutInMs);
    for (size_t i = 0; i < dataBufferVec.size(); i++) {
        auto dataBuffer = dataBufferVec[i];
        if ((dataBuffer == nullptr)) {
            dataBuffer = new (std::nothrow) MxstDataOutput;
            if (dataBuffer == nullptr) {
                LogError << "Allocate memory with \"new MxstDataOutput\" failed. return a vector with size 0"
                         << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return errorVec;
            }
            dataBuffer->errorCode = APP_ERR_STREAM_TIMEOUT;
            std::string errorInfo = "Time out, can not get result in time.";
            dataBuffer->dataSize = static_cast<int>(errorInfo.size());
            dataBuffer->dataPtr = (uint32_t *)malloc(dataBuffer->dataSize);
            if (dataBuffer->dataPtr == nullptr) {
                LogError << "Allocate memory with \"malloc function\" failed. return a vector with size 0"
                         << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                delete dataBuffer;
                dataBuffer = nullptr;
                return errorVec;
            }
            std::copy(errorInfo.begin(), errorInfo.end(), (char *)dataBuffer->dataPtr);
            if (dPtr_->IsStreamExist(streamName) == APP_ERR_STREAM_NOT_EXIST) {
                LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(APP_ERR_STREAM_NOT_EXIST);
                delete dataBuffer;
                dataBuffer = nullptr;
                dataBufferVec.push_back(dPtr_->SetErrorCode(APP_ERR_STREAM_NOT_EXIST,
                    GetErrorInfo(APP_ERR_STREAM_NOT_EXIST)));
            }
            const auto& streamInstance3 = dPtr_->streamMap_[streamName];
            streamInstance3->DropUniqueId(uniqueId);
            LogDebug << "data from stream(" << streamName.c_str() << ")  droped: unique id(" << uniqueId << ").";
        } else {
            LogDebug << "Gets data from stream(" << streamName.c_str() << ") successfully: unique id(" <<
                uniqueId << ").";
        }
    }

    return dataBufferVec;
}

APP_ERROR MxStreamManager::SetElementProperty(const std::string& streamName, const std::string& elementName,
                                              const std::string& propertyName, const std::string& propertyValue)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName) || MxBase::StringUtils::HasInvalidChar(elementName) ||
        MxBase::StringUtils::HasInvalidChar(propertyName) || MxBase::StringUtils::HasInvalidChar(propertyValue)) {
        LogError << "SetElementProperty: the input param contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dPtr_->streamMap_.find(streamName) == dPtr_->streamMap_.end()) {
        LogError << "StreamName: \"" << streamName << "\" not found." << GetErrorInfo(APP_ERR_STREAM_NOT_EXIST);
        return APP_ERR_STREAM_NOT_EXIST;
    }
    auto ret = dPtr_->streamMap_[streamName]->
            SetElementProperty(elementName, propertyName, propertyValue);
    if (ret != APP_ERR_OK) {
        LogError << "streamName: \"" << streamName << "\" elementName: \"" << elementName
                 << "\" propertyName: \"" << propertyName << "\" propertyValue: \"" << propertyValue
                 << "\" set property failed." << GetErrorInfo(ret);
    }
    return ret;
}

std::shared_ptr<MxstDataOutput> MxStreamManager::GetResultSP(const std::string& streamName, int outPluginId,
    const uint32_t& msTimeOut)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName)) {
        LogError << "GetResultSP: the streamName contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return nullptr;
    }

    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        return std::shared_ptr<MxstDataOutput>(dPtr_->SetErrorCode(APP_ERR_STREAM_NOT_EXIST, GetErrorInfo(ret)));
    }
    if (outPluginId < 0) {
        LogError << "outPluginId is negative number." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return std::shared_ptr<MxstDataOutput>(dPtr_->SetErrorCode(APP_ERR_COMM_INVALID_PARAM,
            GetErrorInfo(APP_ERR_COMM_INVALID_PARAM)));
    }
    const auto& streamInstance = dPtr_->streamMap_[streamName];
    std::shared_ptr<MxstDataOutput> dataBuffer = streamInstance->GetResultSP(outPluginId, msTimeOut);
    if (dataBuffer == nullptr) {
        LogError << "streamInstance GetResult return nullptr. " << "stream(" << streamName << ")"
                 << GetErrorInfo(APP_ERR_COMM_INNER);
        return std::shared_ptr<MxstDataOutput>(dPtr_->SetErrorCode(APP_ERR_COMM_INNER,
            GetErrorInfo(APP_ERR_COMM_INNER)));
    }

    LogDebug << "Gets data from stream(" << streamName.c_str() << ") and output(" << outPluginId << ") successfully.";
    return dataBuffer;
}

std::shared_ptr<MxstDataOutput> MxStreamManager::GetResultWithUniqueIdSP(const std::string& streamName,
    uint64_t uniqueId, uint32_t timeOutMs)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName)) {
        LogError << "GetResultWithUniqueIdSP: the streamName contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return nullptr;
    }
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        return std::shared_ptr<MxstDataOutput>(dPtr_->SetErrorCode(APP_ERR_STREAM_NOT_EXIST,
            GetErrorInfo(APP_ERR_STREAM_NOT_EXIST)));
    }

    const auto& streamInstance1 = dPtr_->streamMap_[streamName];
    std::shared_ptr<MxstDataOutput> dataBuffer = streamInstance1->GetResultWithUniqueIdSP(uniqueId, timeOutMs);
    if ((dataBuffer == nullptr)) {
        dataBuffer = MxBase::MemoryHelper::MakeShared<MxstDataOutput>();
        if (dataBuffer == nullptr) {
            LogError << "Allocate memory with \"new MxstDataOutput\" failed."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return std::shared_ptr<MxstDataOutput>();
        }
        dataBuffer->errorCode = APP_ERR_STREAM_TIMEOUT;
        std::string errorInfo = "Time out, can not get result in time.";
        dataBuffer->dataSize = static_cast<int>(errorInfo.size());
        dataBuffer->dataPtr = (uint32_t *)malloc(dataBuffer->dataSize);
        if (dataBuffer->dataPtr == nullptr) {
            LogError << "Allocate memory with \"malloc function\" failed."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return std::shared_ptr<MxstDataOutput>();
        }
        std::copy(errorInfo.begin(), errorInfo.end(), (char *)dataBuffer->dataPtr);
        if (dPtr_->IsStreamExist(streamName) == APP_ERR_STREAM_NOT_EXIST) {
            LogError << "stream(" << streamName << ") not exist." << GetErrorInfo(APP_ERR_STREAM_NOT_EXIST);
            return std::shared_ptr<MxstDataOutput>(dPtr_->SetErrorCode(APP_ERR_STREAM_NOT_EXIST,
                GetErrorInfo(APP_ERR_STREAM_NOT_EXIST)));
        }
        const auto& streamInstance3 = dPtr_->streamMap_[streamName];
        streamInstance3->DropUniqueId(uniqueId);
        LogDebug << "data from stream(" << streamName.c_str() << ")  droped: unique id(" << uniqueId << ").";
    } else {
        LogDebug << "Gets data from stream(" << streamName.c_str() << ") successfully: unique id(" << uniqueId << ").";
    }

    return dataBuffer;
}

std::vector<std::shared_ptr<MxstDataOutput>> MxStreamManager::GetMultiResultWithUniqueIdSP(
    const std::string& streamName,
    uint64_t uniqueId, uint32_t timeOutMs)
{
    if (MxBase::StringUtils::HasInvalidChar(streamName)) {
        LogError << "GetMultiResultWithUniqueIdSP: the streamName contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return {};
    }
    std::vector<std::shared_ptr<MxstDataOutput>> errorVec;
    APP_ERROR ret = dPtr_->IsStreamExist(streamName);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "stream(" << streamName << ") not exist." << GetErrorInfo(ret);
        errorVec.push_back(std::shared_ptr<MxstDataOutput>(dPtr_->SetErrorCode(APP_ERR_STREAM_NOT_EXIST,
            GetErrorInfo(APP_ERR_STREAM_NOT_EXIST))));
        return errorVec;
    }

    const auto& streamInstance1 = dPtr_->streamMap_[streamName];
    std::vector<std::shared_ptr<MxstDataOutput>> dataBufferVec =
        streamInstance1->GetMultiResultWithUniqueIdSP(uniqueId, timeOutMs);
    for (size_t i = 0; i < dataBufferVec.size(); i++) {
        auto dataBuffer = dataBufferVec[i];
        if ((dataBuffer == nullptr)) {
            dataBuffer = MxBase::MemoryHelper::MakeShared<MxstDataOutput>();
            if (dataBuffer == nullptr) {
                LogError << "Allocate memory with \"new MxstDataOutput\" failed. return a vector with size 0"
                         << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return errorVec;
            }
            dataBuffer->errorCode = APP_ERR_STREAM_TIMEOUT;
            std::string errorInfo = "Time out, can not get result in time.";
            dataBuffer->dataSize = static_cast<int>(errorInfo.size());
            dataBuffer->dataPtr = (uint32_t *)malloc(dataBuffer->dataSize);
            if (dataBuffer->dataPtr == nullptr) {
                LogError << "Allocate memory with \"malloc function\" failed. return a vector with size 0"
                         << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return errorVec;
            }
            std::copy(errorInfo.begin(), errorInfo.end(), (char *)dataBuffer->dataPtr);
            if (dPtr_->IsStreamExist(streamName) == APP_ERR_STREAM_NOT_EXIST) {
                LogError << "stream(" << streamName << ") not exist." << GetErrorInfo(APP_ERR_STREAM_NOT_EXIST);
                dataBufferVec.push_back(std::shared_ptr<MxstDataOutput>(dPtr_->SetErrorCode(APP_ERR_STREAM_NOT_EXIST,
                    GetErrorInfo(APP_ERR_STREAM_NOT_EXIST))));
            }
            const auto& streamInstance3 = dPtr_->streamMap_[streamName];
            streamInstance3->DropUniqueId(uniqueId);
            LogDebug << "data from stream(" << streamName.c_str() << ")  droped: unique id(" << uniqueId << ").";
        } else {
            LogDebug << "Gets data from stream(" << streamName.c_str() << ") successfully: unique id(" <<
                uniqueId << ").";
        }
    }

    return dataBufferVec;
}

template<>
APP_ERROR MxStreamManager::SendPacket(Packet<MxstDataInput>& packet, const std::string& streamName,
    const std::string& elementName)
{
    MxstDataInput dataBuffer = packet.GetItem();
    return SendData(streamName, elementName, dataBuffer);
}

template<>
APP_ERROR MxStreamManager::SendPacket(Packet<MxClassList>& packet, const std::string& streamName,
    const std::string& elementName)
{
    std::shared_ptr<google::protobuf::Message> messagePtr;
    MxClassList mxClassList = packet.GetItem();
    messagePtr = std::make_shared<MxTools::MxpiClassList>();
    APP_ERROR ret = ClassListTransform(mxClassList, messagePtr);
    if (ret != APP_ERR_OK) {
        LogError << "SendPacket: ClassList transform failed, please check."
                 << GetErrorInfo(ret);
        return ret;
    }
    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = elementName;
    dataBuffer.messagePtr = messagePtr;
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    return SendProtobuf(streamName, elementName, dataBufferVec);
}

template<>
APP_ERROR MxStreamManager::SendPacket(Packet<MxImageMaskList>& packet, const std::string& streamName,
    const std::string& elementName)
{
    std::shared_ptr<google::protobuf::Message> messagePtr;
    MxImageMaskList mxImageMaskList = packet.GetItem();
    messagePtr = std::make_shared<MxTools::MxpiImageMaskList>();
    APP_ERROR ret = ImageMaskListTransform(mxImageMaskList, messagePtr);
    if (ret != APP_ERR_OK) {
        LogError << "SendPacket: ImageMaskList transform failed, please check."
                 << GetErrorInfo(ret);
        return ret;
    }
    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = elementName;
    dataBuffer.messagePtr = messagePtr;
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    return SendProtobuf(streamName, elementName, dataBufferVec);
}

template<>
APP_ERROR MxStreamManager::SendPacket(Packet<MxObjectList>& packet, const std::string& streamName,
    const std::string& elementName)
{
    std::shared_ptr<google::protobuf::Message> messagePtr;
    MxObjectList mxObjectList = packet.GetItem();
    messagePtr = std::make_shared<MxTools::MxpiObjectList>();
    APP_ERROR ret = ObjectListTransform(mxObjectList, messagePtr);
    if (ret != APP_ERR_OK) {
        LogError << "SendPacket: ObjectList transform failed, please check."
                 << GetErrorInfo(ret);
        return ret;
    }
    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = elementName;
    dataBuffer.messagePtr = messagePtr;
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    return SendProtobuf(streamName, elementName, dataBufferVec);
}

template<>
APP_ERROR MxStreamManager::SendPacket(Packet<MxTensorPackageList>& packet, const std::string& streamName,
    const std::string& elementName)
{
    std::shared_ptr<google::protobuf::Message> messagePtr;
    MxTensorPackageList mxTensorPackageList = packet.GetItem();
    messagePtr = std::make_shared<MxTools::MxpiTensorPackageList>();
    APP_ERROR ret = TensorPackageListTransform(mxTensorPackageList, messagePtr);
    if (ret != APP_ERR_OK) {
        LogError << "SendPacket: TensorPackageList transform failed, please check."
                 << GetErrorInfo(ret);
        return ret;
    }
    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = elementName;
    dataBuffer.messagePtr = messagePtr;
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    return SendProtobuf(streamName, elementName, dataBufferVec);
}

template<>
APP_ERROR MxStreamManager::SendPacket(Packet<MxPoseList>& packet, const std::string& streamName,
    const std::string& elementName)
{
    std::shared_ptr<google::protobuf::Message> messagePtr;
    MxPoseList mxPoseList = packet.GetItem();
    messagePtr = std::make_shared<MxTools::MxpiPoseList>();
    APP_ERROR ret = PoseListTransform(mxPoseList, messagePtr);
    if (ret != APP_ERR_OK) {
        LogError << "SendPacket: PoseList transform failed, please check."
                 << GetErrorInfo(ret);
        return ret;
    }
    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = elementName;
    dataBuffer.messagePtr = messagePtr;
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    return SendProtobuf(streamName, elementName, dataBufferVec);
}

template<>
APP_ERROR MxStreamManager::SendPacket(Packet<MxTextObjectList>& packet, const std::string& streamName,
    const std::string& elementName)
{
    std::shared_ptr<google::protobuf::Message> messagePtr;
    MxTextObjectList mxTextObjectList = packet.GetItem();
    messagePtr = std::make_shared<MxTools::MxpiTextObjectList>();
    APP_ERROR ret = TextObjectListTransform(mxTextObjectList, messagePtr);
    if (ret != APP_ERR_OK) {
        LogError << "SendPacket: TextObjectList transform failed, please check."
                 << GetErrorInfo(ret);
        return ret;
    }
    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = elementName;
    dataBuffer.messagePtr = messagePtr;
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    return SendProtobuf(streamName, elementName, dataBufferVec);
}

template<>
APP_ERROR MxStreamManager::SendPacket(Packet<MxTextsInfoList>& packet, const std::string& streamName,
    const std::string& elementName)
{
    std::shared_ptr<google::protobuf::Message> messagePtr;
    MxTextsInfoList mxTextsInfoList = packet.GetItem();
    messagePtr = std::make_shared<MxTools::MxpiTextsInfoList>();
    APP_ERROR ret = TextsInfoListTransform(mxTextsInfoList, messagePtr);
    if (ret != APP_ERR_OK) {
        LogError << "SendPacket: TextObjectList transform failed, please check."
                 << GetErrorInfo(ret);
        return ret;
    }
    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = elementName;
    dataBuffer.messagePtr = messagePtr;
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    return SendProtobuf(streamName, elementName, dataBufferVec);
}

template<>
APP_ERROR MxStreamManager::SendPacket(Packet<MxVisionList>& packet, const std::string& streamName,
    const std::string& elementName)
{
    std::shared_ptr<google::protobuf::Message> messagePtr;
    MxVisionList mxVisionList = packet.GetItem();
    messagePtr = std::make_shared<MxTools::MxpiVisionList>();
    APP_ERROR ret = VisionListTransform(mxVisionList, messagePtr);
    if (ret != APP_ERR_OK) {
        LogError << "SendPacket: VisionList transform failed, please check."
                 << GetErrorInfo(ret);
        return ret;
    }
    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = elementName;
    dataBuffer.messagePtr = messagePtr;
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    return SendProtobuf(streamName, elementName, dataBufferVec);
}

template<>
APP_ERROR MxStreamManager::GetPacket(Packet<std::shared_ptr<MxstBufferOutput>>& packet, const std::string& streamName,
    const std::string& outElement, const uint32_t& msTimeOut)
{
    std::vector<std::string> dataSourceVec = {};
    if (outElement.find("appsink") == std::string::npos) {
        LogError << "Invalid outElement, please set it appsink plugin." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxstBufferAndMetadataOutput dataOutput = GetResult(streamName, outElement, dataSourceVec, msTimeOut);

    packet.SetItem(dataOutput.bufferOutput);

    return APP_ERR_OK;
}

template<>
APP_ERROR MxStreamManager::GetPacket(Packet<std::shared_ptr<MxVisionList>>& packet, const std::string& streamName,
    const std::string& outElement, const uint32_t& msTimeOut)
{
    std::vector<std::string> dataSourceVec = {outElement};
    if (outElement.find("appsink") != std::string::npos) {
        LogError << "Invalid outElement, please don't set it appsink plugin."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxstBufferAndMetadataOutput dataOutput = GetResult(streamName, "appsink0", dataSourceVec, msTimeOut);
    if (dataOutput.metadataVec.size() != 0x1) {
        LogError << "Don't find metaData." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    auto metaPtr = dataOutput.metadataVec[0x0].dataPtr;

    std::shared_ptr<MxVisionList> packetPtr = std::make_shared<MxVisionList>();
    packet.SetItem(packetPtr);
    return Message2Packet<MxVisionList>(metaPtr, packet);
}

template<>
APP_ERROR MxStreamManager::GetPacket(Packet<std::shared_ptr<MxClassList>>& packet, const std::string& streamName,
    const std::string& outElement, const uint32_t& msTimeOut)
{
    std::vector<std::string> dataSourceVec = {outElement};
    if (outElement.find("appsink") != std::string::npos) {
        LogError << "Invalid outElement, please don't set it appsink plugin"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxstBufferAndMetadataOutput dataOutput = GetResult(streamName, "appsink0", dataSourceVec, msTimeOut);
    if (dataOutput.metadataVec.size() != 0x1) {
        LogError << "Don't find metaData." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    auto metaPtr = dataOutput.metadataVec[0x0].dataPtr;

    std::shared_ptr<MxClassList> packetPtr = std::make_shared<MxClassList>();
    packet.SetItem(packetPtr);
    return Message2Packet<MxClassList>(metaPtr, packet);
}

template<>
APP_ERROR MxStreamManager::GetPacket(Packet<std::shared_ptr<MxImageMaskList>>& packet, const std::string& streamName,
    const std::string& outElement, const uint32_t& msTimeOut)
{
    std::vector<std::string> dataSourceVec = {outElement};
    if (outElement.find("appsink") != std::string::npos) {
        LogError << "Invalid outElement, please don't set it appsink plugin"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxstBufferAndMetadataOutput dataOutput = GetResult(streamName, "appsink0", dataSourceVec, msTimeOut);
    if (dataOutput.metadataVec.size() != 0x1) {
        LogError << "Don't find metaData." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    auto metaPtr = dataOutput.metadataVec[0x0].dataPtr;

    std::shared_ptr<MxImageMaskList> packetPtr = std::make_shared<MxImageMaskList>();
    packet.SetItem(packetPtr);
    return Message2Packet<MxImageMaskList>(metaPtr, packet);
}

template<>
APP_ERROR MxStreamManager::GetPacket(Packet<std::shared_ptr<MxObjectList>>& packet, const std::string& streamName,
    const std::string& outElement, const uint32_t& msTimeOut)
{
    std::vector<std::string> dataSourceVec = {outElement};
    if (outElement.find("appsink") != std::string::npos) {
        LogError << "Invalid outElement, please don't set it appsink plugin."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxstBufferAndMetadataOutput dataOutput = GetResult(streamName, "appsink0", dataSourceVec, msTimeOut);
    if (dataOutput.metadataVec.size() != 0x1) {
        LogError << "Don't find metaData." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    auto metaPtr = dataOutput.metadataVec[0x0].dataPtr;

    std::shared_ptr<MxObjectList> packetPtr = std::make_shared<MxObjectList>();
    packet.SetItem(packetPtr);
    return Message2Packet<MxObjectList>(metaPtr, packet);
}

template<>
APP_ERROR MxStreamManager::GetPacket(Packet<std::shared_ptr<MxTensorPackageList>>& packet,
    const std::string& streamName, const std::string& outElement, const uint32_t& msTimeOut)
{
    std::vector<std::string> dataSourceVec = {outElement};
    if (outElement.find("appsink") != std::string::npos) {
        LogError << "Invalid outElement, please don't set it appsink plugin."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxstBufferAndMetadataOutput dataOutput = GetResult(streamName, "appsink0", dataSourceVec, msTimeOut);
    if (dataOutput.metadataVec.size() != 0x1) {
        LogError << "Don't find metaData." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    auto metaPtr = dataOutput.metadataVec[0x0].dataPtr;

    std::shared_ptr<MxTensorPackageList> packetPtr = std::make_shared<MxTensorPackageList>();
    packet.SetItem(packetPtr);
    return Message2Packet<MxTensorPackageList>(metaPtr, packet);
}

template<>
APP_ERROR MxStreamManager::GetPacket(Packet<std::shared_ptr<MxPoseList>>& packet, const std::string& streamName,
    const std::string& outElement, const uint32_t& msTimeOut)
{
    std::vector<std::string> dataSourceVec = {outElement};
    if (outElement.find("appsink") != std::string::npos) {
        LogError << "Invalid outElement, please don't set it appsink plugin."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxstBufferAndMetadataOutput dataOutput = GetResult(streamName, "appsink0", dataSourceVec, msTimeOut);
    if (dataOutput.metadataVec.size() != 0x1) {
        LogError << "Don't find metaData." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    auto metaPtr = dataOutput.metadataVec[0x0].dataPtr;

    std::shared_ptr<MxPoseList> packetPtr = std::make_shared<MxPoseList>();
    packet.SetItem(packetPtr);
    return Message2Packet<MxPoseList>(metaPtr, packet);
}

template<>
APP_ERROR MxStreamManager::GetPacket(Packet<std::shared_ptr<MxTextObjectList>>& packet, const std::string& streamName,
    const std::string& outElement, const uint32_t& msTimeOut)
{
    std::vector<std::string> dataSourceVec = {outElement};
    if (outElement.find("appsink") != std::string::npos) {
        LogError << "Invalid outElement, please don't set it appsink plugin."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxstBufferAndMetadataOutput dataOutput = GetResult(streamName, "appsink0", dataSourceVec, msTimeOut);
    if (dataOutput.metadataVec.size() != 0x1) {
        LogError << "Don't find metaData." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    auto metaPtr = dataOutput.metadataVec[0x0].dataPtr;

    std::shared_ptr<MxTextObjectList> packetPtr = std::make_shared<MxTextObjectList>();
    packet.SetItem(packetPtr);
    return Message2Packet<MxTextObjectList>(metaPtr, packet);
}

template<>
APP_ERROR MxStreamManager::GetPacket(Packet<std::shared_ptr<MxTextsInfoList>>& packet, const std::string& streamName,
    const std::string& outElement, const uint32_t& msTimeOut)
{
    std::vector<std::string> dataSourceVec = {outElement};
    if (outElement.find("appsink") != std::string::npos) {
        LogError << "Invalid outElement, please don't set it appsink plugin."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    MxstBufferAndMetadataOutput dataOutput = GetResult(streamName, "appsink0", dataSourceVec, msTimeOut);
    if (dataOutput.metadataVec.size() != 0x1) {
        LogError << "Don't find metaData." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    auto metaPtr = dataOutput.metadataVec[0x0].dataPtr;

    std::shared_ptr<MxTextsInfoList> packetPtr = std::make_shared<MxTextsInfoList>();
    packet.SetItem(packetPtr);
    return Message2Packet<MxTextsInfoList>(metaPtr, packet);
}
// initialize the static variables here
std::atomic<bool> MxStreamManager::rotateTimeFlag_(true);
std::atomic<bool> MxStreamManager::rotateNumberFlag_(true);
std::atomic<bool> MxStreamManager::performanceStatisticsFlag_(true);
std::atomic<bool> MxStreamManager::dynamicFlag_(true);
}  // namespace MxStream
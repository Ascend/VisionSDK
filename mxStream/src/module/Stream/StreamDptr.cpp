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
 * Description: Stream private interface for internal use only.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "StreamDptr.h"
#include "MxStream/StreamManager/MxsmStream.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxsmDescription.h"
#include "MxStreamManagerDptr.h"

namespace  {
    static bool g_isStreamManagerInit = false;
    static std::mutex g_threadsMutex;
    static std::mutex g_initMutex;
} /* namespace  */

namespace MxStream {
StreamDptr::StreamDptr(Stream* qPtr): qPtr_(qPtr)
{
}

StreamDptr::~StreamDptr()
{
    LogDebug << "StreamDptr destroy.";
    if (!isStop_) {
        Stop();
    }
    DestroyThreads();
}

APP_ERROR StreamDptr::SendData(const std::string& elementName, std::vector<MxstMetadataInput>& metadataVec,
    MxstBufferInput& dataBuffer)
{
    return mxStreamManager_->SendData(streamName_, elementName, metadataVec, dataBuffer);
}

APP_ERROR StreamDptr::SendMultiDataWithUniqueId(std::vector<int> inPluginIdVec,
    std::vector<MxstDataInput>& dataInputVec,
    uint64_t& uniqueId)
{
    return mxStreamManager_->SendMultiDataWithUniqueId(streamName_, inPluginIdVec, dataInputVec, uniqueId);
}

MxstBufferAndMetadataOutput StreamDptr::GetResult(const std::string& elementName,
    const std::vector<std::string>& dataSourceVec, const uint32_t& msTimeOut)
{
    return mxStreamManager_->GetResult(streamName_, elementName, dataSourceVec, msTimeOut);
}

std::vector<MxstDataOutput*> StreamDptr::GetMultiResultWithUniqueId(uint64_t uniqueId,
    uint32_t timeOutMs)
{
    return mxStreamManager_->GetMultiResultWithUniqueId(streamName_, uniqueId, timeOutMs);
}

APP_ERROR StreamDptr::SetElementProperty(const std::string& elementName, const std::string& propertyName,
    const std::string& propertyValue)
{
    return mxStreamManager_->SetElementProperty(streamName_, elementName, propertyName, propertyValue);
}

APP_ERROR StreamDptr::BaseBuild()
{
    streamName_ = qPtr_->mxsmDescription_->GetStreamName();
    if (MxBase::StringUtils::HasInvalidChar(streamName_)) {
        MxBase::StringUtils::ReplaceInvalidChar(streamName_);
        LogError << "The streamName contains invalid char, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = Init();
    if (ret != APP_ERR_OK) {
        LogError << "Build stream(" << streamName_ << ") failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = CreateThreads();
    if (ret != APP_ERR_OK) {
        LogError << "Build stream(" << streamName_ << ") failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = mxStreamManager_->dPtr_->CreateSingleStream(qPtr_->mxsmDescription_);
    if (ret != APP_ERR_OK) {
        LogError << "Build stream(" << streamName_ << ") failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR StreamDptr::Stop()
{
    isStop_ = true;
    APP_ERROR ret = mxStreamManager_->dPtr_->IsStreamExist(streamName_);
    if (ret == APP_ERR_STREAM_NOT_EXIST) {
        LogError << "Stream(" << streamName_ << ") not exist." << GetErrorInfo(ret);
        return ret;
    }
    ret = mxStreamManager_->StopStream(streamName_);
    if (ret != APP_ERR_OK) {
        LogError << "Stop stream(" << streamName_ << ") failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR StreamDptr::Init()
{
    std::unique_lock<std::mutex> taskLock(g_initMutex);
    if (mxStreamManager_ != nullptr && !g_isStreamManagerInit) {
        APP_ERROR ret = mxStreamManager_->InitManager();
        if (ret != APP_ERR_OK) {
            LogError << "Init stream manager failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return ret;
        }
        g_isStreamManagerInit = true;
    }
    return APP_ERR_OK;
}

APP_ERROR StreamDptr::CreateThreads()
{
    std::unique_lock<std::mutex> taskLock(g_threadsMutex);
    if (mxStreamManager_->dPtr_->isThreadsDestroyed_) {
        APP_ERROR ret = mxStreamManager_->dPtr_->CreateManagementThreads();
        if (ret != APP_ERR_OK) {
            LogError << "Create threads failed." << GetErrorInfo(ret);
            return ret;
        }
        mxStreamManager_->dPtr_->isThreadsDestroyed_ = false;
    }
    return APP_ERR_OK;
}

void StreamDptr::DestroyThreads()
{
    std::unique_lock<std::mutex> taskLock(g_threadsMutex);
    if (mxStreamManager_->dPtr_->streamMap_.empty() && !mxStreamManager_->dPtr_->isThreadsDestroyed_) {
        mxStreamManager_->dPtr_->DestroyManagementThreads();
        mxStreamManager_->dPtr_->isThreadsDestroyed_ = true;
    }
}

std::shared_ptr<MxStreamManager> StreamDptr::mxStreamManager_ = std::make_shared<MxStreamManager>();
}
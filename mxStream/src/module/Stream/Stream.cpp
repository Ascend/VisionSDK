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
 * Description: Determines and parses the plug-in attributes set in the pipeline.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "MxStream/Stream/Stream.h"
#include "MxStream/StreamManager/MxsmStream.h"
#include "MxStream/StreamManager/MxsmDescription.h"
#include "StreamDptr.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Utils/StringUtils.h"

namespace MxStream {
Stream::Stream(const std::string& pipelinePath)
{
    mxsmDescription_ = std::make_shared<MxsmDescription>(pipelinePath);
    dPtr_ = std::make_shared<StreamDptr>(this);
}

Stream::Stream(const std::string& pipelinePath, const std::string& streamName)
{
    mxsmDescription_ = std::make_shared<MxsmDescription>(pipelinePath, streamName);

    dPtr_ = std::make_shared<StreamDptr>(this);
}

Stream::~Stream()
{
    LogDebug << "Stream destroy.";
    MxBase::Log::UpdateFileMode();
}

APP_ERROR Stream::SendData(const std::string& elementName, std::vector<MxstMetadataInput>& metadataVec,
    MxstBufferInput& dataBuffer)
{
    return dPtr_->SendData(elementName, metadataVec, dataBuffer);
}

APP_ERROR Stream::SendMultiDataWithUniqueId(std::vector<int> inPluginIdVec,
    std::vector<MxstDataInput>& dataInputVec,
    uint64_t& uniqueId)
{
    return dPtr_->SendMultiDataWithUniqueId(inPluginIdVec, dataInputVec, uniqueId);
}

MxstBufferAndMetadataOutput Stream::GetResult(const std::string& elementName,
    const std::vector<std::string>& dataSourceVec, const uint32_t& msTimeOut)
{
    return dPtr_->GetResult(elementName, dataSourceVec, msTimeOut);
}

std::vector<MxstDataOutput*> Stream::GetMultiResultWithUniqueId(uint64_t uniqueId, uint32_t timeOutMs)
{
    return dPtr_->GetMultiResultWithUniqueId(uniqueId, timeOutMs);
}

APP_ERROR Stream::SetElementProperty(const std::string& elementName, const std::string& propertyName,
    const std::string& propertyValue)
{
    return dPtr_->SetElementProperty(elementName, propertyName, propertyValue);
}

void Stream::SetDeviceId(const std::string& deviceId)
{
    mxsmDescription_->streamObject_[STREAM_CONFIG_KEY][STREAM_CONFIG_DEVICEID] = deviceId;
}

std::string Stream::ToJson() const
{
    return mxsmDescription_->streamObject_.dump();
}

APP_ERROR Stream::Stop()
{
    return dPtr_->Stop();
}

APP_ERROR Stream::Start()
{
    return dPtr_->BaseBuild();
}

APP_ERROR Stream::Build()
{
    LogError << "The member function 'Build()' of Stream is unsupported." << GetErrorInfo(APP_ERR_COMM_FAILURE);
    return APP_ERR_COMM_FAILURE;
}
}
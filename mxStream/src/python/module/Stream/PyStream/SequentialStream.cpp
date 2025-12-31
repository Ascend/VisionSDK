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
 * Description: convert c++ SequentialStream to python class.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "Stream/PyStream/SequentialStream.h"
#include "Stream/PyStream/StreamUtils.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxStream/Stream/SequentialStream.h"
namespace PyStream {
SequentialStream::SequentialStream(const std::string &name)
{
    stream_ = MxBase::MemoryHelper::MakeShared<MxStream::SequentialStream>(name);
    if (stream_ == nullptr) {
        LogError << "Create SequentialStream object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

void SequentialStream::add(const PluginNode &pluginNode)
{
    auto nodePtr = pluginNode.GetNode();
    if (nodePtr == nullptr) {
        LogError << "PluginNode object ptr is null." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_POINTER));
    }
    auto ret = ((MxStream::SequentialStream *)stream_.get())->Add(*nodePtr);
    if (ret != APP_ERR_OK) {
        LogError << "Add node failed." << GetErrorInfo(ret);
        throw std::runtime_error("Add node failed.");
    }
}
void SequentialStream::build()
{
    Build(stream_);
}
void SequentialStream::stop()
{
    Stop(stream_);
}
void SequentialStream::set_device_id(const std::string &deviceId)
{
    SetDeviceId(stream_, deviceId);
}
void SequentialStream::send(
    const std::string &elementName, const std::vector<MetadataInput> &metadataVec, const BufferInput &dataBuffer)
{
    SendData(stream_, elementName, metadataVec, dataBuffer);
}
void SequentialStream::send_multi_data_with_unique_id(
    int inPluginNum, std::vector<MxDataInput> &dataInputVec, uint64_t &uniqueId)
{
    SendMultiDataWithUniqueId(stream_, inPluginNum, dataInputVec, uniqueId);
}
DataOutput SequentialStream::get_result(
    const std::string &elementName, const std::vector<std::string> &dataSourceVec, const int &msTimeOut)
{
    return GetResult(stream_, elementName, dataSourceVec, msTimeOut);
}
std::string SequentialStream::to_json() const
{
    return ToJson(stream_);
}
std::vector<MxDataOutput> SequentialStream::get_multi_result_with_unique_id(uint64_t uniqueId, const int &timeOutMs)
{
    return GetMultiResultWithUniqueId(stream_, uniqueId, timeOutMs);
}
}  // namespace PyStream
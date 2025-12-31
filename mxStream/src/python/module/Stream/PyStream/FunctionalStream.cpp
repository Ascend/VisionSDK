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
 * Description: convert c++ FunctionalStream to python class.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "Stream/PyStream/FunctionalStream.h"
#include "Stream/PyStream/StreamUtils.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxStream/Stream/FunctionalStream.h"
#include "MxStream/Stream/PluginNode.h"

#include "PyUtils/PyDataHelper.h"
namespace PyStream {
FunctionalStream::FunctionalStream(const std::string &name)
{
    stream_ = MxBase::MemoryHelper::MakeShared<MxStream::FunctionalStream>(name);
    if (stream_ == nullptr) {
        LogError << "Create FunctionalStream object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

FunctionalStream::FunctionalStream(
    const std::string &name, const std::vector<PluginNode> &inputs, const std::vector<PluginNode> &outputs)
{
    std::vector<MxStream::PluginNode> inputsTmp;
    std::vector<MxStream::PluginNode> outputsTmp;
    for (auto &node : inputs) {
        auto nodePtr = node.GetNode();
        if (nodePtr == nullptr) {
            LogError << "PluginNode object ptr is null." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_POINTER));
        }
        inputsTmp.emplace_back(*node.GetNode());
    }
    for (auto &node : outputs) {
        outputsTmp.emplace_back(*node.GetNode());
    }

    stream_ = MxBase::MemoryHelper::MakeShared<MxStream::FunctionalStream>(name, inputsTmp, outputsTmp);
    if (stream_ == nullptr) {
        LogError << "Create FunctionalStream object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

void FunctionalStream::build()
{
    Build(stream_);
}
void FunctionalStream::stop()
{
    Stop(stream_);
}
void FunctionalStream::set_device_id(const std::string &deviceId)
{
    SetDeviceId(stream_, deviceId);
}
void FunctionalStream::send(
    const std::string &elementName, const std::vector<MetadataInput> &metadataVec, const BufferInput &dataBuffer)
{
    SendData(stream_, elementName, metadataVec, dataBuffer);
}
void FunctionalStream::send_multi_data_with_unique_id(
    int inPluginNum, std::vector<MxDataInput> &dataInputVec, uint64_t &uniqueId)
{
    SendMultiDataWithUniqueId(stream_, inPluginNum, dataInputVec, uniqueId);
}
DataOutput FunctionalStream::get_result(
    const std::string &elementName, const std::vector<std::string> &dataSourceVec, const int &msTimeOut)
{
    return GetResult(stream_, elementName, dataSourceVec, msTimeOut);
}
std::string FunctionalStream::to_json() const
{
    return ToJson(stream_);
}
std::vector<MxDataOutput> FunctionalStream::get_multi_result_with_unique_id(uint64_t uniqueId, const int &timeOutMs)
{
    return GetMultiResultWithUniqueId(stream_, uniqueId, timeOutMs);
}
}  // namespace PyStream
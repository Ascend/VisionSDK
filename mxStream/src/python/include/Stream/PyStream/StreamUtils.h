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
 * Description: convert c++ Stream to python class.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef STREAM_UTILS_H
#define STREAM_UTILS_H

#include <iostream>
#include <string>
#include <memory>

#include "MxStream/Stream/Stream.h"
#include "Stream/PyDataType/PyDataType.h"

namespace PyStream {
void Build(const std::shared_ptr<MxStream::Stream> &stream);
void Stop(const std::shared_ptr<MxStream::Stream> &stream);
void SetDeviceId(const std::shared_ptr<MxStream::Stream> &stream, const std::string &deviceId);
void SendData(const std::shared_ptr<MxStream::Stream> &stream, const std::string &elementName,
    const std::vector<MetadataInput> &metadataVec, const BufferInput &dataBuffer);
void SendMultiDataWithUniqueId(const std::shared_ptr<MxStream::Stream> &stream, int inPluginNum,
    std::vector<MxDataInput> &dataInputVec, uint64_t &uniqueId);
DataOutput GetResult(const std::shared_ptr<MxStream::Stream> &stream, const std::string &elementName,
    const std::vector<std::string> &dataSourceVec, const int &msTimeOut = MxStream::DELAY_TIME);
std::string ToJson(const std::shared_ptr<MxStream::Stream> &stream);
std::vector<MxDataOutput> GetMultiResultWithUniqueId(
    const std::shared_ptr<MxStream::Stream> &stream, uint64_t uniqueId, unsigned int timeOutMs);
}  // namespace PyStream
#endif

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

#ifndef SEQUENTIAL_STREAM_H
#define SEQUENTIAL_STREAM_H

#include "MxStream/Stream/Stream.h"
#include "Stream/PyPluginNode/PluginNode.h"
#include "Stream/PyDataType/PyDataType.h"

namespace PyStream {
class SequentialStream {
public:
    explicit SequentialStream(const std::string &name);
    void build();
    void stop();
    void set_device_id(const std::string &deviceId);
    void send(
        const std::string &elementName, const std::vector<MetadataInput> &metadataVec, const BufferInput &dataBuffer);
    void send_multi_data_with_unique_id(int inPluginNum, std::vector<MxDataInput> &dataInputVec, uint64_t &uniqueId);
    DataOutput get_result(const std::string &elementName, const std::vector<std::string> &dataSourceVec,
        const int &msTimeOut = MxStream::DELAY_TIME);
    std::string to_json() const;
    std::vector<MxDataOutput> get_multi_result_with_unique_id(
        uint64_t uniqueId, const int &timeOutMs = MxStream::DELAY_TIME);
    void add(const PluginNode &pluginNode);

private:
    SequentialStream() = delete;
    SequentialStream(const SequentialStream &) = delete;
    SequentialStream(const SequentialStream &&) = delete;
    SequentialStream &operator=(const SequentialStream &) = delete;
    SequentialStream &operator=(const SequentialStream &&) = delete;
    std::shared_ptr<MxStream::Stream> stream_ = nullptr;
};
}  // namespace PyStream
#endif

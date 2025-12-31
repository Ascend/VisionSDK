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
 * Description: Determines the plug-in set in the pipeline and creates an inference flow.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MXSTREAM_STREAM_H
#define MXSTREAM_STREAM_H

#include <memory>
#include <vector>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxStream/DataType/DataType.h"
#include "MxStream/DataType/StateInfo.h"
#include "MxStream/Stream/PluginNode.h"
#include "MxStream/StreamManager/MxsmDataType.h"

namespace MxStream {
class MxsmStream;
class StreamDptr;
class MxsmDescription;
class Stream {
public:
    explicit Stream(const std::string& pipelinePath);
    Stream(const std::string& pipelinePath, const std::string& streamName);
    virtual ~Stream();

    APP_ERROR SendData(const std::string& elementName, std::vector<MxstMetadataInput>& metadataVec,
        MxstBufferInput& dataBuffer);
    MxstBufferAndMetadataOutput GetResult(const std::string& elementName,
        const std::vector<std::string>& dataSourceVec, const uint32_t& msTimeOut = DELAY_TIME);
    APP_ERROR SendMultiDataWithUniqueId(std::vector<int> inPluginIdVec,
        std::vector<MxstDataInput>& dataInputVec,
        uint64_t& uniqueId);
    std::vector<MxstDataOutput*> GetMultiResultWithUniqueId(uint64_t uniqueId, uint32_t timeOutMs = DELAY_TIME);
    APP_ERROR SetElementProperty(const std::string& elementName, const std::string& propertyName,
        const std::string& propertyValue);
    void SetDeviceId(const std::string& deviceId);
    std::string ToJson() const;

    virtual APP_ERROR Build();
    APP_ERROR Start();
    APP_ERROR Stop();

protected:
    std::shared_ptr<StreamDptr> dPtr_ = nullptr;
    std::shared_ptr<MxsmDescription> mxsmDescription_ = nullptr;

private:
    Stream() = delete;
    Stream(const Stream &) = delete;
    Stream(const Stream &&) = delete;
    Stream& operator=(const Stream &) = delete;
    Stream& operator=(const Stream &&) = delete;
    friend class StreamDptr;
};
}
#endif // MXSTREAM_STREAM_H
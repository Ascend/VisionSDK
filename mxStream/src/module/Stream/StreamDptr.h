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

#ifndef MXSM_STREAM_DPTR_H
#define MXSM_STREAM_DPTR_H

#include <memory>
#include <vector>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxStream/DataType/DataType.h"
#include "MxStream/Stream/PluginNode.h"
#include "MxStream/StreamManager/MxsmDataType.h"
#include "MxStream/Stream/Stream.h"
#include "MxBase/Common/HiddenAttr.h"
#include "MxStream/StreamManager/MxStreamManager.h"

namespace MxStream {
class SDK_UNAVAILABLE_FOR_OTHER StreamDptr {
public:
    explicit StreamDptr(Stream *);
    StreamDptr(const StreamDptr &) = delete;
    StreamDptr& operator=(const StreamDptr &) = delete;
    ~StreamDptr();

    APP_ERROR SendData(const std::string& elementName, std::vector<MxstMetadataInput>& metadataVec,
        MxstBufferInput& dataBuffer);
    MxstBufferAndMetadataOutput GetResult(const std::string& elementName,
        const std::vector<std::string>& dataSourceVec, const uint32_t& msTimeOut = DELAY_TIME);
    APP_ERROR SendMultiDataWithUniqueId(std::vector<int> inPluginIdVec,
        std::vector<MxstDataInput>& dataInputVec,
        uint64_t& uniqueId);
    std::vector<MxstDataOutput*> GetMultiResultWithUniqueId(uint64_t uniqueId, uint32_t timeOutMs);
    APP_ERROR SetElementProperty(const std::string& elementName, const std::string& propertyName,
        const std::string& propertyValue);

    APP_ERROR BaseBuild();
    APP_ERROR Stop();

public:
    Stream *qPtr_ = nullptr;
    std::string streamName_;
    static std::shared_ptr<MxStreamManager> mxStreamManager_;
    bool isStop_ = false;

private:
    APP_ERROR CreateThreads();
    void DestroyThreads();
    APP_ERROR Init();

private:
    StreamDptr() = delete;
    StreamDptr(const StreamDptr &&) = delete;
    StreamDptr& operator=(const StreamDptr &&) = delete;
};
}
#endif // MXSM_STREAM_DPTR_H
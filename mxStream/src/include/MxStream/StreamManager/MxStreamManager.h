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

#ifndef MX_STREAM_MANAGER_H
#define MX_STREAM_MANAGER_H

#include <map>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "MxStream/StreamManager/MxsmDataType.h"
#include "MxBase/Common/HiddenAttr.h"
#include "MxStream/Packet/Packet.h"
#include "MxStream/Packet/PacketDataType.h"

namespace MxStream {
class MxsmStream;

/* *
 * @description: manages the lifetime and the datasource of Streams
 */
class MxStreamManagerDptr;
class SDK_AVAILABLE_FOR_OUT MxStreamManager {
public:
    MxStreamManager();
    ~MxStreamManager();
    /* *
     * @description: Stream manager initialization
     * @param argStrings: value of input arguments
     * @return: APP_ERROR
     */
    APP_ERROR InitManager(const std::vector<std::string>& argStrings = std::vector<std::string>());

    /* *
     * @description: Stream manager initialization
     * @param globalCfg: AppGlobalCfg struct
     * @param argStrings: value of input arguments
     * @return: APP_ERROR
     */
    APP_ERROR InitManager(const MxBase::AppGlobalCfgExtra &globalCfgExtra,
                          const std::vector<std::string>& argStrings = std::vector<std::string>());

    /* *
     * @description: create and run Streams from config stream
     * @param StreamsConfig: number of input arguments
     * @return: APP_ERROR
     */
    APP_ERROR CreateMultipleStreams(const std::string& streamsConfig);
    /* *
     * @description: stop and destroy specified Stream
     * @param: streamName: the name of the target stream
     * @return: APP_ERROR
     */
    APP_ERROR StopStream(const std::string& streamName);
    /* *
     * @description: stop and destroy all Streams
     * @param: void
     * @return: APP_ERROR
     */
    APP_ERROR DestroyAllStreams();
    /* *
     * @description: send data to the input plugin of the Stream
     * @param StreamName: the name of the target Stream
     * @param inPluginId: the index of the input plugin
     * @param dataBuffer: the databuffer to be sent
     * @return: APP_ERROR
     */
    APP_ERROR SendData(const std::string& streamName, int inPluginId, MxstDataInput& dataBuffer);
    APP_ERROR SendData(const std::string& streamName, const std::string& elementName, MxstDataInput& dataBuffer);
    /* *
     * @description: get result from the output plugin of the Stream
     * @param StreamName: the name of the target Stream
     * @param outPluginId: the index of the output plugin
     * @return: APP_ERROR
     */
    MxstDataOutput* GetResult(const std::string& streamName, int outPluginId, const uint32_t& msTimeOut = DELAY_TIME);
    /* *
     * @description: send data with unique Id to the input plugin of the Stream
     * @param inPluginId: the index of the input plugin
     * @param dataBuffer: the data to be sent
     * @return: APP_ERROR
    */
    APP_ERROR SendDataWithUniqueId(const std::string& streamName, const std::string& elementName,
        MxstDataInput& dataBuffer, uint64_t& uniqueId);
    APP_ERROR SendDataWithUniqueId(const std::string& streamName, int inPluginId,
        MxstDataInput& dataBuffer, uint64_t& uniqueId);
    APP_ERROR SendMultiDataWithUniqueId(const std::string& streamName, std::vector<int> inPluginIdVec,
        std::vector<MxstDataInput>& dataBufferVec, uint64_t& uniqueId);
    /* *
     * @description: get result from the output plugin of the Stream
     * @param outPluginId: the index of the output plugin
     * @return: MxstDataOutput
     */
    MxstDataOutput* GetResultWithUniqueId(const std::string& streamName, uint64_t uniqueId,
        unsigned int timeOutInMs = DELAY_TIME);
    std::vector<MxstDataOutput*> GetMultiResultWithUniqueId(const std::string& streamName, uint64_t uniqueId,
        unsigned int timeOutInMs = DELAY_TIME);
    /* *
     * @description: create and run Streams from stream config file
     * @param streamsFilePath: stream config file
     * @return: APP_ERROR
     */
    APP_ERROR CreateMultipleStreamsFromFile(const std::string& streamsFilePath);
    /* *
     * @description: send protobuf vector to the input plugin of the Stream
     * @param StreamName: the name of the target Stream
     * @param inPluginId: the index of the input plugin
     * @param protoVec: the protobuf vector to be sent
     * @return: APP_ERROR
     */
    APP_ERROR SendProtobuf(const std::string& streamName, const std::string& elementName,
        std::vector<MxstProtobufIn>& protoVec);
    APP_ERROR SendProtobuf(const std::string& streamName, int inPluginId,
        std::vector<MxstProtobufIn>& protoVec);
    std::vector<MxstProtobufOut> GetProtobuf(const std::string& streamName, int outPluginId,
        const std::vector<std::string>& keyVec);
    APP_ERROR SendData(const std::string& streamName,
        const std::string& elementName, std::vector<MxstMetadataInput>& metadataVec,
        MxstBufferInput& bufferInput);
    MxstBufferAndMetadataOutput GetResult(const std::string& streamName, const std::string& elementName,
        const std::vector<std::string>& dataSourceVec, const uint32_t& msTimeOut = DELAY_TIME);
    APP_ERROR SetElementProperty(const std::string& streamName, const std::string& elementName,
                                 const std::string& propertyName, const std::string& propertyValue);
    std::shared_ptr<MxstDataOutput> GetResultSP(const std::string& streamName, int outPluginId,
        const uint32_t& msTimeOut = DELAY_TIME);
    std::shared_ptr<MxstDataOutput> GetResultWithUniqueIdSP(const std::string& streamName,
        uint64_t uniqueId, uint32_t timeOutMs = DELAY_TIME);
    std::vector<std::shared_ptr<MxstDataOutput>> GetMultiResultWithUniqueIdSP(const std::string& streamName,
        uint64_t uniqueId, uint32_t timeOutMs = DELAY_TIME);

    template <class T>
    APP_ERROR SendPacket(Packet<T>& packet, const std::string& streamName, const std::string& elementName);

    template <class T>
    APP_ERROR GetPacket(Packet<T>& packet, const std::string& streamName, const std::string& outElement,
        const uint32_t& msTimeOut = DELAY_TIME);

public:
    static std::atomic<bool> rotateTimeFlag_;
    static std::atomic<bool> rotateNumberFlag_;
    static std::atomic<bool> dynamicFlag_;
    static std::atomic<bool> performanceStatisticsFlag_;
private:
    APP_ERROR CreateManagementThreads();
    void DestroyManagementThreads();
    MxStreamManager(const MxStreamManager &) = delete;
    MxStreamManager(const MxStreamManager &&) = delete;
    MxStreamManager& operator=(const MxStreamManager &) = delete;
    MxStreamManager& operator=(const MxStreamManager &&) = delete;
private:
    friend class MxStreamManagerDptr;
    friend class StreamDptr;
    std::shared_ptr<MxStream::MxStreamManagerDptr> dPtr_ = nullptr;
};
}  // end namespace MxStream

#endif
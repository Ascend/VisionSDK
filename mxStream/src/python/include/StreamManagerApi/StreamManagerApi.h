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
 * Description: Basic process management and external interfaces provided by the Python version.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MX_STREAM_MANAGER_API_H
#define MX_STREAM_MANAGER_API_H

#include <string>
#include <vector>
#include <memory>
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxStream/StreamManager/MxStreamManager.h"

const unsigned int DELAY_TIME_EX = 3000;

namespace PyStreamManager {
struct RoiBox {
    float x0;
    float y0;
    float x1;
    float y1;
};

struct MxDataInput {
    std::string data;
    int fragmentId;
    std::string customParam;
    std::vector<RoiBox> roiBoxs;
};

struct MxDataInputExt {
    std::string data;
    int fragmentId;
    std::string customParam;
    std::string mxpiFrameInfo;
    std::string mxpiVisionInfo;
    std::vector<RoiBox> roiBoxs;
};

struct MxBufferInput {
    std::string data;
    std::string mxpiFrameInfo;
    std::string mxpiVisionInfo;
};

struct MxDataOutput {
    int errorCode = 0;
    std::string data;
};

struct MxBufferOutput {
    int errorCode = 0;
    std::string errorMsg;
    std::string data;

    MxBufferOutput() = default;

    explicit MxBufferOutput(int errorCode, const std::string &errorMsg = "")
        : errorCode(errorCode), errorMsg(std::move(errorMsg))
    {}

    void SetErrorInfo(int errorCodeIn, const std::string &errorMsgIn)
    {
        errorCode = errorCodeIn;
        errorMsg = errorMsgIn;
    }
};

struct MxProtobufIn {
    std::string key;
    std::string type;
    std::string protobuf;
};

struct MxMetadataInput {
    std::string dataSource;
    std::string dataType;
    std::string serializedMetadata;
};

struct MxProtobufOut {
    int errorCode = 0;
    std::string messageName;
    std::string messageBuf;
};

struct MxMetadataOutput {
    int errorCode = 0;
    std::string errorMsg;
    std::string dataType;
    std::string serializedMetadata;

    MxMetadataOutput() = default;

    explicit MxMetadataOutput(int errorCode, const std::string &errorMsg = "")
        : errorCode(errorCode), errorMsg(std::move(errorMsg))
    {}

    void SetErrorInfo(int errorCodeIn, const std::string &errorMsgIn)
    {
        errorCode = errorCodeIn;
        errorMsg = errorMsgIn;
    }
};

struct MxBufferAndMetadataOutput {
    int errorCode = 0;
    std::string errorMsg;
    MxBufferOutput bufferOutput;
    std::vector<MxMetadataOutput> metadataVec = {};

    MxBufferAndMetadataOutput() = default;

    explicit MxBufferAndMetadataOutput(int errorCode, const std::string &errorMsg = "")
        : errorCode(errorCode), errorMsg(std::move(errorMsg))
    {}

    void SetErrorInfo(int errorCodeIn, const std::string &errorMsgIn)
    {
        errorCode = errorCodeIn;
        errorMsg = errorMsgIn;
    }
};

class StreamManagerApi {
public:
    StreamManagerApi();

    ~StreamManagerApi();

    /**
     * @description: Stream manager initialization
     * @param argStrings: value of input arguments
     * @return: 0-success, other-failure
     */
    int InitManager(const std::vector<std::string> &argStrings = std::vector<std::string>());

    /**
     * @description: create and run Streams from config stream
     * @param StreamsConfig: streams config
     * @return: 0-success, other-failure
     */
    int CreateMultipleStreams(const std::string &streamsConfig) const;

    /**
     * @description: create and run Streams from stream config file
     * @param StreamsConfig: streams config file
     * @return: 0-success, other-failure
     */
    int CreateMultipleStreamsFromFile(const std::string &streamsFilePath) const;

    /**
     * @description: allow user to stop stream and restart later
     * @param StreamName: name of the stream
     * @return: 0-success, other-failure
     */
    int StopStream(const std::string &streamName) const;

    /**
     * @description: stop and destroy all Streams
     * @return: 0-success, other-failure
     */
    int DestroyAllStreams() const;

    /**
     * @description: send data to the input plugin of the Stream, use with GetResult function
     * @param StreamName: the name of the target Stream
     * @param inPluginId: the index of the input plugin
     * @param dataInput: the inferData to be sent
     * @return: 0-success, other-failure
     */
    int SendData(const std::string &streamName, const int &inPluginId, const MxDataInput &dataInput) const;
    int SendData(const std::string &streamName, const std::string &elementName, const MxDataInput &dataInput) const;
    /**
     * @description: get result from the output plugin of the Stream, the method is blocked
     * @param StreamName: the name of the target Stream
     * @param outPluginId: the index of the output plugin
     * @return: MxDataOutput
     */
    MxDataOutput GetResult(const std::string &streamName, const int &outPluginId,
        const unsigned int &msTimeOut = MxStream::DELAY_TIME) const;

    /**
     * @description: send data to the input plugin of the Stream, use with GetResultWithUniqueId function
     * @param StreamName: the name of the target Stream
     * @param inPluginId: the index of the input plugin
     * @param dataInput: the inferData to be sent
     * @return: uniqueId-using in GetResultWithUniqueId,  -1-send failure
     */
    int SendDataWithUniqueId(const std::string &streamName, const int &inPluginId, const MxDataInput &dataInput) const;
    int SendDataWithUniqueId(
        const std::string &streamName, const std::string &elementName, const MxDataInput &dataInput) const;
    /**
     * @description: get result from the output plugin of the Stream, the method is blocked
     * @param StreamName: the name of the target Stream
     * @param outPluginId: the index of the output plugin
     * @return: MxDataOutput
     */
    MxDataOutput GetResultWithUniqueId(
        const std::string &streamName, unsigned long uniqueId, unsigned int timeOutInMs) const;

    /**
     * @description: send protobuf to the input plugin of the Stream, use with SendProtobuf function
     * @param StreamName: the name of the target Stream
     * @param inPluginId: the index of the input plugin
     * @param protobufVec: the protobuf vector to be sent
     * @return: 0-success, other-failure
     */
    int SendProtobuf(
        const std::string &streamName, const int &inPluginId, const std::vector<MxProtobufIn> &protobufVec) const;
    int SendProtobuf(const std::string &streamName, const std::string &elementName,
        const std::vector<MxProtobufIn> &protobufVec) const;
    /**
     * @description: get protobuf from the output plugin of the Stream, the method is blocked
     * @param StreamName: the name of the target Stream
     * @param outPluginId: the index of the output plugin
     * @param keyVec: protobuf key vector
     * @return: std::vector<MxProtobufOut>
     */
    std::vector<MxProtobufOut> GetProtobuf(
        const std::string &streamName, const int &outPluginId, const std::vector<std::string> &keyVec) const;

    int SendData(const std::string &streamName, const std::string &elementName,
        const std::vector<MxMetadataInput> &metadataVec, const MxBufferInput &databuffer) const;
    MxBufferAndMetadataOutput GetResult(const std::string &streamName, const std::string &elementName,
        const std::vector<std::string> &dataSourceVec, const unsigned int &msTimeOut = MxStream::DELAY_TIME) const;

private:
    std::unique_ptr<MxStream::MxStreamManager> mxStreamManager_;
};
}  // namespace PyStream
#endif

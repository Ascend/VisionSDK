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
 * Create: 2020
 * History: NA
 */

#ifndef MXSM_STREAM_H
#define MXSM_STREAM_H

#include <nlohmann/json.hpp>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <map>
#include <string>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "MxStream/StreamManager/MxsmElement.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/BlockingQueue/BlockingQueue.h"
#include "MxStream/StreamManager/MxsmDataType.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxStream/DataType/StateInfo.h"

namespace MxStream {
enum MXST_TRANSMISSION_MODE {
    MXST_TRANSMISSION_DEFAULT,
    MXST_TRANSMISSION_NORMAL,
    MXST_TRANSMISSION_UNIQUE_ID,
    MXST_TRANSMISSION_UNIQUE_ID_MULTI
};
enum MXST_RESULT_STATUS {
    MXST_RESULT_STATUS_DEFAULT,
    MXST_RESULT_STATUS_DROP
};

enum INPUT_OUTPUT_ELEMENT {
    INPUT_ELEMENT,
    OUTPUT_ELEMENT
};

struct CallbackData {
    MXST_TRANSMISSION_MODE* transMode;
    std::map<uint64_t, MxstProtobufAndBuffer *>* outputMap;
    std::map<uint64_t, std::vector<MxstProtobufAndBuffer *>>* multiOutputMap;
    std::map<uint64_t, MXST_RESULT_STATUS>* resultStatusMap;
    MxBase::BlockingQueue<MxstProtobufAndBuffer *>* outputQueue;
    std::mutex* mutex;
    std::mutex* resultStatusMapMutex;
    std::condition_variable* outputMapCond;
    std::mutex* appsrcEnoughDataMutex;
    std::condition_variable* appsrcEnoughDataCond;
    bool* appsrcEnoughDataFlag;
    std::vector<GstElement *>* multiAppsinkVec;
    std::map<uint64_t, std::shared_ptr<std::condition_variable>>* uniqueIdCvMap;
};

struct ElementAndOrder {
    std::string elementName;
    int order;
};

/* *
 * @description: create or destroy a Stream
 */
class MxsmStream {
public:
    MxsmStream() : transMode_(MXST_TRANSMISSION_NORMAL), gstStream_(nullptr), isBusWatched(false), uniqueId_(0),
        isTransModeInitialized_(false){};
    ~MxsmStream();

    /* *
     * @description: create and run a single Stream from json object
     * @param StreamName: the name of the Stream
     * @param StreamObject: json object which descripes a Stream
     * @return: APP_ERROR
     */
    APP_ERROR CreateStream(const std::string& streamName, const nlohmann::json& streamObject);
    /* * stop and destroy the Stream
     */
    /* *
     * @description: stop and destroy a single Stream from json object
     * @param: void
     * @return: APP_ERROR
     */
    APP_ERROR DestroyStream();

    /* *
     * @description: send data to the input plugin of the Stream
     * @param inPluginId: the index of the input plugin
     * @param dataBuffer: the data to be sent
     * @return: APP_ERROR
     */
    APP_ERROR SendData(int inPluginId, MxstDataInput& dataBuffer);
    APP_ERROR SendData(const std::string elementName, MxstDataInput& dataBuffer);
    /* *
     * @description: get result from the output plugin of the Stream
     * @param outPluginId: the index of the output plugin
     * @return: MxstDataOutput
     */
    MxstDataOutput* GetResult(int outPluginId, const uint32_t& msTimeOut = DELAY_TIME);

    /* *
 * @description: send data to the input plugin of the Stream
 * @param inPluginId: the index of the input plugin
 * @param dataBuffer: the data to be sent
 * @return: APP_ERROR
 */
    APP_ERROR SendDataWithUniqueId(int inPluginId, MxstDataInput& dataBuffer, uint64_t& uniqueId);
    APP_ERROR SendDataWithUniqueId(const std::string elementName, MxstDataInput& dataBuffer, uint64_t& uniqueId);
    APP_ERROR SendMultiDataWithUniqueId(std::vector<int> inPluginIdVec, std::vector<MxstDataInput>& dataBufferVec,
        uint64_t& uniqueId);
    /* *
     * @description: get result from the output plugin of the Stream
     * @param outPluginId: the index of the output plugin
     * @return: MxstDataOutput
     */
    MxstDataOutput* GetResultWithUniqueId(uint64_t uniqueId, unsigned int timeOutMs = DELAY_TIME);
    std::vector<MxstDataOutput*> GetMultiResultWithUniqueId(uint64_t uniqueId, unsigned int timeOutMs = DELAY_TIME);

    void DropUniqueId(uint64_t uniqueId);

    APP_ERROR SendProtobuf(int inPluginId, std::vector<MxstProtobufIn>& protoVec);
    APP_ERROR SendProtobuf(const std::string elementName, std::vector<MxstProtobufIn>& protoVec);
    std::vector<MxstProtobufOut> GetProtobuf(int outPluginId, const std::vector<std::string>& keyVec);
    APP_ERROR SendData(const std::string& elementName,
        std::vector<MxstMetadataInput>& metadataVec, MxstBufferInput& dataBuffer);
    MxstBufferAndMetadataOutput GetResult(const std::string& elementName,
        const std::vector<std::string>& dataSourceVec, const uint32_t& msTimeOut = DELAY_TIME);
    void DestroyAppSinkBufQueVec();
    APP_ERROR SetElementProperty(const std::string& elementName,
                                 const std::string& propertyName,
                                 const std::string& propertyValue,
                                 bool dataSourceMeta = false);
    std::shared_ptr<MxstDataOutput> GetResultSP(int outPluginId, const uint32_t& msTimeOut = DELAY_TIME);
    std::shared_ptr<MxstDataOutput> GetResultWithUniqueIdSP(uint64_t uniqueId, uint32_t timeOutMs = DELAY_TIME);
    std::vector<std::shared_ptr<MxstDataOutput>> GetMultiResultWithUniqueIdSP(uint64_t uniqueId,
        uint32_t timeOutMs = DELAY_TIME);

public:
    std::map<uint64_t, MxStream::MxstProtobufAndBuffer *> outputMap_;
    std::map<uint64_t, std::vector<MxstProtobufAndBuffer *>> multiOutputMap_;
    std::map<uint64_t, MXST_RESULT_STATUS> resultStatusMap_;
    std::map<std::string, std::vector<std::pair<std::string, int>>> dataSourceMap_;
    MXST_TRANSMISSION_MODE transMode_;

private:

    /* *
     * @description: gstreamer callback function for handling bus message
     * @param bus: the bus of the Stream
     * @param msg: the message sent from the bus
     * @param userData: user data sent to the callback function
     * @return: APP_ERROR
     */

    static APP_ERROR BusWatchCallbackFunc(GstBus* bus, GstMessage* msg, gpointer userData);

    /* *
     * @description: create a element and add to the Stream
     * @param elementName: the name of the element
     * @param elementObject: the json object of a element
     * @return: APP_ERROR
     */
    APP_ERROR AddElementToStream(const std::string& elementName, nlohmann::json elementObject);

    /* *
     * @description: link elements defined in the Stream
     * @param: void
     * @return: APP_ERROR
     */
    APP_ERROR LinkElements();
    /* *
     * @description: check exsitence of a given element
     * @param elementName: the name of the element
     * @return: APP_ERROR
     */
    APP_ERROR HaveElement(const std::string& elementName);

    /* *
     * @description: find unlinked elments which are not allowed
     * @param: void
     * @return: APP_ERROR
     */
    APP_ERROR FindUnlinkedElements();

    /* *
     * @description: pull appsink sample
     * @param: sink the pointer of appsink element
     * @param: data the pointer of appsinkBufQueVec element
     * @return: APP_ERROR
     */
    static APP_ERROR PullSinkSampleCallback(GstElement *sink, void *data);
    static APP_ERROR PullSinkSaveNormalResult(CallbackData& callbackData, MxstProtobufAndBuffer& mxstOutput);
    static APP_ERROR PullSinkSaveUniqueResult(CallbackData& callbackData,
            MxstProtobufAndBuffer& mxstOutput, MxTools::MxpiBuffer& mxpiBuffer);
    static APP_ERROR PullSinkSaveUniqueResultMulti(CallbackData& callbackData, MxstProtobufAndBuffer& mxstOutput,
        MxTools::MxpiBuffer& mxpiBuffer);
    static APP_ERROR GenerateOutputData(GstBuffer& buffer, MxstDataOutput& mxstDataOutput, GstSample& sample);
    APP_ERROR GetStreamDeviceId(const nlohmann::json& streamObject);
    std::string GetNextElementName(const std::string& elementName);
    APP_ERROR DealApp(std::unique_ptr<MxsmElement>& newElement, const std::string& elementName);
    APP_ERROR DealAppsink(std::unique_ptr<MxsmElement>& newElement, const std::string& elementName);
    APP_ERROR CheckSendFuncTransMode(MXST_TRANSMISSION_MODE targetTransMode, const std::string& funcName);
    APP_ERROR CheckGetFuncTransMode(MXST_TRANSMISSION_MODE targetTransMode, const std::string& funcName);
    APP_ERROR DropDataWhenDestroyStream(GstPad* element);
    APP_ERROR RemoveAppSinkAndDropData(const GstElement* appsink);
    APP_ERROR SendEosAndFlushEvent();
    static MxstProtobufAndBuffer* CreateOutputBuffer();
    APP_ERROR AddElement(const nlohmann::json& streamObject);
    APP_ERROR ReorderElements(std::vector<std::pair<std::string, ElementAndOrder>>& previousElement);
    APP_ERROR BuildPreviousElementPair(std::vector<std::pair<std::string, ElementAndOrder>>& previousElement,
        const std::string& srcElementName, const int& currentOrder, const std::string& destElementName);
    bool IsInOutElementNameCorrect(const std::string& elementName, INPUT_OUTPUT_ELEMENT status);
    APP_ERROR SendProtobufComm(std::vector<MxstProtobufIn>& protoVec, MxTools::MxpiBuffer* &mxpiBuffer);
    APP_ERROR SendDataComm(MxstDataInput& dataBuffer, MxTools::MxpiBuffer* &mxpiBuffer);
    APP_ERROR SendDataWithUniqueIdComm(MxstDataInput& dataBuffer, uint64_t& uniqueId,
        MxTools::MxpiBuffer* &mxpiBuffer);
    APP_ERROR SendDataWithUniqueIdCommMulti(MxstDataInput& dataBuffer,
    uint64_t& uniqueId, MxTools::MxpiBuffer* &mxpiBuffer, bool setUniqueIdFlag);
    APP_ERROR SendProtoAndBufferComm(MxTools::MxpiBuffer* &mxpiBuffer,
        std::vector<MxstMetadataInput>& protoVec, MxstBufferInput& dataBuffer);
    MxstBufferAndMetadataOutput& GenerateMetaAndBufferOutput(const std::vector<std::string> &dataSourceVec,
        MxstBufferAndMetadataOutput &bufferAndMetaOut,
        MxstProtobufAndBuffer *protobufAndBuffer) const;
    MxstBufferAndMetadataOutput& CheckRequirementsForGetResult(
        const std::string& elementName, MxstBufferAndMetadataOutput& bufferAndMetaOut);
    bool IsAppsrcEnoughData();
    std::shared_ptr<MxstFrameExternalInfo> CreateExternalInfo(const MxstDataInput &dataBuffer, uint64_t &uniqueId);
    std::shared_ptr<MxstFrameExternalInfo> CreateExternalInfoMulti(const MxstDataInput &dataBuffer, uint64_t &uniqueId,
        bool setUniqueIdFlag);
    void EraseElements();
    void LoopRun();
    APP_ERROR IsInPluginIdVecValid(std::vector<int> inPluginIdVec);
    APP_ERROR CreateStreamCore(const std::string& streamName, const nlohmann::json& streamObject);
    APP_ERROR DestroyStreamCore();
    bool IsValidNumber(const std::string& text);
    APP_ERROR GetElementAndOrder(std::unique_ptr<MxsmElement>& newElement);
    APP_ERROR AppendElementToSourceMap(std::unique_ptr<MxsmElement> &newElement, std::string elementName,
        int elementOrder);
    APP_ERROR SetDataSourceProperty();

private:
    GstElement* gstStream_;
    bool isBusWatched;
    std::map<std::string, std::unique_ptr<MxsmElement>> elementMap_;
    std::vector<GstAppSrc *> appsrcVec_;
    std::map<std::string, GstAppSrc *> appsrcMap_;
    std::vector<GstElement *> appsinkVec_ = {};
    std::vector<GstElement *> multiAppsinkVec_ = {};
    std::map<std::string, uint32_t> appsinkIndexMap_;
    std::vector<MxBase::BlockingQueue<MxstProtobufAndBuffer *> *> appsinkBufQueVec_ = {};
    std::timed_mutex sendDataMutex_;
    uint64_t uniqueId_;
    std::string streamDeviceId_;
    bool isTransModeInitialized_;
    std::mutex dataMapMutex_;
    std::string streamName_;
    std::mutex resultStatusMapMutex_;
    std::condition_variable outputMapCond_;
    std::mutex appsrcEnoughDataMutex_;
    std::condition_variable appsrcEnoughDataCond_;
    bool appsrcEnoughDataFlag_ = false;
    std::map<uint64_t, std::shared_ptr<std::condition_variable>> uniqueIdCvMap_;
    StreamState streamState_ = STREAM_STATE_NEW;
    std::thread threadLoop_;
    GMainLoop *loop_ = nullptr;
    std::vector<CallbackData *> callbackDataVec_;

private:
    MxsmStream(const MxsmStream &) = delete;
    MxsmStream(const MxsmStream &&) = delete;
    MxsmStream& operator=(const MxsmStream &) = delete;
    MxsmStream& operator=(const MxsmStream &&) = delete;
};
}  // end namespace MxStream

#endif

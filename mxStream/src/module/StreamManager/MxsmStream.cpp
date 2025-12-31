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

#include "MxStream/StreamManager/MxsmStream.h"
#include <unistd.h>
#include <gst/app/gstappsink.h>
#include <cctype>
#include <algorithm>
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxTools/PluginToolkit/base/MxGstBase.h"
#include "MxTools/PluginToolkit/metadata/MxpiAiInfoSproc.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"
#include "MxStream/StreamManager/MxsmDescription.h"

using namespace MxTools;
using namespace std;

namespace {
const int OUTPUT_MAP_MAX_SIZE = 10;
const int SEND_DATA_TIMEOUT = 3; // unit: second
const long DEFAULT_MAX_DATA_SIZE = 1073741824;
const size_t MAX_LINK_MAP_SIZE = 128;
const std::vector<std::string> DYNAMIC_PROPERTY_ALLOW_LIST = { // element is facotry name
    "mxpi_channelselector", "mxpi_channelosdcoordsconverter", "mxpi_channelimagesstitcher", "mxpi_rtspsrc",
    "mxpi_object2osdinstances", "mxpi_class2osdinstances", "mxpi_bufferstablizer", "mxpi_skipframe"
};

template<typename Type>
Type ToNumber(const std::string& text)
{
    std::istringstream iss(text);
    Type num;
    iss >> num;
    return num;
}

APP_ERROR SetErrorInfoToDataOutput(MxTools::MxpiBuffer& mxpiBuffer, MxStream::MxstDataOutput& mxstDataOutput)
{
    APP_ERROR ret = APP_ERR_COMM_FAILURE;
    MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
    auto errorInfoPtr = mxpiMetadataManager.GetErrorInfo();
    if (errorInfoPtr != nullptr) {
        std::string result;
        auto errorInfo = *(std::static_pointer_cast<std::map<std::string, MxpiErrorInfo>>(errorInfoPtr));
        for (const auto& it : errorInfo) {
            result += (it.second).errorInfo + "\n";
            mxstDataOutput.errorCode = (it.second).ret;
        }
        if (MxBase::MemoryHelper::CheckDataSize(result.size()) != APP_ERR_OK) {
            LogError << "Datasize error. dataSize = " << result.size() << "." <<GetErrorInfo(APP_ERR_COMM_FAILURE);
            return ret;
        }
        mxstDataOutput.dataSize = static_cast<int>(result.size());
        mxstDataOutput.dataPtr = (uint32_t *) malloc(result.size());
        if (mxstDataOutput.dataPtr == nullptr) {
            LogError << "Malloc memory error. dataSize = " << result.size()
                     << "." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return ret;
        }
        std::copy(result.begin(), result.end(), (char *)mxstDataOutput.dataPtr);
        LogDebug << "SetErrorInfoToDataOutput result:" << result;
        ret = APP_ERR_OK;
    }
    return ret;
}

APP_ERROR SetMxpiObject(std::shared_ptr<MxTools::MxpiObjectList>& mxpiObjectList,
                        std::vector<MxStream::CropRoiBox>& roiBoxs)
{
    for (const auto& roiBox : roiBoxs) {
        MxTools::MxpiObject* objectData = mxpiObjectList->add_objectvec();
        if (objectData == nullptr) {
            LogError << "Fail to add objectData." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        objectData->set_x0(roiBox.x0);
        objectData->set_y0(roiBox.y0);
        objectData->set_x1(roiBox.x1);
        objectData->set_y1(roiBox.y1);
    }
    return APP_ERR_OK;
}
}

namespace MxStream {
static void AppsrcNeedData(const GstElement* &, guint, CallbackData& callbackData)
{
    callbackData.appsrcEnoughDataMutex->lock();
    *callbackData.appsrcEnoughDataFlag = false;
    callbackData.appsrcEnoughDataMutex->unlock();
    callbackData.appsrcEnoughDataCond->notify_all();
}

static void AppsrcEnoughData(const GstElement* &, CallbackData& callbackData)
{
    callbackData.appsrcEnoughDataMutex->lock();
    *callbackData.appsrcEnoughDataFlag = true;
    callbackData.appsrcEnoughDataMutex->unlock();
}

APP_ERROR MxsmStream::PullSinkSaveNormalResult(CallbackData& callbackData, MxstProtobufAndBuffer& mxstOutput)
{
    LogDebug << "pull normal result from appsink.";
    return callbackData.outputQueue->Push(&mxstOutput, TRUE);
}

APP_ERROR MxsmStream::PullSinkSaveUniqueResult(CallbackData& callbackData, MxstProtobufAndBuffer& mxstOutput,
    MxTools::MxpiBuffer& mxpiBuffer)
{
    LogDebug << "pull unique-id result from appsink.";
    MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
    auto externalInfoVoidPtr = mxpiMetadataManager.GetMetadata("MxstFrameExternalInfo");
    if (externalInfoVoidPtr == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST)
                 << "cannot get service info. please copy metadata MxstFrameExternalInfo when create a new MxpiBuffer.";
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST;
    }
    auto externalInfo = std::static_pointer_cast<MxstFrameExternalInfo>(externalInfoVoidPtr);
    callbackData.resultStatusMapMutex->lock();
    if (callbackData.resultStatusMap->find(externalInfo->uniqueId) != callbackData.resultStatusMap->end()) {
        callbackData.resultStatusMap->erase(externalInfo->uniqueId);
        callbackData.resultStatusMapMutex->unlock();
        LogDebug << "drop result of unique id:" << externalInfo->uniqueId << ".";
        return APP_ERR_STREAM_TIMEOUT;
    }
    callbackData.resultStatusMapMutex->unlock();

    LogDebug << "save result of unique id:" << externalInfo->uniqueId << ".";
    std::unique_lock<std::mutex> mapMutex(*callbackData.mutex);
    if (callbackData.outputMap->size() >= OUTPUT_MAP_MAX_SIZE) {
        callbackData.outputMapCond->wait(mapMutex);
    }
    callbackData.outputMap->insert(
        std::pair<uint64_t, MxStream::MxstProtobufAndBuffer *>(externalInfo->uniqueId, &mxstOutput));

    if ((callbackData.uniqueIdCvMap != nullptr) &&
        (callbackData.uniqueIdCvMap->find(externalInfo->uniqueId) != callbackData.uniqueIdCvMap->end())) {
        callbackData.uniqueIdCvMap->at(externalInfo->uniqueId)->notify_all();
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::PullSinkSaveUniqueResultMulti(CallbackData& callbackData, MxstProtobufAndBuffer& mxstOutput,
    MxTools::MxpiBuffer& mxpiBuffer)
{
    LogDebug << "pull unique-id result from multiple appsink.";
    MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
    auto externalInfoVoidPtr = mxpiMetadataManager.GetMetadata("MxstFrameExternalInfo");
    if (externalInfoVoidPtr == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST)
                 << "cannot get service info. please copy metadata MxstFrameExternalInfo when create a new MxpiBuffer.";
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST;
    }
    auto externalInfo = std::static_pointer_cast<MxstFrameExternalInfo>(externalInfoVoidPtr);
    callbackData.resultStatusMapMutex->lock();
    if (callbackData.resultStatusMap->find(externalInfo->uniqueId) != callbackData.resultStatusMap->end()) {
        callbackData.resultStatusMap->erase(externalInfo->uniqueId);
        callbackData.resultStatusMapMutex->unlock();
        LogDebug << "drop result of unique id:" << externalInfo->uniqueId << ".";
        return APP_ERR_STREAM_TIMEOUT;
    }
    callbackData.resultStatusMapMutex->unlock();

    LogDebug << "save result of unique id:" << externalInfo->uniqueId << ".";
    std::unique_lock<std::mutex> mapMutex(*callbackData.mutex);
    if (callbackData.multiOutputMap->size() >= OUTPUT_MAP_MAX_SIZE) {
        callbackData.outputMapCond->wait(mapMutex);
    }
    LogDebug << "inserting output to unique id:" << externalInfo->uniqueId << ".";
    if (callbackData.multiOutputMap->find(externalInfo->uniqueId) == callbackData.multiOutputMap->end()) {
        std::vector<MxstProtobufAndBuffer *> multiOutputVec = {&mxstOutput};
        callbackData.multiOutputMap->insert(
            std::pair<uint64_t, std::vector<MxstProtobufAndBuffer *>>(externalInfo->uniqueId, multiOutputVec));
        LogDebug << "unique-id doesn't exist in the multioutputmap, insert a new result into the multioutputmap.";
    } else {
        auto outputVec = callbackData.multiOutputMap->find(externalInfo->uniqueId);
        std::vector<MxstProtobufAndBuffer *> *multiOutputVec = &(outputVec->second);
        multiOutputVec->push_back(&mxstOutput);
        LogDebug << "insert a new result into the multioutputmap, the size of the map is " << outputVec->second.size();
    }

    if ((callbackData.uniqueIdCvMap != nullptr) &&
        (callbackData.uniqueIdCvMap->find(externalInfo->uniqueId) != callbackData.uniqueIdCvMap->end()) &&
        (callbackData.multiOutputMap->find(externalInfo->uniqueId))->second.size() ==
        callbackData.multiAppsinkVec->size()) {
        callbackData.uniqueIdCvMap->at(externalInfo->uniqueId)->notify_all();
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::GenerateOutputData(GstBuffer& buffer, MxstDataOutput& mxstDataOutput, GstSample&)
{
    GstMapInfo mapInfo;
    gst_buffer_map(&buffer, &mapInfo, GST_MAP_READ);
    if (mapInfo.size == 0) {
        gst_buffer_unmap(&buffer, &mapInfo);
        std::string emptyResult = "{}";
        mxstDataOutput.dataSize = static_cast<int>(emptyResult.size());
        mxstDataOutput.dataPtr = (uint32_t *) malloc(emptyResult.size());
        if (mxstDataOutput.dataPtr == nullptr) {
            mxstDataOutput.errorCode = APP_ERR_ACL_BAD_ALLOC;
            return APP_ERR_ACL_BAD_ALLOC;
        }
        std::copy(emptyResult.begin(), emptyResult.end(), (char *)mxstDataOutput.dataPtr);
        mxstDataOutput.errorCode = APP_ERR_OK;
        return APP_ERR_OK;
    } else if (mapInfo.size > DEFAULT_MAX_DATA_SIZE) {
        LogError << "The size of mapinfo is too large." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        gst_buffer_unmap(&buffer, &mapInfo);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    mxstDataOutput.dataSize = static_cast<int>(mapInfo.size);
    mxstDataOutput.dataPtr = (uint32_t *) malloc(mapInfo.size);
    if (mxstDataOutput.dataPtr == nullptr) {
        gst_buffer_unmap(&buffer, &mapInfo);
        return APP_ERR_ACL_BAD_ALLOC;
    }
    std::copy((char *)mapInfo.data, (char *)mapInfo.data + mapInfo.size, (char *)mxstDataOutput.dataPtr);
    gst_buffer_unmap(&buffer, &mapInfo);
    mxstDataOutput.errorCode = APP_ERR_OK;

    return APP_ERR_OK;
}

APP_ERROR MxsmStream::PullSinkSampleCallback(GstElement *sink, void *data)
{
    APP_ERROR ret = APP_ERR_OK;
    GstSample *sample = nullptr;
    /* Retrieve the buffer */
    g_signal_emit_by_name(sink, "pull-sample", &sample);
    if (sample == nullptr) {
        return APP_ERR_ACL_BAD_ALLOC;
    }
    /* The only thing we do in this example is print a * to indicate a received buffer */
    GstBuffer* buffer = gst_sample_get_buffer(sample);
    if (buffer == nullptr) {
        gst_sample_unref(sample);
        return APP_ERR_ACL_BAD_ALLOC;
    }
    auto mxstOutput = CreateOutputBuffer();
    if (mxstOutput == nullptr) {
        gst_sample_unref(sample);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    MxpiBuffer mxpiBuffer {};
    mxpiBuffer.buffer = (void *)buffer;
    MxpiMetaData mxpiMetaData {mxpiBuffer.buffer};
    auto metaData = (MxpiAiInfos*) MxpiMetaGet(mxpiMetaData);
    mxstOutput->mxpiProtobufMap = metaData->mxpiProtobufMap;

    if (SetErrorInfoToDataOutput(mxpiBuffer, *(mxstOutput->dataOutput)) != APP_ERR_OK) {
        ret = GenerateOutputData(*buffer, *(mxstOutput->dataOutput), *sample);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to generate output, buffer is empty." << GetErrorInfo(ret);
            delete mxstOutput->dataOutput;
            mxstOutput->dataOutput = nullptr;
            delete mxstOutput;
            mxstOutput = nullptr;
            gst_sample_unref(sample);
            return APP_ERR_ACL_BAD_ALLOC;
        }
    }

    auto callbackData = static_cast<CallbackData*>(data);
    if (*callbackData->transMode == MXST_TRANSMISSION_NORMAL) {
        ret = PullSinkSaveNormalResult(*callbackData, *mxstOutput);
    } else if (*callbackData->transMode == MXST_TRANSMISSION_UNIQUE_ID) {
        ret = PullSinkSaveUniqueResult(*callbackData, *mxstOutput, mxpiBuffer);
    } else if (*callbackData->transMode == MXST_TRANSMISSION_UNIQUE_ID_MULTI) {
        ret = PullSinkSaveUniqueResultMulti(*callbackData, *mxstOutput, mxpiBuffer);
    } else {
        ret = APP_ERR_STREAM_TRANS_MODE_INVALID;
        LogError << "Stream trans mode is invalid." << GetErrorInfo(ret);
    }
    if (ret != APP_ERR_OK) {
        delete mxstOutput->dataOutput;
        mxstOutput->dataOutput = nullptr;
        delete mxstOutput;
        mxstOutput = nullptr;
    }
    gst_sample_unref(sample);
    return ret;
}

MxstProtobufAndBuffer* MxsmStream::CreateOutputBuffer()
{
    auto mxstOutput = new (nothrow) MxstProtobufAndBuffer;
    if (mxstOutput == nullptr) {
        LogError << "Allocate memory with \"new MxstDataOutput\" failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    auto mxstDataOutput = new (nothrow) MxstDataOutput;
    if (mxstDataOutput == nullptr) {
        LogError << "Allocate memory with \"new MxstProtobufAndBuffer\" failed."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        delete mxstOutput;
        mxstOutput = nullptr;
        return nullptr;
    }
    mxstOutput->dataOutput = mxstDataOutput;
    return mxstOutput;
}

MxsmStream::~MxsmStream()
{
    if (gstStream_ != nullptr) {
        DestroyStream();
        gstStream_ = nullptr;
    }
}

APP_ERROR MxsmStream::BusWatchCallbackFunc(GstBus*, GstMessage* msg, gpointer userData)
{
    auto Stream = (MxsmStream*)userData;
    GstElement* gstPipe = Stream->gstStream_;
    if (gstPipe == nullptr) {
        LogError << "Stream does not exist." << GetErrorInfo(APP_ERR_STREAM_NOT_EXIST);
        return APP_ERR_STREAM_NOT_EXIST;
    }

    switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_STATE_CHANGED: {
            if (GST_ELEMENT(GST_MESSAGE_SRC(msg)) != gstPipe) {
                break;
            }
            GstState eOld = GST_STATE_VOID_PENDING;
            GstState eCur = GST_STATE_VOID_PENDING;
            GstState ePending = GST_STATE_VOID_PENDING;

            gst_message_parse_state_changed(msg, &eOld, &eCur, &ePending);
            LogInfo << "state changed: " << gst_element_state_get_name(eOld) << "->"
                     << gst_element_state_get_name(eCur);
            break;
        }
        case GST_MESSAGE_ERROR: {
            GError *err = nullptr;
            gchar *dbg = nullptr;

            gst_message_parse_error(msg, &err, &dbg);
            LogError << "Bus error: " << err->message << "(" << (dbg ? dbg : "no details")
                     << "). Trying to reconnect, please make sure the video stream is running properly and "
                     << "wait for reconnect." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            g_clear_error(&err);
            g_free(dbg);
            break;
        }
        default:
            break;
    }

    return APP_ERR_OK;
}

APP_ERROR MxsmStream::HaveElement(const std::string& elementName)
{
    auto find = elementMap_.find(elementName);
    if (find != elementMap_.end()) {
        return APP_ERR_STREAM_ELEMENT_EXIST;
    }

    return APP_ERR_STREAM_ELEMENT_NOT_EXIST;
}

APP_ERROR MxsmStream::DealApp(unique_ptr<MxsmElement>& newElement, const std::string& elementName)
{
    if (newElement->factoryName_ == "appsrc") {
        appsrcVec_.push_back((GstAppSrc*)newElement->gstElement_);
        appsrcMap_.insert(pair<string, GstAppSrc *>(elementName, (GstAppSrc*)newElement->gstElement_));
        auto callbackData = new (nothrow) CallbackData;
        if (callbackData == nullptr) {
            LogError << "Allocate memory with \"new CallbackData\" failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        callbackDataVec_.emplace_back(callbackData);
        callbackData->appsrcEnoughDataMutex = &appsrcEnoughDataMutex_;
        callbackData->appsrcEnoughDataCond = &appsrcEnoughDataCond_;
        callbackData->appsrcEnoughDataFlag = &appsrcEnoughDataFlag_;
        g_signal_connect(newElement->gstElement_, "need-data", G_CALLBACK (AppsrcNeedData), callbackData);
        g_signal_connect(newElement->gstElement_, "enough-data", G_CALLBACK (AppsrcEnoughData), callbackData);
    } else if (newElement->factoryName_ == "appsink") {
        DealAppsink(newElement, elementName);
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::DealAppsink(unique_ptr<MxsmElement>& newElement, const std::string& elementName)
{
    if (!IsInOutElementNameCorrect(elementName, OUTPUT_ELEMENT)) {
        LogError << "Output element name is error." << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID);
        return APP_ERR_STREAM_ELEMENT_INVALID;
    }
    auto appsinkBufQue = new (nothrow) MxBase::BlockingQueue<MxstProtobufAndBuffer *>;
    if (appsinkBufQue == nullptr) {
        LogError << "Allocate memory with new BlockingQueue failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    auto callbackData = new (nothrow) CallbackData;
    if (callbackData == nullptr) {
        LogError << "Allocate memory with new CallbackData failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        delete appsinkBufQue;
        return APP_ERR_COMM_ALLOC_MEM;
    }
    appsinkBufQueVec_.push_back(appsinkBufQue);
    appsinkIndexMap_.insert(std::pair<std::string, uint32_t>(newElement->elementName_, appsinkVec_.size()));
    appsinkVec_.push_back(newElement->gstElement_);
    multiAppsinkVec_.push_back(newElement->gstElement_);
    g_object_set(GST_APP_SINK(newElement->gstElement_), "emit-signals", TRUE, "caps", FALSE, NULL);
    callbackDataVec_.emplace_back(callbackData);
    callbackData->outputMap = &outputMap_;
    callbackData->multiOutputMap = &multiOutputMap_;
    callbackData->outputQueue = appsinkBufQue;
    callbackData->transMode = &transMode_;
    callbackData->resultStatusMap = &resultStatusMap_;
    callbackData->resultStatusMapMutex = &resultStatusMapMutex_;
    callbackData->outputMapCond = &outputMapCond_;
    callbackData->mutex = &dataMapMutex_;
    callbackData->uniqueIdCvMap = &uniqueIdCvMap_;
    callbackData->multiAppsinkVec = &multiAppsinkVec_;
    g_signal_connect(GST_APP_SINK(newElement->gstElement_), "new-sample",
        G_CALLBACK(MxsmStream::PullSinkSampleCallback), callbackData);
    return APP_ERR_OK;
}

bool MxsmStream::IsInOutElementNameCorrect(const string& elementName, INPUT_OUTPUT_ELEMENT status)
{
    string appsrc = "appsrc";
    string appsink = "appsink";
    if ((elementName.find(appsrc) != std::string::npos) && (status == INPUT_ELEMENT)) {
        int appsrcLen = static_cast<int>(appsrc.size());
        string appsrcStr1 = elementName.substr(0, appsrcLen);
        string appsrcStr2 = elementName.substr(appsrcLen);
        if (appsrcStr1 != appsrc) {
            LogError << "The input element name(" << elementName
                     << ") is incorrect, the elementName should be appsrc+digit."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        for (size_t i = 0; i < appsrcStr2.size(); i++) {
            if (!isdigit(appsrcStr2[i])) {
                LogError << "The input element name(" << elementName
                         << ") is incorrect, the elementName should be appsrc+digit."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return false;
            }
        }
        return true;
    } else if ((elementName.find(appsink) != std::string::npos) && (status == OUTPUT_ELEMENT)) {
        int appsinkLen = static_cast<int>(appsink.size());
        string appsinkStr1 = elementName.substr(0, appsinkLen);
        string appsinkStr2 = elementName.substr(appsinkLen);
        if (appsinkStr1 != appsink) {
            LogError << "The output element name(" << elementName
                     << ") is incorrect, the elementName should be appsink+digit."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        for (size_t i = 0; i < appsinkStr2.size(); i++) {
            if (!isdigit(appsinkStr2[i])) {
                LogError << "The output element name(" << elementName
                         << ") is incorrect, the elementName should be appsink+digit."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return false;
            }
        }
        return true;
    }
    return false;
}

APP_ERROR MxsmStream::AppendElementToSourceMap(std::unique_ptr<MxsmElement> &newElement, std::string elementName,
    int elementOrder)
{
    if (dataSourceMap_[elementName].size() == 0) {
        LogError << "Invalid dataSourceMap." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((dataSourceMap_[elementName][0].second == -1 && elementOrder != -1) ||
        (dataSourceMap_[elementName][0].second != -1 && elementOrder == -1)) {
        LogError << "All of the metadata linked to element(" << elementName << ") should contain a port or not."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (elementOrder != -1) {
        for (auto curPair : dataSourceMap_[elementName]) {
            if (elementOrder == curPair.second) {
                LogError << "The nextMeta link to the plugin with same port has been detected."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
    }
    std::pair<std::string, int> newPair(newElement->elementName_, elementOrder);
    dataSourceMap_[elementName].push_back(newPair);
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::GetElementAndOrder(std::unique_ptr<MxsmElement> &newElement)
{
    for (auto dest : newElement->metaDataNextLink_) {
        std::string elementName = "";
        int elementOrder = -1;
        size_t pos = dest.find(':');
        if (pos == std::string::npos) {
            elementName = dest;
        } else {
            elementName = dest.substr(0, pos);
            std::string sequence = dest.substr(pos + 1, dest.size());
            if (pos == 0 || sequence.size() == 0 || !(std::stringstream(sequence) >> elementOrder)) {
                LogError << "Invalid element name or port in metadata link."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        if (dataSourceMap_.find(elementName) == dataSourceMap_.end()) {
            if (dataSourceMap_.size() > MAX_LINK_MAP_SIZE) {
                LogError << "Link map for datasource is out of range."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            std::pair<std::string, int> newPair(newElement->elementName_, elementOrder);
            dataSourceMap_[elementName] = {newPair};
        } else {
            APP_ERROR ret = AppendElementToSourceMap(newElement, elementName, elementOrder);
            if (ret != APP_ERR_OK) {
                return ret;
            }
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::AddElementToStream(const std::string& elementName, nlohmann::json elementObject)
{
    string elemName = GetNextElementName(elementName);
    if (HaveElement(elemName) == APP_ERR_STREAM_ELEMENT_EXIST) {
        LogError << "Stream(" << GST_OBJECT_NAME(gstStream_) << ") add element failed, already have element("
                 << elemName.c_str() << ")." << GetErrorInfo(APP_ERR_STREAM_ELEMENT_EXIST);
        return APP_ERR_STREAM_ELEMENT_EXIST;
    }
    auto newElement = std::unique_ptr<MxsmElement>(new (nothrow) MxsmElement(streamName_));
    if (newElement == nullptr) {
        LogError << "Allocate memory with \"new MxsmElement\" failed."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    // set stream_config deviceId for element streamDeviceId_
    if (!streamDeviceId_.empty()) {
        newElement->streamDeviceId_ = streamDeviceId_;
    }
    APP_ERROR ret = newElement->CreateElement(elemName, elementObject);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    auto factoryObj = elementObject.find(ELEMENT_FACTORY);
    if (factoryObj == elementObject.end() || !(*factoryObj).is_string()) {
        LogError << "Can not find element factory name in the config."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_FACTORY);
        return APP_ERR_ELEMENT_INVALID_FACTORY;
    }
    newElement->factoryName_ = *factoryObj;
    ret = GetElementAndOrder(newElement);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = DealApp(newElement, elementName);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    gst_bin_add(GST_BIN(gstStream_), newElement->gstElement_);
    elementMap_[newElement->elementName_] = std::move(newElement);
    LogInfo << "Adds "<< elemName <<" element to Stream successfully.";
    return ret;
}

int GetElementOrder(const string& elementName)
{
    int order = 0;
    int number = count(elementName.begin(), elementName.end(), ':');
    if (number == 1) {
        size_t pos = elementName.find(':');
        std::string eleName = elementName.substr(0, pos);
        std::string sequence = elementName.substr(pos + 1, elementName.size());
        if (!(std::stringstream(sequence) >> order)) {
            return -1;
        }
    }
    return order;
}

bool SortPreviousElement(const std::pair<string, ElementAndOrder>& iter1,
    const std::pair<string, ElementAndOrder>& iter2)
{
    int order1 = GetElementOrder(iter1.first);
    int order2 = GetElementOrder(iter2.first);
    if (order1 == order2) {
        ElementAndOrder elemAndOrder1 = iter1.second;
        ElementAndOrder elemAndOrder2 = iter2.second;
        return elemAndOrder1.order < elemAndOrder2.order;
    }
    return order1 < order2;
}

APP_ERROR MxsmStream::BuildPreviousElementPair(std::vector<std::pair<string, ElementAndOrder>>& previousElement,
    const string& srcElementName, const int& currentOrder, const string& destElementName)
{
    ElementAndOrder elementAndOrder;
    elementAndOrder.elementName = srcElementName;
    elementAndOrder.order = currentOrder;
    auto elementPair = make_pair(destElementName, elementAndOrder);
    for (auto iter = previousElement.begin(); iter != previousElement.end(); iter++) {
        if (iter->first == destElementName) {
            LogError << "Pipline error for exist the same next plugin, srcElementName:"
                     << srcElementName << " destElementName:" << destElementName
                     << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
            return APP_ERR_STREAM_INVALID_LINK;
        }
    }
    previousElement.push_back(elementPair);
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::ReorderElements(std::vector<std::pair<string, ElementAndOrder>>& previousElement)
{
    for (const auto& iter : elementMap_) {
        auto& currentElement = iter.second;
        string srcElementName = currentElement->elementName_;
        auto nextLinkMap = currentElement->nextLinkMap_;
        auto nextNames = currentElement->nextLinkElementNames_;
        if (currentElement->nextLinkMap_.empty())
            continue;

        for (size_t i = 0; i < nextNames.size(); i++) {
            if (nextLinkMap[nextNames[i]].GetState() != MXSM_LINK_STATE_LINKPENDING &&
                nextLinkMap[nextNames[i]].GetState() != MXSM_LINK_STATE_RELINKPENDING) {
                continue;
            }
            string elementName = GetNextElementName(nextNames[i]);
            if (HaveElement(elementName) == APP_ERR_STREAM_ELEMENT_NOT_EXIST) {
                return APP_ERR_STREAM_ELEMENT_NOT_EXIST;
            }
            string destElementName = nextNames[i];
            APP_ERROR ret = BuildPreviousElementPair(previousElement, srcElementName, i, destElementName);
            if (ret != APP_ERR_OK) {
                LogError << "Build previous element pair failed." << GetErrorInfo(ret);
                return ret;
            }
        }
    }
    sort(previousElement.begin(), previousElement.end(), SortPreviousElement);
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::LinkElements()
{
    vector<pair<string, ElementAndOrder>> previousElement;
    APP_ERROR ret = ReorderElements(previousElement);
    if (ret != APP_ERR_OK) {
        LogError << "Reorder element error." << GetErrorInfo(ret);
        return ret;
    }
    map<std::string, int> elementNum;
    for (auto iter = previousElement.begin(); iter != previousElement.end(); iter++) {
        ElementAndOrder elemAndOrder = iter->second;
        string elementName = GetNextElementName(iter->first);
        auto& nextElement = elementMap_[elementName];
        auto& currentElement = elementMap_[elemAndOrder.elementName];
        int order = GetElementOrder(iter->first);
        if (order != elementNum[elementName]) {
            LogError << "The port number of (" << currentElement->elementName_
                     << ") is incorrect, the element order is "
                     << elementNum[elementName] << ", the element name is " << iter->first
                     << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
            return APP_ERR_STREAM_INVALID_LINK;
        }
        elementNum[elementName]++;
        ret = MxsmElement::PerformLink(currentElement->gstElement_,
            nextElement->gstElement_, elemAndOrder.order, order);
        if (ret != APP_ERR_OK) {
            LogError << "Link element(" << currentElement->elementName_ << ") and element("
                     << nextElement->elementName_ << ") failed." << GetErrorInfo(ret);
            return ret;
        }
        LogDebug << GetErrorInfo(ret) << "Link element(" << currentElement->elementName_
                 << ") port:" << elemAndOrder.order << " and element("
                 << nextElement->elementName_ << ") port:" << order << " successful.";
        currentElement->SetStateNormal();
    }

    LogInfo << "Links all elements successfully.";
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::FindUnlinkedElements()
{
    for (const auto& iter : elementMap_) {
        auto& element = iter.second;
        APP_ERROR ret = element->FindUnlinkedPad();
        if (ret != APP_ERR_OK) {
            LogError << "element(" << iter.first << ") is not linked." << GetErrorInfo(ret);
            return ret;
        }
    }

    return APP_ERR_OK;
}

bool MxsmStream::IsValidNumber(const std::string& text)
{
    for (auto &ch : text) {
        if (!isdigit(ch)) {
            return false;
        }
    }
    return true;
}

APP_ERROR MxsmStream::GetStreamDeviceId(const nlohmann::json& streamObject)
{
    auto iter = streamObject.find(STREAM_CONFIG);
    auto jsonObj = *iter;
    auto it = jsonObj.find(DEVICEID_KEY_NAME);
    if (it == jsonObj.end()) {
        LogWarn << "stream_config deviceId doesn't have deviceId prpoperty, use default deviceId or plugin deviceId.";
        return APP_ERR_OK;
    }
    if (!(*it).is_string()) {
        LogWarn << "Invalid stream_config property.";
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if ((*it).empty()) {
        LogWarn << "deviceId property of stream_config is null, use default deviceId or plugin deviceId.";
        return APP_ERR_OK;
    }
    if (!IsValidNumber(*it)) {
        LogError << "deviceId property of stream_config is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    int streamDeviceId = ToNumber<int>(*it);
    LogDebug << "streamDeviceId(" << streamDeviceId << ").";
    if (streamDeviceId >= 0 && to_string(streamDeviceId).size() == static_cast<std::string>(*it).size()) {
        streamDeviceId_ = *it;
        LogInfo << "Gets the value of stream_config deviceId(" << *it << ").";
        return APP_ERR_OK;
    } else {
        LogError << "Invalid deviceId(" << *it
                 << ") of stream_config deviceId, deviceId must be greater than or equal to 0."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
}

string MxsmStream::GetNextElementName(const string& elementName)
{
    int number = count(elementName.begin(), elementName.end(), ':');
    if (number == 1) {
        size_t pos = elementName.find(':');
        return elementName.substr(0, pos);
    }
    return elementName;
}

bool SortElement(const std::pair<string, nlohmann::json>& iter1, const std::pair<string, nlohmann::json>& iter2)
{
    string appsink = "appsink";
    if ((iter1.first.find(appsink) != std::string::npos)
        && (iter2.first.find(appsink) != std::string::npos)) {
        int len = static_cast<int>(appsink.size());
        string str1 = iter1.first.substr(len);
        int num1 = 0;
        std::stringstream(str1) >> num1;
        string str2 = iter2.first.substr(len);
        int num2 = 0;
        std::stringstream(str2) >> num2;
        return num1 < num2;
    } else {
        return iter1.first < iter2.first;
    }
}

APP_ERROR MxsmStream::AddElement(const nlohmann::json& streamObject)
{
    string appsink = "appsink";
    vector<std::pair<string, nlohmann::json>> streamValueVec;
    for (const auto& iter : streamObject.items()) {
        if (iter.key().find(appsink) != std::string::npos) {
            string str1 = iter.key().substr(appsink.size());
            int num = 0;
            if (!(std::stringstream(str1) >> num)) {
                LogError << "Element name: \"" << iter.key()
                         << "\" is invalid. For example: element name is \"appsinkxxx\", \"xxx\" must be integer"
                         << GetErrorInfo(APP_ERR_COMM_FAILURE);
                return APP_ERR_COMM_FAILURE;
            }
        }
        streamValueVec.push_back(std::make_pair(iter.key(), iter.value()));
    }
    std::sort(streamValueVec.begin(), streamValueVec.end(), SortElement);
    for (const auto& iter : streamValueVec) {
        auto elementName = iter.first;
        // skip stream_config deviceId, it is not a plugin, for more normal can use a struct
        if (iter.first == STREAM_CONFIG) {
            continue;
        }
        LogInfo << "Add element to stream, element name is " << elementName;
        APP_ERROR ret = AddElementToStream(elementName, iter.second);
        if (ret != APP_ERR_OK) {
            LogError << " adds " << elementName << " element to Stream failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::SetDataSourceProperty()
{
    auto metaDataSortFunc = [] (std::pair<std::string, int> pair1, std::pair<std::string, int> pair2) {
        return pair1.second <= pair2.second;
    };
    for (auto iter = dataSourceMap_.begin(); iter != dataSourceMap_.end(); iter++) {
        const std::string elementName = iter->first;
        if (elementMap_.find(elementName) == elementMap_.end()) {
            LogError << "Cannot find the destination of the metadata next link."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        sort(iter->second.begin(), iter->second.end(), metaDataSortFunc);
        const std::string dataSourceKey = "dataSource";
        std::string dataSourceValue = "";

        for (size_t idx = 0; idx < iter->second.size(); idx++) {
            if (iter->second[idx].second >= static_cast<int>(iter->second.size())) {
                LogError << "The port of next metadata is out of range."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            std::string sourceItem = iter->second[idx].first;
            std::string appendStr = (idx == iter->second.size() - 1) ? sourceItem : sourceItem + ",";
            dataSourceValue += appendStr;
        }
        LogDebug << "Setting element(" << elementName << ") dataSource as \"" << dataSourceValue << "\".";
        APP_ERROR ret = SetElementProperty(elementName, dataSourceKey, dataSourceValue, true);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::CreateStreamCore(const std::string& streamName, const nlohmann::json& streamObject)
{
    gstStream_ = gst_pipeline_new(streamName.c_str());
    if (!gstStream_) {
        LogInfo << "Failed to create Stream instance, named " << streamName.c_str() << ".";
        return APP_ERR_STREAM_CREATE_FAILED;
    }

    gst_bus_add_signal_watch(GST_ELEMENT_BUS(gstStream_));
    isBusWatched = true;
    g_signal_connect(GST_ELEMENT_BUS(gstStream_), "message", G_CALLBACK(MxsmStream::BusWatchCallbackFunc), this);

    // get stream_config deviceId from pipeline
    if (streamObject.find(STREAM_CONFIG) != streamObject.end() && GetStreamDeviceId(streamObject) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    } else {
        LogInfo << "stream_config deviceId is null, all elements use default deviceId or plugin deviceId.";
    }
    APP_ERROR ret = AddElement(streamObject);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = LinkElements();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = SetDataSourceProperty();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    for (const auto& iter : elementMap_) {
        elementMap_[iter.first]->HandlePerformanceStatistics();
    }

    ret = FindUnlinkedElements();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    GstStateChangeReturn changeRet = gst_element_set_state(gstStream_, GST_STATE_PLAYING);
    if (changeRet == GST_STATE_CHANGE_FAILURE) {
        LogError << "Failed to set the state of the Stream, named: " << streamName.c_str() << "."
                 << GetErrorInfo(APP_ERR_STREAM_CHANGE_STATE_FAILED);
        return APP_ERR_STREAM_CHANGE_STATE_FAILED;
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::CreateStream(const std::string& streamName, const nlohmann::json& streamObject)
{
    if (streamState_ == STREAM_STATE_NORMAL || streamState_ == STREAM_STATE_BUILD_INPROGRESS) {
        LogError << streamName << " has been created, do not cerate repetitively."
                 << GetErrorInfo(APP_ERR_STREAM_EXIST);
        return APP_ERR_STREAM_EXIST;
    }
    streamState_ = STREAM_STATE_BUILD_INPROGRESS;
    if (gstStream_ != nullptr || streamObject.empty()) {
        streamState_ = STREAM_STATE_BUILD_FAILED;
        LogError << "stream(" << streamName << ") is empty." << GetErrorInfo(APP_ERR_STREAM_EXIST);
        return APP_ERR_STREAM_EXIST;
    }
    this->streamName_ = streamName;
    APP_ERROR ret = MxsmDescription::ValidateStreamObject(streamObject);
    if (ret != APP_ERR_OK) {
        streamState_ = STREAM_STATE_BUILD_FAILED;
        LogError << "Creates "<< streamName <<" Stream failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = CreateStreamCore(streamName, streamObject);
    if (ret != APP_ERR_OK) {
        streamState_ = STREAM_STATE_BUILD_FAILED;
        LogError << "Creates "<< streamName <<" Stream failed." << GetErrorInfo(ret);
        return ret;
    }
    streamState_ = STREAM_STATE_NORMAL;
    PerformanceStatisticsManager::GetInstance()->E2eStatisticsRegister(streamName);
    PerformanceStatisticsManager::GetInstance()->ThroughputRateStatisticsRegister(streamName);
    threadLoop_ = std::thread(std::bind(&MxsmStream::LoopRun, this));
    threadLoop_.detach();
    return ret;
}

void MxsmStream::LoopRun()
{
    loop_ = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop_);
    g_main_loop_unref(loop_);
    loop_ = nullptr;
}

void MxsmStream::DestroyAppSinkBufQueVec()
{
    for (auto& iter : appsinkBufQueVec_) {
        iter->Restart();
        while (!iter->IsEmpty()) {
            MxstProtobufAndBuffer* ptr = nullptr;
            iter->Pop(ptr);
            if (ptr != nullptr && ptr->dataOutput != nullptr) {
                delete ptr->dataOutput;
                ptr->dataOutput = nullptr;
                delete ptr;
                ptr = nullptr;
            } else if (ptr != nullptr && ptr->dataOutput == nullptr) {
                delete ptr;
                ptr = nullptr;
            }
        }
        iter->Stop();
        delete iter;
        iter = nullptr;
    }
    appsinkBufQueVec_.clear();
}

static GstPadProbeReturn DropDataCallbackFunc(GstPad *, GstPadProbeInfo *, gpointer)
{
    LogDebug << "Begin to drop data in the callback function.";
    return GST_PAD_PROBE_DROP;
}

APP_ERROR MxsmStream::DropDataWhenDestroyStream(GstPad* pad)
{
    LogDebug << "Begin to drop data from pad.";
    if (pad == nullptr) {
        LogError << "Invalid pad." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // get src pad
    GstPad* blockPad = gst_pad_get_peer(pad);
    gst_pad_add_probe(blockPad, GST_PAD_PROBE_TYPE_BUFFER, (GstPadProbeCallback) DropDataCallbackFunc, nullptr,
                      nullptr);
    gst_object_unref (blockPad);
    return APP_ERR_OK;
}

void MxsmStream::EraseElements()
{
    std::lock_guard<std::mutex> lock(g_queueSizeStatisticsMtx);
    for (auto iter = elementMap_.begin(); iter != elementMap_.end();) {
        if (iter->second->factoryName_ == "queue") {
            auto queueIter = std::find(g_queueGstElementVec.begin(),
                                       g_queueGstElementVec.end(), iter->second->gstElement_);
            if (queueIter != g_queueGstElementVec.end()) {
                g_queueGstElementVec.erase(queueIter);
            }
        }
        elementMap_.erase(iter++);
    }
}

APP_ERROR MxsmStream::DestroyStreamCore()
{
    if (isBusWatched) {
        gst_bus_remove_signal_watch(GST_ELEMENT_BUS(gstStream_));
        isBusWatched = false;
    }

    APP_ERROR ret = SendEosAndFlushEvent();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to send eos or flush event to stream(" << GST_OBJECT_NAME(gstStream_) << ")."
                 << GetErrorInfo(ret);
        return ret;
    }

    for (auto& iter : appsinkBufQueVec_) {
        iter->Stop();
    }

    for (auto appSink : appsinkVec_) {
        ret = RemoveAppSinkAndDropData(appSink);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to remove appsink(" << GST_ELEMENT_NAME(appSink) << ")."
                     << GetErrorInfo(ret);
            return ret;
        }
    }
    appsinkVec_.clear();
    multiAppsinkVec_.clear();
    appsrcVec_.clear();
    appsrcMap_.clear();
    DestroyAppSinkBufQueVec();

    GstStateChangeReturn changeRet = gst_element_set_state(gstStream_, GST_STATE_NULL);
    if (changeRet == GST_STATE_CHANGE_FAILURE) {
        LogError << "Failed to set the state of the Stream, named: " << GST_OBJECT_NAME(gstStream_) << "."
                 << GetErrorInfo(APP_ERR_STREAM_CHANGE_STATE_FAILED);
        return APP_ERR_STREAM_CHANGE_STATE_FAILED;
    }
    EraseElements();

    std::string streamName(GST_OBJECT_NAME(gstStream_));
    gst_object_unref(GST_OBJECT(gstStream_));
    gstStream_ = nullptr;
    uniqueIdCvMap_.clear();
    for (auto callbackData : callbackDataVec_) {
        delete callbackData;
        callbackData = nullptr;
    }
    callbackDataVec_.clear();
    if (loop_ != nullptr) {
        g_main_loop_quit(loop_);
    }
    LogInfo << "Destroys the stream(" << streamName << ") successfully.";
    return ret;
}

APP_ERROR MxsmStream::DestroyStream()
{
    if (streamState_ == STREAM_STATE_DESTROY) {
        return APP_ERR_OK;
    }
    LogInfo << "Begin to destroy stream(" << GST_OBJECT_NAME(gstStream_) << ").";
    outputMapCond_.notify_one();
    if (gstStream_ == nullptr) {
        LogError << "Stream instance does not exist." << GetErrorInfo(APP_ERR_STREAM_NOT_EXIST);
        return APP_ERR_STREAM_NOT_EXIST;
    }
    APP_ERROR ret = DestroyStreamCore();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    streamState_ = STREAM_STATE_DESTROY;
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::SendEosAndFlushEvent()
{
    GstStructure *eventStruct = gst_structure_new("custom_eos", "opt_out", G_TYPE_BOOLEAN, TRUE, NULL);
    GstEvent *event = gst_event_new_custom(GST_EVENT_CUSTOM_DOWNSTREAM, eventStruct);
    if (gst_element_send_event(GST_ELEMENT(gstStream_), event)) {
        LogInfo << "Send custom eos to the Stream successfully.";
    } else {
        LogWarn << "Failed to send custom eos to the Stream.";
    }

    if (gst_element_send_event(GST_ELEMENT(gstStream_), gst_event_new_eos())) {
        LogInfo << "Send eos to the Stream successfully.";
    } else {
        LogWarn << "Failed to send eos to the Stream.";
    }

    if (gst_element_send_event(GST_ELEMENT(gstStream_), gst_event_new_flush_start())) {
        LogInfo << "Flushes the Stream data successfully.";
    } else {
        LogWarn << "Failed to flush the Stream data.";
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::RemoveAppSinkAndDropData(const GstElement *appsink)
{
    LogDebug << "Begin to run remove appsink and drop data.";
    APP_ERROR ret = APP_ERR_OK;
    GstIterator* padIterator = gst_element_iterate_sink_pads(GST_ELEMENT(appsink));
    if (padIterator == nullptr) {
        LogError << "Failed to get sink pads of element(" << GST_ELEMENT_NAME(appsink) << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    GValue vPad = G_VALUE_INIT;
    if (gst_iterator_next(padIterator, &vPad) != GST_ITERATOR_OK) {
        LogError << "Failed to get a sink pad of element(" << GST_ELEMENT_NAME(appsink) << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        gst_iterator_free(padIterator);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    gst_iterator_free(padIterator);
    GstPad* sinkPad = GST_PAD(g_value_get_object(&vPad));
    if (sinkPad == nullptr) {
        LogError << "Failed to get the pad from GValue." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ret = DropDataWhenDestroyStream(sinkPad);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to drop input data of appsink." << GetErrorInfo(ret);
        return ret;
    }
    GstStateChangeReturn retChangeState = gst_element_set_state(GST_ELEMENT(appsink), GST_STATE_NULL);
    if (retChangeState != GST_STATE_CHANGE_SUCCESS) {
        LogError << "Failed to set the state of element(" << GST_ELEMENT_NAME(appsink) << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    gboolean retBoolean = gst_bin_remove(GST_BIN(gstStream_), GST_ELEMENT(appsink));
    if (retBoolean != TRUE) {
        LogError << "Failed to remove the element(" << GST_ELEMENT_NAME(appsink) << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return ret;
}

APP_ERROR MxsmStream::CheckSendFuncTransMode(MXST_TRANSMISSION_MODE targetTransMode, const std::string& funcName)
{
    if (!isTransModeInitialized_) {
        transMode_ = targetTransMode;
        isTransModeInitialized_ = true;
    } else if (transMode_ != targetTransMode) {
        LogError << "cannot send data with \"" << funcName << "()\", use the same function as last time."
                 << GetErrorInfo(APP_ERR_STREAM_TRANS_MODE_NOT_MATCHED);
        return APP_ERR_STREAM_TRANS_MODE_NOT_MATCHED;
    }

    return APP_ERR_OK;
}

APP_ERROR MxsmStream::CheckGetFuncTransMode(MXST_TRANSMISSION_MODE targetTransMode, const std::string& funcName)
{
    if (transMode_ != targetTransMode) {
        LogError << "cannot get data with \"" << funcName << "()\", use the other matched with the send function."
                 << GetErrorInfo(APP_ERR_STREAM_TRANS_MODE_NOT_MATCHED);
        return APP_ERR_STREAM_TRANS_MODE_NOT_MATCHED;
    }

    return APP_ERR_OK;
}

/* * send Protobuf to the input plugin of the Stream
 */
APP_ERROR MxsmStream::SendProtobufComm(std::vector<MxstProtobufIn>& protoVec, MxTools::MxpiBuffer* &mxpiBuffer)
{
    APP_ERROR ret = CheckSendFuncTransMode(MXST_TRANSMISSION_NORMAL, "SendProtobuf");
    if (ret != APP_ERR_OK) {
        LogError << "Call SendProtobuf() failed." << GetErrorInfo(ret);
        return ret;
    }
    MxTools::InputParam inputParam;
    inputParam.dataSize = 0;
    mxpiBuffer = MxTools::MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    if (mxpiBuffer == nullptr) {
        LogError << "Create mxpiBuffer object failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }

    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    for (size_t i = 0; i < protoVec.size(); i++) {
        if (protoVec[i].messagePtr == nullptr) {
            LogError << "MessagePtr is null." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        ret = mxpiMetadataManager.AddProtoMetadata(protoVec[i].key, protoVec[i].messagePtr);
        if (ret != APP_ERR_OK) {
            LogError << "Add metadata failed, fail to push buffer." << GetErrorInfo(ret);
            MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
            return ret;
        }
    }

    return APP_ERR_OK;
}


APP_ERROR MxsmStream::SendProtoAndBufferComm(MxTools::MxpiBuffer* &mxpiBuffer,
    std::vector<MxstMetadataInput>& protoVec, MxstBufferInput& dataBuffer)
{
    APP_ERROR ret = CheckSendFuncTransMode(MXST_TRANSMISSION_NORMAL, "SendProtoAndBuffer");
    if (ret != APP_ERR_OK) {
        LogError << "Call SendProtoAndBuffer() failed." << GetErrorInfo(ret);
        return ret;
    }

    MxTools::InputParam inputParam;
    inputParam.key = "appsrc";
    inputParam.dataSize = dataBuffer.dataSize;
    inputParam.ptrData = (void*)dataBuffer.dataPtr;
    inputParam.mxpiFrameInfo = dataBuffer.mxpiFrameInfo;
    inputParam.mxpiVisionInfo = dataBuffer.mxpiVisionInfo;
    mxpiBuffer = MxTools::MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    if (mxpiBuffer == nullptr) {
        LogError << "Create mxpiBuffer object failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }

    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    for (size_t i = 0; i < protoVec.size(); i++) {
        if (protoVec[i].messagePtr == nullptr) {
            LogError << "MessagePtr is null."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        ret = mxpiMetadataManager.AddProtoMetadata(protoVec[i].dataSource, protoVec[i].messagePtr);
        if (ret != APP_ERR_OK) {
            LogError << "Add metadata failed, fail to push buffer."
                     << GetErrorInfo(ret);
            MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::SendData(const std::string& elementName,
    std::vector<MxstMetadataInput>& metadataVec, MxstBufferInput& dataBuffer)
{
    if (appsrcMap_.find(elementName) == appsrcMap_.end()) {
        LogError << "Fail to push buffer, inPluginId is out of range or dataBuffer.dataPtr is null."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
        return APP_ERR_STREAM_INVALID_LINK;
    }
    MxTools::MxpiBuffer* mxpiBuffer = nullptr;
    APP_ERROR ret = SendProtoAndBufferComm(mxpiBuffer, metadataVec, dataBuffer);
    if (ret != APP_ERR_OK) {
        LogError << "SendProtoAndBufferComm error." << GetErrorInfo(ret);
        return ret;
    }

    std::unique_lock<decltype(sendDataMutex_)> sendDataLock(sendDataMutex_);
    auto buffer = (GstBuffer*)mxpiBuffer->buffer;
    int gstRet = gst_app_src_push_buffer(appsrcMap_[elementName], buffer);
    if (gstRet != APP_ERR_OK) {
        LogError << "Failed to push buffer." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return APP_ERR_COMM_FAILURE;
    }
    delete mxpiBuffer;
    mxpiBuffer = nullptr;
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::SendProtobuf(int inPluginId, std::vector<MxstProtobufIn>& protoVec)
{
    if ((size_t)inPluginId >= appsrcVec_.size()) {
        LogError << "Fail to push buffer, inPluginId is out of range or dataBuffer.dataPtr is null."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
        return APP_ERR_STREAM_INVALID_LINK;
    }
    MxTools::MxpiBuffer* mxpiBuffer = nullptr;
    APP_ERROR ret = SendProtobufComm(protoVec, mxpiBuffer);
    if (ret != APP_ERR_OK) {
        LogError << "SendProtobufComm error." << GetErrorInfo(ret);
        return ret;
    }
    std::unique_lock<decltype(sendDataMutex_)> sendDataLock(sendDataMutex_);
    auto buffer = (GstBuffer*)mxpiBuffer->buffer;
    int gstRet = gst_app_src_push_buffer(appsrcVec_[inPluginId], buffer);
    if (gstRet != APP_ERR_OK) {
        LogError << "Failed to push buffer" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return APP_ERR_COMM_FAILURE;
    }
    delete mxpiBuffer;
    mxpiBuffer = nullptr;
    return APP_ERR_OK;
}

/* * send Protobuf to the input plugin of the Stream
 */
APP_ERROR MxsmStream::SendProtobuf(const string elementName, std::vector<MxstProtobufIn>& protoVec)
{
    if (appsrcMap_.find(elementName) == appsrcMap_.end()) {
        LogError << "Fail to push buffer, inPluginId is out of range or dataBuffer.dataPtr is null."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
        return APP_ERR_STREAM_INVALID_LINK;
    }
    MxTools::MxpiBuffer* mxpiBuffer = nullptr;
    APP_ERROR ret = SendProtobufComm(protoVec, mxpiBuffer);
    if (ret != APP_ERR_OK) {
        LogError << "SendProtobufComm error." << GetErrorInfo(ret);
        return ret;
    }
    std::unique_lock<decltype(sendDataMutex_)> sendDataLock(sendDataMutex_);
    auto buffer = (GstBuffer*)mxpiBuffer->buffer;
    int gstRet = gst_app_src_push_buffer(appsrcMap_[elementName], buffer);
    if (gstRet != APP_ERR_OK) {
        LogError << "Failed to push buffer." << GetErrorInfo(APP_ERR_COMM_FAILURE) ;
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return APP_ERR_COMM_FAILURE;
    }
    delete mxpiBuffer;
    mxpiBuffer = nullptr;
    return APP_ERR_OK;
}

/* * send data to the input plugin of the Stream
 */
APP_ERROR MxsmStream::SendDataComm(MxstDataInput& dataBuffer, MxTools::MxpiBuffer* &mxpiBuffer)
{
    APP_ERROR ret = CheckSendFuncTransMode(MXST_TRANSMISSION_NORMAL, "SendData");
    if (ret != APP_ERR_OK) {
        LogError << "Call SendData() failed." << GetErrorInfo(ret);
        return ret;
    }

    auto mxpiObjectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiObjectList>();
    if (mxpiObjectList == nullptr) {
        LogError << "Send data comm MakeShared failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    MxstServiceInfo mxServiceInfo(dataBuffer.serviceInfo);
    ret = SetMxpiObject(mxpiObjectList, dataBuffer.serviceInfo.roiBoxs);
    if (ret != APP_ERR_OK) {
        LogError << "Set mxpiObject failed." << GetErrorInfo(ret);
        return ret;
    }

    MxTools::InputParam inputParam;
    inputParam.key = "appsrc";
    inputParam.dataSize = dataBuffer.dataSize;
    inputParam.ptrData = (void*)dataBuffer.dataPtr;
    mxpiBuffer = MxTools::MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    if (mxpiBuffer == nullptr) {
        LogError << "Create mxpiBuffer object failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }

    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    ret = mxpiMetadataManager.AddMetadata("appInput", std::static_pointer_cast<void>(mxpiObjectList));
    if (ret != APP_ERR_OK) {
        LogError << "Add metadata failed, fail to push buffer." << GetErrorInfo(ret);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::SendData(int inPluginId, MxstDataInput& dataBuffer)
{
    if ((size_t)inPluginId >= appsrcVec_.size() || dataBuffer.dataPtr == nullptr) {
        LogError << "Fail to push buffer, inPluginId is out of range or dataBuffer.dataPtr is null."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
        return APP_ERR_STREAM_INVALID_LINK;
    }
    MxTools::MxpiBuffer* mxpiBuffer = nullptr;
    APP_ERROR ret = SendDataComm(dataBuffer, mxpiBuffer);
    if (ret != APP_ERR_OK) {
        LogError << "SendDataComm error." << GetErrorInfo(ret);
        return ret;
    }

    std::unique_lock<decltype(sendDataMutex_)> sendDataLock(sendDataMutex_);
    auto buffer = (GstBuffer*)mxpiBuffer->buffer;
    int gstRet = gst_app_src_push_buffer(appsrcVec_[inPluginId], buffer);
    if (gstRet != APP_ERR_OK) {
        LogError << "Failed to push buffer." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return APP_ERR_COMM_FAILURE;
    }
    delete mxpiBuffer;
    mxpiBuffer = nullptr;
    return APP_ERR_OK;
}

/* * send data to the input plugin of the Stream
 */
APP_ERROR MxsmStream::SendData(const string elementName, MxstDataInput& dataBuffer)
{
    if (appsrcMap_.find(elementName) == appsrcMap_.end() || dataBuffer.dataPtr == nullptr) {
        LogError << "Fail to push buffer, inPluginId is out of range or dataBuffer.dataPtr is null."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
        return APP_ERR_STREAM_INVALID_LINK;
    }
    MxTools::MxpiBuffer* mxpiBuffer = nullptr;
    APP_ERROR ret = SendDataComm(dataBuffer, mxpiBuffer);
    if (ret != APP_ERR_OK) {
        LogError << "SendDataComm error." << GetErrorInfo(ret);
        return ret;
    }

    std::unique_lock<decltype(sendDataMutex_)> sendDataLock(sendDataMutex_);
    auto buffer = (GstBuffer*)mxpiBuffer->buffer;
    int gstRet = gst_app_src_push_buffer(appsrcMap_[elementName], buffer);
    if (gstRet != APP_ERR_OK) {
        LogError << "Failed to push buffer." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return APP_ERR_COMM_FAILURE;
    }
    delete mxpiBuffer;
    mxpiBuffer = nullptr;
    return APP_ERR_OK;
}
/* * get result from the output plugin of the Stream
 */
MxstDataOutput* MxsmStream::GetResult(int outPluginId, const uint32_t& msTimeOut)
{
    APP_ERROR ret = CheckGetFuncTransMode(MXST_TRANSMISSION_NORMAL, "GetResult");
    if (ret != APP_ERR_OK) {
        LogWarn << "Call GetResult() failed.";
        return nullptr;
    }
    MxstProtobufAndBuffer* outPut = nullptr;
    if (outPluginId >= 0 && ((unsigned int)outPluginId < appsinkBufQueVec_.size())) {
        auto ret = appsinkBufQueVec_[outPluginId]->Pop(outPut, msTimeOut);
        if (ret != APP_ERR_OK) {
            LogWarn << "Exceed the time limit(" << msTimeOut << ").";
            return nullptr;
        }
    }
    if (outPut == nullptr) {
        LogWarn << "GetResult return null.";
        return nullptr;
    }
    auto result = outPut->dataOutput;
    delete outPut;
    outPut = nullptr;
    return result;
}

MxstBufferAndMetadataOutput MxsmStream::GetResult(const std::string& elementName,
    const std::vector<std::string>& dataSourceVec, const uint32_t& msTimeOut)
{
    MxstBufferAndMetadataOutput bufferAndMetaOut;
    bufferAndMetaOut = CheckRequirementsForGetResult(elementName, bufferAndMetaOut);
    if (bufferAndMetaOut.errorCode != APP_ERR_OK) {
        return bufferAndMetaOut;
    }

    int outPluginId = static_cast<int>(appsinkIndexMap_.at(elementName));
    if (outPluginId < 0 || ((unsigned int)outPluginId >= appsinkBufQueVec_.size())) {
        std::string errorMsg = "The given index(" + std::to_string(outPluginId) + ") of output plugin is not valid.";
        LogError << errorMsg << GetErrorInfo(APP_ERR_COMM_FAILURE);
        bufferAndMetaOut.SetErrorInfo(APP_ERR_COMM_FAILURE, errorMsg);
        return bufferAndMetaOut;
    }

    MxstProtobufAndBuffer* protobufAndBuffer = nullptr;
    auto ret = appsinkBufQueVec_[outPluginId]->Pop(protobufAndBuffer, msTimeOut);
    if (protobufAndBuffer == nullptr || ret != APP_ERR_OK) {
        if (protobufAndBuffer != nullptr) {
            delete protobufAndBuffer->dataOutput;
            protobufAndBuffer->dataOutput = nullptr;
            delete protobufAndBuffer;
            protobufAndBuffer = nullptr;
        }
        std::string errorMsg = GetErrorInfo(APP_ERR_COMM_FAILURE) + "Get empty result" +
            ", or exceed the time limit(" + std::to_string(msTimeOut) + ").";
        LogDebug << errorMsg;
        bufferAndMetaOut.SetErrorInfo(APP_ERR_COMM_FAILURE, errorMsg);
        return bufferAndMetaOut;
    }
    bufferAndMetaOut = GenerateMetaAndBufferOutput(dataSourceVec, bufferAndMetaOut, protobufAndBuffer);

    delete protobufAndBuffer->dataOutput;
    protobufAndBuffer->dataOutput = nullptr;
    delete protobufAndBuffer;
    protobufAndBuffer = nullptr;

    return bufferAndMetaOut;
}

MxstBufferAndMetadataOutput& MxsmStream::GenerateMetaAndBufferOutput(const vector<std::string>& dataSourceVec,
    MxstBufferAndMetadataOutput &bufferAndMetaOut,
    MxstProtobufAndBuffer *protobufAndBuffer) const
{
    LogDebug << "Begin to generate metadata and buffer output.";
    if (protobufAndBuffer->dataOutput->errorCode != APP_ERR_OK) {
        bufferAndMetaOut.errorCode = protobufAndBuffer->dataOutput->errorCode;
        bufferAndMetaOut.errorMsg = std::string((char *)protobufAndBuffer->dataOutput->dataPtr,
            protobufAndBuffer->dataOutput->dataSize);
        return bufferAndMetaOut;
    }

    auto outputBuffer = MxBase::MemoryHelper::MakeShared<MxstBufferOutput>();
    if (outputBuffer == nullptr) {
        std::string errorMessage = "generate meta and buffer output MakeShared failed.";
        LogError << errorMessage << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        bufferAndMetaOut.errorCode = APP_ERR_COMM_ALLOC_MEM;
        bufferAndMetaOut.errorMsg = errorMessage;
        return bufferAndMetaOut;
    }
    outputBuffer->dataSize = protobufAndBuffer->dataOutput->dataSize;
    outputBuffer->dataPtr = protobufAndBuffer->dataOutput->dataPtr;
    protobufAndBuffer->dataOutput->dataPtr = nullptr;

    for (auto& dataSource : dataSourceVec) {
        MxstMetadataOutput outputMeta;
        if (protobufAndBuffer->mxpiProtobufMap.find(dataSource) != protobufAndBuffer->mxpiProtobufMap.end()) {
            outputMeta.dataPtr = protobufAndBuffer->mxpiProtobufMap[dataSource];
            const google::protobuf::Descriptor* desc = outputMeta.dataPtr->GetDescriptor();
            if (desc == nullptr) {
                LogError << "Get descriptor failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
                outputMeta.errorCode = APP_ERR_COMM_INVALID_POINTER;
            } else {
                outputMeta.dataType = desc->name();
            }
        } else {
            std::string errorMsg = GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST) +
                "DataSource(" + dataSource + ") does not exist.";
            outputMeta.errorCode = APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST;
            LogDebug << errorMsg;
        }

        bufferAndMetaOut.metadataVec.push_back(outputMeta);
    }
    bufferAndMetaOut.bufferOutput = outputBuffer;
    return bufferAndMetaOut;
}

MxstBufferAndMetadataOutput& MxsmStream::CheckRequirementsForGetResult(
    const std::string& elementName, MxstBufferAndMetadataOutput& bufferAndMetaOut)
{
    if (transMode_ != MXST_TRANSMISSION_NORMAL) {
        std::string errorMsg = "Cannot get data with \"GetResult()\", use the other matched with send data function.";
        LogError << errorMsg << GetErrorInfo(APP_ERR_STREAM_TRANS_MODE_NOT_MATCHED);
        bufferAndMetaOut.SetErrorInfo(APP_ERR_STREAM_TRANS_MODE_NOT_MATCHED, errorMsg);
        return bufferAndMetaOut;
    }
    auto it = appsinkIndexMap_.find(elementName);
    if (it == appsinkIndexMap_.end()) {
        std::string errorMsg = "ElementName(" + elementName + ") is not a name of output element.";
        LogError << errorMsg << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID);
        bufferAndMetaOut.SetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID, errorMsg);
        return bufferAndMetaOut;
    }
    return bufferAndMetaOut;
}

/*
 * get protobuf from the output plugin of the Stream
 */
std::vector<MxstProtobufOut> MxsmStream::GetProtobuf(int outPluginId, const std::vector<std::string>& keyVec)
{
    std::vector<MxstProtobufOut> dataBufferVec;
    dataBufferVec.clear();
    if (transMode_ == MXST_TRANSMISSION_DEFAULT) {
        LogWarn << "please send data before get result.";
        MxstProtobufOut outPut(MXST_TRANSMISSION_DEFAULT);
        dataBufferVec.push_back(outPut);
        return dataBufferVec;
    }
    if (transMode_ != MXST_TRANSMISSION_NORMAL) {
        LogError << "Cannot get data with \"GetProtobuf()\", use the other matched with send data function."
                 << GetErrorInfo(APP_ERR_STREAM_TRANS_MODE_NOT_MATCHED);
        MxstProtobufOut outPut(APP_ERR_STREAM_TRANS_MODE_NOT_MATCHED);
        dataBufferVec.push_back(outPut);
        return dataBufferVec;
    }
    if (outPluginId >= 0 && ((unsigned int)outPluginId < appsinkBufQueVec_.size())) {
        MxstProtobufAndBuffer* inferResult = nullptr;
        appsinkBufQueVec_[outPluginId]->Pop(inferResult);
        if (inferResult == nullptr) {
            LogError << "GetProtobuf return null." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return dataBufferVec;
        }
        for (auto& key : keyVec) {
            if (inferResult->mxpiProtobufMap.find(key) != inferResult->mxpiProtobufMap.end()) {
                MxstProtobufOut outPut(inferResult->dataOutput->errorCode, key);
                outPut.messagePtr = inferResult->mxpiProtobufMap[key];
                dataBufferVec.push_back(outPut);
            } else {
                LogWarn<< "GetProtobuf key is not exist.";
            }
        }
        delete inferResult->dataOutput;
        inferResult->dataOutput = nullptr;
        delete inferResult;
        inferResult = nullptr;
    } else {
        LogError << "GetProtobuf outPluginId is not exist. outPluginId = " << outPluginId << "."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }

    return dataBufferVec;
}

APP_ERROR MxsmStream::SendDataWithUniqueIdComm(MxstDataInput& dataBuffer,
    uint64_t& uniqueId, MxTools::MxpiBuffer* &mxpiBuffer)
{
    APP_ERROR ret = CheckSendFuncTransMode(MXST_TRANSMISSION_UNIQUE_ID, "SendDataWithUniqueId");
    if (ret != APP_ERR_OK) {
        return ret;
    }

    auto mxpiObjectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiObjectList>();
    if (mxpiObjectList == nullptr) {
        LogError << "Send data with unique id comm MakeShared failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    ret = SetMxpiObject(mxpiObjectList, dataBuffer.serviceInfo.roiBoxs);
    if (ret != APP_ERR_OK) {
        LogError << "Set mxpiObject failed." << GetErrorInfo(ret);
        return ret;
    }
    MxTools::InputParam inputParam;
    inputParam.key = "appsrc";
    inputParam.dataSize = dataBuffer.dataSize;
    inputParam.ptrData = (void*)dataBuffer.dataPtr;
    mxpiBuffer = MxTools::MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    if (mxpiBuffer == nullptr) {
        LogError << "Create mxpiBuffer object failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }

    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    ret = mxpiMetadataManager.AddMetadata("appInput", std::static_pointer_cast<void>(mxpiObjectList));
    if (ret != APP_ERR_OK) {
        LogError << "Add metadata(appInput) failed, fail to push buffer." << GetErrorInfo(ret);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return ret;
    }
    auto externalInfo = CreateExternalInfo(dataBuffer, uniqueId);
    if (externalInfo == nullptr) {
        LogError << "Create external info failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    ret = mxpiMetadataManager.AddMetadata("MxstFrameExternalInfo", std::static_pointer_cast<void>(externalInfo));
    if (ret != APP_ERR_OK) {
        LogError << "Add metadata(serviceInfo) failed, fail to push buffer" << GetErrorInfo(ret);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return ret;
    }
    std::unique_lock<decltype(sendDataMutex_)> sendDataLock(sendDataMutex_);
    std::unique_lock<decltype(dataMapMutex_)> dataMapLock(dataMapMutex_);
    if (uniqueIdCvMap_.find(uniqueId) == uniqueIdCvMap_.end()) {
        uniqueIdCvMap_[uniqueId] = MxBase::MemoryHelper::MakeShared<std::condition_variable>();
        if (uniqueIdCvMap_[uniqueId] == nullptr) {
            LogError << "Allocate memory with \"make_shared condition_variable\" failed."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    return APP_ERR_OK;
}

// SendDataWithUniqueIdComm support multi output
APP_ERROR MxsmStream::SendDataWithUniqueIdCommMulti(MxstDataInput& dataBuffer,
    uint64_t& uniqueId, MxTools::MxpiBuffer* &mxpiBuffer, bool setUniqueIdFlag)
{
    auto ret = CheckSendFuncTransMode(MXST_TRANSMISSION_UNIQUE_ID_MULTI, "SendDataWithUniqueId");
    if (ret != APP_ERR_OK) {
        return ret;
    }

    auto mxpiObjectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiObjectList>();
    if (mxpiObjectList == nullptr) {
        LogError << "Send data with unique id comm MakeShared failed."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    ret = SetMxpiObject(mxpiObjectList, dataBuffer.serviceInfo.roiBoxs);
    if (ret != APP_ERR_OK) {
        LogError << "Set mxpiObject failed." << GetErrorInfo(ret);
        return ret;
    }
    MxTools::InputParam inputParam;
    inputParam.key = "appsrc";
    inputParam.dataSize = dataBuffer.dataSize;
    inputParam.ptrData = (void*)dataBuffer.dataPtr;
    mxpiBuffer = MxTools::MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    if (mxpiBuffer == nullptr) {
        LogError << "Create mxpiBuffer object failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }

    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    ret = mxpiMetadataManager.AddMetadata("appInput", std::static_pointer_cast<void>(mxpiObjectList));
    if (ret != APP_ERR_OK) {
        LogError << "Add metadata(appInput) failed, fail to push buffer." << GetErrorInfo(ret);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return ret;
    }
    auto externalInfo = CreateExternalInfoMulti(dataBuffer, uniqueId, setUniqueIdFlag);
    if (externalInfo == nullptr) {
        LogError << "Create external info failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    ret = mxpiMetadataManager.AddMetadata("MxstFrameExternalInfo", std::static_pointer_cast<void>(externalInfo));
    if (ret != APP_ERR_OK) {
        LogError << "Add metadata(serviceInfo) failed, fail to push buffer." << GetErrorInfo(ret);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return ret;
    }
    std::unique_lock<decltype(sendDataMutex_)> sendDataLock(sendDataMutex_);
    std::unique_lock<decltype(dataMapMutex_)> dataMapLock(dataMapMutex_);
    if (uniqueIdCvMap_.find(uniqueId) == uniqueIdCvMap_.end()) {
        uniqueIdCvMap_[uniqueId] = MxBase::MemoryHelper::MakeShared<std::condition_variable>();
        if (uniqueIdCvMap_[uniqueId] == nullptr) {
            LogError << "Allocate memory with \"make_shared condition_variable\" failed."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    return APP_ERR_OK;
}

shared_ptr<MxstFrameExternalInfo> MxsmStream::CreateExternalInfo(const MxstDataInput &dataBuffer, uint64_t &uniqueId)
{
    auto externalInfo = MxBase::MemoryHelper::MakeShared<MxstFrameExternalInfo>();
    if (externalInfo == nullptr) {
        LogError << "Send data comm MakeShared failed."<< GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    externalInfo->customParam = dataBuffer.serviceInfo.customParam;
    externalInfo->fragmentId = dataBuffer.serviceInfo.fragmentId;
    uniqueId = uniqueId_++;
    LogDebug << "putting uniqueId(" << uniqueId << ") into externalInfo";
    externalInfo->uniqueId = uniqueId;
    return externalInfo;
}

shared_ptr<MxstFrameExternalInfo> MxsmStream::CreateExternalInfoMulti(const MxstDataInput &dataBuffer,
    uint64_t &uniqueId, bool setUniqueIdFlag)
{
    auto externalInfo = MxBase::MemoryHelper::MakeShared<MxstFrameExternalInfo>();
    if (externalInfo == nullptr) {
        LogError << "send data comm MakeShared failed."<< GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    externalInfo->customParam = dataBuffer.serviceInfo.customParam;
    externalInfo->fragmentId = dataBuffer.serviceInfo.fragmentId;
    if (setUniqueIdFlag) {
        uniqueId = uniqueId_++;
    }
    LogDebug << "putting uniqueId(" << uniqueId << ") into externalInfo";
    externalInfo->uniqueId = uniqueId;
    return externalInfo;
}

bool MxsmStream::IsAppsrcEnoughData()
{
    std::time_t t = std::time(0);
    std::unique_lock<std::mutex> lock(appsrcEnoughDataMutex_);
    while (appsrcEnoughDataFlag_ && std::time(0) - t < SEND_DATA_TIMEOUT && std::time(0) - t >= 0) {
        appsrcEnoughDataCond_.wait_for(lock, std::chrono::seconds(1));
    }
    return appsrcEnoughDataFlag_;
}

APP_ERROR MxsmStream::SendDataWithUniqueId(int inPluginId, MxstDataInput& dataBuffer, uint64_t& uniqueId)
{
    if ((size_t)inPluginId >= appsrcVec_.size() || dataBuffer.dataPtr == nullptr) {
        LogError << "inPluginId is out of range or dataPtr is null."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
        return APP_ERR_STREAM_INVALID_LINK;
    }

    if (IsAppsrcEnoughData()) {
        LogError << "SendDataWithUniqueId error." << GetErrorInfo(APP_ERR_QUEUE_FULL);
        return APP_ERR_QUEUE_FULL;
    }

    MxTools::MxpiBuffer* mxpiBuffer = nullptr;
    APP_ERROR ret = SendDataWithUniqueIdComm(dataBuffer, uniqueId, mxpiBuffer);
    if (ret != APP_ERR_OK) {
        LogError << "SendDataWithUniqueIdComm error." << GetErrorInfo(ret);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return ret;
    }

    auto buffer = (GstBuffer*)mxpiBuffer->buffer;
    int gstRet = gst_app_src_push_buffer(appsrcVec_[inPluginId], buffer);
    if (gstRet != APP_ERR_OK) {
        LogError << "Failed to push buffer." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return APP_ERR_COMM_FAILURE;
    }
    delete mxpiBuffer;
    mxpiBuffer = nullptr;
    return APP_ERR_OK;
}

APP_ERROR MxsmStream::IsInPluginIdVecValid(std::vector<int> inPluginIdVec)
{
    for (int i = 0; i < (int)inPluginIdVec.size(); i++) {
        if ((int)inPluginIdVec[i] != i) {
            LogError << "inPluginIdVec[" << i << "] is (" << inPluginIdVec[i] <<
                     "), which is invalid, it should be " << i <<"." << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
            return APP_ERR_STREAM_INVALID_CONFIG;
        }
    }
    return APP_ERR_OK;
}

// SendDataWithUniqueId support multi-input
APP_ERROR MxsmStream::SendMultiDataWithUniqueId(std::vector<int> inPluginIdVec,
    std::vector<MxstDataInput>& dataBufferVec, uint64_t& uniqueId)
{
    if (inPluginIdVec.size() != dataBufferVec.size()) {
        LogError << "DataBufferVec and inPluginVec has " << "different size."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
        return APP_ERR_STREAM_INVALID_LINK;
    }
    for (size_t i = 0; i < inPluginIdVec.size(); i++) {
        if (unsigned(inPluginIdVec[i]) >= appsrcVec_.size()) {
            LogError << "InPluginIdVec is out of range, " << "the number of appsrc is (" << appsrcVec_.size()
                     << "), inPluginVec[" << i << "] is " << inPluginIdVec[i] << "."
                     << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
            return APP_ERR_STREAM_INVALID_LINK;
        }
    }

    APP_ERROR ret = IsInPluginIdVecValid(inPluginIdVec);
    if (ret != APP_ERR_OK) {
        LogError << "InPluginIdVec is invalid. Please make sure input(inPluginIdVec) is correct."
                 << GetErrorInfo(ret);
        return ret;
    }

    for (size_t i = 0; i < inPluginIdVec.size(); i++) {
        MxTools::MxpiBuffer* mxpiBuffer = nullptr;
        bool setUniqueIdFlag = false;
        if (i == 0) {
            setUniqueIdFlag = true;
        }
        if (IsAppsrcEnoughData()) {
            LogError << "SendDataWithUniqueId error." << GetErrorInfo(APP_ERR_QUEUE_FULL);
            return APP_ERR_QUEUE_FULL;
        }

        ret = SendDataWithUniqueIdCommMulti(dataBufferVec[i], uniqueId, mxpiBuffer, setUniqueIdFlag);
        if (ret != APP_ERR_OK) {
            LogError << "SendDataWithUniqueIdComm error." << GetErrorInfo(ret);
            MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
            return ret;
        }

        auto buffer = (GstBuffer*)mxpiBuffer->buffer;
        int gstRet = gst_app_src_push_buffer(appsrcVec_[inPluginIdVec[i]], buffer);
        if (gstRet != APP_ERR_OK) {
            LogError << "Failed to push buffer." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
            return APP_ERR_COMM_FAILURE;
        }
        delete mxpiBuffer;
        mxpiBuffer = nullptr;
    }

    return APP_ERR_OK;
}

APP_ERROR MxsmStream::SendDataWithUniqueId(const string elementName, MxstDataInput& dataBuffer, uint64_t& uniqueId)
{
    if (appsrcMap_.find(elementName) == appsrcMap_.end() || dataBuffer.dataPtr == nullptr) {
        LogError << "inPluginId is out of range or dataPtr is null."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
        return APP_ERR_STREAM_INVALID_LINK;
    }
    if (IsAppsrcEnoughData()) {
        LogError  << "SendDataWithUniqueId error." << GetErrorInfo(APP_ERR_QUEUE_FULL);
        return APP_ERR_QUEUE_FULL;
    }

    MxTools::MxpiBuffer* mxpiBuffer = nullptr;
    APP_ERROR ret = SendDataWithUniqueIdComm(dataBuffer, uniqueId, mxpiBuffer);
    if (ret != APP_ERR_OK) {
        LogError << "SendDataWithUniqueIdComm error." << GetErrorInfo(ret);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return ret;
    }
    auto buffer = (GstBuffer*)mxpiBuffer->buffer;
    int gstRet = gst_app_src_push_buffer(appsrcMap_[elementName], buffer);
    if (gstRet != APP_ERR_OK) {
        LogError << "Failed to push buffer." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return APP_ERR_COMM_FAILURE;
    }
    delete mxpiBuffer;
    mxpiBuffer = nullptr;
    return APP_ERR_OK;
}

MxstDataOutput* MxsmStream::GetResultWithUniqueId(uint64_t uniqueId, unsigned int timeOutMs)
{
    APP_ERROR ret = CheckGetFuncTransMode(MXST_TRANSMISSION_UNIQUE_ID, "GetResultWithUniqueId");
    if (ret != APP_ERR_OK) {
        LogWarn << "Call GetResultWithUniqueId() failed.";
        return nullptr;
    }

    MxstProtobufAndBuffer* outPut = nullptr;
    MxstDataOutput* ptr = nullptr;

    std::unique_lock<std::mutex> mapMutex(dataMapMutex_);

    if (outputMap_.find(uniqueId) != outputMap_.end()) {
        outPut = outputMap_.at(uniqueId);
        if (outPut == nullptr) {
            LogWarn << "GetResultWithUniqueId return null.";
            return nullptr;
        }
        ptr = outPut->dataOutput;
        outputMap_.erase(uniqueId);
        delete outPut;
        outPut = nullptr;
        outputMapCond_.notify_one();
        return ptr;
    }

    if (uniqueIdCvMap_.find(uniqueId) != uniqueIdCvMap_.end()) {
        uniqueIdCvMap_[uniqueId]->wait_for(mapMutex, std::chrono::milliseconds(timeOutMs));
    } else {
        LogWarn << "Failed find " << uniqueId << " in uniqueIdCvMap";
    }

    auto iter = uniqueIdCvMap_.find(uniqueId);
    if (iter != uniqueIdCvMap_.end()) {
        uniqueIdCvMap_.erase(iter);
    }

    if (outputMap_.find(uniqueId) != outputMap_.end()) {
        outPut = outputMap_.at(uniqueId);
        if (outPut == nullptr) {
            LogWarn << "GetResultWithUniqueId return null.";
            return nullptr;
        }
        ptr = outPut->dataOutput;
        outputMap_.erase(uniqueId);
        delete outPut;
        outPut = nullptr;
        outputMapCond_.notify_one();
    } else {
        LogWarn << "Failed to find " << uniqueId << " in outputMap";
    }

    return ptr;
}

// getResultWithUniqueId with multi-output
std::vector<MxstDataOutput*> MxsmStream::GetMultiResultWithUniqueId(uint64_t uniqueId, unsigned int timeOutMs)
{
    std::vector<MxstDataOutput*> errorVec;
    APP_ERROR ret = CheckGetFuncTransMode(MXST_TRANSMISSION_UNIQUE_ID_MULTI, "GetMultiResultWithUniqueId");
    if (ret != APP_ERR_OK) {
        LogWarn << "Call GetMultiResultWithUniqueId() failed. return a vector with size 0.";
        return errorVec;
    }

    std::vector<MxstDataOutput*> outputVec;
    std::unique_lock<std::mutex> mapMutex(dataMapMutex_);
    if (uniqueIdCvMap_.find(uniqueId) != uniqueIdCvMap_.end()) {
        uniqueIdCvMap_[uniqueId]->wait_for(mapMutex, std::chrono::milliseconds(timeOutMs));
    }
    auto iter = uniqueIdCvMap_.find(uniqueId);
    if (iter != uniqueIdCvMap_.end()) {
        uniqueIdCvMap_.erase(iter);
    }

    if (multiOutputMap_.find(uniqueId) != multiOutputMap_.end()) {
        auto ptrVec = multiOutputMap_.at(uniqueId);
        LogDebug << "the size of the output is " << ptrVec.size();
        if (ptrVec.size() == 0) {
            LogError << "GetResult with output return failed. push_back an empty MxstDataOutput."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            MxstDataOutput* emptyOut = 0;
            outputVec.push_back(emptyOut);
            return outputVec;
        }
        for (size_t i = 0; i < ptrVec.size(); i++) {
            LogDebug << "Now getting result of output[" << i << "].";
            outputVec.push_back(ptrVec[i]->dataOutput);
            delete ptrVec[i];
            ptrVec[i] = nullptr;
        }

        multiOutputMap_.erase(uniqueId);
        outputMapCond_.notify_one();
    } else {
        LogError << "Failed to find output in multiOutputMap. push_back a nullptr."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }

    return outputVec;
}

void MxsmStream::DropUniqueId(uint64_t uniqueId)
{
    std::lock_guard<std::mutex> resultStatusMapMutex(resultStatusMapMutex_);
    resultStatusMap_.insert(std::pair<uint64_t, MXST_RESULT_STATUS>(uniqueId, MXST_RESULT_STATUS_DROP));
}

APP_ERROR MxsmStream::SetElementProperty(const std::string& elementName,
                                         const std::string& propertyName,
                                         const std::string& propertyValue,
                                         bool dataSourceMeta)
{
    auto element = elementMap_.find(elementName);
    if (element == elementMap_.end()) {
        LogError << "streamName: \"" << streamName_ << "\" elementName: \"" << elementName << "\" not found"
                 << GetErrorInfo(APP_ERR_STREAM_ELEMENT_NOT_EXIST);
        return APP_ERR_STREAM_ELEMENT_NOT_EXIST;
    }
    if (!dataSourceMeta && DYNAMIC_PROPERTY_ALLOW_LIST.end() == std::find(DYNAMIC_PROPERTY_ALLOW_LIST.begin(),
        DYNAMIC_PROPERTY_ALLOW_LIST.end(), elementMap_[elementName]->factoryName_)) {
        LogError << "streamName: \"" << streamName_ << "\" elementName: \"" << elementName
                 << "\" doesn't allow to set dynamic property." << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID);
        return APP_ERR_STREAM_ELEMENT_INVALID;
    }
    bool needUnref = false;
    GObjectClass* klass = MxsmElement::GetElementClass(elementMap_[elementName]->factoryName_, needUnref);
    if (!klass) {
        LogError << "streamName: \"" << streamName_ << "\" elementName: \"" << elementName << "\" "
                 << "factoryName: \"" << elementMap_[elementName]->factoryName_ << "\" get element class failed."
                 << GetErrorInfo(APP_ERR_ELEMENT_INVALID_FACTORY);
        return APP_ERR_ELEMENT_INVALID_FACTORY;
    }
    auto ret = MxsmElement::ValidateElementSingleProperty(klass, propertyName, propertyValue);
    if (APP_ERR_OK != ret) {
        if (needUnref) {
            g_type_class_unref(klass);
        }
        LogError << "streamName: \"" << streamName_ << "\" elementName: \"" << elementName
                 << "\" propertyName: \"" << propertyName << "\" propertyValue: \"" << propertyValue
                 << "\" validate property failed." << GetErrorInfo(ret);
        return ret;
    }
    if (needUnref) {
        g_type_class_unref(klass);
    }
    if (!element->second->SetSingleProperty(propertyName, propertyValue)) {
        LogError << "streamName: \"" << streamName_ << "\" elementName: \"" << elementName
                 << "\" propertyName: \"" << propertyName << "\" propertyValue: \"" << propertyValue
                 << "\" set property failed." << GetErrorInfo(APP_ERR_ELEMENT_INVALID_PROPERTIES);
        return APP_ERR_ELEMENT_INVALID_PROPERTIES;
    }
    return APP_ERR_OK;
}

std::shared_ptr<MxstDataOutput> MxsmStream::GetResultSP(int outPluginId, const uint32_t& msTimeOut)
{
    MxstDataOutput* output = GetResult(outPluginId, msTimeOut);
    std::shared_ptr<MxstDataOutput> outputSp(output);
    return outputSp;
}

std::shared_ptr<MxstDataOutput> MxsmStream::GetResultWithUniqueIdSP(uint64_t uniqueId, uint32_t timeOutMs)
{
    MxstDataOutput* output = GetResultWithUniqueId(uniqueId, timeOutMs);
    std::shared_ptr<MxstDataOutput> outputSp(output);
    return outputSp;
}

std::vector<std::shared_ptr<MxstDataOutput>> MxsmStream::GetMultiResultWithUniqueIdSP(uint64_t uniqueId,
    uint32_t timeOutMs)
{
    std::vector<MxstDataOutput*> outputVec = GetMultiResultWithUniqueId(uniqueId, timeOutMs);
    std::vector<std::shared_ptr<MxstDataOutput>> outputVecSp;
    for (auto i : outputVec) {
        std::shared_ptr<MxstDataOutput> temSp(i);
        outputVecSp.push_back(temSp);
    }
    return outputVecSp;
}
}  // namespace MxStream
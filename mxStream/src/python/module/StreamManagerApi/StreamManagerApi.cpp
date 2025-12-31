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

#include "StreamManagerApi/StreamManagerApi.h"

#include "StreamManagerApi/StreamManagerInner.h"
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"

#include "PyUtils/PyDataHelper.h"

namespace PyStreamManager {
StreamManagerApi::StreamManagerApi() : mxStreamManager_(nullptr)
{}

StreamManagerApi::~StreamManagerApi()
{}

int StreamManagerApi::InitManager(const std::vector<std::string> &argStrings)
{
    PyThreadState *pyState = PyEval_SaveThread();
    if (mxStreamManager_ == nullptr) {
        MxStream::MxStreamManager* mxStreamManager = new (std::nothrow) MxStream::MxStreamManager;
        if (mxStreamManager == nullptr) {
            LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            PyEval_RestoreThread(pyState);
            return APP_ERR_COMM_INIT_FAIL;
        }
        mxStreamManager_ = std::unique_ptr<MxStream::MxStreamManager>(mxStreamManager);
    }
    APP_ERROR ret = mxStreamManager_->InitManager(argStrings);
    PyEval_RestoreThread(pyState);
    return ret;
}

int StreamManagerApi::CreateMultipleStreams(const std::string &streamsConfig) const
{
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed. Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_PYTHON_INIT_ERROR;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    APP_ERROR ret = mxStreamManager_->CreateMultipleStreams(streamsConfig);
    PyEval_RestoreThread(pyState);
    return ret;
}

int StreamManagerApi::CreateMultipleStreamsFromFile(const std::string &streamsFilePath) const
{
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed. Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_PYTHON_INIT_ERROR;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    APP_ERROR ret = mxStreamManager_->CreateMultipleStreamsFromFile(streamsFilePath);
    PyEval_RestoreThread(pyState);
    return ret;
}

int StreamManagerApi::StopStream(const std::string &streamName) const
{
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed. Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_PYTHON_INIT_ERROR;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    APP_ERROR ret = mxStreamManager_->StopStream(streamName);
    PyEval_RestoreThread(pyState);
    return ret;
}

int StreamManagerApi::DestroyAllStreams() const
{
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed. Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_PYTHON_INIT_ERROR;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    APP_ERROR ret = mxStreamManager_->DestroyAllStreams();
    PyEval_RestoreThread(pyState);
    return ret;
}

int StreamManagerApi::SendData(const std::string &streamName, const int &inPluginId, const MxDataInput &dataInput) const
{
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed. Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_PYTHON_INIT_ERROR;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    MxStream::MxstDataInput mxstDataInput;
    SendDataComm(mxstDataInput, dataInput);
    APP_ERROR ret = mxStreamManager_->SendData(streamName, inPluginId, mxstDataInput);
    PyEval_RestoreThread(pyState);
    return ret;
}

int StreamManagerApi::SendData(
    const std::string &streamName, const std::string &elementName, const MxDataInput &dataInput) const
{
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed. Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_PYTHON_INIT_ERROR;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    MxStream::MxstDataInput mxstDataInput;
    SendDataComm(mxstDataInput, dataInput);
    APP_ERROR ret = mxStreamManager_->SendData(streamName, elementName, mxstDataInput);
    PyEval_RestoreThread(pyState);
    return ret;
}

int StreamManagerApi::SendProtobuf(
    const std::string &streamName, const int &inPluginId, const std::vector<MxProtobufIn> &protobufVec) const
{
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed, Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_PYTHON_INIT_ERROR;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    std::vector<MxStream::MxstProtobufIn> protoVec;
    int ret = SendProtobufComm(protoVec, protobufVec);
    if (ret != APP_ERR_OK) {
        LogError << "StreamManagerApi SendProtobufComm failed." << GetErrorInfo(ret);
        PyEval_RestoreThread(pyState);
        return ret;
    }
    ret = mxStreamManager_->SendProtobuf(streamName, inPluginId, protoVec);
    PyEval_RestoreThread(pyState);
    return ret;
}

int StreamManagerApi::SendProtobuf(
    const std::string &streamName, const std::string &elementName, const std::vector<MxProtobufIn> &protobufVec) const
{
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed, Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_PYTHON_INIT_ERROR;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    std::vector<MxStream::MxstProtobufIn> protoVec;
    APP_ERROR ret = SendProtobufComm(protoVec, protobufVec);
    if (ret != APP_ERR_OK) {
        LogError << "StreamManagerApi SendProtobufComm failed." << GetErrorInfo(ret);
        PyEval_RestoreThread(pyState);
        return ret;
    }
    ret = mxStreamManager_->SendProtobuf(streamName, elementName, protoVec);
    PyEval_RestoreThread(pyState);
    return ret;
}

int StreamManagerApi::SendData(const std::string &streamName, const std::string &elementName,
    const std::vector<MxMetadataInput> &metadataVec, const MxBufferInput &databuffer) const
{
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed. Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_PYTHON_INIT_ERROR;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    MxStream::MxstBufferInput mxstDataInput;
    mxstDataInput.dataSize = static_cast<int>(databuffer.data.size());
    mxstDataInput.dataPtr = (uint32_t *)databuffer.data.c_str();
    mxstDataInput.mxpiFrameInfo.ParseFromString(databuffer.mxpiFrameInfo);
    mxstDataInput.mxpiVisionInfo.ParseFromString(databuffer.mxpiVisionInfo);

    std::vector<MxStream::MxstMetadataInput> metadataCppVec;
    for (const auto &metadataIn : metadataVec) {
        MxStream::MxstMetadataInput metadataTemp;
        metadataTemp.dataSource = metadataIn.dataSource;
        metadataTemp.messagePtr =
            PyStream::PyDataHelper::GetProtobufPtr(metadataIn.dataType, metadataIn.serializedMetadata);
        if (metadataTemp.messagePtr == nullptr) {
            LogError << "Transform string to messagePtr failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            PyEval_RestoreThread(pyState);
            return APP_ERR_COMM_FAILURE;
        }
        PyStream::PyDataHelper::ReBuildMxpiList(metadataIn.dataType, metadataTemp.messagePtr);
        if (metadataTemp.messagePtr == nullptr) {
            LogError << "Transform messageStr to messagePtr failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            PyEval_RestoreThread(pyState);
            return APP_ERR_COMM_FAILURE;
        }
        metadataCppVec.push_back(metadataTemp);
    }
    APP_ERROR ret = mxStreamManager_->SendData(streamName, elementName, metadataCppVec, mxstDataInput);
    PyEval_RestoreThread(pyState);
    return ret;
}

std::vector<MxProtobufOut> StreamManagerApi::GetProtobuf(
    const std::string &streamName, const int &outPluginId, const std::vector<std::string> &keyVec) const
{
    std::vector<MxProtobufOut> outputVec;
    outputVec.clear();
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed, Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        MxProtobufOut outPut;
        outPut.errorCode = APP_PYTHON_INIT_ERROR;
        outPut.messageName = "The initialization is not performed, Call the InitManager method first.";
        outputVec.push_back(outPut);
        return outputVec;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    auto dataBufferVec = mxStreamManager_->GetProtobuf(streamName, outPluginId, keyVec);
    if (dataBufferVec.size() == 0) {
        LogWarn << "The input parameter outPluginId is invalid, get output size is 0. outPluginId = " << outPluginId;
        PyEval_RestoreThread(pyState);
        return outputVec;
    }
    for (const auto &dataBuffer : dataBufferVec) {
        MxProtobufOut output;
        output.errorCode = dataBuffer.errorCode;
        output.messageName = dataBuffer.messageName;
        if (dataBuffer.messagePtr != nullptr) {
            std::shared_ptr<google::protobuf::Message> messagePtr =
                PyStream::PyDataHelper::ParseMessagePtrToString(dataBuffer.messagePtr);
            if (messagePtr == nullptr) {
                LogError << "Transform messagePtr to messageStr failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
                continue;
            }
            output.messageBuf = PyStream::PyDataHelper::GetMessageString(messagePtr);
            outputVec.push_back(output);
        }
    }
    PyEval_RestoreThread(pyState);
    return outputVec;
}

MxBufferAndMetadataOutput StreamManagerApi::GetResult(const std::string &streamName, const std::string &elementName,
    const std::vector<std::string> &dataSourceVec, const unsigned int &msTimeOut) const
{
    MxBufferAndMetadataOutput metaAndBufferOut;
    metaAndBufferOut.metadataVec.clear();
    if (mxStreamManager_ == nullptr) {
        std::string errorMsg = "The initialization is not performed, Call the InitManager method first.";
        LogError << errorMsg << GetErrorInfo(APP_ERR_COMM_FAILURE);
        metaAndBufferOut.SetErrorInfo(APP_ERR_STREAM_NOT_EXIST, errorMsg);
        return metaAndBufferOut;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    MxBufferOutput bufferOutput;
    MxStream::MxstBufferAndMetadataOutput metaAndBuffer =
        mxStreamManager_->GetResult(streamName, elementName, dataSourceVec, msTimeOut);
    if (metaAndBuffer.errorCode != APP_ERR_OK) {
        metaAndBufferOut.SetErrorInfo(metaAndBuffer.errorCode, metaAndBuffer.errorMsg);
        PyEval_RestoreThread(pyState);
        return metaAndBufferOut;
    }

    bufferOutput.errorCode = metaAndBuffer.bufferOutput->errorCode;
    if (metaAndBuffer.bufferOutput->dataPtr != nullptr && metaAndBuffer.bufferOutput->dataSize != 0) {
        std::string result((char *)metaAndBuffer.bufferOutput->dataPtr, metaAndBuffer.bufferOutput->dataSize);
        bufferOutput.data = result;
    }
    metaAndBufferOut.bufferOutput = bufferOutput;
    for (const auto &dataBuffer : metaAndBuffer.metadataVec) {
        MxMetadataOutput output;
        output.dataType = dataBuffer.dataType;
        if (dataBuffer.dataPtr != nullptr) {
            std::shared_ptr<google::protobuf::Message> messagePtr =
                PyStream::PyDataHelper::ParseMessagePtrToString(dataBuffer.dataPtr);
            if (messagePtr == nullptr) {
                LogError << "Transform messagePtr to messageStr failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
                continue;
            }
            output.serializedMetadata = PyStream::PyDataHelper::GetMessageString(messagePtr);
            metaAndBufferOut.metadataVec.push_back(output);
        }
    }
    PyEval_RestoreThread(pyState);
    return metaAndBufferOut;
}

MxDataOutput StreamManagerApi::GetResult(
    const std::string &streamName, const int &outPluginId, const unsigned int &msTimeOut) const
{
    MxDataOutput output;
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed. Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        output.errorCode = APP_PYTHON_INIT_ERROR;
        return output;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    MxStream::MxstDataOutput *ret = mxStreamManager_->GetResult(streamName, outPluginId, msTimeOut);
    if (ret == nullptr) {
        output.errorCode = APP_ERR_COMM_INNER;
        output.data = GetErrorInfo(APP_ERR_COMM_INNER);
        PyEval_RestoreThread(pyState);
        return output;
    }

    output.errorCode = ret->errorCode;
    if (ret->dataPtr == nullptr || ret->dataSize == 0) {
        output.data = GetErrorInfo(ret->errorCode);
    } else {
        std::string result((char *)ret->dataPtr, ret->dataSize);
        output.data = result;
    }
    delete ret;
    ret = nullptr;
    PyEval_RestoreThread(pyState);
    return output;
}

int StreamManagerApi::SendDataWithUniqueId(
    const std::string &streamName, const int &inPluginId, const MxDataInput &dataInput) const
{
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed. Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_PYTHON_INIT_ERROR;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    MxStream::MxstDataInput mxstDataInput;
    SendDataWithUniqueIdComm(mxstDataInput, dataInput);

    uint64_t uniqueId = 0;
    APP_ERROR ret = mxStreamManager_->SendDataWithUniqueId(streamName, inPluginId, mxstDataInput, uniqueId);
    if (ret != APP_ERR_OK) {
        LogError << "SendDataWithUniqueId error." << GetErrorInfo(ret);
        PyEval_RestoreThread(pyState);
        return APP_PYTHON_INIT_ERROR;
    }
    PyEval_RestoreThread(pyState);
    return uniqueId;
}

int StreamManagerApi::SendDataWithUniqueId(
    const std::string &streamName, const std::string &elementName, const MxDataInput &dataInput) const
{
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed. Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_PYTHON_INIT_ERROR;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    MxStream::MxstDataInput mxstDataInput;
    SendDataWithUniqueIdComm(mxstDataInput, dataInput);

    uint64_t uniqueId = 0;
    APP_ERROR ret = mxStreamManager_->SendDataWithUniqueId(streamName, elementName, mxstDataInput, uniqueId);
    if (ret != APP_ERR_OK) {
        LogError << "SendDataWithUniqueId error." << GetErrorInfo(ret);
        PyEval_RestoreThread(pyState);
        return APP_PYTHON_INIT_ERROR;
    }
    PyEval_RestoreThread(pyState);
    return uniqueId;
}

MxDataOutput StreamManagerApi::GetResultWithUniqueId(
    const std::string &streamName, unsigned long uniqueId, unsigned int timeOutInMs) const
{
    MxDataOutput output;
    if (mxStreamManager_ == nullptr) {
        LogError << "The initialization is not performed. Call the InitManager method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        output.errorCode = APP_PYTHON_INIT_ERROR;
        return output;
    }
    PyThreadState *pyState = PyEval_SaveThread();
    MxStream::MxstDataOutput *ret = mxStreamManager_->GetResultWithUniqueId(streamName, uniqueId, timeOutInMs);
    if (ret == nullptr) {
        output.errorCode = APP_ERR_COMM_INNER;
        output.data = GetErrorInfo(APP_ERR_COMM_INNER);
        PyEval_RestoreThread(pyState);
        return output;
    }

    output.errorCode = ret->errorCode;
    if (ret->dataPtr == nullptr || ret->dataSize == 0) {
        output.data = GetErrorInfo(ret->errorCode);
    } else {
        std::string result((char *)ret->dataPtr, ret->dataSize);
        output.data = result;
    }
    PyEval_RestoreThread(pyState);

    delete ret;
    ret = nullptr;
    return output;
}
}  // namespace PyStream
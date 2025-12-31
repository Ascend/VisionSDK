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

#include "Stream/PyStream/StreamUtils.h"
#include "MxBase/Log/Log.h"
#include "PyUtils/PyDataHelper.h"

namespace {
constexpr int MAX_PLUGIN_NUM = 1024;
constexpr int MIN_PLUGIN_NUM = 1;
}  // namespace
namespace PyStream {
std::string ToJson(const std::shared_ptr<MxStream::Stream> &stream)
{
    if (stream == nullptr) {
        LogError << "The stream does not exist." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return "";
    }
    return stream->ToJson();
}

void SetDeviceId(const std::shared_ptr<MxStream::Stream> &stream, const std::string &deviceId)
{
    if (stream == nullptr) {
        LogError << "The stream does not exist." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_FAILURE));
    }
    stream->SetDeviceId(deviceId);
}

void Build(const std::shared_ptr<MxStream::Stream> &stream)
{
    if (stream == nullptr) {
        LogError << "The stream does not exist." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_FAILURE));
    }
    auto ret = stream->Build();
    if (ret != APP_ERR_OK) {
        LogError << "build stream failed." << GetErrorInfo(ret);
        throw std::runtime_error("build stream failed.");
    }
}

void Stop(const std::shared_ptr<MxStream::Stream> &stream)
{
    if (stream == nullptr) {
        LogError << "The stream does not exist." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_FAILURE));
    }
    auto ret = stream->Stop();
    if (ret != APP_ERR_OK) {
        LogError << "Stop stream failed." << GetErrorInfo(ret);
        throw std::runtime_error("Stop stream failed.");
    }
}

void SendData(const std::shared_ptr<MxStream::Stream> &stream, const std::string &elementName,
    const std::vector<MetadataInput> &metadataVec, const BufferInput &dataBuffer)
{
    if (stream == nullptr) {
        LogError << "The initialization is not performed. Call the build method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error("build stream first.");
    }
    PyThreadState *pyState = PyEval_SaveThread();

    MxStream::MxstBufferInput mxstDataInput;
    PyDataHelper::PyBufferToC(dataBuffer, mxstDataInput);
    std::vector<MxStream::MxstMetadataInput> metadataCppVec;
    for (const auto &metadataIn : metadataVec) {
        MxStream::MxstMetadataInput metadataTemp;
        APP_ERROR ret = PyDataHelper::PyMetadataToC(metadataIn, metadataTemp);
        if (ret != APP_ERR_OK) {
            PyEval_RestoreThread(pyState);
            LogError << "Failed to copy metadata." << GetErrorInfo(ret);
            throw std::runtime_error("Failed to copy metadata.");
        }
        metadataCppVec.push_back(metadataTemp);
    }

    auto ret = stream->SendData(elementName, metadataCppVec, mxstDataInput);
    if (ret != APP_ERR_OK) {
        PyEval_RestoreThread(pyState);
        LogError << "Failed to send data." << GetErrorInfo(ret);
        throw std::runtime_error("Failed to send data.");
    }
    PyEval_RestoreThread(pyState);
}

void SendMultiDataWithUniqueId(const std::shared_ptr<MxStream::Stream> &stream, int inPluginNum,
    std::vector<MxDataInput> &dataInputVec, uint64_t &uniqueId)
{
    if (stream == nullptr) {
        LogError << "The initialization is not performed. Call the build method first."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error("build stream first.");
    }

    if (inPluginNum < MIN_PLUGIN_NUM || inPluginNum > MAX_PLUGIN_NUM) {
        LogError << "The inPluginNum [" << inPluginNum << "] is invalid, which should be in range [" << MIN_PLUGIN_NUM
                 << ", " << MAX_PLUGIN_NUM << "]." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error("inPluginNum is invalid.");
    }

    PyThreadState *pyState = PyEval_SaveThread();

    auto inPluginIdVec = PyDataHelper::NumToInPluginVec(inPluginNum);

    std::vector<MxStream::MxstDataInput> mxstDataInputVec;
    for (const auto &dataIn : dataInputVec) {
        MxStream::MxstDataInput dataTemp;
        PyDataHelper::PyMxdataInputToC(dataIn, dataTemp);
        mxstDataInputVec.push_back(dataTemp);
    }

    auto ret = stream->SendMultiDataWithUniqueId(inPluginIdVec, mxstDataInputVec, uniqueId);
    if (ret != APP_ERR_OK) {
        PyEval_RestoreThread(pyState);
        LogError << "Failed to send data." << GetErrorInfo(ret);
        throw std::runtime_error("Failed to send data.");
    }
    PyEval_RestoreThread(pyState);
}

DataOutput GetResult(const std::shared_ptr<MxStream::Stream> &stream, const std::string &elementName,
    const std::vector<std::string> &dataSourceVec, const int &msTimeOut)
{
    DataOutput dataOutput;
    dataOutput.metadata_list.clear();
    if (stream == nullptr) {
        std::string errorMsg = "The initialization is not performed, Call the build method first.";
        LogError << errorMsg << GetErrorInfo(APP_ERR_COMM_FAILURE);
        dataOutput.set_error_info(APP_ERR_STREAM_NOT_EXIST, errorMsg);
        return dataOutput;
    }
    PyThreadState *pyState = PyEval_SaveThread();

    MxStream::MxstBufferAndMetadataOutput metaAndBuffer = stream->GetResult(elementName, dataSourceVec, msTimeOut);
    if (metaAndBuffer.errorCode != APP_ERR_OK) {
        dataOutput.set_error_info(metaAndBuffer.errorCode, metaAndBuffer.errorMsg);
        PyEval_RestoreThread(pyState);
        return dataOutput;
    }

    PyDataHelper::OutputToPy(metaAndBuffer, dataOutput);
    PyEval_RestoreThread(pyState);
    return dataOutput;
}

std::vector<MxDataOutput> GetMultiResultWithUniqueId(
    const std::shared_ptr<MxStream::Stream> &stream, uint64_t uniqueId, unsigned int timeOutMs)
{
    std::vector<MxDataOutput> mxDataOutputVec;
    MxDataOutput mxDataOutputErr;
    if (stream == nullptr) {
        std::string errorMsg = "The initialization is not performed, Call the build method first.";
        LogError << errorMsg << GetErrorInfo(APP_ERR_COMM_FAILURE);
        mxDataOutputErr.errorCode = APP_ERR_STREAM_NOT_EXIST;
        mxDataOutputVec.push_back(mxDataOutputErr);
        return mxDataOutputVec;
    }
    PyThreadState *pyState = PyEval_SaveThread();

    auto mxstDataOutputVec = stream->GetMultiResultWithUniqueId(uniqueId, timeOutMs);
    if (mxstDataOutputVec.size() == 0) {
        std::string errorMsg = "Get empty result from GetMultiResultWithUniqueId.";
        LogError << errorMsg << GetErrorInfo(APP_ERR_COMM_FAILURE);
        mxDataOutputErr.errorCode = APP_ERR_COMM_FAILURE;
        mxDataOutputVec.push_back(mxDataOutputErr);
        PyEval_RestoreThread(pyState);
        return mxDataOutputVec;
    }
    if (mxstDataOutputVec[0]->errorCode != APP_ERR_OK) {
        std::string errorMsg = "Failed to call GetMultiResultWithUniqueId.";
        LogError << errorMsg << GetErrorInfo(APP_ERR_COMM_FAILURE);
        mxDataOutputErr.errorCode = mxstDataOutputVec[0]->errorCode;
        mxDataOutputVec.push_back(mxDataOutputErr);
        PyEval_RestoreThread(pyState);
        return mxDataOutputVec;
    }

    for (const auto dataOut : mxstDataOutputVec) {
        MxDataOutput dataTemp;
        PyDataHelper::MxstDataOutputToPy(dataOut, dataTemp);
        mxDataOutputVec.push_back(dataTemp);
    }
    PyEval_RestoreThread(pyState);
    return mxDataOutputVec;
}
}  // namespace PyStream
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
* Description: Manage the tensor of AscendStream.
* Author: MindX SDK
* Create: 2024
* History: NA
*/
#include <algorithm>
#include "StreamTensorManager.h"

namespace MxBase {

StreamTensorManager StreamTensorManager::instance_;

StreamTensorManager::StreamTensorManager() noexcept
{
}

StreamTensorManager::~StreamTensorManager()
{
}

bool StreamTensorManager::CheckStreamResource(aclrtStream stream)
{
    return streamMutexMap_.count(stream) == 0 || streamTensorMap_.count(stream) == 0 ||
            streamMutexMap_[stream] == nullptr ||  streamTensorMap_[stream] == nullptr;
}

APP_ERROR StreamTensorManager::DeInit()
{
    streamTensorMap_.clear();
    streamMutexMap_.clear();
    return APP_ERR_OK;
}

APP_ERROR StreamTensorManager::AddStream(aclrtStream stream)
{
    std::lock_guard<std::mutex> lock(streamMtx_);
    // only create value again if the stream is already the key in the streamMutexMap_ and streamTensorMap_
    if (streamMutexMap_.count(stream) != 0 && streamTensorMap_.count(stream) != 0 &&
        streamMutexMap_[stream] == nullptr && streamTensorMap_[stream] == nullptr) {
        streamMutexMap_[stream] = std::make_shared<std::mutex>();
        streamTensorMap_[stream] = std::make_shared<std::list<Tensor>>();
    // create key-value pairs and put them into the corresponding map
    } else if (streamMutexMap_.count(stream) == 0 && streamTensorMap_.count(stream) == 0) {
        streamMutexMap_.emplace(stream, std::make_shared<std::mutex>());
        streamTensorMap_.emplace(stream, std::make_shared<std::list<Tensor>>());
    } else {
    // abnormal case
        LogError << "Stream resource is abnormal in AddStream function." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (CheckStreamResource(stream)) {
        LogError << "Stream resource is not properly initialized." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

std::shared_ptr<std::mutex> StreamTensorManager::GetStreamMutex(aclrtStream stream)
{
    if (CheckStreamResource(stream)) {
        LogError << "Stream resource is abnormal in GetStreamMutex function." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return unusedMtx_;
    }
    return streamMutexMap_[stream];
}

APP_ERROR StreamTensorManager::GetStreamTensorListLastTensor(aclrtStream stream, MxBase::Tensor& tensor)
{
    if (CheckStreamResource(stream)) {
        LogError << "Stream resource is abnormal in GetStreamTensorList function."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    std::lock_guard<std::mutex> lock(*streamMutexMap_[stream]);
    if (!streamTensorMap_[stream]->empty()) {
        tensor = streamTensorMap_[stream]->back();
    }
    return APP_ERR_OK;
}

APP_ERROR StreamTensorManager::DeleteStream(aclrtStream stream)
{
    if (CheckStreamResource(stream)) {
        LogError << "Stream resource is abnormal in DeleteStream function." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    std::lock_guard<std::mutex> lock(streamMtx_);
    streamMutexMap_[stream] = nullptr;
    streamTensorMap_[stream] = nullptr;
    return APP_ERR_OK;
}

APP_ERROR StreamTensorManager::ClearTensorsByStream(aclrtStream stream)
{
    if (CheckStreamResource(stream)) {
        LogError << "Stream resource is abnormal in ClearTensorsByStream function."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    std::lock_guard<std::mutex> lock(*streamMutexMap_[stream]);
    streamTensorMap_[stream]->clear();
    return APP_ERR_OK;
}

APP_ERROR StreamTensorManager::AddTensor(aclrtStream stream, const MxBase::Tensor& tensor)
{
    if (CheckStreamResource(stream)) {
        LogError << "Stream resource is abnormal in AddTensor function." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    std::lock_guard<std::mutex> lock(*streamMutexMap_[stream]);
    streamTensorMap_[stream]->push_back(tensor);
    return APP_ERR_OK;
}

APP_ERROR StreamTensorManager::DeleteTensor(aclrtStream stream, MxBase::Tensor& tensor)
{
    if (CheckStreamResource(stream)) {
        LogError << "Stream resource is abnormal in DeleteTensor function." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    std::lock_guard<std::mutex> lock(*streamMutexMap_[stream]);
    auto it = std::find(streamTensorMap_[stream]->begin(), streamTensorMap_[stream]->end(), tensor);
    if (it != streamTensorMap_[stream]->end()) {
        streamTensorMap_[stream]->erase(streamTensorMap_[stream]->begin(), ++it);
    } else {
        LogWarn << "Tensor is not found in the relative tensor list of streamTensorMap_";
    }
    return APP_ERR_OK;
}
}
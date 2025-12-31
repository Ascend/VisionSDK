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
 * Description: SsdMobilenetFpn_Mindspore model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "ObjectPostProcessors/SsdMobilenetFpnMindsporePost.h"
#include "MxBase/Log/Log.h"
#include "SsdMobilenetFpnMindsporePostDptr.hpp"


namespace MxBase {
SsdMobilenetFpnMindsporePost::SsdMobilenetFpnMindsporePost()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::SsdMobilenetFpnMindsporePostDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create SsdMobilenetFpnMindsporePostDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}


SsdMobilenetFpnMindsporePost::SsdMobilenetFpnMindsporePost(const SsdMobilenetFpnMindsporePost &other)
    : MxBase::ObjectPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::SsdMobilenetFpnMindsporePostDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create SsdMobilenetFpnMindsporePostDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    *dPtr_ = *(other.dPtr_);
}

SsdMobilenetFpnMindsporePost& SsdMobilenetFpnMindsporePost::operator=(const SsdMobilenetFpnMindsporePost &other)
{
    if (this == &other) {
        return *this;
    }
    ObjectPostProcessBase::operator=(other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::SsdMobilenetFpnMindsporePostDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create SsdMobilenetFpnMindsporePostDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    *dPtr_ = *(other.dPtr_);
    return *this;
}

uint64_t SsdMobilenetFpnMindsporePost::GetCurrentVersion()
{
    return MINDX_SDK_VERSION;
}

APP_ERROR SsdMobilenetFpnMindsporePost::CheckDptr()
{
    LogDebug << "SsdMobilenetFpnMindsporePost CheckDptr: checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR SsdMobilenetFpnMindsporePost::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogDebug << "Start to Init SsdMobilenetFpnMindsporePost.";
    APP_ERROR ret = ObjectPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in ObjectPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = configData_.GetFileValue<float>("IOU_THRESH", dPtr_->iouThresh_, 0.0f, 1.0f);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read iouThresh_ from config, default value("
        << dPtr_->DEFAULT_IOU_THRESH << ") will be used as iouThresh_.";
        LogError << "Fail to superInit in ObjectPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "End to Init SsdMobilenetFpnMindsporePost.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

APP_ERROR SsdMobilenetFpnMindsporePost::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR SsdMobilenetFpnMindsporePost::Process(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<ObjectInfo>> &objectInfos, const std::vector<ResizedImageInfo> &resizedImageInfos,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Start to Process SsdMobilenetFpnMindsporePost.";
    APP_ERROR ret = APP_ERR_OK;
    auto inputs = tensors;
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = dPtr_->CheckAndMoveTensors(inputs);
    if (ret != APP_ERR_OK) {
        LogError << "MoveTensorsAndCheck failed." << GetErrorInfo(ret);
        return ret;
    }
    auto batchSize = static_cast<size_t>(tensors[0].GetShape()[0]);
    if (resizedImageInfos.size() != batchSize) {
        LogError << "Check input vector size failed, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ret = dPtr_->ObjectDetectionOutput(inputs, objectInfos, resizedImageInfos, cropRoiBoxes_);
    if (ret != APP_ERR_OK) {
        LogError << "ObjectDetectionOutput failed." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "End to Process SsdMobilenetFpnMindsporePost.";
    return APP_ERR_OK;
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::SsdMobilenetFpnMindsporePost> GetObjectInstance()
{
    LogInfo << "Begin to get SsdMobilenetFpnMindsporePost instance.";
    auto instance = MemoryHelper::MakeShared<SsdMobilenetFpnMindsporePost>();
    if (instance == nullptr) {
        LogError << "Create SsdMobilenetFpnMindsporePost object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get SsdMobilenetFpnMindsporePost instance.";
    return instance;
}
}
#endif
}

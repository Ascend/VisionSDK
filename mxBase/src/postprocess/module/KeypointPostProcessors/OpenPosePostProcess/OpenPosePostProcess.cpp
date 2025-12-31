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
 * Description: OpenPose model post-processing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "KeypointPostProcessors/OpenPosePostProcess.h"
#include "OpenPosePostProcessDptr.hpp"
#include "MxBase/Log/Log.h"

namespace MxBase {
using namespace std;

OpenPosePostProcess::OpenPosePostProcess()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::OpenPosePostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create OpenPosePostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

OpenPosePostProcess::OpenPosePostProcess(const OpenPosePostProcess &other) : MxBase::KeypointPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::OpenPosePostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create OpenPosePostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    *dPtr_ = *(other.dPtr_);
}

OpenPosePostProcess& OpenPosePostProcess::operator=(const OpenPosePostProcess &other)
{
    if (this == &other) {
        return *this;
    }
    KeypointPostProcessBase::operator=(other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::OpenPosePostProcessDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create OpenPosePostProcessDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    dPtr_->keyPointNum_ =  other.dPtr_->keyPointNum_;
    dPtr_->modelType_ = other.dPtr_->modelType_;
    dPtr_->filterSize_ = other.dPtr_->filterSize_;
    dPtr_->sigma_ = other.dPtr_->sigma_;
    return *this;
}

APP_ERROR OpenPosePostProcess::CheckDptr()
{
    LogDebug << "OpenPosePostProcess CheckDptr(). Checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR OpenPosePostProcess::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogDebug << "Start to Init OpenPosePostProcess.";
    APP_ERROR ret = KeypointPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to superInit in KeypointPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    dPtr_->GetOpenPoseConfig();
    LogDebug << "End to Init OpenPosePostProcess.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

APP_ERROR OpenPosePostProcess::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR OpenPosePostProcess::Process(const std::vector<TensorBase>& tensors,
                                       std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos,
                                       const std::vector<ResizedImageInfo>& resizedImageInfos,
                                       const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Start to Process OpenPosePostProcess.";
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

    ret = dPtr_->Upsample(inputs, resizedImageInfos, keyPointInfos);
    if (ret != APP_ERR_OK) {
        LogError  << "Upsample failed." << GetErrorInfo(ret);
        return ret;
    }

    LogKeyPointInfos(keyPointInfos);
    LogDebug << "End to Process OpenPosePostProcess.";
    return APP_ERR_OK;
}

uint64_t OpenPosePostProcess::GetCurrentVersion()
{
    return dPtr_->CURRENT_VERSION;
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::OpenPosePostProcess> GetKeypointInstance()
{
    LogInfo << "Begin to get OpenPosePostProcess instance.";
    auto instance = MemoryHelper::MakeShared<OpenPosePostProcess>();
    if (instance == nullptr) {
        LogError << "Create OpenPosePostProcess object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get OpenPosePostProcess instance.";
    return instance;
}
}
#endif
}


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
 * Description: HigherHRnet model post-processing.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#include "HigherHRnetPostProcessDptr.hpp"


namespace MxBase {
HigherHRnetPostProcess::HigherHRnetPostProcess()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::HigherHRnetPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create HigherHRnetPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

HigherHRnetPostProcess::HigherHRnetPostProcess(const HigherHRnetPostProcess &other)
    : MxBase::KeypointPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::HigherHRnetPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create HigherHRnetPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    *dPtr_ = *(other.dPtr_);
}

HigherHRnetPostProcess& HigherHRnetPostProcess::operator=(const HigherHRnetPostProcess &other)
{
    if (this == &other) {
        return *this;
    }
    KeypointPostProcessBase::operator=(other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::HigherHRnetPostProcessDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create HigherHRnetPostProcessDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    dPtr_->keyPointNum_ = other.dPtr_->keyPointNum_;
    dPtr_->channelNum_ = other.dPtr_->channelNum_;
    dPtr_->imageFeatHight_ = other.dPtr_->imageFeatHight_;
    dPtr_->imageFeatWidth_ = other.dPtr_->imageFeatWidth_;
    dPtr_->heightIndex_ = other.dPtr_->heightIndex_;
    dPtr_->widthIndex_ = other.dPtr_->widthIndex_;
    dPtr_->thresh_ = other.dPtr_->thresh_;
    dPtr_->orderIdx_ = other.dPtr_->orderIdx_;
    dPtr_->defaultDict_ = other.dPtr_->defaultDict_;
    return *this;
}

APP_ERROR HigherHRnetPostProcess::CheckDptr()
{
    LogDebug << "HigherHRnetPostProcess CheckDptr(). Checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR HigherHRnetPostProcess::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogDebug << "Start to Init HigherHRnetPostProcess.";
    APP_ERROR ret = KeypointPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to superInit in KeypointPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    configData_.GetFileValueWarn("KEYPOINT_NUM", dPtr_->keyPointNum_, 0x0, 0x64);
    configData_.GetFileValueWarn("SCORE_THRESH", dPtr_->thresh_, 0.0f, 1.0f);
    isInitConfig_ = true;
    return APP_ERR_OK;
}

APP_ERROR HigherHRnetPostProcess::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR HigherHRnetPostProcess::Process(const std::vector<TensorBase>& tensors,
                                          std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos,
                                          const std::vector<ResizedImageInfo>& resizedImageInfos,
                                          const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Start to Process HigherHRnetPostProcess.";
    APP_ERROR ret = APP_ERR_OK;
    auto inputs = tensors;
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = dPtr_->CheckAndMoveTensors(inputs);
    if (ret != APP_ERR_OK) {
        LogError << "CheckAndMoveTensors failed." << GetErrorInfo(ret);
        return ret;
    }
    auto batchSize = static_cast<size_t>(tensors[0].GetShape()[0]);
    if (resizedImageInfos.size() != batchSize) {
        LogError << "Check input vector size failed, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ret = dPtr_->KeyPointsDetect(tensors, resizedImageInfos, keyPointInfos);
    if (ret != APP_ERR_OK) {
        LogError << "Process HigherHRNet failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

uint64_t HigherHRnetPostProcess::GetCurrentVersion()
{
    return MINDX_SDK_VERSION;
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::HigherHRnetPostProcess> GetKeypointInstance()
{
    LogInfo << "Begin to get OHigherHRnetPostProcess instance.";
    auto instance = MemoryHelper::MakeShared<HigherHRnetPostProcess>();
    if (instance == nullptr) {
        LogError << "Create HigherHRnetPostProcess object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get HigherHRnetPostProcess instance.";
    return instance;
}
}
#endif
}
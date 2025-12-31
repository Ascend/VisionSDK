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
 * Description: Deeplabv3Post model post-processing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "SegmentPostProcessors/Deeplabv3Post.h"
#include "MxBase/Log/Log.h"
#include "Deeplabv3PostDptr.hpp"

namespace MxBase {
enum class FrameworkType {
    TENSORFLOW = 0,
    PYTORCH = 1,
    MINDSPORE = 2
};
Deeplabv3Post::Deeplabv3Post()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::Deeplabv3PostDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create Deeplabv3PostDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

Deeplabv3Post::~Deeplabv3Post() {}

Deeplabv3Post::Deeplabv3Post(const Deeplabv3Post &other) : MxBase::SemanticSegPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::Deeplabv3PostDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create Deeplabv3PostDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    *dPtr_ = *(other.dPtr_);
}

uint64_t Deeplabv3Post::GetCurrentVersion()
{
    return MINDX_SDK_VERSION;
}

Deeplabv3Post& Deeplabv3Post::operator=(const Deeplabv3Post &other)
{
    if (this == &other) {
        return *this;
    }
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::Deeplabv3PostDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create Deeplabv3PostDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    SemanticSegPostProcessBase::operator=(other);
    *dPtr_ = *(other.dPtr_);
    return *this;
}

APP_ERROR Deeplabv3Post::CheckDptr()
{
    LogDebug << "Deeplabv3Post CheckDptr: checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR Deeplabv3Post::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogDebug << "Start to Init Deeplabv3Post.";
    APP_ERROR ret = SemanticSegPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in SemanticSegPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = configData_.GetFileValue<int>("FRAMEWORK_TYPE", dPtr_->frameworkType_);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read FRAMEWORK_TYPE from config, default value(1) will be used as frameworkType_.";
    }
    LogDebug << "End to Init Deeplabv3Post.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

APP_ERROR Deeplabv3Post::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR Deeplabv3Post::Process(const std::vector<TensorBase>& tensors,
    std::vector<SemanticSegInfo>& semanticSegInfos, const std::vector<ResizedImageInfo>& resizedImageInfos,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Start to Process Deeplabv3Post.";
    APP_ERROR ret = APP_ERR_OK;
    auto inputs = tensors;
    if (resizedImageInfos.size() != tensors.size()) {
        LogError << "Check images error! Please check input of resizedImageInfos."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = dPtr_->CheckAndMoveTensors(inputs);
    if (ret != APP_ERR_OK) {
        LogError << "CheckAndMoveTensors failed." << GetErrorInfo(ret);
        return ret;
    }
    switch ((FrameworkType)(dPtr_->frameworkType_)) {
        case FrameworkType::TENSORFLOW:
            ret = dPtr_->TensorflowFwOutput(inputs, resizedImageInfos, semanticSegInfos);
            if (ret != APP_ERR_OK) {
                LogError << "Execute TensorflowFwOutput failed" << GetErrorInfo(ret);
                return ret;
            }
            break;
        case FrameworkType::PYTORCH:
            dPtr_->PytorchFwOutput(inputs, resizedImageInfos, semanticSegInfos);
            break;
        case FrameworkType::MINDSPORE:
            dPtr_->MindsporeFwOutput(inputs, resizedImageInfos, semanticSegInfos);
            break;
        default:
            LogError << "The modelType_(" << modelType_ << ") is not supported." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
    }
    LogDebug << "End to Process Deeplabv3Post.";
    return APP_ERR_OK;
}
#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::Deeplabv3Post> GetSemanticSegInstance()
{
    LogInfo << "Begin to get Deeplabv3Post instance.";
    auto instance = MemoryHelper::MakeShared<Deeplabv3Post>();
    if (instance == nullptr) {
        LogError << "Create Deeplabv3Post object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get Deeplabv3Post instance.";
    return instance;
}
}
#endif
}
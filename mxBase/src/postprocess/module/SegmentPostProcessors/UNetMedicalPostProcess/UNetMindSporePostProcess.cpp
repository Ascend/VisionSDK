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
 * Description: UNetMindSporePostProcess model post-processing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "SegmentPostProcessors/UNetMindSporePostProcess.h"
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <iostream>
#include "MxBase/Log/Log.h"
#include "UNetMindSporePostProcessDptr.hpp"

namespace MxBase {
UNetMindSporePostProcess::UNetMindSporePostProcess()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::UNetMindSporePostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create UNetMindSporePostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

UNetMindSporePostProcess::UNetMindSporePostProcess(const UNetMindSporePostProcess &other)
    : MxBase::SemanticSegPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::UNetMindSporePostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create UNetMindSporePostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    *dPtr_ = *(other.dPtr_);
}

UNetMindSporePostProcess& UNetMindSporePostProcess::operator=(const UNetMindSporePostProcess &other)
{
    if (this == &other) {
        return *this;
    }
    SemanticSegPostProcessBase::operator=(other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::UNetMindSporePostProcessDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create UNetMindSporePostProcessDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    *dPtr_ = *(other.dPtr_);
    return *this;
}

uint64_t UNetMindSporePostProcess::GetCurrentVersion()
{
    return MINDX_SDK_VERSION;
}

APP_ERROR UNetMindSporePostProcess::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogDebug << "Start to Init UNetMindSporePostProcess.";
    APP_ERROR ret = SemanticSegPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in SemanticSegPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = configData_.GetFileValue<uint32_t>("POST_TYPE", dPtr_->postType_, 0x0, 0x10);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "No POST_TYPE in config file, default value("
                                 << dPtr_->postType_ << ").";
    }
    ret = configData_.GetFileValue<uint32_t>("RESIZE_TYPE", dPtr_->resizeType_, 0x0, 0x10);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "No RESIZE_TYPE in config file, default value("
                << dPtr_->resizeType_ << ").";
    }
    LogDebug << "End to Init UNetMindSporePostProcess.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

APP_ERROR UNetMindSporePostProcess::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR UNetMindSporePostProcess::Process(const std::vector<TensorBase>& tensors,
    std::vector<SemanticSegInfo>& semanticSegInfos, const std::vector<ResizedImageInfo>& resizedImageInfos,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Start to Process UNetMindSporePostProcess.";
    APP_ERROR ret = APP_ERR_OK;
    if (tensors.empty() || tensors[0].GetShape().empty() || resizedImageInfos.size() != tensors[0].GetShape()[0]) {
        LogError << "check images error! Please check input of resizedImageInfos. resizedImageInfos.size("
                 << resizedImageInfos.size() << ") must be equal to batch size." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
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

    switch ((UNetMindSporePostProcessDptr::PostType)dPtr_->postType_) {
        LogInfo << dPtr_->postType_;
        case UNetMindSporePostProcessDptr::ARGMAX_RESIZE_NEAR:
            ret = dPtr_->ArgmaxSemanticSegOutput(inputs, resizedImageInfos, semanticSegInfos);
            break;
        case UNetMindSporePostProcessDptr::OM_ARGMAX_DICT_OUT:
            ret = dPtr_->ModelArgmaxDirectOutput(inputs, resizedImageInfos, semanticSegInfos);
            break;
        default:
            LogError << "The postType_(" << dPtr_->postType_<< ") is not supported."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
    }
    if (ret != APP_ERR_OK) {
        LogError << "Execute argmax output failed" << GetErrorInfo(ret);
        return ret;
    }

    if (dPtr_->resizeType_ == 1) {
        for (size_t i = 0; i < semanticSegInfos.size(); i++) {
            CoordinatesReduction(resizedImageInfos[i], semanticSegInfos[i]);
        }
    }

    LogDebug << "End to Process UNetMindSporePostProcess.";
    return APP_ERR_OK;
}

APP_ERROR UNetMindSporePostProcess::CheckDptr()
{
    LogDebug << "Processing CheckDptr(). Checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::UNetMindSporePostProcess> GetSemanticSegInstance()
{
    LogInfo << "Begin to get UNetMindSporePostProcess instance.";
    auto instance = MemoryHelper::MakeShared<UNetMindSporePostProcess>();
    if (instance == nullptr) {
        LogError << "Create UNetMindSporePostProcess object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get UNetMindSporePostProcess instance.";
    return instance;
}
}
#endif
}
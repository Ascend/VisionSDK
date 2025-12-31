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
 * Description: TransformerPostProcess model post-processing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */


#include "TextGenerationPostProcessors/TransformerPostProcess.h"
#include "MxBase/Log/Log.h"
#include "TransformerPostProcessDptr.hpp"

namespace MxBase {
TransformerPostProcess::TransformerPostProcess()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::TransformerPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create TransformerPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

TransformerPostProcess::TransformerPostProcess(const TransformerPostProcess &other)
    : MxBase::TextGenerationPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::TransformerPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create TransformerPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    *dPtr_ = *(other.dPtr_);
}

TransformerPostProcess &TransformerPostProcess::operator=(const TransformerPostProcess &other)
{
    if (this == &other) {
        return *this;
    }
    TextGenerationPostProcessBase::operator=(other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::TransformerPostProcessDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create TransformerPostProcessDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    dPtr_->breakIdx_ = other.dPtr_->breakIdx_;
    return *this;
}

APP_ERROR TransformerPostProcess::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogDebug << "Start to Init TransformerPostProcess.";
    APP_ERROR ret = TextGenerationPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in TextGenerationPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    configData_.GetFileValueWarn<uint32_t>("BREAK_INDEX", dPtr_->breakIdx_, 0x0, 0x1388);
    LogDebug << "End to Init TransformerPostProcess.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

APP_ERROR TransformerPostProcess::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR TransformerPostProcess::Process(const std::vector<TensorBase>& tensors, std::vector<TextsInfo>& textInfos,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Start to Process TransformerPostProcess.";
    APP_ERROR ret = APP_ERR_OK;
    auto inputs = tensors;
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = (dPtr_->CheckAndMoveTensors)(inputs);
    if (ret != APP_ERR_OK) {
        LogError << "CheckAndMoveTensors failed." << GetErrorInfo(ret);
        return ret;
    }
    (dPtr_->TextGenerationOutput)(tensors, textInfos);
    LogDebug << "End to Process TransformerPostProcess.";
    return APP_ERR_OK;
}

uint64_t TransformerPostProcess::GetCurrentVersion()
{
    return MINDX_SDK_VERSION;
}

APP_ERROR TransformerPostProcess::CheckDptr()
{
    LogDebug << "Processing CheckDptr(). Checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

extern "C" {
std::shared_ptr<MxBase::TransformerPostProcess> GetTextGenerationInstance()
{
    LogInfo << "Begin to get TransformerPostProcess instance.";
    auto instance = MemoryHelper::MakeShared<MxBase::TransformerPostProcess>();
    if (instance == nullptr) {
        LogError << "Create TransformerPostProcess object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get TransformerPostProcess instance.";
    return instance;
}
}
}


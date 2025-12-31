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
 * Description: Crnn model post-processing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "TextGenerationPostProcessors/CrnnPostProcess.h"
#include "MxBase/Log/Log.h"
#include "CrnnPostProcessDptr.hpp"

namespace MxBase {
CrnnPostProcess::CrnnPostProcess()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::CrnnPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create CrnnPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

CrnnPostProcess::CrnnPostProcess(const CrnnPostProcess &other) : MxBase::TextGenerationPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::CrnnPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create CrnnPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    *dPtr_ = *(other.dPtr_);
}

CrnnPostProcess &CrnnPostProcess::operator=(const CrnnPostProcess &other)
{
    if (this == &other) {
        return *this;
    }
    TextGenerationPostProcessBase::operator=(other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::CrnnPostProcessDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create CrnnPostProcessDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    dPtr_->objectNum_ = other.dPtr_->objectNum_;
    dPtr_->blankIdx_ = other.dPtr_->blankIdx_;
    dPtr_->withArgmax_ = other.dPtr_->withArgmax_;
    return *this;
}

APP_ERROR CrnnPostProcess::CheckDptr()
{
    LogDebug << "CrnnPostProcess CheckDptr: checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR CrnnPostProcess::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogDebug << "Start to Init CrnnPostProcess.";
    APP_ERROR ret = TextGenerationPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in TextGenerationPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = configData_.GetFileValue<uint32_t>("OBJECT_NUM", dPtr_->objectNum_, (uint32_t)0x0, (uint32_t)0x3e8);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read OBJECT_NUM from config, default value(0) will be used as objectNum_.";
    }
    ret = configData_.GetFileValue<uint32_t>("BLANK_INDEX", dPtr_->blankIdx_, (uint32_t)0x0, (uint32_t)0x2710);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read BLANK_INDEX from config, default value(0) will be used as blankIdx_.";
    }
    ret = configData_.GetFileValue<bool>("WITH_ARGMAX", dPtr_->withArgmax_);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read WITH_ARGMAX from config, default value(false) will be used "
                                    "as withArgmax_.";
    }
    LogDebug << "End to Init CrnnPostProcess.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

APP_ERROR CrnnPostProcess::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR CrnnPostProcess::Process(const std::vector<TensorBase>& tensors, std::vector<TextsInfo>& textInfos,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Start to Process CrnnPostProcess.";
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
    (dPtr_->TextGenerationOutput)(tensors, textInfos);
    LogDebug << "End to Process CrnnPostProcess.";
    return APP_ERR_OK;
}

uint64_t CrnnPostProcess::GetCurrentVersion()
{
    return MINDX_SDK_VERSION;
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::CrnnPostProcess> GetTextGenerationInstance()
{
    LogInfo << "Begin to get CrnnPostProcess instance.";
    auto instance = MemoryHelper::MakeShared<MxBase::CrnnPostProcess>();
    if (instance == nullptr) {
        LogError << "Create CrnnPostProcess object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get CrnnPostProcess instance.";
    return instance;
}
}
#endif
}


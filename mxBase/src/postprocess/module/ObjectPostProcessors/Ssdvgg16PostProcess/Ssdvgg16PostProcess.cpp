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
 * Description: Ssdvgg16 model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "ObjectPostProcessors/Ssdvgg16PostProcess.h"
#include "Ssdvgg16PostProcessDptr.hpp"
#include "MxBase/Log/Log.h"

namespace MxBase {
Ssdvgg16PostProcess::Ssdvgg16PostProcess()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::Ssdvgg16PostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create Ssdvgg16PostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

Ssdvgg16PostProcess::Ssdvgg16PostProcess(const Ssdvgg16PostProcess &other)
    : MxBase::ObjectPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::Ssdvgg16PostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create Ssdvgg16PostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    *dPtr_ = *(other.dPtr_);
}

Ssdvgg16PostProcess& Ssdvgg16PostProcess::operator=(const Ssdvgg16PostProcess &other)
{
    if (this == &other) {
        return *this;
    }
    ObjectPostProcessBase::operator=(other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::Ssdvgg16PostProcessDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create Ssdvgg16PostProcessDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    *dPtr_ = *(other.dPtr_);
    return *this;
}
uint64_t Ssdvgg16PostProcess::GetCurrentVersion()
{
    return MINDX_SDK_VERSION;
}

APP_ERROR Ssdvgg16PostProcess::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogDebug << "Start to Init Ssdvgg16PostProcessor.";
    APP_ERROR ret = ObjectPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in ObjectPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "End to Init Ssdvgg16PostProcessor.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

APP_ERROR Ssdvgg16PostProcess::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR Ssdvgg16PostProcess::CheckDptr()
{
    LogDebug << "Processing CheckDptr(). Checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR Ssdvgg16PostProcess::Process(const std::vector<TensorBase>& tensors,
    std::vector<std::vector<ObjectInfo>>& objectInfos, const std::vector<ResizedImageInfo>& resizedImageInfos,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Start to Process Ssdvgg16PostProcessor.";
    APP_ERROR ret = APP_ERR_OK;
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }

    auto inputs = tensors;
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
    ret = dPtr_->ObjectDetectionOutput(inputs, objectInfos);
    if (ret != APP_ERR_OK) {
        LogError << "ObjectDetectionOutput failed." << GetErrorInfo(ret);
        return ret;
    }
    for (uint32_t i = 0; i < resizedImageInfos.size(); i++) {
        CoordinatesReduction(i, resizedImageInfos[i], objectInfos[i]);
    }
    LogObjectInfos(objectInfos);
    LogDebug << "End to Process Ssdvgg16PostProcessor.";
    return APP_ERR_OK;
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::Ssdvgg16PostProcess> GetObjectInstance()
{
    LogInfo << "Begin to get SsdvggPostProcess instance.";
    auto instance = MemoryHelper::MakeShared<Ssdvgg16PostProcess>();
    if (instance == nullptr) {
        LogError << "Create Ssdvgg16PostProcess object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get SsdvggPostProcess instance.";
    return instance;
}
}
#endif
}


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
 * Create: 2021
 * History: NA
 */
#include "ObjectPostProcessors/RetinaNetPostProcess.h"
#include "RetinaNetPostProcessDptr.hpp"
#include "MxBase/Log/Log.h"
#include "MxBase/CV/ObjectDetection/Nms/Nms.h"
#include "acl/acl.h"

namespace MxBase {
RetinaNetPostProcess::RetinaNetPostProcess()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::RetinaNetPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create RetinaNetPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

RetinaNetPostProcess::RetinaNetPostProcess(const RetinaNetPostProcess &other) : MxBase::ObjectPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::RetinaNetPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create RetinaNetPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    *dPtr_ = *(other.dPtr_);
}

RetinaNetPostProcess &RetinaNetPostProcess::operator = (const RetinaNetPostProcess &other)
{
    if (this == &other) {
        return *this;
    }
    ObjectPostProcessBase::operator = (other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::RetinaNetPostProcessDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create RetinaNetPostProcessDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_OUT_OF_MEM);
            return *this;
        }
    }
    *dPtr_ = *(other.dPtr_);
    return *this;
}

APP_ERROR RetinaNetPostProcess::CheckDptr()
{
    LogDebug << "Processing CheckDptr(). Checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR RetinaNetPostProcess::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogDebug << "Start to Init RetinaNetPostProcessor.";
    APP_ERROR ret = ObjectPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in ObjectPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }

    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }

    ret = dPtr_->GetConfigValue();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to get config value." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "End to Init RetinaNetPostProcessor.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

APP_ERROR RetinaNetPostProcess::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR RetinaNetPostProcess::Process(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<ObjectInfo>> &objectInfos, const std::vector<ResizedImageInfo> &resizedImageInfos,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogInfo << "Start to Process RetinaNetPostProcess.";
    APP_ERROR ret = CheckDptr();
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
    switch (RetinaNetPostProcessDptr::ModelType(dPtr_->modelType_)) {
        case RetinaNetPostProcessDptr::TENSORFLOW:
            dPtr_->TensorflowOutput(inputs, objectInfos, resizedImageInfos);
            break;
        default:
            LogError << "The modelType_(" << dPtr_->modelType_ << ") is not supported."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
    }
    LogInfo << "End to Process RetinaNetPostProcess.";
    return APP_ERR_OK;
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::RetinaNetPostProcess> GetObjectInstance()
{
    LogInfo << "Begin to get RetinaNetPostProcess instance.";
    auto instance = MemoryHelper::MakeShared<RetinaNetPostProcess>();
    if (instance == nullptr) {
        LogError << "Create RetinaNetPostProcess object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get RetinaNetPostProcess instance.";
    return instance;
}
}
#endif
}
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
 * Description: SsdMobilenetv1Fpn model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "ObjectPostProcessors/SsdMobilenetv1FpnPostProcess.h"
#include "SsdMobilenetv1FpnPostProcessDptr.hpp"
#include "MxBase/Log/Log.h"

namespace MxBase {
SsdMobilenetv1FpnPostProcess::SsdMobilenetv1FpnPostProcess()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::SsdMobilenetv1FpnPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create SsdMobilenetv1FpnPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

SsdMobilenetv1FpnPostProcess::SsdMobilenetv1FpnPostProcess(const SsdMobilenetv1FpnPostProcess &other)
    : MxBase::ObjectPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::SsdMobilenetv1FpnPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create SsdMobilenetv1FpnPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    *dPtr_ = *(other.dPtr_);
}

SsdMobilenetv1FpnPostProcess& SsdMobilenetv1FpnPostProcess::operator=(const SsdMobilenetv1FpnPostProcess &other)
{
    if (this == &other) {
        return *this;
    }
    ObjectPostProcessBase::operator=(other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::SsdMobilenetv1FpnPostProcessDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create SsdMobilenetv1FpnPostProcessDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    *dPtr_ = *(other.dPtr_);
    return *this;
}

uint64_t SsdMobilenetv1FpnPostProcess::GetCurrentVersion()
{
    return MINDX_SDK_VERSION;
}

APP_ERROR SsdMobilenetv1FpnPostProcess::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogDebug << "Start to Init SsdMobilenetv1FpnPostProcessor.";
    APP_ERROR ret = ObjectPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in ObjectPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "End to Init SsdMobilenetv1FpnPostProcessor.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

APP_ERROR SsdMobilenetv1FpnPostProcess::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR SsdMobilenetv1FpnPostProcess::CheckDptr()
{
    LogDebug << "Processing CheckDptr(). Checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR SsdMobilenetv1FpnPostProcess::Process(const std::vector<TensorBase>& tensors,
    std::vector<std::vector<ObjectInfo>>& objectInfos, const std::vector<ResizedImageInfo>& resizedImageInfos,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Start to Process SsdMobilenetv1FpnPostProcessor.";
    APP_ERROR ret = APP_ERR_OK;
    if (tensors.empty() || tensors[0].GetShape().empty() || resizedImageInfos.size() != tensors[0].GetShape()[0]) {
        LogError << "check images error! Please check input of resizedImageInfos. resizedImageInfos.size("
                 << resizedImageInfos.size() << ") must be equal to batch size." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
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

    ret = dPtr_->ObjectDetectionOutput(inputs, objectInfos);
    if (ret != APP_ERR_OK) {
        LogError << "ObjectDetectionOutput failed." << GetErrorInfo(ret);
        return ret;
    }
    for (uint32_t i = 0; i < resizedImageInfos.size(); i++) {
        CoordinatesReduction(i, resizedImageInfos[i], objectInfos[i]);
    }

    LogObjectInfos(objectInfos);
    LogDebug << "End to Process SsdMobilenetv1FpnPostProcessor.";
    return APP_ERR_OK;
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::SsdMobilenetv1FpnPostProcess> GetObjectInstance()
{
    LogInfo << "Begin to get SsdMobilenetv1FpnPostProcess instance.";
    auto instance = MemoryHelper::MakeShared<SsdMobilenetv1FpnPostProcess>();
    if (instance == nullptr) {
        LogError << "Create SsdMobilenetv1FpnPostProcess object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get SsdMobilenetv1FpnPostProcess instance.";
    return instance;
}
}
#endif
}


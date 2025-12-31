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
 * Description: FasterRcnn model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "ObjectPostProcessors/FasterRcnnPostProcess.h"
#include "FasterRcnnPostProcessDptr.hpp"
#include "MxBase/Log/Log.h"
#include "MxBase/CV/ObjectDetection/Nms/Nms.h"

namespace MxBase {
FasterRcnnPostProcess::FasterRcnnPostProcess()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::FasterRcnnPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create FasterRcnnPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

FasterRcnnPostProcess::FasterRcnnPostProcess(const FasterRcnnPostProcess &other) : MxBase::ObjectPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::FasterRcnnPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create FasterRcnnPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    *dPtr_ = *(other.dPtr_);
}

uint64_t FasterRcnnPostProcess::GetCurrentVersion()
{
    return MINDX_SDK_VERSION;
}

FasterRcnnPostProcess& FasterRcnnPostProcess::operator=(const FasterRcnnPostProcess &other)
{
    if (this == &other) {
        return *this;
    }
    ObjectPostProcessBase::operator=(other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::FasterRcnnPostProcessDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create FasterRcnnPostProcessDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    *dPtr_ = *(other.dPtr_);
    return *this;
}

APP_ERROR FasterRcnnPostProcess::CheckDptr()
{
    LogDebug << "Processing CheckDptr(). Checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR FasterRcnnPostProcess::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogDebug << "Start to Init FasterRcnnPostProcess.";
    APP_ERROR ret = ObjectPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in ObjectPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    dPtr_->GetFasterRcnnConfig();
    LogDebug << "End to Init FasterRcnnPostProcess.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

APP_ERROR FasterRcnnPostProcess::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR FasterRcnnPostProcess::Process(const std::vector<TensorBase>& tensors,
    std::vector<std::vector<ObjectInfo>>& objectInfos, const std::vector<ResizedImageInfo>& resizedImageInfos,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Start to Process FasterRcnnPostProcess.";
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
    bool normalizedFlag = true;
    ret = dPtr_->GetTensorsObjectInfos(inputs, objectInfos, resizedImageInfos, normalizedFlag);
    if (ret != APP_ERR_OK) {
        LogError << "Execute FasterRcnn postprocess failed." << GetErrorInfo(ret);
        return ret;
    }
    if (dPtr_->framework_ != "mindspore") {
        if (objectInfos.size() < resizedImageInfos.size()) {
            LogError << "The objectInfos'size is less than resizedImageInfos'size."
                     << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
            return APP_ERR_COMM_OUT_OF_RANGE;
        }
        for (size_t i = 0; i < resizedImageInfos.size(); i++) {
            CoordinatesReduction(i, resizedImageInfos[i], objectInfos[i], normalizedFlag);
        }
    }
    LogObjectInfos(objectInfos);
    LogDebug << "End to Process FasterRcnnPostProcess.";
    return APP_ERR_OK;
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::FasterRcnnPostProcess> GetObjectInstance()
{
    LogInfo << "Begin to get FasterRcnnPostProcess instance.";
    auto instance = MemoryHelper::MakeShared<FasterRcnnPostProcess>();
    if (instance == nullptr) {
        LogError << "Create FasterRcnnPostProcess object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get FasterRcnnPostProcess instance.";
    return instance;
}
}
#endif
}


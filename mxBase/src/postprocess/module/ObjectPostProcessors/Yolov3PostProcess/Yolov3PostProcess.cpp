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
 * Description: Post-processing of Yolov3 model.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "ObjectPostProcessors/Yolov3PostProcess.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Maths/FastMath.h"
#include "MxBase/CV/ObjectDetection/Nms/Nms.h"
#include "Yolov3PostProcessDptr.hpp"

namespace MxBase {
Yolov3PostProcess::Yolov3PostProcess()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::Yolov3PostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create Yolov3PostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

Yolov3PostProcess::Yolov3PostProcess(const Yolov3PostProcess &other) : MxBase::ObjectPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::Yolov3PostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create Yolov3PostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    *dPtr_ = *(other.dPtr_);
}

Yolov3PostProcess& Yolov3PostProcess::operator=(const Yolov3PostProcess &other)
{
    if (this == &other) {
        return *this;
    }
    ObjectPostProcessBase::operator=(other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::Yolov3PostProcessDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create Yolov3PostProcessDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    *dPtr_ = *(other.dPtr_);
    return *this;
}

APP_ERROR Yolov3PostProcess::CheckDptr()
{
    LogDebug << "Processing CheckDptr(). Checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR Yolov3PostProcess::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogDebug << "Start to Init Yolov3PostProcess.";
    APP_ERROR ret = ObjectPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in ObjectPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }

    dPtr_->GetConfigValue();
    std::string str;
    ret = configData_.GetFileValue<std::string>("BIASES", str);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Failed to get biases";
    }
    ret = dPtr_->GetBiases(str);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get biases." << GetErrorInfo(ret);
        return ret;
    }
    ret = dPtr_->CheckFrameWork();
    if (ret != APP_ERR_OK) {
        LogError << "CheckFrameWork failed." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "End to Init Yolov3PostProcess.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

APP_ERROR Yolov3PostProcess::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR Yolov3PostProcess::Process(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<ObjectInfo>> &objectInfos, const std::vector<ResizedImageInfo> &resizedImageInfos,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Start to Process Yolov3PostProcess.";
    APP_ERROR ret = APP_ERR_OK;
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }

    if (resizedImageInfos.empty()) {
        LogError << "resizedImageInfos is not provided which is necessary for Yolov3PostProcess."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    auto inputs = tensors;
    ret = dPtr_->CheckAndMoveTensors(inputs);
    if (ret != APP_ERR_OK) {
        LogError << "CheckAndMoveTensors failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = dPtr_->ObjectDetectionOutput(inputs, objectInfos, resizedImageInfos);
    if (ret != APP_ERR_OK) {
        LogError << "ObjectDetectionOutput failed." << GetErrorInfo(ret);
        return ret;
    }
    for (uint32_t i = 0; i < resizedImageInfos.size(); i++) {
        CoordinatesReduction(i, resizedImageInfos[i], objectInfos[i]);
    }
    LogObjectInfos(objectInfos);
    LogDebug << "End to Process Yolov3PostProcess.";
    return APP_ERR_OK;
}

uint64_t Yolov3PostProcess::GetCurrentVersion()
{
    return MINDX_SDK_VERSION;
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::Yolov3PostProcess> GetObjectInstance()
{
    LogInfo << "Begin to get Yolov3PostProcess instance.";
    auto instance = MemoryHelper::MakeShared<Yolov3PostProcess>();
    if (instance == nullptr) {
        LogError << "Create Yolov3PostProcess object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get Yolov3PostProcess instance.";
    return instance;
}
}
#endif
}


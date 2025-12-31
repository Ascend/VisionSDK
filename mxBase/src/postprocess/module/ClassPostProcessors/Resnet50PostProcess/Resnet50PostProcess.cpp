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
 * Description: Resnet50 model post-processing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "ClassPostProcessors/Resnet50PostProcess.h"
#include "Resnet50PostProcessDptr.hpp"
#include "MxBase/Log/Log.h"
#include "MxBase/Maths/FastMath.h"

namespace MxBase {
Resnet50PostProcess::Resnet50PostProcess()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::Resnet50PostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create Resnet50PostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

Resnet50PostProcess::Resnet50PostProcess(const Resnet50PostProcess &other) : MxBase::ClassPostProcessBase(other)
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::Resnet50PostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create Resnet50PostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    *dPtr_ = *(other.dPtr_);
}

Resnet50PostProcess& Resnet50PostProcess::operator=(const Resnet50PostProcess &other)
{
    if (this == &other) {
        return *this;
    }
    ClassPostProcessBase::operator=(other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::Resnet50PostProcessDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create Resnet50PostProcessDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    *dPtr_ = *(other.dPtr_);
    return *this;
}

uint64_t Resnet50PostProcess::GetCurrentVersion()
{
    return MINDX_SDK_VERSION;
}

APP_ERROR Resnet50PostProcess::CheckDptr()
{
    LogDebug << "Resnet50PostProcess CheckDptr: checking if dPtr_ is a nullptr.";

    if (dPtr_ == nullptr) {
        LogError << "dPtr_ is a nullptr. Please check the constructor method."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR Resnet50PostProcess::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogDebug << "Start to Init Resnet50PostProcess.";
    APP_ERROR ret = ClassPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in ClassPostProcessBase." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    configData_.GetFileValueWarn("SOFTMAX", dPtr_->softmax_);
    configData_.GetFileValueWarn("CLASS_NUM", dPtr_->classNum_, (uint32_t)0x0, (uint32_t)0x7d0);
    configData_.GetFileValueWarn("TOP_K", dPtr_->topK_, (uint32_t)0x0, (uint32_t)0x10);
    LogDebug << "End to Init Resnet50PostProcess.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

APP_ERROR Resnet50PostProcess::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR Resnet50PostProcess::Process(const std::vector<TensorBase>& tensors,
    std::vector<std::vector<ClassInfo>> &classInfos,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Start to Process Resnet50PostProcess.";
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
    auto softmaxTensor = inputs[0];
    uint32_t batchSize = softmaxTensor.GetShape()[0];
    if (batchSize > MxBase::MAX_BATCH_SIZE) {
        LogError << "Batchsize is too large." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    void *softmaxTensorPtr = softmaxTensor.GetBuffer();
    if (softmaxTensorPtr == nullptr) {
        return APP_ERR_COMM_INVALID_POINTER;
    }
    uint32_t topk = std::min(dPtr_->topK_, dPtr_->classNum_);
    for (uint32_t i = 0; i < batchSize; i++) {
        std::vector<uint32_t> idx = {};
        std::vector<float> softmax = {};
        for (uint32_t j = 0; j < dPtr_->classNum_; j++) {
            idx.push_back(j);
            float value = *((float*)softmaxTensorPtr + i * dPtr_->classNum_ + j);
            softmax.push_back(value);
        }
        if (dPtr_->softmax_) {
            fastmath::softmax(softmax);
        }
        auto cmp = [&softmax] (uint32_t index_1, uint32_t index_2) {
            return softmax[index_1] > softmax[index_2];
        };
        std::sort(idx.begin(), idx.end(), cmp);

        std::vector<ClassInfo> topkClassInfos = {};
        for (uint32_t j = 0; j < topk; j++) {
            ClassInfo clsInfo = {};
            clsInfo.classId = (int)idx[j];
            clsInfo.confidence = softmax[idx[j]];
            clsInfo.className = configData_.GetClassName(idx[j]);
            topkClassInfos.push_back(clsInfo);
        }
        classInfos.push_back(topkClassInfos);
    }
    LogDebug << "End to Process Resnet50PostProcess.";
    return APP_ERR_OK;
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::Resnet50PostProcess> GetClassInstance()
{
    LogInfo << "Begin to get Resnet50PostProcess instance.";
    auto instance = MemoryHelper::MakeShared<MxBase::Resnet50PostProcess>();
    if (instance == nullptr) {
        LogError << "Create Resnet50PostProcess object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get Resnet50PostProcess instance.";
    return instance;
}
}
#endif
}


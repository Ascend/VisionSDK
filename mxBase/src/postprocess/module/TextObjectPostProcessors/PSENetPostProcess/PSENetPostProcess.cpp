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
 * Description: PSENet model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "TextObjectPostProcessors/PSENetPostProcess.h"
#include <unordered_map>
#include "MxBase/Log/Log.h"
#include "MxBase/Maths/FastMath.h"
#include "MxBase/Maths/MathFunction.h"
#include "PSENetPostProcessDptr.hpp"

namespace MxBase {
PSENetPostProcess &PSENetPostProcess::operator = (const PSENetPostProcess &other)
{
    if (this == &other) {
        return *this;
    }
    TextObjectPostProcessBase::operator = (other);
    if (dPtr_ == nullptr) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::PSENetPostProcessDptr>(this);
        if (dPtr_ == nullptr) {
            LogError << "Create PSENetPostProcessDptr object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return *this;
        }
    }
    dPtr_->min_kernel_area_ = other.dPtr_->min_kernel_area_;
    dPtr_->pse_scale_ = other.dPtr_->pse_scale_;
    dPtr_->min_score_ = other.dPtr_->min_score_;
    dPtr_->min_area_ = other.dPtr_->min_area_;
    dPtr_->kernel_num_ = other.dPtr_->kernel_num_;
    dPtr_->objectNumTensor_ = other.dPtr_->objectNumTensor_;
    return *this;
}
PSENetPostProcess::PSENetPostProcess()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::PSENetPostProcessDptr>(this);
    if (dPtr_ == nullptr) {
        LogError << "Create PSENetPostProcessDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    dPtr_->min_kernel_area_ = MIN_KERNEL_AREA;
    dPtr_->pse_scale_ = PSE_SCALE;
    dPtr_->min_score_ = MIN_SCORE;
    dPtr_->min_area_ = MIN_AREA;
    dPtr_->kernel_num_ = KERNEL_NUM;
}

PSENetPostProcess::~PSENetPostProcess() {}

/*
 * @description Load the configs and labels from the file.
 * @param labelPath config path and label path.
 * @return APP_ERROR error code.
 */

APP_ERROR PSENetPostProcess::Init(const std::map<std::string, std::string> &postConfig)
{
    if (isInitConfig_) {
        return APP_ERR_OK;
    }
    LogInfo << "Begin to initialize PSENetPostProcess.";
    // Open config file
    APP_ERROR ret = TextObjectPostProcessBase::Init(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in PSENetPostProcess." << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // read param
    ret = configData_.GetFileValue<float>("MIN_KERNEL_AREA", dPtr_->min_kernel_area_);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read " << MIN_KERNEL_AREA << " from config, default is: " <<
            dPtr_->min_kernel_area_;
    }
    ret = configData_.GetFileValue<float>("PSE_SCALE", dPtr_->pse_scale_);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read " << PSE_SCALE << " from config, default is: " << dPtr_->pse_scale_;
    }
    ret = configData_.GetFileValue<float>("MIN_SCORE", dPtr_->min_score_);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read " << MIN_SCORE << " from config, default is: " << dPtr_->min_score_;
    }
    ret = configData_.GetFileValue<float>("MIN_AREA", dPtr_->min_area_);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read " << MIN_AREA << " from config, default is: " << dPtr_->min_area_;
    }
    ret = configData_.GetFileValue<int>("KERNEL_NUM", dPtr_->kernel_num_);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read " << KERNEL_NUM << " from config, default is: " << dPtr_->kernel_num_;
    }
    LogInfo << "End to initialize PSENetPostProcess.";
    isInitConfig_ = true;
    return APP_ERR_OK;
}

/*
 * @description: Do nothing temporarily.
 * @return: APP_ERROR error code.
 */
APP_ERROR PSENetPostProcess::DeInit()
{
    LogInfo << "Begin to deinitialize PSENetPostProcess.";
    LogInfo << "End to deialize PSENetPostProcess.";
    return APP_ERR_OK;
}

APP_ERROR PSENetPostProcess::Process(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<TextObjectInfo>> &textObjInfos, const std::vector<ResizedImageInfo> &resizedImageInfos,
    const std::map<std::string, std::shared_ptr<void>> &)
{
    LogDebug << "Begin to process PSENetPostProcess.";
    auto inputs = tensors;
    APP_ERROR ret = CheckDptr();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = dPtr_->CheckAndMoveTensors(inputs);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in PSENetPostProcess." << GetErrorInfo(ret);
        return ret;
    }
    auto batchSize = static_cast<size_t>(tensors[0].GetShape()[0]);
    if (resizedImageInfos.size() != batchSize) {
        LogError << "Check input vector size failed, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ret = dPtr_->ObjectDetectionOutput(tensors, textObjInfos, resizedImageInfos);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to execute ObjectDetectionOutput in PSENetPostProcess." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "End to process PSENetPostProcess.";
    return APP_ERR_OK;
}

APP_ERROR PSENetPostProcess::CheckDptr()
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
std::shared_ptr<MxBase::PSENetPostProcess> GetTextObjectInstance()
{
    LogInfo << "Begin to get PSENetPostProcess instance.";
    auto instance = MemoryHelper::MakeShared<MxBase::PSENetPostProcess>();
    if (instance == nullptr) {
        LogError << "Create PSENetPostProcess object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    }
    LogInfo << "End to get PSENetPostProcess instance.";
    return instance;
}
}
}
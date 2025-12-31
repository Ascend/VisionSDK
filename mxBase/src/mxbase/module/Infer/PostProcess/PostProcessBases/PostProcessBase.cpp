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
 * Description: Used to initialize the model post-processing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "MxBase/PostProcessBases/PostProcessBase.h"
#include "MxBase/Log/Log.h"

namespace MxBase {
PostProcessBase& PostProcessBase::operator=(const PostProcessBase &other)
{
    if (this == &other) {
        return *this;
    }
    configData_ = other.configData_;
    checkModelFlag_ = other.checkModelFlag_;
    return *this;
}

APP_ERROR PostProcessBase::Init(const std::map<std::string, std::string> &postConfig)
{
    APP_ERROR ret = LoadConfigData(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "LoadConfigData failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR PostProcessBase::DeInit()
{
    return APP_ERR_OK;
}

uint64_t PostProcessBase::GetCurrentVersion()
{
    return MINDX_SDK_VERSION;
}

APP_ERROR PostProcessBase::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    APP_ERROR ret = APP_ERR_OK;
    for (auto &input : tensors) {
        ret = input.ToHost();
        if (ret != APP_ERR_OK) {
            LogError << "Tensor deploy to host failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR PostProcessBase::LoadConfigData(const std::map<std::string, std::string> &postConfig)
{
    LogDebug << "Start to LoadConfigData in PostProcessBase.";
    bool flag = false;
    std::string data = "";
    ConfigMode mode = ConfigMode::CONFIGCONTENT;
    if (postConfig.find("postProcessConfigPath") != postConfig.end() &&
        !(postConfig.find("postProcessConfigPath")->second).empty()) {
        data = postConfig.find("postProcessConfigPath")->second;
        mode = ConfigMode::CONFIGFILE;
        flag = true;
    }
    // Prefer "JSON_CONTENT" than "JSON_PATH"
    if (postConfig.find("postProcessConfigContent") != postConfig.end() &&
        !(postConfig.find("postProcessConfigContent")->second).empty()) {
        data = postConfig.find("postProcessConfigContent")->second;
        mode = ConfigMode::CONFIGCONTENT;
        flag = true;
    }

    APP_ERROR ret = APP_ERR_OK;
    if (flag) {
        ConfigUtil util = {};
        ret = util.LoadConfiguration(data, configData_, mode);
        if (ret != APP_ERR_OK) {
            LogError << "LoadConfiguration failed. Please check either \"postProcessConfigContent\" "
                                         "or \"postProcessConfigPath\" is set correctly." << GetErrorInfo(ret);
            return ret;
        }
    } else {
        LogWarn << "Get key \"postProcessConfigPath\" and \"postProcessConfigContent\" failed."
                   " No postprocess config will be read.";
    }

    // load labels
    if (postConfig.find("labelPath") != postConfig.end() && !(postConfig.find("labelPath")->second).empty()) {
        data = postConfig.find("labelPath")->second;
        ret = configData_.LoadLabels(data);
        if (ret != APP_ERR_OK) {
            LogError << "LoadLabels failed. Please check either \"labelPath\" is set correctly." << GetErrorInfo(ret);
            return ret;
        }
    } else {
        LogWarn << "Get key \"labelPath\" failed. No postprocess label will be read.";
    }
    return APP_ERR_OK;
}

void* PostProcessBase::GetBuffer(const TensorBase& tensor, uint32_t index) const
{
    if (tensor.GetBuffer() == nullptr) {
        LogError << "Fail to GetBuffer from tensor. make sure the input tensor is correct."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return nullptr;
    }
    if (tensor.GetShape().empty() || tensor.GetShape()[0] <= index) {
        LogError << "Fail to GetBuffer[" << index << "] from tensor." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return nullptr;
    }
    return (uint8_t *)tensor.GetBuffer() + index * tensor.GetByteSize() / tensor.GetShape()[0];
}

bool PostProcessBase::JudgeResizeType(const ResizedImageInfo& resizedImageInfo)
{
    return (resizedImageInfo.resizeType == RESIZER_TF_KEEP_ASPECT_RATIO ||
            resizedImageInfo.resizeType == RESIZER_MS_KEEP_ASPECT_RATIO ||
            resizedImageInfo.resizeType == RESIZER_RESCALE ||
            resizedImageInfo.resizeType == RESIZER_RESCALE_DOUBLE);
}
}
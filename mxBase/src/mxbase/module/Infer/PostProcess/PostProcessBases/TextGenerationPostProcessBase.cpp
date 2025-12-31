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
 * Description: Accepts the model inference output tensor and outputs the text generation result.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/PostProcessBases/TextGenerationPostProcessBase.h"
#include "MxBase/Log/Log.h"

namespace MxBase {
TextGenerationPostProcessBase& TextGenerationPostProcessBase::operator=(const TextGenerationPostProcessBase &other)
{
    if (this == &other) {
        return *this;
    }
    PostProcessBase::operator=(other);
    return *this;
}

APP_ERROR TextGenerationPostProcessBase::Init(const std::map<std::string, std::string> &postConfig)
{
    LogDebug << "Start to init TranslationPostProcessBase.";
    APP_ERROR ret = LoadConfigData(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "LoadConfigData failed." << GetErrorInfo(ret);
        return ret;
    }
    configData_.GetFileValueWarn<uint32_t>("CLASS_NUM", classNum_, (uint32_t)0x0, (uint32_t)0x2710);
    LogDebug << "End to init TranslationPostProcessBase.";
    return APP_ERR_OK;
}

APP_ERROR TextGenerationPostProcessBase::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR TextGenerationPostProcessBase::Process(const std::vector<TensorBase>&,
                                                 std::vector<TextsInfo>&,
                                                 const std::map<std::string, std::shared_ptr<void>> &)
{
    LogError << "Process() has not been overridden in subclass!" << GetErrorInfo(APP_ERR_COMM_UNREALIZED);
    return APP_ERR_COMM_UNREALIZED;
}
}
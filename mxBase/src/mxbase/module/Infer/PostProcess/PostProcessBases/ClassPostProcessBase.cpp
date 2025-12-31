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
 * Description: Classification detection post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/PostProcessBases/ClassPostProcessBase.h"
#include "MxBase/ConfigUtil/ConfigUtil.h"
#include "MxBase/Log/Log.h"

namespace MxBase {
ClassPostProcessBase& ClassPostProcessBase::operator=(const ClassPostProcessBase &other)
{
    if (this == &other) {
        return *this;
    }
    PostProcessBase::operator=(other);
    classNum_ = other.classNum_;
    topK_ = other.topK_;
    return *this;
}

APP_ERROR ClassPostProcessBase::Init(const std::map<std::string, std::string> &postConfig)
{
    APP_ERROR ret = LoadConfigData(postConfig);
    if (ret != APP_ERR_OK) {
        LogError << "LoadConfigData failed." << GetErrorInfo(ret);
        return ret;
    }
    configData_.GetFileValueWarn("CLASS_NUM", classNum_, (uint32_t)0x0, (uint32_t)0x3e8);
    return APP_ERR_OK;
}

APP_ERROR ClassPostProcessBase::DeInit()
{
    return APP_ERR_OK;
}

APP_ERROR ClassPostProcessBase::Process(const std::vector<TensorBase>&,
                                        std::vector<std::vector<ClassInfo>> &,
                                        const std::map<std::string, std::shared_ptr<void>> &)
{
    LogError << "Process() has not been overridden in subclass!" << GetErrorInfo(APP_ERR_COMM_UNREALIZED);
    return APP_ERR_COMM_UNREALIZED;
}
}
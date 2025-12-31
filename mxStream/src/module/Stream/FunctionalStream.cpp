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
 * Description: Determines and parses the plug-in attributes set in the pipeline.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "MxStream/Stream/FunctionalStream.h"
#include "StreamDptr.h"
#include "MxBase/Log/Log.h"
#include "MxStream/StreamManager/MxsmDescription.h"

namespace MxStream {
FunctionalStream::FunctionalStream(const std::string& name, const std::vector<PluginNode>& inputs,
    const std::vector<PluginNode>& outputs): Stream(name), inputs_(inputs), outputs_(outputs)
{
}

FunctionalStream::FunctionalStream(const std::string& name): Stream(name)
{
}

FunctionalStream::~FunctionalStream()
{
}

APP_ERROR FunctionalStream::Build()
{
    APP_ERROR ret = mxsmDescription_->CreateDescription(inputs_, outputs_);
    if (ret != APP_ERR_OK) {
        LogError << "Create stream description failed." << GetErrorInfo(ret);
        return ret;
    }
    return dPtr_->BaseBuild();
}
}
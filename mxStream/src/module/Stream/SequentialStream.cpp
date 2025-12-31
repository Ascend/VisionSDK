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

#include "MxStream/Stream/SequentialStream.h"
#include "MxStream/StreamManager/MxsmDescription.h"
#include "StreamDptr.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxStream {
SequentialStream::SequentialStream(const std::string &name): Stream(name)
{
}

SequentialStream::~SequentialStream()
{
    LogDebug << "SequentialStream destroy.";
}

APP_ERROR SequentialStream::Add(const PluginNode &pluginNode)
{
    pluginNodeVec_.push_back(pluginNode);
    return APP_ERR_OK;
}

APP_ERROR SequentialStream::Build()
{
    APP_ERROR ret = mxsmDescription_->CreateDescription(pluginNodeVec_);
    if (ret != APP_ERR_OK) {
        LogError << "Create stream description failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return ret;
    }
    return dPtr_->BaseBuild();
}
}
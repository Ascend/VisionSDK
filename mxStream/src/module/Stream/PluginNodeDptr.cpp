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
 * Description: PluginNode private interface for internal use only.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxStream/Stream/PluginNode.h"
#include "PluginNodeDptr.h"

namespace MxStream {
PluginNodeDptr::PluginNodeDptr(): id_(0)
{
    pluginObject_ = nlohmann::json::object();
}

void PluginNodeDptr::SetNextNode(const std::string& nextNodeName)
{
    if (pluginObject_.find("next") == pluginObject_.end() || pluginObject_["next"].is_null()) {
        pluginObject_["next"] = nextNodeName;
        return;
    }
    auto next = nextNodeName;
    if (pluginObject_["next"].is_string()) {
        auto next0 = pluginObject_["next"];
        if (next0 == next) {
            return;
        }
        pluginObject_["next"] = nlohmann::json::array({next0, next});
    } else if (pluginObject_["next"].is_array()) {
        auto nextArray = pluginObject_["next"];
        if (std::find(nextArray.begin(), nextArray.end(), next) != nextArray.end()) {
            return;
        }
        pluginObject_["next"][pluginObject_["next"].size()] = nextNodeName;
    } else {
        LogError << "The \"next\" property of PluginNode is invalid"
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
}
}
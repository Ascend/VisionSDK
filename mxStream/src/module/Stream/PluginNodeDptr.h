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

#ifndef MXSM_PLUGIN_NODE_DPTR_H
#define MXSM_PLUGIN_NODE_DPTR_H

#include "MxBase/Common/HiddenAttr.h"

namespace MxStream {
class SDK_UNAVAILABLE_FOR_OTHER PluginNodeDptr {
public:
    PluginNodeDptr();
    ~PluginNodeDptr() = default;

    void SetNextNode(const std::string& nextPluginName);

public:
    int id_ = 0;
    std::string pluginName_;
    std::string factory_;
    nlohmann::json pluginObject_;
    std::map<std::string, std::string> props_ = {};
    std::vector<int> preNodeVec_ = {};
    std::vector<PluginNode> nextNodeVec_ = {};

private:
    PluginNodeDptr(const PluginNodeDptr &) = default;
    PluginNodeDptr(const PluginNodeDptr &&) = delete;
    PluginNodeDptr& operator=(const PluginNodeDptr &) = delete;
    PluginNodeDptr& operator=(const PluginNodeDptr &&) = delete;
};
}
#endif // MXSM_PLUGIN_NODE_DPTR_H
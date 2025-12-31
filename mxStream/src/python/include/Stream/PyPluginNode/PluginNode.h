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
 * Description: convert c++ PluginNode to python class.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef PLUGIN_NODE_H
#define PLUGIN_NODE_H

#include <string>
#include <vector>
#include <memory>
#include "MxStream/Stream/PluginNode.h"
namespace PyStream {
class PluginNode {
public:
    PluginNode();
    PluginNode(const std::string &factory,
        const std::map<std::string, std::string> &props = std::map<std::string, std::string>(),
        const std::string &name = "");
    ~PluginNode() = default;
    PluginNode operator()(const std::vector<PluginNode> &args);
    PluginNode(const PluginNode& other);
    std::string plugin_name() const;
    std::string factory() const;
    std::string to_json() const;
    std::shared_ptr<MxStream::PluginNode> GetNode() const;

private:
    std::shared_ptr<MxStream::PluginNode> pluginNode_ = nullptr;
};
}  // namespace PyStream
#endif
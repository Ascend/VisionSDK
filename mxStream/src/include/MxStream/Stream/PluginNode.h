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
 * Description: PluginNode structure.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MXSTREAM_PLUGIN_NODE_H
#define MXSTREAM_PLUGIN_NODE_H

#include <map>
#include <nlohmann/json.hpp>

namespace MxStream {
class PluginNodeDptr;

class PluginNode {
public:
    PluginNode(const std::string& factory,
        const std::map<std::string, std::string>& props = std::map<std::string, std::string>(),
        const std::string& name = "");
    PluginNode(const PluginNode &);
    PluginNode(const PluginNode &&);
    ~PluginNode();

    PluginNode& operator()(PluginNode& preNode);

    template<typename... Args>
    PluginNode& operator()(PluginNode& preNode, Args& ... args)
    {
        operator()(preNode);
        operator()(args...);
        return *this;
    }

    PluginNode& operator()(std::vector<PluginNode>& preNodeList);

    int PluginId() const;
    std::string PluginName() const;
    std::string Factory() const;
    std::map<std::string, std::string> Properties() const;
    std::string ToJson() const;

private:
    void SetPluginName(const std::string& name);
    void SetNextNode(const std::string& name);
    void SetNextNodes();
    std::vector<PluginNode>& NextNodes() const;

private:
    std::shared_ptr<PluginNodeDptr> dPtr_ = nullptr;
    static int pluginCount_;

private:
    PluginNode() = delete;
    PluginNode& operator=(const PluginNode &) = delete;
    PluginNode& operator=(const PluginNode &&) = delete;
    friend class PluginNodeDptr;
    friend class Stream;
    friend class StreamDptr;
    friend class SequentialStream;
    friend class FunctionalStream;
    friend class MxsmDescription;
};
}
#endif // MXSTREAM_PLUGIN_NODE_H
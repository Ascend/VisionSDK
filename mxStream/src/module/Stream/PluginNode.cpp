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

#include "MxStream/Stream/PluginNode.h"
#include "PluginNodeDptr.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxStream {
static inline bool IsPreNodeExists(const std::vector<int>& preNodeVec, int findId)
{
    for (int pluginId : preNodeVec) {
        if (pluginId == findId) {
            return true;
        }
    }
    return false;
}

static inline bool IsNextNodeExists(const std::vector<PluginNode>& nextNodeVec, const PluginNode& findNode)
{
    for (auto& nextNode : nextNodeVec) {
        if (nextNode.PluginId() == findNode.PluginId()) {
            return true;
        }
    }
    return false;
}

PluginNode::PluginNode(const std::string& factory, const std::map<std::string, std::string>& props,
    const std::string& name)
{
    dPtr_ = std::make_shared<PluginNodeDptr>();
    dPtr_->id_ = ++pluginCount_;
    dPtr_->factory_ = factory;
    dPtr_->props_ = props;
    if (!name.empty()) {
        dPtr_->pluginName_ = name;
    }
    dPtr_->pluginObject_["factory"] = factory;
    if (!props.empty()) {
        dPtr_->pluginObject_["props"] = nlohmann::json::object();
        for (auto &iter : props) {
            dPtr_->pluginObject_["props"][iter.first] = iter.second;
        }
    }
}

PluginNode::PluginNode(const PluginNode& other)
{
    dPtr_ = other.dPtr_;
}

PluginNode::PluginNode(const PluginNode &&other)
{
    dPtr_ = other.dPtr_;
}

PluginNode::~PluginNode()
{
}

PluginNode& PluginNode::operator()(PluginNode& preNode)
{
    if (!IsNextNodeExists(preNode.dPtr_->nextNodeVec_, *this)) {
        preNode.dPtr_->nextNodeVec_.emplace_back(*this);
    }
    if (!IsPreNodeExists(dPtr_->preNodeVec_, preNode.dPtr_->id_)) {
        dPtr_->preNodeVec_.emplace_back(preNode.dPtr_->id_);
    }
    return *this;
}

PluginNode& PluginNode::operator()(std::vector<PluginNode>& preNodeList)
{
    for (auto preNode: preNodeList) {
        operator()(preNode);
    }
    return *this;
}

void PluginNode::SetPluginName(const std::string& name)
{
    dPtr_->pluginName_ = name;
}

void PluginNode::SetNextNode(const std::string& name)
{
    dPtr_->SetNextNode(name);
}

int PluginNode::PluginId() const
{
    return dPtr_->id_;
}

void PluginNode::SetNextNodes()
{
    for (auto& nextNode : dPtr_->nextNodeVec_) {
        std::string nextName;
        if (nextNode.dPtr_->preNodeVec_.size() <= 1) {
            nextName = nextNode.dPtr_->pluginName_;
            dPtr_->SetNextNode(nextName);
            continue;
        }
        int index = 0;
        for (int preNodeId : nextNode.dPtr_->preNodeVec_) {
            if (preNodeId == dPtr_->id_) {
                break;
            }
            ++index;
        }
        std::stringstream ss;
        ss << index;
        nextName = nextNode.dPtr_->pluginName_ + ":" + ss.str();
        dPtr_->SetNextNode(nextName);
    }
}

std::string PluginNode::PluginName() const
{
    return dPtr_->pluginName_;
}

std::string PluginNode::Factory() const
{
    return dPtr_->factory_;
}

std::map<std::string, std::string> PluginNode::Properties() const
{
    return dPtr_->props_;
}

std::string PluginNode::ToJson() const
{
    return dPtr_->pluginObject_.dump();
}

std::vector<PluginNode>& PluginNode::NextNodes() const
{
    return dPtr_->nextNodeVec_;
}

int PluginNode::pluginCount_;
}
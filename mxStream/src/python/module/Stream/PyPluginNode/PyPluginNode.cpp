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
#include <Python.h>
#include "Stream/PyPluginNode/PluginNode.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace PyStream {
PluginNode::PluginNode() : pluginNode_(nullptr)
{}
PluginNode::PluginNode(
    const std::string &factory, const std::map<std::string, std::string> &props, const std::string &name)
{
    pluginNode_ = MxBase::MemoryHelper::MakeShared<MxStream::PluginNode>(factory, props, name);
    if (pluginNode_ == nullptr) {
        LogError << "Create PluginNode object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}
PluginNode::PluginNode(const PluginNode &other)
{
    pluginNode_ = other.GetNode();
}
std::string PluginNode::plugin_name() const
{
    if (pluginNode_ == nullptr) {
        LogError << "PluginNode object ptr is null." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return "";
    }
    return pluginNode_->PluginName();
}

std::string PluginNode::factory() const
{
    if (pluginNode_ == nullptr) {
        LogError << "PluginNode object ptr is null." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return "";
    }
    return pluginNode_->Factory();
}

std::string PluginNode::to_json() const
{
    if (pluginNode_ == nullptr) {
        LogError << "PluginNode object ptr is null." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return "";
    }
    return pluginNode_->ToJson();
}

PluginNode PluginNode::operator()(const std::vector<PluginNode> &args)
{
    PyThreadState *pyState = PyEval_SaveThread();
    for (auto &arg : args) {
        auto nodePtr = arg.GetNode();
        if (nodePtr == nullptr) {
            LogError << "PluginNode object ptr is null." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_POINTER));
        }
        pluginNode_->operator()(*nodePtr);
    }
    PyEval_RestoreThread(pyState);
    return *this;
}

std::shared_ptr<MxStream::PluginNode> PluginNode::GetNode() const
{
    return pluginNode_;
}
}  // namespace PyStream

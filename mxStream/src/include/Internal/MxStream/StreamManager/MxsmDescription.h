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
 * Description: A description of a stream.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef MXSM_DESCRIPTION_H
#define MXSM_DESCRIPTION_H

#include <vector>
#include <list>
#include "MxStream/Stream/PluginNode.h"
#include "MxBase/Log/Log.h"

namespace MxStream {
class MxsmDescription {
public:
    MxsmDescription() = default;
    /*
    * @description: suitable for SequentialStream
    */
    APP_ERROR CreateDescription(std::vector<PluginNode>& pluginNodeVec);
    /*
    * @description: suitable for FunctionStream
    */
    APP_ERROR CreateDescription(std::vector<PluginNode>& inputs, std::vector<PluginNode>& outputs);
    /*
    * @description: create MxsmDescription using a pipeline file
    */
    explicit MxsmDescription(const std::string name);
    /*
    * @description: create MxsmDescription using a pipeline file with mulit stream
    */
    MxsmDescription(const std::string pipelinePath, const std::string streamnName);
    /*
    * @description: create MxsmDescription using existing streamName and streamObject
    */
    MxsmDescription(const std::string& streamName, const nlohmann::json& streamObject);
    /*
    * @description: return multiable MxsmDescription if pipeline file describe multible stream
    */
    static std::vector<MxsmDescription> GetStreamDescFromPipeline(const std::string pipelinePath);

    ~MxsmDescription();

    static APP_ERROR ValidateStreamObject(const nlohmann::json& streamObject);

public:
    std::string GetStreamName();
    nlohmann::json GetStreamJson();

private:
    std::string streamName_;
    nlohmann::json streamObject_;
    std::map<std::string, unsigned int> pluginCountMap_ = {};
    bool isDescCreated_ = false;
    size_t setPluginNameTimes_ = 0;
    size_t setNextNodeTimes_ = 0;
    size_t traverseNodeTimes_ = 0;

private:
    APP_ERROR SetPluginName(PluginNode& pluginNode);
    APP_ERROR CreatePipeline(std::vector<PluginNode>& pluginNodeVec);
    APP_ERROR CreatePipeline(std::vector<PluginNode>& inputs, std::vector<PluginNode>& outputs);
    APP_ERROR SetPluginName(std::vector<PluginNode>& inputs, std::vector<PluginNode>& outputs);
    APP_ERROR SetNextNodes(std::vector<PluginNode>& inputs, std::vector<PluginNode>& outputs);
    APP_ERROR SetPluginJsonObjects(std::vector<PluginNode>& inputs, std::vector<PluginNode>& outputs);
    APP_ERROR TraversePluginNode(const PluginNode& curNode, std::list<int>& existNodeList);
    APP_ERROR SetPluginNameWithRecursion(PluginNode& curNode, std::list<int>& existNodeList);
    APP_ERROR SetNextNodeWithRecursion(PluginNode& curNode, std::list<int>& existNodeList);

    static APP_ERROR ValidateElementLinks(const nlohmann::json& elementObject,
        const nlohmann::json &streamObject);
    static APP_ERROR ValidateOneDirectionLinks(const nlohmann::json& link, const nlohmann::json& streamObject);
    static std::string GetNextElementName(const std::string& elementName);

private:
    friend class Stream;
};
}
#endif // MXSM_DESCRIPTION_H
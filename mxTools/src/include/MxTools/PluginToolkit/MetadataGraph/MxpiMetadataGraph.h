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
 * Description: Manage the relationship between metadata, which is used for generating serialized output data.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPLUGINGENERATOR_MXPIMETADATAGRAPH_H
#define MXPLUGINGENERATOR_MXPIMETADATAGRAPH_H

#include <nlohmann/json.hpp>
#include <google/protobuf/message.h>

#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxTools {
// defines one member of the list(metadata value) as a DirectedNode
struct DirectedNode {
    std::string nodeName;
    std::string typeName;
    bool isValid = true;
    bool isRead = false;
    google::protobuf::Message const *nodeMessage;  // one member of the list(metadata value), not the list
    std::vector<DirectedNode*> nextNodes;
};

class MxpiMetadataGraph {
public:
    MxpiMetadataGraph();
    ~MxpiMetadataGraph();

    /* *
     * @description: add node list to the nodeLists
     * @param nodeListName: the node list
     * @param nodeListMessage: the protobuf message list to be added
     * @return: APP_ERROR
     */
    APP_ERROR AddNodeList(const std::string& nodeListName, std::shared_ptr<google::protobuf::Message> nodeListMessage);
    /* *
     * @description: mark nodes in a given list as valid
     * @param nodeListName: the node list
     * @return: APP_ERROR
     */
    APP_ERROR MarkNodeListAsValid(const std::string& nodeListName);
    /* *
     * @description: mark nodes in a given list as invalid
     * @param nodeListName: the node list
     * @return: APP_ERROR
     */
    APP_ERROR MarkNodeListAsInvalid(const std::string& nodeListName);
    /* *
     * @description: mark all nodes in the graph as invalid
     * @param
     * @return: APP_ERROR
     */
    APP_ERROR MarkAllNodesAsInvalid();
    /* *
     * @description: remove all message of a node list
     * @param nodeListName: the node list
     * @return: APP_ERROR
     */
    APP_ERROR RemoveNodeListMessage(const std::string& nodeListName);
    /* *
     * @description: generate json string base on the valid nodes in the graph
     * @param
     * @return: std::string
     */
    std::string GetJsonString();
    std::string GetJsonStringFromNodelist(const std::string& nodeListName);

    void SetEraseHeaderVecFlag(bool flag);

    MxpiMetadataGraph(const MxpiMetadataGraph &) = delete;
    MxpiMetadataGraph(const MxpiMetadataGraph &&) = delete;
    MxpiMetadataGraph& operator=(const MxpiMetadataGraph &) = delete;
    MxpiMetadataGraph& operator=(const MxpiMetadataGraph &&) = delete;

private:
    /* *
     * @description: add one node to the graph and root node list
     * @param nodeName: the node name used for keeping node message, it is "parentName"+"memberId"
     * @param nodeMessage: the protobuf message to be added
     * @return: APP_ERROR
     */
    APP_ERROR AddNode(const std::string& nodeName, google::protobuf::Message const *nodeMessage);
    /* *
     * @description: mark a single node as valid, then it will be included in the result
     * @param nodeName: the node name
     * @return: APP_ERROR
     */
    APP_ERROR MarkNodeAsValid(const std::string& nodeName);
    /* *
     * @description: mark a single node as invalid, then it will be excluded in the result
     * @param nodeName: the node name
     * @return: APP_ERROR
     */
    APP_ERROR MarkNodeAsInvalid(const std::string& nodeName);
    /* *
     * @description: remove the message, then it will be excluded in the result
     * @param nodeName: the node name
     * @return: APP_ERROR
     */
    APP_ERROR RemoveNodeMessage(const std::string& nodeName);

    void GetBase64StrFromVisionDataIfHave(DirectedNode& node);
    void GetJsonValueFromOneNode(DirectedNode& node, std::string& key, nlohmann::json& jsonValue);
    nlohmann::json GetJsonScanOneNode(DirectedNode& node, std::string& key);
    void GetJsonScanNextNodes(DirectedNode& node, nlohmann::json& output);
    void SqueezeGraph(DirectedNode& node, std::vector<DirectedNode*>& nextNodes);
    void DebugPrintGraphInfo(DirectedNode& rootNode);
    void LinkNodesOrSaveNewNode(const std::string& nodeName, const google::protobuf::Message* nodeMessage,
        const google::protobuf::FieldDescriptor* field, std::shared_ptr<DirectedNode> newNodeSptr,
        const google::protobuf::Reflection* refl);
    void AssignNodeValue(const std::string &nodeListName,
        const std::shared_ptr<mindxsdk_private::protobuf::Message> &nodeListMessage,
        const std::string &typeName, std::shared_ptr<DirectedNode> NodeSptr) const;

private:
    DirectedNode rootNode_;
    std::map<std::string, std::vector<std::string>> nodeLists_;
    std::map<std::string, std::shared_ptr<DirectedNode>> graphNodes_;
    std::vector<std::string> noticeInfo_;
    bool isEraseHeaderVecInfo_ = true;
};
}  // namespace MxTools
#endif  // MXPLUGINGENERATOR_MXPIMETADATAGRAPH_H
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

#include "MxTools/PluginToolkit/MetadataGraph/MxpiMetadataGraph.h"
#include <google/protobuf/util/json_util.h>
#include <cstring>
#include "MxBase/Log/Log.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Utils/StringUtils.h"

namespace MxTools {
const std::string FRAME_INFO_DESC = "MxpiFrameInfo";
const std::string CONTENT_STRING_DESC = "MxpiContentString";

MxpiMetadataGraph::MxpiMetadataGraph()
{
    rootNode_.nodeMessage = nullptr;
    rootNode_.isValid = false;
    rootNode_.isRead = false;
    rootNode_.nodeName = "RootNode";
}

MxpiMetadataGraph::~MxpiMetadataGraph()
{}

APP_ERROR MxpiMetadataGraph::AddNode(const std::string& nodeName, google::protobuf::Message const *nodeMessage)
{
    LogDebug << "Begin to add a directed node(" << nodeName << ").";
    if (nodeMessage == nullptr) {
        LogWarn << "Invalid node message of node(" << nodeName << ").";
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (graphNodes_.find(nodeName) != graphNodes_.end()) {
        LogWarn << "Directed node name(" << nodeName << ") already exists.";
        return APP_ERR_PLUGIN_TOOLKIT_NODE_ALREADY_EXIST;
    }
    const google::protobuf::Descriptor* desc = nodeMessage->GetDescriptor();
    if (desc == nullptr) {
        LogWarn << "Invalid node message of node(" << nodeName << "). Invalid protobuf message descriptor.";
        return APP_ERR_COMM_INVALID_POINTER;
    }
    const google::protobuf::FieldDescriptor* field = desc->field(0);
    if (field == nullptr) {
        LogWarn << "Invalid node message of node(" << nodeName << "). Invalid protobuf message field descriptor.";
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (field->name() != "headerVec") {
        LogWarn << "The first member of the node(" << nodeName << ") message is not headerVec.";
        return APP_ERR_PLUGIN_TOOLKIT_MESSAGE_NOT_MATCH;
    }

    // create directed node
    auto newNodeSptr = MxBase::MemoryHelper::MakeShared<DirectedNode>();
    if (newNodeSptr == nullptr) {
        LogError << "Create DirectedNode object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    newNodeSptr->nodeName = nodeName;
    newNodeSptr->nodeMessage = nodeMessage;
    newNodeSptr->typeName = desc->name();
    graphNodes_.emplace(std::pair<std::string, std::shared_ptr<DirectedNode>>(nodeName, newNodeSptr));

    // link nodes and save new node to the rootNode_ if applicable
    const google::protobuf::Reflection* refl = nodeMessage->GetReflection();
    if (refl == nullptr) {
        LogWarn << "Invalid node message of node(" << nodeName << "). Invalid protobuf message reflection.";
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (refl->FieldSize(*nodeMessage, field) == 0) {
        LogDebug << "This is a root node(" << nodeName << ").";
        rootNode_.nextNodes.push_back(newNodeSptr.get());
    }
    LinkNodesOrSaveNewNode(nodeName, nodeMessage, field, newNodeSptr, refl);
    LogDebug << "End to add a directed node(" << nodeName << ").";
    return APP_ERR_OK;
}

void MxpiMetadataGraph::LinkNodesOrSaveNewNode(const std::string& nodeName,
    const google::protobuf::Message* nodeMessage, const google::protobuf::FieldDescriptor* field,
    std::shared_ptr<DirectedNode> newNodeSptr, const google::protobuf::Reflection* refl)
{
    for (int i = 0; i < refl->FieldSize(*nodeMessage, field); i++) {
        const google::protobuf::Message& headerMessage = refl->GetRepeatedMessage(*nodeMessage, field, i);
        auto* metaHeader = (const MxpiMetaHeader*)&headerMessage;
        std::string dataSource = metaHeader->datasource().empty() ? metaHeader->parentname() : metaHeader->datasource();
        std::string parentNodeName = dataSource + "_" + std::to_string(metaHeader->memberid());
        if (graphNodes_.find(parentNodeName) == graphNodes_.end()) {
            if (nodeLists_.find(dataSource) != nodeLists_.end()) {
                LogWarn << "memberid(" << metaHeader->memberid() << ") of nodeName(" << nodeName << ") is invalid.";
                continue;
            }
            LogDebug << "This is a root node(" << nodeName << ").";
            rootNode_.nextNodes.push_back(newNodeSptr.get());
            continue;
        }
        std::shared_ptr<DirectedNode> parentNodeSptr = graphNodes_.at(parentNodeName);
        auto parentNode = (DirectedNode*)parentNodeSptr.get();
        parentNode->nextNodes.push_back(newNodeSptr.get());
        if (nodeName == parentNode->nodeName) {
            LogWarn << "node`s parent name can not be itself.";
        }
    }
}

APP_ERROR CheckNodeListMessagePointer(std::shared_ptr<google::protobuf::Message> nodeListMessage,
                                      const google::protobuf::Descriptor* desc,
                                      const google::protobuf::FieldDescriptor* field,
                                      const google::protobuf::Reflection* refl,
                                      const std::string& nodeListName)
{
    if (MxBase::StringUtils::HasInvalidChar(nodeListName)) {
        LogError << "NodeListName has invalid char." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (nodeListMessage == nullptr) {
        LogWarn << "Invalid node list("<< nodeListName << ") message.";
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (desc == nullptr) {
        LogWarn << "Invalid node list("<< nodeListName << ") message. Invalid protobuf message descriptor.";
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (field == nullptr) {
        LogWarn << "Invalid node list("<< nodeListName << ") message. Invalid protobuf message field descriptor.";
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (refl == nullptr) {
        LogWarn << "Invalid node list("<< nodeListName << ") message. Invalid protobuf message reflection.";
        return APP_ERR_COMM_INVALID_POINTER;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiMetadataGraph::AddNodeList(
    const std::string& nodeListName, std::shared_ptr<google::protobuf::Message> nodeListMessage)
{
    LogDebug << "Begin to add a node list(" << nodeListName << ").";
    // add new nodes to graph
    const google::protobuf::Descriptor* desc = nodeListMessage->GetDescriptor();
    const google::protobuf::FieldDescriptor* field = desc->field(0);
    const google::protobuf::Reflection* refl = nodeListMessage->GetReflection();
    APP_ERROR ret = CheckNodeListMessagePointer(nodeListMessage, desc, field, refl, nodeListName);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::vector<std::string> nodeList;
    std::string typeName = desc->name();
    if (typeName == FRAME_INFO_DESC) {
        auto tmpNodeSptr = MxBase::MemoryHelper::MakeShared<DirectedNode>();
        if (tmpNodeSptr == nullptr) {
            LogError << "Create DirectedNode object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        AssignNodeValue(nodeListName, nodeListMessage, typeName, tmpNodeSptr);
        graphNodes_.insert(std::pair<std::string, std::shared_ptr<DirectedNode>>(nodeListName, tmpNodeSptr));
        rootNode_.nextNodes.push_back(tmpNodeSptr.get());
        nodeList.push_back(nodeListName);
        nodeLists_.insert(std::pair<std::string, std::vector<std::string>>(nodeListName, nodeList));
    } else if (typeName != CONTENT_STRING_DESC) {
        for (int i = 0; i < refl->FieldSize(*nodeListMessage, field); i++) {
            std::string nodeName = nodeListName + "_" + std::to_string(i);
            const google::protobuf::Message& nodeMessage = refl->GetRepeatedMessage(*nodeListMessage, field, i);
            APP_ERROR ret = AddNode(nodeName, &nodeMessage);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to add the node(" << nodeName << ")." << GetErrorInfo(ret);
                return ret;
            }
            nodeList.push_back(nodeName);
        }
        // add nodes to nodeLists_
        nodeLists_.insert(std::pair<std::string, std::vector<std::string>>(nodeListName, nodeList));
        LogDebug << "End to add a node list(" << nodeListName << ").";
    }
    return APP_ERR_OK;
}

void MxpiMetadataGraph::AssignNodeValue(const std::string &nodeListName,
    const std::shared_ptr<google::protobuf::Message> &nodeListMessage,
    const std::string &typeName, std::shared_ptr<DirectedNode> NodeSptr) const
{
    NodeSptr->nodeName = nodeListName;
    NodeSptr->nodeMessage = nodeListMessage.get();
    NodeSptr->typeName = typeName;
}

APP_ERROR MxpiMetadataGraph::MarkNodeAsValid(const std::string& nodeName)
{
    LogDebug << "Begin to mark a node(" << nodeName << ") as valid.";
    if (graphNodes_.find(nodeName) == graphNodes_.end()) {
        LogWarn << "Directed node name(" << nodeName << ") does not exists.";
        return APP_ERR_PLUGIN_TOOLKIT_NODE_NOT_EXIST;
    }
    auto markNode = graphNodes_.at(nodeName);
    markNode->isValid = true;
    LogDebug << "End to mark a node(" << nodeName << ") as valid.";
    return APP_ERR_OK;
}

APP_ERROR MxpiMetadataGraph::MarkNodeAsInvalid(const std::string& nodeName)
{
    LogDebug << "Begin to mark a node(" << nodeName << ") as invalid.";
    if (graphNodes_.find(nodeName) == graphNodes_.end()) {
        LogWarn << "Directed node(" << nodeName << ") does not exists.";
        return APP_ERR_PLUGIN_TOOLKIT_NODE_NOT_EXIST;
    }
    auto markNode = graphNodes_.at(nodeName);
    markNode->isValid = false;
    LogDebug << "End to mark a node(" << nodeName << ") as invalid.";
    return APP_ERR_OK;
}

APP_ERROR MxpiMetadataGraph::MarkNodeListAsValid(const std::string& nodeListName)
{
    if (MxBase::StringUtils::HasInvalidChar(nodeListName)) {
        LogError << "NodeListName has invalid char." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Begin to mark node list(" << nodeListName <<") as valid.";
    if (nodeLists_.find(nodeListName) == nodeLists_.end()) {
        LogWarn << "Node list(" << nodeListName << ")  does not exist.";
        return APP_ERR_PLUGIN_TOOLKIT_NODELIST_NOT_EXIST;
    }

    for (auto nodeName : nodeLists_.at(nodeListName)) {
        APP_ERROR ret = MarkNodeAsValid(nodeName);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    LogDebug << "End to mark node list(" << nodeListName <<") as valid.";
    return APP_ERR_OK;
}

APP_ERROR MxpiMetadataGraph::MarkNodeListAsInvalid(const std::string& nodeListName)
{
    if (MxBase::StringUtils::HasInvalidChar(nodeListName)) {
        LogError << "NodeListName has invalid char." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Begin to mark node list(" << nodeListName << ") as invalid.";
    if (nodeLists_.find(nodeListName) == nodeLists_.end()) {
        LogWarn << "Node list(" << nodeListName << ") does not exist.";
        return APP_ERR_PLUGIN_TOOLKIT_NODELIST_NOT_EXIST;
    }

    for (auto nodeName : nodeLists_.at(nodeListName)) {
        APP_ERROR ret = MarkNodeAsInvalid(nodeName);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    LogDebug << "End to mark node list(" << nodeListName << ") as invalid.";
    return APP_ERR_OK;
}

APP_ERROR MxpiMetadataGraph::MarkAllNodesAsInvalid()
{
    LogDebug << "Begin to mark all nodes as invalid.";
    for (auto iter : graphNodes_) {
        auto node = iter.second;
        LogDebug << "mark node(" << node->nodeName << ") as invalid.";
        node->isValid = false;
    }
    LogDebug << "End to mark all nodes as invalid.";
    return APP_ERR_OK;
}

APP_ERROR MxpiMetadataGraph::RemoveNodeMessage(const std::string& nodeName)
{
    LogDebug << "Begin to mark a node(" << nodeName << ") as invalid.";
    if (graphNodes_.find(nodeName) == graphNodes_.end()) {
        LogWarn << "Directed node(" << nodeName << ") does not exist.";
        return APP_ERR_PLUGIN_TOOLKIT_NODE_NOT_EXIST;
    }
    auto markNode = graphNodes_.at(nodeName);
    markNode->nodeMessage = nullptr;
    LogDebug << "End to mark a node(" << nodeName << ") as invalid.";
    return APP_ERR_OK;
}

APP_ERROR MxpiMetadataGraph::RemoveNodeListMessage(const std::string& nodeListName)
{
    if (MxBase::StringUtils::HasInvalidChar(nodeListName)) {
        LogError << "NodeListName has invalid char." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Begin to remove node list(" << nodeListName << ") message.";
    if (nodeLists_.find(nodeListName) == nodeLists_.end()) {
        LogWarn << "Node list(" << nodeListName << ") does not exist.";
        return APP_ERR_PLUGIN_TOOLKIT_NODELIST_NOT_EXIST;
    }

    for (auto nodeName : nodeLists_.at(nodeListName)) {
        APP_ERROR ret = RemoveNodeMessage(nodeName);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    LogDebug << "End to remove node list(" << nodeListName << ") message.";
    return APP_ERR_OK;
}

void MxpiMetadataGraph::GetBase64StrFromVisionDataIfHave(DirectedNode& node)
{
    if (typeid(*node.nodeMessage) != typeid(MxpiVision)) {
        return;
    }
    MxpiVision *vision = const_cast<MxpiVision *>(reinterpret_cast<const MxpiVision *>(node.nodeMessage));
    if (!vision->has_visiondata()) {
        return;
    }
    MxpiVisionData *visionData = vision->mutable_visiondata();
    if (visionData->datasize() <= 0) {
        return;
    }

    std::string dataStr;
    if ((MxBase::MemoryData::MemoryType)visionData->memtype() != MxBase::MemoryData::MEMORY_HOST) {
        MxBase::MemoryData memorySrc((void *)visionData->dataptr(), visionData->datasize(),
            (MxBase::MemoryData::MemoryType)visionData->memtype());
        MxBase::MemoryData memoryDst(visionData->datasize(), MxBase::MemoryData::MEMORY_HOST);
        APP_ERROR ret =  MxBase::MemoryHelper::MxbsMallocAndCopy(memoryDst, memorySrc);
        if (ret != APP_ERR_OK) {
            LogError << "GetBase64StrFromVisionData aclrtMemcpy failed." << GetErrorInfo(ret);
            return;
        }
        dataStr = std::string((char *)memoryDst.ptrData, memoryDst.size);
        MxBase::MemoryHelper::MxbsFree(memoryDst);
    } else {
        dataStr = std::string((char *)visionData->dataptr(), visionData->datasize());
    }
    visionData->set_datastr(dataStr);
}

void MxpiMetadataGraph::GetJsonValueFromOneNode(DirectedNode& node, std::string& key, nlohmann::json& jsonValue)
{
    LogDebug << "Begin to get json value form node(" << node.nodeName << ").";
    std::string jsonString;
    if (node.nodeMessage == nullptr) {
        LogWarn << "Metadata in " << node.nodeName << " is nullptr, discard.";
        key = node.typeName;
        jsonValue = {};
        return;
    }
    const google::protobuf::Descriptor* desc = node.nodeMessage->GetDescriptor();
    if (desc == nullptr) {
        LogWarn << "Invalid node message of node(" << node.nodeName << "). Invalid protobuf message descriptor.";
        return;
    }
    key = desc->name();

    // convert vision data buffer to base64 string if need
    GetBase64StrFromVisionDataIfHave(node);

    // print json string of the current node
    google::protobuf::util::JsonPrintOptions options;
    options.always_print_primitive_fields = true;
    auto status =
        google::protobuf::util::MessageToJsonString(*node.nodeMessage, &jsonString, options);
    jsonValue = nlohmann::json::parse(jsonString);
    if (isEraseHeaderVecInfo_ && jsonValue.find("headerVec") != jsonValue.end()) {
        jsonValue.erase("headerVec");
    }
    LogDebug << "End to get json value form node(" << node.nodeName << ").";
}

// mxpitodo what if the node has more than one parents.
nlohmann::json MxpiMetadataGraph::GetJsonScanOneNode(DirectedNode& node, std::string& key)
{
    LogDebug << "Begin to scan node(" << node.nodeName << ").";
    nlohmann::json jsonValue;
    if (!node.isValid) {
        LogWarn << "node(" << node.nodeName << ") is invalid.";
        return jsonValue;
    }
    // add json value from one node
    GetJsonValueFromOneNode(node, key, jsonValue);

    // iterate through next nodes
    GetJsonScanNextNodes(node, jsonValue);
    LogDebug << "End to scan node(" << node.nodeName << ").";
    return jsonValue;
}

static bool SortCompare(const DirectedNode* nodeA, const DirectedNode* nodeB)
{
    auto dataTypeA = nodeA->typeName;
    auto dataTypeB = nodeB->typeName;
    return std::strcmp(dataTypeA.c_str(), dataTypeB.c_str()) < 0;
}

void MxpiMetadataGraph::GetJsonScanNextNodes(DirectedNode& node, nlohmann::json& output)
{
    LogDebug << "Begin to scan next node(" << node.nodeName << ").";
    if (node.nextNodes.empty()) {
        return;
    }
    // iterate through next nodes
    auto nextNodes = node.nextNodes;
    std::sort(nextNodes.begin(), nextNodes.end(), SortCompare);
    std::string lastKey;
    std::string currentKey;
    nlohmann::json jsonArray;
    int index = 0;
    for (auto iterNextNode : nextNodes) {
        nlohmann::json jsonValue = GetJsonScanOneNode(*iterNextNode, currentKey);

        if ((!lastKey.empty()) && (lastKey != currentKey)) {
            output[lastKey] = jsonArray;
            jsonArray.clear();
            index = 0;
        }
        lastKey = currentKey;
        jsonArray[index] = jsonValue;
        index++;
    }
    if (!currentKey.empty()) {
        output[currentKey] = jsonArray;
    }
    LogDebug << "End to scan next node(" << node.nodeName << ").";
}

void MxpiMetadataGraph::SqueezeGraph(DirectedNode& node, std::vector<DirectedNode*>& nextNodes)
{
    LogDebug << "Begin to squeeze graph start with node(" << node.nodeName << ").";
    if (nextNodes.empty() || nextNodes[0]->isRead) {
        return;
    }

    for (auto iterNextNode : nextNodes) {
        iterNextNode->isRead = true;
        std::vector<DirectedNode*> nextNodesChild = std::move(iterNextNode->nextNodes);
        std::sort(nextNodesChild.begin(), nextNodesChild.end(), SortCompare);
        if (iterNextNode->isValid) {
            node.nextNodes.push_back(iterNextNode);
            SqueezeGraph(*iterNextNode, nextNodesChild);
        } else {
            SqueezeGraph(node, nextNodesChild);
        }
    }
    LogDebug << "End to squeeze graph start with node(" << node.nodeName << ").";
}

std::string MxpiMetadataGraph::GetJsonString()
{
    LogDebug << "Begin to get json string of the graph.";
    std::vector<DirectedNode*> nextNodes = std::move(rootNode_.nextNodes);
    std::sort(nextNodes.begin(), nextNodes.end(), SortCompare);
    rootNode_.isRead = false;
    SqueezeGraph(rootNode_, nextNodes);

    nlohmann::json jsonValue;
    GetJsonScanNextNodes(rootNode_, jsonValue);
    LogDebug << "End to get json string of the graph.";
    return jsonValue.dump();
}

std::string MxpiMetadataGraph::GetJsonStringFromNodelist(const std::string& nodeListName)
{
    LogDebug << "Begin to get json string from node list(" << nodeListName <<").";
    if (nodeLists_.find(nodeListName) == nodeLists_.end()) {
        if (MxBase::StringUtils::HasInvalidChar(nodeListName)) {
            LogError << "NodeListName has invalid char." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        } else {
            LogWarn << "Node list(" << nodeListName << ")does not exist.";
        }
        return std::string();
    }

    int nodeIndex = 0;
    std::string lastKey;
    std::string currentKey;
    nlohmann::json output;
    nlohmann::json jsonArray;
    for (const auto& nodeName : nodeLists_.at(nodeListName)) {
        if (graphNodes_.find(nodeName) == graphNodes_.end()) {
            LogWarn << "Node (" << nodeName << ")does not exist.";
            continue;
        }
        auto markNode = graphNodes_.at(nodeName);
        nlohmann::json jsonValue;
        if (!markNode->isValid) {
            LogWarn << "Node(" << markNode->nodeName << ") is invalid.";
            continue;
        }

        // add json value from one node
        GetJsonValueFromOneNode(*markNode, currentKey, jsonValue);
        if ((!lastKey.empty()) && (lastKey != currentKey)) {
            output[lastKey] = jsonArray;
            jsonArray.clear();
            nodeIndex = 0;
        }
        lastKey = currentKey;
        jsonArray[nodeIndex] = jsonValue;
        nodeIndex++;
    }
    if (!currentKey.empty()) {
        output[currentKey] = jsonArray;
    }
    LogDebug << "End to get json string from node list(" << nodeListName <<").";
    return output.dump();
}

void MxpiMetadataGraph::DebugPrintGraphInfo(DirectedNode& rootNode)
{
    for (auto& node : rootNode.nextNodes) {
        LogDebug << "graph info nodeName(" << node->nodeName << ").";
        DebugPrintGraphInfo(*node);
    }
}

void MxpiMetadataGraph::SetEraseHeaderVecFlag(bool flag)
{
    isEraseHeaderVecInfo_ = flag;
}
}  // namespace MxTools

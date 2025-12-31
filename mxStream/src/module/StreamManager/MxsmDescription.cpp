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

#include "MxStream/StreamManager/MxsmDescription.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxStream/DataType/DataType.h"
#include "MxStream/StreamManager/MxsmElement.h"

namespace MxStream {
const size_t MAX_REC_TIMES = 4096;
const size_t MAX_ELEMENT_NUMS = 5120;

APP_ERROR MxsmDescription::CreateDescription(std::vector<PluginNode>& pluginNodeVec)
{
    if (isDescCreated_) {
        LogWarn << "Stream description has been created before.";
        return APP_ERR_OK;
    }
    APP_ERROR ret = CreatePipeline(pluginNodeVec);
    if (ret != APP_ERR_OK) {
        LogError << "MxsmDescription construction failed." << GetErrorInfo(ret);
        return ret;
    }
    isDescCreated_ = true;
    return APP_ERR_OK;
}

APP_ERROR MxsmDescription::CreateDescription(std::vector<PluginNode>& inputs, std::vector<PluginNode>& outputs)
{
    if (isDescCreated_) {
        LogWarn << "Stream description has been created before.";
        return APP_ERR_OK;
    }
    APP_ERROR ret = CreatePipeline(inputs, outputs);
    if (ret != APP_ERR_OK) {
        LogError << "MxsmDescription construction failed." << GetErrorInfo(ret);
        return ret;
    }
    isDescCreated_ = true;
    return APP_ERR_OK;
}

MxsmDescription::MxsmDescription(const std::string name)
{
    streamName_ = name;
    streamObject_ = nlohmann::json::object();
    if (MxBase::FileUtils::CheckFileExists(name)) {
        isDescCreated_ = true;
        nlohmann::json StreamJson;
        std::string realFilePath;
        if (!MxBase::FileUtils::RegularFilePath(name, realFilePath)) {
            LogError << "Failed to get canonicalized file path." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return;
        }
        std::string pipelineContent = MxBase::FileUtils::ReadFileContent(realFilePath);
        try {
            StreamJson = nlohmann::json::parse(pipelineContent);
            if (MxBase::StringUtils::HasInvalidChar(StreamJson.dump())) {
                LogError << "Pipeline has invalid char." << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
                return;
            }
        } catch (const std::exception& ex) {
            LogError << "Pipeline is not a valid json. Parse json value of stream failed.\
                Error: (" << ex.what() << ")." << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
            return;
        }
        if (!StreamJson.is_object() || StreamJson.empty()) {
            LogError << "Pipeline is not a valid json object or it is empty."
                     << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
            return;
        }
        if (StreamJson.size() != 1) {
            LogError << "Pipeline has " << StreamJson.size() << " stream objects."
                     << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
            return;
        }
        for (auto& iter: StreamJson.items()) {
            streamName_ = iter.key();
            streamObject_ = iter.value();
        }
    }
    if (streamObject_.find(STREAM_CONFIG_KEY) == streamObject_.end()) {
        streamObject_[STREAM_CONFIG_KEY] = nlohmann::json::object();
    }
}

MxsmDescription::MxsmDescription(const std::string pipelinePath, const std::string streamName)
{
    if (!MxBase::FileUtils::CheckFileExists(pipelinePath)) {
        LogError << "Pipeline is not existed." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return;
    }
    streamObject_ = nlohmann::json::object();
    nlohmann::json StreamJson;
    std::string pipelineContent = MxBase::FileUtils::ReadFileContent(pipelinePath);
    try {
        StreamJson = nlohmann::json::parse(pipelineContent);
        if (MxBase::StringUtils::HasInvalidChar(StreamJson)) {
            LogError << "Pipeline has invalid char." << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
            return;
        }
    } catch (const std::exception& ex) {
        LogError << "Pipeline is not a valid json. Parse json value of stream failed.\
            Error: (" << ex.what() << ")." << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
        return;
    }
    if (!StreamJson.is_object() || StreamJson.empty()) {
        LogError << "Pipeline is not a valid json object or it is empty."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
        return;
    }
    if (StreamJson.find(streamName) == StreamJson.end()) {
        LogError << "StreamName is not existed in pipeline file."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
        return;
    }
    for (auto& iter: StreamJson.items()) {
        streamName_ = iter.key();
        if (streamName_ != streamName) {
            continue;
        } else {
            streamObject_ = iter.value();
            break;
        }
    }
    if (streamObject_.find(STREAM_CONFIG_KEY) == streamObject_.end()) {
        streamObject_[STREAM_CONFIG_KEY] = nlohmann::json::object();
    }
    isDescCreated_ = true;
}

MxsmDescription::MxsmDescription(const std::string& streamName,
    const nlohmann::json& streamObject): streamName_(streamName), streamObject_(streamObject)
{
}

std::vector<MxsmDescription> MxsmDescription::GetStreamDescFromPipeline(const std::string pipelinePath)
{
    std::vector<MxsmDescription> mxsmDescriptionVec;
    nlohmann::json StreamJson;
    std::string pipelineContent = MxBase::FileUtils::ReadFileContent(pipelinePath);
    try {
        StreamJson = nlohmann::json::parse(pipelineContent);
        if (MxBase::StringUtils::HasInvalidChar(StreamJson)) {
            LogError << "Pipeline has invalid char." << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
            return mxsmDescriptionVec;
        }
    } catch (const std::exception& ex) {
        LogError << "The stream config file is not a valid json. Parse json value of stream failed. Error: ("
                 << ex.what() << ")." << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
        return mxsmDescriptionVec;
    }
    if (!StreamJson.is_object() || StreamJson.empty()) {
        LogError << "The stream config file is not a valid json object or it is empty."
                 << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
        return mxsmDescriptionVec;
    }
    LogInfo << "The stream config file has " << StreamJson.size() << " stream objects.";
    for (auto& iter: StreamJson.items()) {
        std::string streamName = iter.key();
        nlohmann::json streamObject = iter.value();
        if (streamObject.find(STREAM_CONFIG_KEY) == streamObject.end()) {
            streamObject[STREAM_CONFIG_KEY] = nlohmann::json::object();
        }
        MxsmDescription mxsmDescription(streamName, streamObject);
        mxsmDescriptionVec.push_back(mxsmDescription);
    }
    return mxsmDescriptionVec;
}

MxsmDescription::~MxsmDescription()
{
}

std::string MxsmDescription::GetStreamName()
{
    return streamName_;
}

nlohmann::json MxsmDescription::GetStreamJson()
{
    return streamObject_;
}

APP_ERROR MxsmDescription::SetPluginName(PluginNode& pluginNode)
{
    if (!pluginNode.PluginName().empty()) {
        return APP_ERR_OK;
    }
    std::string factory(pluginNode.Factory());
    if (pluginCountMap_.find(factory) == pluginCountMap_.end()) {
        pluginCountMap_[factory] = 0;
    } else {
        ++pluginCountMap_[factory];
    }
    std::stringstream ss;
    ss << pluginCountMap_[factory];
    pluginNode.SetPluginName(factory + ss.str());
    return APP_ERR_OK;
}

APP_ERROR MxsmDescription::CreatePipeline(std::vector<PluginNode>& pluginNodeVec)
{
    if (pluginNodeVec.empty()) {
        LogError << "Plugin Node Vector can not be empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (auto iter = pluginNodeVec.begin(); iter != pluginNodeVec.end(); ++iter) {
        APP_ERROR ret = SetPluginName(*iter);
        if (ret != APP_ERR_OK) {
            LogError << "Set plugin name failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return ret;
        }
    }
    for (auto iter = pluginNodeVec.begin() + 1; iter != pluginNodeVec.end(); ++iter) {
        (iter - 1)->SetNextNode(iter->PluginName());
    }
    for (auto iter = pluginNodeVec.begin(); iter != pluginNodeVec.end(); ++iter) {
        streamObject_[iter->PluginName()] = nlohmann::json::parse(iter->ToJson());
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmDescription::SetPluginName(std::vector<PluginNode>& inputs, std::vector<PluginNode>& outputs)
{
    APP_ERROR ret = APP_ERR_OK;
    size_t inputsSize = inputs.size();
    size_t outputsSize = outputs.size();
    for (size_t i = 0; i < inputsSize; ++i) {
        SetPluginName(inputs[i]);
        std::list<int> existNodeList;
        existNodeList.emplace_back(inputs[i].PluginId());
        ret = SetPluginNameWithRecursion(inputs[i], existNodeList);
        existNodeList.pop_back();
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    for (size_t i = 0; i < outputsSize; ++i) {
        SetPluginName(outputs[i]);
        std::list<int> existNodeList;
        existNodeList.emplace_back(outputs[i].PluginId());
        ret = SetPluginNameWithRecursion(outputs[i], existNodeList);
        existNodeList.pop_back();
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    return ret;
}

APP_ERROR MxsmDescription::CreatePipeline(std::vector<PluginNode>& inputs, std::vector<PluginNode>& outputs)
{
    APP_ERROR ret = SetPluginName(inputs, outputs);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = SetNextNodes(inputs, outputs);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = SetPluginJsonObjects(inputs, outputs);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmDescription::SetNextNodes(std::vector<PluginNode>& inputs, std::vector<PluginNode>& outputs)
{
    APP_ERROR ret = APP_ERR_OK;
    size_t inputsSize = inputs.size();
    size_t outputsSize = outputs.size();
    for (size_t i = 0; i < inputsSize; ++i) {
        inputs[i].SetNextNodes();
        std::list<int> existNodeList;
        existNodeList.emplace_back(inputs[i].PluginId());
        ret = SetNextNodeWithRecursion(inputs[i], existNodeList);
        existNodeList.pop_back();
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    for (size_t i = 0; i < outputsSize; ++i) {
        outputs[i].SetNextNodes();
        std::list<int> existNodeList;
        existNodeList.emplace_back(outputs[i].PluginId());
        ret = SetNextNodeWithRecursion(outputs[i], existNodeList);
        existNodeList.pop_back();
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    return ret;
}

APP_ERROR MxsmDescription::SetPluginJsonObjects(std::vector<PluginNode>& inputs, std::vector<PluginNode>& outputs)
{
    APP_ERROR ret = APP_ERR_OK;
    size_t inputsSize = inputs.size();
    size_t outputsSize = outputs.size();
    for (size_t i = 0; i < inputsSize; ++i) {
        std::list<int> existNodeList;
        existNodeList.emplace_back(inputs[i].PluginId());
        ret = TraversePluginNode(inputs[i], existNodeList);
        existNodeList.pop_back();
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    for (size_t i = 0; i < outputsSize; ++i) {
        std::list<int> existNodeList;
        existNodeList.emplace_back(outputs[i].PluginId());
        ret = TraversePluginNode(outputs[i], existNodeList);
        existNodeList.pop_back();
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    return ret;
}

APP_ERROR MxsmDescription::TraversePluginNode(const PluginNode& curNode, std::list<int>& existNodeList)
{
    traverseNodeTimes_++;
    if (traverseNodeTimes_ > MAX_REC_TIMES) {
        LogError << "Too many recursion times in pipeline." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    streamObject_[curNode.PluginName()] = nlohmann::json::parse(curNode.ToJson());
    for (auto &nextNode : curNode.NextNodes()) {
        if (std::find(existNodeList.begin(), existNodeList.end(), nextNode.PluginId()) != existNodeList.end()) {
            LogError << "PluginNode of FunctionalStream"
                     << " is a loop PluginNode." << GetErrorInfo(APP_ERR_STREAM_ELEMENT_EXIST);
            return APP_ERR_STREAM_ELEMENT_EXIST;
        }
        existNodeList.emplace_back(nextNode.PluginId());
        APP_ERROR ret = TraversePluginNode(nextNode, existNodeList);
        existNodeList.pop_back();
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmDescription::SetPluginNameWithRecursion(PluginNode& curNode, std::list<int>& existNodeList)
{
    setPluginNameTimes_++;
    if (setPluginNameTimes_ > MAX_REC_TIMES) {
        LogError << "Too many recursion times in pipeline." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (auto &nextNode : curNode.NextNodes()) {
        SetPluginName(nextNode);
        if (std::find(existNodeList.begin(), existNodeList.end(), nextNode.PluginId()) != existNodeList.end()) {
            LogError << "PluginNode of FunctionalStream"
                     << " is a loop PluginNode." << GetErrorInfo(APP_ERR_STREAM_ELEMENT_EXIST);
            return APP_ERR_STREAM_ELEMENT_EXIST;
        }
        existNodeList.emplace_back(nextNode.PluginId());
        APP_ERROR ret = SetPluginNameWithRecursion(nextNode, existNodeList);
        existNodeList.pop_back();
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmDescription::SetNextNodeWithRecursion(PluginNode& curNode, std::list<int>& existNodeList)
{
    setNextNodeTimes_++;
    if (setNextNodeTimes_ > MAX_REC_TIMES) {
        LogError << "Too many recursion times in pipeline." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (auto &nextNode : curNode.NextNodes()) {
        nextNode.SetNextNodes();
        if (std::find(existNodeList.begin(), existNodeList.end(), nextNode.PluginId()) != existNodeList.end()) {
            LogError << "PluginNode of FunctionalStream"
                     << " is a loop PluginNode." << GetErrorInfo(APP_ERR_STREAM_ELEMENT_EXIST);
            return APP_ERR_STREAM_ELEMENT_EXIST;
        }
        existNodeList.emplace_back(nextNode.PluginId());
        APP_ERROR ret = SetNextNodeWithRecursion(nextNode, existNodeList);
        existNodeList.pop_back();
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    return APP_ERR_OK;
}

/* * Validate Stream json object
 */
APP_ERROR MxsmDescription::ValidateStreamObject(const nlohmann::json& streamObject)
{
    if (streamObject.empty()) {
        LogError << "Stream object is empty, invalid stream config" << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
        return APP_ERR_STREAM_INVALID_CONFIG;
    }
    size_t elementNum = 0;
    for (auto iter = streamObject.begin(); iter != streamObject.end(); ++iter) {
        if (!iter.value().is_object()) {
            LogError << "The value of stream is not an object, invalid stream config"
                     << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
            return APP_ERR_STREAM_INVALID_CONFIG;
        }
        elementNum++;
        if (elementNum > MAX_ELEMENT_NUMS) {
            LogError << "Too many elements in pipeline." << GetErrorInfo(APP_ERR_STREAM_INVALID_CONFIG);
            return APP_ERR_STREAM_INVALID_CONFIG;
        }
        nlohmann::json elementObject = iter.value();
        if (elementObject.find(ELEMENT_FACTORY) == elementObject.end()) {
            continue;
        }

        APP_ERROR ret = MxsmElement::ValidateElementObject(elementObject);
        if (ret != APP_ERR_OK) {
            std::string element = iter.key();
            if (MxBase::StringUtils::HasInvalidChar(element)) {
                MxBase::StringUtils::ReplaceInvalidChar(element);
            }
            auto factoryName = elementObject.find(ELEMENT_FACTORY);
            if (factoryName != elementObject.end() && (*factoryName).is_string()) {
                LogError << element << " is an invalid element of "<< *factoryName <<"."
                         << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID) ;
            } else {
                LogError << "Element(" << element << ") is invalid." << GetErrorInfo(APP_ERR_STREAM_ELEMENT_INVALID) ;
            }
            return ret;
        }

        ret = ValidateElementLinks(elementObject, streamObject);
        if (ret != APP_ERR_OK) {
            LogError << "The next link is invalid." << GetErrorInfo(ret);
            return ret;
        }
        LogInfo << "Validates element successfully.";
    }

    return APP_ERR_OK;
}

APP_ERROR MxsmDescription::ValidateElementLinks(const nlohmann::json& elementObject,
    const nlohmann::json& streamObject)
{
    auto nextElementIter = elementObject.find(ELEMENT_NEXT);
    if (nextElementIter != elementObject.end()) {
        APP_ERROR ret = ValidateOneDirectionLinks(nextElementIter.value(), streamObject);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxsmDescription::ValidateOneDirectionLinks(const nlohmann::json& link,
    const nlohmann::json& streamObject)
{
    if (link.is_string()) {
        const std::string& next = link;
        std::string elementName = GetNextElementName(next);
        if (streamObject.find(elementName) == streamObject.end()) {
            LogError << "Can not find the next link, invalid links."
                     << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
            return APP_ERR_STREAM_INVALID_LINK;
        }
    } else if (link.is_array()) {
        nlohmann::json tmpArray = link;
        for (const auto& iter : tmpArray) {
            if (!iter.is_string()) {
                return APP_ERR_STREAM_INVALID_CONFIG;
            }
            std::string elementName = GetNextElementName(iter);
            if (streamObject.find(elementName) == streamObject.end()) {
                LogError << "Can not find the link, invalid links."
                         << GetErrorInfo(APP_ERR_STREAM_INVALID_LINK);
                return APP_ERR_STREAM_INVALID_LINK;
            }
        }
    } else {
        return APP_ERR_STREAM_INVALID_CONFIG;
    }

    return APP_ERR_OK;
}

std::string MxsmDescription::GetNextElementName(const std::string& elementName)
{
    int number = count(elementName.begin(), elementName.end(), ':');
    if (number == 1) {
        size_t pos = elementName.find(':');
        return elementName.substr(0, pos);
    }
    return elementName;
}
}
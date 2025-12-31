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
 * Description: Defines the metadata manager to store data after operations on the buffer or metadata.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiAiInfoSproc.h"
#include "MxTools/PluginToolkit/metadata/MxpiAiInfos.h"
#include "MxTools/PluginToolkit/MetadataGraph/MxpiMetadataGraph.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxpiMetadataManagerDptr.hpp"
#include "MxBase/ErrorCode/ErrorCode.h"

using namespace MxTools;
using namespace std;


namespace {
const std::string ERROR_INFO_KEY = "ErrorInfo";
using errorFunc = std::string(*)(const APP_ERROR err, std::string callingFuncName);
static errorFunc GetErrorInfos = GetErrorInfo;
const std::vector<std::string> invalidKeys = {
    "ReserveMetadataGraph", "ReservedVisionList", "ErrorInfo"
};
static bool HasInvalidKey(std::string key)
{
    for (auto invalidKey : invalidKeys) {
        if (key == invalidKey) {
            return true;
        }
    }
    return false;
}
}

MxpiMetadataManager::MxpiMetadataManager(MxpiBuffer& mxpiBuffer)
{
    pMxpiMetadataManagerDptr_ = MxBase::MemoryHelper::MakeShared<MxpiMetadataManagerDptr>();
    if (pMxpiMetadataManagerDptr_ == nullptr) {
        LogError << "Create MxpiMetadataManagerDptr object failed. Failed to allocate memory."
                 << GetErrorInfos(APP_ERR_COMM_ALLOC_MEM, "");
        throw std::runtime_error(GetErrorInfos(APP_ERR_COMM_ALLOC_MEM, ""));
    }
    pMxpiMetadataManagerDptr_->mxpiBuffer_ = mxpiBuffer;
}

MxpiMetadataManager::~MxpiMetadataManager() {}

APP_ERROR MxpiMetadataManager::AddMetadata(const std::string& key, std::shared_ptr<void> metadata)
{
    if (MxBase::StringUtils::HasInvalidChar(key)) {
        LogError << "Input key has invalid char." << GetErrorInfos(APP_ERR_COMM_INVALID_PARAM, "");
        return APP_ERR_COMM_INVALID_PARAM;
    }

    LogDebug << "Begin to add a metadata(" << key <<") on the buffer.";
    if (key.empty()) {
        LogError << "The key cannot be empty." << GetErrorInfos(APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_EMPTY, "");
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_EMPTY;
    }
    if (metadata == nullptr) {
        LogError << "metadata pointer is nullptr." << GetErrorInfos(APP_ERR_COMM_INVALID_POINTER, "");
        return APP_ERR_COMM_INVALID_POINTER;
    }
    MxpiAiInfos* currentMetaInfo = nullptr;
    APP_ERROR ret = pMxpiMetadataManagerDptr_->GetMxpiMetaInfos(currentMetaInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::unique_lock<std::mutex> sendDataLock(*currentMetaInfo->metadataMutex);
    return pMxpiMetadataManagerDptr_->AddMetadataInternal(key, metadata, currentMetaInfo);
}

APP_ERROR MxpiMetadataManager::AddProtoMetadata(const std::string& key, std::shared_ptr<void> metadata)
{
    if (MxBase::StringUtils::HasInvalidChar(key)) {
        LogError << "Input key has invalid char." << GetErrorInfos(APP_ERR_COMM_INVALID_PARAM, "");
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (HasInvalidKey(key)) {
        LogError << "Input key has invalid key." << GetErrorInfos(APP_ERR_COMM_INVALID_PARAM, "");
        return APP_ERR_COMM_INVALID_PARAM;
    }
    
    LogDebug << "Begin to add a metadata(" << key <<") on the buffer.";
    if (key.empty()) {
        LogError << "The key cannot be empty." << GetErrorInfos(APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_EMPTY, "");
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_EMPTY;
    }
    if (metadata == nullptr) {
        LogError << "metadata pointer is nullptr." << GetErrorInfos(APP_ERR_COMM_INVALID_POINTER, "");
        return APP_ERR_COMM_INVALID_POINTER;
    }
    MxpiAiInfos* currentMetaInfo = nullptr;
    APP_ERROR ret = pMxpiMetadataManagerDptr_->GetMxpiMetaInfos(currentMetaInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::unique_lock<std::mutex> sendDataLock(*currentMetaInfo->metadataMutex);
    MxpiAiPMetaData mxpiAiPMetaData {currentMetaInfo};
    if (HadProtobufKey(mxpiAiPMetaData, key)) {
        LogWarn << "Already has the metadata key(" << key <<"), can't add it again.";
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ALREADY_EXIST;
    }
    currentMetaInfo->mxpiProtobufMap[key] = std::static_pointer_cast<google::protobuf::Message>(metadata);
    auto mxpiMetadataGraph = pMxpiMetadataManagerDptr_->GetMetadataGraphInstanceInternal(currentMetaInfo);
    if (mxpiMetadataGraph == nullptr) {
        LogError << "mxpiMetadataGraph pointer is nullptr." << GetErrorInfos(APP_ERR_COMM_INVALID_POINTER, "");
        return APP_ERR_COMM_INVALID_POINTER;
    }
    mxpiMetadataGraph->AddNodeList(key, std::static_pointer_cast<google::protobuf::Message>(metadata));
    LogDebug << "End to add a proto metadata(" << key <<") on the buffer.";
    return APP_ERR_OK;
}

std::shared_ptr<void> MxpiMetadataManager::GetMetadata(const std::string& key)
{
    if (MxBase::StringUtils::HasInvalidChar(key)) {
        LogError << "Input key has invalid char." << GetErrorInfos(APP_ERR_COMM_INVALID_PARAM, "");
        return nullptr;
    }

    LogDebug << "Begin to get the metadata(" << key <<") from the buffer.";
    if (key.empty()) {
        LogError << "The key cannot be empty." << GetErrorInfos(APP_ERR_COMM_FAILURE, "");
        return nullptr;
    }
    MxpiAiInfos* currentMetaInfo = nullptr;
    APP_ERROR ret = pMxpiMetadataManagerDptr_->GetMxpiMetaInfos(currentMetaInfo);
    if (ret != APP_ERR_OK) {
        return nullptr;
    }
    std::unique_lock<std::mutex> sendDataLock(*currentMetaInfo->metadataMutex);
    return pMxpiMetadataManagerDptr_->GetMetadataInternal(key, currentMetaInfo);
}

std::shared_ptr<void> MxpiMetadataManager::GetMetadataWithType(const std::string& key, std::string type)
{
    if (MxBase::StringUtils::HasInvalidChar(key) || MxBase::StringUtils::HasInvalidChar(type)) {
        LogError << "Input key or type has invalid char." << GetErrorInfos(APP_ERR_COMM_INVALID_PARAM, "");
        return nullptr;
    }

    LogDebug << "Begin to get the metadata(" << key <<") with type(" << type << ") from the buffer.";
    auto foo = GetMetadata(key);
    if (foo == nullptr) {
        LogError << "Fail to GetMetadata with key(" << key << ")." << GetErrorInfos(APP_ERR_COMM_FAILURE, "");
        return nullptr;
    }
    const google::protobuf::Descriptor* desc = ((google::protobuf::Message*)foo.get())->GetDescriptor();
    if (desc == nullptr) {
        LogError << "Invalid metadata descriptor." << GetErrorInfos(APP_ERR_COMM_FAILURE, "");
        return nullptr;
    }
    if (desc->name() != type) {
        LogError << "The type of metadata (" << desc->name() << ") is not matched to designed (" << type << ")."
                 << GetErrorInfos(APP_ERR_COMM_FAILURE, "");
        return nullptr;
    }
    return foo;
}

APP_ERROR MxpiMetadataManager::RemoveMetadata(const std::string& key)
{
    if (MxBase::StringUtils::HasInvalidChar(key)) {
        LogError << "Input key has invalid char." << GetErrorInfos(APP_ERR_COMM_INVALID_PARAM, "");
        return APP_ERR_COMM_INVALID_PARAM;
    }

    LogDebug << "Begin to remove the metadata(" << key <<") from the buffer.";
    if (key.empty()) {
        LogError << "The key cannot be empty." << GetErrorInfos(APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_EMPTY, "");
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_EMPTY;
    }
    MxpiAiInfos* currentMetaInfo = nullptr;
    APP_ERROR ret = pMxpiMetadataManagerDptr_->GetMxpiMetaInfos(currentMetaInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::unique_lock<std::mutex> sendDataLock(*currentMetaInfo->metadataMutex);
    return pMxpiMetadataManagerDptr_->RemoveMetadataInternal(key, currentMetaInfo);
}

APP_ERROR MxpiMetadataManager::RemoveProtoMetadata(const std::string& key)
{
    if (MxBase::StringUtils::HasInvalidChar(key)) {
        LogError << "Input key has invalid char." << GetErrorInfos(APP_ERR_COMM_INVALID_PARAM, "");
        return APP_ERR_COMM_INVALID_PARAM;
    }

    LogDebug << "Begin to remove the proto metadata(" << key <<") from the buffer.";
    if (key.empty()) {
        LogError << "The key cannot be empty." << GetErrorInfos(APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_EMPTY, "");
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_EMPTY;
    }
    MxpiAiInfos* currentMetaInfo = nullptr;
    APP_ERROR ret = pMxpiMetadataManagerDptr_->GetMxpiMetaInfos(currentMetaInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::unique_lock<std::mutex> sendDataLock(*currentMetaInfo->metadataMutex);

    ret = pMxpiMetadataManagerDptr_->RemoveMetadataInternal(key, currentMetaInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    auto mxpiMetadataGraph = pMxpiMetadataManagerDptr_->GetMetadataGraphInstanceInternal(currentMetaInfo);
    if (mxpiMetadataGraph == nullptr) {
        LogError << "mxpiMetadataGraph pointer is nullptr." << GetErrorInfos(APP_ERR_COMM_INVALID_POINTER, "");
        return APP_ERR_COMM_INVALID_POINTER;
    }
    ret = mxpiMetadataGraph->RemoveNodeListMessage(key);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = mxpiMetadataGraph->MarkNodeListAsInvalid(key);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    LogDebug << "End to remove the proto metadata(" << key <<") from the buffer.";
    return APP_ERR_OK;
}

APP_ERROR MxpiMetadataManager::CopyMetadata(MxpiBuffer& targetMxpiBuffer)
{
    LogDebug << "Begin to copy metadatas from source buffer to target buffer.";
    MxpiAiInfos* currentMetaInfo = nullptr;
    APP_ERROR ret = pMxpiMetadataManagerDptr_->GetMxpiMetaInfos(currentMetaInfo);
    if (ret != APP_ERR_OK) {
        LogError << "GetMxpiMetaInfos failed." << GetErrorInfos(ret, "");
        return ret;
    }
    std::unique_lock<std::mutex> sendDataLock(*currentMetaInfo->metadataMutex);
    auto metaDataMap = currentMetaInfo->mxpiAiInfoMap;
    auto protobufMap = currentMetaInfo->mxpiProtobufMap;
    if (targetMxpiBuffer.buffer == nullptr) {
        LogError << "targetMxpiBuffer is nullptr." << GetErrorInfos(APP_ERR_COMM_FAILURE, "");
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_BUFFER_IS_NULL;
    }
    MxpiMetaData targetMxpiMetaData {targetMxpiBuffer.buffer};
    MxpiAiInfos* targetMetaData = (MxpiAiInfos*) MxpiMetaGet(targetMxpiMetaData);

    if (targetMetaData == nullptr) {
        LogError << "targetMetaData is nullptr." << GetErrorInfos(APP_ERR_COMM_FAILURE, "");
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
    }
    for (auto it = metaDataMap.begin(); it != metaDataMap.end(); it++) {
        targetMetaData->mxpiAiInfoMap[it->first] = it->second;
    }
    for (auto it = protobufMap.begin(); it != protobufMap.end(); it++) {
        targetMetaData->mxpiProtobufMap[it->first] = it->second;
    }
    LogDebug << "End to copy metadatas from source buffer to target buffer.";
    return APP_ERR_OK;
}

std::shared_ptr<MxpiMetadataGraph> MxpiMetadataManager::GetMetadataGraphInstance()
{
    LogDebug << "Begin to get metadata graph instance.";
    MxpiAiInfos* currentMetaInfo = nullptr;
    APP_ERROR ret = pMxpiMetadataManagerDptr_->GetMxpiMetaInfos(currentMetaInfo);
    if (ret != APP_ERR_OK) {
        return nullptr;
    }
    return pMxpiMetadataManagerDptr_->GetMetadataGraphInstanceInternal(currentMetaInfo);
}

APP_ERROR MxpiMetadataManager::AddErrorInfo(const std::string pluginName, MxpiErrorInfo errorInfo)
{
    if (MxBase::StringUtils::HasInvalidChar(pluginName)) {
        LogError << "Input pluginName has invalid char." << GetErrorInfos(APP_ERR_COMM_INVALID_PARAM, "");
        return APP_ERR_COMM_INVALID_PARAM;
    }

    LogDebug << "Begin to add the error information of plugin(" << pluginName <<").";
    if (pluginName.empty()) {
        LogError << "The pluginName cannot be empty." << GetErrorInfos(APP_ERR_COMM_FAILURE, "");
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_PLUGINNAME_EMPTY;
    }
    MxpiAiInfos* currentMetaInfo = nullptr;
    APP_ERROR ret = pMxpiMetadataManagerDptr_->GetMxpiMetaInfos(currentMetaInfo);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::unique_lock<std::mutex> sendDataLock(*currentMetaInfo->metadataMutex);
    // if the ERROR_INFO_KEY doesn't exist, create a new one
    auto metadataPtr = pMxpiMetadataManagerDptr_->GetMetadataInternal(ERROR_INFO_KEY, currentMetaInfo);
    if (metadataPtr == nullptr) {
        auto mxpiErrorInfoPtr = MxBase::MemoryHelper::MakeShared<std::map<std::string, MxpiErrorInfo>>();
        if (mxpiErrorInfoPtr == nullptr) {
            LogError << "Create map of MxpiErrorInfo object failed. Failed to allocate memory."
                     << GetErrorInfos(APP_ERR_COMM_ALLOC_MEM, "");
            return APP_ERR_COMM_ALLOC_MEM;
        }
        ret = pMxpiMetadataManagerDptr_->AddMetadataInternal(ERROR_INFO_KEY,
            std::static_pointer_cast<void>(mxpiErrorInfoPtr), currentMetaInfo);
        if (ret != APP_ERR_OK) {
            return APP_ERR_PLUGIN_TOOLKIT_METADATA_ADD_ERROR_INFO_FAIL;
        }
    }
    // get data from metadata
    metadataPtr = pMxpiMetadataManagerDptr_->GetMetadataInternal(ERROR_INFO_KEY, currentMetaInfo);
    auto errorInfoMetadataPtr = std::static_pointer_cast<std::map<std::string, MxpiErrorInfo>>(metadataPtr);
    if (errorInfoMetadataPtr->find(pluginName) != errorInfoMetadataPtr->end()) {
        LogWarn << "The key("<< pluginName <<") of error information already exists, can't add it again.";
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_PLUGIN_NAME_KEY_ALREADY_EXIST;
    }
    // add errorInfo by the key of pluginName
    errorInfoMetadataPtr->insert(std::make_pair(pluginName, errorInfo));
    LogDebug << "End to add the error information of plugin(" << pluginName <<").";
    return APP_ERR_OK;
}

std::shared_ptr<std::map<std::string, MxpiErrorInfo>> MxpiMetadataManager::GetErrorInfo()
{
    LogDebug << "Begin to get error information of all plugins.";
    MxpiAiInfos* currentMetaInfo = nullptr;
    APP_ERROR ret = pMxpiMetadataManagerDptr_->GetMxpiMetaInfos(currentMetaInfo);
    if (ret != APP_ERR_OK) {
        return nullptr;
    }
    std::unique_lock<std::mutex> sendDataLock(*currentMetaInfo->metadataMutex);
    auto metadata = pMxpiMetadataManagerDptr_->GetMetadataInternal(ERROR_INFO_KEY, currentMetaInfo);
    auto errorInfo = std::static_pointer_cast<std::map<std::string, MxpiErrorInfo>>(metadata);
    LogDebug << "End to get error information of all plugins.";
    return errorInfo;
}

std::map<std::string, std::shared_ptr<void>> MxpiMetadataManager::GetAllMetaData()
{
    LogDebug << "Begin to get all metadata.";
    MxpiAiInfos* currentMetaInfo = nullptr;
    APP_ERROR ret = pMxpiMetadataManagerDptr_->GetMxpiMetaInfos(currentMetaInfo);
    if (ret != APP_ERR_OK) {
        return std::map<std::string, std::shared_ptr<void>>();
    }
    std::unique_lock<std::mutex> sendDataLock(*currentMetaInfo->metadataMutex);
    auto allMetaMap = currentMetaInfo->mxpiAiInfoMap;
    allMetaMap.insert(currentMetaInfo->mxpiProtobufMap.begin(), currentMetaInfo->mxpiProtobufMap.end());

    return allMetaMap;
}

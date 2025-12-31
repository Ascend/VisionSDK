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
 * Description: MxpiMetadataManager private interface for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */
#ifndef MXPIMETADATAMANAGER_DPTR_H
#define MXPIMETADATAMANAGER_DPTR_H

namespace {
const std::string RESERVE_METADATA_GRAPH_KEY = "ReserveMetadataGraph";
}

namespace MxTools {
struct MxpiAiInfos;

class SDK_UNAVAILABLE_FOR_OTHER MxpiMetadataManagerDptr {
public:
    APP_ERROR GetMxpiMetaInfos(MxpiAiInfos* &currentMetaInfo);

    std::shared_ptr<void> GetMetadataInternal(const std::string &key, MxpiAiInfos *currentMetaInfo);

    APP_ERROR AddMetadataInternal(const std::string &key, std::shared_ptr<void> metadata, MxpiAiInfos *currentMetaInfo);

    std::shared_ptr<MxpiMetadataGraph> GetMetadataGraphInstanceInternal(MxpiAiInfos *currentMetaInfo);

    APP_ERROR RemoveMetadataInternal(const std::string &key, MxpiAiInfos *metaData);

    MxpiBuffer mxpiBuffer_;
};

APP_ERROR MxpiMetadataManagerDptr::GetMxpiMetaInfos(MxpiAiInfos* &currentMetaInfo)
{
    if (mxpiBuffer_.buffer == nullptr) {
        LogError << "Buffer is null, add metadata failed."
                 << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_BUFFER_IS_NULL);
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_BUFFER_IS_NULL;
    }
    MxpiMetaData mxpiMetaData {mxpiBuffer_.buffer};
    currentMetaInfo = (MxpiAiInfos*) MxpiMetaGet(mxpiMetaData);
    if (currentMetaInfo == nullptr) {
        bool isValidMetadata = CreateMetaData(mxpiMetaData);
        if (!isValidMetadata) {
            LogError << "Create metadata failed."
                     << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL);
            return APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
        }
        currentMetaInfo = (MxpiAiInfos*) MxpiMetaGet(mxpiMetaData);
        if (currentMetaInfo == nullptr) {
            LogError << "Create metadata failed."
                     << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL);
            return APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
        }
    }

    return APP_ERR_OK;
}

std::shared_ptr<void> MxpiMetadataManagerDptr::GetMetadataInternal(const std::string &key, MxpiAiInfos *currentMetaInfo)
{
    auto iter1 = currentMetaInfo->mxpiAiInfoMap.find(key);
    auto iter2 = currentMetaInfo->mxpiProtobufMap.find(key);
    if ((iter1 == currentMetaInfo->mxpiAiInfoMap.end() && iter2 == currentMetaInfo->mxpiProtobufMap.end())) {
        LogDebug << "Get metadata from map failed (" << key <<") not exist in buffer.";
        return nullptr;
    } else if ((iter1 != currentMetaInfo->mxpiAiInfoMap.end() && iter2 != currentMetaInfo->mxpiProtobufMap.end())) {
        LogError << "Get metadata from map confilct (" << key <<") exist both in buffer."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    } else if (iter1 != currentMetaInfo->mxpiAiInfoMap.end() && iter2 == currentMetaInfo->mxpiProtobufMap.end()) {
        LogDebug << "End to get the metadata(" << key <<") from the buffer.";
        return iter1->second;
    } else if (iter1 == currentMetaInfo->mxpiAiInfoMap.end() && iter2 != currentMetaInfo->mxpiProtobufMap.end()) {
        LogDebug << "End to get the metadata(" << key <<") from the buffer.";
        return iter2->second;
    }
    return nullptr;
}

APP_ERROR MxpiMetadataManagerDptr::AddMetadataInternal(const std::string &key, std::shared_ptr<void> metadata,
    MxpiAiInfos *currentMetaInfo)
{
    MxpiAiPMetaData mxpiAiPMetaData {currentMetaInfo};
    if (HadProtobufKey(mxpiAiPMetaData, key)) {
        LogWarn << "Already has the metadata key(" << key <<"), can't add it again.";
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ALREADY_EXIST;
    }
    currentMetaInfo->mxpiAiInfoMap[key] = metadata;
    LogDebug << "End to add a metadata(" << key << ") on the buffer.";
    return APP_ERR_OK;
}

std::shared_ptr<MxpiMetadataGraph> MxpiMetadataManagerDptr::GetMetadataGraphInstanceInternal(
    MxpiAiInfos *currentMetaInfo)
{
    std::shared_ptr<MxpiMetadataGraph> mxpiMetadataGraph = nullptr;
    auto metadata = GetMetadataInternal(RESERVE_METADATA_GRAPH_KEY, currentMetaInfo);
    if (metadata) {
        mxpiMetadataGraph = std::static_pointer_cast<MxpiMetadataGraph>(metadata);
    } else {
        mxpiMetadataGraph = MxBase::MemoryHelper::MakeShared<MxpiMetadataGraph>();
        if (mxpiMetadataGraph == nullptr) {
            LogError << "Create MxpiMetadataGraph object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return nullptr;
        }
        AddMetadataInternal(RESERVE_METADATA_GRAPH_KEY, std::static_pointer_cast<void>(mxpiMetadataGraph),
            currentMetaInfo);
    }
    LogDebug << "End to get metadata graph instance.";
    return mxpiMetadataGraph;
}

APP_ERROR MxpiMetadataManagerDptr::RemoveMetadataInternal(const std::string &key, MxpiAiInfos *metaData)
{
    MxpiAiPMetaData mxpiAiPMetaData {metaData};
    if (!HadProtobufKey(mxpiAiPMetaData, key)) {
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST;
    }
    if (mxpiAiPMetaData.pMetaData->mxpiAiInfoMap.find(key) != mxpiAiPMetaData.pMetaData->mxpiAiInfoMap.end()) {
        metaData->mxpiAiInfoMap.erase(key);
        if (metaData->mxpiAiInfoMap.empty()) {
            MxpiMetaData mxpiMetaData {mxpiBuffer_.buffer};
            if (!MxpiMetaRemove(mxpiMetaData)) {
                return APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ERASE_FAIL;
            }
        }
    } else if (mxpiAiPMetaData.pMetaData->mxpiProtobufMap.find(key) !=
        mxpiAiPMetaData.pMetaData->mxpiProtobufMap.end()) {
        metaData->mxpiProtobufMap.erase(key);
        if (metaData->mxpiProtobufMap.empty()) {
            MxpiMetaData mxpiMetaData {mxpiBuffer_.buffer};
            if (!MxpiMetaRemove(mxpiMetaData)) {
                return APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_ERASE_FAIL;
            }
        }
    }
    LogDebug << "End to remove the metadata(" << key << ") from the buffer.";
    return APP_ERR_OK;
}
} // namespace MxTools

#endif // MXPIMETADATAMANAGER_DPTR_H
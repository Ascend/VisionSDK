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

#ifndef MXPLUGINGENERATOR_MXPIMETADATAMANAGER_H
#define MXPLUGINGENERATOR_MXPIMETADATAMANAGER_H

#include <memory>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxTools/PluginToolkit/base/MxPluginBase.h"
#include "MxBase/Common/HiddenAttr.h"

namespace MxTools {
    struct MxpiAiInfos;
}

namespace MxTools {
/**
 * @api
 * @brief Definition of error information data.
 */
struct MxpiErrorInfo {
    APP_ERROR ret;
    std::string errorInfo;
};
/**
 * @api
 * @brief Definition of MxpiMetadataManager class.
 */
class MxpiMetadataManagerDptr;
class MxpiMetadataGraph;
class SDK_AVAILABLE_FOR_OUT MxpiMetadataManager {
public:
    /**
     * @api
     * @brief Initialize a metadata manager with the given buffer.
     * @param mxpiBuffer
     */
    explicit MxpiMetadataManager(MxpiBuffer& mxpiBuffer);

    ~MxpiMetadataManager();

    /**
     * @api
     * @brief Add a metadata to the buffer with the key.
     * @param key
     * @param metadata
     * @return APP_ERROR
     */
    APP_ERROR AddMetadata(const std::string& key, std::shared_ptr<void> metadata);

    /**
     * @api
     * @brief Add a protometadata to the buffer with the key.
     * @param key
     * @param metadata
     * @return APP_ERROR
     */
    APP_ERROR AddProtoMetadata(const std::string& key, std::shared_ptr<void> metadata);

    /**
     * @api
     * @brief Get a metadata from the buffer with the key.
     * @param key
     * @return APP_ERROR
     */
    std::shared_ptr<void> GetMetadata(const std::string& key);

    /**
     * @api
     * @brief Get a metadata from the buffer with the key and designed type.
     * @param key, designed type
     * @return APP_ERROR
     */
    std::shared_ptr<void> GetMetadataWithType(const std::string& key, std::string type);

    /**
     * @api
     * @brief Remove a metadata from the buffer with the key.
     * @param key
     * @return APP_ERROR
     */
    APP_ERROR RemoveMetadata(const std::string& key);

    /**
     * @api
     * @brief Remove a prorometadata from the buffer with the key.
     * @param key
     * @return APP_ERROR
     */
    APP_ERROR RemoveProtoMetadata(const std::string& key);

    /**
     * @api
     * @brief Copy all metadatas from the buffer to target buffer.
     * @param targetMxpiBuffer
     * @return APP_ERROR
     */
    APP_ERROR CopyMetadata(MxpiBuffer& targetMxpiBuffer);

    /**
     * @api
     * @brief Get metadata graph instance.
     * @return std::shared_ptr<MxpiMetadataGraph>
     */
    std::shared_ptr<MxpiMetadataGraph> GetMetadataGraphInstance();

    /**
     * @api
     * @brief Add the plugin's error informations by the key of plugin name into the metadata.
     * @param pluginName
     * @param errorInfo
     * @return APP_ERROR
     */
    APP_ERROR AddErrorInfo(const std::string pluginName, MxpiErrorInfo errorInfo);

    /**
     * @api
     * @brief Get the plugin's error informations by the key of plugin name from the metadata.
     * @return std::shared_ptr<std::map<std::string, MxpiErrorInfo>>
     */
    std::shared_ptr<std::map<std::string, MxpiErrorInfo>> GetErrorInfo();

    /**
     * @api
     * @brief Get all metadata from buffer.
     * @return metadata map
     */
    std::map<std::string, std::shared_ptr<void>> GetAllMetaData();

private:
    MxpiMetadataManager(const MxpiMetadataManager &) = delete;

    MxpiMetadataManager(const MxpiMetadataManager &&) = delete;

    MxpiMetadataManager &operator=(const MxpiMetadataManager &) = delete;

    MxpiMetadataManager &operator=(const MxpiMetadataManager &&) = delete;

private:
    std::shared_ptr<MxTools::MxpiMetadataManagerDptr> pMxpiMetadataManagerDptr_;
};
}
#endif // MXPLUGINGENERATOR_MXPIMETADATAMANAGER_H
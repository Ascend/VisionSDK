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
 * Description: Used to export MxpiBuffer data of the upstream plug-in.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiDumpData/MxpiDumpData.h"
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;
APP_ERROR MxpiDumpData::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    LogInfo << "initialize MxpiDumpData(" << elementName_ << ").";
    std::vector<std::string> parameterNamesPtr = {"filterMetaDataKeys", "requiredMetaDataKeys",
                                                  "location", "dumpMemoryData"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    auto filterMetaDataKeys = std::static_pointer_cast<std::string>(configParamMap["filterMetaDataKeys"]);
    LogInfo << "filterMetaDataKeys=" << *filterMetaDataKeys << "(" << elementName_ << ").";
    filterMetaDataKeys_ = MxBase::StringUtils::Split(*filterMetaDataKeys, ',');

    auto requiredMetaDataKeys = std::static_pointer_cast<std::string>(configParamMap["requiredMetaDataKeys"]);
    LogInfo << "requiredMetaDataKeys=" << *requiredMetaDataKeys << "(" << elementName_ << ").";
    requiredMetaDataKeys_ = MxBase::StringUtils::Split(*requiredMetaDataKeys, ',');

    location_ = *(std::static_pointer_cast<std::string>(configParamMap["location"]));
    LogInfo << "set location_ successfully! (" << elementName_ << ").";

    dumpMemoryData_ = *(std::static_pointer_cast<std::string>(configParamMap["dumpMemoryData"]));

    auto index = location_.find_last_of(MxBase::FileSeparator());
    if (index != std::string::npos) {
        std::string dirs = location_.substr(0, index);
        if (!MxBase::FileUtils::CreateDirectories(dirs)) {
            LogError << "Element(" << elementName_ << ") create directory in location_ error. "
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
    }

    return APP_ERR_OK;
}

APP_ERROR MxpiDumpData::DeInit()
{
    LogInfo << "element(" << elementName_ << ") deInitialize.";
    return APP_ERR_OK;
}

APP_ERROR MxpiDumpData::Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer)
{
    LogDebug << "element(" << elementName_ << ") Begin to process MxpiDumpData.";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (dumpMemoryData_ == "false") {
        MxpiMetadataManager metadataManager(*mxpiBuffer[0]);
        std::shared_ptr<std::string> dumpMemoryValuePtr = MemoryHelper::MakeShared<std::string>("false");
        if (dumpMemoryValuePtr == nullptr) {
            LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            MxpiBufferManager::DestroyBuffer(mxpiBuffer[0]);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        metadataManager.AddMetadata("=dumpMemory=", dumpMemoryValuePtr);
    }

    std::string jsonString = DoDump(*mxpiBuffer[0], filterMetaDataKeys_, requiredMetaDataKeys_);
    if (location_.empty()) {
        MxTools::InputParam inputParam;
        inputParam.dataSize = static_cast<int>(jsonString.size());
        inputParam.ptrData = (void*) jsonString.c_str();
        auto newBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
        if (newBuffer != nullptr) {
            ret = SendData(0, *newBuffer);
            if (ret != APP_ERR_OK) {
                LogError << "SendData error." << GetErrorInfo(ret);
                MxpiBufferManager::DestroyBuffer(mxpiBuffer[0]);
                return ret;
            }
        }

        MxpiBufferManager::DestroyBuffer(mxpiBuffer[0]);
        LogDebug << "element(" << elementName_ << ") End to process MxpiDumpData.";
        return APP_ERR_OK;
    }

    ret = WriteToFile(jsonString);
    if (ret != APP_ERR_OK) {
        LogError << "MxpiDumpData: Write file failed in process." << GetErrorInfo(ret);
        return ret;
    }
    if (dumpMemoryData_ == "false") {
        MxpiMetadataManager metadataManager(*mxpiBuffer[0]);
        metadataManager.RemoveMetadata("=dumpMemory=");
    }
    ret = SendData(0, *mxpiBuffer[0]);
    if (ret != APP_ERR_OK) {
        LogError << "SendData error." << GetErrorInfo(ret);
        return ret;
    }

    LogDebug << "element(" << elementName_ << ") End to process MxpiDumpData.";
    return ret;
}

APP_ERROR MxpiDumpData::WriteToFile(const std::string& jsonString)
{
    if (index_ == 0) {
        if (!MxBase::FileUtils::WriteFileContent(location_, jsonString)) {
            LogError << "MxpiDumpData: Write file failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
    } else {
        std::string fileName = location_ + MxBase::StringUtils::ToString<uint>(index_);
        LogInfo << "element(" << elementName_ << ") dump file.";
        if (!MxBase::FileUtils::WriteFileContent(fileName, jsonString)) {
            LogError << "MxpiDumpData: Write file failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
    }

    ++index_;
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiDumpData::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto filterMetaDtaKeys = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "filterMetaDataKeys", "filterMetaDataKeys", "enter the keys to be filtered, split by ','", "", "", ""
    });
    auto requiredMetaDataKeys = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "requiredMetaDataKeys", "requiredMetaDataKeys", "enter the required keys, split by ','", "", "", ""
    });
    auto location = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "location", "location", "the location to output the file", "", "", ""
    });
    auto dumpMemoryData = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dumpMemoryData", "dumpMemoryData", "whether to dump vision memory or tensor memory", "true", "", ""
    });
    properties = { filterMetaDtaKeys, requiredMetaDataKeys, location, dumpMemoryData };
    return properties;
}

namespace {
MX_PLUGIN_GENERATE(MxpiDumpData)
}

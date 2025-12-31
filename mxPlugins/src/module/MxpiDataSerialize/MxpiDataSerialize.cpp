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
 * Description: Serialize the result into a JSON string.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <google/protobuf/util/json_util.h>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/MetadataGraph/MxpiMetadataGraph.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"
#include "MxPlugins/MxpiDataSerialize/MxpiDataSerialize.h"

using namespace MxTools;
using namespace MxPlugins;

APP_ERROR MxpiDataSerialize::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    LogInfo << "Begin to initialize MxpiDataSerialize(" << pluginName_ << ").";
    std::vector<std::string> parameterNamesPtr = {"outputDataKeys", "eraseHeaderVecFlag"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    auto key = std::static_pointer_cast<std::string>(configParamMap["outputDataKeys"]);
    inputKeys_ = MxBase::StringUtils::Split(*key, ',');
    if (MxBase::StringUtils::HasInvalidChar(*key)) {
        LogError << "Key has invalid char." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    } else {
        LogInfo << "outputDataKeys(" << *key << ").";
    }
    isEraseHeaderVecInfo_ = *std::static_pointer_cast<bool>(configParamMap["eraseHeaderVecFlag"]);
    LogInfo << "End to initialize MxpiDataSerialize(" << pluginName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiDataSerialize::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiDataSerialize(" << pluginName_ << ").";
    LogInfo << "End to deinitialize MxpiDataSerialize(" << pluginName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiDataSerialize::Process(std::vector<MxpiBuffer*>& mxpiBuffer)
{
    LogDebug << "Begin to process MxpiDataSerialize(" << pluginName_ << ").";
    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        if (mxpiBuffer[i] == nullptr) {
            continue;
        }
        MxpiBuffer *newBuffer = DoSerialize(*mxpiBuffer[i]);
        if (newBuffer == nullptr) {
            LogError << "The newBuffer is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            for (size_t j = i; j < mxpiBuffer.size(); j++) {
                MxpiBufferManager::DestroyBuffer(mxpiBuffer[j]);
            }
            return APP_ERR_COMM_INVALID_POINTER;
        }
        SendData(0, *newBuffer);
        MxpiBufferManager::DestroyBuffer(mxpiBuffer[i]);
    }
    LogDebug << "End to process MxpiDataSerialize(" << pluginName_ << ").";
    return APP_ERR_OK;
}

static MxpiBuffer* GetResultBuffer(const std::string& result)
{
    InputParam inputParam;
    inputParam.dataSize = static_cast<int>(result.size());
    inputParam.ptrData = (void *)result.c_str();
    return MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
}

MxpiBuffer* MxpiDataSerialize::DoSerialize(MxpiBuffer& buffer)
{
    MxpiMetadataManager mxpiMetadataManager(buffer);
    auto mxpiMetadataGraph = mxpiMetadataManager.GetMetadataGraphInstance();
    if (mxpiMetadataGraph == nullptr) {
        LogError << "Get metadata graph instance failed." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return nullptr;
    }
    APP_ERROR ret = APP_ERR_OK;
    std::string result;
    auto errorInfoPtr = mxpiMetadataManager.GetErrorInfo();
    if (errorInfoPtr != nullptr) {
        LogDebug << "Input data is invalid, element(" << elementName_ <<") plugin will not be executed rightly.";
        auto errorInfo = *(std::static_pointer_cast<std::map<std::string, MxpiErrorInfo>>(errorInfoPtr));
        for (auto it = errorInfo.begin(); it != errorInfo.end(); it++) {
            result = (it->second).errorInfo + "\n";
            ret = (it->second).ret;
        }
        MxpiBuffer* resultBuffer = GetResultBuffer(result);
        if (resultBuffer == nullptr) {
            return nullptr;
        }
        AddErrorInfoMetadata(*resultBuffer, result, ret);
        AddExternalInfoMetadata(buffer, *resultBuffer);
        return resultBuffer;
    }

    mxpiMetadataGraph->SetEraseHeaderVecFlag(isEraseHeaderVecInfo_);
    if (!inputKeys_.empty()) {
        if (inputKeys_.size() == 1) {
            result = mxpiMetadataGraph->GetJsonStringFromNodelist(inputKeys_[0]);
        } else {
            mxpiMetadataGraph->MarkAllNodesAsInvalid();
            for (auto key : inputKeys_) {
                mxpiMetadataGraph->MarkNodeListAsValid(key);
            }
            result = mxpiMetadataGraph->GetJsonString();
        }
    } else {
        result = mxpiMetadataGraph->GetJsonString();
    }
    if (result == "null") {
        result = "{}";
    }
    LogDebug << "element(" << elementName_ << ") Serialize result(" << result << ").";
    MxpiBuffer* resultBuffer = GetResultBuffer(result);
    if (resultBuffer == nullptr) {
        return nullptr;
    }
    AddExternalInfoMetadata(buffer, *resultBuffer);
    mxpiMetadataManager.CopyMetadata(*resultBuffer);
    return resultBuffer;
}

APP_ERROR MxpiDataSerialize::AddErrorInfoMetadata(MxTools::MxpiBuffer& mxpiBuffer, std::string errorInfo,
    const APP_ERROR& errorCode)
{
    MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
    MxpiErrorInfo mxpiErrorInfo;
    mxpiErrorInfo.errorInfo = errorInfo;
    mxpiErrorInfo.ret = errorCode;
    auto ret = mxpiMetadataManager.AddErrorInfo(pluginName_, mxpiErrorInfo);
    return ret;
}

void MxpiDataSerialize::AddExternalInfoMetadata(MxTools::MxpiBuffer& mxpiBufferOld, MxTools::MxpiBuffer& mxpiBufferNew)
{
    MxpiMetadataManager mxpiMetadataManager(mxpiBufferOld);
    MxpiMetadataManager mxpiMetadataManagerNew(mxpiBufferNew);
    auto metadata = mxpiMetadataManager.GetMetadata("MxstFrameExternalInfo");
    if (metadata) {
        LogDebug << "Found metadata(MxstFrameExternalInfo), move to new buffer.";
        mxpiMetadataManagerNew.AddMetadata("MxstFrameExternalInfo", metadata);
    }
}

std::vector<std::shared_ptr<void>> MxpiDataSerialize::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto keyElementProperty = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "outputDataKeys", "keys", "input the keys you want, split by ','", "", "", ""
    });
    auto headerVecElementProperty = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
        UINT, "eraseHeaderVecFlag", "HeaderVec", "If headerVec info is not needed, please set it 1", 1, 0, 1
    });

    properties = { keyElementProperty, headerVecElementProperty };
    return properties;
}

MxpiPortInfo MxpiDataSerialize::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxpiPortInfo MxpiDataSerialize::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"result/json-result"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

namespace {
    MX_PLUGIN_GENERATE(MxpiDataSerialize)
}
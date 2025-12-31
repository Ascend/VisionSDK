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
 * Description: Used to filter duplicate targets in the overlapping area after partitioning and adjust ports.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiNmsOverlapedRoi/MxpiNmsOverlapedRoiV2/MxpiNmsOverlapedRoiV2.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

APP_ERROR MxpiNmsOverlapedRoiV2::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    LogInfo << "Begin to process (" << elementName_ << ") init.";
    std::vector<std::string> parameterNamesPtr = {"dataSourceDetection", "dataSourceBlock", "nmsThreshold"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    // object that was detected
    previousPluginName_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceDetection"]);
    previousPluginName_ = MxPluginsAutoDataSource(elementName_, 1, "dataSourceDetection",
        previousPluginName_, dataSourceKeys_);
    if (previousPluginName_.empty()) {
        LogError << "Property dataSourceDetection is \"\",  please check element(" << elementName_
                 << ")'s previous element." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    } else {
        LogInfo << "element(" << elementName_ << ") property dataSourceDetection(" << previousPluginName_ << ").";
    }
    // block that was divided
    blockPluginName_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceBlock"]);
    blockPluginName_ = MxPluginsAutoDataSource(elementName_, 0, "dataSourceBlock",
                                               blockPluginName_, dataSourceKeys_);
    if (blockPluginName_.empty()) {
        LogError << "Property dataSourceBlock is \"\", please check element(" << elementName_
                 << ")'s previous element." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    } else {
        LogInfo << "element(" << elementName_ << ") property dataSourceBlock(" << blockPluginName_ << ").";
    }
    // nms threshold
    nmsValue_ = *std::static_pointer_cast<float>(configParamMap["nmsThreshold"]);
    LogInfo << "element(" << elementName_ << ") property nmsThreshold(" << nmsValue_ << ").";

    // status must be SYNC
    if (status_ != MxTools::SYNC) {
        LogInfo << "element(" << elementName_
                << ") status must be sync(1), you set status async(0), so force status to sync(1).";
        status_ = MxTools::SYNC;
    }

    LogInfo << "End to process (" << elementName_ << ") init.";
    return APP_ERR_OK;
}

APP_ERROR MxpiNmsOverlapedRoiV2::Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer)
{
    LogDebug << "Begin to process (" << elementName_ << ").";
    errorInfo_.str("");
    // check error information
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK || !IsHadDynamicPad(mxpiBuffer)) {
        return APP_ERR_COMM_INVALID_PARAM;
    }

    ret = ErrorInfoProcess(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return APP_ERR_OK;
    }
    // check data source
    ret = CheckDataSource(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return APP_ERR_OK;
    }
    // nms process
    ret = GetBlockDataInfo(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(mxpiBuffer, ret);
        return APP_ERR_OK;
    }
    LogDebug << "End to process  (" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiNmsOverlapedRoiV2::ErrorInfoProcess(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    MxpiMetadataManager mxpiMetadataManagerPort0(*mxpiBuffer[0]);
    MxpiMetadataManager mxpiMetadataManagerPort1(*mxpiBuffer[1]);
    // mxpibuffer from input port zero has error information
    if (mxpiMetadataManagerPort0.GetErrorInfo() != nullptr && mxpiMetadataManagerPort1.GetErrorInfo() == nullptr) {
        LogDebug << "Input data from input port 0 is invalid, element(" << elementName_
                 << ") plugin will not be executed rightly.";
        MxpiBufferManager::DestroyBuffer(mxpiBuffer[1]);
        SendData(0, *mxpiBuffer[0]);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    // mxpibuffer from input port one has error information
    if (mxpiMetadataManagerPort0.GetErrorInfo() == nullptr && mxpiMetadataManagerPort1.GetErrorInfo() != nullptr) {
        LogDebug << "Input data from input port 1 is invalid, element(" << elementName_
                 << ") plugin will not be executed rightly.";
        MxpiBufferManager::DestroyBuffer(mxpiBuffer[0]);
        SendData(0, *mxpiBuffer[1]);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    // mxpibuffers from input port zero and one have error information
    if (mxpiMetadataManagerPort0.GetErrorInfo() != nullptr && mxpiMetadataManagerPort1.GetErrorInfo() != nullptr) {
        LogDebug << "Input data from input port 0 and 1 are invalid, element(" << elementName_
                 << ") plugin will not be executed rightly.";
        MxpiBufferManager::DestroyBuffer(mxpiBuffer[1]);
        SendData(0, *mxpiBuffer[0]);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiNmsOverlapedRoiV2::CheckDataSource(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    MxpiMetadataManager mxpiMetadataManagerPort0(*mxpiBuffer[0]);
    MxpiMetadataManager mxpiMetadataManagerPort1(*mxpiBuffer[1]);
    if (mxpiMetadataManagerPort0.GetMetadata(blockPluginName_) == nullptr ||
        mxpiMetadataManagerPort1.GetMetadata(previousPluginName_) == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << " metadata is null.";
        MxpiBufferManager::DestroyBuffer(mxpiBuffer[1]);
        SendData(0, *mxpiBuffer[0]);
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiNmsOverlapedRoiV2::GetBlockDataInfo(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer)
{
    // get block information
    MxpiMetadataManager mxpiMetadataManagerPort0(*mxpiBuffer[0]);
    std::shared_ptr<void> blockMetaDataPtr = mxpiMetadataManagerPort0.GetMetadata(blockPluginName_);
    if (blockMetaDataPtr == nullptr) {
        LogError << "MxpiNmsOverlapedRoiV2: blockMetaDataPtr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    auto foo = (google::protobuf::Message*)blockMetaDataPtr.get();
    auto desc = foo->GetDescriptor();
    if (!desc) {
        errorInfo_ << "Get Metadata descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM; // self define the error code
    }
    if (desc->name() != "MxpiObjectList") {
        errorInfo_ << "Not MxpiObjectList object." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    // get detection information
    MxpiMetadataManager mxpiMetadataManagerPort1(*mxpiBuffer[1]);
    std::shared_ptr<void> detMetaDataPtr = mxpiMetadataManagerPort1.GetMetadata(previousPluginName_);
    if (detMetaDataPtr == nullptr) {
        LogError << "MxpiNmsOverlapedRoiV2: detMetaDataPtr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    foo = (google::protobuf::Message*)detMetaDataPtr.get();
    desc = foo->GetDescriptor();
    if (!desc) {
        errorInfo_ << "Get Metadata descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (desc->name() != "MxpiObjectList") {
        errorInfo_ << "Not MxpiObjectList object." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    auto mxpiObjectList = std::static_pointer_cast<MxTools::MxpiObjectList>(blockMetaDataPtr);
    auto mxpiObjectListPre = std::static_pointer_cast<MxTools::MxpiObjectList>(detMetaDataPtr);
    CompareBlockObject(mxpiObjectList);
    auto metaDataPtr = MemoryHelper::MakeShared<MxTools::MxpiObjectList>();
    if (metaDataPtr == nullptr) {
        LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    APP_ERROR ret = FilterRepeatObject(mxpiObjectListPre, metaDataPtr);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to filter repeat object." << GetErrorInfo(ret);
        return ret;
    }
    mxpiMetadataManagerPort0.AddProtoMetadata(elementName_, metaDataPtr);
    MxpiBufferManager::DestroyBuffer(mxpiBuffer[1]);
    SendData(0, *mxpiBuffer[0]);
    return APP_ERR_OK;
}

APP_ERROR MxpiNmsOverlapedRoiV2::SendMxpiErrorInfo(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, APP_ERROR errorCode)
{
    MxpiErrorInfo mxpiErrorInfo;
    mxpiErrorInfo.ret = errorCode;
    mxpiErrorInfo.errorInfo = errorInfo_.str();

    MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);
    auto result = mxpiMetadataManager.AddErrorInfo(elementName_, mxpiErrorInfo);
    if (result != APP_ERR_OK) {
        LogError << "Failed to AddErrorInfo." << GetErrorInfo(result);
        return result;
    }
    MxpiBufferManager::DestroyBuffer(mxpiBuffer[1]);
    result = SendData(0, *mxpiBuffer[0]);
    return result;
}

std::vector<std::shared_ptr<void>> MxpiNmsOverlapedRoiV2::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto objectName = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceDetection", "name", "the key for object that was detected by model", "auto", "", ""
    });
    auto blockName = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceBlock", "name", "the key of block that was divided by man", "auto", "", ""
    });
    auto nmsValue = std::make_shared<ElementProperty<float>>(ElementProperty<float> {
        FLOAT, "nmsThreshold", "nms", "the threshold of nms", 0.45f, 0.f, 1.f
    });

    properties.push_back(objectName);
    properties.push_back(blockName);
    properties.push_back(nmsValue);
    return properties;
}

MxTools::MxpiPortInfo MxpiNmsOverlapedRoiV2::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> inputCaps = {{"metadata/object"}, {"metadata/object"}};
    GenerateStaticInputPortsInfo(inputCaps, inputPortInfo);
    return inputPortInfo;
}

MxTools::MxpiPortInfo MxpiNmsOverlapedRoiV2::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> outputCaps = {{"metadata/object"}};
    GenerateStaticOutputPortsInfo(outputCaps, outputPortInfo);
    return outputPortInfo;
}

namespace {
    MX_PLUGIN_GENERATE(MxpiNmsOverlapedRoiV2)
}
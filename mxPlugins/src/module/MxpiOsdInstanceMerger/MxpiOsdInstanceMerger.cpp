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
 * Description: Summarize drawing units from multiple input ports.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiOsdInstanceMerger/MxpiOsdInstanceMerger.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxTools;
using namespace MxBase;
using namespace MxPlugins;
namespace {
const int OP_INPUT_DATA_IDX = 0;
}

APP_ERROR MxpiOsdInstanceMerger::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    status_ = SYNC;
    LogInfo << "Begin to initialize MxpiOsdInstanceMerger(" << elementName_ << ").";
    // get parameters from configParamMap.
    APP_ERROR ret = APP_ERR_OK;
    if (sinkPadNum_ < 1) {
        ret = APP_ERR_STREAM_INVALID_LINK;
        LogError << "MxpiOsdInstanceMerger requires at least 1 inputs." <<  GetErrorInfo(ret);
        return ret;
    }
    ret = InitConfig(configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Init configs failed." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiOsdInstanceMerger(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiOsdInstanceMerger::InitConfig(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    std::vector<std::string> parameterNamesPtr = {"dataSourceList"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    std::string dataSourceListString_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceList"]);
    if (dataSourceListString_ == "auto") {
        dataSourceList_ = dataSourceKeys_;
    } else {
        dataSourceList_ = StringUtils::SplitWithRemoveBlank(dataSourceListString_, ',');
        if (dataSourceList_.size() != sinkPadNum_) {
            LogError << "DataSourceList_ of size" << dataSourceList_.size() << "is not equal to sinkPadNum_("
                     << sinkPadNum_ << ")." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiOsdInstanceMerger::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiOsdinstanceMerger(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiOsdinstanceMerger(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiOsdInstanceMerger::CheckMetaData(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
    std::vector<MxTools::MxpiOsdInstancesList> &validOsdInstancesLists)
{
    // Check metadata empty.
    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[i]);
        if (mxpiMetadataManager.GetErrorInfo() != nullptr) {
            LogWarn << GetErrorInfo(APP_ERR_COMM_FAILURE, elementName_) << "Input Port(" << i
                << ") has errorInfos, element(" << elementName_ << ") will not be executed.";
            continue;
        }
        // Check metadata empty.
        auto foo = mxpiMetadataManager.GetMetadata(dataSourceList_[i]);
        if (foo == nullptr) {
            LogDebug << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL, elementName_)
                << "metadata is empty in input port(" << i << ").";
            continue;
        }
        // Check metadata type.
        auto osdInstancesList = std::static_pointer_cast<MxTools::MxpiOsdInstancesList>(
                mxpiMetadataManager.GetMetadataWithType(dataSourceList_[i], "MxpiOsdInstancesList"));
        if ((osdInstancesList == nullptr) || (osdInstancesList->osdinstancesvec_size() == 0)) {
            LogDebug << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL, elementName_)
                << "metadata is empty in input port(" << i << ").";
            continue;
        }

        validOsdInstancesLists.push_back(*osdInstancesList.get());
    }
    if (validOsdInstancesLists.empty()) {
        LogDebug << "empty validOsdInstancesLists. plugin(" << elementName_ << ") will not be executed.";
        return APP_ERR_COMM_NO_EXIST;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiOsdInstanceMerger::Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiOsdinstanceMerger(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    std::vector<MxTools::MxpiOsdInstancesList> validOsdInstancesLists = {};
    ret = CheckMetaData(mxpiBuffer, validOsdInstancesLists);
    if (ret != APP_ERR_OK) {
        LogDebug << GetErrorInfo(ret, elementName_) << errorInfo_.str();
        for (size_t i = 1; i < mxpiBuffer.size(); i++) {
            MxpiBufferManager::DestroyBuffer(mxpiBuffer[i]);
        }
        SendData(0, *mxpiBuffer[OP_INPUT_DATA_IDX]);
        return ret;
    }

    MxpiOsdInstancesList outputOsdInstancesList;
    ret = CoreProcess(validOsdInstancesLists, outputOsdInstancesList);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        for (size_t i = 1; i < mxpiBuffer.size(); i++) {
            MxpiBufferManager::DestroyBuffer(mxpiBuffer[i]);
        }
        SendMxpiErrorInfo(*mxpiBuffer[OP_INPUT_DATA_IDX], elementName_, ret, errorInfo_.str());
        return ret;
    }

    MxTools::MxpiBuffer *osdBuffer = mxpiBuffer[OP_INPUT_DATA_IDX];
    MxTools::MxpiMetadataManager osdManager(*osdBuffer);
    auto instance = MemoryHelper::MakeShared<MxpiOsdInstancesList>(outputOsdInstancesList);
    if (instance == nullptr) {
        errorInfo_ << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        LogError << errorInfo_.str();
        for (size_t i = 1; i < mxpiBuffer.size(); i++) {
            MxpiBufferManager::DestroyBuffer(mxpiBuffer[i]);
        }
        SendMxpiErrorInfo(*mxpiBuffer[OP_INPUT_DATA_IDX], elementName_, APP_ERR_COMM_ALLOC_MEM, errorInfo_.str());
        return APP_ERR_COMM_ALLOC_MEM;
    }
    ret = osdManager.AddProtoMetadata(elementName_, instance);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add proto metadata failed in Process." << GetErrorInfo(ret);
        for (size_t i = 1; i < mxpiBuffer.size(); i++) {
            MxpiBufferManager::DestroyBuffer(mxpiBuffer[i]);
        }
        SendMxpiErrorInfo(*mxpiBuffer[OP_INPUT_DATA_IDX], elementName_, ret, errorInfo_.str());
        return ret;
    }
    SendData(0, *osdBuffer);
    for (size_t i = 1; i < mxpiBuffer.size(); i++) {
        MxpiBufferManager::DestroyBuffer(mxpiBuffer[i]);
    }
    LogDebug << "End to process MxpiOsdinstanceMerger(" << elementName_ << ").";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiOsdInstanceMerger::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto dataSourceList = (std::make_shared<ElementProperty<std::string>>)(ElementProperty<std::string> {
            STRING, "dataSourceList", "dataSourceOsd", "dataSource of OSD instances strings", "auto", "", ""
    });
    properties.push_back(dataSourceList);
    return properties;
}

MxTools::MxpiPortInfo MxpiOsdInstanceMerger::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = { { "ANY" } };
    GenerateDynamicInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxTools::MxpiPortInfo MxpiOsdInstanceMerger::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/osd"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

APP_ERROR MxpiOsdInstanceMerger::CoreProcess(std::vector<MxTools::MxpiOsdInstancesList> &validOsdInstancesLists,
                                             MxTools::MxpiOsdInstancesList &outputOsdInstancesList)
{
    APP_ERROR ret = APP_ERR_OK;
    for (size_t i = 1; i < validOsdInstancesLists.size(); i++) {
        if (validOsdInstancesLists[i].osdinstancesvec_size() != validOsdInstancesLists[0].osdinstancesvec_size()) {
            ret = APP_ERR_COMM_FAILURE;
            errorInfo_ << "The lengths of osdInstancesList must be the same." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
    }

    for (int i = 0; i < validOsdInstancesLists[0].osdinstancesvec_size(); i++) {
        auto osdDest = outputOsdInstancesList.add_osdinstancesvec();
        if (CheckPtrIsNullptr(osdDest, "osdDest"))  return APP_ERR_COMM_ALLOC_MEM;
        for (auto osdSrc : validOsdInstancesLists) {
            osdDest->mutable_osdrectvec()->MergeFrom(osdSrc.osdinstancesvec(i).osdrectvec());
            osdDest->mutable_osdtextvec()->MergeFrom(osdSrc.osdinstancesvec(i).osdtextvec());
            osdDest->mutable_osdcirclevec()->MergeFrom(osdSrc.osdinstancesvec(i).osdcirclevec());
            osdDest->mutable_osdlinevec()->MergeFrom(osdSrc.osdinstancesvec(i).osdlinevec());
        }
    }
    return APP_ERR_OK;
}

namespace {
MX_PLUGIN_GENERATE(MxpiOsdInstanceMerger)
}
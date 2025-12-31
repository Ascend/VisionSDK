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
 * Description: Input data from multiple ports is output sequentially through one port.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiParallel2Serial/MxpiParallel2Serial.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const std::string PROPERTY_DEFAULT_VALUE = "";
const char SPLIT_RULE = ',';
}

MxpiParallel2Serial::MxpiParallel2Serial()
{
    // set useDevice_ false if plugin not use device
    useDevice_ = false;
}

std::string MxpiParallel2Serial::Vec2Str(const std::vector<std::string>& strVec)
{
    std::string strVal = "";
    for (size_t i = 0; i < strVec.size(); i++) {
        if (strVec[i] == "") {
            return "";
        }
        strVal += strVec[i];
        if (i != strVec.size() - 1) {
            strVal += ",";
        }
    }
    return strVal;
}

APP_ERROR MxpiParallel2Serial::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiParallel2Serial(" << elementName_ << ").";
    // get parameters from website.
    parentName_ = dataSource_;
    if (parentName_ == "auto") {
        std::string tmpDataSource = Vec2Str(dataSourceKeys_);
        LogInfo << "element(" << elementName_ << ") not set property dataSource, use default value.";
        parentName_ = tmpDataSource;
    }
    std::vector<std::string> parameterNamesPtr = {"removeParentData"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    removeParentData_ = *std::static_pointer_cast<int>(configParamMap["removeParentData"]);
    // check property
    ret = CheckProperty();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    LogInfo << "End to initialize MxpiParallel2Serial(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiParallel2Serial::CheckProperty()
{
    if (status_ == MxTools::ASYNC) {
        LogInfo << "element(" << elementName_ << ") will work in async(0) mode.";
    } else {
        LogInfo << "element(" << elementName_ << ") will work in sync(1) mode.";
    }
    // num of sink pad can not less than one
    if (sinkPadNum_ == 0) {
        LogError << "The number of input ports is zero, please check your pipeline."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (!parentName_.empty()) {
        LogInfo << "You select special usage, element(" << elementName_ << ") will some add meta data.";
        dataKeyVec_ = StringUtils::SplitWithRemoveBlank(parentName_, SPLIT_RULE);
        if (dataKeyVec_.size() != sinkPadNum_) {
            LogError << "The number(" << sinkPadNum_ << ") of input ports is not equal to the number("
                     << dataKeyVec_.size() << ") of key." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
        }
    } else {
        LogInfo << "You select common usage, element(" << elementName_ << ") will not write/delete any data.";
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiParallel2Serial::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiParallel2Serial(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiParallel2Serial(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiParallel2Serial::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    APP_ERROR ret = APP_ERR_OK;
    if (status_ == SYNC) {
        LogDebug << "Begin to process MxpiParallel2Serial(" << elementName_ << ") in SYNC mode.";
        ret = CheckMxpiBufferIsValid(mxpiBuffer);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        for (size_t i = 0; i < mxpiBuffer.size(); i++) {
            if (parentName_.empty()) {
                CommonUse(*mxpiBuffer[i]);
            } else {
                SpecialUse(*mxpiBuffer[i], i);
            }
        }
        LogDebug << "End to process MxpiParallel2Serial(" << elementName_ << ") in SYNC mode.";
        return APP_ERR_OK;
    }

    LogDebug << "Begin to process MxpiParallel2Serial(" << elementName_ << ").";
    errorInfo_.str("");
    int idx = -1;
    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        if (mxpiBuffer[i] != nullptr) {
            idx = static_cast<int>(i);
            break;
        }
    }
    if (idx == -1) {
        LogError << "No MxpiBuffer received." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    MxpiBuffer* inputMxpiBuffer = mxpiBuffer[idx];
    MxTools::MxpiMetadataManager mxpiMetadataManager(*inputMxpiBuffer);
    // previous error handle
    if (mxpiMetadataManager.GetErrorInfo() != nullptr) {
        LogDebug << "Input data is invalid, element(" << elementName_ <<") plugin will not be executed rightly.";
        SendData(0, *inputMxpiBuffer);
        return APP_ERR_OK;
    }
    if (parentName_.empty()) {
        ret = CommonUse(*inputMxpiBuffer);
    } else {
        ret = SpecialUse(*inputMxpiBuffer, idx);
    }
    LogDebug << "End to process MxpiParallel2Serial(" << elementName_ << ").";
    return ret;
}

std::vector<std::shared_ptr<void>> MxpiParallel2Serial::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto removeParentDataProSptr = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "removeParentData", "parentMetaData", "remove the metaData of parent plugin", 0, 0, 1
    });
    properties.push_back(removeParentDataProSptr);
    return properties;
}

MxpiPortInfo MxpiParallel2Serial::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateDynamicInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiParallel2Serial::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

APP_ERROR MxpiParallel2Serial::CommonUse(MxpiBuffer& mxpiBuffer)
{
    return SendData(0, mxpiBuffer);
}

APP_ERROR MxpiParallel2Serial::SpecialUse(MxpiBuffer& mxpiBuffer, const int idx)
{
    APP_ERROR ret = APP_ERR_OK;
    MxTools::MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
    std::shared_ptr<void> metadata = mxpiMetadataManager.GetMetadata(dataKeyVec_[idx]);
    if (metadata == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << "metadata is null."
                 << " We obtain buffer from the " << idx
                 << "th input port, please check the property(parentName) of plugin.";
        return SendData(0, mxpiBuffer);
    }
    ret = mxpiMetadataManager.AddProtoMetadata(elementName_, metadata);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add proto meta data failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return SendMxpiErrorInfo(mxpiBuffer, elementName_, ret, errorInfo_.str());
    }
    if (removeParentData_) { // Remove the metadata of previous buffer
        ret = mxpiMetadataManager.RemoveMetadata(dataKeyVec_[idx]);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Remove metadata failed." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            SendMxpiErrorInfo(mxpiBuffer, elementName_, ret, errorInfo_.str());
            return ret;
        }
    }
    return SendData(0, mxpiBuffer);
}
namespace {
    MX_PLUGIN_GENERATE(MxpiParallel2Serial)
}
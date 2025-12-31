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
 * Description: MxpiSkipFrame.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "MxPlugins/MxpiSkipFrame/MxpiSkipFrame.h"
#include "MxBase/Log/Log.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

APP_ERROR MxpiSkipFrame::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiSkipFrame(" << pluginName_ << ").";
    // get parameters from website.
    configParamMap_ = &configParamMap;
    // init props
    auto ret = InitAndRefreshProps();
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "skip frame nunmber(" << skipFrameNum_ << ").";
    LogInfo << "End to initialize MxpiSkipFrame(" << pluginName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiSkipFrame::InitAndRefreshProps()
{
    std::vector<std::string> parameterNamesPtr = {"frameNum"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, *configParamMap_);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    skipFrameNum_ = *std::static_pointer_cast<uint>((*configParamMap_)["frameNum"]);
    return APP_ERR_OK;
}

APP_ERROR MxpiSkipFrame::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiSkipFrame(" << pluginName_ << ").";
    LogInfo << "End to deinitialize MxpiSkipFrame(" << pluginName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiSkipFrame::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // refresh props
    InitAndRefreshProps();
    // error info process
    MxpiBuffer *inputMxpiBuffer = mxpiBuffer[0];
    MxpiMetadataManager mxpiMetadataManager(*inputMxpiBuffer);
    auto errorInfoPtr = mxpiMetadataManager.GetErrorInfo();
    if (errorInfoPtr != nullptr) {
        LogWarn << "Input data is invalid, element(" << pluginName_ <<") plugin will not be executed rightly.";
        SendData(0, *inputMxpiBuffer);
        return APP_ERR_COMM_FAILURE;
    }
    // skip frame process
    if (skipFrameNum_ == 0) {
        SendData(0, *inputMxpiBuffer);
    } else {
        frameCount_++;
        if ((frameCount_ % (skipFrameNum_ + 1)) == 0) {
            frameCount_ = 0;
            SendData(0, *inputMxpiBuffer);
        } else {
            MxpiBufferManager::DestroyBuffer(inputMxpiBuffer);
        }
    }
    return ret;
}

std::vector<std::shared_ptr<void>> MxpiSkipFrame::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto prop1 = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
            UINT,
            "frameNum",
            "frameNum",
            "the number of skip frame",
            0, 0, 100
    });
    properties.push_back(prop1);
    return properties;
}

MxpiPortInfo MxpiSkipFrame::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiSkipFrame::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}
namespace {
    MX_PLUGIN_GENERATE(MxpiSkipFrame)
}
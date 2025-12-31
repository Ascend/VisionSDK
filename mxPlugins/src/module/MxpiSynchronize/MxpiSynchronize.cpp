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
 * Description: Push data to output ports only after all input ports have data.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/Log/Log.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"
#include "MxPlugins/MxpiSynchronize/MxpiSynchronize.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

APP_ERROR MxpiSynchronize::Init(std::map<std::string, std::shared_ptr<void>> &)
{
    LogInfo << "Begin to initialize MxpiSynchronize(" << elementName_ << ").";
    if (sinkPadNum_ == 0) {
        LogError << "The number of input ports is zero. please check your pipeline."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    // status must be SYNC
    if (status_ != MxTools::SYNC) {
        LogDebug << "element(" << elementName_
                 << ") status must be sync(1), you set status async(0), so force status to sync(1).";
        status_ = MxTools::SYNC;
    }
    LogInfo << "End to initialize MxpiSynchronize(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiSynchronize::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiSynchronize(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiSynchronize(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiSynchronize::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiSynchronize(" << elementName_ << ").";
    errorInfo_.str("");
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (IsSameBuffer(mxpiBuffer)) {
        ReleaseRedundantBuffer(mxpiBuffer);
        MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);
        if (mxpiMetadataManager.GetErrorInfo() != nullptr) {
            LogDebug << "Input data is invalid, element(" << elementName_ <<") plugin will not be executed rightly.";
            SendData(0, *mxpiBuffer[0]);
            return APP_ERR_OK;
        }
        LogDebug << "element(" << elementName_ << ") synchronize all input success.";
        SendData(0, *mxpiBuffer[0]);
    } else {
        ReleaseRedundantBuffer(mxpiBuffer);
        errorInfo_ << "Buffer is different, please confirm your usage." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, APP_ERR_COMM_FAILURE, errorInfo_.str());
    }
    LogDebug << "End to process MxpiSynchronize(" << elementName_ << ").";
    return ret;
}

MxpiPortInfo MxpiSynchronize::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateDynamicInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiSynchronize::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

bool MxpiSynchronize::IsSameBuffer(std::vector<MxTools::MxpiBuffer*> &inputVec)
{
    for (size_t i = 1; i < inputVec.size(); i++) {
        if (inputVec[i - 1]->buffer != inputVec[i]->buffer) {
            return false;
        }
    }
    return true;
}

void MxpiSynchronize::ReleaseRedundantBuffer(std::vector<MxTools::MxpiBuffer*> &inputVec)
{
    for (size_t i = 1; i < inputVec.size(); i++) {
        MxpiBufferManager::DestroyBuffer(inputVec[i]);
    }
}
namespace {
    MX_PLUGIN_GENERATE(MxpiSynchronize)
}
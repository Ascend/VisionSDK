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
 * Description: Used to post-process the output tensor of semantic segmentation model inference.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiModelPostProcessors/MxpiSemanticSegPostProcessor.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeConverter.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

APP_ERROR MxpiSemanticSegPostProcessor::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    APP_ERROR ret = APP_ERR_OK;
    LogInfo << "Begin to initialize MxpiSemanticSegPostProcessor(" << elementName_ << ").";
    // open post-processing dynamic lib
    ret = MxImagePostProcessorBase::Init(configParamMap);  // Open a base so.
    if (ret != APP_ERR_OK) {
        LogError << "Fail to Init in MxImagePostProcessorBase." << GetErrorInfo(ret);
        return ret;
    }
    if (funcLanguage_ == "python") {
        ret = OpenPostProcessLib(configParamMap, "libdeeplabv3post.so");
        if (ret != APP_ERR_OK) {
            LogError << "OpenPostProcessLib Failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    // InitPostProcessInstance.
    ret = InitPostProcessInstance<GetSemanticSegInstanceFunc>(configParamMap, "GetSemanticSegInstance");
    if (ret != APP_ERR_OK) {
        LogError << "Fail to InitPostProcessInstance." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiSemanticSegPostProcessor(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiSemanticSegPostProcessor::Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiSemanticSegPostProcessor(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);

    ret = MxImagePostProcessorBase::Process(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        if (!errorInfo_.str().empty()) {
            LogError << "Fail to Process in MxImagePostProcessorBase." << GetErrorInfo(ret);
        }
        return ret;
    }

    std::vector<SemanticSegInfo> semanticSegInfos = {};

    ret = std::static_pointer_cast<SemanticSegPostProcessBase>(instance_)->Process(
        tensors_, semanticSegInfos, resizedImageInfos_);
    tensors_.clear();
    resizedImageInfos_.clear();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to Process SemanticSegPostProcessor." << GetErrorInfo(ret);
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        return ret;
    }
    if (!semanticSegInfos.empty()) {
        auto startTime0 = std::chrono::high_resolution_clock::now();
        ret = mxpiMetadataManager.AddProtoMetadata(elementName_, ConstructProtobuf(semanticSegInfos, dataSource_));
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Add proto metadata failed in Process." << GetErrorInfo(ret);
            SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
            return ret;
        }
        auto endTime0 = std::chrono::high_resolution_clock::now();
        double costTime0 = std::chrono::duration<double, std::milli>(endTime0 - startTime0).count();
        LogDebug << "time of ConstructProtobuf in MxpiSemanticSegPostProcessor: " << costTime0 << "ms";
    }
    SendData(0, *mxpiBuffer[0]);
    LogDebug << "End to process MxpiSemanticSegPostProcessor(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiSemanticSegPostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiSemanticSegPostProcessor(" << elementName_ << ").";
    APP_ERROR ret = MxImagePostProcessorBase::DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to DeInit in MxImagePostProcessorBase." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to deinitialize MxpiSemanticSegPostProcessor(" << elementName_ << ").";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiSemanticSegPostProcessor::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties = MxImagePostProcessorBase::DefineProperties();
    return properties;
}

MxpiPortInfo MxpiSemanticSegPostProcessor::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/semanticseg"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

namespace {
MX_PLUGIN_GENERATE(MxpiSemanticSegPostProcessor)
}
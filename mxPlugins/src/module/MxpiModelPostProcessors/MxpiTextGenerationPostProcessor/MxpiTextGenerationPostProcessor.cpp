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
 * Description: The output tensor of text generation model inference is post-processed.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiModelPostProcessors/MxpiTextGenerationPostProcessor.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeConverter.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;
namespace {
bool IsTextsInfosEmpty(std::vector<TextsInfo> textsInfos)
{
    if (textsInfos.size() == 0) {
        return true;
    }
    uint32_t sum = 0;
    for (auto vec : textsInfos) {
        sum += vec.text.size();
    }
    if (sum == 0) {
        return true;
    }
    return false;
}
}

APP_ERROR MxpiTextGenerationPostProcessor::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    APP_ERROR ret = APP_ERR_OK;
    LogInfo << "Begin to initialize MxpiTextGenerationPostProcessor(" << elementName_ << ").";
    // open post-processing dynamic lib
    ret = MxModelPostProcessorBase::Init(configParamMap);  // Open a base so.
    if (ret != APP_ERR_OK) {
        LogError << "Fail to Init in MxModelPostProcessorBase." << GetErrorInfo(ret);
        return ret;
    }
    if (funcLanguage_ == "python") {
        ret = OpenPostProcessLib(configParamMap, "libcrnnpostprocess.so");
        if (ret != APP_ERR_OK) {
            LogError << "OpenPostProcessLib Failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    // InitPostProcessInstance.
    ret = InitPostProcessInstance<GetTextGenerationInstanceFunc>(configParamMap, "GetTextGenerationInstance");
    if (ret != APP_ERR_OK) {
        LogError << "Fail to InitPostProcessInstance." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiTextGenerationPostProcessor(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiTextGenerationPostProcessor::Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiTextGenerationPostProcessor(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);

    ret = MxModelPostProcessorBase::Process(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        if (!errorInfo_.str().empty()) {
            LogError << "Fail to Process in MxModelPostProcessorBase." << GetErrorInfo(ret);
        }
        return ret;
    }

    std::vector<TextsInfo> textsInfo = {};
    ret = std::static_pointer_cast<TextGenerationPostProcessBase>(instance_)->Process(tensors_, textsInfo);
    tensors_.clear();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to Process TextGenerationPostProcessor." << GetErrorInfo(ret);
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        return ret;
    }
    if (!IsTextsInfosEmpty(textsInfo)) {
        ret = mxpiMetadataManager.AddProtoMetadata(elementName_, ConstructProtobuf(textsInfo, dataSource_));
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Add proto metadata failed in Process." << GetErrorInfo(ret);
            SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
            return ret;
        }
    }
    SendData(0, *mxpiBuffer[0]);
    LogDebug << "End to process MxpiTextGenerationPostProcessor(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiTextGenerationPostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiTextGenerationPostProcessor(" << elementName_ << ").";
    APP_ERROR ret = MxModelPostProcessorBase::DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to DeInit in MxModelPostProcessorBase." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to deinitialize MxpiTextGenerationPostProcessor(" << elementName_ << ").";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiTextGenerationPostProcessor::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties = MxModelPostProcessorBase::DefineProperties();
    return properties;
}

MxpiPortInfo MxpiTextGenerationPostProcessor::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/tensor"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiTextGenerationPostProcessor::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/texts"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

namespace {
MX_PLUGIN_GENERATE(MxpiTextGenerationPostProcessor)
}
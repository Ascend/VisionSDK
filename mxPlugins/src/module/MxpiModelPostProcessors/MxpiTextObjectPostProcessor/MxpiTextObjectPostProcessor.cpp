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
 * Description: Postprocessing Output Tensor of Text Object Box Detection Model Inference.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiModelPostProcessors/MxpiTextObjectPostProcessor.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeConverter.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;
namespace {
template<typename T>
bool IsVVectorEmpty(std::vector<std::vector<T>> vvector)
{
    if (vvector.size() == 0) {
        return true;
    }
    uint32_t sum = 0;
    for (auto vec : vvector) {
        sum += vec.size();
    }
    if (sum == 0) {
        return true;
    }
    return false;
}
}

APP_ERROR MxpiTextObjectPostProcessor::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    APP_ERROR ret = APP_ERR_OK;
    LogInfo << "Begin to initialize MxpiTextObjectPostProcessor(" << elementName_ << ").";
    ret = MxImagePostProcessorBase::Init(configParamMap);  // Open a base so.
    if (ret != APP_ERR_OK) {
        LogError << "Fail to Init in MxImagePostProcessorBase." << GetErrorInfo(ret);
        return ret;
    }
    if (funcLanguage_ == "python") {
        ret = OpenPostProcessLib(configParamMap, "libctpnpostprocess.so");
        if (ret != APP_ERR_OK) {
            LogError << "OpenPostProcessLib Failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    // InitPostProcessInstance.
    ret = InitPostProcessInstance<GetTextObjectInstanceFunc>(configParamMap, "GetTextObjectInstance");
    if (ret != APP_ERR_OK) {
        LogError << "Fail to InitPostProcessInstance." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiTextObjectPostProcessor(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiTextObjectPostProcessor::Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiTextObjectPostProcessor(" << elementName_ << ").";
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

    std::vector<std::vector<TextObjectInfo>> textObjectInfos = {};

    ret = std::static_pointer_cast<TextObjectPostProcessBase>(instance_)->Process(
        tensors_, textObjectInfos, resizedImageInfos_);
    tensors_.clear();
    resizedImageInfos_.clear();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to Process TextObjectPostProcessor." << GetErrorInfo(ret);
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        return ret;
    }
    if (!IsVVectorEmpty(textObjectInfos)) {
        ret = mxpiMetadataManager.AddProtoMetadata(elementName_, ConstructProtobuf(textObjectInfos, dataSource_));
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Add proto metadata failed in Process." << GetErrorInfo(ret);
            SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
            return ret;
        }
    }
    SendData(0, *mxpiBuffer[0]);
    LogDebug << "End to process MxpiTextObjectPostProcessor(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiTextObjectPostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiTextObjectPostProcessor(" << elementName_ << ").";
    APP_ERROR ret = MxImagePostProcessorBase::DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to DeInit in MxImagePostProcessorBase." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to deinitialize MxpiTextObjectPostProcessor(" << elementName_ << ").";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiTextObjectPostProcessor::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties = MxImagePostProcessorBase::DefineProperties();
    return properties;
}

MxpiPortInfo MxpiTextObjectPostProcessor::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/tensor"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiTextObjectPostProcessor::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/texts"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

namespace {
MX_PLUGIN_GENERATE(MxpiTextObjectPostProcessor)
}
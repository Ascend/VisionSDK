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
 * Description: Post-processing of the output tensor of classification model inference.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiModelPostProcessors/MxpiClassPostProcessor.h"
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
APP_ERROR MxpiClassPostProcessor::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiClassPostProcessor(" << elementName_ << ").";
    APP_ERROR ret = MxModelPostProcessorBase::Init(configParamMap);  // Open a base so.
    if (ret != APP_ERR_OK) {
        LogError << "Fail to Init in MxModelPostProcessorBase." << GetErrorInfo(ret);
        return ret;
    }
    if (funcLanguage_ == "python") {
        ret = OpenPostProcessLib(configParamMap, "libresnet50postprocess.so");
        if (ret != APP_ERR_OK) {
            LogError << "OpenPostProcessLib Failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    // InitPostProcessInstance.
    ret = InitPostProcessInstance<GetClassInstanceFunc>(configParamMap, "GetClassInstance");
    if (ret != APP_ERR_OK) {
        LogError << "Fail to InitPostProcessInstance." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxpiClassPostProcessor(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiClassPostProcessor::Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiClassPostProcessor(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);
    std::vector<std::vector<ClassInfo>> classInfos = {};
    ret = MxModelPostProcessorBase::Process(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        if (!errorInfo_.str().empty()) {
            LogError << "Fail to Process in MxModelPostProcessorBase." << GetErrorInfo(ret);
        }
        return ret;
    }
    ret = std::static_pointer_cast<ClassPostProcessBase>(instance_)->Process(tensors_, classInfos);
    tensors_.clear();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to Process ClassPostProcessor." << GetErrorInfo(ret);
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        return ret;
    }
    if (!IsVVectorEmpty(classInfos)) {
        ret = mxpiMetadataManager.AddProtoMetadata(elementName_, ConstructProtobuf(classInfos, dataSource_));
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Add proto metadata failed in Process." << GetErrorInfo(ret);
            SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
            return ret;
        }
    }
    SendData(0, *mxpiBuffer[0]);
    LogDebug << "End to process MxpiClassPostProcessor(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiClassPostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiClassPostProcessor(" << elementName_ << ").";
    APP_ERROR ret = MxModelPostProcessorBase::DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to DeInit in MxModelPostProcessorBase." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to deinitialize MxModelPostProcessorBase(" << elementName_ << ").";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiClassPostProcessor::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties = MxModelPostProcessorBase::DefineProperties();
    return properties;
}

MxpiPortInfo MxpiClassPostProcessor::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/tensor"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiClassPostProcessor::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/class"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

namespace {
MX_PLUGIN_GENERATE(MxpiClassPostProcessor)
}
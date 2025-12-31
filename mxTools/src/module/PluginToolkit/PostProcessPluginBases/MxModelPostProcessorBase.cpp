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
 * Description: Interface of the base class of the model post-processing plug-in.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxTools/PluginToolkit/PostProcessPluginBases/MxModelPostProcessorBase.h"
#include <iomanip>
#include <dlfcn.h>
#include "MxBase/Utils/FileUtils.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"

using namespace MxBase;
using namespace MxTools;

namespace {
bool IsDataContinuous(std::shared_ptr<MxTools::MxpiTensorPackageList>& pTensors, int index)
{
    if (pTensors->tensorpackagevec_size() == 0) {
        LogError << "protobuf message vector is empty." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return false;
    }
    auto lastPtrEndPosition = (uint8_t *) pTensors->tensorpackagevec(0).tensorvec(index).tensordataptr();
    for (int i = 0; i < pTensors->tensorpackagevec_size(); ++i) {
        if (pTensors->tensorpackagevec(i).tensorvec_size() <= index) {
            LogError << "protobuf message vector is invalid." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
            return false;
        }
        auto dataPtr = (uint8_t*)pTensors->tensorpackagevec(i).tensorvec(index).tensordataptr();
        LogDebug << "lastPtrEndPosition: " << (uint32_t*)lastPtrEndPosition
                 << ". nowDataPtr: " << (uint32_t*)dataPtr;
        if (lastPtrEndPosition != dataPtr) {
            return false;
        }
        lastPtrEndPosition += (uint32_t) pTensors->tensorpackagevec(i).tensorvec(index).tensordatasize();
    }
    return true;
}

TensorBase GetContinuousTensors(std::shared_ptr<MxTools::MxpiTensorPackageList>& pTensors, int index)
{
    TensorBase fakeTensor {};
    if (pTensors->tensorpackagevec_size() == 0) {
        LogError << "protobuf message vector is empty." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return fakeTensor;
    }
    if (pTensors->tensorpackagevec(0).tensorvec_size() <= index) {
        LogError << "protobuf message vector is invalid." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return fakeTensor;
    }
    MemoryData memoryData = {};
    memoryData.deviceId = (int32_t)pTensors->tensorpackagevec(0).tensorvec(index).deviceid();
    memoryData.type = (MemoryData::MemoryType)pTensors->tensorpackagevec(0).tensorvec(index).memtype();
    memoryData.size = (size_t) (pTensors->tensorpackagevec(0).tensorvec(index).tensordatasize()
                           * pTensors->tensorpackagevec_size());
    memoryData.ptrData = (void *) pTensors->tensorpackagevec(0).tensorvec(index).tensordataptr();
    std::vector<uint32_t> outputShape = {};
    outputShape.push_back(pTensors->tensorpackagevec_size());
    for (int k = 1; k < pTensors->tensorpackagevec(0).tensorvec(index).tensorshape_size(); ++k) {
        outputShape.push_back((uint32_t) pTensors->tensorpackagevec(0).tensorvec(index).tensorshape(k));
    }
    TensorBase resultTensor {
        memoryData, true, outputShape, (TensorDataType)pTensors->tensorpackagevec(0).tensorvec(index).tensordatatype()
    };
    return resultTensor;
}

std::vector<TensorBase> GetSeperatedTensors(std::shared_ptr<MxTools::MxpiTensorPackageList>& pTensors, int index)
{
    std::vector<TensorBase> tmpTensors;
    for (int i = 0; i < pTensors->tensorpackagevec_size(); ++i) {
        if (pTensors->tensorpackagevec(i).tensorvec_size() <= index) {
            LogError << "protobuf message vector is invalid." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
            return {};
        }
        MemoryData memoryData = {};
        memoryData.deviceId = (int32_t)pTensors->tensorpackagevec(i).tensorvec(index).deviceid();
        memoryData.type = (MemoryData::MemoryType)pTensors->tensorpackagevec(i).tensorvec(index).memtype();
        memoryData.size = (uint32_t) pTensors->tensorpackagevec(i).tensorvec(index).tensordatasize();
        memoryData.ptrData = (void *) pTensors->tensorpackagevec(i).tensorvec(index).tensordataptr();
        std::vector<uint32_t> outputShape = {};
        for (int k = 0; k < pTensors->tensorpackagevec(i).tensorvec(index).tensorshape_size(); ++k) {
            outputShape.push_back((uint32_t) pTensors->tensorpackagevec(i).tensorvec(index).tensorshape(k));
        }
        TensorBase tmpTensor(memoryData, true, outputShape,
                             (TensorDataType)pTensors->tensorpackagevec(0).tensorvec(index).tensordatatype());
        tmpTensors.push_back(tmpTensor);
    }
    return tmpTensors;
}

bool CheckConfigParamMap(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    std::vector<std::string> keys = {
        "postProcessConfigPath", "postProcessConfigContent", "labelPath",
        "postProcessLibPath", "funcLanguage", "className", "pythonModule"
    };
    for (auto key : keys) {
        if (configParamMap.find(key) == configParamMap.end()) {
            LogError << "Property(" << key << ") not exists in plugin!" << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
            return false;
        }
        if (configParamMap[key] == nullptr) {
            LogError << "Property(" << key << " is nullptr in plugin!" << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
            return false;
        }
    }
    for (auto iter = configParamMap.begin(); iter !=  configParamMap.end(); iter++) {
        if (iter->second == nullptr) {
            LogError << "ConfigParamMap contains nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return false;
        }
    }
    return true;
}
} // namespace

APP_ERROR MxModelPostProcessorBase::InitConfig(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    if (!CheckConfigParamMap(configParamMap)) {
        LogError << "This interface is not allowed to use in this way!"
                 << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    if (configParamMap["postProcessConfigPath"] != nullptr) {
        configPath_ = *std::static_pointer_cast<std::string>(configParamMap.find("postProcessConfigPath")->second);
        postConfigParamMap_["postProcessConfigPath"] = configPath_;
    }
    if (configParamMap["postProcessConfigContent"] != nullptr) {
        postConfigParamMap_["postProcessConfigContent"] =
            *std::static_pointer_cast<std::string>(configParamMap["postProcessConfigContent"]);
    }
    if (configParamMap["labelPath"] != nullptr) {
        labelPath_ = *std::static_pointer_cast<std::string>(configParamMap["labelPath"]);
        postConfigParamMap_["labelPath"] = labelPath_;
    }
    if (funcLanguage_ == "c++") {
        // Open a base modelpostprocessor so.
        APP_ERROR ret = OpenPostProcessLib(configParamMap);
        if (ret != APP_ERR_OK) {
            LogError << "OpenPostProcessLib Failed." << GetErrorInfo(ret);
            return ret;
        }
    } else if (funcLanguage_ == "python") {
        if (configParamMap["className"] == nullptr || configParamMap["pythonModule"] == nullptr ||
            configParamMap["postProcessLibPath"] == nullptr) {
            LogError << "One of [className, pythonModule, postProcessLibPath] is nullptr, please check."
                     << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
        }
        className_ = *std::static_pointer_cast<std::string>(configParamMap["className"]);
        pythonModule_ = *std::static_pointer_cast<std::string>(configParamMap["pythonModule"]);
        if (pythonModule_.empty() || className_.empty()) {
            LogError << "One of [className, pythonModule] is nullptr, please check."
                     << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
        }
        postProcessLibPath_ = *std::static_pointer_cast<std::string>(configParamMap["postProcessLibPath"]);
    }
    return APP_ERR_OK;
}

APP_ERROR MxModelPostProcessorBase::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxModelPostProcessorPluginBase.";
    doPreErrorCheck_ = true;
    doPreMetaDataCheck_ = true;
    if (!CheckConfigParamMap(configParamMap)) {
        LogError << "This interface is not allowed to use separately!" << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    if (dataSourceKeys_.empty()) {
        LogError << "This interface is not allowed to use separately!" << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    dataSource_ = (dataSource_ == "auto") ? dataSourceKeys_[0] : dataSource_;
    if (dataSource_.empty()) {
        LogError << " Property dataSource is \"\","
                 << " Please check element's previous element." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    dataSourceKeys_ = {dataSource_};
    funcLanguage_ = *std::static_pointer_cast<std::string>(configParamMap["funcLanguage"]);
    if (funcLanguage_ != "c++" && funcLanguage_ != "python") {
        LogError << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = InitConfig(configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "InitProcess failed" << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "End to initialize MxModelPostProcessorPluginBase.";
    return APP_ERR_OK;
}

APP_ERROR MxModelPostProcessorBase::ConstructTensor(std::shared_ptr<MxTools::MxpiTensorPackageList>& pTensors,
                                                    std::vector<MxBase::TensorBase>& tensors)
{
    APP_ERROR ret = APP_ERR_OK;
    LogDebug << "Start to ConstructTensor in postprocess plugin.";
    if (pTensors->tensorpackagevec_size() == 0) {
        LogError << "protobuf message vector is empty." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    for (int j = 0; j < pTensors->tensorpackagevec(0).tensorvec_size(); ++j) {
        if (IsDataContinuous(pTensors, j)) {
            TensorBase resultTensor = GetContinuousTensors(pTensors, j);
            tensors.push_back(resultTensor);
        } else {
            LogDebug << "Tensor data is detected as noncontinuous. BatchVector will be performed.";
            std::vector<TensorBase> tmpTensors = GetSeperatedTensors(pTensors, j);
            TensorBase resultTensor;
            ret = TensorBase::BatchVector(tmpTensors, resultTensor, true);
            if (ret != APP_ERR_OK) {
                errorInfo_ << "Fail to form batched tensors.";
                LogError << errorInfo_.str() << GetErrorInfo(ret);
                return ret;
            }
            tensors.push_back(resultTensor);
        }
    }
    LogDebug << "End to ConstructTensor in postprocess plugin.";
    return APP_ERR_OK;
}

APP_ERROR MxModelPostProcessorBase::Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxModelPostProcessorPluginBase.";
    APP_ERROR ret = APP_ERR_OK;
    errorInfo_.str("");
    if (mxpiBuffer.empty() || mxpiBuffer[0] == nullptr || !mxpiBuffer[0]->buffer) {
        LogError << "This interface is not allowed to use separately!" << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);

    if (mxpiMetadataManager.GetMetadata(dataSource_) == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL)
                 << "Fail to get MxpiTensorPackageList from dataSource.";
        SendData(0, *mxpiBuffer[0]);
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
    }

    auto tensorPackageList = std::static_pointer_cast<MxTools::MxpiTensorPackageList>(
            mxpiMetadataManager.GetMetadataWithType(dataSource_, "MxpiTensorPackageList"));
    if (tensorPackageList == nullptr) {
        errorInfo_ << "Fail to get MxpiTensorPackageList from "
                   << "dataSource.";
        LogError << errorInfo_.str() << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        return APP_ERR_COMM_INVALID_POINTER;
    }

    ret = ConstructTensor(tensorPackageList, tensors_);
    if (ret != APP_ERR_OK) {
        tensors_.clear();
        errorInfo_ << "Fail to ConstructTensor from MxpiTensorPackageList.";
        LogError << errorInfo_.str() << GetErrorInfo(ret) ;
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        return ret;
    }
    LogDebug << "End to process MxModelPostProcessorPluginBase.";
    return APP_ERR_OK;
}

APP_ERROR MxModelPostProcessorBase::DeInit()
{
    LogInfo << "Begin to deinitialize MxModelPostProcessorPluginBase.";
    APP_ERROR ret = APP_ERR_OK;
    if (instance_ != nullptr) {
        try {
            ret = instance_->DeInit();
            if (ret != APP_ERR_OK) {
                LogError << "Fail to deInitialize post process." << GetErrorInfo(ret);
            }
        } catch (const std::exception& e) {
            ret = APP_ERR_COMM_FAILURE;
            LogError << " When calling the Process function in the "
                     << "post-processing so, an exception was thrown." << GetErrorInfo(ret) ;
        }
        // dlclose would release all resource, smart_ptr should reset will dlclose
        instance_.reset();
        dlclose(handle_);
    }
    LogInfo << "End to deinitialize MxModelPostProcessorPluginBase.";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxModelPostProcessorBase::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto configPath = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "postProcessConfigPath", "config", "config path", "", "", ""
    });
    auto configContent = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "postProcessConfigContent", "config", "config content, its type is [json::object]",
            "", "", ""
    });
    auto labelPath = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "labelPath", "label", "label path", "", "", ""
    });
    auto dynamicPath = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "postProcessLibPath", "lib", "post processor lib path", "", "", ""
    });
    auto funcLanguage = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "funcLanguage", "name",
        "the func Language of plugin", "c++", "", ""
    });
    auto className = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "className", "class name",
        "the func class name of post plugin", "", "", ""
    });
    auto pythonModule = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "pythonModule", "python module name",
        "the python module name of post plugin", "", "", ""
    });
    properties.push_back(configPath);
    properties.push_back(configContent);
    properties.push_back(labelPath);
    properties.push_back(dynamicPath);
    properties.push_back(funcLanguage);
    properties.push_back(className);
    properties.push_back(pythonModule);
    return properties;
}

MxpiPortInfo MxModelPostProcessorBase::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/tensor"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxModelPostProcessorBase::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/tensor"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    value = {{"ANY"}};
    GenerateDynamicOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

APP_ERROR MxModelPostProcessorBase::OpenPostProcessLib(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    // Set dynamic lib path
    if (configParamMap.find("postProcessLibPath") == configParamMap.end()) {
        LogError << "Property(postProcessLibPath) not exists in plugin!" << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    APP_ERROR ret = APP_ERR_OK;
    std::string dynamicPath = *std::static_pointer_cast<std::string>(configParamMap["postProcessLibPath"]);
    if (dynamicPath.empty()) {
        ret = APP_ERR_COMM_NO_EXIST;
        LogError << "postProcessLibPath is not set!" << GetErrorInfo(ret);
        return ret;
    }
    if (!CheckPostProcessLibPath(dynamicPath)) {
        return APP_ERR_COMM_NO_EXIST;
    }

    std::string dynamicRegPath;
    if (!FileUtils::RegularFilePath(dynamicPath, dynamicRegPath) || !FileUtils::IsFileValid(dynamicRegPath, true)) {
        LogError << "check file failed, check list: regular, exist, size, owner, permission."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    
    handle_ = dlopen((dynamicRegPath).c_str(), RTLD_LAZY);
    if (handle_ == nullptr) {
        LogError << "Open dynamic lib fail. [" << dlerror() << "]." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR MxModelPostProcessorBase::OpenPostProcessLib(
    std::map<std::string, std::shared_ptr<void>> &configParamMap, const std::string &postProcessDllName)
{
    // Set dynamic lib path
    if (configParamMap.find("postProcessLibPath") == configParamMap.end()) {
        LogError << "Property(postProcessLibPath) not exists in plugin!" << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }
    APP_ERROR ret = APP_ERR_OK;
    std::string dynamicPath = *std::static_pointer_cast<std::string>(configParamMap["postProcessLibPath"]);
    if (dynamicPath.empty()) {
        ret = APP_ERR_COMM_NO_EXIST;
        LogError << "postProcessLibPath is not set!" << GetErrorInfo(ret);
        return ret;
    }
    if (funcLanguage_ == "python" && !MxBase::FileUtils::CheckFileExists(dynamicPath)) {
        auto sdkHome = std::getenv("MX_SDK_HOME");
        if (sdkHome == nullptr) {
            LogError << "Environment Variables [MX_SDK_HOME] is empty." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
            return APP_ERR_COMM_NO_EXIST;
        }
        dynamicPath=std::string(sdkHome) + std::string("/lib/modelpostprocessors/") + postProcessDllName;
    }
    if (!CheckPostProcessLibPath(dynamicPath)) {
        return APP_ERR_COMM_NO_EXIST;
    }
    std::string dynamicRegPath;
    if (!FileUtils::RegularFilePath(dynamicPath, dynamicRegPath) || !FileUtils::IsFileValid(dynamicRegPath, true)) {
        LogError << "check file failed, check list: regular, exist, size, owner, permission."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    handle_ = dlopen((dynamicRegPath).c_str(), RTLD_LAZY);
    if (handle_ == nullptr) {
        LogError << "Open dynamic lib fail. [" << dlerror() << "]." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }
    return APP_ERR_OK;
}

bool MxModelPostProcessorBase::CheckPostProcessLibPath(std::string &filePath)
{
    if (!MxBase::FileUtils::CheckFileExists(filePath)) {
        std::string::size_type pos = filePath.rfind('/');
        if (pos == std::string::npos) {
            LogInfo << "Postprocessing DLL of plugin.";
            auto sdkHome = std::getenv("MX_SDK_HOME");
            if (sdkHome == nullptr) {
                LogError << "Environment Variables [MX_SDK_HOME] is empty." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
                return false;
            }
            filePath = std::string(sdkHome) + std::string("/lib/modelpostprocessors/") + filePath;
            if (MxBase::FileUtils::CheckFileExists(filePath)) {
                return true;
            }
            pos = filePath.rfind('/');
        }
        LogError << "The postprocessing DLL does not exist." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return false;
    }
    return true;
}
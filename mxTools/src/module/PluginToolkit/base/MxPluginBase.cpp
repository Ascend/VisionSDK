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
 * Description: A user-defined plug-in needs to inherit the plug-in base class.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxTools/PluginToolkit/base/MxPluginBase.h"
#include "MxTools/PluginToolkit/base/MxGstBase.h"
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxBase/Log/Log.h"
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"
#include "MxPluginBaseDptr.hpp"

using namespace MxBase;

namespace {
    constexpr int MAX_PLUGIN_NUM = 1024;
    constexpr int MIN_PLUGIN_NUM = 1;
}

namespace MxTools {
std::map<std::string, std::vector<ImageSize>> MxPluginBase::elementDynamicImageSize_;

MxPluginBase::MxPluginBase()
{
    pMxPluginBaseDptr_ = MxBase::MemoryHelper::MakeShared<MxPluginBaseDptr>();
    if (pMxPluginBaseDptr_ == nullptr) {
        LogError << "Create MxPluginBaseDptr object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

MxPluginBase::~MxPluginBase() {}

APP_ERROR MxPluginBase::RunProcess(std::vector<MxpiBuffer*>& mxpiBuffer)
{
    APP_ERROR ret = APP_ERR_OK;
    if (mxpiBuffer.size() < 1) {
        LogError << "invalid mxpiBuffer input. size must not be equal to 0!" << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    errorInfo_.str("");
    if (status_ == ASYNC) {
        ret = AsyncPreProcessCheck(mxpiBuffer);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    } else if (status_ == SYNC) {
        ret = SyncPreProcessCheck(mxpiBuffer);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    try {
        Process(mxpiBuffer);
    } catch (const std::exception& e) {
        errorInfo_ << "An Exception occurred. Error message: (" << e.what() << ").";
        LogError << errorInfo_.str() << GetErrorInfo(APP_ERR_COMM_INNER);
        DestroyExtraBuffers(mxpiBuffer, UINT32_MAX);
        InputParam inputParam = {};
        inputParam.key = "";
        inputParam.deviceId = -1;
        inputParam.dataSize = 1;
        MxpiBuffer* outputMxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);
        if (outputMxpiBuffer == nullptr) {
            LogError << "Create null buffer failed." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        SendMxpiErrorInfo(*outputMxpiBuffer, elementName_, APP_ERR_COMM_INNER, errorInfo_.str());
    }
    return APP_ERR_OK;
}

APP_ERROR MxPluginBase::AsyncPreProcessCheck(std::vector<MxpiBuffer*>& mxpiBuffer)
{
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
    MxTools::MxpiMetadataManager manager(*mxpiBuffer[idx]);
    if (doPreErrorCheck_) {
        if (manager.GetErrorInfo() != nullptr) {
            LogDebug << "Input Port(" << idx << ") has errorInfos, element will not be executed.";
            SendDataToAllPorts(*mxpiBuffer[idx]);
            return APP_ERR_COMM_FAILURE;
        }
    }
    if (doPreMetaDataCheck_) {
        if (manager.GetMetadata(dataSourceKeys_[idx]) == nullptr) {
            LogDebug << "metadata in dataSource is empty, plugin will not be executed.";
            SendDataToAllPorts(*mxpiBuffer[idx]);
            return APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxPluginBase::SyncPreProcessCheck(std::vector<MxpiBuffer*>& mxpiBuffer)
{
    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        if (mxpiBuffer[i] == nullptr) {
            LogError << "The mxpiBuffer contains nullptr, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            for (size_t j = 0; j < mxpiBuffer.size(); j++) {
                MxpiBufferManager::DestroyBuffer(mxpiBuffer[j]);
            }
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    if (doPreErrorCheck_) {
        MxTools::MxpiMetadataManager manager(*mxpiBuffer[0]);
        for (size_t i = 0; i < mxpiBuffer.size(); i++) {
            MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[i]);
            std::shared_ptr<std::map<std::string, MxpiErrorInfo>> errors = mxpiMetadataManager.GetErrorInfo();
            if (errors == nullptr) {
                continue;
            }
            LogDebug << "Input Port(" << i << ") has errorInfos, element will not be executed.";
            if (i == 0) {
                continue;
            }
            for (auto it = errors->begin(); it != errors->end(); it++) {
                manager.AddErrorInfo(it->first, it->second);
            }
        }
        if (manager.GetErrorInfo() != nullptr) {
            SendDataToAllPorts(*mxpiBuffer[0]);
            DestroyExtraBuffers(mxpiBuffer, 0);
            return APP_ERR_COMM_FAILURE;
        }
    }

    if (doPreMetaDataCheck_) {
        MxTools::MxpiMetadataManager manager(*mxpiBuffer[0]);
        if (sinkPadNum_ > dataSourceKeys_.size()) {
            for (size_t j = 0; j < mxpiBuffer.size(); j++) {
                MxpiBufferManager::DestroyBuffer(mxpiBuffer[j]);
            }
            return APP_ERR_COMM_OUT_OF_RANGE;
        }
        for (size_t i = 0; i < sinkPadNum_; i++) {
            MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[i]);
            if (mxpiMetadataManager.GetMetadata(dataSourceKeys_[i]) != nullptr) {
                continue;
            }
            LogDebug << "Metadata in dataSource is empty, plugin will not be executed.";
            for (size_t j = 0; j < sinkPadNum_; j++) {
                mxpiMetadataManager.CopyMetadata(*mxpiBuffer[0]);
            }
            SendDataToAllPorts(*mxpiBuffer[0]);
            DestroyExtraBuffers(mxpiBuffer, 0);
            return APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxPluginBase::SendData(int index, MxpiBuffer& mxpiBuffer)
{
    MxpiBuffer* bufferPtr = &mxpiBuffer;
    if (bufferPtr == nullptr || bufferPtr->buffer == nullptr) {
        delete bufferPtr;
        bufferPtr = nullptr;
        LogError << "Invalid mxpiBuffer." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    auto outBuffer = (GstBuffer*) bufferPtr->buffer;
    delete bufferPtr;
    bufferPtr = nullptr;
    auto filter = (MxGstBase*) (pMxPluginBaseDptr_->elementInstance_);
    if (filter == nullptr) {
        gst_buffer_unref(outBuffer);
        LogError << "Please set element instance before send data." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (filter->srcPadVec.size() > MAX_PAD_NUM) {
        gst_buffer_unref(outBuffer);
        LogError << "The number of output ports has to be smaller than " << MAX_PAD_NUM << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (index >= (int)filter->srcPadVec.size() || index < 0) {
        gst_buffer_unref(outBuffer);
        LogError << "Invalid output prot index(" << index << "). Has to be smaller than "
                 << filter->srcPadVec.size() << ", and lager than or equal to 0."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    bool isExist = IsStreamElementNameExist(reinterpret_cast<uintptr_t>(filter));
    if (isExist) {
        StreamElementName streamElementName = g_streamElementNameMap[reinterpret_cast<uintptr_t>(filter)];
        PerformanceStatisticsManager::GetInstance()->PluginStatisticsSetStartBlockTime(streamElementName.streamName,
                                                                                       streamElementName.elementName);
    }
    auto ret = gst_pad_push(filter->srcPadVec[index], outBuffer);
    if (isExist) {
        StreamElementName streamElementName = g_streamElementNameMap[reinterpret_cast<uintptr_t>(filter)];
        PerformanceStatisticsManager::GetInstance()->PluginStatisticsSetEndBlockTime(streamElementName.streamName,
                                                                                     streamElementName.elementName);
    }
    return ConvertReturnCodeToLocal(GST_FLOW_TYPE, ret);
}

APP_ERROR MxPluginBase::SendDataToAllPorts(MxpiBuffer& mxpiBuffer)
{
    if (mxpiBuffer.buffer == nullptr) {
        LogError << "Invalid mxpiBuffer." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = APP_ERR_OK;
    for (size_t i = 0; i < srcPadNum_; i++) {
        gst_buffer_ref((GstBuffer*) mxpiBuffer.buffer);
        auto *tmpBuffer = new(std::nothrow) MxpiBuffer {mxpiBuffer.buffer, nullptr};
        if (tmpBuffer == nullptr) {
            gst_buffer_unref((GstBuffer*) mxpiBuffer.buffer);
            MxpiBufferManager::DestroyBuffer(&mxpiBuffer);
            LogError << "New MxpiBuffer failed, will not send data to next plugin"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        ret = SendData(i, *tmpBuffer);
    }
    MxpiBufferManager::DestroyBuffer(&mxpiBuffer);
    return ret;
}

void MxPluginBase::GenerateStaticPortsInfo(PortDirection direction,
    const std::vector<std::vector<std::string>>& portsDesc, MxpiPortInfo& portInfo)
{
    int portNum = static_cast<int>(portsDesc.size());
    if (portNum < MIN_PLUGIN_NUM || portNum > MAX_PLUGIN_NUM) {
        LogError << "The portsDesc.size() [" << portNum << "] is invalid, which should be in range [" << MIN_PLUGIN_NUM
                 << ", " << MAX_PLUGIN_NUM << "]." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error("The size of portsDesc is invalid.");
    }
    portInfo.portNum += portNum;
    if (portInfo.portNum < 0 || portInfo.portNum > MAX_PLUGIN_NUM) {
        LogError << "The portInfo.portNum[" << portInfo.portNum << "] is invalid, which should be in range ["
                 << 0 << ", " << MAX_PLUGIN_NUM << "]." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error("The param portInfo.portNum is invalid.");
    }
    portInfo.direction = direction;
    std::vector<PortTypeDesc> portsType(portNum, STATIC);
    portInfo.types.insert(portInfo.types.end(), portsType.begin(), portsType.end());
    portInfo.portDesc.insert(portInfo.portDesc.end(), portsDesc.begin(), portsDesc.end());
}

void MxPluginBase::GenerateStaticInputPortsInfo(const std::vector<std::vector<std::string>>& portsDesc,
    MxpiPortInfo& inputPortInfo)
{
    GenerateStaticPortsInfo(INPUT_PORT, portsDesc, inputPortInfo);
}

void MxPluginBase::GenerateStaticOutputPortsInfo(const std::vector<std::vector<std::string>>& portsDesc,
    MxpiPortInfo& outputPortInfo)
{
    GenerateStaticPortsInfo(OUTPUT_PORT, portsDesc, outputPortInfo);
}

void MxPluginBase::GenerateDynamicPortsInfo(PortDirection direction,
    const std::vector<std::vector<std::string>>& portsDesc, MxpiPortInfo& portInfo)
{
    int portNum = static_cast<int>(portsDesc.size());
    if (portNum < MIN_PLUGIN_NUM || portNum > MAX_PLUGIN_NUM) {
        LogError << "The portsDesc.size() [" << portNum << "] is invalid, which should be in range [" << MIN_PLUGIN_NUM
                 << ", " << MAX_PLUGIN_NUM << "]." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error("The size of portsDesc is invalid.");
    }
    portInfo.portNum += portNum;
    if (portInfo.portNum < 0 || portInfo.portNum > MAX_PLUGIN_NUM) {
        LogError << "The portInfo.portNum[" << portInfo.portNum << "] is invalid, which should be in range ["
                 << 0 << ", " << MAX_PLUGIN_NUM << "]." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        throw std::runtime_error("The param portInfo.portNum is invalid.");
    }
    portInfo.direction = direction;
    std::vector<PortTypeDesc> portsType(portNum, DYNAMICS);
    portInfo.types.insert(portInfo.types.end(), portsType.begin(), portsType.end());
    portInfo.portDesc.insert(portInfo.portDesc.end(), portsDesc.begin(), portsDesc.end());
}

void MxPluginBase::GenerateDynamicInputPortsInfo(const std::vector<std::vector<std::string>>& portsDesc,
    MxpiPortInfo& inputPortInfo)
{
    GenerateDynamicPortsInfo(INPUT_PORT, portsDesc, inputPortInfo);
}

void MxPluginBase::GenerateDynamicOutputPortsInfo(const std::vector<std::vector<std::string>>& portsDesc,
    MxpiPortInfo& outputPortInfo)
{
    GenerateDynamicPortsInfo(OUTPUT_PORT, portsDesc, outputPortInfo);
}

MxpiPortInfo MxPluginBase::DefineInputPorts()
{
    MxpiPortInfo defaultInputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticInputPortsInfo(value, defaultInputPortInfo);

    return defaultInputPortInfo;
}

MxpiPortInfo MxPluginBase::DefineOutputPorts()
{
    MxpiPortInfo defaultOutputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticOutputPortsInfo(value, defaultOutputPortInfo);

    return defaultOutputPortInfo;
}

std::vector<std::shared_ptr<void>> MxPluginBase::DefineProperties()
{
    auto configProPtr = std::make_shared<ElementProperty<std::string>>();
    configProPtr->defaultValue = "";
    configProPtr->name = "config";
    configProPtr->nickName = "config";
    configProPtr->type = STRING;
    configProPtr->desc = "default config param";

    std::vector<std::shared_ptr<void>> properties;
    properties.push_back(std::static_pointer_cast<void>(configProPtr));
    return properties;
}

void MxPluginBase::SetElementInstance(void* elementInstance)
{
    pMxPluginBaseDptr_->elementInstance_ = elementInstance;
}

void MxPluginBase::ConfigParamLock()
{
    pMxPluginBaseDptr_->configParamMutex_.lock();
}

void MxPluginBase::ConfigParamUnlock()
{
    pMxPluginBaseDptr_->configParamMutex_.unlock();
}

std::string MxPluginBase::DoDump(MxTools::MxpiBuffer& mxpiBuffer, const std::vector<std::string>& filterKeys,
                                 const std::vector<std::string>& requiredKeys) const
{
    return MxpiBufferDump::DoDump(mxpiBuffer, filterKeys, requiredKeys);
}

MxTools::MxpiBuffer* MxPluginBase::DoLoad(MxTools::MxpiBuffer& mxpiBuffer)
{
    return MxpiBufferDump::DoLoad(mxpiBuffer, this->deviceId_);
}

APP_ERROR MxPluginBase::SendMxpiErrorInfo(MxpiBuffer& mxpiBuffer, const std::string& pluginName,
    APP_ERROR errorCode, const std::string& errorText)
{
    APP_ERROR ret = APP_ERR_OK;
    MxpiErrorInfo mxpiErrorInfo;
    mxpiErrorInfo.ret = errorCode;
    mxpiErrorInfo.errorInfo = errorText;

    MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
    ret = mxpiMetadataManager.AddErrorInfo(pluginName, mxpiErrorInfo);
    if (ret != APP_ERR_OK) {
        MxpiBufferManager::DestroyBuffer(&mxpiBuffer);
        LogError << "Failed to AddErrorInfo." << GetErrorInfo(ret);
        return ret;
    }
    if (mxpiBuffer.buffer == nullptr) {
        MxpiBufferManager::DestroyBuffer(&mxpiBuffer);
        LogError << "Invalid mxpiBuffer." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < srcPadNum_; i++) {
        gst_buffer_ref((GstBuffer *) mxpiBuffer.buffer);
        auto tmpBuffer = new (std::nothrow) MxpiBuffer {mxpiBuffer.buffer, nullptr};
        if (tmpBuffer == nullptr) {
            LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            MxpiBufferManager::DestroyBuffer(&mxpiBuffer);
            return APP_ERR_COMM_INIT_FAIL;
        }
        ret = SendData(i, *tmpBuffer);
    }
    MxpiBufferManager::DestroyBuffer(&mxpiBuffer);
    return ret;
}

void MxPluginBase::DestroyExtraBuffers(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, size_t exceptPort)
{
    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        if (i == exceptPort)
            continue;
        MxpiBufferManager::DestroyBuffer(mxpiBuffer[i]);
    }
}

APP_ERROR MxPluginBase::SetOutputDataKeys()
{
    outputDataKeys_.clear();
    if (srcPadNum_ == 0) {
        LogDebug << "Element has no output port.";
        return APP_ERR_OK;
    } else if (srcPadNum_ == 1) {
        LogDebug << "Element has one output port.";
        outputDataKeys_.push_back(elementName_);
        return APP_ERR_OK;
    }

    LogDebug << "Element has " << srcPadNum_ << " output ports.";
    for (size_t i = 0; i < srcPadNum_; i++) {
        auto key = elementName_ + "_" + std::to_string(i);
        outputDataKeys_.push_back(key);
    }

    return APP_ERR_OK;
}

std::string MxPluginBase::GetElementNameWithObjectAddr()
{
    return std::to_string(reinterpret_cast<uintptr_t>(this)) + "_" + elementName_;
}
}
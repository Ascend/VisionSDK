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
 * Description: Transfers memory data between devices and hosts..
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiDataTransfer/MxpiDataTransfer.h"
#include "MxBase/Log/Log.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxPlugins;
using namespace MxTools;
using namespace MxBase;

namespace {
enum class MemoryTransferMode {
    MEMORY_TRANSFER_AUTO = 0,
    MEMORY_TRANSFER_D2H,
    MEMORY_TRANSFER_H2D,
};
}

std::string MxpiDataTransfer::GetTransModeDescription(uint transferMode)
{
    switch (transferMode) {
        case (uint)MemoryTransferMode::MEMORY_TRANSFER_AUTO:
            return "auto(transfer to other memory device automatically)";
        case (uint)MemoryTransferMode::MEMORY_TRANSFER_D2H:
            return "device2host";
        case (uint)MemoryTransferMode::MEMORY_TRANSFER_H2D:
            return "host2device";
        default:
            return "undefined";
    }
}

APP_ERROR MxpiDataTransfer::GetTransferMode(std::string& transferModeString)
{
    APP_ERROR ret = APP_ERR_OK;
    if (transferModeString == "auto") {
        transferMode_ = (uint)MemoryTransferMode::MEMORY_TRANSFER_AUTO;
    } else if (transferModeString == "d2h") {
        transferMode_ = (uint)MemoryTransferMode::MEMORY_TRANSFER_D2H;
    } else if (transferModeString == "h2d") {
        transferMode_ = (uint)MemoryTransferMode::MEMORY_TRANSFER_H2D;
    } else {
        LogError << R"(Invalid transfer mode, supports "auto", "d2h", and "h2d" only.)"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    LogDebug << "Element(" << elementName_ << ") is running in " << GetTransModeDescription(transferMode_) << " mode.";

    return ret;
}

APP_ERROR MxpiDataTransfer::IsRemoveSourceData(std::string& removeDataSelection)
{
    APP_ERROR ret = APP_ERR_OK;
    if (removeDataSelection == "yes") {
        removeSourceData_ = true;
        LogDebug << "Element(" << elementName_ << ") will free the memory in the input data.";
    } else if (removeDataSelection == "no") {
        removeSourceData_ = false;
        LogDebug << "Element(" << elementName_ << ") will not free the memory in the input data.";
    } else {
        LogError << R"(Invalid config of removeParentData, supports "yes", "no" only.)"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return ret;
}

APP_ERROR MxpiDataTransfer::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    LogInfo << "Begin to initialize MxpiDataTransfer(" << elementName_ << ").";
    APP_ERROR ret = APP_ERR_OK;
    // Get the property values by key
    dataSource_ = MxPluginsAutoDataSource(elementName_, 0, "dataSource", dataSource_, dataSourceKeys_);
    if (dataSource_.empty()) {
        LogError << "Property dataSource is \"\","
                 << " please check element(" << elementName_ << ")'s previous element."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    } else {
        LogInfo << "The dataSource of element(" << elementName_ << ") is " << dataSource_ << ".";
    }
    std::vector<std::string> parameterNamesPtr = {"transferMode", "removeSourceData"};
    ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    auto transferModeSptr = std::static_pointer_cast<std::string>(configParamMap["transferMode"]);
    ret = GetTransferMode(*transferModeSptr);
    if (ret != APP_ERR_OK) {
        LogError << "Invalid transfer mode." << GetErrorInfo(ret);
        return ret;
    }
    LogInfo << "The transferMode of element(" << elementName_ << ") is " << *transferModeSptr << ".";
    auto removeSourceDataSptr = std::static_pointer_cast<std::string>(configParamMap["removeSourceData"]);
    LogInfo << "The element(" << elementName_ << ") remove source data or not: " << *removeSourceDataSptr << ".";
    ret = IsRemoveSourceData(*removeSourceDataSptr);
    if (ret != APP_ERR_OK) {
        LogError << "Invalid transfer config of removeParentData." << GetErrorInfo(ret);
        return ret;
    }

    LogInfo << "End to initialize MxpiDataTransfer(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiDataTransfer::ValidateInputData(std::shared_ptr<google::protobuf::Message>& inputData,
                                              std::string& dataType)
{
    APP_ERROR ret = APP_ERR_OK;
    const google::protobuf::Descriptor *desc = inputData->GetDescriptor();
    if (!desc) {
        errorInfo_ << "Get input data's descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM; // self define the error code
    }
    dataType = desc->name();
    if ((dataType != "MxpiVisionList") && (dataType != "MxpiTensorPackageList")) {
        errorInfo_ << "Element(" << elementName_ << ") supports MxpiVisionList and MxpiTensorPackageList only."
                   << "But the input data is " << dataType << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return ret;
}

APP_ERROR MxpiDataTransfer::FillDestMemoryInfo(MemoryData& srcData, MemoryData& destData)
{
    if (transferMode_ == (uint)MemoryTransferMode::MEMORY_TRANSFER_AUTO) {
        FillDestInfoUnderAutoMode(srcData, destData);
    } else if (transferMode_ == (uint)MemoryTransferMode::MEMORY_TRANSFER_D2H) {
        destData.type = MemoryData::MEMORY_HOST;
        CheckNotNeededTransferD2H(srcData, destData);
        LogDebug << elementName_ << ": transfer data from device(" << srcData.deviceId << ")'s to host memory.";
    } else if (transferMode_ == (uint)MemoryTransferMode::MEMORY_TRANSFER_H2D) {
        destData.type = MemoryData::MEMORY_DVPP;
        destData.deviceId = deviceId_;
        CheckNotNeededTransferH2D(srcData, destData);
        LogDebug << elementName_ << ": transfer data from host to device(" << srcData.deviceId << ")'s memory.";
    } else {
        errorInfo_ << "Element(" << elementName_ << ") gets invalid transfer mode(" << transferMode_ << ")."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    destData.size = srcData.size;

    return APP_ERR_OK;
}

void MxpiDataTransfer::CheckNotNeededTransferD2H(MemoryData &srcData, MemoryData &destData)
{
    if (srcData.type == destData.type) {
        LogWarn << elementName_ << ": no need to transfer memory data: source data is in host memory already.";
    }
}

void MxpiDataTransfer::CheckNotNeededTransferH2D(MemoryData &srcData, MemoryData &destData)
{
    if ((srcData.type == MemoryData::MEMORY_DVPP) && (srcData.deviceId == destData.deviceId)) {
        LogWarn << elementName_ << ": no need to transfer data: source data is in device("
                << deviceId_ << ")'s memory already.";
    }
}

void MxpiDataTransfer::FillDestInfoUnderAutoMode(MemoryData &srcData, MemoryData &destData)
{
    if (srcData.type == MemoryData::MEMORY_HOST || srcData.type == MemoryData::MEMORY_HOST_MALLOC ||
        srcData.type == MemoryData::MEMORY_HOST_NEW) {
        destData.type = MemoryData::MEMORY_DVPP;
        destData.deviceId = deviceId_;
        LogDebug << elementName_ << ": transfer memory data from host to device(" << deviceId_ << ")'s memory.";
    } else {
        destData.type = MemoryData::MEMORY_HOST;
        LogDebug << elementName_ << ": transfer memory data from device(" << srcData.deviceId << ") to host memory.";
    }
}

APP_ERROR MxpiDataTransfer::MemoryTransfer(MemoryData& srcData, MemoryData& destData)
{
    APP_ERROR ret = FillDestMemoryInfo(srcData, destData);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "[" << elementName_ << "] memory transfer: "
        << "source(ptr=" << srcData.ptrData << ", type=" << srcData.type
        << ", size=" << srcData.size << ", deviceId=" << srcData.deviceId << "), "
        << "dest(ptr=" << destData.ptrData << ", type=" << destData.type
        << ", size=" << destData.size << ", deviceId=" << destData.deviceId << ").";

    ret = MemoryHelper::MxbsMallocAndCopy(destData, srcData);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Failed to create destination memory, type(" << destData.type
                   << "), size(" << destData.size << ")." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }

    return APP_ERR_OK;
}

APP_ERROR MxpiDataTransfer::ClearVisionMemory(MxpiVisionData& visionData, MemoryData& memoryData)
{
    if (removeSourceData_) {
        APP_ERROR ret = MemoryHelper::MxbsFree(memoryData);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Failed to free parent vision memory." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
        visionData.set_dataptr(0);
        visionData.set_datasize(0);
    }

    return APP_ERR_OK;
}

APP_ERROR MxpiDataTransfer::DataTransferVisionList(std::shared_ptr<google::protobuf::Message>& inputData,
    std::shared_ptr<google::protobuf::Message>& outputData)
{
    APP_ERROR ret = APP_ERR_OK;
    auto visionListInput = std::static_pointer_cast<MxpiVisionList>(inputData);
    MxpiVisionList *mxpiVisionList = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionList == nullptr) {
        errorInfo_ << "The mxpiVisionList out of memory." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::shared_ptr<MxpiVisionList> visionListOutput(mxpiVisionList, g_deleteFuncMxpiVisionList);
    for (int i = 0; i < visionListInput->visionvec_size(); i++) {
        MemoryData srcData((void*)visionListInput->visionvec(i).visiondata().dataptr(),
            visionListInput->visionvec(i).visiondata().datasize(),
            (MemoryData::MemoryType)visionListInput->visionvec(i).visiondata().memtype(),
            visionListInput->visionvec(i).visiondata().deviceid());
        MemoryData destData;
        ret = MemoryTransfer(srcData, destData);
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            return ret;
        }

        // save data to the output vision list
        auto visionOutput = visionListOutput->add_visionvec();
        if (CheckPtrIsNullptr(visionOutput, "visionOutput"))  return APP_ERR_COMM_ALLOC_MEM;
        auto headerVecOutPut = visionOutput->add_headervec();
        if (CheckPtrIsNullptr(headerVecOutPut, "headerVecOutPut"))  return APP_ERR_COMM_ALLOC_MEM;
        headerVecOutPut->set_datasource(dataSource_);
        headerVecOutPut->set_memberid(i);
        visionOutput->mutable_visioninfo()->CopyFrom(visionListInput->visionvec(i).visioninfo());
        visionOutput->mutable_visiondata()->set_datasize(destData.size);
        visionOutput->mutable_visiondata()->set_dataptr((uint64_t)destData.ptrData);
        visionOutput->mutable_visiondata()->set_deviceid(destData.deviceId);
        visionOutput->mutable_visiondata()->set_freefunc((uint64_t)destData.free);
        visionOutput->mutable_visiondata()->set_memtype((MxpiMemoryType)destData.type);

        // free original memory if required
        ret = ClearVisionMemory(*visionListInput->mutable_visionvec(i)->mutable_visiondata(), srcData);
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            return ret;
        }
    }

    outputData = std::static_pointer_cast<google::protobuf::Message>(visionListOutput);
    return ret;
}

APP_ERROR MxpiDataTransfer::ClearTensorMemory(MxpiTensor& tensorData, MemoryData& memoryData)
{
    if (removeSourceData_) {
        APP_ERROR ret = MemoryHelper::MxbsFree(memoryData);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Failed to free parent tensor memory." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
        tensorData.set_tensordataptr(0);
        tensorData.set_tensordatasize(0);
    }

    return APP_ERR_OK;
}

APP_ERROR MxpiDataTransfer::DataTransferTensorPackageList(std::shared_ptr<google::protobuf::Message>& inputData,
    std::shared_ptr<google::protobuf::Message>& outputData)
{
    APP_ERROR ret = APP_ERR_OK;
    auto tensorPackageListInput = std::static_pointer_cast<MxpiTensorPackageList>(inputData);
    MxpiTensorPackageList *mxpiTensorPackageList = new (std::nothrow) MxpiTensorPackageList;
    if (mxpiTensorPackageList == nullptr) {
        errorInfo_ << "The mxpiTensorPackageList out of memory." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::shared_ptr<MxpiTensorPackageList> tensorPackageListOutput(mxpiTensorPackageList,
        g_deleteFuncMxpiTensorPackageList);
    for (int i = 0; i < tensorPackageListInput->tensorpackagevec_size(); i++) {
        auto tensorPackageInput = tensorPackageListInput->tensorpackagevec(i);
        auto tensorPackageOutput = tensorPackageListOutput->add_tensorpackagevec();
        if (CheckPtrIsNullptr(tensorPackageOutput, "tensorPackageOutput"))  return APP_ERR_COMM_ALLOC_MEM;
        auto headerVecOutPut = tensorPackageOutput->add_headervec();
        if (CheckPtrIsNullptr(headerVecOutPut, "headerVecOutPut"))  return APP_ERR_COMM_ALLOC_MEM;
        headerVecOutPut->set_datasource(dataSource_);
        headerVecOutPut->set_memberid(i);
        for (int j = 0; j < tensorPackageInput.tensorvec_size(); j++) {
            auto tensorInput = tensorPackageInput.tensorvec(j);
            MemoryData srcData((void *) tensorInput.tensordataptr(), tensorInput.tensordatasize(),
                (MemoryData::MemoryType) tensorInput.memtype(), tensorInput.deviceid());
            MemoryData destData;
            ret = MemoryTransfer(srcData, destData);
            if (ret != APP_ERR_OK) {
                LogError << errorInfo_.str() << GetErrorInfo(ret);
                return ret;
            }

            // save data to the output tensor vector
            auto tensorOutput = tensorPackageOutput->add_tensorvec();
            if (CheckPtrIsNullptr(tensorOutput, "tensorOutput"))  return APP_ERR_COMM_ALLOC_MEM;
            tensorOutput->mutable_tensorshape()->CopyFrom(tensorInput.tensorshape());
            tensorOutput->set_tensordatasize(destData.size);
            tensorOutput->set_tensordataptr((uint64_t) destData.ptrData);
            tensorOutput->set_deviceid(destData.deviceId);
            tensorOutput->set_freefunc((uint64_t) destData.free);
            tensorOutput->set_memtype((MxpiMemoryType) destData.type);

            // free original memory if required
            ret = ClearTensorMemory(*tensorPackageListInput->mutable_tensorpackagevec(i)->mutable_tensorvec(j),
                                    srcData);
            if (ret != APP_ERR_OK) {
                LogError << errorInfo_.str() << GetErrorInfo(ret);
                return ret;
            }
        }
    }
    outputData = std::static_pointer_cast<google::protobuf::Message>(tensorPackageListOutput);

    return ret;
}

APP_ERROR MxpiDataTransfer::Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer)
{
    LogDebug << "Begin to process MxpiDataTransfer(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // Check the input data.
    MxpiBuffer* buffer = mxpiBuffer[0];
    MxpiMetadataManager mxpiMetadataManager(*buffer);
    errorInfo_.str("");

    auto inputDataSptr = std::static_pointer_cast<google::protobuf::Message>(
            mxpiMetadataManager.GetMetadata(dataSource_));
    if (inputDataSptr == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << "metadata is null.";
        SendData(0, *buffer);
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
    }
    std::string dataType;
    ret = ValidateInputData(inputDataSptr, dataType);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }

    // Data transfer
    std::shared_ptr<google::protobuf::Message> outputDataSptr = nullptr;
    if (dataType == "MxpiVisionList") {
        LogDebug << "Element(" << elementName_ << ") start to transfer MxpiVisionList.";
        ret = DataTransferVisionList(inputDataSptr, outputDataSptr);
    } else if (dataType == "MxpiTensorPackageList") {
        LogDebug << "Element(" << elementName_ << ") start to transfer MxpiTensorPackageList.";
        ret = DataTransferTensorPackageList(inputDataSptr, outputDataSptr);
    }
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }

    ret = mxpiMetadataManager.AddProtoMetadata(elementName_, std::static_pointer_cast<void>(outputDataSptr));
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Failed to add proto metadata." << GetErrorInfo(ret);
        SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    SendData(0, *buffer); // Send the data to downstream element
    LogDebug << "End to process MxpiDataTransfer(" << elementName_ << ").";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiDataTransfer::DefineProperties()
{
    // Define an A to store properties
    std::vector<std::shared_ptr<void>> properties;
    // Set the type and related information of the properties, and the key is the name
    auto transferModePropSptr = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "transferMode", "transferMode",
            "move the memory in three modes: auto, d2h(device2host), h2d(host2device)", "auto", "null", "null"
    });
    auto removeSourceDataPropSptr = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "removeSourceData", "removeData",
            "free the memory inside the input data", "yes", "NULL", "NULL"
    });
    properties = { transferModePropSptr, removeSourceDataPropSptr };
    return properties;
}

APP_ERROR MxpiDataTransfer::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiDataTransfer(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiDataTransfer(" << elementName_ << ").";
    return APP_ERR_OK;
}

MxpiPortInfo MxpiDataTransfer::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/yuv", "metadata/tensor", "image/rgb"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxpiPortInfo MxpiDataTransfer::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/yuv", "metadata/tensor", "image/rgb"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

// Register the MxpiDataTransfer plugin with macro
namespace {
MX_PLUGIN_GENERATE(MxpiDataTransfer)
}
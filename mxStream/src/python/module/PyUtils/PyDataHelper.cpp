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
 * Description: A class of data process. Including transform python-C++ data, parse proto data to string, etc.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "PyUtils/PyDataHelper.h"

#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"

namespace {
const unsigned int MAX_INPUT_SIZE = 2147483637;  // Maximum string length for input plugins
enum class CopyType {
    COPY_PTR_TO_HOST = 0,
    COPY_STR_TO_HOST = 1,
};

APP_ERROR RebuildVisionList(std::shared_ptr<google::protobuf::Message> &messagePtr)
{
    auto mxpiVisionList = std::static_pointer_cast<MxTools::MxpiVisionList>(messagePtr);
    auto visionVec = mxpiVisionList->mutable_visionvec();
    bool getVision = false;
    for (auto &mxpiVision : *visionVec) {
        MxTools::MxpiVisionData *visionData = mxpiVision.mutable_visiondata();
        MxBase::MemoryData srcData{visionData->datastr().size(), MxBase::MemoryData::MEMORY_HOST_MALLOC};
        APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(srcData);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc." << GetErrorInfo(ret);
            return ret;
        }
        std::copy((char *)visionData->datastr().c_str(),
            (char *)visionData->datastr().c_str() + visionData->datastr().size(),
            (char *)srcData.ptrData);
        visionData->set_dataptr((google::protobuf::uint64)srcData.ptrData);

        visionData->set_datasize(visionData->datastr().size());
        visionData->set_memtype(MxTools::MXPI_MEMORY_HOST_MALLOC);
        visionData->set_datastr("");
        getVision = true;
    }
    if (!getVision) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR ReBuildTensorPackageList(std::shared_ptr<google::protobuf::Message> &messagePtr)
{
    auto mxpiTensorPackageList = (MxTools::MxpiTensorPackageList *)messagePtr.get();
    auto tensorPackageVec = mxpiTensorPackageList->mutable_tensorpackagevec();
    bool getTensor = false;
    for (auto &tensorPackage : *tensorPackageVec) {
        auto tensorVec = tensorPackage.mutable_tensorvec();
        for (auto &mxpiTensor : *tensorVec) {
            MxBase::MemoryData srcData{mxpiTensor.datastr().size(), MxBase::MemoryData::MEMORY_HOST_MALLOC};
            APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(srcData);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to malloc." << GetErrorInfo(ret);
                return ret;
            }
            std::copy((char *)mxpiTensor.datastr().c_str(),
                (char *)mxpiTensor.datastr().c_str() + mxpiTensor.datastr().size(),
                (char *)srcData.ptrData);
            mxpiTensor.set_tensordataptr((google::protobuf::uint64)srcData.ptrData);

            mxpiTensor.set_tensordatasize(mxpiTensor.datastr().size());
            mxpiTensor.set_memtype(MxTools::MXPI_MEMORY_HOST_MALLOC);
            mxpiTensor.set_datastr("");
            getTensor = true;
        }
    }
    if (!getTensor) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR CopyDataPtrToHost(size_t size, size_t deviceId, unsigned long int dataPtr, MxBase::MemoryData &memoryDataDst,
    MxBase::MemoryData::MemoryType srcMemoryType)
{
    if (size == 0) {
        LogWarn << "size is 0. it's not need to copy.";
        return APP_ERR_OK;
    }
    MxBase::MemoryData memoryDataSrc((void *)dataPtr, size, srcMemoryType, deviceId);
    memoryDataDst.size = size;
    memoryDataDst.deviceId = static_cast<int32_t>(deviceId);
    memoryDataDst.ptrData = nullptr;
    memoryDataDst.type = MxBase::MemoryData::MEMORY_HOST;
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
    if (ret != APP_ERR_OK) {
        LogError << "Memory malloc failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR CopyDataStrToHost(size_t size, int32_t deviceId, const std::string &dataStr,
    MxBase::MemoryData &memoryDataDst, MxBase::MemoryData::MemoryType srcMemoryType)
{
    MxBase::MemoryData memoryDataSrc((void *)dataStr.c_str(), size, srcMemoryType, deviceId);
    memoryDataDst.size = size;
    memoryDataDst.deviceId = static_cast<int32_t>(deviceId);
    memoryDataDst.ptrData = nullptr;
    memoryDataDst.type = MxBase::MemoryData::MEMORY_HOST;
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
    if (ret != APP_ERR_OK) {
        LogError << "Memory malloc failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR SetMxpiVisionData(MxTools::MxpiVisionData &visionData, MxBase::MemoryData &memoryDataDst,
    MxBase::MemoryData::MemoryType memoryType, CopyType copyType)
{
    auto deviceManager = MxBase::DeviceManager::GetInstance();
    MxBase::DeviceContext deviceContext;
    deviceContext.devId = static_cast<int32_t>(visionData.deviceid());
    APP_ERROR ret = deviceManager->SetDevice(deviceContext);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set deviceID(" << deviceContext.devId << ")." << GetErrorInfo(ret);
        return ret;
    }
    if (copyType == CopyType::COPY_PTR_TO_HOST) {
        ret = CopyDataPtrToHost(
            visionData.datasize(), visionData.deviceid(), visionData.dataptr(), memoryDataDst, memoryType);
    } else if (copyType == CopyType::COPY_STR_TO_HOST) {
        visionData.set_datasize(visionData.datastr().size());
        ret = CopyDataStrToHost(
            visionData.datastr().size(), visionData.deviceid(), visionData.datastr(), memoryDataDst, memoryType);
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to copy protobuf to host." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

std::shared_ptr<google::protobuf::Message> ParseVisionList(const std::shared_ptr<google::protobuf::Message> &messagePtr)
{
    auto mxpiVisionList = std::static_pointer_cast<MxTools::MxpiVisionList>(messagePtr);
    auto visionVec = mxpiVisionList->mutable_visionvec();
    for (auto &mxpiVision : *visionVec) {
        auto visionData = mxpiVision.mutable_visiondata();
        if (visionData->memtype() == MxTools::MXPI_MEMORY_HOST ||
            visionData->memtype() == MxTools::MXPI_MEMORY_HOST_MALLOC ||
            visionData->memtype() == MxTools::MXPI_MEMORY_HOST_NEW) {
            visionData->set_datastr(std::string((char *)visionData->dataptr(), visionData->datasize()));
        } else {
            MxBase::MemoryData memoryDataDst = {};
            APP_ERROR ret = SetMxpiVisionData(*visionData,
                memoryDataDst,
                (MxBase::MemoryData::MemoryType)visionData->memtype(),
                CopyType::COPY_PTR_TO_HOST);
            if (ret != APP_ERR_OK) {
                LogError << "Parse VisionList string to messagePtr error." << GetErrorInfo(ret);
                return std::shared_ptr<google::protobuf::Message>(nullptr);
            }
            visionData->set_datastr(std::string((char *)memoryDataDst.ptrData, visionData->datasize()));
            MxBase::MemoryHelper::MxbsFree(memoryDataDst);
        }
    }
    return messagePtr;
}

APP_ERROR SetMxpiTensor(MxTools::MxpiTensor &mxpiTensor, MxBase::MemoryData &memoryDataDst,
    MxBase::MemoryData::MemoryType memoryType, CopyType copyType)
{
    auto deviceManager = MxBase::DeviceManager::GetInstance();
    MxBase::DeviceContext deviceContext;
    deviceContext.devId = static_cast<int32_t>(mxpiTensor.deviceid());
    APP_ERROR ret = deviceManager->SetDevice(deviceContext);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set deviceID(" << deviceContext.devId << ")." << GetErrorInfo(ret);
        return ret;
    }
    if (copyType == CopyType::COPY_PTR_TO_HOST) {
        ret = CopyDataPtrToHost(
            mxpiTensor.tensordatasize(), mxpiTensor.deviceid(), mxpiTensor.tensordataptr(), memoryDataDst, memoryType);
    } else if (copyType == CopyType::COPY_STR_TO_HOST) {
        mxpiTensor.set_tensordatasize(mxpiTensor.datastr().size());
        ret = CopyDataStrToHost(
            mxpiTensor.datastr().size(), mxpiTensor.deviceid(), mxpiTensor.datastr(), memoryDataDst, memoryType);
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to copy protobuf to host." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR SetMxpiTensorStr(MxTools::MxpiTensor &mxpiTensor)
{
    if (mxpiTensor.memtype() == MxTools::MXPI_MEMORY_HOST || mxpiTensor.memtype() == MxTools::MXPI_MEMORY_HOST_MALLOC ||
        mxpiTensor.memtype() == MxTools::MXPI_MEMORY_HOST_NEW) {
        mxpiTensor.set_datastr(std::string((char *)mxpiTensor.tensordataptr(), mxpiTensor.tensordatasize()));
    } else {
        MxBase::MemoryData memoryDataDst = {};
        APP_ERROR ret = SetMxpiTensor(mxpiTensor,
            memoryDataDst,
            (MxBase::MemoryData::MemoryType)mxpiTensor.memtype(),
            CopyType::COPY_PTR_TO_HOST);
        if (ret != APP_ERR_OK) {
            LogError << "Parse TensorPackageList string to messagePtr error." << GetErrorInfo(ret);
            return ret;
        }
        mxpiTensor.set_datastr(std::string((char *)memoryDataDst.ptrData, mxpiTensor.tensordatasize()));
        MxBase::MemoryHelper::MxbsFree(memoryDataDst);
    }
    return APP_ERR_OK;
}

std::shared_ptr<google::protobuf::Message> ParseTensorPackageList(
    const std::shared_ptr<google::protobuf::Message> &messagePtr)
{
    auto mxpiTensorPackageList = std::static_pointer_cast<MxTools::MxpiTensorPackageList>(messagePtr);
    auto tensorPackageVec = mxpiTensorPackageList->mutable_tensorpackagevec();
    for (auto &tensorPackage : *tensorPackageVec) {
        auto tensorVec = tensorPackage.mutable_tensorvec();
        for (auto &mxpiTensor : *tensorVec) {
            auto ret = SetMxpiTensorStr(mxpiTensor);
            if (ret != APP_ERR_OK) {
                return std::shared_ptr<google::protobuf::Message>(nullptr);
            }
        }
    }
    return messagePtr;
}
}  // namespace
namespace PyStream {
std::shared_ptr<google::protobuf::Message> PyDataHelper::GetProtobufPtr(
    const std::string &protoDataType, const std::string &protobuf)
{
    if (protobuf.size() > MAX_INPUT_SIZE) {
        LogError << "Input data size(" << protobuf.size() << ") out of range[0, " << MAX_INPUT_SIZE << "]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return nullptr;
    }
    google::protobuf::Message *message = nullptr;
    const google::protobuf::Descriptor *descriptor =
        google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(protoDataType);
    if (!descriptor) {
        LogError << "MxpiDataTransfer error: Cannot get descriptor." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    const google::protobuf::Message *prototype =
        google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
    if (!prototype) {
        LogError << "MxpiDataTransfer error: Cannot get prototype." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    message = prototype->New();
    if (message == nullptr) {
        LogError << "Failed to create Message." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    if (!message->ParseFromString(protobuf)) {
        LogError << "Failed to ParseFromString" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        delete message;
        message = nullptr;
        return nullptr;
    }
    if (protoDataType == "MxTools.MxpiVisionList") {
        std::shared_ptr<MxTools::MxpiVisionList> msg(
            (MxTools::MxpiVisionList *)message, MxTools::g_deleteFuncMxpiVisionList);
        return msg;
    } else if (protoDataType == "MxTools.MxpiTensorPackageList") {
        std::shared_ptr<MxTools::MxpiTensorPackageList> msg(
            (MxTools::MxpiTensorPackageList *)message, MxTools::g_deleteFuncMxpiTensorPackageList);
        return msg;
    }

    std::shared_ptr<google::protobuf::Message> msg(message);
    return msg;
}

void PyDataHelper::ReBuildMxpiList(
    const std::string &protoDataType, std::shared_ptr<google::protobuf::Message> &messagePtr)
{
    APP_ERROR ret = APP_ERR_OK;
    if (protoDataType == "MxTools.MxpiVisionList") {
        ret = RebuildVisionList(messagePtr);
        if (ret != APP_ERR_OK) {
            LogError << "ReBuild VisionList failed, return message null." << GetErrorInfo(ret);
            messagePtr = std::shared_ptr<google::protobuf::Message>(nullptr);
        }
    } else if (protoDataType == "MxTools.MxpiTensorPackageList") {
        ret = ReBuildTensorPackageList(messagePtr);
        if (ret != APP_ERR_OK) {
            LogError << "ReBuild MxpiTensorPackageList failed, return message null." << GetErrorInfo(ret);
            messagePtr = std::shared_ptr<google::protobuf::Message>(nullptr);
        }
    }
}

void PyDataHelper::PyBufferToC(const BufferInput &srcData, MxStream::MxstBufferInput &dstData)
{
    dstData.dataSize = static_cast<int>(srcData.data.size());
    dstData.dataPtr = (uint32_t *)srcData.data.c_str();
    dstData.mxpiFrameInfo.ParseFromString(srcData.frame_info);
    dstData.mxpiVisionInfo.ParseFromString(srcData.vision_info);
}

void PyDataHelper::PyMxdataInputToC(const MxDataInput &mxdataInput, MxStream::MxstDataInput &mxstDataInput)
{
    mxstDataInput.dataSize = static_cast<int>(mxdataInput.data.size());
    mxstDataInput.dataPtr = (uint32_t *)mxdataInput.data.c_str();
    mxstDataInput.serviceInfo.fragmentId = mxdataInput.fragmentId;
    mxstDataInput.serviceInfo.customParam = mxdataInput.customParam;
    for (const auto &roi : mxdataInput.roiBoxs) {
        MxStream::CropRoiBox cropRoiBox{roi.x0, roi.y0, roi.x1, roi.y1};
        mxstDataInput.serviceInfo.roiBoxs.push_back(cropRoiBox);
    }
}

std::vector<int> PyDataHelper::NumToInPluginVec(const int inPluginNum)
{
    std::vector<int> inPluginIdVec;
    for (int i = 0; i < inPluginNum; i++) {
        inPluginIdVec.push_back(i);
    }
    return inPluginIdVec;
}

APP_ERROR PyDataHelper::PyMetadataToC(const MetadataInput &srcData, MxStream::MxstMetadataInput &dstData)
{
    LogDebug << "key:" << srcData.data_source << " type:" << srcData.data_type;
    dstData.dataSource = srcData.data_source;
    dstData.messagePtr = PyDataHelper::GetProtobufPtr(srcData.data_type, srcData.serialized_metadata);
    if (dstData.messagePtr == nullptr) {
        LogError << "Transform string to messagePtr failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    PyDataHelper::ReBuildMxpiList(srcData.data_type, dstData.messagePtr);
    if (dstData.messagePtr == nullptr) {
        LogError << "Transform messageStr to messagePtr failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

void PyDataHelper::OutputToPy(const MxStream::MxstBufferAndMetadataOutput &srcData, DataOutput &dstData)
{
    dstData.buffer_output.error_code = srcData.bufferOutput->errorCode;
    if (srcData.bufferOutput->dataPtr != nullptr && srcData.bufferOutput->dataSize != 0) {
        std::string result((char *)srcData.bufferOutput->dataPtr, srcData.bufferOutput->dataSize);
        dstData.buffer_output.data = result;
    }
    for (const auto &dataBuffer : srcData.metadataVec) {
        MetadataOutput output;
        output.data_type = dataBuffer.dataType;
        if (dataBuffer.dataPtr != nullptr) {
            std::shared_ptr<google::protobuf::Message> messagePtr =
                PyDataHelper::ParseMessagePtrToString(dataBuffer.dataPtr);
            if (messagePtr == nullptr) {
                LogError << "Transform messagePtr to messageStr failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
                continue;
            }
            output.serialized_metadata = PyDataHelper::GetMessageString(messagePtr);
            dstData.metadata_list.push_back(output);
        }
    }
}

void PyDataHelper::MxstDataOutputToPy(MxStream::MxstDataOutput *srcData, MxDataOutput &dstData)
{
    dstData.errorCode = srcData->errorCode;
    if (srcData->dataPtr != nullptr && srcData->dataSize != 0) {
        std::string result((char *)srcData->dataPtr);
        dstData.data = result;
        dstData.dataSize = srcData->dataSize;
    }
}

std::shared_ptr<google::protobuf::Message> PyDataHelper::ParseMessagePtrToString(
    std::shared_ptr<google::protobuf::Message> messagePtr)
{
    const google::protobuf::Descriptor *descriptor = messagePtr->GetDescriptor();
    if (descriptor == nullptr) {
        LogError << "Get descriptor failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    if (descriptor->name() == "MxpiVisionList") {
        return ParseVisionList(messagePtr);
    } else if (descriptor->name() == "MxpiTensorPackageList") {
        return ParseTensorPackageList(messagePtr);
    }
    return messagePtr;
}

std::string PyDataHelper::GetMessageString(const std::shared_ptr<google::protobuf::Message> &messagePtr)
{
    std::string messageStr;
    if (messagePtr != nullptr && !messagePtr->SerializeToString(&messageStr)) {
        LogError << "Serialize protobuf message to string failed" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return "";
    }
    return messageStr;
}
}  // namespace PyStream
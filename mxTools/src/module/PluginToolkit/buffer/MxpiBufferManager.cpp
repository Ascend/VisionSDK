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
 * Description: Buffer manager for creating custom plugins.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include <iostream>
#include <algorithm>
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/Log/Log.h"

using namespace MxTools;
using MxBase::MemoryData;

namespace {
const std::string FRAME_INFO_KEY = "ReservedFrameInfo";
const std::string RESERVED_VISION_LIST_KEY = "ReservedVisionList";
enum class CreateBufferMemoryType {
    HOST,
    DEVICE,
    DEVICE_WITH_MEM,
    HOST_WITH_MEM
};
struct HostBufferMetaDataPtr {
    std::shared_ptr<MxpiVisionList> mxpiVisionList;
    std::shared_ptr<std::string> ptr;
    MemoryData memoryData;
};

MxpiBuffer* CreateMxpiBuffer()
{
    auto mxpiBuffer = new (std::nothrow) MxpiBuffer;
    if (mxpiBuffer == nullptr) {
        LogError << "create MxpiBuffer object, allocate memory with \"new MxpiBuffer\" failed."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    mxpiBuffer->buffer = gst_buffer_new();
    if (mxpiBuffer->buffer == nullptr) {
        LogError << "create gst buffer, gst_buffer_new failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        delete mxpiBuffer;
        return nullptr;
    }
    return mxpiBuffer;
}
 
APP_ERROR CreateMetaDataPtr(const InputParam& inputParam, HostBufferMetaDataPtr& dataPtr,
                            bool isDeviceOrUseMemoryType = false,
                            CreateBufferMemoryType type = CreateBufferMemoryType::HOST)
{
    LogDebug << "Begin to create vision list";
 
    std::shared_ptr<MxpiVisionList> mxpiVisionList = nullptr;
    auto mxpiVisionListPtr = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionListPtr == nullptr) {
        LogError << "mxpiVisionList pointer is nullptr." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    mxpiVisionList.reset(mxpiVisionListPtr, g_deleteFuncMxpiVisionList);
    LogDebug << "End to create vision list";
 
    dataPtr.mxpiVisionList = mxpiVisionList;
 
    auto mxpiVision = dataPtr.mxpiVisionList->add_visionvec();
    if (mxpiVision == nullptr) {
        LogError << "mxpiVision is nullptr. Create MxpiVisionList object failed."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    auto mxpiVisionInfo = mxpiVision->mutable_visioninfo();
    mxpiVisionInfo->CopyFrom(inputParam.mxpiVisionInfo);
    if (isDeviceOrUseMemoryType) {
        auto mxpiVisionData = mxpiVision->mutable_visiondata();
        if (mxpiVisionData == nullptr) {
            LogError << "The mxpiVisionData is nullptr. Get mxpiVisionData object failed."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        mxpiVisionData->set_datasize(inputParam.dataSize);
        mxpiVisionData->set_deviceid(inputParam.deviceId);
        if (type == CreateBufferMemoryType::DEVICE) {
            mxpiVisionData->set_memtype(MXPI_MEMORY_DVPP);
            mxpiVisionData->set_dataptr(reinterpret_cast<uintptr_t>(dataPtr.memoryData.ptrData));
        } else if (type == CreateBufferMemoryType::DEVICE_WITH_MEM) {
            mxpiVisionData->set_memtype(inputParam.mxpiMemoryType);
            mxpiVisionData->set_dataptr(reinterpret_cast<uintptr_t>(inputParam.ptrData));
        } else if (type == CreateBufferMemoryType::HOST_WITH_MEM) {
            mxpiVisionData->set_memtype(inputParam.mxpiMemoryType);
            mxpiVisionData->set_dataptr(reinterpret_cast<uintptr_t>(inputParam.ptrData));
            mxpiVisionData->set_datatype((MxTools::MxpiDataType)inputParam.dataType);
        }
    }
 
    dataPtr.ptr = MxBase::MemoryHelper::MakeShared<std::string>(inputParam.key);
    if (dataPtr.ptr == nullptr) {
        LogError << "Create string object failed. Failed to allocate memory."
                    << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    return APP_ERR_OK;
}
}

MxpiBufferManager::MxpiBufferManager() {}

MxpiBufferManager::~MxpiBufferManager() {}

MxpiBuffer* MxpiBufferManager::CreateHostBuffer(const InputParam& inputParam)
{
    LogDebug << "Begin to create host buffer. Memory size(" << inputParam.dataSize << ").";
    APP_ERROR ret = MxBase::MemoryHelper::CheckDataSizeAllowZero(inputParam.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to create host buffer with invalid memory size:" << inputParam.dataSize << "."
                 << GetErrorInfo(ret);
        return nullptr;
    }
    auto mxpiBuffer = CreateMxpiBuffer();
    if (mxpiBuffer == nullptr) {
        LogError << "create host buffer failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    if (inputParam.dataSize != 0 && inputParam.ptrData != nullptr) {
        ret = GstAppendMemory(inputParam, mxpiBuffer);
        if (ret != APP_ERR_OK) {
            LogError << "GstAppendMemory failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            return nullptr;
        }
    } else {
        LogDebug << "create host buffer, Memory size(0).";
    }
    if (!inputParam.key.empty()) {
        HostBufferMetaDataPtr metaDataPtr;
        ret = CreateMetaDataPtr(inputParam, metaDataPtr);
        if (ret != APP_ERR_OK) {
            LogError << "Create mxpiVisionList pointer or string object failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            return nullptr;
        }
        AddMetadataInfo(RESERVED_VISION_LIST_KEY, *mxpiBuffer, std::static_pointer_cast<void>(metaDataPtr.ptr));
        MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
        ret = mxpiMetadataManager.AddProtoMetadata(inputParam.key,
                                                   std::static_pointer_cast<void>(metaDataPtr.mxpiVisionList));
        if (ret != APP_ERR_OK) {
            LogError << "AddProtoMetadata failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            return nullptr;
        }
    }
    AddFrameInfoToMetadata(FRAME_INFO_KEY, *mxpiBuffer, inputParam.mxpiFrameInfo);
    LogDebug << "End to create host buffer. Memory size(" << inputParam.dataSize << ").";
    return mxpiBuffer;
}

APP_ERROR MxpiBufferManager::GstAppendMemory(const InputParam& inputParam, MxpiBuffer* mxpiBuffer)
{
    auto buffer = g_malloc(inputParam.dataSize);
    if (buffer == nullptr) {
        LogError << "create host buffer, g_malloc failed. dataSize(" << inputParam.dataSize << ")."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    GstMemory *gstMemory = gst_memory_new_wrapped(GST_MEMORY_FLAG_READONLY, buffer,
        inputParam.dataSize, 0, inputParam.dataSize, buffer, g_free);
    if (gstMemory == nullptr) {
        LogError << "Create gst memory wrapper failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        g_free(buffer);
        return APP_ERR_COMM_FAILURE;
    }
    if (mxpiBuffer->buffer == nullptr) {
        LogError << "Null ptr." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        g_free(buffer);
        gst_memory_unref(gstMemory);
        return APP_ERR_COMM_FAILURE;
    }
    gst_buffer_append_memory((GstBuffer*) mxpiBuffer->buffer, gstMemory);
    return APP_ERR_OK;
}

MxpiBuffer* MxpiBufferManager::CreateHostBufferAndCopyData(const InputParam& inputParam)
{
    LogDebug << "Begin to create host buffer and copy data. Memory size(" << inputParam.dataSize << ").";
    if (MxBase::MemoryHelper::CheckDataSizeAllowZero(inputParam.dataSize) != APP_ERR_OK) {
        LogError << "Failed to create host buffer with invalid memory size:" << inputParam.dataSize << "."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    auto mxpiBuffer = CreateMxpiBuffer();
    if (mxpiBuffer == nullptr) {
        return nullptr;
    }
    if (inputParam.dataSize > 0 && inputParam.ptrData != nullptr) {
        APP_ERROR ret = GstAppendMemory(inputParam, mxpiBuffer);
        if (ret != APP_ERR_OK) {
            LogError << "GstAppendMemory failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            return nullptr;
        }
        GstMapInfo gstMapInfo;
        gst_buffer_map((GstBuffer*) mxpiBuffer->buffer, &gstMapInfo, GST_MAP_WRITE);
        std::copy((uint8_t*) inputParam.ptrData, (uint8_t*) inputParam.ptrData + inputParam.dataSize, gstMapInfo.data);
        gst_buffer_unmap((GstBuffer*) mxpiBuffer->buffer, &gstMapInfo);
    } else {
        LogInfo << "create host buffer and copy data, Memory size(0).";
    }
    if (!inputParam.key.empty()) {
        HostBufferMetaDataPtr dataPtr;
        APP_ERROR ret = CreateMetaDataPtr(inputParam, dataPtr);
        if (ret != APP_ERR_OK) {
            LogError << "Create mxpiVisionList pointer or string object failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            return nullptr;
        }
        AddMetadataInfo(RESERVED_VISION_LIST_KEY, *mxpiBuffer, std::static_pointer_cast<void>(dataPtr.ptr));
        MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
        ret = mxpiMetadataManager.AddProtoMetadata(inputParam.key,
                                                   std::static_pointer_cast<void>(dataPtr.mxpiVisionList));
        if (ret != APP_ERR_OK) {
            LogError << "AddProtoMetadata failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            return nullptr;
        }
    }
    AddFrameInfoToMetadata(FRAME_INFO_KEY, *mxpiBuffer, inputParam.mxpiFrameInfo);
    LogDebug << "End to create host buffer and copy data. Memory size(" << inputParam.dataSize << ").";
    return mxpiBuffer;
}

APP_ERROR MxpiBufferManager::AddData(const InputParam& inputParam, void* buffer)
{
    LogDebug << "Begin to add MxpiVisionList and MxpiFrameInfo data on the buffer.";
    if (buffer == nullptr) {
        LogError << "The buffer cannot be nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }

    MxpiBuffer mxpiBuffer;
    mxpiBuffer.buffer = buffer;
    if (!inputParam.key.empty()) {
        auto mxpiVisionList = CreateVisionList();
        if (mxpiVisionList == nullptr) {
            LogError << "mxpiVisionList pointer is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        auto mxpiVision =  mxpiVisionList->add_visionvec();
        if (mxpiVision == nullptr) {
            LogError << "mxpiVision pointer is nullptr." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        auto mxpiVisionInfo = mxpiVision->mutable_visioninfo();
        mxpiVisionInfo->CopyFrom(inputParam.mxpiVisionInfo);
        auto ptr = MxBase::MemoryHelper::MakeShared<std::string>(inputParam.key);
        if (ptr == nullptr) {
            LogError << "Create string object failed. Failed to allocate memory."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        AddMetadataInfo(RESERVED_VISION_LIST_KEY, mxpiBuffer, std::static_pointer_cast<void>(ptr));
        MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
        mxpiMetadataManager.AddProtoMetadata(inputParam.key, std::static_pointer_cast<void>(mxpiVisionList));
    }
    AddFrameInfoToMetadata(FRAME_INFO_KEY, mxpiBuffer, inputParam.mxpiFrameInfo);
    LogDebug << "End to add MxpiVisionList and MxpiFrameInfo data on the buffer. Memory size("
             << inputParam.dataSize << ").";
    return APP_ERR_OK;
}

MxpiBuffer* MxpiBufferManager::CreateDeviceBuffer(const InputParam& inputParam)
{
    LogDebug << "Begin to create device buffer. Memory size("
             << inputParam.dataSize << "), deviceId(" << inputParam.deviceId << ").";
    if (!CheckInputParam(inputParam)) {
        return nullptr;
    }
    auto mxpiBuffer = CreateMxpiBuffer();
    if (mxpiBuffer == nullptr) {
        return nullptr;
    }
    if (!inputParam.key.empty()) {
        MemoryData memoryData(nullptr, inputParam.dataSize, MemoryData::MEMORY_DVPP, inputParam.deviceId);
        if (inputParam.dataSize != 0 && inputParam.ptrData != nullptr) {
            if (APP_ERR_OK != MxBase::MemoryHelper::MxbsMalloc(memoryData)) {
                LogError << "Dvpp memory malloc failed. data size: " << memoryData.size << "."
                         << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                DestroyBuffer(mxpiBuffer);
                return nullptr;
            }
        } else {
            LogDebug << "create device buffer, Memory size(0).";
        }
        HostBufferMetaDataPtr dataPtr;
        dataPtr.memoryData = memoryData;
        APP_ERROR ret = CreateMetaDataPtr(inputParam, dataPtr, true, CreateBufferMemoryType::DEVICE);
        if (ret != APP_ERR_OK) {
            LogError << "Create mxpiVisionList pointer or string object failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            MxBase::MemoryHelper::MxbsFree(memoryData);
            return nullptr;
        }
        AddMetadataInfo(RESERVED_VISION_LIST_KEY, *mxpiBuffer, std::static_pointer_cast<void>(dataPtr.ptr));
        MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
        ret = mxpiMetadataManager.AddProtoMetadata(inputParam.key,
                                                   std::static_pointer_cast<void>(dataPtr.mxpiVisionList));
        if (ret != APP_ERR_OK) {
            LogError << "AddProtoMetadata failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            MxBase::MemoryHelper::MxbsFree(memoryData);
            return nullptr;
        }
    }
    AddFrameInfoToMetadata(FRAME_INFO_KEY, *mxpiBuffer, inputParam.mxpiFrameInfo);
    LogDebug << "End to create device buffer end. Memory size("
             << inputParam.dataSize << "), deviceId(" << inputParam.deviceId << ").";
    return mxpiBuffer;
}

MxpiBuffer* MxpiBufferManager::CreateDeviceBufferAndCopyData(const InputParam& inputParam)
{
    LogDebug << "Begin to create device buffer and copy data. Memory size(" << inputParam.dataSize << ").";
    if (!CheckInputParam(inputParam)) {
        LogError << "Get deviceId(" << inputParam.deviceId <<") status failed."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    auto mxpiBuffer = CreateMxpiBuffer();
    if (mxpiBuffer == nullptr) {
        LogError << "create device buffer and copy data failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    if (!inputParam.key.empty()) {
        MemoryData memoryDataDst;
        memoryDataDst.ptrData = nullptr;
        if (!CopyDeviceMemory(memoryDataDst, inputParam)) {
            LogError << "copy device memory failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            DestroyBuffer(mxpiBuffer);
            return nullptr;
        }
        HostBufferMetaDataPtr dataPtr;
        dataPtr.memoryData = memoryDataDst;
        APP_ERROR ret = CreateMetaDataPtr(inputParam, dataPtr, true, CreateBufferMemoryType::DEVICE);
        if (ret != APP_ERR_OK) {
            LogError << "Create mxpiVisionList pointer or string object failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            MxBase::MemoryHelper::MxbsFree(memoryDataDst);
            return nullptr;
        }
        AddMetadataInfo(RESERVED_VISION_LIST_KEY, *mxpiBuffer, std::static_pointer_cast<void>(dataPtr.ptr));
        MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
        ret = mxpiMetadataManager.AddProtoMetadata(inputParam.key,
                                                   std::static_pointer_cast<void>(dataPtr.mxpiVisionList));
        if (ret != APP_ERR_OK) {
            LogError << "AddProtoMetadata failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            MxBase::MemoryHelper::MxbsFree(memoryDataDst);
            return nullptr;
        }
    }
    AddFrameInfoToMetadata(FRAME_INFO_KEY, *mxpiBuffer, inputParam.mxpiFrameInfo);
    LogDebug << "End to create device buffer and copy data. Memory size(" << inputParam.dataSize << ").";
    return mxpiBuffer;
}

MxpiBuffer* MxpiBufferManager::CreateDeviceBufferWithMemory(const InputParam& inputParam)
{
    LogDebug << "Begin to create device buffer. Memory size("
             << inputParam.dataSize << "), deviceId(" << inputParam.deviceId << ").";
    if (!CheckInputParam(inputParam)) {
        return nullptr;
    }
    if (inputParam.mxpiMemoryType != MXPI_MEMORY_DVPP && inputParam.mxpiMemoryType != MXPI_MEMORY_DEVICE) {
        LogError << "Please check InputParam.mxpiMemoryType info, it must be device or dvpp."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return nullptr;
    }
    auto mxpiBuffer = CreateMxpiBuffer();
    if (mxpiBuffer == nullptr) {
        LogError << "create device buffer failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    if (!inputParam.key.empty()) {
        HostBufferMetaDataPtr dataPtr;
        APP_ERROR ret = CreateMetaDataPtr(inputParam, dataPtr, true, CreateBufferMemoryType::DEVICE_WITH_MEM);
        if (ret != APP_ERR_OK) {
            LogError << "Create mxpiVisionList pointer or string object failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            return nullptr;
        }
        AddMetadataInfo(RESERVED_VISION_LIST_KEY, *mxpiBuffer, std::static_pointer_cast<void>(dataPtr.ptr));
        MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
        ret = mxpiMetadataManager.AddProtoMetadata(inputParam.key,
                                                   std::static_pointer_cast<void>(dataPtr.mxpiVisionList));
        if (ret != APP_ERR_OK) {
            LogError << "AddProtoMetadata failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            return nullptr;
        }
    }
    AddFrameInfoToMetadata(FRAME_INFO_KEY, *mxpiBuffer, inputParam.mxpiFrameInfo);
    LogDebug << "End to create device buffer end. Memory size("
             << inputParam.dataSize << "), deviceId(" << inputParam.deviceId << ").";
    return mxpiBuffer;
}

MxpiBuffer* MxpiBufferManager::CreateHostBufferWithMemory(const InputParam& inputParam)
{
    LogDebug << "Begin to create host buffer. Memory size("
             << inputParam.dataSize << "), deviceId(" << inputParam.deviceId << ").";
    APP_ERROR ret = MxBase::MemoryHelper::CheckDataSize(inputParam.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "create device buffer, input allocate memory size abnormal"
                 << GetErrorInfo(ret);
        return nullptr;
    }

    auto mxpiBuffer = CreateMxpiBuffer();
    if (mxpiBuffer == nullptr) {
        LogError << "create device buffer failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    if (!inputParam.key.empty()) {
        HostBufferMetaDataPtr dataPtr;
        ret = CreateMetaDataPtr(inputParam, dataPtr, true, CreateBufferMemoryType::HOST_WITH_MEM);
        if (ret != APP_ERR_OK) {
            LogError << "Create mxpiVisionList pointer or string object failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            return nullptr;
        }
        AddMetadataInfo(RESERVED_VISION_LIST_KEY, *mxpiBuffer, std::static_pointer_cast<void>(dataPtr.ptr));
        MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
        ret = mxpiMetadataManager.AddProtoMetadata(inputParam.key,
                                                   std::static_pointer_cast<void>(dataPtr.mxpiVisionList));
        if (ret != APP_ERR_OK) {
            LogError << "AddProtoMetadata failed." << GetErrorInfo(ret);
            DestroyBuffer(mxpiBuffer);
            return nullptr;
        }
    }
    AddFrameInfoToMetadata(FRAME_INFO_KEY, *mxpiBuffer, inputParam.mxpiFrameInfo);
    LogDebug << "End to create device buffer end. Memory size("
             << inputParam.dataSize << "), deviceId(" << inputParam.deviceId << ").";
    return mxpiBuffer;
}

MxpiFrame MxpiBufferManager::GetHostDataInfo(MxpiBuffer& mxpiBuffer)
{
    LogDebug << "Begin to get host buffer data information.";
    MxpiFrame mxpiFrame;
    // MxpiFrame->MxpiFrameInfo
    MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
    auto frameMetadata = mxpiMetadataManager.GetMetadata(FRAME_INFO_KEY);
    if (frameMetadata != nullptr) {
        auto mxpiFrameInfoTmp = std::static_pointer_cast<MxpiFrameInfo>(frameMetadata);
        auto mxpiFrameInfo = mxpiFrame.mutable_frameinfo();
        mxpiFrameInfo->CopyFrom(*mxpiFrameInfoTmp);
    }

    GstMapInfo gstMapInfo;
    gst_buffer_map((GstBuffer*) mxpiBuffer.buffer, &gstMapInfo, GST_MAP_READ);
    auto visionMetadataKey = mxpiMetadataManager.GetMetadata(RESERVED_VISION_LIST_KEY);
    if (visionMetadataKey == nullptr && gstMapInfo.size == 0) {
        gst_buffer_unmap((GstBuffer*) mxpiBuffer.buffer, &gstMapInfo);
        return mxpiFrame;
    }

    // MxpiFrame->MxpiVision
    auto mxpiVisionList = mxpiFrame.mutable_visionlist();
    auto mxpiVision = mxpiVisionList->add_visionvec();
    if (mxpiVision == nullptr) {
        LogError << "mxpiVision pointer is nullptr. Get host buffer data information Failed."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        gst_buffer_unmap((GstBuffer*) mxpiBuffer.buffer, &gstMapInfo);
        return mxpiFrame;
    }
    if (visionMetadataKey != nullptr) {
        auto keyTmp = std::static_pointer_cast<std::string>(visionMetadataKey);
        std::string key(keyTmp->data(), keyTmp->size());
        auto visionMetadata = mxpiMetadataManager.GetMetadata(key);
        if (visionMetadata != nullptr) {
            auto mxpiVisionListTmp = std::static_pointer_cast<MxpiVisionList>(visionMetadata);
            if (mxpiVisionListTmp->visionvec().size() > 0) {
                auto mxpiVisionInfo = mxpiVision->mutable_visioninfo();
                mxpiVisionInfo->CopyFrom(mxpiVisionListTmp->visionvec(0).visioninfo());
            }
        }
    }

    if (gstMapInfo.size != 0) {
        auto mxpiVisionData = mxpiVision->mutable_visiondata();
        mxpiVisionData->set_dataptr(reinterpret_cast<uintptr_t>(gstMapInfo.data));
        mxpiVisionData->set_datasize(gstMapInfo.size);
        mxpiVisionData->set_memtype(MXPI_MEMORY_HOST);
    }

    gst_buffer_unmap((GstBuffer*) mxpiBuffer.buffer, &gstMapInfo);
    LogDebug << "End to get host buffer data information.";
    return mxpiFrame;
}

MxpiFrame MxpiBufferManager::GetDeviceDataInfo(MxpiBuffer& mxpiBuffer)
{
    LogDebug << "Begin to get device buffer data information.";
    MxpiFrame mxpiFrame;
    // MxpiFrame->MxpiFrameInfo
    MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
    auto metadata = mxpiMetadataManager.GetMetadata(FRAME_INFO_KEY);
    if (metadata != nullptr) {
        auto mxpiFrameInfoTmp = std::static_pointer_cast<MxpiFrameInfo>(metadata);
        auto mxpiFrameInfo = mxpiFrame.mutable_frameinfo();
        mxpiFrameInfo->CopyFrom(*mxpiFrameInfoTmp);
    }
    // MxpiFrame->MxpiVision
    metadata = mxpiMetadataManager.GetMetadata(RESERVED_VISION_LIST_KEY);
    if (metadata == nullptr) {
        LogError << "Metadata pointer is nullptr. Get device buffer data information failed."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return mxpiFrame;
    }
    auto keyTmp = std::static_pointer_cast<std::string>(metadata);
    std::string key(keyTmp->data(), keyTmp->size());
    metadata = mxpiMetadataManager.GetMetadata(key);
    if (metadata != nullptr) {
        auto mxpiVisionListTmp = std::static_pointer_cast<MxpiVisionList>(metadata);
        if (!mxpiVisionListTmp->visionvec().empty()) {
            auto mxpiVisionList = mxpiFrame.mutable_visionlist();
            auto mxpiVision = mxpiVisionList->add_visionvec();
            if (mxpiVision == nullptr) {
                LogError << "mxpiVision pointer is nullptr. Get device buffer data information failed."
                            << GetErrorInfo(APP_ERR_COMM_FAILURE);
                return mxpiFrame;
            }
            auto mxpiVisionInfo = mxpiVision->mutable_visioninfo();
            mxpiVisionInfo->CopyFrom(mxpiVisionListTmp->visionvec(0).visioninfo());
            auto mxpiVisionData = mxpiVision->mutable_visiondata();
            mxpiVisionData->CopyFrom(mxpiVisionListTmp->visionvec(0).visiondata());
        }
    }
    LogDebug << "End to get device buffer data information.";
    return mxpiFrame;
}

APP_ERROR MxpiBufferManager::DestroyBuffer(MxpiBuffer* mxpiBuffer)
{
    LogDebug << "Begin to destroy buffer.";
    if (mxpiBuffer == nullptr) {
        LogDebug << "The mxpiBuffer is null.";
        return APP_ERR_OK;
    }
    if (mxpiBuffer->buffer == nullptr) {
        LogDebug << "The gstbuffer is null.";
        delete mxpiBuffer;
        mxpiBuffer = nullptr;
        return APP_ERR_OK;
    }
    int refcount = GST_MINI_OBJECT_REFCOUNT((GstBuffer*) mxpiBuffer->buffer);
    if (refcount > 0) {
        if (refcount == 1) {
            LogDebug << "buffer ref count(1), buffer will be destroy.";
        }
        gst_buffer_unref((GstBuffer*) mxpiBuffer->buffer);
    } else {
        LogWarn << "buffer ref count(refcount) is not valid.";
    }
    delete mxpiBuffer;
    mxpiBuffer = nullptr;
    LogDebug << "End to destroy buffer.";
    return APP_ERR_OK;
}

void MxpiBufferManager::AddFrameInfoToMetadata(const std::string& key, MxpiBuffer &mxpiBuffer,
                                               const MxpiFrameInfo& mxpiFrameInfo)
{
    LogDebug << "Begin to add MxpiFrameInfo on the metadata(" << key << ").";
    auto ptr = MxBase::MemoryHelper::MakeShared<MxpiFrameInfo>();
    if (ptr == nullptr) {
        LogError << "Create MxpiFrameInfo object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return;
    }
    MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
    if (mxpiMetadataManager.GetMetadata(key)) {
        LogWarn << "Metadata(" << key << ") already exits. The old key will be replaced by the new one.";
        mxpiMetadataManager.RemoveMetadata(key);
    }
    ptr->CopyFrom(mxpiFrameInfo);
    mxpiMetadataManager.AddProtoMetadata(key, ptr);
    LogDebug << "End to add MxpiFrameInfo on the metadata(" << key << ").";
}

void MxpiBufferManager::AddMetadataInfo(const std::string key, MxpiBuffer& mxpiBuffer, std::shared_ptr<void> ptr)
{
    LogDebug << "Begin to add metadata(" << key << ") on the buffer.";
    MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
    if (mxpiMetadataManager.GetMetadata(key)) {
        LogWarn << "Metadata(" << key << ") already exits. The old key will be replaced by the new one.";
        mxpiMetadataManager.RemoveMetadata(key);
    }
    mxpiMetadataManager.AddMetadata(key, ptr);
    LogDebug << "End to add metadata(" << key << ") on the buffer.";
}

std::shared_ptr<MxpiVisionList> MxpiBufferManager::CreateVisionList()
{
    LogDebug << "Begin to create vision list";

    std::shared_ptr<MxpiVisionList> mxpiVisionList = nullptr;
    auto mxpiVisionListPtr = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionListPtr == nullptr) {
        LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return nullptr;
    }
    mxpiVisionList.reset(mxpiVisionListPtr, g_deleteFuncMxpiVisionList);

    LogDebug << "End to create vision list";
    return mxpiVisionList;
}

bool MxpiBufferManager::IsDeviceUsing(const int& deviceId)
{
    bool result = false;
    MxBase::DeviceContext deviceContext;
    deviceContext.devId = deviceId;
    if (MxBase::DeviceManager::GetInstance()->GetCurrentDevice(deviceContext) == APP_ERR_OK) {
        if (deviceContext.devStatus == MxBase::DeviceContext::DeviceStatus::USING) {
            result = true;
        }
    }
    return result;
}

bool MxpiBufferManager::CopyDeviceMemory(MemoryData& memoryDataDst, const InputParam& inputParam)
{
    if (inputParam.dataSize != 0 && inputParam.ptrData != nullptr) {
        MemoryData memoryDataSrc(inputParam.ptrData, inputParam.dataSize,
            MemoryData::MEMORY_DEVICE, inputParam.deviceId);
        memoryDataDst.size = inputParam.dataSize;
        memoryDataDst.deviceId = inputParam.deviceId;
        memoryDataDst.type = MemoryData::MEMORY_DVPP;
        auto ret = MxBase::MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
        if (ret != APP_ERR_OK) {
            LogError << "Memory malloc and copy failed." << GetErrorInfo(ret);
            return false;
        }
    } else {
        LogInfo << "copy device memory failed, Memory size(0).";
    }
    return true;
}

bool MxpiBufferManager::CheckInputParam(const InputParam& inputParam)
{
    if (!IsDeviceUsing(inputParam.deviceId)) {
        LogError << "Get deviceId(" << inputParam.deviceId <<") status failed."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }

    APP_ERROR ret = MxBase::MemoryHelper::CheckDataSizeAllowZero(inputParam.dataSize);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to create buffer with invalid memory size:" << inputParam.dataSize << "."
                 << GetErrorInfo(ret);
        return false;
    }

    return true;
}

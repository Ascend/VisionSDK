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
 * Description: Smart Pointer Remover.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPI_DATATYPEDELETERV2_H
#define MXPI_DATATYPEDELETERV2_H

#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Common/Version.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"

namespace MxTools {
namespace {
    void SetDeviceID(MxBase::DeviceManager *deviceManager, MxBase::DeviceContext deviceContext, bool curDeviceIsOk)
    {
        if (curDeviceIsOk) {
            if (deviceManager == nullptr) {
                LogError << "Pointer deviceManager can not be nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
                return;
            }
            MxBase::DeviceContext deviceContextOld;
            APP_ERROR ret = deviceManager->GetCurrentDevice(deviceContextOld);
            if (ret == APP_ERR_OK && deviceContextOld.devId == deviceContext.devId) {
                return;
            }
            ret = deviceManager->SetDevice(deviceContext);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to set deviceID." << GetErrorInfo(ret);
            }
        }
    }
};

bool MatPtrDeleter(uint64_t dataptr, uint64_t matptr);

auto g_deleteFuncMxpiVisionList = [](MxpiVisionList* mxpiVisionList) {
    if (mxpiVisionList == nullptr) {
        LogWarn << "g_deleteFuncMxpiVisionList: Pointer mxpiVisionList is nullptr.";
        return;
    }
    MxBase::DeviceManager *deviceManager = MxBase::DeviceManager::GetInstance();
    MxBase::DeviceContext deviceContextOld;
    bool curDeviceIsOk = true;
    APP_ERROR ret = deviceManager->GetCurrentDevice(deviceContextOld);
    if (ret != APP_ERR_OK) {
        curDeviceIsOk = false;
    }
    for (const auto& it : mxpiVisionList->visionvec()) {
        const auto& mxpiVisionData = it.visiondata();
        bool deleteMat = MatPtrDeleter((uint64_t)mxpiVisionData.dataptr(), (uint64_t)mxpiVisionData.matptr());
        if (deleteMat) {
            continue;
        }
        MxBase::DeviceContext deviceContext;
        deviceContext.devId = mxpiVisionData.deviceid();
        MxBase::MemoryData memoryData;
        memoryData.size = mxpiVisionData.datasize();
        memoryData.deviceId = mxpiVisionData.deviceid();
        memoryData.type = (MxBase::MemoryData::MemoryType)mxpiVisionData.memtype();
        memoryData.ptrData = (void *)mxpiVisionData.dataptr();
        if (memoryData.type == MxBase::MemoryData::MemoryType::MEMORY_DEVICE ||
            memoryData.type == MxBase::MemoryData::MemoryType::MEMORY_DVPP) {
            SetDeviceID(deviceManager, deviceContext, true);
        }
        if (memoryData.ptrData == nullptr || memoryData.size == 0) {
            continue;
        }
        ret = MxBase::MemoryHelper::MxbsFree(memoryData);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to free memory of MxpiVisionList, pointer("
                     << memoryData.ptrData << "), type(" << memoryData.type << ")."
                     << GetErrorInfo(ret);
            SetDeviceID(deviceManager, deviceContextOld, curDeviceIsOk);
            continue;
        }
    }
    SetDeviceID(deviceManager, deviceContextOld, curDeviceIsOk);
    delete mxpiVisionList;
    mxpiVisionList = nullptr;
};

auto g_deleteFuncMxpiTensorPackageList = [](MxpiTensorPackageList* mxpiTensorPackageList) {
    if (mxpiTensorPackageList == nullptr) {
        LogWarn << "g_deleteFuncMxpiTensorPackageList: Pointer mxpiTensorPackageList is nullptr.";
        return;
    }
    MxBase::DeviceManager *deviceManager = MxBase::DeviceManager::GetInstance();
    MxBase::DeviceContext deviceContextOld;
    bool curDeviceIsOk = true;
    APP_ERROR ret = deviceManager->GetCurrentDevice(deviceContextOld);
    if (ret != APP_ERR_OK) {
        curDeviceIsOk = false;
    }
    for (const auto& itTensorPackage : mxpiTensorPackageList->tensorpackagevec()) {
        for (const auto& itTensor : itTensorPackage.tensorvec()) {
            MxBase::MemoryData memoryData;
            MxBase::DeviceContext deviceContext;
            deviceContext.devId = itTensor.deviceid();
            memoryData.size = itTensor.tensordatasize();
            memoryData.deviceId = itTensor.deviceid();
            memoryData.type = (MxBase::MemoryData::MemoryType)itTensor.memtype();
            memoryData.ptrData = (void *)itTensor.tensordataptr();
            if (memoryData.type == MxBase::MemoryData::MemoryType::MEMORY_DEVICE ||
                memoryData.type == MxBase::MemoryData::MemoryType::MEMORY_DVPP) {
                SetDeviceID(deviceManager, deviceContext, true);
            }
            if (memoryData.ptrData == nullptr || memoryData.size == 0) {
                continue;
            }
            ret = MxBase::MemoryHelper::MxbsFree(memoryData);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to free memory of MxpiTensorPackageList, pointer("
                         << memoryData.ptrData << "), type(" << memoryData.type << ")."
                         << GetErrorInfo(ret);
                SetDeviceID(deviceManager, deviceContextOld, curDeviceIsOk);
                continue;
            }
        }
    }
    SetDeviceID(deviceManager, deviceContextOld, curDeviceIsOk);
    delete mxpiTensorPackageList;
    mxpiTensorPackageList = nullptr;
};
}
#endif // MXPI_DATATYPEDELETERV2_H

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
 * Description: Device-side management.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/DeviceManager/DeviceManager.h"
#include <iostream>
#include <memory>
#include <cstring>
#include "acl/acl.h"
#include "acl/acl_base.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "ResourceManager/DvppPool/DvppPool.h"

namespace {
const std::string MXBASE_LIB_PATH = "/lib/libmxbase.so";

bool UserPermissionCheck()
{
    std::string sdkHome = "/usr/local";
    auto sdkHomeEnv = std::getenv("MX_SDK_HOME");
    if (sdkHomeEnv) {
        sdkHome = sdkHomeEnv;
    }
    std::string libFile = sdkHome + MXBASE_LIB_PATH;
    std::string regularFilePath;
    if (!MxBase::FileUtils::RegularFilePath(libFile, regularFilePath)) {
        LogError << "Failed to regular file path of libmxbase.so." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    if (!MxBase::FileUtils::IsFileValid(regularFilePath, true)) {
        LogError << "Invalid lib file!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    return true;
}
}

namespace MxBase {
const std::string SOCNAME310 = "Ascend310";
const std::string SOCNAME310P = "Ascend710";
const std::string SOCNAME310P3 = "Ascend310P";
const std::string SOCNAME310B = "Ascend310B";
const std::string SOCNAME910B = "Ascend910B";

DeviceManager::~DeviceManager()
{
    std::lock_guard<std::mutex> lock(mtx_);
    if ((initCounter_ != 0) && !destroyFlag_) {
        LogDebug << "DeviceManager Acl Resource is not released.";
        return;
    }
}

DeviceManager* DeviceManager::GetInstance()
{
    static DeviceManager deviceManager;
    return &deviceManager;
}

bool DeviceManager::IsInitDevices() const
{
    bool status = initCounter_ > 0;
    return status;
}

bool DeviceManager::IsAscend310()
{
    auto socName = aclrtGetSocName();
    if (socName == nullptr) {
        LogError << "aclrtGetSocName() return nullptr. please check your environment "
                 << "variables and compilation options to make sure you use the correct ACL library."
                 << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return false;
    }
    return socName == SOCNAME310;
}

bool DeviceManager::IsAscend310B()
{
    auto socName = aclrtGetSocName();
    if (socName == nullptr) {
        LogError << "aclrtGetSocName() return nullptr. please check your environment "
                 << "variables and compilation options to make sure you use the correct ACL library."
                 << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return false;
    }
    // if socName contains substring "Ascend310B", return true
    if (std::strstr(socName, SOCNAME310B.c_str()) != nullptr) {
        return true;
    } else {
        return false;
    }
}

bool DeviceManager::IsAscend310P()
{
    auto socName = aclrtGetSocName();
    if (socName == nullptr) {
        LogError << "aclrtGetSocName() return nullptr. please check your environment "
                 << "variables and compilation options to make sure you use the correct ACL library."
                 << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return false;
    }

    // if socName contains substring "Ascend710" or "Ascend310P", return true
    if (std::strstr(socName, SOCNAME310P.c_str()) != nullptr) {
        return true;
    } else if (std::strstr(socName, SOCNAME310P3.c_str()) != nullptr) {
        return true;
    } else {
        return false;
    }
}

bool DeviceManager::IsAtlas800IA2()
{
    auto socName = aclrtGetSocName();
    if (socName == nullptr) {
        LogError << "aclrtGetSocName() return nullptr. please check your environment "
                 << "variables and compilation options to make sure you use the correct ACL library."
                 << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return false;
    }
    // if socName contains substring "Ascend910B", return true
    if (std::strstr(socName, SOCNAME910B.c_str()) != nullptr) {
        return true;
    } else {
        return false;
    }
}

std::string DeviceManager::GetSocName()
{
    std::string socName;
    if (IsAscend310()) {
        socName = "310";
    } else if (IsAscend310B()) {
        socName = "310B";
    } else if (IsAtlas800IA2()) {
        socName = "Atlas 800I A2";
    } else {
        socName = "310P";
    }
    return socName;
}

/**
 * @description: initialize all devices
 * @param: configFilePath
 * @return: init_device_result
 */
APP_ERROR DeviceManager::InitDevices(std::string)
{
    std::lock_guard<std::mutex> lock(mtx_);
    initCounter_++;
    if (initCounter_ > 1) {
        return APP_ERR_OK;
    }
    
    if (!UserPermissionCheck()) {
        LogError << "Check user permission failed, please check the owner of process."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }

    APP_ERROR ret = aclInit(nullptr);
    if (ret != APP_ERR_OK) {
        initCounter_ = 0;
        LogError << "Failed to initialize all devices." << GetErrorInfo(ret, "aclInit");
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = aclrtGetDeviceCount(&deviceCount_);
    if (ret != APP_ERR_OK) {
        initCounter_ = 0;
        aclFinalize();
        LogError << "Failed to get all devices count." << GetErrorInfo(ret, "aclrtGetDeviceCount");
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

/**
 * @description: release all devices
 * @param: void
 * @return: destroy_devices_result
 */
APP_ERROR DeviceManager::DestroyDevices()
{
    APP_ERROR deInitRet = APP_ERR_OK;
    APP_ERROR ret = APP_ERR_OK;
    if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
        ret = DvppPool::GetInstance().DeInit();
        if (ret != APP_ERR_OK) {
            LogError << "DvppPool DeInit failed." << GetErrorInfo(ret);
            deInitRet = APP_ERR_COMM_FAILURE;
        }
    }
    std::lock_guard<std::mutex> lock(mtx_);

    if (initCounter_ == 0) {
        return APP_ERR_COMM_OUT_OF_RANGE;
    }

    if (destroyFlag_) {
        LogDebug << "Destroy devices has already been executed.";
        return APP_ERR_OK;
    }

    LogInfo << "DestroyDevices begin";
    for (auto item : contexts_) {
        LogDebug << "destroy device:" << item.first;
        ret = aclrtDestroyContext(item.second.get());
        if (ret != APP_ERR_OK) {
            LogError << "aclrtDestroyContext failed." << GetErrorInfo(ret, "aclrtDestroyContext");
            deInitRet = APP_ERR_ACL_FAILURE;
        }
        ret = aclrtResetDevice(item.first);
        if (ret != APP_ERR_OK) {
            LogError << "aclrtResetDevice failed." << GetErrorInfo(ret, "aclrtResetDevice");
            deInitRet = APP_ERR_ACL_FAILURE;
        }
        LogDebug << "aclrtDestroyContext finished!";
    }
    contexts_.clear();

    ret = aclFinalize();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to release all devices." << GetErrorInfo(ret, "aclFinalize");
        deInitRet = APP_ERR_ACL_FAILURE;
    }

    destroyFlag_ = true;
    LogInfo << "DestroyDevices finished!";
    return deInitRet;
}

/**
 * @description: get all devices count
 * @param: deviceCount
 * @return: get_devices_count_result
 */
APP_ERROR DeviceManager::GetDevicesCount(uint32_t& deviceCount)
{
    deviceCount = deviceCount_;
    return APP_ERR_OK;
}

/**
 * @description: get current running device
 * @param: device
 * @return: get_current_device_result
 */
APP_ERROR DeviceManager::GetCurrentDevice(DeviceContext& device)
{
    std::lock_guard<std::mutex> lock(mtx_);
    aclrtContext currentContext = nullptr;
    APP_ERROR ret = aclrtGetCurrentContext(&currentContext);
    if (ret != APP_ERR_OK) {
        // In multi-threaded mode, the context may not been set. So Log level use Debug
        LogDebug << "Cannot get the context for current thread.";
        return APP_ERR_ACL_FAILURE;
    }

    for (const auto &item : contexts_) {
        if (item.second.get() == currentContext) {
            device.devId = item.first;
            device.devStatus = DeviceContext::DeviceStatus::USING;
            return APP_ERR_OK;
        }
    }

    return APP_ERR_COMM_FAILURE;
}

/**
 * @description: set one device for running
 * @param: device
 * @return: set_device_result
 */
APP_ERROR DeviceManager::SetDevice(DeviceContext device)
{
    if (!IsInitDevices()) {
        APP_ERROR ret = InitDevices();
        if (ret != APP_ERR_OK) {
            LogError << "Init device failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    std::lock_guard<std::mutex> lock(mtx_);
    auto deviceId = device.devId;
    APP_ERROR ret = CheckDeviceId(deviceId);
    if (ret != APP_ERR_OK) {
        LogError << "Device Id is out of range[0, " << deviceCount_ << ")." << GetErrorInfo(ret);
        return ret;
    }
    if (contexts_.find(device.devId) == contexts_.end()) {
        APP_ERROR ret = aclrtSetDevice(device.devId);
        if (ret != APP_ERR_OK) {
            LogError << "Calling aclrtSetDevice failed." << GetErrorInfo(ret, "aclrtSetDevice");
            return APP_ERR_ACL_FAILURE;
        }
        aclrtContext newContext = nullptr;
        ret = aclrtCreateContext(&newContext, device.devId);
        if (ret != APP_ERR_OK) {
            LogError << "Calling aclrtCreateContext failed." << GetErrorInfo(ret, "aclrtCreateContext");
            return APP_ERR_ACL_FAILURE;
        }
        std::shared_ptr<void> context(newContext, [] (void *) {});
        contexts_[device.devId] = context;
    } else {
        APP_ERROR ret = aclrtSetCurrentContext(contexts_[deviceId].get());
        if (ret != APP_ERR_OK) {
            LogError << "Calling aclrtSetCurrentContext failed."
                     << GetErrorInfo(ret, "aclrtSetCurrentContext");
            return APP_ERR_ACL_FAILURE;
        }
    }
    return APP_ERR_OK;
}

/**
 * @description: check device id
 * @param: deviceId
 * @return: check_device_id_result
 */
APP_ERROR DeviceManager::CheckDeviceId(int32_t deviceId)
{
    if (deviceId < 0) {
        LogError << "deviceId(" << deviceId << ") should be larger than or equal to 0."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (deviceId > (int32_t)deviceCount_ - 1) {
        LogError << "deviceId(" << deviceId << ") should be less than or equal to deviceCount(" << deviceCount_ << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}
}  // namespace MxBase
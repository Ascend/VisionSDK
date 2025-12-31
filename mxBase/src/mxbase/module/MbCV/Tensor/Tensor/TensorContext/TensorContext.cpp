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
 * Description: Set the context to the corresponding device ID.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "TensorContext.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Log/Log.h"
#include "acl/acl.h"

namespace MxBase {
TensorContext::TensorContext()
{
    if (!DeviceManager::GetInstance()->IsInitDevices()) {
        APP_ERROR ret = DeviceManager::GetInstance()->InitDevices();
        if (ret != APP_ERR_OK) {
            LogError << "DeviceManager InitDevices failed." << GetErrorInfo(ret);
            return;
        }
    }
}

TensorContext::~TensorContext()
{
}

APP_ERROR TensorContext::SetContext(const uint32_t &deviceId)
{
    DeviceContext device = {};
    device.devId = deviceId;
    APP_ERROR ret = DeviceManager::GetInstance()->SetDevice(device);
    if (ret != APP_ERR_OK) {
        LogError << "SetDevice failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

std::shared_ptr<TensorContext> TensorContext::GetInstance()
{
    static std::shared_ptr<TensorContext> tensorContext = MemoryHelper::MakeShared<TensorContext>();
    if (tensorContext == nullptr) {
        LogError << "Create TensorContext object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
    return tensorContext;
}
}

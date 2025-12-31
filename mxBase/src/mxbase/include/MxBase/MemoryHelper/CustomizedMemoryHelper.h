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
 * Description: customized memory helper.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */
#ifndef CUSTOMIZED_MEMORY_HELPER_H
#define CUSTOMIZED_MEMORY_HELPER_H

#include "MemoryHelper.h"
#include "acl/ops/acl_dvpp.h"
#include "acl/dvpp/hi_dvpp.h"

namespace MxBase {

    __attribute__ ((unused)) static g_dvppMallocFuncType g_dvppMallocFunP = nullptr;
    __attribute__ ((unused)) static g_dvppFreeFuncType g_dvppFreeFunP = nullptr;
    __attribute__ ((unused)) static g_deviceMallocFuncType g_deviceMallocFunP = nullptr;
    __attribute__ ((unused)) static g_deviceFreeFuncType g_deviceFreeFunP = nullptr;

    bool CheckDVPPMemoryFuncPair();
    bool CheckDeviceMemoryFuncPair();

    APP_ERROR DVPPMemoryMallocFunc(hi_u32 dev_id, hi_void **dev_ptr, hi_u64 size);
    APP_ERROR DVPPMemoryFreeFunc(hi_void *dev_ptr);
    APP_ERROR DeviceMemoryMallocFunc(void** devPtr, unsigned int size, MxMemMallocPolicy policy);
    APP_ERROR DeviceMemoryFreeFunc(void *devPtr);
    APP_ERROR aclrtMallocAdapter(void** devPtr, unsigned int size, MxMemMallocPolicy policy);

}

#endif // CUSTOMIZED_MEMORY_HELPER_H

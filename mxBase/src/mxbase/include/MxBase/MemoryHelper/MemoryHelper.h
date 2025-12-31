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
 * Description: Memory Management on the Host and Device Sides.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MEMORY_HELPER_H
#define MEMORY_HELPER_H

#include <memory>

#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Common/HiddenAttr.h"
#include "MxBase/Asynchron/AscendStream.h"

namespace MxBase {
    typedef enum MxMemMallocPolicy {
        MX_MEM_MALLOC_HUGE_FIRST,
        MX_MEM_MALLOC_HUGE_ONLY,
        MX_MEM_MALLOC_NORMAL_ONLY,
        MX_MEM_MALLOC_HUGE_FIRST_P2P,
        MX_MEM_MALLOC_HUGE_ONLY_P2P,
        MX_MEM_MALLOC_NORMAL_ONLY_P2P,
        MX_MEM_TYPE_LOW_BAND_WIDTH = 0x0100,
        MX_MEM_TYPE_HIGH_BAND_WIDTH = 0x1000,
    } MxMemMallocPolicy;

    using g_dvppMallocFuncType = APP_ERROR (*)(unsigned int, void**, unsigned long long);
    using g_dvppFreeFuncType = APP_ERROR (*)(void*);
    using g_deviceMallocFuncType = APP_ERROR (*)(void**, unsigned int, MxMemMallocPolicy);
    using g_deviceFreeFuncType = APP_ERROR (*)(void*);

    APP_ERROR DVPPMallocFuncHookReg(g_dvppMallocFuncType pFun);
    APP_ERROR DVPPFreeFuncHookReg(g_dvppFreeFuncType pFun);
    APP_ERROR DeviceMallocFuncHookReg(g_deviceMallocFuncType pFun);
    APP_ERROR DeviceFreeFuncHookReg(g_deviceFreeFuncType pFun);

    struct SDK_AVAILABLE_FOR_OUT MemoryData {
    enum MemoryType {
        MEMORY_HOST = 0,
        MEMORY_DEVICE,
        MEMORY_DVPP,
        MEMORY_HOST_MALLOC,
        MEMORY_HOST_NEW
    };

    MemoryData() = default;

    MemoryData(size_t size, MemoryType type = MEMORY_HOST, int32_t deviceId = 0)
        : size(size), deviceId(deviceId), type(type) {}

    MemoryData(void* ptrData, size_t size, MemoryType type = MEMORY_HOST, int32_t deviceId = 0)
        : ptrData(ptrData), size(size), deviceId(deviceId), type(type) {}

    void* ptrData = nullptr;
    size_t size;
    int32_t deviceId;
    MemoryType type;
    APP_ERROR (*free)(void*) = nullptr;
};

class SDK_AVAILABLE_FOR_OUT MemoryHelper {
public:
    // malloc memory

    static APP_ERROR MxbsMalloc(MemoryData& data);
    static APP_ERROR MxbsFree(MemoryData& data);
    static APP_ERROR MxbsMemset(MemoryData& data, int32_t value, size_t count);
    static APP_ERROR MxbsMemset(MemoryData &data, int32_t value, size_t count, AscendStream &stream);
    static APP_ERROR MxbsMemcpy(MemoryData& dest, const MemoryData& src, size_t count);
    static APP_ERROR MxbsMallocAndCopy(MemoryData& dest, const MemoryData& src);

    static APP_ERROR SetMaxDataSize(long size);
    static APP_ERROR CheckDataSize(long size);
    static APP_ERROR CheckDataSizeAllowZero(long size);

    template<typename T, typename... Args>
    static std::shared_ptr<T> MakeShared(Args && ... args)
    {
        std::shared_ptr<T> ptr = nullptr;
        try {
            ptr = std::make_shared<T>(args...);
        } catch (const std::exception& ex) {
        }
        return ptr;
    }
};
}  // namespace MxBase
#endif

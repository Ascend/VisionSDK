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

#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "acl/acl.h"
#include "acl/ops/acl_dvpp.h"
#include "acl/dvpp/hi_dvpp.h"
#include "MxBase/Log/Log.h"
#include "dvpp/securec.h"

namespace {
    constexpr long MAX_MALLOC_SIZE = 4294967296;
    constexpr long MIN_MALLOC_SIZE = 1;
    constexpr long DEFAULT_MAX_DATA_SIZE = 4294967296;
}

namespace MxBase {
using MemoryDataFreeFuncPointer = APP_ERROR (*)(void*);

static long g_defaultMaxDataSize = DEFAULT_MAX_DATA_SIZE;

static inline bool IsMemoryOnDevice(MemoryData::MemoryType memType)
{
    return (memType == MemoryData::MEMORY_DEVICE || memType == MemoryData::MEMORY_DVPP);
}

static inline bool IsMemoryOnHost(MemoryData::MemoryType memType)
{
    return (memType == MemoryData::MEMORY_HOST || memType == MemoryData::MEMORY_HOST_MALLOC ||
            memType == MemoryData::MEMORY_HOST_NEW);
}

static inline bool IsHostToDevice(const MemoryData &dest, const MemoryData &src)
{
    return (IsMemoryOnDevice(dest.type) && IsMemoryOnHost(src.type));
}

static inline bool IsDeviceToDevice(const MemoryData &dest, const MemoryData &src)
{
    return (IsMemoryOnDevice(dest.type) && IsMemoryOnDevice(src.type));
}

static inline bool IsHostToHost(const MemoryData &dest, const MemoryData &src)
{
    return (IsMemoryOnHost(dest.type) && IsMemoryOnHost(src.type));
}

static inline bool IsDeviceToHost(const MemoryData &dest, const MemoryData &src)
{
    return (IsMemoryOnHost(dest.type) && IsMemoryOnDevice(src.type));
}

APP_ERROR DVPPMallocFuncHookReg(g_dvppMallocFuncType pFun)
{
    LogDebug << "Register the hook function, use customized dvpp malloc function";
    MxBase::g_dvppMallocFunP = pFun;
    return APP_ERR_OK;
}

APP_ERROR DVPPFreeFuncHookReg(g_dvppFreeFuncType pFun)
{
    LogDebug << "Register the hook function, use customized dvpp free function";
    MxBase::g_dvppFreeFunP = pFun;
    return APP_ERR_OK;
}

APP_ERROR DeviceMallocFuncHookReg(g_deviceMallocFuncType pFun)
{
    LogDebug << "Register the hook function, use customized device malloc function";
    MxBase::g_deviceMallocFunP = pFun;
    return APP_ERR_OK;
}

APP_ERROR DeviceFreeFuncHookReg(g_deviceFreeFuncType pFun)
{
    LogDebug << "Register the hook function, use customized device free function";
    MxBase::g_deviceFreeFunP = pFun;
    return APP_ERR_OK;
}

APP_ERROR FreeFuncDelete(void* ptr)
{
    delete[] (int8_t*)ptr;
    return APP_ERR_OK;
}

APP_ERROR FreeFuncCFree(void* ptr)
{
    free(ptr);
    return APP_ERR_OK;
}

bool CheckDVPPMemoryFuncPair()
{
    if (MxBase::g_dvppMallocFunP == hi_mpi_dvpp_malloc && MxBase::g_dvppFreeFunP != hi_mpi_dvpp_free) {
        LogWarn << "DVPP memory allocate and free function do not match, default function will be used";
        return false;
    }
    if (MxBase::g_dvppMallocFunP != hi_mpi_dvpp_malloc && MxBase::g_dvppFreeFunP == hi_mpi_dvpp_free) {
        LogWarn << "DVPP memory allocate and free function do not match, default function will be used";
        return false;
    }
    if (MxBase::g_dvppMallocFunP == nullptr || MxBase::g_dvppFreeFunP == nullptr) {
        return false;
    }
    return true;
}

bool CheckDeviceMemoryFuncPair()
{
    if (MxBase::g_deviceMallocFunP == aclrtMallocAdapter && MxBase::g_deviceFreeFunP != aclrtFree) {
        LogWarn << "Device memory allocate and free function do not match, default function will be used";
        return false;
    }
    if (MxBase::g_deviceMallocFunP != aclrtMallocAdapter && MxBase::g_deviceFreeFunP == aclrtFree) {
        LogWarn << "Device memory allocate and free function do not match, default function will be used";
        return false;
    }
    if (MxBase::g_deviceMallocFunP == nullptr || MxBase::g_deviceFreeFunP == nullptr) {
        return false;
    }
    return true;
}

APP_ERROR DeviceMemoryMallocFunc(void** devPtr, unsigned int size, MxMemMallocPolicy policy)
{
    if (!CheckDeviceMemoryFuncPair()) {
        LogDebug << "use default aclrtMalloc/aclrtFree";
        MxBase::g_deviceMallocFunP = aclrtMallocAdapter;
        MxBase::g_deviceFreeFunP = aclrtFree;
    }
    APP_ERROR ret = MxBase::g_deviceMallocFunP(devPtr, size, policy);
    if (ret != APP_ERR_OK) {
        if (MxBase::g_deviceMallocFunP == aclrtMallocAdapter) {
            LogError << "Failed to malloc device memory" << GetErrorInfo(ret, "aclrtMalloc");
            return APP_ERR_ACL_BAD_ALLOC;
        }
        LogError << "Failed to use customized malloc function to allocate device memory"
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    return ret;
}

APP_ERROR aclrtMallocAdapter(void** devPtr, unsigned int size, MxMemMallocPolicy policy)
{
    aclrtMemMallocPolicy aclPolicy = static_cast<aclrtMemMallocPolicy>(policy);
    APP_ERROR ret = aclrtMalloc(devPtr, size, aclPolicy);
    if (ret != APP_ERR_OK) {
        LogError << "aclrtMallocAdapter, Failed to malloc device memory" << GetErrorInfo(ret, "aclrtMalloc");
        return APP_ERR_ACL_BAD_ALLOC;
    }
    return ret;
}

APP_ERROR DeviceMemoryFreeFunc(void *devPtr)
{
    if (!CheckDeviceMemoryFuncPair()) {
        LogDebug << "use default aclrtMalloc/aclrtFree";
        MxBase::g_deviceMallocFunP = aclrtMallocAdapter;
        MxBase::g_deviceFreeFunP = aclrtFree;
    }
    APP_ERROR ret = MxBase::g_deviceFreeFunP(devPtr);
    if (ret != APP_ERR_OK) {
        if (MxBase::g_deviceFreeFunP == aclrtFree) {
            LogError << "Failed to free device memory" << GetErrorInfo(ret, "aclrtFree");
            return APP_ERR_COMM_FREE_MEM;
        }
        LogError << "Failed to use customized free function to free device memory"
                 << GetErrorInfo(APP_ERR_COMM_FREE_MEM);
        return APP_ERR_COMM_FREE_MEM;
    }
    return ret;
}

APP_ERROR DVPPMemoryMallocFunc(hi_u32 dev_id, hi_void **dev_ptr, hi_u64 size)
{
    if (!CheckDVPPMemoryFuncPair()) {
        LogDebug << "use default hi_mpi_dvpp_malloc/hi_mpi_dvpp_free";
        MxBase::g_dvppMallocFunP = hi_mpi_dvpp_malloc;
        MxBase::g_dvppFreeFunP = hi_mpi_dvpp_free;
    }
    APP_ERROR ret = MxBase::g_dvppMallocFunP(dev_id, dev_ptr, size);
    if (ret != APP_ERR_OK) {
        if (MxBase::g_dvppMallocFunP == hi_mpi_dvpp_malloc) {
            LogError << "Failed to malloc dvpp memory" << GetErrorInfo(ret, "hi_mpi_dvpp_malloc");
            return APP_ERR_COMM_ALLOC_MEM;
        }
        LogError << "Failed to use customized malloc function to allocate dvpp memory"
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    return ret;
}

APP_ERROR DVPPMemoryFreeFunc(hi_void *dev_ptr)
{
    if (!CheckDVPPMemoryFuncPair()) {
        LogDebug << "use default hi_mpi_dvpp_malloc/hi_mpi_dvpp_free";
        MxBase::g_dvppMallocFunP = hi_mpi_dvpp_malloc;
        MxBase::g_dvppFreeFunP = hi_mpi_dvpp_free;
    }
    APP_ERROR ret = MxBase::g_dvppFreeFunP(dev_ptr);
    if (ret != APP_ERR_OK) {
        if (MxBase::g_dvppFreeFunP == hi_mpi_dvpp_free) {
            LogError << "Failed to free dvpp memory" << GetErrorInfo(ret, "hi_mpi_dvpp_free");
            return APP_ERR_COMM_FREE_MEM;
        }
        LogError << "Failed to use customized malloc function to free dvpp memory"
                 << GetErrorInfo(APP_ERR_COMM_FREE_MEM);
        return APP_ERR_COMM_FREE_MEM;
    }
    return ret;
}

static APP_ERROR MallocThenSet(MemoryData& data)
{
    APP_ERROR ret = APP_ERR_OK;
    data.ptrData = malloc(data.size);
    if (data.ptrData == nullptr) {
        LogError << "Malloc failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    int res = memset_s(data.ptrData, data.size, 0, data.size);
    if (res != 0) {
        LogError << "Memset failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        free(data.ptrData);
        data.ptrData = nullptr;
        return APP_ERR_COMM_ALLOC_MEM;
    }
    data.free = (MemoryDataFreeFuncPointer)FreeFuncCFree;
    return ret;
}

static APP_ERROR MallocByDataType(MemoryData& data)
{
    APP_ERROR ret = APP_ERR_OK;
    switch (data.type) {
        case MemoryData::MEMORY_HOST:
            ret = aclrtMallocHost(&(data.ptrData), data.size);
            data.free = aclrtFreeHost;
            if (ret != APP_ERR_OK) {
                LogError << "Fail to malloc host memory." << GetErrorInfo(ret, "aclrtMallocHost");
                return APP_ERR_ACL_BAD_ALLOC;
            }
            return ret;
        case MemoryData::MEMORY_DEVICE:
            ret = DeviceMemoryMallocFunc(&(data.ptrData), data.size, MX_MEM_MALLOC_HUGE_FIRST);
            data.free = DeviceMemoryFreeFunc;
            if (ret != APP_ERR_OK) {
                LogError << "Fail to malloc device memory." << GetErrorInfo(ret);
                return APP_ERR_ACL_BAD_ALLOC;
            }
            return ret;
        case MemoryData::MEMORY_DVPP:
            if (DeviceManager::IsAscend310() || DeviceManager::IsAscend310B()) {
                ret = acldvppMalloc(&(data.ptrData), data.size);
                data.free = acldvppFree;
            } else if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
                ret = DVPPMemoryMallocFunc(data.deviceId, &(data.ptrData), data.size);
                data.free = DVPPMemoryFreeFunc;
            }
            if (ret != APP_ERR_OK) {
                LogError << "Fail to malloc dvpp memory." << GetErrorInfo(ret, "acldvppMalloc");
                return APP_ERR_ACL_BAD_ALLOC;
            }
            return ret;
        case MemoryData::MEMORY_HOST_MALLOC:
            ret = MallocThenSet(data);
            if (ret != APP_ERR_OK) {
                LogError << "Fail to malloc host memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            }
            return ret;
        case MemoryData::MEMORY_HOST_NEW:
            data.ptrData = (void *) (new(std::nothrow) int8_t[data.size]);
            if (data.ptrData == nullptr) {
                LogError << "Fail to new host memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return APP_ERR_COMM_ALLOC_MEM;
            }
            data.free = (MemoryDataFreeFuncPointer) FreeFuncDelete;
            return ret;
        default:
            LogError << "The module type is not defined." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
    }
}

static APP_ERROR Malloc(MemoryData& data)
{
    APP_ERROR ret = MemoryHelper::CheckDataSize(data.size);
    if (ret != APP_ERR_OK) {
        LogError << "Data size out of range." << GetErrorInfo(ret);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ret = MallocByDataType(data);
    if (ret != APP_ERR_OK) {
        LogError  << "Malloc ptrData failed." << GetErrorInfo(ret);
        data.ptrData = nullptr;
        return APP_ERR_ACL_BAD_ALLOC;
    }
    return ret;
}

static APP_ERROR Free(MemoryData& data)
{
    if (data.ptrData == nullptr) {
        LogError << "Free failed, ptrData is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    APP_ERROR ret = APP_ERR_OK;
    int8_t *ptrData = nullptr;
    switch (data.type) {
        case MemoryData::MEMORY_HOST:
            ret = aclrtFreeHost(data.ptrData);
            break;
        case MemoryData::MEMORY_DEVICE:
            ret = DeviceMemoryFreeFunc(data.ptrData);
            break;
        case MemoryData::MEMORY_DVPP:
            if (DeviceManager::IsAscend310() || DeviceManager::IsAscend310B()) {
                ret = acldvppFree(data.ptrData);
            } else if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
                ret = DVPPMemoryFreeFunc(data.ptrData);
            } else {
                LogError << "Free memory failed or the device is neither 310 nor 310Pro."
                         << GetErrorInfo(APP_ERR_ACL_BAD_FREE);
                ret = APP_ERR_ACL_BAD_FREE;
            }
            break;
        case MemoryData::MEMORY_HOST_MALLOC:
            free(data.ptrData);
            ret = APP_ERR_OK;
            break;
        case MemoryData::MEMORY_HOST_NEW:
            ptrData = (int8_t*)data.ptrData;
            delete[] ptrData;
            ptrData = nullptr;
            ret = APP_ERR_OK;
            break;
        default:
            LogError << "Free failed, the module type is not defined, data type:" << data.type
                     << "." << GetErrorInfo(APP_ERR_ACL_BAD_FREE);
            return APP_ERR_ACL_BAD_FREE;
    }
    if (ret != APP_ERR_OK) {
        LogError << "Free ptrData failed."
                 << GetErrorInfo(ret, "aclrtFreeHost or aclrtFree");
        return APP_ERR_ACL_BAD_FREE;
    }
    data.ptrData = nullptr;
    return ret;
}

static APP_ERROR Memset(MemoryData& data, int32_t value, size_t count)
{
    if (data.ptrData == nullptr) {
        LogError << "Memset failed, ptrData is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    APP_ERROR ret = MemoryHelper::CheckDataSize(data.size);
    if (ret != APP_ERR_OK) {
        LogError << "Data size out of range." << GetErrorInfo(ret);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (count == 0 || count > data.size) {
        LogError << "Please check data size: " << data.size << ", or count: " << count << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ret = aclrtMemset(data.ptrData, data.size, value, count);
    if (ret != APP_ERR_OK) {
        LogError << "Memset ptrData failed." << GetErrorInfo(ret, "aclrtMemset");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

static APP_ERROR Memset(MemoryData& data, int32_t value, size_t count, AscendStream &stream)
{
    if (data.ptrData == nullptr) {
        LogError << "Memset failed, ptrData is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    APP_ERROR ret = MemoryHelper::CheckDataSize(data.size);
    if (ret != APP_ERR_OK) {
        LogError << "Data size out of range." << GetErrorInfo(ret);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (count == 0 || count > data.size) {
        LogError << "Please check data size: " << data.size << ", or count: " << count << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ret = aclrtMemsetAsync(data.ptrData, data.size, value, count, stream.stream);
    if (ret != APP_ERR_OK) {
        LogError << "Memset ptrData failed." << GetErrorInfo(ret, "aclrtMemsetAsync");
        return APP_ERR_ACL_FAILURE;
    }
    if (stream.isDefault_) {
        ret = stream.Synchronize();
        if (ret != APP_ERR_OK) {
            LogError << "Stream synchronize failed when memset." << GetErrorInfo(ret);
        }
    }
    return ret;
}

static APP_ERROR Memcpy(MemoryData& dest, const MemoryData& src, size_t count)
{
    if (dest.ptrData == nullptr || src.ptrData == nullptr) {
        LogError << "Memcpy failed, ptrData is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if ((count == 0) || (count > dest.size)) {
        LogError << "Please check dest size: " << dest.size << ", or count: " << count << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = APP_ERR_COMM_INVALID_PARAM;
    if (IsDeviceToHost(dest, src)) {
        ret = aclrtMemcpy(dest.ptrData, dest.size, src.ptrData, count, ACL_MEMCPY_DEVICE_TO_HOST);
    } else if (IsHostToHost(dest, src)) {
        ret = aclrtMemcpy(dest.ptrData, dest.size, src.ptrData, count, ACL_MEMCPY_HOST_TO_HOST);
    } else if (IsDeviceToDevice(dest, src)) {
        ret = aclrtMemcpy(dest.ptrData, dest.size, src.ptrData, count, ACL_MEMCPY_DEVICE_TO_DEVICE);
    } else if (IsHostToDevice(dest, src)) {
        ret = aclrtMemcpy(dest.ptrData, dest.size, src.ptrData, count, ACL_MEMCPY_HOST_TO_DEVICE);
    } else {
        LogError << "No category of memory copy matched." << GetErrorInfo(APP_ERR_ACL_BAD_COPY);
        return APP_ERR_ACL_BAD_COPY ;
    }
    if (ret != APP_ERR_OK) {
        LogError << "Memcpy ptrData failed." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY;
    }
    return ret;
}

APP_ERROR MemoryHelper::MxbsMallocAndCopy(MemoryData& dest, const MemoryData& src)
{
    if (src.ptrData == nullptr) {
        LogError << "MxbsMallocAndCopy failed, ptrData of src is nullptr."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (src.size == 0) {
        LogError << "Please check src size: " << src.size << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = Malloc(dest);
    if (ret != APP_ERR_OK) {
        LogError << "MxbsMallocAndCopy function malloc ptrData failed." << GetErrorInfo(ret);
        return ret;
    }

    ret = Memcpy(dest, src, src.size);
    if (ret != APP_ERR_OK) {
        LogError << "MxbsMallocAndCopy function memcpy failed." << GetErrorInfo(ret);
        ret = dest.free(dest.ptrData);
        if (ret != APP_ERR_OK) {
            LogError << "MxbsMallocAndCopy function free failed." << GetErrorInfo(ret);
        }
        dest.ptrData = nullptr;
        return APP_ERR_ACL_BAD_COPY;
    }
    return ret;
}

APP_ERROR MemoryHelper::MxbsMalloc(MemoryData& data)
{
    return Malloc(data);
}

APP_ERROR MemoryHelper::MxbsFree(MemoryData& data)
{
    return Free(data);
}

APP_ERROR MemoryHelper::MxbsMemset(MemoryData &data, int32_t value, size_t count)
{
    return Memset(data, value, count);
}

APP_ERROR MemoryHelper::MxbsMemset(MemoryData &data, int32_t value, size_t count, AscendStream &stream)
{
    return Memset(data, value, count, stream);
}

APP_ERROR MemoryHelper::MxbsMemcpy(MemoryData& dest, const MemoryData& src, size_t count)
{
    return Memcpy(dest, src, count);
}

APP_ERROR MemoryHelper::SetMaxDataSize(long size)
{
    if ((size > MAX_MALLOC_SIZE) || (size < MIN_MALLOC_SIZE)) {
        LogError << "Input data size(" << size << ") out of range["
                 << MIN_MALLOC_SIZE << "," << MAX_MALLOC_SIZE << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    g_defaultMaxDataSize = size;

    return APP_ERR_OK;
}

APP_ERROR MemoryHelper::CheckDataSize(long size)
{
    if ((size > g_defaultMaxDataSize) || (size < MIN_MALLOC_SIZE)) {
        LogError << "Input data size(" << size << ") out of range["
                 << MIN_MALLOC_SIZE << "," << g_defaultMaxDataSize << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return APP_ERR_OK;
}

APP_ERROR MemoryHelper::CheckDataSizeAllowZero(long size)
{
    if ((size > g_defaultMaxDataSize) || (size < 0)) {
        LogError << "Input data size(" << size << ") out of range[0,"
                 << g_defaultMaxDataSize << "]." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return APP_ERR_OK;
}
}  // namespace MxBase

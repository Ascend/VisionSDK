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
 * Description: Implements the buffer copy function of the Host/Device,Host/Host,Device/Device.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "TensorBuffer.h"
#include "MbCV/Tensor/Tensor/TensorContext/TensorContext.h"
#include "MxBase/Log/Log.h"
#include "dvpp/securec.h"

namespace MxBase {
APP_ERROR TensorBuffer::SetContext() const
{
    if (IsDevice()) {
        APP_ERROR ret = TensorContext::GetInstance()->SetContext(deviceId);
        if (ret != APP_ERR_OK) {
            LogError << "SetContext failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR TensorBuffer::TensorBufferMalloc(TensorBuffer &buffer)
{
    // SetContext
    APP_ERROR ret = buffer.SetContext();
    if (ret != APP_ERR_OK) {
        LogError << "SetContext failed." << GetErrorInfo(ret);
        return ret;
    }

    // Malloc
    MxBase::MemoryData memorydata(buffer.size, buffer.type, buffer.deviceId);
    ret = MemoryHelper::MxbsMalloc(memorydata);
    if (ret != APP_ERR_OK) {
        LogError << "MemoryHelper::MxbsMalloc failed." << GetErrorInfo(ret);
        return ret;
    }
    const TensorBuffer buf = buffer;
    auto deleter = [buf] (void *p) {
        buf.SetContext();
        MxBase::MemoryData memoryData(p, buf.size, buf.type, buf.deviceId);
        MxBase::MemoryHelper::MxbsFree(memoryData);
    };
    buffer.data.reset(memorydata.ptrData, deleter);
    return APP_ERR_OK;
}

APP_ERROR TensorBuffer::CheckCopyValid(const TensorBuffer &buffer1, const TensorBuffer &buffer2)
{
    if (buffer1.size != buffer2.size) {
        LogError << "The param1 data size(" << buffer1.size << ") not match to param2 size(" << buffer2.size << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (buffer1.data.get() == nullptr) {
        LogError << "The param1 pointer is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    if (buffer2.data.get() == nullptr) {
        LogError << "The param2 pointer is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return APP_ERR_OK;
}

TensorBufferCopyType TensorBuffer::GetBufferCopyType(const TensorBuffer &buffer1, const TensorBuffer &buffer2)
{
    if (buffer1.IsHost() && buffer2.IsHost()) {
        return TensorBufferCopyType::HOST_AND_HOST;
    }

    if (buffer1.IsDevice() && buffer2.IsHost()) {
        return TensorBufferCopyType::HOST_AND_DEVICE;
    }

    if (buffer1.IsHost() && buffer2.IsDevice()) {
        return TensorBufferCopyType::HOST_AND_DEVICE;
    }

    if (buffer1.deviceId != buffer2.deviceId) {
        return TensorBufferCopyType::DEVICE_AND_DIFF_DEVICE;
    }
    return TensorBufferCopyType::DEVICE_AND_SAME_DEVICE;
}

APP_ERROR TensorBuffer::CopyBetweenHost(TensorBuffer &dst, const TensorBuffer &src)
{
    APP_ERROR ret = CheckCopyValid(dst, src);
    if (ret != APP_ERR_OK) {
        LogError << "CheckCopyValid failed." << GetErrorInfo(ret);
        return ret;
    }
    if (dst.IsDevice() || src.IsDevice()) {
        LogError << "Data is not in host, src(" << src.type << "), dst(" << dst.type << ")."
                 << GetErrorInfo(APP_ERR_ACL_BAD_COPY);
        return APP_ERR_ACL_BAD_COPY;
    }
    std::copy(static_cast<uint8_t *>(src.data.get()), static_cast<uint8_t *>(src.data.get()) + src.size,
              static_cast<uint8_t *>(dst.data.get()));
    return APP_ERR_OK;
}

APP_ERROR TensorBuffer::CopyBetweenHostDevice(TensorBuffer &dst, const TensorBuffer &src)
{
    APP_ERROR ret = CheckCopyValid(dst, src);
    if (ret != APP_ERR_OK) {
        LogError << "CheckCopyValid failed." << GetErrorInfo(ret);
        return ret;
    }

    if (dst.IsHost() && src.IsDevice()) {
        ret = src.SetContext();
        if (ret != APP_ERR_OK) {
            LogError << "SetContext failed." << GetErrorInfo(ret);
            return ret;
        }
    } else if (dst.IsDevice() && src.IsHost()) {
        ret = dst.SetContext();
        if (ret != APP_ERR_OK) {
            LogError << "SetContext failed." << GetErrorInfo(ret);
            return ret;
        }
    } else {
        LogWarn << "dst and src tensor buffer are both on host or both on device.";
    }

    MemoryData dstMemory(dst.data.get(), dst.size, dst.type, dst.deviceId);
    MemoryData srcMemory(src.data.get(), src.size, src.type, src.deviceId);
    ret = MemoryHelper::MxbsMemcpy(dstMemory, srcMemory, dst.size);
    if (ret != APP_ERR_OK) {
        LogError << "MemoryHelper::MxbsMemcpy failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR TensorBuffer::CopyBetweenSameDevice(TensorBuffer &dst, const TensorBuffer &src)
{
    APP_ERROR ret = CheckCopyValid(dst, src);
    if (ret != APP_ERR_OK) {
        LogError << "CheckCopyValid failed." << GetErrorInfo(ret);
        return ret;
    }
    if (dst.deviceId != src.deviceId) {
        LogWarn << "Tow tensorBuffer have different device id.";
        return CopyBetweenDiffDevice(dst, src);
    }
    ret = src.SetContext();
    if (ret != APP_ERR_OK) {
        LogError << "SetContext failed." << GetErrorInfo(ret);
        return ret;
    }

    MemoryData dstMemory(dst.data.get(), dst.size, dst.type, dst.deviceId);
    MemoryData srcMemory(src.data.get(), src.size, src.type, src.deviceId);
    ret = MemoryHelper::MxbsMemcpy(dstMemory, srcMemory, dst.size);
    if (ret != APP_ERR_OK) {
        LogError << "MemoryHelper::MxbsMemcpy failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR TensorBuffer::CopyBetweenDiffDevice(TensorBuffer &dst, const TensorBuffer &src)
{
    APP_ERROR ret = CheckCopyValid(dst, src);
    if (ret != APP_ERR_OK) {
        LogError << "CheckCopyValid failed." << GetErrorInfo(ret);
        return ret;
    }
    if (dst.deviceId == src.deviceId) {
        LogWarn << "Tow tensorBuffer have same device id.";
        return CopyBetweenSameDevice(dst, src);
    }
    TensorBuffer host(src.size);
    ret = TensorBuffer::TensorBufferMalloc(host);
    if (ret != APP_ERR_OK) {
        LogError << "TensorBuffer::TensorBufferMalloc failed." << GetErrorInfo(ret);
        return ret;
    }

    ret = CopyBetweenHostDevice(host, src);
    if (ret != APP_ERR_OK) {
        LogError << "CopyBetweenHostDevice failed." << GetErrorInfo(ret);
        return ret;
    }

    ret = CopyBetweenHostDevice(dst, host);
    if (ret != APP_ERR_OK) {
        LogError << "CopyBetweenHostDevice failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR TensorBuffer::TensorBufferCopy(TensorBuffer &dst, const TensorBuffer &src)
{
    APP_ERROR ret = CheckCopyValid(dst, src);
    if (ret != APP_ERR_OK) {
        LogError << "CheckCopyValid failed." << GetErrorInfo(ret);
        return ret;
    }

    auto copyType = GetBufferCopyType(dst, src);
    // host to host
    if (copyType == TensorBufferCopyType::HOST_AND_HOST) {
        ret = CopyBetweenHost(dst, src);
        if (ret != APP_ERR_OK) {
            LogError << "CopyBetweenHost failed." << GetErrorInfo(ret);
            return ret;
        }
        return APP_ERR_OK;
    }
    // device to host or host to device
    if (copyType == TensorBufferCopyType::HOST_AND_DEVICE) {
        ret = CopyBetweenHostDevice(dst, src);
        if (ret != APP_ERR_OK) {
            LogError << "CopyBetweenHostDevice failed." << GetErrorInfo(ret);
            return ret;
        }
        return APP_ERR_OK;
    }

    // devie a to device a
    if (copyType == TensorBufferCopyType::DEVICE_AND_SAME_DEVICE) {
        ret = CopyBetweenSameDevice(dst, src);
        if (ret != APP_ERR_OK) {
            LogError << "CopyBetweenSameDevice failed." << GetErrorInfo(ret);
            return ret;
        }
        return ret;
    }
    // device a to device b
    ret = CopyBetweenDiffDevice(dst, src);
    if (ret != APP_ERR_OK) {
        LogError << "CopyBetweenDiffDevice failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}
}
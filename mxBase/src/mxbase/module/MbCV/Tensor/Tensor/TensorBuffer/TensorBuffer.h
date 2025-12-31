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

#ifndef TENSOR_BUFFER_H
#define TENSOR_BUFFER_H

#include <vector>
#include <string>
#include <memory>


#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxBase {
enum class TensorBufferCopyType {
    HOST_AND_HOST = 0,
    HOST_AND_DEVICE,
    DEVICE_AND_SAME_DEVICE,
    DEVICE_AND_DIFF_DEVICE
};

class TensorBuffer {
public:
    TensorBuffer() {}
    ~TensorBuffer() {}
    TensorBuffer(uint32_t size, MemoryData::MemoryType type, int32_t deviceId)
        : size(size), type(type), deviceId(deviceId) {}
    TensorBuffer(uint32_t size, int32_t deviceId)
        : size(size), deviceId(deviceId) {}
    explicit TensorBuffer(uint32_t size) : size(size) {}
    TensorBuffer(void *ptr, uint32_t size, MemoryData::MemoryType type = MemoryData::MemoryType::MEMORY_HOST_NEW,
        int32_t deviceId = -1)
        : size(size), type(type), deviceId(deviceId)
    {
        data.reset(ptr, [] (void *) {});
    }

    bool IsDevice() const
    {
        if (type == MemoryData::MemoryType::MEMORY_DEVICE || type == MemoryData::MemoryType::MEMORY_DVPP) {
            return true;
        }
        return false;
    }

    bool IsHost() const
    {
        return !IsDevice();
    }

    APP_ERROR SetContext() const;
    static APP_ERROR TensorBufferMalloc(TensorBuffer &buffer);
    static APP_ERROR TensorBufferCopy(TensorBuffer &dst, const TensorBuffer &src);

    static TensorBufferCopyType GetBufferCopyType(const TensorBuffer &buffer1, const TensorBuffer &buffer2);
    static APP_ERROR CheckCopyValid(const TensorBuffer &buffer1, const TensorBuffer &buffer2);
    static APP_ERROR CopyBetweenHost(TensorBuffer &dst, const TensorBuffer &src);
    static APP_ERROR CopyBetweenHostDevice(TensorBuffer &dst, const TensorBuffer &src);
    static APP_ERROR CopyBetweenSameDevice(TensorBuffer &dst, const TensorBuffer &src);
    static APP_ERROR CopyBetweenDiffDevice(TensorBuffer &dst, const TensorBuffer &src);

private:
    friend class TensorBase;
    uint32_t size = 0;
    MemoryData::MemoryType type = MemoryData::MemoryType::MEMORY_HOST_NEW;
    int32_t deviceId = -1;
    std::shared_ptr<void> data = nullptr;
};
}

#endif
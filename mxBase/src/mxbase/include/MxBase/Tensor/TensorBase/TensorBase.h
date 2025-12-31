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
 * Description: Constructing Tensor Class and Providing Its Attribute Interfaces.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef TENSOR_CORE_H
#define TENSOR_CORE_H

#include <iostream>
#include <type_traits>
#include <limits>

#include "TensorDataType.h"
#include "MxBase/E2eInfer/Rect/Rect.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Log/Log.h"

template<typename T, typename U, typename = typename std::enable_if<std::numeric_limits<U>::is_integer>::type,
    typename = typename std::enable_if<std::numeric_limits<U>::is_integer>::type>
void GetIndices(std::vector<T> &indices, U value)
{
    indices.push_back((T)value);
    return;
}
template<typename T, typename U, typename... Ix,
    typename = typename std::enable_if<std::numeric_limits<T>::is_integer>::type,
    typename = typename std::enable_if<std::numeric_limits<U>::is_integer>::type>
void GetIndices(std::vector<T> &indices, U value, Ix... idxs)
{
    indices.push_back((T)value);
    GetIndices(indices, idxs...);
}

template<typename T, typename U, typename = typename std::enable_if<std::numeric_limits<T>::is_integer>::type,
typename = typename std::enable_if<std::numeric_limits<U>::is_integer>::type>
void GetIndices(std::vector<T> &indices, std::vector<U> values)
{
    indices.clear();
    for (auto v : values) {
        indices.push_back((T)v);
    }
    return;
}

namespace MxBase {

std::string GetTensorDataTypeDesc(TensorDataType type);

class TensorBuffer;
class TensorShape;
class TensorBase {
public:
    TensorBase();
    virtual ~TensorBase() = default;
    TensorBase(const TensorBase &tensor) = default;
    // tensor构造函数
    TensorBase(const MemoryData &memoryData, const bool &isBorrowed,
        const std::vector<uint32_t> &shape, const TensorDataType &type);
    TensorBase(const std::vector<uint32_t> &shape, const TensorDataType &type,
        const MemoryData::MemoryType &bufferType, const int32_t &deviceId);
    TensorBase(const std::vector<uint32_t> &shape, const TensorDataType &type, const int32_t &deviceId);
    TensorBase(const std::vector<uint32_t> &shape, const TensorDataType &type);
    TensorBase(const std::vector<uint32_t> &shape);
    TensorBase& operator=(const TensorBase &other);
    static APP_ERROR TensorBaseMalloc(TensorBase &tensor);
    static APP_ERROR TensorBaseCopy(TensorBase &dst, const TensorBase &src);
    // 获取tensor部署的设备类型
    MemoryData::MemoryType GetTensorType() const;
    // buffer记录的数据量
    size_t GetSize() const;
    // buffer 字节数据量
    size_t GetByteSize() const;
    // tensor 的shape
    std::vector<uint32_t> GetShape() const;
    // Set tensor shape
    APP_ERROR SetShape(std::vector<uint32_t> shape);
    std::vector<uint32_t> GetStrides() const;
    // tensor 的 device
    int32_t GetDeviceId() const;
    // tensor 数据类型
    TensorDataType GetDataType() const;
    uint32_t GetDataTypeSize() const;
    // 判断是否在Host
    bool IsHost() const;
    // 判断是否在Device
    bool IsDevice() const;
    // 获取tensor指针
    void* GetBuffer() const;
    APP_ERROR GetBuffer(void *&ptr, const std::vector<uint32_t> &indices) const;
    // host to device
    APP_ERROR ToDevice(int32_t deviceId);
    // host to dvpp
    APP_ERROR ToDvpp(int32_t deviceId);
    // device to host
    APP_ERROR ToHost();
    static APP_ERROR BatchConcat(const std::vector<TensorBase> &inputs, TensorBase &output);
    static APP_ERROR BatchStack(const std::vector<TensorBase> &inputs, TensorBase &output);
    // 组batch
    static APP_ERROR BatchVector(const std::vector<TensorBase> &inputs, TensorBase &output,
        const bool &keepDims = false);
    // 详细信息
    std::string GetDesc();
    APP_ERROR CheckTensorValid() const;

    // set valid roi for tensor
    APP_ERROR SetValidRoi(Rect rect);
    // get valid roi for tensor
    Rect GetValidRoi() const;

    template<typename... Param>
    static APP_ERROR CreateTensorBase(TensorBase &tensor, Param... params)
    {
        tensor = TensorBase(params...);
        auto ret = tensor.CheckTensorValid();
        if (ret != APP_ERR_OK) {
            LogError << "Tensor is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return ret;
        }
        return APP_ERR_OK;
    }

    template<typename T, typename... Ix>
    APP_ERROR GetBuffer(T* &value, Ix... index) const
    {
        std::vector<uint32_t> indices = {};
        GetIndices(indices, index...);
        void *ptr = nullptr;
        APP_ERROR ret = GetBuffer(ptr, indices);
        if (ret != APP_ERR_OK) {
            LogError << "GetBuffer failed." << GetErrorInfo(ret);
            return ret;
        }
        value = (T*)ptr;
        return APP_ERR_OK;
    }

    template<typename T, typename... Ix>
    APP_ERROR GetValue(T &value, Ix... index) const
    {
        if (!IsHost()) {
            LogError << "This tensor is not in host. you should deploy it to host."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        if (sizeof(T) != GetDataTypeSize()) {
            LogError << "Output date type is not match to tensor date type(" << GetDataType() << ")"
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        T *ptr = nullptr;
        APP_ERROR ret = GetBuffer(ptr, index...);
        if (ret != APP_ERR_OK) {
            LogError << "GetBuffer failed." << GetErrorInfo(ret);
            return ret;
        }
        if (ptr == nullptr) {
            LogError << "Value ptr is nullptr" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        value = *ptr;
        return APP_ERR_OK;
    }

private:
    static APP_ERROR BatchMalloc(const std::vector<TensorBase> &inputs, TensorBase &output,
        const std::vector<uint32_t> &batchShape);
    static APP_ERROR CheckBatchTensors(const std::vector<TensorBase> &inputs, const bool &checkFirstDim);
    APP_ERROR MallocAndCopyToDevice(int32_t deviceId, MemoryData::MemoryType memoryType);
private:
    std::shared_ptr<TensorBuffer> buffer_ = nullptr;
    std::shared_ptr<TensorShape> shape_ = nullptr;
    TensorDataType dataType_ = TENSOR_DTYPE_UINT8;
};
}
#endif


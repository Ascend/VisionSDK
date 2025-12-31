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

#include "MxBase/Tensor/TensorBase/TensorBase.h"
#include <map>
#include <algorithm>
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MbCV/Tensor/Tensor/TensorShape.h"
#include "MbCV/Tensor/Tensor/TensorBuffer/TensorBuffer.h"

namespace {
const uint32_t ZERO_BYTE = 0;
const uint32_t ONE_BYTE = 1;
const uint32_t TWO_BYTE = 2;
const uint32_t FOUR_BYTE = 4;
const uint32_t EIGHT_BYTE = 8;

const std::map<MxBase::TensorDataType, uint32_t> DATA_TYPE_TO_BYTE_SIZE_MAP = {
    {MxBase::TENSOR_DTYPE_UNDEFINED, ZERO_BYTE},
    {MxBase::TENSOR_DTYPE_UINT8, ONE_BYTE},
    {MxBase::TENSOR_DTYPE_INT8, ONE_BYTE},
    {MxBase::TENSOR_DTYPE_UINT16, TWO_BYTE},
    {MxBase::TENSOR_DTYPE_INT16, TWO_BYTE},
    {MxBase::TENSOR_DTYPE_UINT32, FOUR_BYTE},
    {MxBase::TENSOR_DTYPE_INT32, FOUR_BYTE},
    {MxBase::TENSOR_DTYPE_UINT64, EIGHT_BYTE},
    {MxBase::TENSOR_DTYPE_INT64, EIGHT_BYTE},
    {MxBase::TENSOR_DTYPE_FLOAT16, TWO_BYTE},
    {MxBase::TENSOR_DTYPE_FLOAT32, FOUR_BYTE},
    {MxBase::TENSOR_DTYPE_DOUBLE64, EIGHT_BYTE},
    {MxBase::TENSOR_DTYPE_BOOL, ONE_BYTE}
};

const std::map<MxBase::TensorDataType, std::string> DATA_TYPE_TO_STRING_MAP = {
    {MxBase::TENSOR_DTYPE_UNDEFINED, "undefined"},
    {MxBase::TENSOR_DTYPE_UINT8, "uint8"},
    {MxBase::TENSOR_DTYPE_INT8, "int8"},
    {MxBase::TENSOR_DTYPE_UINT16, "uint16"},
    {MxBase::TENSOR_DTYPE_INT16, "int16"},
    {MxBase::TENSOR_DTYPE_UINT32, "uint32"},
    {MxBase::TENSOR_DTYPE_INT32, "int32"},
    {MxBase::TENSOR_DTYPE_UINT64, "uint64"},
    {MxBase::TENSOR_DTYPE_INT64, "int64"},
    {MxBase::TENSOR_DTYPE_FLOAT16, "float16"},
    {MxBase::TENSOR_DTYPE_FLOAT32, "float32"},
    {MxBase::TENSOR_DTYPE_DOUBLE64, "double64"},
    {MxBase::TENSOR_DTYPE_BOOL, "bool"}
};
}
static uint32_t GetDataTypeByteSize(const MxBase::TensorDataType &dtype)
{
    const auto iter = DATA_TYPE_TO_BYTE_SIZE_MAP.find(dtype);
    if (iter != DATA_TYPE_TO_BYTE_SIZE_MAP.end()) {
        return iter->second;
    }
    return ZERO_BYTE;
}
namespace MxBase {
std::string GetTensorDataTypeDesc(TensorDataType type)
{
    if (DATA_TYPE_TO_STRING_MAP.find(type) != DATA_TYPE_TO_STRING_MAP.end()) {
        return DATA_TYPE_TO_STRING_MAP.find(type)->second;
    }
    return DATA_TYPE_TO_STRING_MAP.at(TENSOR_DTYPE_UNDEFINED);
}

TensorBase& TensorBase::operator=(const TensorBase &other)
{
    if (this == &other) {
        return *this;
    }
    buffer_ = other.buffer_;
    shape_ = other.shape_;
    dataType_ = other.dataType_;
    return *this;
}

// tensor构造函数
TensorBase::TensorBase()
{
    shape_ = std::make_shared<TensorShape>();
    buffer_ = std::make_shared<TensorBuffer>();
}

TensorBase::TensorBase(const MemoryData &memoryData, const bool &isBorrowed, const std::vector<uint32_t> &shape,
    const TensorDataType &type) : dataType_(type)
{
    shape_ = std::make_shared<TensorShape>(shape);
    uint32_t bytes = 0;
    if (DATA_TYPE_TO_BYTE_SIZE_MAP.find(type) != DATA_TYPE_TO_BYTE_SIZE_MAP.end()) {
        bytes = DATA_TYPE_TO_BYTE_SIZE_MAP.find(type)->second;
    } else {
        LogError << "Incorrect TensorDataType, should be in the range [-1, 12] but now it is " << static_cast<int>(type)
                 << "." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    if (memoryData.size != shape_->GetSize() * bytes) {
        LogError << "memoryData.size should match shape. Now memoryData.size is " << memoryData.size
                 << ", but the size of the data required for the input shape is " << shape_->GetSize() * bytes << "."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    buffer_ = std::make_shared<TensorBuffer>();
    buffer_->type = memoryData.type;
    buffer_->size = memoryData.size;
    buffer_->deviceId = (int32_t)memoryData.deviceId;
    if (isBorrowed) {
        auto deleter = [] (void *) {};
        buffer_->data.reset(memoryData.ptrData, deleter);
    } else {
        const TensorBuffer buffer = *buffer_;
        auto deleter = [buffer] (void *p) {
            buffer.SetContext();
            MxBase::MemoryData memoryData(p, buffer.size, buffer.type, buffer.deviceId);
            MxBase::MemoryHelper::MxbsFree(memoryData);
        };
        buffer_->data.reset(memoryData.ptrData, deleter);
    }
}

TensorBase::TensorBase(const std::vector<uint32_t> &shape, const TensorDataType &type,
    const MemoryData::MemoryType &bufferType, const int32_t &deviceId) : dataType_(type)
{
    shape_ = std::make_shared<TensorShape>(shape);
    uint32_t bytes = 0;
    if (DATA_TYPE_TO_BYTE_SIZE_MAP.find(type) != DATA_TYPE_TO_BYTE_SIZE_MAP.end()) {
        bytes = DATA_TYPE_TO_BYTE_SIZE_MAP.find(type)->second;
    } else {
        LogError << "Incorrect TensorDataType, should be in the range [-1, 12] but now it is " << static_cast<int>(type)
                 << "." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    buffer_ = std::make_shared<TensorBuffer>(shape_->GetSize() * bytes, bufferType, deviceId);
}

TensorBase::TensorBase(const std::vector<uint32_t> &shape, const TensorDataType &type) : dataType_(type)
{
    shape_ = std::make_shared<TensorShape>(shape);
    uint32_t bytes = 0;
    if (DATA_TYPE_TO_BYTE_SIZE_MAP.find(type) != DATA_TYPE_TO_BYTE_SIZE_MAP.end()) {
        bytes = DATA_TYPE_TO_BYTE_SIZE_MAP.find(type)->second;
    } else {
        LogError << "Incorrect TensorDataType, should be in the range [-1, 12] but now it is " << static_cast<int>(type)
                 << "." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    buffer_ = std::make_shared<TensorBuffer>(shape_->GetSize() * bytes);
}

TensorBase::TensorBase(const std::vector<uint32_t> &shape, const TensorDataType &type, const int32_t &deviceId)
    : dataType_(type)
{
    shape_ = std::make_shared<TensorShape>(shape);
    uint32_t bytes = 0;
    if (DATA_TYPE_TO_BYTE_SIZE_MAP.find(type) != DATA_TYPE_TO_BYTE_SIZE_MAP.end()) {
        bytes = DATA_TYPE_TO_BYTE_SIZE_MAP.find(type)->second;
    } else {
        LogError << "Incorrect TensorDataType, should be in the range [-1, 12] but now it is " << static_cast<int>(type)
                 << "." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    buffer_ = std::make_shared<TensorBuffer>(shape_->GetSize() * bytes, deviceId);
}

TensorBase::TensorBase(const std::vector<uint32_t> &shape)
{
    shape_ = std::make_shared<TensorShape>(shape);
    buffer_ = std::make_shared<TensorBuffer>(shape_->GetSize());
}

APP_ERROR TensorBase::TensorBaseMalloc(TensorBase &tensor)
{
    APP_ERROR ret = tensor.CheckTensorValid();
    if (ret != APP_ERR_OK) {
        LogError << "Tensor is invalid." << GetErrorInfo(ret);
        return ret;
    }
    ret = TensorBuffer::TensorBufferMalloc(*tensor.buffer_);
    if (ret != APP_ERR_OK) {
        LogError << "TensorBufferMalloc failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR TensorBase::TensorBaseCopy(TensorBase &dst, const TensorBase &src)
{
    APP_ERROR ret = dst.CheckTensorValid();
    if (ret != APP_ERR_OK) {
        LogError << "The dst tensor is invalid." << GetErrorInfo(ret);
        return ret;
    }

    ret = src.CheckTensorValid();
    if (ret != APP_ERR_OK) {
        LogError << "The src tensor is invalid." << GetErrorInfo(ret);
        return ret;
    }

    ret = TensorBuffer::TensorBufferCopy(*dst.buffer_, *src.buffer_);
    if (ret != APP_ERR_OK) {
        LogError << "TensorBufferCopy failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

// 判断是否在Host
bool TensorBase::IsHost() const
{
    if (buffer_.get() == nullptr) {
        return false;
    }
    return buffer_->IsHost();
}

// 判断是否在Device
bool TensorBase::IsDevice() const
{
    if (buffer_.get() == nullptr) {
        return false;
    }
    return buffer_->IsDevice();
}

APP_ERROR TensorBase::MallocAndCopyToDevice(int32_t deviceId, MemoryData::MemoryType memoryType)
{
    if (buffer_ == nullptr) {
        return APP_ERR_COMM_INVALID_POINTER;
    }
    DeviceContext device = {};
    DeviceManager::GetInstance()->GetCurrentDevice(device);
    if (GetDeviceId() == deviceId && buffer_->type == memoryType) {
        return APP_ERR_OK;
    }
    // device a to device b
    APP_ERROR ctxRet;
    TensorBuffer newBuffer(buffer_->size, memoryType, deviceId);
    APP_ERROR ret = TensorBuffer::TensorBufferMalloc(newBuffer);
    if (ret != APP_ERR_OK) {
        LogError << "TensorBuffer::TensorBufferMalloc failed." << GetErrorInfo(ret);
        ctxRet = MxBase::DeviceManager::GetInstance()->SetDevice(device);
        if (ctxRet != APP_ERR_OK) {
            LogError << "Fail to recover context in MallocAndCopyToDevice" << GetErrorInfo(ctxRet);
        }
        return ret;
    }
    ret = TensorBuffer::TensorBufferCopy(newBuffer, *buffer_);
    if (ret != APP_ERR_OK) {
        LogError << "TensorBuffer::TensorBufferCopy failed." << GetErrorInfo(ret);
        ctxRet = MxBase::DeviceManager::GetInstance()->SetDevice(device);
        if (ctxRet != APP_ERR_OK) {
            LogError << "Fail to recover context in MallocAndCopyToDevice" << GetErrorInfo(ctxRet);
        }
        return ret;
    }
    *buffer_ = newBuffer;
    DeviceContext dstDevice = {};
    dstDevice.devId = deviceId;
    ctxRet = MxBase::DeviceManager::GetInstance()->SetDevice(dstDevice);
    if (ctxRet != APP_ERR_OK) {
        LogError << "Fail to recover context in MallocAndCopyToDevice." << GetErrorInfo(ctxRet);
    }
    return APP_ERR_OK;
}
APP_ERROR TensorBase::ToDevice(int32_t deviceId)
{
    return MallocAndCopyToDevice(deviceId, MemoryData::MemoryType::MEMORY_DEVICE);
}

APP_ERROR TensorBase::ToDvpp(int32_t deviceId)
{
    return MallocAndCopyToDevice(deviceId, MemoryData::MemoryType::MEMORY_DVPP);
}

APP_ERROR TensorBase::ToHost()
{
    if (buffer_.get() == nullptr) {
        return APP_ERR_COMM_INVALID_POINTER;
    }

    if (IsHost()) {
        return APP_ERR_OK;
    }
    TensorBuffer host(buffer_->size);
    APP_ERROR ret = TensorBuffer::TensorBufferMalloc(host);
    if (ret != APP_ERR_OK) {
        LogError << "TensorBuffer::TensorBufferMalloc failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = TensorBuffer::TensorBufferCopy(host, *buffer_);
    if (ret != APP_ERR_OK) {
        LogError << "TensorBuffer::TensorBufferCopy failed." << GetErrorInfo(ret);
        return ret;
    }
    *buffer_ = host;
    return APP_ERR_OK;
}

// 获取tensor部署的设备类型
MemoryData::MemoryType TensorBase::GetTensorType() const
{
    if (buffer_.get() == nullptr) {
        return MemoryData::MemoryType::MEMORY_HOST_NEW;
    }
    return buffer_->type;
}

// buffer记录的数据量
size_t TensorBase::GetSize() const
{
    if (shape_.get() == nullptr) {
        return 0;
    }
    return shape_->GetSize();
}
// buffer 字节数据量
size_t TensorBase::GetByteSize() const
{
    if (shape_.get() == nullptr) {
        return 0;
    }
    uint32_t bytes = 0;
    if (DATA_TYPE_TO_BYTE_SIZE_MAP.find(dataType_) != DATA_TYPE_TO_BYTE_SIZE_MAP.end()) {
        bytes = DATA_TYPE_TO_BYTE_SIZE_MAP.find(dataType_)->second;
    }
    return shape_->GetSize() * bytes;
}
// tensor 的shape
std::vector<uint32_t> TensorBase::GetShape() const
{
    if (shape_.get() == nullptr) {
        return std::vector<uint32_t>();
    }
    return shape_->GetShape();
}

std::vector<uint32_t> TensorBase::GetStrides() const
{
    std::vector<uint32_t> strides = {};
    auto shape = GetShape();
    uint32_t stride = GetDataTypeByteSize(dataType_);
    for (int32_t i = static_cast<int>(shape.size()) - 1; i >= 0; i--) {
        strides.push_back(stride);
        stride *= shape[i];
    }
    std::reverse(std::begin(strides), std::end(strides));
    return strides;
}

// tensor 的 device
int32_t TensorBase::GetDeviceId() const
{
    if (buffer_.get() == nullptr) {
        return -1;
    }
    return buffer_->deviceId;
}
// tensor 数据类型
TensorDataType TensorBase::GetDataType() const
{
    return dataType_;
}

uint32_t TensorBase::GetDataTypeSize() const
{
    return GetDataTypeByteSize(dataType_);
}

// 获取tensor指针
void* TensorBase::GetBuffer() const
{
    if (buffer_.get() == nullptr) {
        return nullptr;
    }
    return buffer_->data.get();
}

APP_ERROR TensorBase::GetBuffer(void *&ptr, const std::vector<uint32_t> &indices) const
{
    auto strides = GetStrides();
    auto shape = GetShape();
    if (indices.size() > strides.size()) {
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    for (size_t i = 0; i < indices.size(); i++) {
        if (indices[i] >= shape[i]) {
            LogError << "The indices are out of range." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
            return APP_ERR_COMM_OUT_OF_RANGE;
        }
    }
    uint32_t offset = 0;
    for (uint32_t i = 0; i < indices.size(); i++) {
        offset += indices[i] * strides[i];
    }
    auto dataBuffer = GetBuffer();
    if (dataBuffer == nullptr) {
        LogError << "GetBuffer: The dataBuffer is empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    ptr = (void*)((uint8_t*)dataBuffer + offset);
    return APP_ERR_OK;
}

APP_ERROR TensorBase::CheckTensorValid() const
{
    if (buffer_.get() == nullptr) {
        LogError << "The tensor is empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_FAILURE;
    }
    if (IsDevice() && GetDeviceId() < 0) {
        LogError << "tensor type is (" << GetTensorDataTypeDesc(GetDataType())
                 << "). but device id is (" << GetDeviceId() << ")"
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR TensorBase::CheckBatchTensors(const std::vector<TensorBase> &inputs, const bool &checkFirstDim)
{
    auto checkFunc = [checkFirstDim] (const TensorBase &t1, const TensorBase &t2) {
        if (t1.GetShape().size() != t2.GetShape().size()) {
            LogError << "The dimension is not match (" << t1.GetShape().size() << ") vs (" << t2.GetShape().size()
                     << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (t1.GetDeviceId() != t2.GetDeviceId()) {
            LogError << "The deviceId is not match (" << t1.GetDeviceId() << ") vs (" << t2.GetDeviceId() << ")"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (t1.GetDataType() != t2.GetDataType()) {
            LogError << "data type is not match (" << GetTensorDataTypeDesc(t1.GetDataType()) << ") vs ("
                     << GetTensorDataTypeDesc(t2.GetDataType()) << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if (t1.GetTensorType() != t2.GetTensorType()) {
            LogError << "The memory type is not match (" << t1.GetTensorType() << ") vs (" << t2.GetTensorType()
                     << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        uint32_t startIndex = checkFirstDim ? 0 : 1;
        for (uint32_t i = startIndex; i < t1.GetShape().size(); i++) {
            if (t1.GetShape()[i] == t2.GetShape()[i]) {
                continue;
            }
            std::string shapeStr1 = "(";
            std::string shapeStr2 = "(";
            for (uint32_t j = 0; j < t1.GetShape().size(); j++) {
                shapeStr1 += std::to_string(t1.GetShape()[j]) + ",";
                shapeStr2 += std::to_string(t2.GetShape()[j]) + ",";
            }
            if (shapeStr1.size() > 0) {
                shapeStr1[shapeStr1.size() - 1] = ')';
                shapeStr2[shapeStr2.size() - 1] = ')';
            }
            LogError << "The tensor shape is not match " << shapeStr1 << " vs " << shapeStr2
                     << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        return true;
    };
    for (size_t i = 1; i < inputs.size(); i++) {
        if (!checkFunc(inputs[0], inputs[i])) {
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR TensorBase::BatchMalloc(const std::vector<TensorBase> &inputs, TensorBase &output,
    const std::vector<uint32_t> &batchShape)
{
    // malloc
    output = TensorBase(batchShape, inputs[0].GetDataType(), inputs[0].GetTensorType(), inputs[0].GetDeviceId());
    APP_ERROR ret = TensorBaseMalloc(output);
    if (ret != APP_ERR_OK) {
        LogError << "TensorBaseMalloc failed." << GetErrorInfo(ret);
        return ret;
    }
    // copy
    uint32_t offset = 0;
    for (uint32_t i = 0; i < inputs.size(); i++) {
        uint8_t *ptr = (uint8_t*)output.GetBuffer() + offset;
        offset += inputs[i].GetByteSize();
        auto patch = TensorBuffer((void*)ptr, inputs[i].GetByteSize(),
            inputs[i].GetTensorType(), inputs[0].GetDeviceId());
        APP_ERROR ret = TensorBuffer::TensorBufferCopy(patch, *inputs[i].buffer_);
        if (ret != APP_ERR_OK) {
            LogError << "TensorBuffer::TensorBufferCopy failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR TensorBase::BatchConcat(const std::vector<TensorBase> &inputs, TensorBase &output)
{
    // check input size
    if (inputs.size() == 0) {
        LogError << "The input size(" << std::to_string(inputs.size()) << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // check
    APP_ERROR ret = CheckBatchTensors(inputs, false);
    if (ret != APP_ERR_OK) {
        LogError << "CheckBatchTensors failed." << GetErrorInfo(ret);
        return ret;
    }
    uint32_t batch = 0;
    for (size_t i = 0; i < inputs.size(); i++) {
        if (inputs[i].GetShape().size() == 0) {
            LogError << "The input(" << i << ") shape size(" << inputs[i].GetShape().size() << ")."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        batch += inputs[i].GetShape()[0];
    }
    std::vector<uint32_t> batchShape = {};
    batchShape.push_back(batch);
    for (uint32_t i = 1; i < inputs[0].GetShape().size(); i++) {
        batchShape.push_back(inputs[0].GetShape()[i]);
    }
    return BatchMalloc(inputs, output, batchShape);
}

APP_ERROR TensorBase::BatchStack(const std::vector<TensorBase> &inputs, TensorBase &output)
{
    // check
    if (inputs.size() == 0) {
        LogError << "The input size(" << std::to_string(inputs.size()) << ")"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    // check shape and device
    APP_ERROR ret = CheckBatchTensors(inputs, true);
    if (ret != APP_ERR_OK) {
        LogError << "CheckBatchTensors failed." << GetErrorInfo(ret);
        return ret;
    }
    std::vector<uint32_t> batchShape = {};
    batchShape.push_back(inputs.size());
    for (uint32_t i = 0; i < inputs[0].GetShape().size(); i++) {
        batchShape.push_back(inputs[0].GetShape()[i]);
    }
    return BatchMalloc(inputs, output, batchShape);
}

APP_ERROR TensorBase::BatchVector(const std::vector<TensorBase> &inputs, TensorBase &output, const bool &keepDims)
{
    for (const auto &input : inputs) {
        auto ret = input.CheckTensorValid();
        if (ret != APP_ERR_OK) {
            LogError << "The input tensor invalid." << GetErrorInfo(ret);
            return ret;
        }
    }

    if (keepDims) {
        APP_ERROR ret = BatchConcat(inputs, output);
        if (ret != APP_ERR_OK) {
            LogError << "BatchConcat failed." << GetErrorInfo(ret);
            return ret;
        }
    } else {
        APP_ERROR ret = BatchStack(inputs, output);
        if (ret != APP_ERR_OK) {
            LogError << "BatchConcat failed." << GetErrorInfo(ret);
            return ret;
        }
    }

    auto ret = output.CheckTensorValid();
    if (ret != APP_ERR_OK) {
        LogError << "The output tensor invalid." << GetErrorInfo(ret);
        return ret;
    }

    return APP_ERR_OK;
}

std::string TensorBase::GetDesc()
{
    std::string shapeStr;
    std::vector<uint32_t> shape = GetShape();
    for (size_t i = 0; i < shape.size(); ++i) {
        shapeStr += std::to_string(shape.at(i));
        if (i != shape.size() - 1) {
            shapeStr += ", ";
        }
    }

    return "<Tensor>\nshape:\t(" + shapeStr + \
        ")\ndtype:\t" + GetTensorDataTypeDesc(GetDataType()) + \
        "\ndevice:\t" + std::to_string(GetDeviceId());
}

APP_ERROR TensorBase::SetValidRoi(Rect rect)
{
    if (shape_ == nullptr) {
        LogError << "The shape of tensor is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return shape_->SetValidRoi(rect);
}

Rect TensorBase::GetValidRoi() const
{
    if (shape_ == nullptr) {
        LogError << "The shape of tensor is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return {};
    }
    return shape_->GetValidRoi();
}

APP_ERROR TensorBase::SetShape(std::vector<uint32_t> shape)
{
    shape_->SetShape(shape);
    return APP_ERR_OK;
}
}

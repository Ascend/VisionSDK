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
 * Description: Converts data with NumPy and converts host, device.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#include "PyTensor/PyTensor.h"

#include <map>
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"
#include "MxBase/E2eInfer/DataType.h"

namespace {
const uint32_t ZERO_BYTE = 0;
const uint32_t ONE_BYTE = 1;
const uint32_t TWO_BYTE = 2;
const uint32_t FOUR_BYTE = 4;
const uint32_t EIGHT_BYTE = 8;
const uint32_t NUMPY_VERSION = 3;
const uint32_t NUMPY_DATA_ATTR_NUM = 2;
const std::map<std::string, MxBase::TensorDType> NUMPY_FORMAT_TO_DATA_TYPE_MAP = {
    {"f4", MxBase::TensorDType::FLOAT32}, {"f2", MxBase::TensorDType::FLOAT16},  {"i1", MxBase::TensorDType::INT8},
    {"i4", MxBase::TensorDType::INT32},   {"u1", MxBase::TensorDType::UINT8},    {"i2", MxBase::TensorDType::INT16},
    {"u2", MxBase::TensorDType::UINT16},  {"u4", MxBase::TensorDType::UINT32},   {"i8", MxBase::TensorDType::INT64},
    {"u8", MxBase::TensorDType::UINT64},  {"f8", MxBase::TensorDType::DOUBLE64}, {"b1", MxBase::TensorDType::BOOL},
};

const std::map<MxBase::TensorDType, std::string> DATA_TYPE_TO_NUMPY_FORMAT_MAP = {
    {MxBase::TensorDType::FLOAT32, "f4"}, {MxBase::TensorDType::FLOAT16, "f2"},  {MxBase::TensorDType::INT8, "i1"},
    {MxBase::TensorDType::INT32, "i4"},   {MxBase::TensorDType::UINT8, "u1"},    {MxBase::TensorDType::INT16, "i2"},
    {MxBase::TensorDType::UINT16, "u2"},  {MxBase::TensorDType::UINT32, "u4"},   {MxBase::TensorDType::INT64, "i8"},
    {MxBase::TensorDType::UINT64, "u8"},  {MxBase::TensorDType::DOUBLE64, "f8"}, {MxBase::TensorDType::BOOL, "b1"},
};

const std::map<MxBase::TensorDType, size_t> DATA_TYPE_TO_BYTE_SIZE_MAP = {
    {MxBase::TensorDType::UNDEFINED, ZERO_BYTE}, {MxBase::TensorDType::UINT8, ONE_BYTE},
    {MxBase::TensorDType::INT8, ONE_BYTE},       {MxBase::TensorDType::UINT16, TWO_BYTE},
    {MxBase::TensorDType::INT16, TWO_BYTE},      {MxBase::TensorDType::UINT32, FOUR_BYTE},
    {MxBase::TensorDType::INT32, FOUR_BYTE},     {MxBase::TensorDType::UINT64, EIGHT_BYTE},
    {MxBase::TensorDType::INT64, EIGHT_BYTE},    {MxBase::TensorDType::FLOAT16, TWO_BYTE},
    {MxBase::TensorDType::FLOAT32, FOUR_BYTE},   {MxBase::TensorDType::DOUBLE64, EIGHT_BYTE},
    {MxBase::TensorDType::BOOL, ONE_BYTE}};

template<typename TargetType>
TargetType SafeCast(float value)
{
    if (value < static_cast<float>(std::numeric_limits<TargetType>::min()) ||
        value > static_cast<float>(std::numeric_limits<TargetType>::max())) {
        throw std::runtime_error("Input value exceeds the range of the target data type.");
    }
    return static_cast<TargetType>(value);
}

APP_ERROR GetDType(PyObject* pyArrayInterface, MxBase::TensorDType& dataType)
{
    PyObject* pyTypestr = PyDict_GetItemString(pyArrayInterface, "typestr");
    if (!pyTypestr) {
        LogError << "Construct Tensor from Numpy failed. Check the type of the input data."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    PyObject* pyBytes = PyUnicode_AsEncodedString(pyTypestr, "utf-8", "strict");
    if (!pyBytes) {
        LogError << "Construct Tensor from Numpy failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    char* pyTypeStr = PyBytes_AsString(pyBytes);
    if (!pyTypeStr) {
        LogError << "Construct Tensor from Numpy failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        Py_DECREF(pyBytes);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::string typeStr(pyTypeStr + 1);
    if (NUMPY_FORMAT_TO_DATA_TYPE_MAP.find(typeStr) == NUMPY_FORMAT_TO_DATA_TYPE_MAP.end()) {
        LogError << "Data type should be one of int8, uint8, int16, uint16, int32, uint32, int64, uint64, float16, "
                    "float32, double, bool."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        Py_DECREF(pyBytes);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    dataType = NUMPY_FORMAT_TO_DATA_TYPE_MAP.find(typeStr)->second;
    Py_DECREF(pyBytes);
    return APP_ERR_OK;
}

APP_ERROR GetShapeAndSize(PyObject* pyArrayInterface, std::vector<uint32_t>& shape, size_t& dataSize)
{
    PyObject* pyShape = PyDict_GetItemString(pyArrayInterface, "shape");
    if (!pyShape || !PyTuple_Check(pyShape)) {
        LogError << "Construct Tensor from Numpy failed. Check the type of the input data."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    long ndim = PyTuple_Size(pyShape);
    if (ndim < 0) {
        LogError << "Construct Tensor from Numpy failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    dataSize = 1;
    for (long i = 0; i < ndim; ++i) {
        PyObject* pyDim = PyTuple_GetItem(pyShape, i);
        size_t tmp = PyLong_AsSize_t(pyDim);
        shape.push_back(static_cast<uint32_t>(tmp));
        dataSize *= tmp;
    }
    return APP_ERR_OK;
}

void PyObjectDeleter(PyObject* obj)
{
    if (obj != nullptr) {
        Py_DECREF(obj);
    }
}
} // namespace
namespace PyBase {
Tensor::Tensor()
{
    tensor_ = MxBase::MemoryHelper::MakeShared<MxBase::Tensor>();
    if (tensor_ == nullptr) {
        LogError << "Create Tensor object failed. Failed to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

Tensor::Tensor(PyObject* obj)
{
    PyObject* pyArrayInterface = PyObject_GetAttrString(obj, "__array_interface__");
    std::shared_ptr<PyObject> pyObjPtr(pyArrayInterface, PyObjectDeleter);
    if (pyArrayInterface == nullptr || !PyDict_Check(pyArrayInterface)) {
        LogError << "Construct Tensor from Numpy failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    auto dataType = MxBase::TensorDType::UINT8;
    APP_ERROR ret = GetDType(pyArrayInterface, dataType);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
    std::vector<uint32_t> shape{};
    size_t dataSize = 1;
    ret = GetShapeAndSize(pyArrayInterface, shape, dataSize);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
    size_t bytes = 0;
    if (DATA_TYPE_TO_BYTE_SIZE_MAP.find(dataType) != DATA_TYPE_TO_BYTE_SIZE_MAP.end()) {
        bytes = DATA_TYPE_TO_BYTE_SIZE_MAP.find(dataType)->second;
    }
    if (dataSize != 0 && std::numeric_limits<size_t>::max() / dataSize < bytes) {
        LogError << "Get invalid Tensor data size." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    dataSize *= bytes;
    tensor_ = std::make_shared<MxBase::Tensor>(shape, dataType, -1);
    ret = tensor_->Malloc();
    if (ret != APP_ERR_OK) {
        LogError << "TensorMalloc failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    PyObject* pyData = PyDict_GetItemString(pyArrayInterface, "data");
    if (!pyData || !PyTuple_Check(pyData)) {
        LogError << "Construct Tensor from Numpy failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    void* buffer = PyLong_AsVoidPtr(PyTuple_GetItem(pyData, 0));
    MxBase::MemoryData destData(tensor_->GetData(), dataSize, MxBase::MemoryData::MemoryType::MEMORY_HOST_MALLOC, -1);
    MxBase::MemoryData srcData(buffer, dataSize, MxBase::MemoryData::MemoryType::MEMORY_HOST_MALLOC, -1);

    MxBase::TensorBase src(srcData, true, shape, static_cast<MxBase::TensorDataType>(dataType));
    MxBase::TensorBase dst(destData, true, shape, static_cast<MxBase::TensorDataType>(dataType));
    ret = MxBase::TensorBase::TensorBaseCopy(dst, src);
    if (ret != APP_ERR_OK) {
        LogError << "TensorBaseCopy failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
}

Tensor::Tensor(const Tensor& other)
{
    tensor_ = other.tensor_;
}

void Tensor::SetTensor(const MxBase::Tensor& src)
{
    (*tensor_) = src;
}

std::string Tensor::GetTypeStr() const
{
    return std::string("|") + DATA_TYPE_TO_NUMPY_FORMAT_MAP.find(tensor_->GetDataType())->second;
}
long long Tensor::GetDataAddr() const
{
    return reinterpret_cast<long long>(tensor_->GetData());
}
void Tensor::to_device(int deviceId)
{
    APP_ERROR ret = tensor_->ToDevice(deviceId);
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
}

void Tensor::to_host()
{
    APP_ERROR ret = tensor_->ToHost();
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
}

void Tensor::set_tensor_value(float value, const MxBase::TensorDType& dataType)
{
    APP_ERROR ret = APP_ERR_OK;
    if (dataType == MxBase::TensorDType::UINT8) {
        uint8_t new_value = SafeCast<uint8_t>(value);
        ret = tensor_->SetTensorValue(new_value);
    } else if (dataType == MxBase::TensorDType::INT32) {
        int32_t new_value = SafeCast<int32_t>(value);
        ret = tensor_->SetTensorValue(new_value);
    } else if (dataType == MxBase::TensorDType::FLOAT16) {
        ret = tensor_->SetTensorValue(value, true);
    } else if (dataType == MxBase::TensorDType::FLOAT32) {
        ret = tensor_->SetTensorValue(value, false);
    } else {
        throw std::runtime_error("Invalid Input Tensor dtype, should be one of dtype.uint8,"
                                 "dtype.int32, dtype.float16, dtype.float32");
    }
    if (ret != APP_ERR_OK) {
        throw std::runtime_error(GetErrorInfo(ret));
    }
}

int Tensor::GetDeviceId() const
{
    return tensor_->GetDeviceId();
}

MxBase::TensorDType Tensor::GetDataType() const
{
    return tensor_->GetDataType();
}

std::vector<uint32_t> Tensor::GetShape() const
{
    return tensor_->GetShape();
}

std::shared_ptr<MxBase::Tensor> Tensor::GetTensorPtr() const
{
    return tensor_;
}

Tensor batch_concat(const std::vector<Tensor>& inputs)
{
    std::vector<MxBase::Tensor> tmpInputs(inputs.size());
    for (size_t i = 0; i < inputs.size(); i++) {
        tmpInputs[i] = *(inputs[i].GetTensorPtr());
    }
    MxBase::Tensor tmpOut;
    APP_ERROR ret = BatchConcat(tmpInputs, tmpOut);
    if (ret != APP_ERR_OK) {
        LogError << "BatchConcat failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
    Tensor output;
    output.SetTensor(tmpOut);
    return output;
}

Tensor transpose(const Tensor& input, const std::vector<uint32_t>& axes)
{
    MxBase::Tensor tmpOut;
    APP_ERROR ret = Transpose(*(input.GetTensorPtr()), tmpOut, axes);
    if (ret != APP_ERR_OK) {
        LogError << "Transpose failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
    Tensor output;
    output.SetTensor(tmpOut);
    return output;
}
} // namespace PyBase
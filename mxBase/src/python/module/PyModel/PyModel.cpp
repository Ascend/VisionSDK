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
 * Description: Used for model loading inference.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include "PyModel/PyModel.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Log/Log.h"
namespace {
const size_t MAX_INPUT_TENSOR_NUM = 1024;
}
namespace PyBase {
DeviceMemory::DeviceMemory(size_t size)
{
    void* ptrData = nullptr;
    MxBase::DeviceMemoryMallocFunc(&ptrData, size, MxBase::MX_MEM_MALLOC_HUGE_FIRST);
    if (ptrData == nullptr) {
        LogError << "Failed to malloc device memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    dataPtr_.reset(ptrData, [](void* data) { MxBase::DeviceMemoryFreeFunc(data); });
}
DeviceMemory::DeviceMemory(const DeviceMemory& other)
{
    dataPtr_ = other.dataPtr_;
}
long long DeviceMemory::get_data()
{
    return reinterpret_cast<long long>(dataPtr_.get());
}

Model::Model(std::string modelPath, int deviceId)
{
    model_ = MxBase::MemoryHelper::MakeShared<MxBase::Model>(modelPath, deviceId);
    if (model_ == nullptr) {
        LogError << "Failed to create model_ object." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

Model::Model(const ModelLoadOptV2& mdlLoadOpt, int deviceId)
{
    mdlLoadOpt_.modelType = static_cast<MxBase::ModelLoadOptV2::ModelType>(mdlLoadOpt.modelType);
    mdlLoadOpt_.loadType = static_cast<MxBase::ModelLoadOptV2::ModelLoadType>(mdlLoadOpt.loadType);
    mdlLoadOpt_.modelPath = mdlLoadOpt.modelPath;
    mdlLoadOpt_.modelPtr = reinterpret_cast<void*>(mdlLoadOpt.modelPtr);
    mdlLoadOpt_.modelSize = mdlLoadOpt.modelSize;
    mdlLoadOpt_.modelWorkPtr = reinterpret_cast<void*>(mdlLoadOpt.modelWorkPtr);
    mdlLoadOpt_.workSize = mdlLoadOpt.workSize;
    mdlLoadOpt_.modelWeightPtr = reinterpret_cast<void*>(mdlLoadOpt.modelWeightPtr);
    mdlLoadOpt_.weightSize = mdlLoadOpt.weightSize;
    model_ = MxBase::MemoryHelper::MakeShared<MxBase::Model>(mdlLoadOpt_, deviceId);
    if (model_ == nullptr) {
        LogError << "Failed to create model_ object." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

Model::Model(const Model& other)
{
    model_ = other.model_;
    mdlLoadOpt_ = other.mdlLoadOpt_;
}
std::vector<Tensor> Model::infer(const std::vector<Tensor>& inputs)
{
    std::vector<MxBase::Tensor> tmpInputs(inputs.size());
    for (size_t i = 0; i < inputs.size(); i++) {
        tmpInputs[i] = *(inputs[i].GetTensorPtr());
    }
    std::vector<MxBase::Tensor> tmpTensorVec = model_->Infer(tmpInputs);
    std::vector<Tensor> tmpOutputs(tmpTensorVec.size());
    for (size_t i = 0; i < tmpOutputs.size(); i++) {
        tmpOutputs[i].SetTensor(tmpTensorVec[i]);
    }
    return tmpOutputs;
}
std::vector<Tensor> Model::infer(const Tensor& input, const std::vector<Tensor>& inputs)
{
    if (inputs.size() + 1 > MAX_INPUT_TENSOR_NUM) {
        LogError << "Input tensor num exceed the upper limit, please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
    }
    std::vector<Tensor> tmpInputs(inputs.size() + 1);
    tmpInputs[0].SetTensor(*input.GetTensorPtr());
    for (size_t i = 0; i < inputs.size(); i++) {
        tmpInputs[i + 1].SetTensor(*inputs[i].GetTensorPtr());
    }
    return infer(tmpInputs);
}
MxBase::TensorDType Model::input_dtype(uint32_t index) const
{
    return model_->GetInputTensorDataType(index);
}
MxBase::TensorDType Model::output_dtype(uint32_t index) const
{
    return model_->GetOutputTensorDataType(index);
}
std::vector<int> Model::input_shape(uint32_t index) const
{
    auto tmp = model_->GetInputTensorShape(index);
    std::vector<int> output(tmp.size());
    for (size_t i = 0; i < output.size(); i++) {
        output[i] = static_cast<int>(tmp[i]);
    }
    return output;
}
std::vector<uint32_t> Model::output_shape(uint32_t index) const
{
    return model_->GetOutputTensorShape(index);
}
std::vector<std::vector<uint32_t>> Model::model_gear() const
{
    auto tmp = model_->GetDynamicGearInfo();
    if (tmp.size() == 0) {
        return {};
    }
    std::vector<std::vector<uint32_t>> output(tmp.size(), std::vector<uint32_t>(tmp[0].size()));
    for (size_t i = 0; i < output.size(); i++) {
        for (size_t j = 0; j < output[0].size(); j++) {
            output[i][j] = static_cast<uint32_t>(tmp[i][j]);
        }
    }
    return output;
}
MxBase::VisionDataFormat Model::GetInputFormat() const
{
    return model_->GetInputFormat();
}
uint32_t Model::GetInputNum() const
{
    return model_->GetInputTensorNum();
}
uint32_t Model::GetOutputNum() const
{
    return model_->GetOutputTensorNum();
}
Model model(std::string modelPath, int deviceId)
{
    return Model(modelPath, deviceId);
}
long long bytes_to_ptr(PyObject *bytes)
{
    long long dataPtr = 0;
    if (PyBytes_Check(bytes)) {
        dataPtr = reinterpret_cast<long long>(PyBytes_AsString(bytes));
        if (dataPtr == 0) {
            LogError << "Get PyBytes failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
        }
    } else {
        PyObject* pyArrayInterface = PyObject_GetAttrString(bytes, "__array_interface__");
        std::shared_ptr<PyObject> pyObjPtr(pyArrayInterface, [](PyObject* obj) {
            if (obj != nullptr) {
                Py_DECREF(obj);
            }
        });
        if (!pyArrayInterface || !PyDict_Check(pyArrayInterface)) {
            LogError << "Get attribute failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
        }
        PyObject* pyData = PyDict_GetItemString(pyArrayInterface, "data");
        if (!pyData || !PyTuple_Check(pyData)) {
            LogError << "Get attribute failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INVALID_PARAM));
        }
        dataPtr = reinterpret_cast<long long>(PyLong_AsVoidPtr(PyTuple_GetItem(pyData, 0)));
    }
    return dataPtr;
}
}  // namespace PyBase
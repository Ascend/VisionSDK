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

#ifndef PY_TENSOR_H
#define PY_TENSOR_H

#include <vector>
#include <memory>
#include <Python.h>
#include "MxBase/E2eInfer/Tensor/Tensor.h"
namespace PyBase {
class Tensor {
public:
    Tensor();
    Tensor(PyObject* buffer);
    Tensor(const Tensor& other);
    ~Tensor() = default;
    void set_tensor_value(float value, const MxBase::TensorDType& dataType);
    void to_device(int deviceId);
    void to_host();
    int GetDeviceId() const;
    MxBase::TensorDType GetDataType() const;
    std::vector<uint32_t> GetShape() const;
    long long GetDataAddr() const;
    std::string GetTypeStr() const;
    void SetTensor(const MxBase::Tensor& src);
    std::shared_ptr<MxBase::Tensor> GetTensorPtr() const;

private:
    std::shared_ptr<MxBase::Tensor> tensor_ = nullptr;
};

PyBase::Tensor batch_concat(const std::vector<PyBase::Tensor>& inputs);
PyBase::Tensor transpose(const PyBase::Tensor& input, const std::vector<uint32_t>& axes);
} // namespace PyBase
#endif

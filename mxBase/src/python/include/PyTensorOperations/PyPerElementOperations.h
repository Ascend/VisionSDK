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
 * Description: Python Matrices Per Element Operations On Tensors.
 * Author: Vision SDK
 * Create: 2025
 * History: NA
 */


#ifndef PY_PER_ELEMENT_OPERATIONS
#define PY_PER_ELEMENT_OPERATIONS

#include "PyTensor/PyTensor.h"
#include "MxBase/E2eInfer/DataType.h"
#include <Python.h>

namespace PyBase {
    PyBase::Tensor convert_to(const PyBase::Tensor& inputTensor, const MxBase::TensorDType& dataType);
    PyBase::Tensor clip(const PyBase::Tensor& inputTensor, float minVal, float maxVal);
    PyBase::Tensor add(const PyBase::Tensor& inputTensor1, const PyBase::Tensor& inputTensor2);
    PyBase::Tensor subtract(const PyBase::Tensor& inputTensor1, const PyBase::Tensor& inputTensor2);
    PyBase::Tensor min_operator(const PyBase::Tensor& inputTensor1, const PyBase::Tensor& inputTensor2);
    PyBase::Tensor max_operator(const PyBase::Tensor& inputTensor1, const PyBase::Tensor& inputTensor2);
    PyBase::Tensor multiply(const PyBase::Tensor& inputTensor1,
                            const PyBase::Tensor& inputTensor2);
    PyBase::Tensor multiply(const PyBase::Tensor& inputTensor1,
                            const PyBase::Tensor& inputTensor2,
                            double scale);
    PyBase::Tensor divide(const PyBase::Tensor& inputTensor1,
                          const PyBase::Tensor& inputTensor2);
    PyBase::Tensor divide(const PyBase::Tensor& inputTensor1,
                          const PyBase::Tensor& inputTensor2,
                          double scale);
} // namespace PyBase
#endif
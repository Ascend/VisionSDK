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

#ifndef PY_MODEL_H
#define PY_MODEL_H

#include <vector>
#include <string>
#include <memory>
#include <Python.h>
#include "PyTensor/PyTensor.h"
#include "MxBase/E2eInfer/Model/Model.h"
#include "MxBase/E2eInfer/DataType.h"

namespace PyBase {
class DeviceMemory {
public:
    DeviceMemory(size_t size);
    DeviceMemory(const DeviceMemory& other);
    ~DeviceMemory() = default;
    long long get_data();

private:
    std::shared_ptr<void> dataPtr_ = nullptr;
};

enum class ModelLoadType {
    LOAD_MODEL_FROM_FILE = 1,
    LOAD_MODEL_FROM_FILE_WITH_MEM,
    LOAD_MODEL_FROM_MEM,
    LOAD_MODEL_FROM_MEM_WITH_MEM
};
enum class ModelType { MODEL_TYPE_OM = 0, MODEL_TYPE_MINDIR };

struct ModelLoadOptV2 {
    ModelType modelType = ModelType::MODEL_TYPE_OM;
    ModelLoadType loadType = ModelLoadType::LOAD_MODEL_FROM_FILE;
    std::string modelPath = "";
    long long modelPtr = 0;
    long long modelWorkPtr = 0;
    long long modelWeightPtr = 0;
    size_t modelSize = 0;
    size_t workSize = 0;
    size_t weightSize = 0;
};
class Model {
public:
    Model(std::string modelPath, int deviceId = 0);
    Model(const ModelLoadOptV2& mdlLoadOpt, int deviceId = 0);
    ~Model() = default;
    Model(const Model& other);
    std::vector<PyBase::Tensor> infer(const std::vector<PyBase::Tensor>& inputs);
    std::vector<PyBase::Tensor> infer(const PyBase::Tensor& input, const std::vector<PyBase::Tensor>& inputs = {});
    MxBase::TensorDType input_dtype(uint32_t index) const;
    MxBase::TensorDType output_dtype(uint32_t index) const;
    std::vector<int> input_shape(uint32_t index) const;
    std::vector<uint32_t> output_shape(uint32_t index) const;
    std::vector<std::vector<uint32_t>> model_gear() const;
    MxBase::VisionDataFormat GetInputFormat() const;
    uint32_t GetInputNum() const;
    uint32_t GetOutputNum() const;

private:
    std::shared_ptr<MxBase::Model> model_;
    MxBase::ModelLoadOptV2 mdlLoadOpt_;
};
Model model(std::string modelPath, int deviceId);
long long bytes_to_ptr(PyObject* data);
} // namespace PyBase
#endif

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
 * Description: Mindir model infer interface.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#ifndef MXMINDIR_MODEL_DESC_H
#define MXMINDIR_MODEL_DESC_H

#include <mutex>
#include "acl/acl.h"
#include "../MindsporeLiteWrapper/MindsporeLiteWrapper.h"
#include "../MxModelDesc/MxModelDesc.h"

namespace MxBase {
class MxMindIRModelDesc : public MxModelDesc {
public:
    MxMindIRModelDesc() = default;

    MxMindIRModelDesc(const MxMindIRModelDesc&) = delete;

    MxMindIRModelDesc& operator=(const MxMindIRModelDesc&) = delete;

    ~MxMindIRModelDesc() final;

    APP_ERROR Init(ModelLoadOptV2 &mdlLoadOpt, const int32_t deviceId = 0) final;

    APP_ERROR SetDevice() final;

    APP_ERROR Infer(std::vector<Tensor>& inputTensors, std::vector<Tensor>& outputTensors,
                    AscendStream& stream = AscendStream::DefaultStream()) final;

    std::vector<Tensor> Infer(std::vector<Tensor>& inputTensors) final;

    uint32_t GetInputTensorNum() const final;

    uint32_t GetOutputTensorNum() const final;

    std::vector<int64_t> GetInputTensorShape(uint32_t index = 0) const final;

    std::vector<uint32_t> GetOutputTensorShape(uint32_t index = 0) const final;

    TensorDType GetInputTensorDataType(uint32_t index = 0) const final;

    TensorDType GetOutputTensorDataType(uint32_t index = 0) const final;

    VisionDataFormat GetInputFormat() const final;

    std::vector<std::vector<uint64_t>> GetDynamicGearInfo() const final;

    VisionDynamicType GetDynamicType() const final;

private:
    APP_ERROR InitMsliteWrapper();

    APP_ERROR InitFromFile(const std::string& modelPath);

    APP_ERROR InitFromMemory(const ModelLoadOptV2 &mdlLoadOpt);

    APP_ERROR GetModelDesc();

    APP_ERROR GetMsliteWrapperLib(std::string& msliteLibFile);

    APP_ERROR CheckTensors(std::vector<Tensor>& tensors, const uint32_t count);

    APP_ERROR MallocAndSetInputTensor(std::vector<Tensor>& inputTensors, std::vector<MiddleTensor>& inputs,
                                      std::vector<std::vector<int64_t>>& inputShape);

    MiddleTensor TransTensorToMDTensor(Tensor& tensor);

    APP_ERROR ModelInference(std::vector<Tensor>& inputTensors, std::vector<Tensor>& outputTensors);

    APP_ERROR ModelInferenceAsync(std::vector<Tensor>& inputTensors, std::vector<Tensor>& outputTensors,
                                  AscendStream& stream = AscendStream::DefaultStream());

    Tensor TransMDTensorToTensor(MiddleTensor& mdTensor);

    APP_ERROR GetModelOutput(std::vector<MiddleTensor>& output, std::vector<Tensor>& outputTensors);

    APP_ERROR MemoryCopyMDTensorToTensor(std::vector<MiddleTensor>& mdTensor, std::vector<Tensor>& tensor);

    APP_ERROR ModelInfer(std::vector<MiddleTensor>& inputs, std::vector<Tensor>& outputTensors,
                                AscendStream& stream = AscendStream::DefaultStream());

    static void ModelInferCallbackFunc(void *arg);

    APP_ERROR CheckInputTensorData(std::vector<Tensor>& tensorVec);

private:
    int32_t deviceId_ = 0;
    void* mindsporeLiteWrapperLib_ = nullptr;
    MindsporeLiteWrapper *mindsporeLiteWapper_ = nullptr;
    std::mutex mindIRModelMutex_;
    CreateInstance createInstance_ = nullptr;
    DestroyInstance destroyInstance_ = nullptr;
    std::vector<MiddleTensor> inputTensors_ = {};
    std::vector<MiddleTensor> outputTensors_ = {};
};

struct ModelInferCallbackParam {
    std::vector<MiddleTensor> inputs;
    std::vector<Tensor> outputs;
    AscendStream& stream;
    MxMindIRModelDesc *mindIRModelDesc;
};
} // MxBase namespace end

#endif
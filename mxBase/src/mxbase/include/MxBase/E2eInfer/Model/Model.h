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
 * Description: Processing of the Model Inference Function.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef MX_MODEL_H
#define MX_MODEL_H

#include <vector>
#include <memory>
#include "MxBase/Asynchron/AscendStream.h"
#include "MxBase/E2eInfer/DataType.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"

namespace MxBase {
class MxModelDesc;
class Model {
public:
    /*
    * @description: Construction function.
    * @param: model path, deviceId
    */
    Model(std::string &modelPath, const int32_t deviceId = 0);

    /*
    * @description: Construction function.
    * @param: model load option, deviceId
    */
    Model(ModelLoadOptV2 &mdlLoadOpt, const int32_t deviceId = 0);

    /*
    * @description: Forbid copy construction.
    */
    Model(const Model&) = delete;

    /*
    * @description: Forbid assign construction.
    */
    Model &operator=(const Model&) = delete;

    /*
    * @description: Default deconstruction function.
    */
    ~Model();

    /*
    * @description: Infer interface.
    * @param: inputTensors: list of input tensors, sizeof(inputTensors) is equal to model's InputTensorNum.
    *         outputTensors: list of output tensors, sizeof(outputTensors) is equal to model's OutputTensorNum.
    *                        Strongly recommend Tensor.Malloc() to assign memory for output tensors.
    */
    APP_ERROR Infer(std::vector<Tensor> &inputTensors, std::vector<Tensor> &outputTensors,
                    AscendStream &stream = AscendStream::DefaultStream());

    /*
    * @description: Another infer interface.
    * @param: inputTensors: list of input tensors, sizeof(inputTensors) is equal to model's InputTensorNum.
    * @return: Output of model. Different from above Infer interface, this Infer helps manage memory of outputTensors.
    */
    std::vector<Tensor> Infer(std::vector<Tensor>& inputTensors);

    /*
    * @description: Get model's InputTensorNum.
    */
    uint32_t GetInputTensorNum();

    /*
    * @description: Get model's OutputTensorNum.
    */
    uint32_t GetOutputTensorNum();

    /*
    * @description: Get No.index inputTensor's shape of Model. (default is No.0)
    */
    std::vector<int64_t> GetInputTensorShape(uint32_t index = 0);

    /*
    * @description: Get No.index outputTensor's shape of Model. (default is No.0)
    */
    std::vector<uint32_t> GetOutputTensorShape(uint32_t index = 0);

    /*
    * @description: Get No.index InputTensor's dataType of Model. (default is No.0)
    * @return: Details seen in "MxBase/E2eInfer/DataType.h".
    */
    MxBase::TensorDType GetInputTensorDataType(uint32_t index = 0);

    /*
    * @description: Get No.index OutputTensor's dataType of Model. (default is No.0)
    * @return: Details seen in "MxBase/E2eInfer/DataType.h".
    */
    MxBase::TensorDType GetOutputTensorDataType(uint32_t index = 0);

    /*
    * @description: Get InputFormat of Model;
    * @return:
    *   enum class VisionDataFormat {
            NCHW = 0,
            NHWC = 1
        };
    */
    MxBase::VisionDataFormat GetInputFormat();

    /**
     * @description: get the gear info of dynamic model
     */
    std::vector<std::vector<uint64_t>> GetDynamicGearInfo();

private:
    std::shared_ptr<MxBase::MxModelDesc> mxModelDesc_;
    APP_ERROR InferAsync(std::vector<Tensor> &inputTensors, std::vector<Tensor> &outputTensors,
                         AscendStream &stream = AscendStream::DefaultStream());
    static void InferAsyncProcess(void* args);
};
}

#endif
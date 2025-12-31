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
 * Create: 2020
 * History: NA
 */

#include "acl/acl.h"
#include "MxBase/Log/Log.h"
#include "ModelInferenceProcessorDptr.hpp"
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"

namespace {
    constexpr size_t MAX_TENSOR_VEC_SIZE = 1024;
}

namespace MxBase {
ModelInferenceProcessor::ModelInferenceProcessor()
{
    dPtr_ = std::make_shared<MxBase::ModelInferenceProcessorDptr>();
}

/*
 * @description Init Model
 * @return APP_ERROR error code
 */
APP_ERROR ModelInferenceProcessor::Init(std::string modelPath, ModelDesc& modelDesc)
{
    LogDebug << "Begin to ModelInferenceProcessor init";
    if (dPtr_ -> isInit_) {
        LogWarn << "ModelInferenceProcessor has been init already.";
        return APP_ERR_OK;
    }
    APP_ERROR ret = dPtr_->CheckFile(modelPath);
    if (ret != APP_ERR_OK) {
        LogError << "Model path is invalidate." << GetErrorInfo(ret);
        return ret;
    }
    ret = dPtr_->InitModel(modelPath);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to init model, the model path is invalidate." << GetErrorInfo(ret);
        return ret;
    }
    ret = dPtr_->InitModelAttr(modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "InitModelAttr failed." << GetErrorInfo(ret);
        DeInit();
        return ret;
    }
    dPtr_ -> isInit_ = true;
    LogDebug << "End to ModelInferenceProcessor init";
    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessor::Init(const std::string& modelPath)
{
    dPtr_->modelDesc_ = {};
    return Init(modelPath, dPtr_->modelDesc_);
}

/*
 * @description Unload Model
 * @return APP_ERROR error code
 */
APP_ERROR ModelInferenceProcessor::DeInit(void)
{
    APP_ERROR ret = aclmdlUnload(dPtr_->modelId_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to unload model." << GetErrorInfo(ret, "aclmdlUnload");
        return APP_ERR_ACL_FAILURE;
    }
    if (dPtr_->aclModelDesc_ != nullptr) {
        ret = aclmdlDestroyDesc((aclmdlDesc*)dPtr_->aclModelDesc_);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to free model desc." << GetErrorInfo(ret, "aclmdlDestroyDesc");
            return APP_ERR_ACL_FAILURE;
        }
        dPtr_->aclModelDesc_ = nullptr;
    }
    if (dPtr_->stream_ != nullptr) {
        ret = aclrtDestroyStream(dPtr_->stream_);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to Destroy stream." << GetErrorInfo(ret);
            return ret;
        }
        dPtr_->stream_ = nullptr;
    }
    return APP_ERR_OK;
}

/*
 * @description Set Dynamic Batch
 * 1.Get model description
 * 2.Execute model infer
 * @return APP_ERROR error code
 */
APP_ERROR ModelInferenceProcessor::ModelInference(
    std::vector<BaseTensor>& inputTensors, std::vector<BaseTensor>& outputTensors, size_t dynamicBatchSize)
{
    // Execute model infer
    DynamicInfo dynamicInfo;
    dynamicInfo.dynamicType = (dynamicBatchSize == 0) ? STATIC_BATCH : DYNAMIC_BATCH;
    dynamicInfo.batchSize = dynamicBatchSize;
    APP_ERROR ret = (dPtr_->ModelInfer)(inputTensors, outputTensors, dynamicInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute model infer." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessor::ModelInference(std::vector<BaseTensor>& inputTensors,
                                                  std::vector<BaseTensor>& outputTensors, DynamicInfo dynamicInfo)
{
    if (inputTensors.empty() || outputTensors.empty()) {
        LogError << "InputTensors or outputTensors are empty. Please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = (dPtr_->ModelInfer)(inputTensors, outputTensors, dynamicInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to execute model infer." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessor::ModelInference(const std::vector<TensorBase>& inputTensors,
    std::vector<TensorBase>& outputTensors, DynamicInfo dynamicInfo)
{
    if (inputTensors.size() > MAX_TENSOR_VEC_SIZE || outputTensors.size() > MAX_TENSOR_VEC_SIZE) {
        LogError << "Input or output tensor vector size exceeds the maximum(" << MAX_TENSOR_VEC_SIZE << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::vector<BaseTensor> inputs = {};
    int batchSize = 0;
    for (size_t i = 0; i < inputTensors.size(); ++i) {
        BaseTensor inputTensor = {};
        inputTensor.buf = inputTensors[i].GetBuffer();
        inputTensor.size = inputTensors[i].GetByteSize();
        std::vector<uint32_t> inputShape = inputTensors[i].GetShape();
        inputs.push_back(std::move(inputTensor));
    }
    std::vector<BaseTensor> outputs = {};
    for (size_t i = 0; i < outputTensors.size(); ++i) {
        BaseTensor outputTensor = {};
        outputTensor.buf = outputTensors[i].GetBuffer();
        outputTensor.size = outputTensors[i].GetByteSize();
        std::vector<uint32_t> inputShape = outputTensors[i].GetShape();
        outputs.push_back(std::move(outputTensor));
    }
    if (dynamicInfo.dynamicType == DYNAMIC_BATCH) {
        return ModelInference(inputs, outputs, dynamicInfo);
    } else {
        return ModelInference(inputs, outputs, batchSize);
    }
}

std::vector<int64_t> ModelInferenceProcessor::GetDynamicBatch() const
{
    return dPtr_->dynamicBatch_;
}

const std::vector<ImageSize>& ModelInferenceProcessor::GetDynamicImageSizes() const
{
    return dPtr_->dynamicImageSizes_;
}

DynamicType ModelInferenceProcessor::GetDynamicType() const
{
    return dPtr_->dynamicType_;
}

const ModelDesc& ModelInferenceProcessor::GetModelDesc() const
{
    return dPtr_->modelDesc_;
}

std::vector<size_t> ModelInferenceProcessor::GetInputFormat() const
{
    std::vector<size_t> inputFormat = {};
    for (size_t i = 0; i < dPtr_->inputTensorCount_; ++i) {
        aclFormat input = aclmdlGetInputFormat((aclmdlDesc*)dPtr_->aclModelDesc_, i);
        inputFormat.push_back(input);
    }
    return inputFormat;
}

std::vector<size_t> ModelInferenceProcessor::GetOutputFormat() const
{
    std::vector<size_t> outputFormat = {};
    for (size_t i = 0; i < dPtr_->outputTensorCount_; ++i) {
        aclFormat input = aclmdlGetOutputFormat((aclmdlDesc*)dPtr_->aclModelDesc_, i);
        outputFormat.push_back(input);
    }
    return outputFormat;
}

DataFormat ModelInferenceProcessor::GetDataFormat() const
{
    return dPtr_->dataFormat_;
}

std::vector<std::vector<int64_t>> ModelInferenceProcessor::GetInputShape() const
{
    return dPtr_->inputShape_;
}

std::vector<std::vector<int64_t>> ModelInferenceProcessor::GetOutputShape() const
{
    return dPtr_->outputShape_;
}

std::vector<TensorDataType> ModelInferenceProcessor::GetInputDataType() const
{
    return dPtr_->inputDataType_;
}

std::vector<TensorDataType> ModelInferenceProcessor::GetOutputDataType() const
{
    return dPtr_->outputDataType_;
}

std::vector<std::vector<uint64_t>> ModelInferenceProcessor::GetDynamicGearInfo()
{
    return dPtr_->GetDynamicGearShape();
}
}  // namespace MxBase

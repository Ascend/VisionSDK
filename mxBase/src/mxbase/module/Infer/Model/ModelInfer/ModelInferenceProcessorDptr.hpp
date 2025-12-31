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
* Description: Private interface of the ModelInferenceProcessor for internal use only.
* Author: MindX SDK
* Create: 2020
* History: NA
*/

#ifndef MODLE_INFERENCEPROCESSOR_DPTR_H
#define MODLE_INFERENCEPROCESSOR_DPTR_H

#include <dirent.h>
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#include "MxBase/Utils/FileUtils.h"

namespace MxBase {
const int IMAGE_ALL_DIMS = 4;

class ModelInferenceProcessorDptr {
public:
    /**
     * @description Init Model
     * 1.Loads offline model data from files
     * 2.Users manage the memory used for model running
     * @return APP_ERROR error code
     */
    APP_ERROR InitModel(std::string modelPath);

    /**
     * @description Get model input sizes and dims
     * @return APP_ERROR error code
     */
    APP_ERROR GetInputTensorsSize(ModelDesc& modelDesc, size_t index);

    /**
     * @description Get model output sizes and dims
     * @return APP_ERROR error code
     */
    APP_ERROR GetOutputTensorsSize(ModelDesc& modelDesc, size_t index);

    /**
     * @description modelInfer
     * 1.creat databuffer
     * 2.releas databuffer
     * @return APP_ERROR error code
     */
    APP_ERROR ModelInfer(std::vector<BaseTensor>& inputTensors, std::vector<BaseTensor>& outputTensors,
                         DynamicInfo dynamicInfo);

    /**
     * @description Set Dynamic Batch
     * @return APP_ERROR error code
     */
    APP_ERROR ModelSetDynamicInfo(ModelDataset& input, const DynamicInfo& dynamicInfo);

    /**
     * @description creat databuffer
     * @return APP_ERROR error code
     */
    void* CreateAndFillDataset(std::vector<BaseTensor>& tensors);

    /**
     * @description release dataBuffer
     */
    void DestroyDataset(ModelDataset& dataset);

    /**
     * @description set dynamic image size
     */
    APP_ERROR SetDynamicImageSize(size_t batchSize);

    /**
     * @description Get model cur output dims
     */
    APP_ERROR GetModelCurOutputDims();

    /**
     * @description get input shape
     */
    APP_ERROR GetTensorShape();

    /**
     * @description get data type
     */
    void GetDataType();

    /**
     * @description get dynamic batch
     */
    APP_ERROR GetBatchSize(ModelDesc& modelDesc, int64_t batchSize);

    /**
     * @description check file
     */
    APP_ERROR CheckFile(std::string& filePath);

    /**
     * @description get output shape of dynamic shape model
     */
    APP_ERROR GetDynamicShape(aclmdlDataset *output);

    /**
     * @description set model's dynamic type
     * 1. set the dynamicType variable
     * 2. used for choosing which acl interface to call
     */
    APP_ERROR SetDynamicType(size_t batchSize);

    /**
     * @description set dynamic dims
     */
    APP_ERROR SetDynamicDimsType();

    /**
     * @description set dynamic shape type
     */
    APP_ERROR SetDynamicShapeType();

    /**
     * @description call acl interface to set dynamic image info
     */
    APP_ERROR SetDynamicImageInfo(aclmdlDataset *mdlDataset, DynamicInfo dynamicInfo);

    /**
     * @description call acl interface to set dynamic dims
     */
    APP_ERROR SetDynamicDims(aclmdlDataset *mdlDataset);

    /**
     * @description call acl interface to set dynamic shape info
     */
    APP_ERROR SetDynamicShapeInfo(aclmdlDataset *mdlDataset);

    /**
     * @description: get dynamic gear info of dynamic model, including dynamic batch, dynamic hw, dynamic dims
     */
    std::vector<std::vector<uint64_t>> GetDynamicGearShape();

    /**
     * @description: Init Model Attr, including outputTensorCount, inputTensorCount, DataType, dynamicType
     */
    APP_ERROR InitModelAttr(ModelDesc& modelDesc);
public:
    uint32_t modelId_ = 0;
    void* aclModelDesc_ = nullptr;
    size_t dynamicTensorIndex_ = 0;
    bool isInit_ = false;
    std::vector<ImageSize> dynamicImageSizes_ = {};
    DynamicType dynamicType_ = STATIC_BATCH;
    ModelDesc modelDesc_ = {};
    DataFormat dataFormat_ = NCHW;
    size_t inputTensorCount_ = 0;
    size_t outputTensorCount_ = 0;
    std::vector<std::vector<int64_t>> inputShape_ {};
    std::vector<std::vector<int64_t>> outputShape_ {};
    std::vector<TensorDataType> inputDataType_ {};
    std::vector<TensorDataType> outputDataType_ {};
    std::vector<int64_t> dynamicBatch_ {};
    std::vector<std::vector<uint32_t>> inputshape {};
    aclrtStream stream_ = {};
    std::vector<std::vector<uint64_t>> dynamicDims_ {};
};

/*
* @description Init Model
* 1.Loads offline model data from files
* 2.Users manage the memory used for model running
* @return APP_ERROR error code
*/
APP_ERROR ModelInferenceProcessorDptr::InitModel(std::string modelPath)
{
    APP_ERROR ret = aclrtCreateStream(&stream_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to Create device stream." << GetErrorInfo(ret);
        return ret;
    }
    // loads offline model data from file without mem
    ret = aclmdlLoadFromFile(modelPath.c_str(), &modelId_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to load offline model data from file." << GetErrorInfo(ret, "aclmdlLoadFromFile");
        aclrtDestroyStream(stream_);
        return APP_ERR_ACL_FAILURE;
    }
    aclModelDesc_ = aclmdlCreateDesc();
    // Obtains the model description
    ret = aclmdlGetDesc((aclmdlDesc*)aclModelDesc_, modelId_);
    if (ret != APP_ERR_OK) {
        if (aclModelDesc_ != nullptr) {
            aclmdlDestroyDesc(static_cast<aclmdlDesc*>(aclModelDesc_));
            aclModelDesc_ = nullptr;
        }
        aclmdlUnload(modelId_);
        aclrtDestroyStream(stream_);
        LogError << "Failed to obtain the model description." << GetErrorInfo(ret, "aclmdlGetDesc");
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessorDptr::GetInputTensorsSize(ModelDesc& modelDesc, size_t index)
{
    aclmdlIODims inputDims;
    APP_ERROR ret = aclmdlGetInputDims((aclmdlDesc*)aclModelDesc_, index, &inputDims);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get input dims." << GetErrorInfo(ret, "aclmdlGetInputDims");
        return APP_ERR_ACL_FAILURE;
    }
    size_t bufferSize = aclmdlGetInputSizeByIndex((aclmdlDesc*)aclModelDesc_, index);
    TensorDesc tensorDesc = TensorDesc();
    tensorDesc.tensorSize = bufferSize;
    for (size_t i = 0; i < inputDims.dimCount; i++) {
        tensorDesc.tensorDims.push_back(inputDims.dims[i]);
    }
    tensorDesc.tensorName = inputDims.name;
    modelDesc.inputTensors.push_back(std::move(tensorDesc));

    aclFormat format = aclmdlGetInputFormat((aclmdlDesc*)aclModelDesc_, index);
    if (format == ACL_FORMAT_NCHW) {
        ImageSize imageSize(static_cast<size_t>(inputDims.dims[0x2]),
                            static_cast<size_t>(inputDims.dims[0x3]));
        dynamicImageSizes_.push_back(imageSize);
        dataFormat_ = NCHW;
    } else if (format == ACL_FORMAT_NHWC) {
        ImageSize imageSize(static_cast<size_t>(inputDims.dims[0x1]),
                            static_cast<size_t>(inputDims.dims[0x2]));
        dynamicImageSizes_.push_back(imageSize);
        dataFormat_ = NHWC;
    }
    ret = GetBatchSize(modelDesc, inputDims.dims[0x0]);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get dynamic batch." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessorDptr::GetOutputTensorsSize(ModelDesc& modelDesc, size_t index)
{
    // Get output size according to index
    size_t bufferSize = aclmdlGetOutputSizeByIndex((aclmdlDesc*)aclModelDesc_, index);
    aclmdlIODims outPut;
    APP_ERROR ret = aclmdlGetOutputDims((aclmdlDesc*)aclModelDesc_, index, &outPut);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get output dims." << GetErrorInfo(ret, " aclmdlGetOutputDims");
        return APP_ERR_ACL_FAILURE;
    }
    TensorDesc tensorDesc = TensorDesc();
    tensorDesc.tensorSize = bufferSize;
    tensorDesc.tensorName = outPut.name;
    for (size_t i = 0; i < outPut.dimCount; i++) {
        tensorDesc.tensorDims.push_back(outPut.dims[i]);
    }
    modelDesc.outputTensors.push_back(std::move(tensorDesc));
    return APP_ERR_OK;
}

/*
* @description modelInfer
* 1.creat databuffer
* 2.releas databuffer
* @return APP_ERROR error code
*/
APP_ERROR ModelInferenceProcessorDptr::ModelInfer(std::vector<BaseTensor>& inputTensors,
                                                  std::vector<BaseTensor>& outputTensors, DynamicInfo dynamicInfo)
{
    ModelDataset inputDataset;
    inputDataset.mdlDataPtr = CreateAndFillDataset(inputTensors);
    if (inputDataset.mdlDataPtr == nullptr) {
        return APP_ERR_COMM_FAILURE;
    }

    APP_ERROR ret = ModelSetDynamicInfo(inputDataset, dynamicInfo);
    if (ret != APP_ERR_OK) {
        DestroyDataset(inputDataset);
        inputDataset.mdlDataPtr = nullptr;
        LogError << "Failed to set model dynamic info." << GetErrorInfo(ret);
        return ret;
    }

    ModelDataset outputDataset;
    outputDataset.mdlDataPtr = CreateAndFillDataset(outputTensors);
    if (outputDataset.mdlDataPtr == nullptr) {
        DestroyDataset(inputDataset);
        inputDataset.mdlDataPtr = nullptr;
        return APP_ERR_COMM_FAILURE;
    }
    // Execute model infer
    APP_ERROR subRet = APP_ERR_OK;
    auto input = static_cast<aclmdlDataset*>(inputDataset.mdlDataPtr);
    auto output = static_cast<aclmdlDataset*>(outputDataset.mdlDataPtr);
    ret = aclmdlExecuteAsync(modelId_, input, output, stream_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to model infer execute." << GetErrorInfo(ret, "aclmdlExecute");
        ret = aclrtSynchronizeStream(stream_);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to synchronize stream."<< GetErrorInfo(ret);
        }
        DestroyDataset(inputDataset);
        DestroyDataset(outputDataset);
        return APP_ERR_ACL_FAILURE;
    }

    if (dynamicType_ == DYNAMIC_SHAPE) {
        ret = GetDynamicShape(output);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to get output shape for dynamic shape model." << GetErrorInfo(ret);
            subRet = APP_ERR_ACL_FAILURE;
        }
    }
    ret = aclrtSynchronizeStream(stream_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to synchronize stream."<< GetErrorInfo(ret);
        subRet = APP_ERR_ACL_FAILURE;
    }
    DestroyDataset(inputDataset);
    DestroyDataset(outputDataset);
    return subRet;
}

APP_ERROR ModelInferenceProcessorDptr::GetDynamicShape(aclmdlDataset *output)
{
    APP_ERROR ret = APP_ERR_OK;
    outputShape_.clear();
    for (size_t i = 0; i < modelDesc_.outputTensors.size(); i++) {
        aclTensorDesc *outputTensorDesc = aclmdlGetDatasetTensorDesc(output, i);
        size_t numDims = aclGetTensorDescNumDims(outputTensorDesc);
        if (numDims == ACL_UNKNOWN_RANK) {
            LogError << "Get dynamic shape output tensor num dims failed." << GetErrorInfo(APP_ERR_ACL_FAILURE);
            return APP_ERR_ACL_FAILURE;
        }
        std::vector<int64_t> tmpShape = {};
        for (size_t j = 0; j < numDims; j++) {
            int64_t dimSize;
            ret = aclGetTensorDescDimV2(outputTensorDesc, j, &dimSize);
            tmpShape.push_back(dimSize);
        }
        outputShape_.push_back(std::move(tmpShape));
    }
    if (ret != APP_ERR_OK) {
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

/*
* @description Set Dynamic Batch
* @return APP_ERROR error code
*/
APP_ERROR ModelInferenceProcessorDptr::ModelSetDynamicInfo(ModelDataset& input, const DynamicInfo& dynamicInfo)
{
    inputshape = dynamicInfo.shape;
    auto mdlDataset = static_cast<aclmdlDataset*>(input.mdlDataPtr);
    switch (dynamicInfo.dynamicType) {
        case STATIC_BATCH:
            return APP_ERR_OK;
        case DYNAMIC_BATCH: {
            APP_ERROR ret = aclmdlSetDynamicBatchSize(modelId_, mdlDataset, dynamicTensorIndex_, dynamicInfo.batchSize);
            if (ret != APP_ERR_OK) {
                LogError << "Dynamic batch set failed, modelId = " << modelId_ << ", index = "
                         << dynamicTensorIndex_ << ", dynamicBatchSize = " << dynamicInfo.batchSize
                         << GetErrorInfo(ret, "aclmdlSetDynamicBatchSize");
                return APP_ERR_ACL_FAILURE;
            }

            LogDebug << "Set dynamicBatchSize(" << dynamicInfo.batchSize << ") successfully.";
            break;
        }
        case DYNAMIC_HW: {
            APP_ERROR ret = SetDynamicImageInfo(mdlDataset, dynamicInfo);
            if (ret != APP_ERR_OK) {
                LogError << "Dynamic image set failed." << GetErrorInfo(ret);
                return APP_ERR_ACL_FAILURE;
            }
            return GetModelCurOutputDims();
        }
        case DYNAMIC_DIMS: {
            APP_ERROR ret = SetDynamicDims(mdlDataset);
            if (ret != APP_ERR_OK) {
                LogError << "Dynamic dims set failed." << GetErrorInfo(ret);
                return ret;
            }
            return GetModelCurOutputDims();
        }
        case DYNAMIC_SHAPE: {
            APP_ERROR ret = SetDynamicShapeInfo(mdlDataset);
            if (ret != APP_ERR_OK) {
                LogError << "Dynamic shape set failed." << GetErrorInfo(ret);
                return ret;
            }
            break;
        }
        default:
            LogError << "dynamicType(" << dynamicInfo.dynamicType << ") is not support."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
    }

    return APP_ERR_OK;
}

/*
* @description creat databuffer
* @return APP_ERROR error code
*/
void* ModelInferenceProcessorDptr::CreateAndFillDataset(std::vector<BaseTensor>& tensors)
{
    APP_ERROR ret = APP_ERR_OK;
    aclmdlDataset* dataset = aclmdlCreateDataset();
    if (dataset == nullptr) {
        LogError << "Failed to create model input." << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return nullptr;
    }

    for (const auto& tensor : tensors) {
        aclDataBuffer* data = aclCreateDataBuffer(tensor.buf, tensor.size);
        if (data == nullptr) {
            ModelDataset mdlDataset;
            mdlDataset.mdlDataPtr = dataset;
            DestroyDataset(mdlDataset);
            LogError << "Failed to create data buffer." << GetErrorInfo(APP_ERR_ACL_FAILURE);
            return nullptr;
        }

        ret = aclmdlAddDatasetBuffer(dataset, data);
        if (ret != APP_ERR_OK) {
            ModelDataset mdlDataset;
            mdlDataset.mdlDataPtr = dataset;
            aclDestroyDataBuffer(data);
            DestroyDataset(mdlDataset);
            LogError << "Failed to add dataset buffer." << GetErrorInfo(ret, "aclmdlAddDatasetBuffer");
            return nullptr;
        }
    }

    return dataset;
}

/*
* @description releas data buffer
*/
void ModelInferenceProcessorDptr::DestroyDataset(ModelDataset& dataset)
{
    // Just release the DataBuffer object and DataSet object, remain the buffer, because it is managerd by user
    if (dataset.mdlDataPtr == nullptr) {
        return;
    }
    auto mdlDataset = static_cast<aclmdlDataset*>(dataset.mdlDataPtr);
    for (size_t i = 0; i < aclmdlGetDatasetNumBuffers(mdlDataset); i++) {
        aclDataBuffer* dataBuffer = aclmdlGetDatasetBuffer(mdlDataset, i);
        if (dataBuffer != nullptr) {
            aclDestroyDataBuffer(dataBuffer);
            dataBuffer = nullptr;
        }
    }
    aclmdlDestroyDataset(mdlDataset);
}

APP_ERROR ModelInferenceProcessorDptr::SetDynamicImageSize(size_t batchSize)
{
    if (dynamicType_ != STATIC_BATCH) {
        return APP_ERR_OK;
    }

    aclmdlHW hwInfo;
    APP_ERROR ret = aclmdlGetDynamicHW((aclmdlDesc*) aclModelDesc_, -1, &hwInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get dynamic image info." << GetErrorInfo(ret, "aclmdlGetDynamicHW");
        return APP_ERR_ACL_FAILURE;
    }

    if (hwInfo.hwCount > 0) {
        dynamicImageSizes_.clear();
        dynamicType_ = DYNAMIC_HW;
        for (size_t i = 0; i < hwInfo.hwCount; ++i) {
            ImageSize imageSize = {hwInfo.hw[i][0], hwInfo.hw[i][1]};
            dynamicImageSizes_.push_back(imageSize);
        }
    }

    if (dynamicType_ == DYNAMIC_HW && batchSize > 1) {
        LogError << "the model support DYNAMIC_HW, but the batchSize is larger than 1. batchSize = " << batchSize << "."
                 << GetErrorInfo(APP_ERR_INFER_DYNAMIC_IMAGE_SIZE_FAIL);
        return APP_ERR_INFER_DYNAMIC_IMAGE_SIZE_FAIL;
    }

    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessorDptr::GetModelCurOutputDims()
{
    aclmdlIODims ioDims;
    for (size_t i = 0; i < aclmdlGetNumOutputs((aclmdlDesc*) aclModelDesc_); ++i) {
        auto ret = aclmdlGetCurOutputDims((aclmdlDesc*) aclModelDesc_, i, &ioDims);
        if (ret != ACL_ERROR_NONE) {
            LogError << "Failed to get cur output dims, index = " << i
                     << "." << GetErrorInfo(ret, "aclmdlGetCurOutputDims");
            return APP_ERR_ACL_FAILURE;
        }
        modelDesc_.outputTensors[i].tensorDims.clear();
        for (size_t j = 0; j < ioDims.dimCount; ++j) {
            modelDesc_.outputTensors[i].tensorDims.push_back(ioDims.dims[j]);
        }
    }
    outputShape_.clear();
    for (size_t i = 0; i < outputTensorCount_; ++i) {
        outputShape_.push_back(modelDesc_.outputTensors[i].tensorDims);
    }

    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessorDptr::GetTensorShape()
{
    aclmdlIODims inputdims;
    for (size_t i = 0; i < inputTensorCount_; ++i) {
        APP_ERROR ret = aclmdlGetInputDimsV2((aclmdlDesc*)aclModelDesc_, i, &inputdims);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to get input dims." << GetErrorInfo(ret, "aclmdlGetInputDimsV2");
            return APP_ERR_ACL_FAILURE;
        }
        std::vector<int64_t> tmpShape = {};
        for (size_t j = 0; j < inputdims.dimCount; ++j) {
            tmpShape.push_back(inputdims.dims[j]);
        }
        inputShape_.push_back(std::move(tmpShape));
    }

    aclmdlIODims outputdims;
    for (size_t i = 0; i < outputTensorCount_; ++i) {
        APP_ERROR ret = aclmdlGetOutputDims((aclmdlDesc*)aclModelDesc_, i, &outputdims);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to get output dims." << GetErrorInfo(ret, "aclmdlGetOutputDims");
            return APP_ERR_ACL_FAILURE;
        }
        std::vector<int64_t> tmpShape = {};
        for (size_t j = 0; j < outputdims.dimCount; ++j) {
            tmpShape.push_back(outputdims.dims[j]);
        }
        outputShape_.push_back(std::move(tmpShape));
    }
    return APP_ERR_OK;
}

void ModelInferenceProcessorDptr::GetDataType()
{
    inputDataType_.clear();
    for (size_t i = 0; i < inputTensorCount_; ++i) {
        aclDataType inputType = aclmdlGetInputDataType((aclmdlDesc*)aclModelDesc_, i);
        inputDataType_.push_back((TensorDataType)inputType);
    }
    outputDataType_.clear();
    for (size_t i = 0; i < outputTensorCount_; ++i) {
        aclDataType outputType = aclmdlGetOutputDataType((aclmdlDesc*)aclModelDesc_, i);
        outputDataType_.push_back((TensorDataType)outputType);
    }
}

APP_ERROR ModelInferenceProcessorDptr::GetBatchSize(ModelDesc& modelDesc, int64_t batchSize)
{
    aclmdlBatch mdlBatch;
    APP_ERROR ret = aclmdlGetDynamicBatch((aclmdlDesc*)aclModelDesc_, &mdlBatch);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get dynamic batch." << GetErrorInfo(ret, "aclmdlGetDynamicBatch");
        return APP_ERR_ACL_FAILURE;
    }
    dynamicBatch_.clear();
    if (mdlBatch.batchCount != 0) {
        modelDesc.batchSizes.clear();
        for (size_t i = 0; i < mdlBatch.batchCount; i++) {
            modelDesc.batchSizes.push_back(mdlBatch.batch[i]);
            dynamicBatch_.push_back(mdlBatch.batch[i]);
        }
        modelDesc.dynamicBatch = true;
        dynamicType_ = DYNAMIC_BATCH;
    } else {
        dynamicType_ = STATIC_BATCH;
        modelDesc.dynamicBatch = false;
        if (modelDesc.batchSizes.empty()) {
            modelDesc.batchSizes.push_back(batchSize);
            dynamicBatch_.push_back(batchSize);
        }
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessorDptr::CheckFile(std::string& filePath)
{
    std::string realPath;
    if (!FileUtils::RegularFilePath(filePath, realPath)) {
        LogError << "Failed to get model, the model path is invalidate." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }

    filePath = realPath;
    if (!FileUtils::IsFileValid(filePath, false)) {
        LogError << "Please check the size, owner, permission of the model." << GetErrorInfo(APP_ERR_ACL_INVALID_FILE);
        return APP_ERR_ACL_INVALID_FILE;
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessorDptr::SetDynamicType(size_t batchSize)
{
    APP_ERROR ret = SetDynamicImageSize(batchSize);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    ret = SetDynamicDimsType();
    if (ret != APP_ERR_OK) {
        return ret;
    }

    ret = SetDynamicShapeType();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessorDptr::SetDynamicDimsType()
{
    if (dynamicType_ != STATIC_BATCH) {
        return APP_ERR_OK;
    }

    size_t gearCount;
    APP_ERROR ret = aclmdlGetInputDynamicGearCount((aclmdlDesc *)aclModelDesc_, -1, &gearCount);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get input dynamic gear count." << GetErrorInfo(ret, "aclmdlGetInputDynamicGearCount");
        return APP_ERR_ACL_FAILURE;
    }
    if (gearCount > 0) {
        dynamicType_ = DYNAMIC_DIMS;
        aclmdlIODims* dims = new(std::nothrow) aclmdlIODims[gearCount];
        if (dims == nullptr) {
            LogError << "allocate memory with failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        ret = aclmdlGetInputDynamicDims((aclmdlDesc *)aclModelDesc_, -1, dims, gearCount);
        if (ret != APP_ERR_OK) {
            delete []dims;
            LogError << "Failed to get input dynamic gear info of dynamic dims model."
                     << GetErrorInfo(ret, "aclmdlGetInputDynamicDims");
            return APP_ERR_ACL_FAILURE;
        }
        for (size_t i = 0; i < gearCount; i++) {
            std::vector<uint64_t> gearShape;
            for (size_t j = 0; j < dims[i].dimCount; j++) {
                gearShape.push_back(dims[i].dims[j]);
            }
            dynamicDims_.push_back(gearShape);
        }
        delete []dims;
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessorDptr::SetDynamicShapeType()
{
    if (dynamicType_ != STATIC_BATCH) {
        return APP_ERR_OK;
    }

    for (size_t i = 0; i < inputTensorCount_; ++i) {
        for (size_t j = 0; j < inputShape_[i].size(); ++j) {
            if (inputShape_[i][j] == -1) {
                dynamicType_ = DYNAMIC_SHAPE;
                return APP_ERR_OK;
            }
        }
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessorDptr::SetDynamicImageInfo(aclmdlDataset *mdlDataset, DynamicInfo dynamicInfo)
{
    const size_t& height = dynamicInfo.imageSize.height;
    const size_t& width = dynamicInfo.imageSize.width;
    APP_ERROR ret = aclmdlSetDynamicHWSize(modelId_, mdlDataset, dynamicTensorIndex_, height, width);
    if (ret != ACL_ERROR_NONE) {
        LogError << "Dynamic image size set failed, modelId = " << modelId_ << ", index = " << dynamicTensorIndex_
                 << ", height = " << height << ", width = " << width << "."
                 << GetErrorInfo(ret, "aclmdlSetDynamicHWSize");
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessorDptr::SetDynamicDims(aclmdlDataset *mdlDataset)
{
    aclmdlIODims ioDims;
    ioDims.dimCount = 0;
    for (size_t i = 0; i < inputshape.size(); i++) {
        for (size_t j = 0; j < inputshape[i].size(); j++) {
            ioDims.dims[ioDims.dimCount + j] = inputshape[i][j];
        }
        ioDims.dimCount += inputshape[i].size();
    }
    APP_ERROR ret = aclmdlSetInputDynamicDims(modelId_, mdlDataset, dynamicTensorIndex_, &ioDims);
    if (ret != APP_ERR_OK) {
        LogError << "Dynamic dims set failed, modelId = " << modelId_ << ", index = "
                 << dynamicTensorIndex_ << ", dimCount = " << ioDims.dimCount
                 << "." << GetErrorInfo(ret, "aclmdlSetInputDynamicDims");
        return APP_ERR_ACL_FAILURE;
    }

    LogDebug << "Set ioDims (" << "dimCount:"<< ioDims.dimCount << ") successfully.";
    return APP_ERR_OK;
}

APP_ERROR ModelInferenceProcessorDptr::SetDynamicShapeInfo(aclmdlDataset *mdlDataset)
{
    aclTensorDesc* inputDesc;
    APP_ERROR ret;
    if (inputDataType_.size() < inputshape.size()) {
        LogError << "Dynamic input tensor size should not less than input shape size."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < inputshape.size(); i++) {
        aclFormat format = aclmdlGetInputFormat((aclmdlDesc*)aclModelDesc_, i);
        int64_t* dims = new(std::nothrow) int64_t[inputshape[i].size()];
        if (dims == nullptr) {
            LogError << "Allocate dynamic shape info memory failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        std::copy(inputshape[i].begin(), inputshape[i].end(), dims);
        inputDesc = aclCreateTensorDesc((aclDataType)inputDataType_[i], inputshape[i].size(), dims, format);
        ret = aclmdlSetDatasetTensorDesc(mdlDataset, inputDesc, i);
        if (ret != APP_ERR_OK) {
            delete []dims;
            aclDestroyTensorDesc(inputDesc);
            LogError << "Dynamic shape set failed, " << "index = " << i << ", dimCount = "
                     << inputshape[i].size() << GetErrorInfo(ret, "aclmdlSetDatasetTensorDesc");
            for (size_t j = 0; j<inputshape[i].size(); j++) {
                LogError << "dims[" << j << "]: " << inputshape[i][j];
            }
            return APP_ERR_COMM_FAILURE;
        }
        delete []dims;
        aclDestroyTensorDesc(inputDesc);
    }

    LogDebug << "Set dynamic shape successfully.";
    return APP_ERR_OK;
}

std::vector<std::vector<uint64_t>> ModelInferenceProcessorDptr::GetDynamicGearShape()
{
    std::vector<std::vector<uint64_t>> dynamicShape;
    switch (dynamicType_) {
        case DYNAMIC_BATCH: {
            for (size_t i = 0; i < dynamicBatch_.size(); i++) {
                std::vector<uint64_t> gearShape;
                gearShape.push_back(dynamicBatch_[i]);
                dynamicShape.push_back(gearShape);
            }
            break;
        }
        case DYNAMIC_HW: {
            for (size_t i = 0; i < dynamicImageSizes_.size(); i++) {
                std::vector<uint64_t> gearShape;
                gearShape.push_back(dynamicImageSizes_[i].height);
                gearShape.push_back(dynamicImageSizes_[i].width);
                dynamicShape.push_back(gearShape);
            }
            break;
        }
        case DYNAMIC_DIMS: {
            dynamicShape = dynamicDims_;
            break;
        }
        default:
            LogWarn << "dynamicType(" << dynamicType_ << ") is not supported to get dynamic gear info";
    }
    return dynamicShape;
}

APP_ERROR ModelInferenceProcessorDptr::InitModelAttr(ModelDesc& modelDesc)
{
    APP_ERROR ret = APP_ERR_OK;
    outputTensorCount_ = aclmdlGetNumOutputs((aclmdlDesc*)aclModelDesc_);
    for (size_t i = 0; i < outputTensorCount_; i++) {
        ret = GetOutputTensorsSize(modelDesc, i);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to get output tensors size." << GetErrorInfo(ret);
            return ret;
        }
    }

    inputTensorCount_ = aclmdlGetNumInputs((aclmdlDesc*)aclModelDesc_);
    for (size_t i = 0; i < inputTensorCount_; i++) {
        ret = GetInputTensorsSize(modelDesc, i);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to get input tensors size." << GetErrorInfo(ret);
            return ret;
        }
    }

    ret = GetTensorShape();
    if (ret != APP_ERR_OK) {
        LogError << "GetTensorShape failed." << GetErrorInfo(ret);
        return ret;
    }

    GetDataType();

    ret = SetDynamicType(modelDesc.batchSizes[0]);
    if (ret != APP_ERR_OK) {
        LogError << "SetDynamicType failed." << GetErrorInfo(ret);
        return ret;
    }

    if (dynamicType_ != STATIC_BATCH && dynamicType_ != DYNAMIC_SHAPE) {
        ret = aclmdlGetInputIndexByName((aclmdlDesc*)aclModelDesc_,
            ACL_DYNAMIC_TENSOR_NAME, &(dynamicTensorIndex_));
        if (ret != APP_ERR_OK) {
            LogError << "Get input index by name failed." << GetErrorInfo(ret, "aclmdlGetInputIndexByName");
            return APP_ERR_COMM_CONNECTION_FAILURE;
        }
    }
    return APP_ERR_OK;
}
}
#endif
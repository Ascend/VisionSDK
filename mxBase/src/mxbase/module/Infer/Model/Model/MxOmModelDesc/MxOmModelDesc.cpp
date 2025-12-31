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
 * Description: Mindx Om Model Inference Function.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#include "MxOmModelDesc.h"
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"

namespace {
    static std::mutex g_mtx;
    constexpr size_t YUV_HEIGHT = 2;
    constexpr size_t JPG_HEIGHT = 3;
    constexpr int BUFFERSIZE = 104857600;
}

namespace MxBase {
/*
* @description Om Model Init
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::Init(ModelLoadOptV2 &mdlLoadOpt, const int32_t deviceId)
{
    deviceId_ = deviceId;
    LogDebug << "Begin to MxOmModelDesc init.";
    // Set device
    APP_ERROR ret = SetDevice();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // Load model and malloc memory.
    ret = InitModel(mdlLoadOpt);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // Obatain the input/output description in model.
    ret = GetModelInputDesc();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // Obtains dynamic information about a model.
    ret = GetDynamicDesc();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = GetModelOutputDesc();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // Pre-allocate model input memory with the largest size.
    ret = MallocInputBuffer();
    if (ret != APP_ERR_OK) {
        return ret;
    }

    // Pre-allocate model output memory with the largest size.
    ret = MallocOutputBuffer();
    if (ret != APP_ERR_OK) {
        return ret;
    }

    LogDebug << "End to MxOmModelDesc init.";
    return ret;
}

/*
* @description Set device
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::SetDevice()
{
    DeviceContext context = {};
    context.devId = static_cast<int>(deviceId_);
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "Model setDevice failed, please check the state of device." << GetErrorInfo(ret);
    }
    return ret;
}

/*
* @description Om Model Infer
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::Infer(std::vector<Tensor>& inputTensors, std::vector<Tensor>& outputTensors,
                               AscendStream& stream)
{
    // Check the input tensors size
    APP_ERROR ret = CheckInputTensors(inputTensors, GetInputTensorNum(), stream);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to check input tensor." << GetErrorInfo(ret);
        stream.SetErrorCode(ret);
        return ret;
    }
    ret = CheckOutputTensors(outputTensors, GetOutputTensorNum(), stream);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to check output tensor." << GetErrorInfo(ret);
        stream.SetErrorCode(ret);
        return ret;
    }
    // Set current device and context
    ret = SetDevice();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to set device." << GetErrorInfo(ret);
        stream.SetErrorCode(ret);
        return ret;
    }
    // set pre-allocated inputtensor and get input shape
    std::vector<std::vector<uint32_t>> inputShape = {};
    ret = SetInputTensor(inputTensors, inputShape, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to set input tensor." << GetErrorInfo(ret);
        stream.SetErrorCode(ret);
        return ret;
    }
    // static check
    ret = CheckInputDType(inputTensors, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to check input dtype." << GetErrorInfo(ret);
        stream.SetErrorCode(ret);
        return ret;
    }
    // static check
    ret = CheckInputShape(inputShape, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to check input shape." << GetErrorInfo(ret);
        stream.SetErrorCode(ret);
        return ret;
    }
    // Internal model infer
    ret = ModelInference(outputTensors, inputShape, stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Model Infer failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

/*
* @description Om Model Infer
* @return std::vector<Tensor> outputTensors
*/
std::vector<Tensor> MxOmModelDesc::Infer(std::vector<Tensor>& inputTensors)
{
    APP_ERROR ret = CheckInputTensors(inputTensors, GetInputTensorNum());
    if (ret != APP_ERR_OK) {
        LogError << "Fail to check input tensor." << GetErrorInfo(ret);
        return {};
    }
    // Create output tensors
    std::vector<Tensor> outputs = {};
    ret = CreatOutputTensors(outputs, inputTensors);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to create output tensor." << GetErrorInfo(ret);
        return {};
    }
    // Model infer
    ret = Infer(inputTensors, outputs);
    if (ret != APP_ERR_OK) {
        LogError << "Infer failed, result is null." << GetErrorInfo(ret);
        return {};
    }
    // Dynamic outputs crop
    if (dynamicInfo_.dynamicType == VisionDynamicType::STATIC_BATCH) {
        return outputs;
    }
    std::vector<Tensor> dynamicOutputs;
    for (size_t i = 0; i < curOutputTensorDesc_.size(); i++) {
        auto modelOutShape = curOutputTensorDesc_[i].tensorShape;
        MxBase::TensorDType tensorDType = curOutputTensorDesc_[i].tensorDType;
        std::vector<Tensor> CropOutputs = {};
        ret = DynamicOutputsCrop(modelOutShape, tensorDType, outputs[i], CropOutputs);
        if (ret != APP_ERR_OK) {
            LogError << "Output Tensor malloc failed, infer interrupted." << GetErrorInfo(ret);
            return {};
        }
        dynamicOutputs.insert(dynamicOutputs.end(), CropOutputs.begin(), CropOutputs.end());
    }
    return dynamicOutputs;
}

uint32_t MxOmModelDesc::GetInputTensorNum() const
{
    return static_cast<uint32_t>(inputTensorDesc_.size());
}

uint32_t MxOmModelDesc::GetOutputTensorNum() const
{
    return static_cast<uint32_t>(outputTensorDesc_.size());
}

std::vector<int64_t> MxOmModelDesc::GetInputTensorShape(uint32_t index) const
{
    if (index >= GetInputTensorNum()) {
        LogError << "Index is out of range. Index: " << index << ". Input tensor size: "
                 << GetInputTensorNum() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return {};
    }
    return inputTensorDesc_[index].tensorShape;
}

std::vector<uint32_t> MxOmModelDesc::GetOutputTensorShape(uint32_t index) const
{
    if (index >= GetOutputTensorNum()) {
        LogError << "Index is out of range. Index: " << index << ". Output tensor size: "
                 << GetOutputTensorNum() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return {};
    }
    return std::vector<uint32_t>(outputTensorDesc_[index].tensorShape.begin(),
            outputTensorDesc_[index].tensorShape.end());
}

TensorDType MxOmModelDesc::GetInputTensorDataType(uint32_t index) const
{
    if (index >= GetInputTensorNum()) {
        LogError << "Index is out of range. Index: " << index << ". Input tensor size: "
                 << GetInputTensorNum() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return TensorDType::UNDEFINED;
    }
    return inputTensorDesc_[index].tensorDType;
}

TensorDType MxOmModelDesc::GetOutputTensorDataType(uint32_t index) const
{
    if (index >= GetOutputTensorNum()) {
        LogError << "Index is out of range. Index: " << index << ". Output tensor size: "
                 << GetOutputTensorNum() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return TensorDType::UNDEFINED;
    }
    return outputTensorDesc_[index].tensorDType;
}

VisionDataFormat MxOmModelDesc::GetInputFormat() const
{
    return dataFormat_;
}

std::vector<std::vector<uint64_t>> MxOmModelDesc::GetDynamicGearInfo() const
{
    std::vector<std::vector<uint64_t>> dynamicShape;
    switch (dynamicInfo_.dynamicType) {
        case VisionDynamicType::DYNAMIC_BATCH: {
            for (size_t i = 0; i < dynamicInfo_.dynamicBatch.size(); i++) {
                std::vector<uint32_t> gearShape;
                gearShape.push_back(dynamicInfo_.dynamicBatch[i]);
                dynamicShape.push_back(std::vector<uint64_t>(gearShape.begin(), gearShape.end()));
            }
            break;
        }
        case VisionDynamicType::DYNAMIC_HW: {
            for (size_t i = 0; i < dynamicInfo_.dynamicSize.size(); i++) {
                std::vector<uint32_t> gearShape;
                gearShape.push_back(dynamicInfo_.dynamicSize[i].height);
                gearShape.push_back(dynamicInfo_.dynamicSize[i].width);
                dynamicShape.push_back(std::vector<uint64_t>(gearShape.begin(), gearShape.end()));
            }
            break;
        }
        case VisionDynamicType::DYNAMIC_DIMS: {
            for (size_t i = 0; i < dynamicInfo_.dynamicDims.size(); i++) {
                dynamicShape.push_back(std::vector<uint64_t>(dynamicInfo_.dynamicDims[i].begin(),
                        dynamicInfo_.dynamicDims[i].end()));
            }
            break;
        }
        default:
            LogWarn << "dynamicType(" << (int32_t)dynamicInfo_.dynamicType
                     << ") is not supported to get dynamic gear info";
    }
    return dynamicShape;
}

VisionDynamicType MxOmModelDesc::GetDynamicType() const
{
    return dynamicInfo_.dynamicType;
}

/*
* @description Deinit model and free pre-allocate memory
* @return None
*/
MxOmModelDesc::~MxOmModelDesc()
{
    APP_ERROR ret = DeInit();
    for (size_t i = 0; i < inputTensor_.size(); i++) {
        ret = DeviceMemoryFreeFunc(inputTensor_[i].dataPtr);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to free pre-mallocated input tensor memory. index : " << i
                     << "." << GetErrorInfo(ret, "aclrtFree");
        }
        inputTensor_[i].dataPtr = nullptr;
        inputTensor_[i].dataSize = 0;
        inputTensor_[i].maxSize = 0;
    }
    for (size_t i = 0; i < outputTensor_.size(); i++) {
        ret = DeviceMemoryFreeFunc(outputTensor_[i].dataPtr);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to free pre-mallocated output tensor memory. index : " << i
                     << "." << GetErrorInfo(ret, "aclrtFree");
        }
        outputTensor_[i].dataPtr = nullptr;
        outputTensor_[i].dataSize = 0;
        outputTensor_[i].maxSize = 0;
    }
}

/*
* @description Init model, obtain the model description and create stream
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::InitModel(ModelLoadOptV2 &mdlLoadOpt)
{
    // create model stream
    APP_ERROR ret = aclrtCreateStream(&stream_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to create device stream." << GetErrorInfo(ret);
        return APP_ERR_ACL_FAILURE;
    }
    // loads model data with option
    ret = LoadModel(mdlLoadOpt);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to create device stream." << GetErrorInfo(ret);
        if (stream_ != nullptr) {
            ret = aclrtDestroyStream(stream_);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to Destroy stream." << GetErrorInfo(ret);
            }
                stream_ = nullptr;
            }
        return APP_ERR_ACL_FAILURE;
    }
    aclModelDesc_ = aclmdlCreateDesc();
    // Obtains the model description
    ret = aclmdlGetDesc((aclmdlDesc*)aclModelDesc_, modelId_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to obtain the model description." << GetErrorInfo(ret);
        if (aclModelDesc_ != nullptr) {
            ret = aclmdlDestroyDesc(static_cast<aclmdlDesc*>(aclModelDesc_));
            if (ret != APP_ERR_OK) {
                LogError << "Failed to free model desc." << GetErrorInfo(ret, "aclmdlDestroyDesc");
            }
            aclModelDesc_ = nullptr;
        }
        if (stream_ != nullptr) {
            ret = aclrtDestroyStream(stream_);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to Destroy stream." << GetErrorInfo(ret);
            }
            stream_ = nullptr;
        }
        return APP_ERR_ACL_FAILURE;
    }
    return APP_ERR_OK;
}

/*
* @description DeInit model and destroy the model description & stream
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::DeInit(void)
{
    APP_ERROR subRet = APP_ERR_OK;
    APP_ERROR ret = aclmdlUnload(modelId_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to unload model." << GetErrorInfo(ret, "aclmdlUnload");
        subRet = APP_ERR_ACL_FAILURE;
    }
    if (aclModelDesc_ != nullptr) {
        ret = aclmdlDestroyDesc(static_cast<aclmdlDesc*>(aclModelDesc_));
        if (ret != APP_ERR_OK) {
            LogError << "Failed to free model desc." << GetErrorInfo(ret, "aclmdlDestroyDesc");
            subRet = APP_ERR_ACL_FAILURE;
        }
        aclModelDesc_ = nullptr;
    }
    if (stream_ != nullptr) {
        ret = aclrtDestroyStream(stream_);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to Destroy stream." << GetErrorInfo(ret);
            subRet = APP_ERR_ACL_FAILURE;
        }
        stream_ = nullptr;
    }
    return subRet;
}

/*
* @description Get model input description
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::GetModelInputDesc()
{
    APP_ERROR ret = APP_ERR_OK;
    std::vector<VisionTensorDesc>().swap(inputTensorDesc_);
    size_t inputTensorCount = aclmdlGetNumInputs((aclmdlDesc*)aclModelDesc_);
    for (size_t i = 0; i < inputTensorCount; i++) {
        // obtain the input shape / datatype / format
        aclmdlIODims inputDims;
        ret = aclmdlGetInputDimsV2((aclmdlDesc*)aclModelDesc_, i, &inputDims);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to get input dims by V2." << GetErrorInfo(ret, "aclmdlGetInputDimsV2");
            return APP_ERR_ACL_FAILURE;
        }
        if (inputDims.dimCount == 0) {
            ret = aclmdlGetInputDims((aclmdlDesc*)aclModelDesc_, i, &inputDims);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to get input dims." << GetErrorInfo(ret, "aclmdlGetInputDims");
                return APP_ERR_ACL_FAILURE;
            }
            if (inputDims.dimCount == 0) {
                LogError << "Input dims count is zero which is invalid." << GetErrorInfo(APP_ERR_ACL_API_NOT_SUPPORT);
                return APP_ERR_ACL_API_NOT_SUPPORT;
            }
        }
        aclDataType dataType = aclmdlGetInputDataType((aclmdlDesc*)aclModelDesc_, i);
        VisionTensorDesc inputTensorDesc = VisionTensorDesc();
        for (size_t j = 0; j < inputDims.dimCount; j++) {
            inputTensorDesc.tensorShape.push_back(inputDims.dims[j]);
        }
        inputTensorDesc.tensorDType = static_cast<TensorDType>(dataType);
        inputTensorDesc_.push_back(std::move(inputTensorDesc));
        aclFormat format = aclmdlGetInputFormat((aclmdlDesc*)aclModelDesc_, i);
        if (format == ACL_FORMAT_NCHW) {
            dataFormat_ = VisionDataFormat::NCHW;
        } else if (format == ACL_FORMAT_NHWC) {
            dataFormat_ = VisionDataFormat::NHWC;
        }
    }
    return ret;
}

/*
* @description Pre-allocate input buffer with the biggest size
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::MallocInputBuffer()
{
    // malloc the input data
    APP_ERROR ret = APP_ERR_OK;
    for (size_t i = 0; i < inputTensorDesc_.size(); i++) {
        size_t dataSize = aclmdlGetInputSizeByIndex((aclmdlDesc*)aclModelDesc_, i);
        if (dataSize == 0x0) {
            dataSize = (size_t)BUFFERSIZE;
        }
        VisionTensorBase inputTensor = {};
        void* dataBuffer = nullptr;
        ret = DeviceMemoryMallocFunc(&dataBuffer, dataSize, MX_MEM_MALLOC_NORMAL_ONLY);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc device memory. deviceID is : "<< deviceId_
                     << "." << GetErrorInfo(ret, "aclrtMalloc");
            return APP_ERR_ACL_BAD_ALLOC;
        }
        inputTensor.dataPtr = dataBuffer;
        inputTensor.dataSize = dataSize;
        inputTensor.maxSize = dataSize;
        inputTensor_.push_back(inputTensor);
    }
    return ret;
}

/*
* @description Pre-allocate output buffer for dynamic shape model
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::MallocOutputBuffer()
{
    // malloc the output data
    APP_ERROR ret = APP_ERR_OK;
    for (size_t i = 0; i < outputTensorDesc_.size(); i++) {
        size_t dataSize = 0;
        if (dynamicInfo_.dynamicType != VisionDynamicType::DYNAMIC_SHAPE) {
            dataSize = aclmdlGetOutputSizeByIndex((aclmdlDesc*)aclModelDesc_, i);
        } else {
            dataSize = (size_t)BUFFERSIZE;
        }
        VisionTensorBase outputTensor = {};
        void* dataBuffer = nullptr;
        ret = DeviceMemoryMallocFunc(&dataBuffer, dataSize, MX_MEM_MALLOC_NORMAL_ONLY);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc device memory for output. deviceID is : "<< deviceId_
                     << "." << GetErrorInfo(ret, "aclrtMalloc");
            return APP_ERR_ACL_BAD_ALLOC;
        }
        outputTensor.dataPtr = dataBuffer;
        outputTensor.dataSize = dataSize;
        outputTensor.maxSize = dataSize;
        outputTensor_.push_back(outputTensor);
    }
    return ret;
}

/*
* @description Get model output description
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::GetModelOutputDesc()
{
    APP_ERROR ret = APP_ERR_OK;
    std::vector<VisionTensorDesc>().swap(outputTensorDesc_);
    size_t outputTensorCount = aclmdlGetNumOutputs((aclmdlDesc*)aclModelDesc_);
    for (size_t i = 0; i < outputTensorCount; i++) {
        // obtain the output shape / datatype
        aclmdlIODims outputDims;
        if (dynamicInfo_.dynamicType == VisionDynamicType::DYNAMIC_SHAPE) {
            ret = aclmdlGetOutputDims((aclmdlDesc*)aclModelDesc_, i, &outputDims);
        } else {
            ret = aclmdlGetCurOutputDims((aclmdlDesc*)aclModelDesc_, i, &outputDims);
        }
        if (ret != APP_ERR_OK || outputDims.dimCount > ACL_MAX_DIM_CNT) {
            LogError << "Failed to get output dims or the it exceeds the upper limit."
                     << GetErrorInfo(ret, "aclmdlGetCurOutputDims or aclmdlGetCurOutputDims");
            return APP_ERR_ACL_FAILURE;
        }
        aclDataType dataType = aclmdlGetOutputDataType((aclmdlDesc*)aclModelDesc_, i);
        VisionTensorDesc outputTensorDesc = VisionTensorDesc();
        for (size_t j = 0; j < outputDims.dimCount; j++) {
            outputTensorDesc.tensorShape.push_back(outputDims.dims[j]);
        }
        outputTensorDesc.tensorDType = static_cast<TensorDType>(dataType);
        outputTensorDesc_.push_back(std::move(outputTensorDesc));
    }
    return ret;
}

/*
* @description Get model current output description
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::GetModelCurOutputDesc()
{
    APP_ERROR ret = APP_ERR_OK;
    std::vector<VisionTensorDesc>().swap(curOutputTensorDesc_);
    size_t outputTensorCount = aclmdlGetNumOutputs((aclmdlDesc*)aclModelDesc_);
    for (size_t i = 0; i < outputTensorCount; i++) {
        // obtain the output shape / datatype
        aclmdlIODims outputDims;
        ret = aclmdlGetCurOutputDims((aclmdlDesc*)aclModelDesc_, i, &outputDims);
        if (ret != APP_ERR_OK || outputDims.dimCount > ACL_MAX_DIM_CNT) {
            LogError << "Failed to get output dims or it exceeds the upper limit."
                     << GetErrorInfo(ret, "aclmdlGetCurOutputDims");
            return APP_ERR_ACL_FAILURE;
        }
        VisionTensorDesc outputTensorDesc = VisionTensorDesc();
        for (size_t j = 0; j < outputDims.dimCount; j++) {
            outputTensorDesc.tensorShape.push_back(outputDims.dims[j]);
        }
        outputTensorDesc.tensorDType = outputTensorDesc_[i].tensorDType;
        curOutputTensorDesc_.push_back(std::move(outputTensorDesc));
    }
    return ret;
}

/*
* @description Get model dynamic description
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::GetDynamicDesc()
{
    // obtain DYNAMIC_BATCH info
    APP_ERROR ret = GetDynamicBatch();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // obtain DYNAMIC_HW info
    ret = GetDynamicSize();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // obtain DYNAMIC_DIMS info
    ret = GetDynamicDims();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // obtain DYNAMIC_SHAPE type
    ret = GetDynamicShapeType();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // obtain dynamic index
    ret = GetDynamicIndex();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return APP_ERR_OK;
}

/*
* @description Get model dynamic batch description
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::GetDynamicBatch()
{
    aclmdlBatch mdlBatch;
    APP_ERROR ret = aclmdlGetDynamicBatch((aclmdlDesc*)aclModelDesc_, &mdlBatch);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get dynamic batch." << GetErrorInfo(ret, "aclmdlGetDynamicBatch");
        return APP_ERR_ACL_FAILURE;
    }
    std::vector<uint32_t>().swap(dynamicInfo_.dynamicBatch);
    if (mdlBatch.batchCount != 0) {
        for (size_t i = 0; i < mdlBatch.batchCount; i++) {
            dynamicInfo_.dynamicBatch.push_back(static_cast<uint32_t>(mdlBatch.batch[i]));
        }
        dynamicInfo_.dynamicType = VisionDynamicType::DYNAMIC_BATCH;
    } else {
        dynamicInfo_.dynamicType = VisionDynamicType::STATIC_BATCH;
        dynamicInfo_.dynamicBatch.push_back(static_cast<uint32_t>(inputTensorDesc_[0].tensorShape[0]));
    }
    return ret;
}

/*
* @description Get model dynamic size description
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::GetDynamicSize()
{
    if (dynamicInfo_.dynamicType != VisionDynamicType::STATIC_BATCH) {
        return APP_ERR_OK;
    }
    aclmdlHW hwInfo;
    APP_ERROR ret = aclmdlGetDynamicHW((aclmdlDesc*) aclModelDesc_, -1, &hwInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get dynamic image info." << GetErrorInfo(ret, "aclmdlGetDynamicHW");
        return APP_ERR_ACL_FAILURE;
    }
    std::vector<MxBase::Size>().swap(dynamicInfo_.dynamicSize);
    if (hwInfo.hwCount > 0) {
        dynamicInfo_.dynamicType = VisionDynamicType::DYNAMIC_HW;
        for (size_t i = 0; i < hwInfo.hwCount; ++i) {
            Size imageSize = {(uint32_t)hwInfo.hw[i][0x1], (uint32_t)hwInfo.hw[i][0x0]};
            dynamicInfo_.dynamicSize.push_back(imageSize);
        }
    }
    return ret;
}

/*
* @description Get model dynamic dims description
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::GetDynamicDims()
{
    if (dynamicInfo_.dynamicType != VisionDynamicType::STATIC_BATCH) {
        return APP_ERR_OK;
    }
    size_t gearCount;
    APP_ERROR ret = aclmdlGetInputDynamicGearCount((aclmdlDesc *)aclModelDesc_, -1, &gearCount);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to get input dynamic gear count." << GetErrorInfo(ret, "aclmdlGetInputDynamicGearCount");
        return APP_ERR_ACL_FAILURE;
    }
    if (gearCount > 0) {
        std::vector<std::vector<uint32_t>>().swap(dynamicInfo_.dynamicDims);
        dynamicInfo_.dynamicType = VisionDynamicType::DYNAMIC_DIMS;
        aclmdlIODims* dims = new(std::nothrow) aclmdlIODims[gearCount];
        if (dims == nullptr) {
            LogError << "allocate dynamic dims info memory failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
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
            std::vector<uint32_t> gearShape;
            for (size_t j = 0; j < dims[i].dimCount; j++) {
                gearShape.push_back(static_cast<uint32_t>(dims[i].dims[j]));
            }
            dynamicInfo_.dynamicDims.push_back(gearShape);
        }
        delete []dims;
    }
    return ret;
}

/*
* @description Get model fully dynamic shape type
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::GetDynamicShapeType()
{
    if (dynamicInfo_.dynamicType != VisionDynamicType::STATIC_BATCH) {
        return APP_ERR_OK;
    }
    for (size_t i = 0; i < inputTensorDesc_.size(); ++i) {
        for (size_t j = 0; j < inputTensorDesc_[i].tensorShape.size(); ++j) {
            if (inputTensorDesc_[i].tensorShape[j] == -1) {
                dynamicInfo_.dynamicType = VisionDynamicType::DYNAMIC_SHAPE;
                return APP_ERR_OK;
            }
        }
    }
    return APP_ERR_OK;
}

/*
* @description Get model dynamic index
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::GetDynamicIndex()
{
    if (dynamicInfo_.dynamicType != VisionDynamicType::STATIC_BATCH &&
        dynamicInfo_.dynamicType != VisionDynamicType::DYNAMIC_SHAPE) {
        APP_ERROR ret = aclmdlGetInputIndexByName((aclmdlDesc*)aclModelDesc_,
            ACL_DYNAMIC_TENSOR_NAME, &(dynamicInfo_.dynamicTensorIndex));
        if (ret != APP_ERR_OK) {
            LogError << "Get input index by name failed." << GetErrorInfo(ret, "aclmdlGetInputIndexByName");
            return APP_ERR_COMM_CONNECTION_FAILURE;
        }
    }
    return APP_ERR_OK;
}

/*
* @description Create output tensors with input tensor info
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::CreatOutputTensors(std::vector<Tensor>& outputs, const std::vector<Tensor>& inputTensors)
{
    for (uint32_t i = 0; i < GetOutputTensorNum(); ++i) {
        auto modelOutShape = GetOutputTensorShape(i);
        if (modelOutShape.size() == 0) {
            LogError << "No." << i << "'s output Tensor is NULL, please check validation of model!"
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        if (dynamicInfo_.dynamicType == VisionDynamicType::DYNAMIC_BATCH) {
            modelOutShape[0] = inputTensors[0].GetShape()[0]; // compatible with dynamic batch
        }
        MxBase::TensorDType tensorDType = GetOutputTensorDataType(i);
        std::vector<Tensor> curOutputs = {};
        APP_ERROR ret = CreateOutputs(modelOutShape, tensorDType, curOutputs, i);
        if (ret != APP_ERR_OK) {
            LogError << "Output Tensor malloc failed, infer interrupted." << GetErrorInfo(ret);
            return ret;
        }
        outputs.insert(outputs.end(), curOutputs.begin(), curOutputs.end());
    }
    return APP_ERR_OK;
}

/*
* @description Create output Tensor with shape, type
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::CreateOutputs(std::vector<uint32_t>& modelOutShape, MxBase::TensorDType tensorDType,
    std::vector<MxBase::Tensor>& outputs, uint32_t index)
{
    APP_ERROR ret = APP_ERR_OK;
    if (dynamicInfo_.dynamicType == VisionDynamicType::DYNAMIC_SHAPE) { // pre-allocate a large piece of memory
        std::vector<uint32_t> dynamicShape = {0x1, 0x1, 0x1, (uint32_t)BUFFERSIZE};
        MxBase::Tensor tensor(outputTensor_[index].dataPtr, dynamicShape, tensorDType, deviceId_);
        outputs.push_back(tensor);
    } else {
        MxBase::Tensor tensor(outputTensor_[index].dataPtr, modelOutShape, tensorDType, deviceId_);
        outputs.push_back(tensor);
    }
    return ret;
}

/*
* @description Crop outputs with model shape
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::DynamicOutputsCrop(std::vector<int64_t>& modelOutShape, MxBase::TensorDType& tensorDType,
    MxBase::Tensor& output, std::vector<MxBase::Tensor>& outputs)
{
    APP_ERROR ret = APP_ERR_OK;
    std::vector<uint32_t> tensorShape = std::vector<uint32_t>(modelOutShape.begin(), modelOutShape.end());
    MxBase::Tensor tensor(tensorShape, tensorDType, deviceId_);
    ret = tensor.Malloc();
    if (ret != APP_ERR_OK) {
        LogError << "Output Tensor malloc failed, infer interrupted." << GetErrorInfo(ret);
        return ret;
    }
    void* srcPtrData = output.GetData();
    size_t dataSize = tensor.GetByteSize();
    void* dsrPtrData = tensor.GetData();
    aclrtMemcpyKind memcpyKind = deviceId_ == -1 ? ACL_MEMCPY_DEVICE_TO_HOST :
                                                   ACL_MEMCPY_DEVICE_TO_DEVICE;

    ret = aclrtMemcpy(dsrPtrData, dataSize, srcPtrData, dataSize, memcpyKind);
    if (ret != APP_ERR_OK) {
        LogError << "Memcpy ptrData failed." << GetErrorInfo(ret, "aclrtMemcpy");
        return APP_ERR_ACL_BAD_COPY;
    }
    outputs.push_back(tensor);
    return ret;
}

/*
* @description Check input shape
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::CheckInputShape(std::vector<std::vector<uint32_t>>& inputTensorShape, AscendStream& Stream)
{
    for (size_t i = 0; i < inputTensorShape.size(); i++) {
        if (inputTensorDesc_[i].tensorShape.size() != inputTensorShape[i].size()) {
            LogError << "Number of InputTensors is not equal to " <<  inputTensorDesc_[i].tensorShape.size() << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        for (size_t j = 0; j < inputTensorShape[i].size(); j++) {
            if (inputTensorDesc_[i].tensorShape[j] != -1 &&
                inputTensorDesc_[i].tensorShape[j] != inputTensorShape[i][j]) {
                LogError << "InputTensor Shape mismatches.\n" << "No." << i << " inputTensor, No." << j << " dim:"
                         << " modelInputShape is " << inputTensorShape[i][j] << ", actualInputShape is "
                         << inputTensorDesc_[i].tensorShape[j] << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                Stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
    }
    return APP_ERR_OK;
}

/*
* @description Check input data type
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::CheckInputDType(std::vector<Tensor>& inputTensors, AscendStream& Stream)
{
    for (size_t i = 0; i < inputTensors.size(); i++) {
        aclDataType acldtype = aclmdlGetInputDataType((aclmdlDesc*)aclModelDesc_, i);
        MxBase::TensorDType tensorDType = inputTensors[i].GetDataType();
        if (static_cast<int>(acldtype) != static_cast<int>(tensorDType)) {
            LogError << "InputTensor Data Type mismatches.\n" << "No." << i << " inputTensor, modelInput data type is "
                     << static_cast<int>(acldtype) << ", actualInput data type is " << static_cast<int>(tensorDType)
                     << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            Stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

/*
* @description Copy input data to pre-allocated memory
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::SetInputTensor(std::vector<Tensor>& inputTensors,
    std::vector<std::vector<uint32_t>>& inputShape, AscendStream& Stream)
{
    // memory copy the input data to pre-allocated memory
    APP_ERROR ret = APP_ERR_OK;
    for (size_t i = 0; i < inputTensors.size(); i++) {
        void* srcDataptr = inputTensors[i].GetData();
        size_t dataSize = inputTensors[i].GetByteSize();
        aclrtMemcpyKind memcpyKind = inputTensors[i].GetDeviceId() == -1 ? ACL_MEMCPY_HOST_TO_DEVICE :
                                                                           ACL_MEMCPY_DEVICE_TO_DEVICE;
        if (srcDataptr == nullptr || inputTensor_[i].dataPtr == nullptr) {
            LogError << "Memcpy failed, inputTensors or inputTensor_ is nullptr."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            Stream.SetErrorCode(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        if (dataSize > inputTensor_[i].maxSize || static_cast<int64_t>(dataSize) < 0) {
            LogError << "Please check inputTensors datasize: " << dataSize
                     << ", or inputTensor_: " << inputTensor_[i].maxSize << "."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            Stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        ret = aclrtMemcpy(inputTensor_[i].dataPtr, inputTensor_[i].maxSize, srcDataptr, dataSize, memcpyKind);
        if (ret != APP_ERR_OK) {
            LogError << "Memcpy ptrData failed." << GetErrorInfo(ret, "aclrtMemcpy");
            Stream.SetErrorCode(APP_ERR_ACL_BAD_COPY);
            return APP_ERR_ACL_BAD_COPY;
        }
        inputTensor_[i].dataSize = dataSize;
        std::vector<uint32_t> shape = inputTensors[i].GetShape();
        inputShape.push_back(shape);
    }
    return ret;
}

/*
* @description internal model inference function
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::ModelInference(std::vector<Tensor>& outputTensors,
                                        std::vector<std::vector<uint32_t>>& inputShape, AscendStream& stream)
{
    aclmdlDataset* inputDataset = CreateAndFillDataset(inputTensor_);
    if (inputDataset == nullptr) {
        stream.SetErrorCode(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    APP_ERROR ret = ModelSetDynamicInfo(inputDataset, inputShape, stream);
    if (ret != APP_ERR_OK) {
        DestroyDataset(inputDataset);
        inputDataset = nullptr;
        LogError << "Failed to set model dynamic info." << GetErrorInfo(ret);
        stream.SetErrorCode(ret);
        return ret;
    }
    aclmdlDataset* outputDataset = CreateAndFillDataset(outputTensors);
    if (outputDataset == nullptr) {
        DestroyDataset(inputDataset);
        inputDataset = nullptr;
        stream.SetErrorCode(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    APP_ERROR subRet = APP_ERR_OK;
    // Execute model infer
    {
        std::lock_guard<std::mutex> lock(g_mtx);
        ret = aclmdlExecuteAsync(modelId_, inputDataset, outputDataset, stream_);
    }
    if (ret != APP_ERR_OK) {
        LogError << "Failed to model infer execute." << GetErrorInfo(ret, "aclmdlExecute");
        subRet = APP_ERR_ACL_FAILURE;
        stream.SetErrorCode(subRet);
        SyncAndFree(inputDataset, outputDataset, stream);
        return APP_ERR_ACL_FAILURE;
    }
    if (dynamicInfo_.dynamicType == VisionDynamicType::DYNAMIC_SHAPE) {
        ret = GetOutputShapeInDynamicShapeMode(outputDataset, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to get output shape for dynamic shape model." << GetErrorInfo(ret);
            stream.SetErrorCode(ret);
            subRet = APP_ERR_ACL_FAILURE;
        }
    }
    ret = SyncAndFree(inputDataset, outputDataset, stream);
    if (ret != APP_ERR_OK) {
        subRet = APP_ERR_ACL_FAILURE;
    }
    return subRet;
}

APP_ERROR MxOmModelDesc::SyncAndFree(aclmdlDataset* inputDataset, aclmdlDataset* outputDataset, AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = aclrtSynchronizeStream(stream_);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Failed to synchronize stream." << GetErrorInfo(ret);
    }
    DestroyDataset(inputDataset);
    DestroyDataset(outputDataset);
    return ret;
}
/*
* @description Create aclmdldataset with Tensor
* @return aclmdlDataset*
*/
aclmdlDataset* MxOmModelDesc::CreateAndFillDataset(std::vector<Tensor>& tensors)
{
    APP_ERROR ret = APP_ERR_OK;
    aclmdlDataset* mdlDataSet = aclmdlCreateDataset();
    if (mdlDataSet == nullptr) {
        LogError << "Failed to create model input." << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return nullptr;
    }
    for (const auto& tensor : tensors) {
        aclDataBuffer* data = aclCreateDataBuffer(tensor.GetData(), tensor.GetByteSize());
        if (data == nullptr) {
            DestroyDataset(mdlDataSet);
            LogError << "Failed to create data buffer." << GetErrorInfo(APP_ERR_ACL_FAILURE);
            return nullptr;
        }

        ret = aclmdlAddDatasetBuffer(mdlDataSet, data);
        if (ret != APP_ERR_OK) {
            aclDestroyDataBuffer(data);
            DestroyDataset(mdlDataSet);
            LogError << "Failed to add dataset buffer." << GetErrorInfo(ret, "aclmdlAddDatasetBuffer");
            return nullptr;
        }
    }
    return mdlDataSet;
}

/*
* @description Create aclmdldataset with VisionTensorBase
* @return aclmdlDataset*
*/
aclmdlDataset* MxOmModelDesc::CreateAndFillDataset(std::vector<VisionTensorBase>& tensorBase)
{
    APP_ERROR ret = APP_ERR_OK;
    aclmdlDataset* mdlDataSet = aclmdlCreateDataset();
    if (mdlDataSet == nullptr) {
        LogError << "Failed to create model input." << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return nullptr;
    }
    for (const auto& tensor : tensorBase) {
        aclDataBuffer* data = aclCreateDataBuffer(tensor.dataPtr, tensor.dataSize);
        if (data == nullptr) {
            DestroyDataset(mdlDataSet);
            LogError << "Failed to create data buffer." << GetErrorInfo(APP_ERR_ACL_FAILURE);
            return nullptr;
        }

        ret = aclmdlAddDatasetBuffer(mdlDataSet, data);
        if (ret != APP_ERR_OK) {
            aclDestroyDataBuffer(data);
            DestroyDataset(mdlDataSet);
            LogError << "Failed to add dataset buffer." << GetErrorInfo(ret, "aclmdlAddDatasetBuffer");
            return nullptr;
        }
    }
    return mdlDataSet;
}

/*
* @description Destroy aclmdldataset
* @return None
*/
void MxOmModelDesc::DestroyDataset(aclmdlDataset* dataset)
{
    // Just release the DataBuffer object and DataSet object, remain the buffer, because it is managerd by user
    if (dataset == nullptr) {
        return;
    }
    for (size_t i = 0; i < aclmdlGetDatasetNumBuffers(dataset); i++) {
        aclDataBuffer* dataBuffer = aclmdlGetDatasetBuffer(dataset, i);
        if (dataBuffer != nullptr) {
            aclDestroyDataBuffer(dataBuffer);
            dataBuffer = nullptr;
        }
    }
    aclmdlDestroyDataset(dataset);
}

/*
* @description Get model output shape in fully dynamic shape mode
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::GetOutputShapeInDynamicShapeMode(aclmdlDataset *output, AscendStream& Stream)
{
    APP_ERROR ret = APP_ERR_OK;
    std::vector<VisionTensorDesc>().swap(curOutputTensorDesc_);
    for (size_t i = 0; i < GetOutputTensorNum(); i++) {
        VisionTensorDesc outputTensor = {};
        aclTensorDesc *outputTensorDesc = aclmdlGetDatasetTensorDesc(output, i);
        size_t numDims = aclGetTensorDescNumDims(outputTensorDesc);
        if (numDims == ACL_UNKNOWN_RANK) {
            LogError << "Get dynamic shape output tensor num dims failed." << GetErrorInfo(APP_ERR_ACL_FAILURE);
            Stream.SetErrorCode(APP_ERR_ACL_FAILURE);
            return APP_ERR_ACL_FAILURE;
        }
        for (size_t j = 0; j < numDims; j++) {
            int64_t dimSize;
            ret = aclGetTensorDescDimV2(outputTensorDesc, j, &dimSize);
            outputTensor.tensorShape.push_back(dimSize);
        }
        outputTensor.tensorDType = outputTensorDesc_[i].tensorDType;
        curOutputTensorDesc_.push_back(std::move(outputTensor));
    }
    return ret;
}

/*
* @description Set model dynamic type and get current model output shape
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::ModelSetDynamicInfo(aclmdlDataset* mdlDataset,
    std::vector<std::vector<uint32_t>>& modelShape, AscendStream& Stream)
{
    switch (dynamicInfo_.dynamicType) {
        case VisionDynamicType::STATIC_BATCH:
            return APP_ERR_OK;
        case VisionDynamicType::DYNAMIC_BATCH: {
            APP_ERROR ret = SetDynamicBatch(mdlDataset, modelShape[0x0][0x0], Stream);
            if (ret != APP_ERR_OK) {
                LogError << "Dynamic batch set failed." << GetErrorInfo(ret);
                Stream.SetErrorCode(ret);
                return ret;
            }
            return GetModelCurOutputDesc();
        }
        case VisionDynamicType::DYNAMIC_HW: {
            APP_ERROR ret = SetDynamicSize(mdlDataset, modelShape, Stream);
            if (ret != APP_ERR_OK) {
                LogError << "Dynamic size set failed." << GetErrorInfo(ret);
                Stream.SetErrorCode(ret);
                return ret;
            }
            return GetModelCurOutputDesc();
        }
        case VisionDynamicType::DYNAMIC_DIMS: {
            APP_ERROR ret = SetDynamicDims(mdlDataset, modelShape, Stream);
            if (ret != APP_ERR_OK) {
                LogError << "Dynamic dims set failed." << GetErrorInfo(ret);
                Stream.SetErrorCode(ret);
                return ret;
            }
            return GetModelCurOutputDesc();
        }
        case VisionDynamicType::DYNAMIC_SHAPE: {
            APP_ERROR ret = SetDynamicShape(mdlDataset, modelShape, Stream);
            if (ret != APP_ERR_OK) {
                LogError << "Dynamic dims set failed." << GetErrorInfo(ret);
                Stream.SetErrorCode(ret);
                return ret;
            }
            break;
        }
        default:
            LogError << "dynamicType(" << int32_t(dynamicInfo_.dynamicType) << ") is not support."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            Stream.SetErrorCode(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
    }

    return APP_ERR_OK;
}

/*
* @description Set model with dynamic batch
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::SetDynamicBatch(aclmdlDataset* mdlDataset, size_t dynamicBatch, AscendStream& Stream)
{
    APP_ERROR ret = aclmdlSetDynamicBatchSize(modelId_, mdlDataset, dynamicInfo_.dynamicTensorIndex, dynamicBatch);
    if (ret != APP_ERR_OK) {
        LogError << "Dynamic batch set failed, modelId = " << modelId_ << ", index = "
                 << dynamicInfo_.dynamicTensorIndex << ", dynamicBatchSize = " << dynamicBatch
                 << GetErrorInfo(ret, "aclmdlSetDynamicBatchSize");
        Stream.SetErrorCode(ret);
        return APP_ERR_ACL_FAILURE;
    }
    LogDebug << "Set dynamicBatchSize(" << dynamicBatch << ") successfully.";
    return ret;
}

/*
* @description Set model with dynamic size
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::SetDynamicSize(aclmdlDataset* mdlDataset, std::vector<std::vector<uint32_t>>& modelShape,
                                        AscendStream& Stream)
{
    std::vector<uint32_t> dynamicsize = {};
    for (size_t i = 0; i < modelShape.size(); i++) {
        if (modelShape[i].size() == 0x4) {
            dynamicsize = modelShape[i];
            break;
        }
    }
    std::vector<uint32_t> index = {};
    if (dataFormat_ == VisionDataFormat::NCHW) {
        index = {0x2, 0x3};
    } else if (dataFormat_ == VisionDataFormat::NHWC) {
        index = {0x1, 0x2};
    }
    size_t height = dynamicsize[index[0x0]];
    size_t width = dynamicsize[index[0x1]];
    if (IsYUVData(dynamicsize)) {
        height = height / JPG_HEIGHT * YUV_HEIGHT;
    }

    APP_ERROR ret = aclmdlSetDynamicHWSize(modelId_, mdlDataset, dynamicInfo_.dynamicTensorIndex, height, width);
    if (ret != ACL_ERROR_NONE) {
        LogError << "Dynamic image size set failed, modelId = " << modelId_ << ", index = "
                 << dynamicInfo_.dynamicTensorIndex << ", height = " << height << ", width = " << width
                 << "." << GetErrorInfo(ret, "aclmdlSetDynamicHWSize");
        Stream.SetErrorCode(ret);
        return APP_ERR_ACL_FAILURE;
    }
    LogDebug << "Dynamic image size set success, modelId = " << modelId_ <<
            ", index = " << dynamicInfo_.dynamicTensorIndex << ", height = " << height << ", width = " << width;
    return ret;
}

/*
* @description Set model with dynamic dims
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::SetDynamicDims(aclmdlDataset *mdlDataset, std::vector<std::vector<uint32_t>>& modelShape,
                                        AscendStream& Stream)
{
    aclmdlIODims ioDims;
    ioDims.dimCount = 0;
    for (size_t i = 0; i < modelShape.size(); i++) {
        for (size_t j = 0; j < modelShape[i].size(); j++) {
            ioDims.dims[ioDims.dimCount + j] = modelShape[i][j];
        }
        ioDims.dimCount += modelShape[i].size();
    }
    APP_ERROR ret = aclmdlSetInputDynamicDims(modelId_, mdlDataset, dynamicInfo_.dynamicTensorIndex, &ioDims);
    if (ret != APP_ERR_OK) {
        LogError << "Dynamic dims set failed, modelId = " << modelId_ << ", index = "
                 << dynamicInfo_.dynamicTensorIndex << ", dimCount = " << ioDims.dimCount
                 << GetErrorInfo(ret, "aclmdlSetInputDynamicDims");
        Stream.SetErrorCode(ret);
        return APP_ERR_ACL_FAILURE;
    }
    LogDebug << "Set ioDims (" << "dimCount:"<< ioDims.dimCount << ") successfully.";
    return APP_ERR_OK;
}

/*
* @description Set model with fully dynamic shape
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::SetDynamicShape(aclmdlDataset *mdlDataset, std::vector<std::vector<uint32_t>>& modelShape,
                                         AscendStream& Stream)
{
    aclTensorDesc* inputDesc;
    APP_ERROR ret = APP_ERR_OK;
    for (size_t i = 0; i < modelShape.size(); i++) {
        aclFormat format = aclmdlGetInputFormat((aclmdlDesc*)aclModelDesc_, i);
        int64_t* dims = new(std::nothrow) int64_t[modelShape[i].size()];
        if (dims == nullptr) {
            LogError << "Allocate dynamic shape info memory failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            Stream.SetErrorCode(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        std::copy(modelShape[i].begin(), modelShape[i].end(), dims);
        inputDesc = aclCreateTensorDesc((aclDataType)inputTensorDesc_[i].tensorDType,
            modelShape[i].size(), dims, format);
        ret = aclmdlSetDatasetTensorDesc(mdlDataset, inputDesc, i);
        if (ret != APP_ERR_OK) {
            delete []dims;
            aclDestroyTensorDesc(inputDesc);
            LogError << "Dynamic shape set failed, " << "index = "
                     << i << ", dimCount = " << modelShape[i].size() << "."
                     << GetErrorInfo(ret, "aclmdlSetDatasetTensorDesc");
            for (size_t j = 0; j < modelShape[i].size(); j++) {
                LogError << "dims[" << j << "]: " << modelShape[i][j];
            }
            Stream.SetErrorCode(ret);
            return APP_ERR_ACL_FAILURE;
        }
        delete []dims;
        aclDestroyTensorDesc(inputDesc);
    }
    LogDebug << "Set dynamic shape successfully.";
    return APP_ERR_OK;
}

/*
* @description Determine if the input is in YUV format
* @return bool true/false
*/
bool MxOmModelDesc::IsYUVData(std::vector<uint32_t>& inputTensor)
{
    size_t channelIdx = 3;
    if (dataFormat_ == VisionDataFormat::NHWC && inputTensor.size() == 0x4
        && inputTensor[channelIdx] == 0x1) {
        return true;
    }
    return false;
}

/*
* @description Check Input Tensors size
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::CheckInputTensors(const std::vector<Tensor>& tensors, const uint32_t count,
                                           AscendStream& Stream)
{
    // Check the input tensors size
    if (tensors.empty() || tensors[0].GetShape().empty()) {
        LogError << "InputTensors is empty. Please check inputTensors." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        Stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (tensors.size() > count) {
        LogError << "InputTensors size is invalid. Please check inputTensors."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        Stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

/*
* @description Check Output Tensors size
* @return APP_ERROR error code
*/
APP_ERROR MxOmModelDesc::CheckOutputTensors(const std::vector<Tensor>& tensors, const uint32_t count,
                                            AscendStream& Stream)
{
    // Check the input tensors size
    if (tensors.empty() || tensors[0].GetShape().empty()) {
        LogError << "OutputTensors is empty. Please check outputTensors." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        Stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (tensors.size() != count) {
        LogError << "OutputTensors size is invalid. Please check outputTensors."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        Stream.SetErrorCode(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxOmModelDesc::SetModelLoadOpt(aclmdlConfigHandle* handle, const ModelLoadOptV2 &opt)
{
    APP_ERROR ret = APP_ERR_OK;
    const char* c = opt.modelPath.c_str();
    if (opt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_FILE_WITH_MEM ||
        opt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_FILE) {
        if (opt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_FILE_WITH_MEM) {
            ret = aclmdlQuerySize(opt.modelPath.c_str(), &workSize_, &weightSize_);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to query model work or weight size." << GetErrorInfo(ret, "aclmdlQuerySize");
                return APP_ERR_ACL_FAILURE;
            }
            ret = SetModelMemOpt(handle, opt);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to set work or weight option." << GetErrorInfo(ret);
                return ret;
            }
        }
        ret = aclmdlSetConfigOpt(handle, ACL_MDL_PATH_PTR, (void*)&c, sizeof(c));
        if (ret != APP_ERR_OK) {
            LogError << "Failed to set model path." << GetErrorInfo(ret, "aclmdlSetConfigOpt");
            return APP_ERR_ACL_FAILURE;
        }
    } else if (opt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_MEM_WITH_MEM ||
               opt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_MEM) {
        if (opt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_MEM_WITH_MEM) {
            ret = aclmdlQuerySizeFromMem(opt.modelPtr, opt.modelSize, &workSize_, &weightSize_);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to query model work or weight size." << GetErrorInfo(ret, "aclmdlQuerySizeFromMem");
                return APP_ERR_ACL_FAILURE;
            }
            ret = SetModelMemOpt(handle, opt);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to set work or weight option." << GetErrorInfo(ret);
                return ret;
            }
        }
        ret = aclmdlSetConfigOpt(handle, ACL_MDL_MEM_ADDR_PTR, &(opt.modelPtr), sizeof(opt.modelPtr));
        if (ret != APP_ERR_OK) {
            LogError << "Failed to set model pointer." << GetErrorInfo(ret, "aclmdlSetConfigOpt");
            return APP_ERR_ACL_FAILURE;
        }
        ret = aclmdlSetConfigOpt(handle, ACL_MDL_MEM_SIZET, &(opt.modelSize), sizeof(opt.modelSize));
        if (ret != APP_ERR_OK) {
            LogError << "Failed to set model size." << GetErrorInfo(ret, "aclmdlSetConfigOpt");
            return APP_ERR_ACL_FAILURE;
        }
    } else {
        LogError << "Invalid ModelLoadType." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxOmModelDesc::SetModelMemOpt(aclmdlConfigHandle* handle, const ModelLoadOptV2 &opt)
{
    APP_ERROR ret = APP_ERR_OK;
    if (opt.modelWorkPtr != nullptr) {
        if (opt.workSize < workSize_) {
            LogError << "Model work size is not enough." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        ret = aclmdlSetConfigOpt(handle, ACL_MDL_WORKSPACE_ADDR_PTR,
                                 &(opt.modelWorkPtr), sizeof(opt.modelWorkPtr));
        if (ret != APP_ERR_OK) {
            LogError << "Failed to set workptr." << GetErrorInfo(ret, "aclmdlSetConfigOpt");
            return APP_ERR_ACL_FAILURE;
        }
        ret = aclmdlSetConfigOpt(handle, ACL_MDL_WORKSPACE_SIZET, &(opt.workSize), sizeof(opt.workSize));
        if (ret != APP_ERR_OK) {
            LogError << "Failed to set workptr size." << GetErrorInfo(ret, "aclmdlSetConfigOpt");
            return APP_ERR_ACL_FAILURE;
        }
    }
    if (opt.modelWeightPtr != nullptr) {
        if (opt.weightSize < weightSize_) {
            LogError << "Model weight size is not enough." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        ret = aclmdlSetConfigOpt(handle, ACL_MDL_WEIGHT_ADDR_PTR, &(opt.modelWeightPtr), sizeof(opt.modelWeightPtr));
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc weightptr." << GetErrorInfo(ret, "aclmdlSetConfigOpt");
            return APP_ERR_ACL_FAILURE;
        }
        ret = aclmdlSetConfigOpt(handle, ACL_MDL_WEIGHT_SIZET, &(opt.weightSize), sizeof(opt.weightSize));
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc weightptr size." << GetErrorInfo(ret, "aclmdlSetConfigOpt");
            return APP_ERR_ACL_FAILURE;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxOmModelDesc::LoadModel(ModelLoadOptV2 &mdlLoadOpt)
{
    auto configHandle = aclmdlCreateConfigHandle();
    if (configHandle == nullptr) {
        LogError << "Failed to create config handle." << GetErrorInfo(APP_ERR_ACL_FAILURE);
        return APP_ERR_ACL_FAILURE;
    }
    size_t loadType = mdlLoadOpt.loadType;
    APP_ERROR ret = aclmdlSetConfigOpt(configHandle, ACL_MDL_LOAD_TYPE_SIZET, (void*)(&loadType), sizeof(loadType));
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set load type to option." << GetErrorInfo(ret, "aclmdlSetConfigOpt");
        ret = aclmdlDestroyConfigHandle(configHandle);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to destroy config handle." << GetErrorInfo(ret, "aclmdlDestroyConfigHandle");
        }
        return APP_ERR_ACL_FAILURE;
    }
    ret = SetModelLoadOpt(configHandle, mdlLoadOpt);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set config handle." << GetErrorInfo(ret);
        ret = aclmdlDestroyConfigHandle(configHandle);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to destroy config handle." << GetErrorInfo(ret, "aclmdlDestroyConfigHandle");
        }
        return APP_ERR_COMM_FAILURE;
    }
    ret = aclmdlLoadWithConfig(configHandle, &modelId_);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to load offline model data." << GetErrorInfo(ret, "aclmdlLoadWithConfig");
        ret = aclmdlDestroyConfigHandle(configHandle);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to destroy config handle." << GetErrorInfo(ret, "aclmdlDestroyConfigHandle");
        }
        return APP_ERR_ACL_FAILURE;
    }
    ret = aclmdlDestroyConfigHandle(configHandle);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to destroy config handle." << GetErrorInfo(ret, "aclmdlDestroyConfigHandle");
    }
    return APP_ERR_OK;
}
} // MxBase namespace end

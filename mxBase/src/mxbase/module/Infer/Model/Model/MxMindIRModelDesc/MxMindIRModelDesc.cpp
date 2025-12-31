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
 * Description: Mindir model infer function.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "MxMindIRModelDesc.h"
#include <dlfcn.h>
#include <thread>
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"

namespace MxBase {
namespace {
const std::string MSLITE_LIB_PATH = "/lib/libmindsporelitewrapper.so";

std::map<TensorDType, MiddleDataType> TensorDTypeToMiddle = {
    {TensorDType::UNDEFINED, MiddleDataType::UNDEFINED},
    {TensorDType::FLOAT32,   MiddleDataType::FLOAT32},
    {TensorDType::FLOAT16,   MiddleDataType::FLOAT16},
    {TensorDType::INT8,      MiddleDataType::INT8},
    {TensorDType::INT16,     MiddleDataType::INT16},
    {TensorDType::INT32,     MiddleDataType::INT32},
    {TensorDType::INT64,     MiddleDataType::INT64},
    {TensorDType::UINT8,     MiddleDataType::UINT8},
    {TensorDType::UINT16,    MiddleDataType::UINT16},
    {TensorDType::UINT32,    MiddleDataType::UINT32},
    {TensorDType::UINT64,    MiddleDataType::UINT64},
    {TensorDType::DOUBLE64,  MiddleDataType::DOUBLE64},
    {TensorDType::BOOL,      MiddleDataType::BOOL}
};

std::map<MiddleDataType, TensorDType> MiddleDTypeToTensor = {
    {MiddleDataType::UNDEFINED, TensorDType::UNDEFINED},
    {MiddleDataType::FLOAT32,   TensorDType::FLOAT32},
    {MiddleDataType::FLOAT16,   TensorDType::FLOAT16},
    {MiddleDataType::INT8,      TensorDType::INT8},
    {MiddleDataType::INT16,     TensorDType::INT16},
    {MiddleDataType::INT32,     TensorDType::INT32},
    {MiddleDataType::INT64,     TensorDType::INT64},
    {MiddleDataType::UINT8,     TensorDType::UINT8},
    {MiddleDataType::UINT16,    TensorDType::UINT16},
    {MiddleDataType::UINT32,    TensorDType::UINT32},
    {MiddleDataType::UINT64,    TensorDType::UINT64},
    {MiddleDataType::DOUBLE64,   TensorDType::DOUBLE64},
    {MiddleDataType::BOOL,      TensorDType::BOOL}
};

std::map<VisionDataFormat, MiddleDataFormat> VisionFormatToMiddle = {
    {VisionDataFormat::NCHW, MiddleDataFormat::NCHW},
    {VisionDataFormat::NHWC, MiddleDataFormat::NHWC}
};

std::map<MiddleDataFormat, VisionDataFormat> MiddleFormatToVision = {
    {MiddleDataFormat::NCHW, VisionDataFormat::NCHW},
    {MiddleDataFormat::NHWC, VisionDataFormat::NHWC}
};
}

MxMindIRModelDesc::~MxMindIRModelDesc()
{
    if (mindsporeLiteWapper_ != nullptr) {
        destroyInstance_(mindsporeLiteWapper_);
        mindsporeLiteWapper_ = nullptr;
    }
    if (mindsporeLiteWrapperLib_ != nullptr) {
        dlclose(mindsporeLiteWrapperLib_);
        mindsporeLiteWrapperLib_ = nullptr;
    }
}

APP_ERROR MxMindIRModelDesc::Init(ModelLoadOptV2 &mdlLoadOpt, const int32_t deviceId)
{
    // Load the mindsporelitewrapper library
    APP_ERROR ret = InitMsliteWrapper();
    if (ret != APP_ERR_OK) {
        LogError << "Mindir model init failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }

    deviceId_ = deviceId;
    ret = SetDevice();
    if (ret != APP_ERR_OK) {
        LogError << "Set device failed." << GetErrorInfo(ret);
        return ret;
    }
    if (mdlLoadOpt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_FILE ||
        mdlLoadOpt.loadType == ModelLoadOptV2::LOAD_MODEL_FROM_FILE_WITH_MEM) {
        ret = InitFromFile(mdlLoadOpt.modelPath.c_str());
    } else {
        ret = InitFromMemory(mdlLoadOpt);
    }
    if (ret != APP_ERR_OK) {
        LogError << "Mindir model init failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }

    // the process of mindir model build will change the log config of sdk, now reset it.
    MxBase::Log::Deinit();
    ret = MxBase::Log::Init();
    if (ret != APP_ERR_OK) {
        LogError << "Initialize log failed." << GetErrorInfo(ret);
        return ret;
    }

    ret = GetModelDesc();
    if (ret != APP_ERR_OK) {
        LogError << "Get model info failed." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR MxMindIRModelDesc::GetMsliteWrapperLib(std::string& msliteLibFile)
{
    auto sdkHomeEnv = std::getenv("MX_SDK_HOME");
    if (!sdkHomeEnv) {
        LogError << "Please set env variable MX_SDK_HOME first." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    std::string libFilePath = sdkHomeEnv + MSLITE_LIB_PATH;
    if (!MxBase::FileUtils::RegularFilePath(libFilePath, msliteLibFile)) {
        LogError << "Failed to regular file path of libmindsporelitewrapper.so."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    if (!MxBase::FileUtils::IsFileValid(msliteLibFile, true)) {
        LogError << "Can not get the library of libmindsporelitewrapper.so, or it's invalid."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR MxMindIRModelDesc::InitMsliteWrapper()
{
    std::string msliteWrapperLib;
    APP_ERROR ret = GetMsliteWrapperLib(msliteWrapperLib);
    if (ret != APP_ERR_OK) {
        LogError << "Get the library of libmindsporelitewrapper.so failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (mindsporeLiteWrapperLib_ == nullptr) {
        mindsporeLiteWrapperLib_ = dlopen(msliteWrapperLib.c_str(), RTLD_LAZY);
    }
    if (!mindsporeLiteWrapperLib_) {
        LogError << "Cannot load library: " << dlerror() << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    // Reset errors
    dlerror();

    // Load the symbols
    createInstance_ = reinterpret_cast<CreateInstance>(dlsym(mindsporeLiteWrapperLib_, "Create"));
    const char *dlSymError = dlerror();
    if (dlSymError) {
        LogError << "Cannot load symbol Create: " << dlSymError << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }

    destroyInstance_ = reinterpret_cast<DestroyInstance>(dlsym(mindsporeLiteWrapperLib_, "Destroy"));
    dlSymError = dlerror();
    if (dlSymError) {
        LogError << "Cannot load symbol Destroy: " << dlSymError << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }

    mindsporeLiteWapper_ = createInstance_();
    if (mindsporeLiteWapper_ == nullptr) {
        LogError << "Create mindsporeLiteWapper instance failed." <<  GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR MxMindIRModelDesc::SetDevice()
{
    // Create and init context
    APP_ERROR ret = mindsporeLiteWapper_->SetDevice(deviceId_);
    if (ret != APP_ERR_OK) {
        LogError << "Mindir model set device failed." <<  GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR MxMindIRModelDesc::InitFromFile(const std::string& modelPath)
{
    APP_ERROR ret = mindsporeLiteWapper_->InitFromFile(modelPath.c_str());
    if (ret != APP_ERR_OK) {
        LogError << "Mindir model init from file failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR MxMindIRModelDesc::InitFromMemory(const ModelLoadOptV2 &mdlLoadOpt)
{
    APP_ERROR ret = mindsporeLiteWapper_->InitFromMemory(mdlLoadOpt);
    if (ret != APP_ERR_OK) {
        LogError << "Mindir model init from memory failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR MxMindIRModelDesc::CheckInputTensorData(std::vector<Tensor>& tensorVec)
{
    for (size_t i = 0; i < tensorVec.size(); i++) {
        if (tensorVec[i].GetDataType() != GetInputTensorDataType(i)) {
            LogError << "The input tensor data type does not match the data type required by the model, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxMindIRModelDesc::Infer(std::vector<Tensor>& inputTensors, std::vector<Tensor>& outputTensors,
                                   AscendStream& stream)
{
    // Check the input tensors size
    APP_ERROR ret = CheckTensors(inputTensors, GetInputTensorNum());
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = CheckInputTensorData(inputTensors);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = CheckTensors(outputTensors, GetOutputTensorNum());
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = ModelInferenceAsync(inputTensors, outputTensors, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Mindir model infer failed." << GetErrorInfo(ret);
        return ret;
    }
    return ret;
}

std::vector<Tensor> MxMindIRModelDesc::Infer(std::vector<Tensor>& inputTensors)
{
    APP_ERROR ret = CheckTensors(inputTensors, GetInputTensorNum());
    if (ret != APP_ERR_OK) {
        return {};
    }
    ret = CheckInputTensorData(inputTensors);
    if (ret != APP_ERR_OK) {
        return {};
    }
    std::vector<Tensor> outputTensors = {};
    ret = ModelInference(inputTensors, outputTensors);
    if (ret != APP_ERR_OK) {
        LogError << "MindIR Model Infer failed." << GetErrorInfo(ret);
        return {};
    }
    return outputTensors;
}

APP_ERROR MxMindIRModelDesc::ModelInference(std::vector<Tensor>& inputTensors, std::vector<Tensor>& outputTensors)
{
    std::vector<MiddleTensor> inputs = {};
    std::vector<std::vector<int64_t>> inputShape = {};
    APP_ERROR ret = MallocAndSetInputTensor(inputTensors, inputs, inputShape);
    if (ret != APP_ERR_OK) {
        LogError << "Set input Tensor failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    std::vector<MiddleTensor> outputs = {};
    {
        std::lock_guard<std::mutex> guard(mindIRModelMutex_);
        ret = mindsporeLiteWapper_->Infer(inputs, outputs);
    }
    if (ret != APP_ERR_OK) {
        LogError << "Mindir model infer failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }

    // create output tensor
    ret = GetModelOutput(outputs, outputTensors);
    if (ret != APP_ERR_OK) {
        LogError << "Mindir model get model output failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }

    DeviceContext context = {};
    context.devId = static_cast<int>(deviceId_);
    ret = MxBase::DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set device, please check the state of device." << GetErrorInfo(ret);
        return ret;
    }

    return APP_ERR_OK;
}

APP_ERROR MxMindIRModelDesc::ModelInferenceAsync(std::vector<Tensor>& inputTensors, std::vector<Tensor>& outputTensors,
                                                 AscendStream& stream)
{
    std::vector<MiddleTensor> inputs = {};
    std::vector<std::vector<int64_t>> inputShape = {};
    APP_ERROR ret = MallocAndSetInputTensor(inputTensors, inputs, inputShape);
    if (ret != APP_ERR_OK) {
        LogError << "Set input Tensor failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    if (stream.isDefault_) {
        LogInfo << "Start execute model infer sync.";
        ret = ModelInfer(inputs, outputTensors);
        if (ret != APP_ERR_OK) {
            LogError << "Execute Model Infer failed." << GetErrorInfo(ret);
        }
        return ret;
    }
    LogInfo << "Start execute model infer async.";
    ModelInferCallbackParam* modelInferCallbackParam = new(std::nothrow) ModelInferCallbackParam{inputs,
                                                                                                 outputTensors,
                                                                                                 stream, this};
    if (modelInferCallbackParam == nullptr) {
        stream.SetErrorCode(APP_ERR_COMM_ALLOC_MEM);
        LogError << "Malloc memory of modelInferCallbackParam failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    ret = aclrtLaunchCallback(ModelInferCallbackFunc, static_cast<void*>(modelInferCallbackParam),
                              ACL_CALLBACK_BLOCK, stream.stream);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        delete modelInferCallbackParam;
        LogError << "Call aclrtLaunchCallback of ModelInferCallback failed."
                 << GetErrorInfo(ret, "aclrtLaunchCallback");
        return APP_ERR_COMM_FAILURE;
    }

    return APP_ERR_OK;
}

APP_ERROR MxMindIRModelDesc::ModelInfer(std::vector<MiddleTensor>& inputs, std::vector<Tensor>& outputTensors,
                                        AscendStream& stream)
{
    APP_ERROR ret = APP_ERR_OK;
    std::vector<MiddleTensor> outputs = {};
    {
        std::lock_guard<std::mutex> guard(mindIRModelMutex_);
        ret = mindsporeLiteWapper_->Infer(inputs, outputs);
    }
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Mindir model infer failed." << GetErrorInfo(ret);
        return ret;
    }

    ret = GetModelOutput(outputs, outputTensors);
    if (ret != APP_ERR_OK) {
        stream.SetErrorCode(ret);
        LogError << "Get model infer output failed." << GetErrorInfo(ret);
        return ret;
    }

    DeviceContext context = {};
    context.devId = static_cast<int>(deviceId_);
    ret = MxBase::DeviceManager::GetInstance()->SetDevice(context);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to set device, please check the state of device." << GetErrorInfo(ret);
        return ret;
    }

    return APP_ERR_OK;
}

void MxMindIRModelDesc::ModelInferCallbackFunc(void *arg)
{
    ModelInferCallbackParam* input = static_cast<ModelInferCallbackParam*>(arg);
    APP_ERROR ret = input->mindIRModelDesc->ModelInfer(input->inputs, input->outputs, input->stream);
    if (ret != APP_ERR_OK) {
        input->stream.SetErrorCode(ret);
        LogError << "Execute mindir model Infer async failed." << GetErrorInfo(ret);
    }
    delete input;
}

uint32_t MxMindIRModelDesc::GetInputTensorNum() const
{
    return inputTensors_.size();
}

uint32_t MxMindIRModelDesc::GetOutputTensorNum() const
{
    return outputTensors_.size();
}

std::vector<int64_t> MxMindIRModelDesc::GetInputTensorShape(uint32_t index) const
{
    if (index >= GetInputTensorNum()) {
        LogWarn << "Index: "<< index << "is out of range, " << "input tensor size: " << GetInputTensorNum();
        return {};
    }
    return inputTensors_[index].dataShape;
}

std::vector<uint32_t> MxMindIRModelDesc::GetOutputTensorShape(uint32_t index) const
{
    if (index >= GetOutputTensorNum()) {
        LogWarn << "Index: "<< index << "is out of range, " << "output tensor size: " << GetOutputTensorNum();
        return {};
    }
    std::vector<int64_t> outputShape = outputTensors_[index].dataShape;
    return std::vector<uint32_t>(outputShape.begin(), outputShape.end());
}

TensorDType MxMindIRModelDesc::GetInputTensorDataType(uint32_t index) const
{
    if (index >= GetInputTensorNum()) {
        LogWarn << "Index: "<< index << "is out of range, " << "input tensor size: " << GetInputTensorNum();
        return TensorDType::UNDEFINED;
    }
    return MiddleDTypeToTensor[inputTensors_[index].dataType];
}

TensorDType MxMindIRModelDesc::GetOutputTensorDataType(uint32_t index) const
{
    if (index >= GetOutputTensorNum()) {
        LogWarn << "Index: "<< index << "is out of range, " << "output tensor size: " << GetOutputTensorNum();
        return TensorDType::UNDEFINED;
    }
    return MiddleDTypeToTensor[outputTensors_[index].dataType];
}

VisionDataFormat MxMindIRModelDesc::GetInputFormat() const
{
    if (inputTensors_.empty()) {
        LogError << "Can not get format of model input, now return default value."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return VisionDataFormat::NHWC;
    }
    return MiddleFormatToVision[inputTensors_[0].dataFormat];
}

std::vector<std::vector<uint64_t>> MxMindIRModelDesc::GetDynamicGearInfo() const
{
    LogWarn << "Mindir model not support dynamic gear currently.";
    return {};
}

VisionDynamicType MxMindIRModelDesc::GetDynamicType() const
{
    LogWarn << "Mindir model not support dynamic type currently.";
    return VisionDynamicType::STATIC_BATCH;
}

APP_ERROR MxMindIRModelDesc::GetModelDesc()
{
    APP_ERROR ret = mindsporeLiteWapper_->GetModelInputs(inputTensors_);
    if (ret != APP_ERR_OK) {
        LogError << "Get model inputs info failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    ret = mindsporeLiteWapper_->GetModelOutputs(outputTensors_);
    if (ret != APP_ERR_OK) {
        LogError << "Get model outputs info failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR MxMindIRModelDesc::MallocAndSetInputTensor(std::vector<Tensor>& inputTensors,
                                                     std::vector<MiddleTensor>& inputs,
                                                     std::vector<std::vector<int64_t>>& inputShape)
{
    for (size_t i = 0; i < inputTensors.size(); i++) {
        inputTensors[i].ToHost();
        std::vector<uint32_t> shape = inputTensors[i].GetShape();
        inputShape.push_back(std::vector<int64_t>(shape.begin(), shape.end()));
        MiddleTensor mdTensor = TransTensorToMDTensor(inputTensors[i]);
        if (mdTensor.dataPtr == nullptr) {
            LogError << "Translate Tensor to MDTensor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        inputs.push_back(mdTensor);
    }
    return APP_ERR_OK;
}

MiddleTensor MxMindIRModelDesc::TransTensorToMDTensor(Tensor& tensor)
{
    if (tensor.GetData() == nullptr) {
        LogError << "Translate Tensor to MDTensor failed, please check Tensor data."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return {};
    }

    std::vector<uint32_t> srcShape = tensor.GetShape();
    std::vector<int64_t> dstShape = std::vector<int64_t>(srcShape.begin(), srcShape.end());
    MiddleTensor mdTensor;
    mdTensor.dataPtr = tensor.GetData();
    mdTensor.dataSize = tensor.GetByteSize();
    mdTensor.dataType = TensorDTypeToMiddle[tensor.GetDataType()];
    mdTensor.dataShape = dstShape;
    return mdTensor;
}

Tensor MxMindIRModelDesc::TransMDTensorToTensor(MiddleTensor& mdTensor)
{
    if (mdTensor.dataPtr == nullptr) {
        LogError << "MDTensor's data is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return {};
    }
    std::vector<int64_t> srcShape = mdTensor.dataShape;
    std::vector<uint32_t> dstShape = std::vector<uint32_t>(srcShape.begin(), srcShape.end());
    MiddleDataType srcDType = mdTensor.dataType;
    TensorDType dsrDType = MiddleDTypeToTensor[srcDType];
    Tensor tensor(mdTensor.dataPtr, dstShape, dsrDType, deviceId_, false, true);
    return tensor;
}

APP_ERROR MxMindIRModelDesc::GetModelOutput(std::vector<MiddleTensor>& output,
                                            std::vector<Tensor>& outputTensors)
{
    if (!outputTensors.empty()) {
        APP_ERROR ret = MemoryCopyMDTensorToTensor(output, outputTensors);
        if (ret != APP_ERR_OK) {
            LogError << "Copy mdTensor data to tensor failed." << GetErrorInfo(ret);
            return APP_ERR_COMM_FAILURE;
        }
        return APP_ERR_OK;
    }
    for (size_t i = 0; i < output.size(); i++) {
        Tensor tensor = TransMDTensorToTensor(output[i]);
        if (tensor.GetData() == nullptr) {
            LogError << "Trans middle tensor to tensor failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        outputTensors.push_back(std::move(tensor));
    }
    return APP_ERR_OK;
}

APP_ERROR MxMindIRModelDesc::MemoryCopyMDTensorToTensor(std::vector<MiddleTensor>& mdTensor,
                                                        std::vector<Tensor>& tensor)
{
    if (mdTensor.empty() || tensor.empty()) {
        LogError << "MDTensor or tensor is empty." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    if (mdTensor.size() != tensor.size()) {
        LogError << "MDTensor size is not match with tensor." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    for (size_t i = 0; i < mdTensor.size(); i++) {
        if (mdTensor[i].dataPtr == nullptr || tensor[i].GetData() == nullptr) {
            LogError << "Translate mdTensor to tensor failed, please check mdTensor or tensor data."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        if (mdTensor[i].dataSize != tensor[i].GetByteSize()) {
            LogError << "MDTensor data size not match with tensor, mdTensor translate failed."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        void* dstDataptr = tensor[i].GetData();
        size_t dataSize = tensor[i].GetByteSize();
        aclrtMemcpyKind copyType = tensor[i].GetDeviceId() == -1 ? ACL_MEMCPY_DEVICE_TO_HOST :
                                   ACL_MEMCPY_DEVICE_TO_DEVICE;
        void* srcDataptr = mdTensor[i].dataPtr;
        APP_ERROR ret = aclrtMemcpy(dstDataptr, dataSize, srcDataptr, dataSize, copyType);
        if (ret != APP_ERR_OK) {
            LogError << "Copy mdTensor data to tensor failed." << GetErrorInfo(ret, "aclrtMemcpy");
            return APP_ERR_ACL_BAD_COPY;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxMindIRModelDesc::CheckTensors(std::vector<Tensor>& tensors, const uint32_t count)
{
    if (tensors.empty()) {
        LogError << "Tensors is empty, please check Tensors." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (tensors.size() != count) {
        LogError << "Tensors size is invalid, please check Tensors." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < count; i++) {
        if (tensors[i].GetShape().empty()) {
            LogError << "Tensor[" << i << "] is invalid, please check Tensors."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

} // MxBase namespace end
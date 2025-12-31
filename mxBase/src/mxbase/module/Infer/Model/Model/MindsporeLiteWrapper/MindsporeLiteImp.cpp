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
 * Description: Mindspore lite wrapper function.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include <map>
#include <memory>
#include "api/model.h"
#include "api/context.h"
#include "api/status.h"
#include "api/types.h"
#include "acl/acl.h"
#include "MindsporeLiteWrapper.h"
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"

namespace MxBase {
namespace {
std::map<MiddleDataType, mindspore::DataType> MiddleDataTypeToMSTensor = {
    {MiddleDataType::UNDEFINED, mindspore::DataType::kTypeUnknown},
    {MiddleDataType::FLOAT32,   mindspore::DataType::kNumberTypeFloat32},
    {MiddleDataType::FLOAT16,   mindspore::DataType::kNumberTypeFloat16},
    {MiddleDataType::INT8,      mindspore::DataType::kNumberTypeInt8},
    {MiddleDataType::INT16,     mindspore::DataType::kNumberTypeInt16},
    {MiddleDataType::INT32,     mindspore::DataType::kNumberTypeInt32},
    {MiddleDataType::INT64,     mindspore::DataType::kNumberTypeInt64},
    {MiddleDataType::UINT8,     mindspore::DataType::kNumberTypeUInt8},
    {MiddleDataType::UINT16,    mindspore::DataType::kNumberTypeUInt16},
    {MiddleDataType::UINT32,    mindspore::DataType::kNumberTypeUInt32},
    {MiddleDataType::UINT64,    mindspore::DataType::kNumberTypeUInt64},
    {MiddleDataType::DOUBLE64,  mindspore::DataType::kNumberTypeFloat64},
    {MiddleDataType::BOOL,      mindspore::DataType::kNumberTypeBool}
};

std::map<mindspore::DataType, MiddleDataType> MSTensorDTypeToMiddle = {
    {mindspore::DataType::kTypeUnknown,       MiddleDataType::UNDEFINED},
    {mindspore::DataType::kNumberTypeFloat32, MiddleDataType::FLOAT32},
    {mindspore::DataType::kNumberTypeFloat16, MiddleDataType::FLOAT16},
    {mindspore::DataType::kNumberTypeInt8,    MiddleDataType::INT8},
    {mindspore::DataType::kNumberTypeInt16,   MiddleDataType::INT16},
    {mindspore::DataType::kNumberTypeInt32,   MiddleDataType::INT32},
    {mindspore::DataType::kNumberTypeInt64,   MiddleDataType::INT64},
    {mindspore::DataType::kNumberTypeUInt8,   MiddleDataType::UINT8},
    {mindspore::DataType::kNumberTypeUInt16,  MiddleDataType::UINT16},
    {mindspore::DataType::kNumberTypeUInt32,  MiddleDataType::UINT32},
    {mindspore::DataType::kNumberTypeUInt64,  MiddleDataType::UINT64},
    {mindspore::DataType::kNumberTypeFloat64, MiddleDataType::DOUBLE64},
    {mindspore::DataType::kNumberTypeBool,    MiddleDataType::BOOL}
};

std::map<MiddleDataFormat, mindspore::Format> MiddleDataFormatToMindspore = {
    {MiddleDataFormat::NCHW, mindspore::Format::NCHW},
    {MiddleDataFormat::NHWC, mindspore::Format::NHWC}
};

std::map<mindspore::Format, MiddleDataFormat> MindsporeFormatToMiddle = {
    {mindspore::Format::NCHW, MiddleDataFormat::NCHW},
    {mindspore::Format::NHWC, MiddleDataFormat::NHWC}
};
}

class MindsporeLiteImp : public MindsporeLiteWrapper {
public:
    APP_ERROR InitFromFile(const char *modelPath) final;

    APP_ERROR InitFromMemory(const ModelLoadOptV2 &mdlLoadOpt) final;

    APP_ERROR SetDevice(const size_t deviceId) final;

    APP_ERROR Infer(std::vector<MiddleTensor> &inputs, std::vector<MiddleTensor> &outputs) final;

    APP_ERROR GetModelInputs(std::vector<MiddleTensor> &inputTensors) final;

    APP_ERROR GetModelOutputs(std::vector<MiddleTensor> &outputTensors) final;

private:
    void TransMSTensorToMiddle(std::vector<mindspore::MSTensor> &msTensors,
                               std::vector<MiddleTensor> &mdTensors) const;
    std::shared_ptr<mindspore::Model> mindsporeModel_;
    std::shared_ptr<mindspore::Context> context_;
    std::vector<mindspore::MSTensor> inputMsTensors_ = {};
    std::vector<mindspore::MSTensor> outputMsTensors_ = {};
};

APP_ERROR MindsporeLiteImp::InitFromFile(const char *modelPath)
{
    mindsporeModel_ = std::make_shared<mindspore::Model>();
    mindspore::Status ret = mindsporeModel_->Build(modelPath, mindspore::kMindIR, context_);
    if (ret != mindspore::kSuccess) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR MindsporeLiteImp::InitFromMemory(const ModelLoadOptV2 &mdlLoadOpt)
{
    mindsporeModel_ = std::make_shared<mindspore::Model>();
    mindspore::Status ret = mindsporeModel_->Build(mdlLoadOpt.modelPtr, mdlLoadOpt.modelSize,
                                                   mindspore::kMindIR, context_);
    if (ret != mindspore::kSuccess) {
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR MindsporeLiteImp::SetDevice(const size_t deviceId)
{
    context_ = std::make_shared<mindspore::Context>();
    // Add npu device info
    std::vector<std::shared_ptr<mindspore::DeviceInfoContext>> &device_list = context_->MutableDeviceInfo();
    std::shared_ptr<mindspore::AscendDeviceInfo> ascend_device_info = std::make_shared<mindspore::AscendDeviceInfo>();
    ascend_device_info->SetDeviceID(deviceId);
    device_list.push_back(ascend_device_info);
    return APP_ERR_OK;
}

APP_ERROR MindsporeLiteImp::Infer(std::vector<MiddleTensor> &inputs, std::vector<MiddleTensor> &outputs)
{
    std::vector<std::vector<int64_t>> inputShapes = {};
    // translate MiddleTensor to MSTensor
    std::vector<mindspore::MSTensor>().swap(inputMsTensors_);
    inputMsTensors_ = mindsporeModel_->GetInputs();
    for (size_t i = 0; i < inputs.size(); i++) {
        std::vector<int64_t> dstShape = inputs[i].dataShape;
        inputShapes.push_back(dstShape);
        inputMsTensors_[i].SetData(inputs[i].dataPtr, false);
    }
    // This operation is applicable to the dynamic shape scenario. In the dynamic shape scenario,
    // the resize is performed to the required gears.
    mindspore::Status ret = mindsporeModel_->Resize(inputMsTensors_, inputShapes);
    if (ret != mindspore::kSuccess) {
        return APP_ERR_COMM_FAILURE;
    }

    auto msModelOutputs = mindsporeModel_->GetOutputs();
    ret = mindsporeModel_->Predict(inputMsTensors_, &msModelOutputs);
    TransMSTensorToMiddle(msModelOutputs, outputs);
    return APP_ERR_OK;
}

APP_ERROR MindsporeLiteImp::GetModelInputs(std::vector<MiddleTensor> &inputTensors)
{
    std::vector<mindspore::MSTensor>().swap(inputMsTensors_);
    inputMsTensors_ = mindsporeModel_->GetInputs();
    if (inputMsTensors_.empty()) {
        return APP_ERR_COMM_FAILURE;
    }
    std::vector<MiddleTensor>().swap(inputTensors);
    TransMSTensorToMiddle(inputMsTensors_, inputTensors);
    return APP_ERR_OK;
}

APP_ERROR MindsporeLiteImp::GetModelOutputs(std::vector<MiddleTensor> &outputTensors)
{
    std::vector<mindspore::MSTensor>().swap(outputMsTensors_);
    outputMsTensors_ = mindsporeModel_->GetOutputs();
    if (outputMsTensors_.empty()) {
        return APP_ERR_COMM_FAILURE;
    }
    std::vector<MiddleTensor>().swap(outputTensors);
    TransMSTensorToMiddle(outputMsTensors_, outputTensors);
    return APP_ERR_OK;
}

void MindsporeLiteImp::TransMSTensorToMiddle(std::vector<mindspore::MSTensor> &msTensors,
                                             std::vector<MiddleTensor> &mdTensors) const
{
    mdTensors.resize(msTensors.size());
    for (size_t i = 0; i < msTensors.size(); i++) {
        mdTensors[i].dataPtr = msTensors[i].MutableData();
        mdTensors[i].dataSize = msTensors[i].DataSize();
        mdTensors[i].dataType = MSTensorDTypeToMiddle[msTensors[i].DataType()];
        mdTensors[i].dataFormat = MindsporeFormatToMiddle[msTensors[i].format()];
        mdTensors[i].dataShape = msTensors[i].Shape();
    }
}

extern "C" MindsporeLiteWrapper *Create()
{
    return new(std::nothrow) MindsporeLiteImp;
}

extern "C" void Destroy(MindsporeLiteWrapper *ptr)
{
    delete ptr;
}
}
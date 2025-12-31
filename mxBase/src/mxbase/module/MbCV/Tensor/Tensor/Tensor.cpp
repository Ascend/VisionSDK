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
 * Description: Constructing Tensor Class and Providing Its Attribute Interfaces.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#include <algorithm>
#include "dvpp/securec.h"
#include "TensorDptr.hpp"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/DataTypeUtils.h"
#include "MxBase/E2eInfer/DataType.h"
#include "MxBase/MemoryHelper/CustomizedMemoryHelper.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"

namespace {
    constexpr size_t MAX_TRANS_TENSOR_DIM = 4;
    constexpr size_t BGR_CHANNEL = 3;
    constexpr size_t BGRA_CHANNEL = 4;
    constexpr size_t MAX_CLONE_SIZE = 6710886;
    constexpr uint32_t SHAPE_DIMENSION_ZERO = 0;
    constexpr uint32_t SHAPE_DIMENSION_ONE = 1;
    constexpr uint32_t SHAPE_DIMENSION_TWO = 2;
}
namespace MxBase {
const int MIN_ROI_SET_DIM = 2;
const int MAX_ROI_SET_DIM = 4;
union TensorValue {
    int32_t int32Value;
    float floatValue;
    uint8_t uint8Value;
};
struct SetTensorValuePara {
    TensorValue value;
    size_t tensorSize;
    void *tensorPtr;
    size_t tensorByteSize;
    MemoryData::MemoryType tensorType;
    int32_t tensorDeviceId;
    TensorDType dataType;
    bool isFloat16;
};

Tensor::Tensor()
{
    dPtr_ = MemoryHelper::MakeShared<MxBase::TensorDptr>();
    if (dPtr_ == nullptr) {
        LogError << "Create Tensor dPtr failed, failed to allocated memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

Tensor::~Tensor() {}

Tensor::Tensor(const Tensor &other)
{
    dPtr_ = other.dPtr_;
}

Tensor::Tensor(const Tensor &other, const Rect &rect)
{
    if (!DeviceManager::IsAscend310P()) {
        LogError << "Tensor constructor with Rect is only supported on 310P now."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    dPtr_ = MemoryHelper::MakeShared<MxBase::TensorDptr>();
    if (dPtr_ == nullptr) {
        LogError << "Create Tensor dPtr failed, failed to allocated memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    if (other.IsEmpty()) {
        LogError << "The other tensor cannot be empty." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
    dPtr_->tensorBase_ = other.dPtr_->tensorBase_;
    auto ret = SetReferRect(rect);
    if (ret != APP_ERR_OK) {
        LogError << "Construct Tensor with setting refer rect failed." << GetErrorInfo(ret);
        throw std::runtime_error("Construct Tensor with setting refer rect failed.");
    }
}

bool Tensor::operator == (const Tensor &other)
{
    return dPtr_ == other.dPtr_;
}

Tensor &Tensor::operator = (const Tensor &other)
{
    if (this == &other) {
        return *this;
    }
    dPtr_ = other.dPtr_;
    return *this;
}

APP_ERROR Tensor::TensorMalloc(Tensor &tensor)
{
    LogWarn << "tensor.Malloc() is recommended to avoid ambiguity.";
    if (tensor.dPtr_ == nullptr || tensor.dPtr_->tensorBase_ == nullptr) {
        LogError << "Invalid tensor, failed to malloc tensor." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    APP_ERROR ret = TensorBase::TensorBaseMalloc(*(tensor.dPtr_->tensorBase_));
    if (ret != APP_ERR_OK) {
        LogError << "Failed to malloc Tensor" << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR Tensor::Malloc()
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "Invalid tensor, failed to malloc tensor." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    APP_ERROR ret = TensorBase::TensorBaseMalloc(*(dPtr_->tensorBase_));
    if (ret != APP_ERR_OK) {
        LogError << "Failed to malloc Tensor" << GetErrorInfo(ret);
    }
    return ret;
}

Tensor::Tensor(const std::vector<uint32_t> &shape, const TensorDType &dataType, const int32_t &deviceId)
{
    if (deviceId < 0) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::TensorDptr>(shape, static_cast<MxBase::TensorDataType>(dataType),
            MxBase::MemoryData::MemoryType::MEMORY_HOST_NEW, deviceId);
    } else {
        dPtr_ = MemoryHelper::MakeShared<MxBase::TensorDptr>(shape, static_cast<MxBase::TensorDataType>(dataType),
            MxBase::MemoryData::MemoryType::MEMORY_DEVICE, deviceId);
    }
    if (dPtr_ == nullptr) {
        LogError << "Create Tensor dPtr failed, failed to allocated memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

Tensor::Tensor(void* usrData, const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType,
    const int32_t &deviceId)
{
    if (deviceId < 0) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::TensorDptr>(usrData, shape,
            static_cast<MxBase::TensorDataType>(dataType), MxBase::MemoryData::MemoryType::MEMORY_HOST_NEW, deviceId);
    } else {
        dPtr_ = MemoryHelper::MakeShared<MxBase::TensorDptr>(usrData, shape,
            static_cast<MxBase::TensorDataType>(dataType), MxBase::MemoryData::MemoryType::MEMORY_DEVICE, deviceId);
    }
    if (dPtr_ == nullptr) {
        LogError << "Create Tensor dPtr failed, failed to allocated memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

Tensor::Tensor(const std::vector<uint32_t> &shape, const TensorDType &dataType, const int32_t &deviceId,
               bool isDvpp)
{
    if (deviceId < 0) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::TensorDptr>(shape, static_cast<MxBase::TensorDataType>(dataType),
                                                             MxBase::MemoryData::MemoryType::MEMORY_HOST, deviceId);
    } else {
        if (isDvpp) {
            dPtr_ = MemoryHelper::MakeShared<MxBase::TensorDptr>(shape, static_cast<MxBase::TensorDataType>(dataType),
                                                                 MxBase::MemoryData::MemoryType::MEMORY_DVPP, deviceId);
        } else {
            dPtr_ = MemoryHelper::MakeShared<MxBase::TensorDptr>(shape, static_cast<MxBase::TensorDataType>(dataType),
                                                                 MxBase::MemoryData::MemoryType::MEMORY_DEVICE,
                                                                 deviceId);
        }
    }
    if (dPtr_ == nullptr) {
        LogError << "Create Tensor dPtr failed, failed to allocated memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

Tensor::Tensor(void* usrData, const std::vector<uint32_t> &shape, const MxBase::TensorDType &dataType,
               const int32_t &deviceId, const bool isDvpp, const bool isBorrowed)
{
    if (deviceId < 0) {
        dPtr_ = MemoryHelper::MakeShared<MxBase::TensorDptr>(usrData, shape,
                                                             static_cast<MxBase::TensorDataType>(dataType),
                                                             MxBase::MemoryData::MemoryType::MEMORY_HOST_NEW,
                                                             isBorrowed, deviceId);
    } else {
        if (isDvpp) {
            dPtr_ = MemoryHelper::MakeShared<MxBase::TensorDptr>(usrData, shape,
                                                                 static_cast<MxBase::TensorDataType>(dataType),
                                                                 MxBase::MemoryData::MemoryType::MEMORY_DVPP,
                                                                 isBorrowed, deviceId);
        } else {
            dPtr_ = MemoryHelper::MakeShared<MxBase::TensorDptr>(usrData, shape,
                                                                 static_cast<MxBase::TensorDataType>(dataType),
                                                                 MxBase::MemoryData::MemoryType::MEMORY_DEVICE,
                                                                 isBorrowed, deviceId);
        }
    }
    if (dPtr_ == nullptr) {
        LogError << "Create Tensor dPtr failed, failed to allocated memory."<< GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
    }
}

void* Tensor::GetData() const
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "The dptr or tensorBase of tensor is nullptr in GetData."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return nullptr;
    }
    return dPtr_->tensorBase_->GetBuffer();
}

std::vector<uint32_t> Tensor::GetShape() const
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "The dptr or tensorBase of tensor is nullptr in GetShape."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return {};
    }
    return dPtr_->tensorBase_->GetShape();
}

APP_ERROR Tensor::SetShape(std::vector<uint32_t> shape)
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "Tensor is null when shape is set." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (shape.size() == 0) {
        LogError << "The input param shape is null when set tensor's shape."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    size_t src_size = 1;
    for (auto s: dPtr_->tensorBase_->GetShape()) {
        size_t tempSize = static_cast<size_t>(s);
        src_size *= tempSize;
    }
    size_t dst_size = 1;
    for (auto s: shape) {
        size_t tempSize = static_cast<size_t>(s);
        dst_size *= tempSize;
    }
    if (src_size != dst_size) {
        LogError << "Tensor shape to be set has the size (" << dst_size << ") does not match the previous size ("
                 << src_size << "), please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return dPtr_->tensorBase_->SetShape(shape);
}

MxBase::TensorDType Tensor::GetDataType() const
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "The dptr or tensorBase of tensor is nullptr in GetDataType."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return MxBase::TensorDType::UNDEFINED;
    }
    return static_cast<MxBase::TensorDType>(dPtr_->tensorBase_->GetDataType());
}

MemoryData::MemoryType Tensor::GetMemoryType() const
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogInfo << "The dptr or tensorBase of tensor is nullptr in GetMemoryType.";
        return MemoryData::MemoryType::MEMORY_HOST_NEW;
    }
    return dPtr_->tensorBase_->GetTensorType();
}

int32_t Tensor::GetDeviceId() const
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        return -1;
    }
    return dPtr_->tensorBase_->GetDeviceId();
}

APP_ERROR Tensor::ToDevice(int32_t deviceId)
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "The dptr or tensorBase of tensor is nullptr, when move Tensor to device."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (!dPtr_->IsDeviceValid(deviceId) || deviceId == -1) {
        LogError << "deviceId is invalid, when move Tensor to device." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = dPtr_->tensorBase_->ToDevice(deviceId);
    if (ret != APP_ERR_OK) {
        LogError << "Tensor to device failed." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR Tensor::ToDvpp(int32_t deviceId)
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "The dptr or tensorBase of tensor is nullptr, when move Tensor to dvpp."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (!dPtr_->IsDeviceValid(deviceId) || deviceId == -1) {
        LogError << "deviceId is invalid, when move Tensor to dvpp." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    APP_ERROR ret = dPtr_->tensorBase_->ToDvpp(deviceId);
    if (ret != APP_ERR_OK) {
        LogError << "Tensor to dvpp failed." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR Tensor::ToHost()
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "Tensor is null, when move Tensor to host." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INIT_FAIL;
    }
    APP_ERROR ret = dPtr_->tensorBase_->ToHost();
    if (ret != APP_ERR_OK) {
        LogError << "Tensor to host failed." << GetErrorInfo(ret);
    }
    return ret;
}

size_t Tensor::GetByteSize() const
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogInfo << "The dptr or tensorBase of tensor is nullptr in GetByteSize.";
        return 0;
    }
    return dPtr_->tensorBase_->GetByteSize();
}

APP_ERROR BatchConcat(const std::vector<Tensor> &inputs, Tensor &output)
{
    std::vector<TensorBase> tensors;
    for (auto tensor : inputs) {
        if (tensor.dPtr_ == nullptr || tensor.dPtr_->tensorBase_ == nullptr) {
            LogError << "Input tensors contains null tensor." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INIT_FAIL;
        }
        tensors.push_back(*(tensor.dPtr_->tensorBase_.get()));
    }
    TensorBase tensorOut;
    APP_ERROR ret = TensorBase::BatchConcat(tensors, tensorOut);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to Concat Tensors" << GetErrorInfo(ret);
        return ret;
    }

    output.dPtr_->tensorBase_ = MemoryHelper::MakeShared<MxBase::TensorBase>(tensorOut.GetShape(),
        tensorOut.GetDataType(), tensorOut.GetTensorType(), tensorOut.GetDeviceId());
    if (output.dPtr_->tensorBase_ == nullptr) {
        LogError << "Construct TensorBase failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    ret = TensorBase::TensorBaseMalloc(*(output.dPtr_->tensorBase_));
    if (ret != APP_ERR_OK) {
        LogError << "TensorBaseMalloc failed." << GetErrorInfo(ret);
        return ret;
    }
    *(output.dPtr_->tensorBase_) = tensorOut;

    return APP_ERR_OK;
}

static APP_ERROR CheckTransInputs(const Tensor &input, std::vector<uint32_t> &axes,
                                  size_t &elementSize, std::vector<uint32_t> &outputShape)
{
    if (input.GetShape().size() <= 1 || input.GetShape().size() > MAX_TRANS_TENSOR_DIM) {
        LogError << "Param check error, input Tensor dimension(" << input.GetShape().size() << ") "
                 << "is unsupported, input dimension should be within [2, 4]."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (axes.size() == 0) {
        for (int i = static_cast<int>(input.GetShape().size() - 1); i >= 0; --i) {
            axes.emplace_back(static_cast<uint32_t>(i));
        }
    }
    if (input.GetShape().size() != axes.size()) {
        LogError << "Param check error, tensor dimension(" << input.GetShape().size() <<
                    ") and axes size(" << axes.size() << ") do not match." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (input.GetDataType() == TensorDType::UINT8) {
        elementSize = ONE_BYTE;
    } else if (input.GetDataType() == TensorDType::FLOAT16) {
        elementSize = TWO_BYTE;
    } else if (input.GetDataType() == TensorDType::FLOAT32) {
        elementSize = FOUR_BYTE;
    } else {
        LogError << "Param check error, unsupported input tensor data type("
                 << GetTensorDataTypeDesc(static_cast<MxBase::TensorDataType>(input.GetDataType())) << "), "
                 << "supported data type include uint8, float16 and float32."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    auto orderedAxes = axes;
    std::sort(orderedAxes.begin(), orderedAxes.end());
    for (size_t i = 0; i < input.GetShape().size(); ++i) {
        if (orderedAxes[i] != i) {
            LogError << "Param check error, invalid tensor dimension(s) in axes, "
                     << "it should be within [0, " << input.GetShape().size() << ") "
                     << "and not duplicative." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        outputShape[i] = input.GetShape()[axes[i]];
    }
    return APP_ERR_OK;
}

static void SetTransParams(const Tensor &input, std::vector<uint32_t> &axes,
                           size_t &continuousIdx, std::vector<size_t> &steps)
{
    for (int i = static_cast<int>(axes.size()) - 1; i >= 0; --i) {
        if (static_cast<int>(axes[i]) != i) {
            continuousIdx = static_cast<size_t>(i + 1);
            break;
        }
    }
    for (size_t i = 0; i < steps.size(); ++i) {
        size_t step2 = 1;
        for (size_t j = axes[i] + 1; j < axes.size(); ++j) {
            step2 *= input.GetShape()[j];
        }
        steps[i] = step2;
    }
}

APP_ERROR Tensor::CheckPrivateParams(const Tensor &input, const Tensor &output)
{
    if (input.dPtr_ == nullptr || input.dPtr_->tensorBase_ == nullptr ||
        input.GetData() == nullptr || input.GetByteSize() == 0) {
        LogError << "Param check error, input tensor is empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (input.dPtr_->tensorBase_->IsDevice()) {
        LogError << "Param check error, input tensor should be on host." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (output.dPtr_->tensorBase_ != nullptr && output.GetData() != nullptr &&
        output.GetByteSize() == input.GetByteSize() && output.dPtr_->tensorBase_->IsDevice()) {
        LogError << "Param check error, output tensor should be on host." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR Tensor::DoTranspose(const Tensor &input, Tensor &output, std::vector<uint32_t> axes)
{
    if (CheckPrivateParams(input, output) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    size_t elementSize = 0;
    std::vector<uint32_t> outputShape(input.GetShape().size());
    if (CheckTransInputs(input, axes, elementSize, outputShape) != APP_ERR_OK) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (output.dPtr_->tensorBase_ == nullptr || output.GetData() == nullptr ||
        output.GetByteSize() != input.GetByteSize()) {
        output.dPtr_->tensorBase_ = MemoryHelper::MakeShared<MxBase::TensorBase>(outputShape,
            static_cast<MxBase::TensorDataType>(input.GetDataType()));
        if (output.dPtr_->tensorBase_ == nullptr) {
            LogError << "Construct tensor base failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
        }
        APP_ERROR ret = TensorBase::TensorBaseMalloc(*(output.dPtr_->tensorBase_));
        if (ret != APP_ERR_OK) {
            LogError << "Fail to malloc output tensor." << GetErrorInfo(ret);
            return ret;
        }
    }
    size_t continuousIdx = 0;
    std::vector<size_t> steps(axes.size());
    SetTransParams(input, axes, continuousIdx, steps);
    size_t step1 = 1;
    for (size_t i = continuousIdx; i < axes.size(); ++i) {
        step1 *= output.GetShape()[i];
    }
    size_t continuousSize = continuousIdx == 0 ? (output.GetByteSize() / elementSize) : step1;
    size_t outerSize = (output.GetByteSize() / elementSize) / continuousSize;
    auto* src = input.GetData();
    auto* dst = output.GetData();
    size_t srcOffset = 0;
    for (size_t i = 0; i < outerSize; ++i) {
        if (memcpy_s(dst, elementSize * continuousSize,
                     src + elementSize * srcOffset, elementSize * continuousSize) != EOK) {
            LogError << "Failed to copy memory data from input to output." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        dst += elementSize * continuousSize;
        for (int j = static_cast<int>(continuousIdx - 1); j >= 0; --j) {
            srcOffset += steps[j];
            if ((srcOffset / steps[j]) % output.GetShape()[j] != 0) {
                break;
            }
            srcOffset -= steps[j] * output.GetShape()[j];
        }
    }
    return APP_ERR_OK;
}

bool Tensor::IsEmpty() const
{
    if (dPtr_ == nullptr) {
        return true;
    }
    if (dPtr_->tensorBase_ == nullptr) {
        return true;
    }
    return dPtr_->tensorBase_->GetBuffer() == nullptr;
}

APP_ERROR Tensor::TensorFree(Tensor &tensor)
{
    if (tensor.dPtr_ == nullptr) {
        return APP_ERR_OK;
    }
    tensor.dPtr_.reset();
    return APP_ERR_OK;
}

APP_ERROR Tensor::SetValidRoi(Rect rect)
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "The dptr or tensorBase of tensor is nullptr in SetValidRoi."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return  APP_ERR_COMM_INVALID_PARAM;
    }
    return dPtr_->tensorBase_->SetValidRoi(rect);
}

Rect Tensor::GetValidRoi() const
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "The dptr or tensorBase of tensor is nullptr in GetValidRoi."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return {};
    }
    return dPtr_->tensorBase_->GetValidRoi();
}

bool Tensor::IsWithMargin() const
{
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "The dptr or tensorBase of tensor is nullptr in IsWithMargin."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return false;
    }
    Rect validRoi = GetValidRoi();
    if (validRoi.x0 == 0 && validRoi.x1 == 0 && validRoi.y0 == 0 && validRoi.y1 == 0) {
        return false;
    }
    auto tensorShape = GetShape();
    auto tensorDim = tensorShape.size();
    if (tensorDim < MIN_ROI_SET_DIM || tensorDim > MAX_ROI_SET_DIM) {
        LogError << "IsWithMargin is only available for the tensor with the shape of HW, HWC or NHWC(N = 1)."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (tensorDim == MAX_ROI_SET_DIM && GetShape()[SHAPE_DIMENSION_ZERO] != 1) {
        LogError << "IsWithMargin is only available for the tensor with the shape of HW, HWC or NHWC(N = 1)."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    uint32_t roiWidth = validRoi.x1 - validRoi.x0;
    uint32_t roiHeight = validRoi.y1 - validRoi.y0;
    uint32_t tensorWidth =
        tensorDim == MAX_ROI_SET_DIM ? tensorShape[SHAPE_DIMENSION_TWO] : tensorShape[SHAPE_DIMENSION_ONE];
    uint32_t tensorHeight =
        tensorDim == MAX_ROI_SET_DIM ? tensorShape[SHAPE_DIMENSION_ONE] : tensorShape[SHAPE_DIMENSION_ZERO];
    return !(roiHeight == tensorHeight && roiWidth == tensorWidth);
}

APP_ERROR Tensor::Clone(const Tensor &src, AscendStream &stream)
{
    if (!DeviceManager::IsAscend310P()) {
        LogError << "Clone referRect is only supported on 310P now." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (this->IsEmpty() || src.IsEmpty()) {
        LogError << "Tensor contains null tensor in Clone." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (src.GetReferRect().x1 == 0 || src.GetReferRect().y1 == 0 ||
        this->GetReferRect().x1 == 0 || this->GetReferRect().y1 == 0) {
        LogError << "The referRect should be set in both src and dst tensor."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    size_t srcSize = 1;
    for (auto s: dPtr_->tensorBase_->GetShape()) {
        size_t tempSize = static_cast<size_t>(s);
        if (tempSize != 0 && srcSize > MAX_CLONE_SIZE / tempSize) {
            LogError << "The size of src should be less than or equal to " << MAX_CLONE_SIZE
                << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        srcSize *= tempSize;
    }

    return dPtr_->ReferRectInplace(src, *this, stream);
}

static APP_ERROR MallocForDeviceAndDVPP(MemoryData &dst, const MemoryData::MemoryType tensorType,
                                        const size_t tensorByteSize)
{
    APP_ERROR ret = APP_ERR_OK;
    if (tensorType == MemoryData::MEMORY_DEVICE) {
        ret = DeviceMemoryMallocFunc(&(dst.ptrData), tensorByteSize, MX_MEM_MALLOC_NORMAL_ONLY);
        dst.free = DeviceMemoryFreeFunc;
        if (ret != APP_ERR_OK) {
            LogError << "Fail to malloc device memory." << GetErrorInfo(ret);
        }
    } else if (tensorType == MemoryData::MEMORY_DVPP) {
        if (DeviceManager::IsAscend310() || DeviceManager::IsAscend310B()) {
            ret = acldvppMalloc(&(dst.ptrData), tensorByteSize);
            dst.free = acldvppFree;
        } else if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
            ret = DVPPMemoryMallocFunc(dst.deviceId, &(dst.ptrData), tensorByteSize);
            dst.free = DVPPMemoryFreeFunc;
        }
        if (ret != APP_ERR_OK) {
            LogError << "Fail to malloc dvpp memory." << GetErrorInfo(ret, "acldvppMalloc");
        }
    } else {
        ret = APP_ERR_COMM_INVALID_PARAM;
        LogError << "Memory data cannot be host, please check!" << GetErrorInfo(ret);
    }
    return ret;
}

static bool CheckClonePara(const Tensor* tensor, AscendStream& stream)
{
    if (tensor->IsEmpty()) {
        LogError << "Tensor contains null tensor in Clone." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return false;
    }
    if (tensor->GetDeviceId() != -1 && tensor->GetDeviceId() != stream.GetDeviceId()) {
        LogError << "Tensor Device(" << tensor->GetDeviceId() << ") and Stream Device("
                 << stream.GetDeviceId() << ") do not match, please check."<< GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

Tensor Tensor::Clone(AscendStream& stream) const
{
    aclrtStream ascendStream = stream.stream;
    Tensor tensor;
    if (!CheckClonePara(this, stream)) {
        return tensor;
    }
    void *tensor_ptr = dPtr_->tensorBase_->GetBuffer();
    size_t tensor_byte_size = dPtr_->tensorBase_->GetByteSize();
    MemoryData::MemoryType tensor_type = dPtr_->tensorBase_->GetTensorType();
    int32_t tensor_device_id = dPtr_->tensorBase_->GetDeviceId();
    MemoryData dst(nullptr, tensor_byte_size, tensor_type, tensor_device_id);
    MemoryData src(tensor_ptr, tensor_byte_size, tensor_type, tensor_device_id);
    if (tensor_type == MemoryData::MEMORY_HOST || tensor_type == MemoryData::MEMORY_HOST_MALLOC ||
        tensor_type == MemoryData::MEMORY_HOST_NEW) {
        APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(dst, src);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to conduct MxbsMallocAndCopy in Clone operation." << GetErrorInfo(ret);
            return tensor;
        }
    }
    if (tensor_type == MemoryData::MEMORY_DEVICE || tensor_type == MemoryData::MEMORY_DVPP) {
        APP_ERROR ret = MallocForDeviceAndDVPP(dst, tensor_type, tensor_byte_size);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to malloc tensor in Clone operation." << GetErrorInfo(ret);
            return tensor;
        }
        ret = aclrtMemcpyAsync(dst.ptrData, tensor_byte_size, tensor_ptr, tensor_byte_size,
                               ACL_MEMCPY_DEVICE_TO_DEVICE, ascendStream);
        if (ret != ACL_SUCCESS) {
            LogError << "Failed to conduct aclrtMemcpyAsync in Clone operation."
                     << GetErrorInfo(ret, "aclrtMemcpyAsync");
            MemoryHelper::MxbsFree(dst);
            return tensor;
        }
    }
    auto shape = dPtr_->tensorBase_->GetShape();
    auto dataType = dPtr_->tensorBase_->GetDataType();
    tensor.dPtr_->tensorBase_ = MemoryHelper::MakeShared<MxBase::TensorBase>(dst, false, shape, dataType);
    if (stream.isDefault_) {
        APP_ERROR ret = stream.Synchronize();
        if (ret != APP_ERR_OK) {
            LogError << "stream Synchronize in Clone failed." << GetErrorInfo(ret);
        }
    }
    return tensor;
}

APP_ERROR SetTensorValueProcess(SetTensorValuePara *para)
{
    if (para == nullptr) {
        return APP_ERR_COMM_INVALID_PARAM;
    }
    void* mem = new (std::nothrow) uint8_t [para->tensorByteSize];
    if (mem == nullptr) {
        delete para;
        LogError << "Fail to new tensor array in SetTensorValueProcess func." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    MemoryData memoryDataDst(para->tensorPtr, para->tensorByteSize, para->tensorType, para->tensorDeviceId);
    MemoryData memoryDataSrc(mem, para->tensorByteSize, MemoryData::MEMORY_HOST_NEW, -1);
    switch (para->dataType) {
        case TensorDType::INT32:
            for (size_t i = 0; i < para->tensorSize; i++) {
                int *dstData = static_cast<int *>(mem);
                dstData[i] = para->value.int32Value;
            }
            break;
        case TensorDType::UINT8:
            for (size_t i = 0; i < para->tensorSize; i++) {
                uint8_t *dstData = static_cast<uint8_t *>(mem);
                dstData[i] = para->value.uint8Value;
            }
            break;
        default:
            if (para->isFloat16) {
                aclFloat16 *dstData = static_cast<aclFloat16 *>(mem);
                aclFloat16 out = aclFloatToFloat16(para->value.floatValue);
                for (size_t i = 0; i < para->tensorSize; i++) {
                    dstData[i] = out;
                }
            } else {
                float *dstData = static_cast<float *>(mem);
                for (size_t i = 0; i < para->tensorSize; i++) {
                    dstData[i] = para->value.floatValue;
                }
            }
    }
    APP_ERROR ret = MemoryHelper::MxbsMemcpy(memoryDataDst, memoryDataSrc, memoryDataSrc.size);
    delete []static_cast<uint8_t *>(mem);
    delete para;
    if (ret != APP_ERR_OK) {
        LogError << "Fail to copy memory data from host to destination in SetTensorValueProcess."
                 << GetErrorInfo(ret);
    }
    return ret;
}

void SetTensorValueCallbackFunc(void *arg)
{
    if (arg == nullptr) {
        LogError << "void *arg is nullptr in SetTensorValueCallbackFunc." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return;
    }
    SetTensorValuePara *para = static_cast<SetTensorValuePara *>(arg);
    LogDebug << "In SetTensorValueCallbackFunc"
             << " tensorSize: " << para->tensorSize << ", tensorPtr: " << para->tensorPtr
             << ", tensorByteSize: " << para->tensorByteSize << ", tensorType: "<< para->tensorType
             << ", tensorDeviceId: " << para->tensorDeviceId << "." ;
    switch (para->dataType) {
        case TensorDType::INT32:
            LogDebug << "Tensor data type: int32, value:" << para->value.int32Value << "." ;
            break;
        case TensorDType::UINT8:
            LogDebug << "Tensor data type: uint8, value:" << para->value.uint8Value << "." ;
            break;
        default:
            if (para->isFloat16) {
                LogDebug << "Tensor data type: float16, value:" << para->value.floatValue << "." ;
            } else {
                LogDebug << "Tensor data type: float32, value:" << para->value.floatValue << "." ;
            }
    }
    APP_ERROR ret = SetTensorValueProcess(para);
    // Throw error.
    if (ret != APP_ERR_OK) {
        LogError << "Execute SetTensorValue failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(ret));
    }
}
APP_ERROR SetTensorValueCallback(SetTensorValuePara &para, AscendStream &stream)
{
    // Call the call back function.
    APP_ERROR ret = aclrtLaunchCallback(SetTensorValueCallbackFunc,
                                        static_cast<void *> (&para), ACL_CALLBACK_BLOCK, stream.stream);
    if (ret != APP_ERR_OK) {
        delete &para;
        LogError <<"Fail to launch call back function in SetTensorValueCallback func."
                 << GetErrorInfo(ret, "aclrtLaunchCallback");
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR Tensor::SetTensorValue(uint8_t value, AscendStream& stream)
{
    if (GetDataType() != TensorDType::UINT8) {
        LogError << "Tensor data type is not uint8, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "Param check error, input tensor is empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    void *tensor_ptr = dPtr_->tensorBase_->GetBuffer();
    size_t tensor_byte_size = dPtr_->tensorBase_->GetByteSize();

    MemoryData::MemoryType tensor_type = dPtr_->tensorBase_->GetTensorType();
    if (GetDeviceId() < 0) {
        LogError <<"Input tensor cannot be on the host, please check." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    int32_t tensor_device_id = dPtr_->tensorBase_->GetDeviceId();

    MemoryData memoryData(tensor_ptr, tensor_byte_size, tensor_type, tensor_device_id);

    if (stream.isDefault_) {
        return MemoryHelper::MxbsMemset(memoryData, static_cast<int32_t>(value), memoryData.size);
    }
    return MemoryHelper::MxbsMemset(memoryData, static_cast<int32_t>(value), memoryData.size, stream);
}

APP_ERROR Tensor::SetTensorValue(int32_t value, AscendStream& stream)
{
    if (GetDataType() != TensorDType::INT32) {
        LogError << "Tensor data type is not int32, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "Param check error, input tensor is empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    void *tensor_ptr = dPtr_->tensorBase_->GetBuffer();
    size_t tensor_byte_size = dPtr_->tensorBase_->GetByteSize();
    size_t tensor_size = dPtr_->tensorBase_->GetSize();

    MemoryData::MemoryType tensor_type = dPtr_->tensorBase_->GetTensorType();
    if (GetDeviceId() < 0) {
        LogError <<"Input tensor cannot be on the host, please check." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    int32_t tensor_device_id = dPtr_->tensorBase_->GetDeviceId();
    SetTensorValuePara *para = new (std::nothrow) SetTensorValuePara;
    if (para == nullptr) {
        LogError << "Fail to new SetTensorValuePara in SetTensorValue func." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    TensorValue tensorValue;
    tensorValue.int32Value = value;
    para->value = tensorValue;
    para->tensorSize = tensor_size;
    para->tensorPtr = tensor_ptr;
    para->tensorByteSize = tensor_byte_size;
    para->tensorType = tensor_type;
    para->tensorDeviceId = tensor_device_id;
    para->dataType = TensorDType::INT32;
    // Execute synchronize SetTensorValueProcess.
    if (stream.isDefault_) {
        return SetTensorValueProcess(para);
    }
    // Call the call back function
    return SetTensorValueCallback(*para, stream);
}

APP_ERROR Tensor::SetTensorValue(float value, bool isFloat16, AscendStream &stream)
{
    if (GetDataType() != TensorDType::FLOAT32 && !isFloat16) {
        LogError << "Tensor data type is not float32, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (GetDataType() != TensorDType::FLOAT16 && isFloat16) {
        LogError << "Tensor data type is not float16, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "Param check error, input tensor is empty." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    void *tensor_ptr = dPtr_->tensorBase_->GetBuffer();
    size_t tensor_byte_size = dPtr_->tensorBase_->GetByteSize();
    size_t tensor_size = dPtr_->tensorBase_->GetSize();

    MemoryData::MemoryType tensor_type = dPtr_->tensorBase_->GetTensorType();
    int32_t tensor_device_id = dPtr_->tensorBase_->GetDeviceId();
    if (GetDeviceId() < 0) {
        LogError <<"Input tensor cannot be on the host, please check." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    TensorDType dataType = isFloat16 ? TensorDType::FLOAT16: TensorDType::FLOAT32;
    SetTensorValuePara *para = new (std::nothrow) SetTensorValuePara;
    if (para == nullptr) {
        LogError << "Fail to new SetTensorValuePara in SetTensorValue func." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    TensorValue tensorValue;
    tensorValue.floatValue = value;
    para->value = tensorValue;
    para->tensorSize = tensor_size;
    para->tensorPtr = tensor_ptr;
    para->tensorByteSize = tensor_byte_size;
    para->tensorType = tensor_type;
    para->tensorDeviceId = tensor_device_id;
    para->dataType = dataType;
    para->isFloat16 = isFloat16;
    // Execute synchronize SetTensorValueProcess.
    if (stream.isDefault_) {
        return SetTensorValueProcess(para);
    }
    // Execute asynchronize SetTensorValueCallback.
    return SetTensorValueCallback(*para, stream);
}

APP_ERROR Tensor::SetReferRect(Rect rect)
{
    if (!DeviceManager::IsAscend310P()) {
        LogError << "SetReferRect is only supported on 310P now." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dPtr_ == nullptr || dPtr_->tensorBase_ == nullptr) {
        LogError << "The dptr or tensorBase of tensor is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    auto tensorShape = GetShape();
    size_t dim = tensorShape.size();
    if (dim < MIN_ROI_SET_DIM || dim > MAX_ROI_SET_DIM) {
        LogError << "SetReferRect is only available for the tensor of HW, HWC and NHWC, but get " << dim << " dims."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dim == MAX_ROI_SET_DIM && tensorShape[0] != 1) {
        LogError << "Batch dim must be 1 if the input tensor is NHWC." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dim > MIN_ROI_SET_DIM && tensorShape[dim - 1] != 1 && tensorShape[dim - 1] != BGR_CHANNEL &&
        tensorShape[dim - 1] != BGRA_CHANNEL) {
        LogError << "Tensor channel must be 1, " << BGR_CHANNEL << " or " << BGRA_CHANNEL << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (rect.x1 <= rect.x0 || rect.y1 <= rect.y0) {
        LogError << "The (x0, y0) of refer rect should strictly be located on the left top of (x1, y1)."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    size_t w = rect.x1 - rect.x0;
    size_t h = rect.y1 - rect.y0;
    size_t offset = dim == MAX_ROI_SET_DIM ? 1 : 0;
    if (w > tensorShape[offset + 1] || h > tensorShape[offset] || rect.x1 > tensorShape[offset + 1] ||
        rect.y1 > tensorShape[offset]) {
        LogError << "SetReferRect failed, rect width(" << w << ") or x1(" << rect.x1 << ") exceed the tensor width("
                 << tensorShape[offset + 1] << "), or height(" << h << ") or y1(" << rect.y1 << ") exceed the tensor "
                 << "height(" << tensorShape[offset] << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    dPtr_->referRect_ = rect;
    return APP_ERR_OK;
}

Rect Tensor::GetReferRect() const
{
    if (dPtr_ == nullptr) {
        LogError << "The dptr of tensor is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return {};
    }
    return dPtr_->referRect_;
}
}
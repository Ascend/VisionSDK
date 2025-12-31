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
 * Description: Tensor Check Framework CommonUtils implement file.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "MxBase/E2eInfer/TensorOperation/TensorCheckUtils.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"

namespace MxBase {
TensorImgParam::TensorImgParam(std::string name, const Tensor &tensor, bool isAllowEmpty)
    : tensorName(name), tensor_(tensor), allowEmpty(isAllowEmpty)
{
    if (tensor.IsEmpty() && allowEmpty) {
        return;
    }
    deviceId = tensor.GetDeviceId();
    std::vector<uint32_t> shape = tensor.GetShape();
    dim = shape.size();
    switch (dim) {
        case DIM_HWC: // HWC
            height = shape[IDX_HWC_H];
            width = shape[IDX_HWC_W];
            channels = shape[IDX_HWC_C];
            break;
        case DIM_HW: // HW
            height = shape[IDX_HWC_H];
            width = shape[IDX_HWC_W];
            channels = 1;
            break;
        default:
            height = 0;
            width = 0;
            channels = 0;
    }

    Rect roi = tensor.GetReferRect();
    bool isEmptyRoi = (roi.x1 == 0 && roi.y1 == 0 && roi.x0 == 0 && roi.y0 == 0);
    if (isEmptyRoi) {
        roiX0 = 0;
        roiY0 = 0;
        roiX1 = width;
        roiY1 = height;
    } else {
        roiX0 = roi.x0;
        roiY0 = roi.y0;
        roiX1 = roi.x1;
        roiY1 = roi.y1;
    }
    roiH = roiY1 - roiY0;
    roiW = roiX1 - roiX0;
    roiStart = (width * roiY0 + roiX0) * channels;

    memType = tensor.GetMemoryType();
    dataType = tensor.GetDataType();
}

TensorImgParamChecker::TensorImgParamChecker(std::string tensorName, std::vector<uint32_t> &hwRange,
    std::vector<MemoryData::MemoryType> &memTypeChoices, std::vector<TensorDType> &dataTypeChoices,
    std::vector<uint32_t> &channelChoices, std::vector<uint32_t> &dimChoices, bool allowEmpty)
    : tensorName_(tensorName),
      hwRange_(hwRange),
      memTypeChoices_(memTypeChoices),
      dataTypeChoices_(dataTypeChoices),
      channelChoices_(channelChoices),
      dimChoices_(dimChoices),
      allowEmpty_(allowEmpty)
{}

std::string TensorImgParamChecker::ToString(TensorDType dataType)
{
    return GetTensorDataTypeDesc(static_cast<MxBase::TensorDataType>(dataType));
}

std::string TensorImgParamChecker::ToString(MemoryData::MemoryType memoryTyp)
{
    switch (memoryTyp) {
        case MemoryData::MEMORY_HOST:
            return "host";
        case MemoryData::MEMORY_DEVICE:
            return "device";
        case MemoryData::MEMORY_DVPP:
            return "dvpp";
        case MemoryData::MEMORY_HOST_MALLOC:
            return "host_malloc";
        case MemoryData::MEMORY_HOST_NEW:
            return "host_new";
        default:
            return "undefined";
    }
}

bool TensorImgParamChecker::CheckNotEmpty(const Tensor &tensor)
{
    if (tensor.IsEmpty()) {
        LogError << "CheckNotEmpty: The tensor " << tensorName_ << " is empty, please check!" <<
            GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool TensorImgParamChecker::CheckDim(uint32_t dim)
{
    if (std::find(dimChoices_.begin(), dimChoices_.end(), dim) == dimChoices_.end()) {
        LogError << "CheckDim: The tensor " << tensorName_ << " dim " << dim <<
            " is not an available option, please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool TensorImgParamChecker::CheckMemType(MemoryData::MemoryType memType)
{
    if (std::find(memTypeChoices_.begin(), memTypeChoices_.end(), memType) == memTypeChoices_.end()) {
        LogError << "CheckMemType: The tensor " << tensorName_ << " memory type(" << ToString(memType) << ")"
                 << " is not an available option, please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool TensorImgParamChecker::CheckDataType(TensorDType dataType)
{
    if (std::find(dataTypeChoices_.begin(), dataTypeChoices_.end(), dataType) == dataTypeChoices_.end()) {
        LogError << "CheckDataType: The tensor " << tensorName_ << " data type " << ToString(dataType)
                 << " is not an available option, please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool TensorImgParamChecker::CheckChannels(uint32_t channels)
{
    if (std::find(channelChoices_.begin(), channelChoices_.end(), channels) == channelChoices_.end()) {
        LogError << "CheckChannels: The tensor " << tensorName_ << " channels " << channels <<
            " is not an available option, please check!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool TensorImgParamChecker::CheckImgSize(uint32_t width, uint32_t height)
{
    uint32_t minW = hwRange_[IDX_MIN_W];
    uint32_t minH = hwRange_[IDX_MIN_H];
    uint32_t maxW = hwRange_[IDX_MAX_W];
    uint32_t maxH = hwRange_[IDX_MAX_H];

    if (width < minW || width > maxW || height < minH || height > maxH) {
        LogError << "CheckImgSize: The tensor " << tensorName_ << " width, height (" << width << ", " << height << ") "
                 << "should be between " << minW << "x" << minH << " and " << maxW << "x" << maxH << "." <<
            GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

bool TensorImgParamChecker::Check(const TensorImgParam &tensorImgParam)
{
    if (allowEmpty_ && tensorImgParam.tensor_.IsEmpty()) {
        return true;
    }
    return CheckNotEmpty(tensorImgParam.tensor_) && CheckMemType(tensorImgParam.memType) &&
        CheckDataType(tensorImgParam.dataType) && CheckDim(tensorImgParam.dim) &&
        CheckChannels(tensorImgParam.channels) && CheckImgSize(tensorImgParam.width, tensorImgParam.height);
}
}
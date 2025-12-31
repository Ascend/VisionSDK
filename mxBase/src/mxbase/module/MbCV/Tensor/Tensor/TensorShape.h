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
 * Description: Tensor shape and size related settings.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef TENSOR_SHAPE_H
#define TENSOR_SHAPE_H

#include "MxBase/E2eInfer/Rect/Rect.h"

namespace MxBase {
const uint32_t MAX_MEMORY_LIMIT = 512 * 1024 * 1024; // memory limit of shape size (512M)
const int MIN_ROI_SET_DIM = 2; // min roi dim for setting
const int GRAY_ROI_SET_DIM = 3; // min roi dim for setting
const int MAX_ROI_SET_DIM = 4; // max roi dim for setting
class TensorShape {
public:
    TensorShape() = default;
    ~TensorShape() = default;

    template<typename T>
    explicit TensorShape(std::vector<T> shape)
    {
        shape_.clear();
        size_t size = 1;
        for (auto s : shape) {
            size_t tempSize = (size_t)s;
            if (tempSize != 0 && size >= MAX_MEMORY_LIMIT / tempSize) {
                throw std::runtime_error("Maximum memory capacity exceeded");
            }
            size *= tempSize;
            shape_.push_back(tempSize);
        }
    }

    uint32_t GetDims() const
    {
        return shape_.size();
    }

    template<typename T>
    void SetShape(std::vector<T> shape)
    {
        shape_.clear();
        size_t size = 1;
        for (auto s : shape) {
            size_t tempSize = (size_t)s;
            if (tempSize != 0 && size >= MAX_MEMORY_LIMIT / tempSize) {
                throw std::runtime_error("Maximum memory capacity exceeded");
            }
            size *= tempSize;
            shape_.push_back(tempSize);
        }
    }

    std::vector<uint32_t> GetShape() const
    {
        size_t shapeSize = shape_.size();
        std::vector<uint32_t> shape(shapeSize);
        for (size_t i = 0; i < shapeSize; i++) {
            shape[i] = static_cast<uint32_t>(shape_[i]);
        }
        return shape;
    }

    uint32_t GetSize() const
    {
        size_t size = 1;
        for (auto s : shape_) {
            size *= s;
        }
        return size;
    }

    size_t operator[](uint32_t idx) const
    {
        if (idx >= shape_.size()) {
            LogError << "The index is out of range. index:" << idx << "." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
            return shape_[0];
        }
        return shape_[idx];
    }

    size_t &operator[](uint32_t idx)
    {
        if (idx >= shape_.size()) {
            LogError << "The index is out of range. index:" << idx << "." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
            return shape_[0];
        }
        return shape_[idx];
    }

    APP_ERROR SetValidRoi(Rect rect)
    {
        size_t tensorDim = GetDims();
        if (tensorDim < MIN_ROI_SET_DIM || tensorDim > MAX_ROI_SET_DIM) {
            LogError << "SetValidRoi is only available for the tensor with the shape of HW, HWC or NHWC(N = 1)."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (tensorDim == MAX_ROI_SET_DIM && GetShape()[0] != 1) {
            LogError << "SetValidRoi is only available for the tensor with the shape of HW, HWC or NHWC(N = 1)."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (rect.x1 <= rect.x0 || rect.y1 <= rect.y0) {
            LogError << "x1 and y1 need to be larger than x0, y0 respectively."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (rect.x0 != 0 || rect.y0 != 0) {
            LogError << "Roi start coordinate (x0, y0) must be (0, 0)."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return  APP_ERR_COMM_INVALID_PARAM;
        }
        size_t heightDim = 0x0;
        size_t widthDim = 0x1;
        if (tensorDim == MAX_ROI_SET_DIM || (tensorDim == GRAY_ROI_SET_DIM && shape_[heightDim] == 1)) {
            widthDim++;
            heightDim++;
        }
        if (rect.y1 > shape_[heightDim] || rect.x1 > shape_[widthDim]) {
            LogError << "SetValidRoi failed, set rect x1(" << rect.x1 << ") or y1(" << rect.y1
                     << ") exceed the maximum width(" << shape_[widthDim] << ") or maximum height("
                     << shape_[heightDim] << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        validRoi_ = rect;
        return APP_ERR_OK;
    }

    Rect GetValidRoi() const
    {
        return validRoi_;
    }

private:
    std::vector<size_t> shape_ = {0};
    Rect validRoi_ = {};
};
}

#endif
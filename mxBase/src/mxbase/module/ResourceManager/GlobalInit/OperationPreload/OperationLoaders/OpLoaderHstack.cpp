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
 * Description: Hstack op preload from json config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "OpLoaderHstack.h"

namespace {
    constexpr size_t NHWC_SHAPE_SIZE = 4;
    constexpr size_t HW_HWC_DIMENSION_ONE = 1;
    constexpr size_t NHWC_DIMENSION_TWO = 2;
}

namespace MxBase {
    OpLoaderHstack::OpLoaderHstack(): OpLoader() {}

    OpLoaderHstack::OpLoaderHstack(std::string opType): OpLoader(opType) {}

    APP_ERROR OpLoaderHstack::CheckOpCustom(std::string inputShape, std::string outputShape)
    {
        auto inputShapeVecs = GetShapeVecs(inputShape);
        auto outputShapeVecs = GetShapeVecs(outputShape);
        if (outputShapeVecs.size() != 1) {
            LogError << "CheckOpCustom: Check Vstack op output vec num is not 1, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        auto numOutDim = outputShapeVecs[0].size();
        if ((numOutDim < HW_DIM) || (numOutDim > NHWC_DIM)) {
            LogError << "CheckOpCustom: Check Vstack op output dimension should be 2 or 4, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        size_t widthDim = HW_HWC_DIMENSION_ONE;
        auto shape = inputShapeVecs[0];
        if (shape.size() == NHWC_SHAPE_SIZE) {
            widthDim = NHWC_DIMENSION_TWO;
        }
        int64_t widthSum = 0;
        for (auto inShape: inputShapeVecs) {
            for (size_t i = 0; i < inShape.size(); ++i) {
                if (i == static_cast<uint64_t>(widthDim)) {
                    widthSum += inShape[widthDim];
                } else if (inShape[i] != outputShapeVecs[0][i]) {
                    LogError << "CheckGeneralOpParams: Input Tensors have different shapes(except dimension of width)!"
                             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
            }
        }
        auto numOutWidth = outputShapeVecs[0][widthDim];
        if (numOutWidth != widthSum) {
            LogError << "CheckOpCustom: Check Vstack op input and output vec width(" << widthSum << ","
                     << numOutWidth << ") num do not match, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        return APP_ERR_OK;
    }

    APP_ERROR OpLoaderHstack::OpCreateParamAttr(aclopAttr *attr, const JsonPtr &jsonPtr, size_t index)
    {
        const size_t maxShapeSize = 4;
        const size_t minShapeSize = 2;
        std::string inputShape;
        auto ret = jsonPtr.GetInputShapeByIndex(index, inputShape);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamAttr: Hstack inputShape is invalid, please check."
                     << GetErrorInfo(ret);
            return ret;
        }
        std::vector<std::vector<int64_t>> shapeVecs = GetShapeVecs(inputShape);
        for (auto shape: shapeVecs) {
            if (shape.size() > maxShapeSize || shape.size() < minShapeSize) {
                LogError << "OpCreateParamAttr: Hstack input shape incorrect, please check the config file."
                         << GetErrorInfo(ret);
                return ret;
            }
        }
        int64_t num = static_cast<int64_t>(shapeVecs.size());
        auto inputShapeVecs = GetShapeVecs(inputShape);
        size_t widthDim = HW_HWC_DIMENSION_ONE;
        if (inputShapeVecs.size() == NHWC_SHAPE_SIZE) {
            widthDim = NHWC_DIMENSION_TWO;
        }
        int64_t concatDim = static_cast<int64_t>(widthDim);
        LogDebug << "set attr N: " << num << " attr concat_dim: " << concatDim;
        aclopSetAttrInt(attr, "N", num);
        aclopSetAttrInt(attr, "concat_dim", concatDim);
        return APP_ERR_OK;
    }
}

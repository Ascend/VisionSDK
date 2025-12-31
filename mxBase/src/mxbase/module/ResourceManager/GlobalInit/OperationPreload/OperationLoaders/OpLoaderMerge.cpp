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
 * Description: Merge op preload from json config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "OpLoaderMerge.h"

namespace MxBase {
    OpLoaderMerge::OpLoaderMerge(): OpLoader() {}

    OpLoaderMerge::OpLoaderMerge(std::string opType): OpLoader(opType) {}

    APP_ERROR OpLoaderMerge::CheckOpCustom(std::string inputShape, std::string outputShape)
    {
        auto inputShapeVecs = GetShapeVecs(inputShape);
        auto outputShapeVecs = GetShapeVecs(outputShape);
        if (outputShapeVecs.size() != 1) {  // 1. Only one output.
            LogError << "CheckOpCustom: Check Merge op output vec num is not 1, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        auto numOutChannel = outputShapeVecs[0][outputShapeVecs[0].size() - 1];
        if ((numOutChannel < HWC_DIM) || (numOutChannel > NHWC_DIM)) {  // RGB or RGBA.
            LogError << "CheckOpCustom: Check Merge op output channel should be 3 or 4, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        int64_t sumInputChannel = 0;
        for (auto inShape: inputShapeVecs) {
            for (size_t i = 0; i < inShape.size() - 1; i++) {
                if (inShape[i] != outputShapeVecs[0][i]) {  // 2. Input and output shape match except channel.
                    LogError << "CheckOpCustom: Check Merge op output vec shape does not match input, please check!"
                             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
            }
            sumInputChannel += inShape[inShape.size() - 1];
        }
        if (numOutChannel != sumInputChannel) {  // 3. Input channel sum equals to output channel num.
            LogError << "CheckOpCustom: Check Merge op input and output vec channel(" << sumInputChannel << ","
                     << numOutChannel << ") num do not match, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        return APP_ERR_OK;
    }

    APP_ERROR OpLoaderMerge::OpCreateParamAttr(aclopAttr *attr, const JsonPtr &jsonPtr, size_t index)
    {
        const size_t maxShapeSize = 4;
        const size_t minShapeSize = 3;
        std::string inputShape;
        auto ret = jsonPtr.GetInputShapeByIndex(index, inputShape);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamAttr: Merge inputShape is invalid, please check." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<std::vector<int64_t>> shapeVecs = GetShapeVecs(inputShape);
        for (auto shape: shapeVecs) {
            if (shape.size() > maxShapeSize || shape.size() < minShapeSize) {
                LogError << "OpCreateParamAttr: Merge input shape incorrect, please check the config file."
                         << GetErrorInfo(APP_ERR_COMM_FAILURE);
                return ret;
            }
        }
        int64_t num = static_cast<int64_t>(shapeVecs.size());
        int64_t concatDim = static_cast<int64_t>(shapeVecs[0].size() - 1);
        LogDebug << "set attr N: " << num << " attr concat_dim: " << concatDim;
        aclopSetAttrInt(attr, "N", num);
        aclopSetAttrInt(attr, "concat_dim", concatDim);
        return APP_ERR_OK;
    }
}

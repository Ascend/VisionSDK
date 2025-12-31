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
 * Description: Config load for preload jason config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "MxBase/Utils/StringUtils.h"
#include "OpLoaderSplit.h"

namespace MxBase {
    OpLoaderSplit::OpLoaderSplit(): OpLoader() {}

    OpLoaderSplit::OpLoaderSplit(std::string opType): OpLoader(opType) {}

    APP_ERROR OpLoaderSplit::CheckOpCustom(std::string inputShape, std::string outputShape)
    {
        auto inputShapeVecs = GetShapeVecs(inputShape);
        auto outputShapeVecs = GetShapeVecs(outputShape);

        if (inputShapeVecs.size() != 1) {  // 1. Only one input.
            LogError << "CheckOpCustom: Check Split op input vec num is not 1, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        auto numChannel = inputShapeVecs[0][inputShapeVecs[0].size() - 1];
        if ((numChannel < HWC_DIM) || (numChannel > NHWC_DIM)) {  // RGB or RGBA.
            LogError << "CheckOpCustom: Check Split op input channel should be 3 or 4, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if (static_cast<int>(outputShapeVecs.size()) != numChannel) {  // 2. Input channel equals to output vec num.
            LogError << "CheckOpCustom: Check Split op output vec num does not match input channel num, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        for (auto outShape: outputShapeVecs) {
            for (size_t i = 0; i < outShape.size() - 1; i++) {  // 3. Shape match except channel.
                if (outShape[i] != inputShapeVecs[0][i]) {
                    LogError << "CheckOpCustom: Check Split op output vec shape does not match input, please check!"
                             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
            }
            if (outShape[outShape.size() - 1] != 1) {  // 4. Output is single channel.
                LogError << "CheckOpCustom: Check Split op output vec last dimension is not 1, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }

        return APP_ERR_OK;
    }

    APP_ERROR OpLoaderSplit::OpCreateParamAttr(aclopAttr *attr, const JsonPtr &jsonPtr, size_t index)
    {
        std::string inputShape;
        auto ret = jsonPtr.GetInputShapeByIndex(index, inputShape);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamAttr: inputShape is invalid, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        std::vector<std::string> inputVect = StringUtils::Split(inputShape, ';');
        if (inputVect.size() == 0) {
            LogError << "OpCreateParamAttr: Invalid input vector size." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto shape = StringUtils::SplitAndCastToInt(inputVect[0], ',');
        std::vector<int64_t> shapeVec;
        for (size_t t = 0; t < shape.size(); t++) {
            shapeVec.push_back((int64_t) shape[t]);
        }
        int64_t splitDim = static_cast<int64_t>(shapeVec.size() - 1);
        int64_t numSplit = shapeVec[shapeVec.size() - 1];
        aclopSetAttrInt(attr, "split_dim", splitDim);
        aclopSetAttrInt(attr, "num_split", numSplit);
        return APP_ERR_OK;
    }
}
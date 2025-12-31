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
 * Description: Tile op preload from json config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "OpLoaderTile.h"

namespace MxBase {
    OpLoaderTile::OpLoaderTile(): OpLoader() {}

    OpLoaderTile::OpLoaderTile(std::string opType): OpLoader(opType) {}

    APP_ERROR OpLoaderTile::CheckOpCustom(std::string inputShape, std::string outputShape)
    {
        auto inputShapeVecs = GetShapeVecs(inputShape);
        auto outputShapeVecs = GetShapeVecs(outputShape);
        if ((inputShapeVecs.size() != 1) || (outputShapeVecs.size() != 1)) {
            LogError << "CheckOpCustom: Check Tile op inputShapeVec or outputShapeVec num is not 1."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto inShape = inputShapeVecs[0];  // [1, 3, 16, 16]
        auto outShape = outputShapeVecs[0];  // [2, 6, 16, 16]
        if (inShape.size() != outShape.size()) {
            LogError << "CheckOpCustom: Check Tile op inputShape and outputShape dimension does not match."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        for (size_t i = 0; i < inShape.size(); i++) {  // [1, 3, 16, 16] AND [1, 4, 16, 16] is invalid.
            if (outShape[i] % inShape[i] != 0) {
                LogError << "CheckOpCustom: Check Tile op outputShape(" << outShape[i] << "), inputShape(" << inShape[i]
                         << "), in dimension " << i << " is not valid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpLoaderTile::OpCreateParamTensor(const std::string inputShape,
                                                std::string inputDataType, OperatorDesc &opDesc)
    {
        LogInfo << "Enter Tile OpCreateParamTensor, inputShape: " << inputShape << ", inputDataType: " << inputDataType;
        APP_ERROR ret = APP_ERR_OK;
        std::vector<std::vector<int64_t>> shapeVecs = GetShapeVecs(inputShape);
        if (shapeVecs.size() == 0) {
            LogError << "OpCreateParamTensor: Tile invalid input shape, please check the config file."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        std::vector<int64_t> shapeVec = {static_cast<int64_t>(shapeVecs[0].size())};
        ret = opDesc.AddInputTensorDesc(shapeVec, OpDataType::OP_INT32);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamTensor: Tile AddInputTensorDesc failed." << GetErrorInfo(ret);
            return ret;
        }
        return ret;
    }
}
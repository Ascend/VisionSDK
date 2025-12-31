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
 * Description: Transpose op preload from json config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "OpLoaderTranspose.h"
#include <algorithm>
#include "MxBase/Utils/StringUtils.h"

namespace MxBase {
    OpLoaderTranspose::OpLoaderTranspose(): OpLoader() {}

    OpLoaderTranspose::OpLoaderTranspose(std::string opType): OpLoader(opType) {}

    APP_ERROR OpLoaderTranspose::CheckOpCustom(std::string inputShape, std::string outputShape)
    {
        auto inputShapeVecs = GetShapeVecs(inputShape);
        auto outputShapeVecs = GetShapeVecs(outputShape);
        if ((inputShapeVecs.size() != 1) || (outputShapeVecs.size() != 1)) {
            LogError << "CheckOpCustom: Check Transpose op input or output shape size is not 1, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        auto inShape = inputShapeVecs[0];
        auto outShape = outputShapeVecs[0];

        std::sort(inShape.begin(), inShape.end());
        std::sort(outShape.begin(), outShape.end());
        if (inShape != outShape) {
            LogError << "CheckOpCustom: Check Transpose op output shape and input shape permutation do not match."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpLoaderTranspose::OpCreateParamTensor(const std::string inputShape,
                                                     std::string inputDataType, OperatorDesc &opDesc)
    {
        APP_ERROR ret = APP_ERR_OK;
        std::vector<std::string> typeVec1 = StringUtils::Split(inputShape, ',');
        std::vector<std::string> typeVec2 = StringUtils::Split(inputDataType, ';');
        if (typeVec1.size() == 0 || typeVec2.size() == 0) {
            LogError << "OpCreateParamTensor: Transpose Invalid input shape, please check the config file."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        std::vector<std::int64_t> shapeVec = {static_cast<int64_t>(typeVec1.size())};
        ret = opDesc.AddInputTensorDesc(shapeVec, OpDataType::OP_INT32);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamTensor: Transpose AddInputTensorDesc failed." << GetErrorInfo(ret);
            return ret;
        }
        return ret;
    }
}
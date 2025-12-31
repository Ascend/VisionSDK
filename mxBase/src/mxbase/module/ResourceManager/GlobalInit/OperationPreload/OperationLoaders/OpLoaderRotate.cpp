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
#include "OpLoaderRotate.h"

namespace MxBase {
    OpLoaderRotate::OpLoaderRotate() : OpLoader() {}

    OpLoaderRotate::OpLoaderRotate(std::string opType) : OpLoader(opType) {}

    APP_ERROR OpLoaderRotate::CheckOpCustom(std::string inputShape, std::string outputShape)
    {
        auto inputShapeVecs = GetShapeVecs(inputShape);
        auto outputShapeVecs = GetShapeVecs(outputShape);
        if ((inputShapeVecs.size() != 1) || (outputShapeVecs.size() != 1)) {
            LogError << "CheckOpCustom: Check Rotate op input or output vec number is invalid, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        auto inShape = inputShapeVecs[0];
        auto outShape = outputShapeVecs[0];
        if (inShape.size() == HWC_DIM) {
            if (inShape[HWC_DIM - 1] != outShape[HWC_DIM - 1]) {
                LogError << "CheckOpCustom: Check Rotate op input and output shape do not match, please check."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }
        if ((inShape[0] == outShape[0]) && (inShape[1] == outShape[1])) {
            return APP_ERR_OK;
        } else if ((inShape[0] == outShape[1]) && (inShape[1] == outShape[0])) {
            return APP_ERR_OK;
        } else {
            LogError << "CheckOpCustom: Check Rotate op input and output shape do not match, please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpLoaderRotate::OpPreload(const JsonPtr &jsonPtr, size_t index, std::string opName, OpSettings opSets)
    {
        LogInfo << "Enter OpPreload: " << opName << ".";
        APP_ERROR ret;
        if (opSets.inputType != opSets.outputType) {
            LogError << "OpPreload: Rotate Op inputType and outputType do not match, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        std::vector<std::string> inputShapeVec = StringUtils::Split(opSets.inputShape, ',');
        std::vector<std::string> outputShapeVec = inputShapeVec;
        std::swap(outputShapeVec[0], outputShapeVec[1]);
        std::string shape1 = opSets.inputShape;
        std::string shape2;
        for (size_t j = 0; j < outputShapeVec.size(); j++) {
            shape2 += outputShapeVec[j];
            if (j == outputShapeVec.size() - 1) {
                break;
            }
            shape2 += ",";
        }

        std::string shapePerm = std::to_string(inputShapeVec.size());
        std::string shapeIn1 = shape1 + ";" + shapePerm;
        std::string typeIn1 = opSets.inputType + ";int32";
        OpSettings opSetsTranspose = {shapeIn1, typeIn1, shape2, opSets.inputType};
        ret = OpLoader::OpPreload(jsonPtr, index, "Transpose", opSetsTranspose);
        if (ret != APP_ERR_OK) {
            LogError << "OpPreload: Fail to precompile op Rotate: Transpose." << GetErrorInfo(ret);
            return ret;
        }
        std::string shapeIn2 = shape1 + ";1";
        OpSettings opSetsReverse1 = {shapeIn2, typeIn1, shape1, opSets.inputType};
        ret = OpLoader::OpPreload(jsonPtr, index, "ReverseV2", opSetsReverse1);
        if (ret != APP_ERR_OK) {
            LogError << "OpPreload: Fail to precompile op Rotate: ReverseV2." << GetErrorInfo(ret);
            return ret;
        }
        std::string shapeIn3 = shape2 + ";1";
        OpSettings opSetsReverse2 = {shapeIn3, typeIn1, shape2, opSets.inputType};
        ret = OpLoader::OpPreload(jsonPtr, index, "ReverseV2", opSetsReverse2);
        if (ret != APP_ERR_OK) {
            LogError << "OpPreload: Fail to precompile op Rotate: ReverseV2." << GetErrorInfo(ret);
            return ret;
        }
        return APP_ERR_OK;
    }
}
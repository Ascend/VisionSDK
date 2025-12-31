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
 * Description: Reduce op preload from json config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "OpLoaderReduce.h"
#include <algorithm>
#include "MxBase/Utils/StringUtils.h"

namespace {
    constexpr size_t HW_SHAPE_SIZE = 2;
    constexpr size_t HWC_SHAPE_SIZE = 3;
    constexpr size_t NHWC_SHAPE_SIZE = 4;
    constexpr size_t TENSOR_DIMENSION_ZERO = 0;
    constexpr size_t TENSOR_DIMENSION_ONE = 1;
    constexpr size_t TENSOR_DIMENSION_TWO = 2;
    constexpr size_t TENSOR_DIMENSION_THREE = 3;
    constexpr int64_t ONE_CHANNEL = 1;
    constexpr int64_t FOUR_CHANNEL = 4;
}
namespace MxBase {
    OpLoaderReduce::OpLoaderReduce(): OpLoader() {}

    OpLoaderReduce::OpLoaderReduce(std::string opType): OpLoader(opType) {}

    APP_ERROR OpLoaderReduce::CheckOpCustom(std::string inputShape, std::string outputShape)
    {
        auto inputShapeVecs = GetShapeVecs(inputShape);
        auto outputShapeVecs = GetShapeVecs(outputShape);
        if ((inputShapeVecs.size() != 1) || (outputShapeVecs.size() != 1)) {
            LogError << "CheckOpCustom: Reduce op input or output shape size is not 1, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        auto inShape = inputShapeVecs[0];
        auto outShape = outputShapeVecs[0];
        std::vector<int64_t> dstShape0 = {};
        std::vector<int64_t> dstShape1 = {};
        size_t srcDimNum = inShape.size();
        switch (srcDimNum) {
            case HW_SHAPE_SIZE:
                 // {W} or {H}
                dstShape0 = std::vector<int64_t>{inShape[TENSOR_DIMENSION_ONE]};
                dstShape1 = std::vector<int64_t>{inShape[TENSOR_DIMENSION_ZERO]};
                break;
            case NHWC_SHAPE_SIZE:
                // {N, W, C} or {N, H, C}
                dstShape0 = std::vector<int64_t>{inShape[0], inShape[TENSOR_DIMENSION_TWO], inShape[srcDimNum - 1]};
                dstShape1 = std::vector<int64_t>{inShape[0], inShape[TENSOR_DIMENSION_ONE], inShape[srcDimNum - 1]};
                break;
            case HWC_SHAPE_SIZE:
                // {W, C} or {H, C}
                dstShape0 = std::vector<int64_t>{inShape[TENSOR_DIMENSION_ONE], inShape[srcDimNum - 1]};
                dstShape1 = std::vector<int64_t>{inShape[TENSOR_DIMENSION_ZERO], inShape[srcDimNum - 1]};
                break;
            default:
                LogInfo << "CheckOpCustom: Get reduce dst shape abnormal, default value is empty vector.";
                break;
        }

        if (outShape != dstShape0 && outShape != dstShape1) {
            LogError << "CheckOpCustom: Check Reduce op output shape and input shape permutation do not match."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }

    APP_ERROR OpLoaderReduce::OpCreateParamTensor(const std::string inputShape,
                                                  std::string inputDataType, OperatorDesc &opDesc)
    {
        APP_ERROR ret = APP_ERR_OK;
        std::vector<std::string> typeVec1 = StringUtils::Split(inputShape, ',');
        std::vector<std::string> typeVec2 = StringUtils::Split(inputDataType, ';');
        if (typeVec1.size() == 0 || typeVec2.size() == 0) {
            LogError << "OpCreateParamTensor: Reduce Invalid input shape, please check the config file."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        std::vector<std::int64_t> shapeVec = {TENSOR_DIMENSION_ONE};
        ret = opDesc.AddInputTensorDesc(shapeVec, OpDataType::OP_INT32);
        if (ret != APP_ERR_OK) {
            LogError << "OpCreateParamTensor: Reduce AddInputTensorDesc failed." << GetErrorInfo(ret);
        }
        return ret;
    }

    APP_ERROR OpLoaderReduce::CheckOpShape(std::string inputShape, std::string outputShape)
    {
        std::string opType = "Reduce";
        auto inputShapeVecs = GetShapeVecs(inputShape);
        auto outputShapeVecs = GetShapeVecs(outputShape);

        OpParams opParams;
        auto iter = OpLoader::toOpParamsCheckMap.find(opType);
        if (iter != OpLoader::toOpParamsCheckMap.end()) {
            opParams = iter->second;
        } else {
            LogWarn << "CheckOpShape: Op [" << opType <<"] check rule not specified in CheckMap, skip CheckOpParams.";
            return APP_ERR_OK;
        }

        if ((inputShapeVecs.size() == 0) || (outputShapeVecs.size() == 0)) {  // 1. Input/Output size non-zero.
            LogError << "CheckOpShape: Op [" << opType << "] shape should not be empty, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        if ((opParams.inputNum != MUL_INPUT) && (static_cast<int>(inputShapeVecs.size()) != opParams.inputNum)) {
            LogError << "CheckOpShape: Op [" << opType << ": input_shape] input number invalid, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;  // 2. Input num check.
        }
        if ((opParams.outputNum != MUL_OUTPUT) && (static_cast<int>(outputShapeVecs.size()) != opParams.outputNum)) {
            LogError << "CheckOpShape: Op [" << opType << ": output_shape] output number invalid, please check!"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;  // 3.Out num check.
        }
        for (auto inShape : inputShapeVecs) {
            if ((opParams.minDims != UNDEFINED_DIM) && (static_cast<int>(inShape.size()) < opParams.minDims)) {
                LogError << "CheckOpShape: Op [" << opType << ": input_shape] input dim invalid, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;  // 4. In shape valid.
            }
            if ((opParams.maxDims != UNDEFINED_DIM) && (static_cast<int>(inShape.size()) > opParams.maxDims)) {
                LogError << "CheckOpShape: Op [" << opType << ": input_shape] input dim invalid, please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;  // 5. In shape valid.
            }
            if (static_cast<int>(inShape.size()) != HW_DIM &&
                (inShape.back() < ONE_CHANNEL || inShape.back() > FOUR_CHANNEL)) {
                LogError << "CheckOpShape: Input Tensors channel number should be in range [1, 4], please check!"
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }

        return APP_ERR_OK;
    }

    APP_ERROR OpLoaderReduce::OpPreload(const JsonPtr &jsonPtr, size_t index, std::string, OpSettings opSets)
    {
        LogInfo << "OpPreload: preload op [Reduce].";
        auto ret = OpLoader::OpPreload(jsonPtr, index, "ReduceSum", opSets);
        if (ret != APP_ERR_OK) {
            LogError << "OpPreload: Fail to precompile Reduce: ReduceSum." << GetErrorInfo(ret);
            return ret;
        }

        ret = OpLoader::OpPreload(jsonPtr, index, "ReduceMean", opSets);
        if (ret != APP_ERR_OK) {
            LogError << "OpPreload: Fail to precompile Reduce: ReduceMean." << GetErrorInfo(ret);
            return ret;
        }

        ret = OpLoader::OpPreload(jsonPtr, index, "ReduceMax", opSets);
        if (ret != APP_ERR_OK) {
            LogError << "OpPreload: Fail to precompile Reduce: ReduceMax." << GetErrorInfo(ret);
            return ret;
        }

        ret = OpLoader::OpPreload(jsonPtr, index, "ReduceMin", opSets);
        if (ret != APP_ERR_OK) {
            LogError << "OpPreload: Fail to precompile Reduce: ReduceMin." << GetErrorInfo(ret);
            return ret;
        }

        return ret;
    }
}
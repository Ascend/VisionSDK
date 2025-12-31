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
 * Description: Resnet50 model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/ModelPostProcessors/ClassPostProcessors/Resnet50PostProcessor.h"
#include "MxBase/Maths/FastMath.h"

using namespace MxBase;

namespace {
const int MIN_OUTPUT_SIZE = 2;
}

namespace MxBase {
APP_ERROR Resnet50PostProcessor::Init(const std::string& configPath, const std::string& labelPath,
    MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize Resnet50PostProcessor.";
    APP_ERROR ret = LoadConfigDataAndLabelMap(configPath, labelPath);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to superInit in Resnet50PostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    ret = configData_.GetFileValue<int>("CLASS_NUM", classNum_, 0, 0x5DC);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read CLASS_NUM from config, default value(0"
                << ") will be used as classNum_. PostProcessor will not be executed correctly"
                << ", please check your config file";
    }
    ret = configData_.GetFileValue<bool>("SOFTMAX", softmax_);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret) << "Fail to read SOFTMAX from config, default value(false"
                << ") will be used as softmax_, which means softmax will not be executed in model postprocessor.";
    }
    configData_.GetFileValueWarn<uint32_t>("CLASSIFIER_TENSOR", classifierTensor_, 0x0, 0x3e8);

    ret = GetModelTensorsShape(modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to GetModelTensorsShape" << GetErrorInfo(ret);
        return ret;
    }
    if (checkModelFlag_) {
        ret = CheckModelCompatibility();
        if (ret != APP_ERR_OK) {
            LogError << "Fail to CheckModelCompatibility in Resnet50PostProcessor."
                     << "Please check the compatibility between model and postprocessor" << GetErrorInfo(ret);
            return ret;
        }
    } else {
        LogWarn << "Compatibility check for model is skipped as CHECK_MODEL is set as false, please ensure your model "
                << "is correct before running.";
    }
    LogInfo << "End to initialize Resnet50PostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR Resnet50PostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize Resnet50PostProcessor.";
    LogInfo << "End to deinitialize Resnet50PostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR Resnet50PostProcessor::Process(std::vector<std::shared_ptr<void>>& featLayerData, int& argmaxIndex,
    float& confidence)
{
    if (classifierTensor_ >= featLayerData.size()) {
        LogError << "TENSOR(" << classifierTensor_ << ") must be less than tensors'size(" << featLayerData.size()
                 << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_FAILURE;
    }
    LogDebug << "Begin to process Resnet50PostProcessor.";
    if (featLayerData[classifierTensor_] == nullptr) {
        LogError << "Resnet50PostProcessor: featLayerData contains nullptr in Process operation."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    float *castData = static_cast<float *>(featLayerData[classifierTensor_].get());
    std::vector<float> result;
    for (int j = 0; j < classNum_; ++j) {
        result.push_back(castData[j]);
    }
    if (softmax_) {
        fastmath::softmax(result);
    }
    std::vector<float>::iterator maxElement = std::max_element(std::begin(result), std::end(result));
    argmaxIndex = maxElement - std::begin(result);
    confidence = *maxElement;
    LogDebug << "End to process Resnet50PostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Check the compatibility between model and modelPostProcessor.
 * @return APP_ERROR error code.
 */
APP_ERROR Resnet50PostProcessor::CheckModelCompatibility()
{
    if (outputTensorShapes_[0].size() < MIN_OUTPUT_SIZE) {
        LogError << "outputTensorShapes_[0] size: (" << outputTensorShapes_[0].size() << ") "
                 << "is less than required (" << MIN_OUTPUT_SIZE << ") " << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[0][0] != (int)modelDesc_.batchSizes.back()) {
        LogError << "outputTensorShapes_[0][0] != modelDesc_.batchSizes.back()."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[0][1] != classNum_) {
        LogError << "outputTensorShapes_[0][1] != classNum_(" << classNum_
                 << "). Please check the correctness of the model or configFile"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}
}
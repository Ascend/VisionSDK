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
 * Description: Crnn model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/ModelPostProcessors/AttributePostProcessors/CrnnPostProcessor.h"

namespace MxBase {
/*
 * @description Load the labels from the file
 * @param labelPath classification label path
 * @return APP_ERROR error code
 */
APP_ERROR CrnnPostProcessor::Init(const std::string& configPath, const std::string& labelPath,
                                  MxBase::ModelDesc modelDesc)
{
    LogInfo << "Begin to initialize CrnnPostProcessor.";
    APP_ERROR ret = LoadConfigDataAndLabelMap(configPath, labelPath);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to LoadConfigDataAndLabelMap in CrnnPostProcessor." << GetErrorInfo(ret);
        return ret;
    }
    ret = configData_.GetFileValue<int>("CLASS_NUM", classNum_, 0x0, 0x2710);
    if (ret != APP_ERR_OK) {
        LogWarn << "Fail to read CLASS_NUM from config, default value(" << classNum_
                << ") will be used as classNum_." << GetErrorInfo(ret);
    }
    ret = configData_.GetFileValue<int>("OBJECT_NUM", objectNum_, 0x0, 0x3e8);
    if (ret != APP_ERR_OK) {
        LogWarn << "Fail to read OBJECT_NUM from config, default value(" << objectNum_
                << ") will be used as objectNum_." << GetErrorInfo(ret);
    }
    ret = GetModelTensorsShape(modelDesc);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to get model tensors shape" << GetErrorInfo(ret);
        return ret;
    }

    if (checkModelFlag_) {
        ret = CheckModelCompatibility();
        if (ret != APP_ERR_OK) {
            LogError << "Fail to CheckModelCompatibility in CrnnPostProcessor."
                     << "Please check the compatibility between model and postprocessor" << GetErrorInfo(ret);
            return ret;
        }
    } else {
        LogWarn << "Compatibility check for model is skipped as CHECK_MODEL is set as false, please ensure your model"
                << "is correct before running.";
    }
    LogInfo << "End to initialize CrnnPostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Do nothing temporarily.
 * @return APP_ERROR error code.
 */
APP_ERROR CrnnPostProcessor::DeInit()
{
    LogInfo << "Begin to deinitialize CrnnPostProcessor.";
    LogInfo << "End to deinitialize CrnnPostProcessor.";
    return APP_ERR_OK;
}

APP_ERROR CrnnPostProcessor::Process(std::vector<std::shared_ptr<void>>& featLayerData, std::string& resultString)
{
    LogDebug << "Begin to process CrnnPostProcessor.";
    resultString = CrnnDetectionOutput(featLayerData);
    LogDebug << "End to process CrnnPostProcessor.";
    return APP_ERR_OK;
}

/*
 * @description: Get the info of detected object from output and resize to original coordinates.
 * @param featLayerData  Vector of output feature data.
 * @return: string
 */
std::string CrnnPostProcessor::CrnnDetectionOutput(std::vector<std::shared_ptr<void>>& featLayerData)
{
    if (featLayerData.size() < 1 || featLayerData[0].get() == nullptr) {
        LogError << "Invalid featLayerData." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return "";
    }
    long long int *objectInfo = static_cast<long long int *>(featLayerData[0].get());
    long long int x = classNum_;
    std::string result = "";
    for (int i = 0; i < objectNum_; i++) {
        if (objectInfo[i] != classNum_ && objectInfo[i] != x) {
            result += configData_.GetClassName(objectInfo[i]);
        }
        x = objectInfo[i];
    }
    LogDebug << "CrnnPostProcessor output string(" << result << ").";
    return result;
}

APP_ERROR CrnnPostProcessor::CheckModelCompatibility()
{
    if (outputTensorShapes_[0].size() < MIN_SIZE) {
        LogError << "outputTensorShapes_[0] size: (" << outputTensorShapes_[0].size() << ") "
                 << "is less than required (" << MIN_SIZE << ") " << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[0][0] != (int)modelDesc_.batchSizes.back()) {
        LogError << "outputTensorShapes_[0][0] != modelDesc_.batchSizes.back()"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (outputTensorShapes_[0][1] != objectNum_) {
        LogError << "outputTensorShapes_[0][1] < objectNum_ (" << objectNum_ << ")."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}
}
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
 * Description: Implement inference post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/ModelPostProcessors/ModelPostProcessorBase/ModelPostProcessorBase.h"
#include "MxBase/Utils/StringUtils.h"

namespace {
const unsigned int MIN_FILENAME_LENGTH = 5;
}

namespace MxBase {
APP_ERROR ModelPostProcessorBase::Process(std::vector<std::shared_ptr<void>>&)
{
    return APP_ERR_OK;
}

APP_ERROR ModelPostProcessorBase::LoadConfigDataAndLabelMap(const std::string& configPath, const std::string& labelPath)
{
    APP_ERROR ret = APP_ERR_OK;
    nlohmann::json config;
    try {
        config = nlohmann::json::parse(configPath);
        LogDebug << "parse config content from content";
        MxBase::ConfigUtil util;
        ret = util.LoadConfiguration(configPath, configData_, MxBase::CONFIGCONTENT);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to load configuration, config path invalidate." << GetErrorInfo(ret);
            return ret;
        }
    } catch (const std::exception& ex) {
        LogDebug << "parse config content from file";
        if ((labelPath.size() <= MIN_FILENAME_LENGTH) || (configPath.size() <= MIN_FILENAME_LENGTH)) {
            LogError << "Too short path for label or config. Please check." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
            return APP_ERR_COMM_OPEN_FAIL;
        }

        // Open config file
        MxBase::ConfigUtil util;
        ret = util.LoadConfiguration(configPath, configData_, MxBase::CONFIGFILE);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to load configuration, config path invalidate." << GetErrorInfo(ret);
            return ret;
        }
    }

    ret = configData_.LoadLabels(labelPath);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to load label file." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR ModelPostProcessorBase::MemoryDataToHost(const int index,
    const std::vector<std::vector<MxBase::BaseTensor>>& tensors, std::vector<std::shared_ptr<void>>& featLayerData)
{
    if (index < 0 || index >= (int)tensors.size()) {
        LogError << "Index(" << index << ") should be less than tensors' size("
                 << tensors.size() << "), and not less than 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (const MxBase::BaseTensor &tensor : tensors[index]) {
        MxBase::MemoryData memorySrc;
        memorySrc.size = tensor.size;
        memorySrc.ptrData = tensor.buf;
        memorySrc.type = MxBase::MemoryData::MEMORY_DEVICE;
        MxBase::MemoryData memoryDst;
        memoryDst.size = tensor.size;
        memoryDst.type = MxBase::MemoryData::MEMORY_HOST_MALLOC;
        APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(memoryDst, memorySrc);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to copy device memory to host for ModelPostProcessor." << GetErrorInfo(ret);
            return ret;
        }
        std::shared_ptr<void> buffer = nullptr;
        buffer.reset((float*) memoryDst.ptrData, memoryDst.free);
        featLayerData.emplace_back(buffer);
    }
    return APP_ERR_OK;
}

APP_ERROR ModelPostProcessorBase::GetModelTensorsShape(MxBase::ModelDesc modelDesc)
{
    for (size_t j = 0; j < modelDesc.outputTensors.size(); j++) {
        if (MxBase::StringUtils::HasInvalidChar(modelDesc.outputTensors[j].tensorName)) {
            LogError << "TensorName has invalid char." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        } else {
            LogInfo << "Shape of output tensor: " << j << " (name: " << modelDesc.outputTensors[j].tensorName
                    << ") of model is as follow: ";
        }
        std::vector<int> outputTensorShape = {};
        for (size_t m = 0; m < modelDesc.outputTensors[j].tensorDims.size(); m++) {
            LogInfo << "   dim " << m << ": " << modelDesc.outputTensors[j].tensorDims[m];
            outputTensorShape.push_back(modelDesc.outputTensors[j].tensorDims[m]);
        }
        outputTensorShapes_.push_back(outputTensorShape);
    }

    for (size_t j = 0; j < modelDesc.inputTensors.size(); j++) {
        LogInfo << "Shape of input tensor: " << j << " (name: " << modelDesc.inputTensors[j].tensorName
                << ") of model is as follow: ";
        std::vector<int> inputTensorShape = {};
        for (size_t m = 0; m < modelDesc.inputTensors[j].tensorDims.size(); m++) {
            LogInfo << "   dim " << m << ": " << modelDesc.inputTensors[j].tensorDims[m];
            inputTensorShape.push_back(modelDesc.inputTensors[j].tensorDims[m]);
        }
        inputTensorShapes_.push_back(inputTensorShape);
    }
    modelDesc_ = modelDesc;

    if (inputTensorShapes_.empty() || outputTensorShapes_.empty()) {
        LogError << "Failed to get outputTensorShapes or inputTensorShapes."
                 << "InputTensor or outputTensor of struct ModelDesc is empty"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return APP_ERR_OK;
}

std::string ModelPostProcessorBase::GetLabelName(int index)
{
    return configData_.GetClassName(index);
}
}  // namespace MxBase
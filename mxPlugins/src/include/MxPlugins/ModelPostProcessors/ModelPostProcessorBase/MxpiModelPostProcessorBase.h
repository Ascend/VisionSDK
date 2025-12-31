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
 * Description: Inference postprocessing base class.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MODEL_POSTPROCESS_BASE_H
#define MODEL_POSTPROCESS_BASE_H

#include <map>
#include <vector>
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/ConfigUtil/ConfigUtil.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

namespace {
unsigned int MIN_FILENAME_LENGTH = 5;
uint32_t VERSION = 3001;
}

namespace MxPlugins {
class MxpiModelPostProcessorBase {
public:
    virtual ~MxpiModelPostProcessorBase() = default;
    /*
    * @description: Load the configs and labels from the file.
    * @param: labelPath config path and label path.
    * @return: APP_ERROR error code.
    */
    virtual APP_ERROR Init(const std::string& configPath, const std::string& labelPath,
        MxBase::ModelDesc modelDesc) = 0;

    /*
    * @description: Do nothing temporarily.
    * @return: APP_ERROR error code.
    */
    virtual APP_ERROR DeInit() = 0;

    /*
    * @description: Postprocess of object detection.
    * @param: metaDataPtr Pointer of metadata.
    * @param: useMpPictureCrop  Flag whether use crop before modelInfer.
    * @param: postImageInfoVec  Width and height of model/image.
    * @param: headerVec  header of image in same buffer.
    * @param: tensors  Output tensors of modelInfer.
    * @return: APP_ERROR error code.
    */
    virtual APP_ERROR Process(std::shared_ptr<void>& metaDataPtr, MxBase::PostProcessorImageInfo postProcessorImageInfo,
        std::vector<MxTools::MxpiMetaHeader>& headerVec, std::vector<std::vector<MxBase::BaseTensor>>& tensors) = 0;

    /*
    * @description: set output tensor shapes.
    */
    void SetOutputTensorShapes(const std::vector<MxBase::TensorDesc>& outputTensors);

    /*
    * @description: set post processor image information.
    */
    virtual void SetAspectRatioImageInfo(
            const std::vector<MxBase::AspectRatioPostImageInfo>&) {};
protected:
    /*
     * @description: Load the configs and labels from the file.
     * @param: labelPath config path and label path.
     * @return: APP_ERROR error code.
     */
    APP_ERROR LoadConfigDataAndLabelMap(const std::string& configPath, const std::string& labelPath);

    /*
     * @description: Copy data from device to host for postprocessor.
     * @param: index  Index of the output pool.
     * @param: tensors  Output tensors.
     * @param: featLayerData  Vector of output feature data.
     * @return: APP_ERROR  Error code.
     */
    APP_ERROR MemoryDataToHost(const int index, const std::vector<std::vector<MxBase::BaseTensor>>& tensors,
                               std::vector<std::shared_ptr<void>>& featLayerData);

    void GetModelTensorsShape(MxBase::ModelDesc modelDesc);

    APP_ERROR IsDescValid(const google::protobuf::Descriptor* desc, std::string elementName);

protected:
    bool checkModelFlag_ = true;
    MxBase::ConfigData configData_;
    MxBase::ModelDesc modelDesc_;
    std::vector<std::vector<int>> outputTensorShapes_;
    std::vector<std::vector<int>> inputTensorShapes_;
};

APP_ERROR MxpiModelPostProcessorBase::LoadConfigDataAndLabelMap(const std::string& configPath,
    const std::string& labelPath)
{
    if ((labelPath.size() <= MIN_FILENAME_LENGTH) || (configPath.size() <= MIN_FILENAME_LENGTH)) {
        LogError << "Too short path for label or config. Please check." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }

    // Open config file
    MxBase::ConfigUtil util;
    APP_ERROR ret = util.LoadConfiguration(configPath, configData_, MxBase::CONFIGFILE);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to load configuration, config path invalidate." << GetErrorInfo(ret);
        return ret;
    }
    configData_.GetFileValueWarn<bool>("CHECK_MODEL", checkModelFlag_);

    ret = configData_.LoadLabels(labelPath);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to load label file." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

void MxpiModelPostProcessorBase::GetModelTensorsShape(MxBase::ModelDesc modelDesc)
{
    for (size_t j = 0; j < modelDesc.outputTensors.size(); j++) {
        LogInfo << "Shape of outputTensors[" << j << "] of model is as follow: ";
        std::vector<int> outputTensorShape = {};
        for (size_t m = 0; m < modelDesc.outputTensors[j].tensorDims.size(); m++) {
            LogInfo << "   dim " << m << ": " << modelDesc.outputTensors[j].tensorDims[m];
            outputTensorShape.push_back(modelDesc.outputTensors[j].tensorDims[m]);
        }
        outputTensorShapes_.push_back(outputTensorShape);
    }

    for (size_t j = 0; j < modelDesc.inputTensors.size(); j++) {
        LogInfo << "Shape of inputTensors[" << j << "] of model is as follow: ";
        std::vector<int> inputTensorShape = {};
        for (size_t m = 0; m < modelDesc.inputTensors[j].tensorDims.size(); m++) {
            LogInfo << "   dim " << m << ": " << modelDesc.inputTensors[j].tensorDims[m];
            inputTensorShape.push_back(modelDesc.inputTensors[j].tensorDims[m]);
        }
        inputTensorShapes_.push_back(inputTensorShape);
    }
    modelDesc_ = modelDesc;
}

APP_ERROR MxpiModelPostProcessorBase::MemoryDataToHost(const int index,
    const std::vector<std::vector<MxBase::BaseTensor>>& tensors, std::vector<std::shared_ptr<void>>& featLayerData)
{
    int tensorSize = static_cast<int>(tensors.size());
    if (tensorSize <= index) {
        LogError << "Tensors size do not match index." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    for (const MxBase::BaseTensor &tensor : tensors[index]) {
        MxBase::MemoryData memorySrc;
        memorySrc.size = tensor.size;
        memorySrc.ptrData = (void *)tensor.buf;
        memorySrc.type = MxBase::MemoryData::MEMORY_DEVICE;
        MxBase::MemoryData memoryDst;
        memoryDst.size = tensor.size;
        memoryDst.type = MxBase::MemoryData::MEMORY_HOST;
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

void MxpiModelPostProcessorBase::SetOutputTensorShapes(const std::vector<MxBase::TensorDesc>& outputTensors)
{
    outputTensorShapes_.clear();
    for (size_t j = 0; j < outputTensors.size(); j++) {
        LogDebug << "Shape of outputTensors[" << j << "] of model is as follow: ";
        std::vector<int> outputTensorShape = {};
        for (size_t m = 0; m < outputTensors[j].tensorDims.size(); m++) {
            LogDebug << "   dim " << m << ": " << outputTensors[j].tensorDims[m];
            outputTensorShape.push_back(outputTensors[j].tensorDims[m]);
        }
        outputTensorShapes_.push_back(outputTensorShape);
    }
}

APP_ERROR MxpiModelPostProcessorBase::IsDescValid(const google::protobuf::Descriptor* desc, std::string elementName)
{
    if (!desc) {
        LogError << "Get input data's descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM; // self define the error code
    }
    if (desc->name() != elementName) {
        LogError << "The type of metadata is not matched to designed."
                 << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    return APP_ERR_OK;
}

using GetInstanceFunc = std::shared_ptr<MxpiModelPostProcessorBase>(*)();
using GetVersionFunc = uint32_t(*)();
}  // namespace MxBase
extern "C" {
    uint32_t GetCurVersion()
    {
        return VERSION;
    }
}
#endif  // MODEL_POSTPROCESS_BASE_H

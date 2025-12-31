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

#ifndef MODEL_POSTPROCESSOR_H
#define MODEL_POSTPROCESSOR_H

#include <map>
#include <vector>
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/ConfigUtil/ConfigUtil.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"

namespace MxBase {
class ModelPostProcessorBase {
public:
    virtual ~ModelPostProcessorBase() = default;
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
     * @description: Get the index and confidence of the most possible class.
     * @param featLayerData  Vector of output feature data.
     * @param argmaxIndex  index of the most possible class.
     * @param confidence  confidence of the most possible class.
     * @return: ErrorCode.
     */
    virtual APP_ERROR Process(std::vector<std::shared_ptr<void>>& featLayerData);

    /*
     * @description: Copy data from device to host for postprocessor.
     * @param: index  Index of the output pool.
     * @param: tensors  Output tensors.
     * @param: featLayerData  Vector of output feature data.
     * @return: APP_ERROR  Error code.
     */
    APP_ERROR MemoryDataToHost(const int index, const std::vector<std::vector<MxBase::BaseTensor>>& tensors,
                               std::vector<std::shared_ptr<void>>& featLayerData);

    /*
     * @description: Get class name by index.
     * @param: int   index;
     * @return: string   class name.
     */
    std::string GetLabelName(int index);

protected:
    /*
     * @description: Load the configs and labels from the file.
     * @param: labelPath config path and label path.
     * @return: APP_ERROR error code.
     */
    APP_ERROR LoadConfigDataAndLabelMap(const std::string& configPath, const std::string& labelPath);

    APP_ERROR GetModelTensorsShape(MxBase::ModelDesc modelDesc);

    APP_ERROR CheckModelCompatibility();

protected:
    bool checkModelFlag_ = true;
    MxBase::ConfigData configData_;
    MxBase::ModelDesc modelDesc_;
    std::vector<std::vector<int>> outputTensorShapes_;
    std::vector<std::vector<int>> inputTensorShapes_;
};

using GetInstanceFunc = std::shared_ptr<ModelPostProcessorBase>(*)();
}  // namespace MxBase
#endif  // MODEL_POSTPROCESSOR_H

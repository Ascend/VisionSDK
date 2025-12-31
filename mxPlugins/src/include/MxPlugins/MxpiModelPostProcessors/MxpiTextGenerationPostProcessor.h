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
 * Description: The output tensor of text generation model inference is post-processed.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPLUGINS_MXPITEXTGENERATIONPOSTPROCESSOR_H
#define MXPLUGINS_MXPITEXTGENERATIONPOSTPROCESSOR_H
#include <atomic>
#include <tuple>

#include "MxBase/PostProcessBases/TextGenerationPostProcessBase.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"
#include "MxBase/PostProcessBases/PostProcessDataType.h"
#include "MxTools/PluginToolkit/PostProcessPluginBases/MxModelPostProcessorBase.h"

/**
 * This plugin is used for loading text generation modelPostProcessor and write results in metadata.
 */
namespace MxPlugins {
class MxpiTextGenerationPostProcessor : public MxTools::MxModelPostProcessorBase {
public:
    /**
    * @description: Init configs, get postProcess instance.
    * @param configParamMap: config.
    * @return: Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap) override;

    /**
    * @description: DeInit postProcess instance.
    * @return: Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @description: MxpiTextGenerationPostProcessor plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

    /**
    * @description: MxpiTextGenerationPostProcessor plugin define properties.
    * @return: properties.
    */
    static std::vector<std::shared_ptr<void>> DefineProperties();

    /**
    * @api
    * @brief Define the number and data type of input ports.
    * @return MxTools::MxpiPortInfo.
    */
    static MxTools::MxpiPortInfo DefineInputPorts();

    /**
    * @api
    * @brief Define the number and data type of output ports.
    * @return MxTools::MxpiPortInfo.
    */
    static MxTools::MxpiPortInfo DefineOutputPorts();
};
}
#endif // MXPLUGINS_MXPITEXTGENERATIONPOSTPROCESSOR
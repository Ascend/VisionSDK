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

#ifndef MXBASE_RESNET50POSTPROCESSOR_H
#define MXBASE_RESNET50POSTPROCESSOR_H

#include "MxBase/ModelPostProcessors/ModelPostProcessorBase/ModelPostProcessorBase.h"

namespace MxBase {
class SDK_DEPRECATED_FOR() Resnet50PostProcessor : public MxBase::ModelPostProcessorBase {
public:
    /*
     * @description Load the configs and labels from the file.
     * @param labelPath config path and label path.
     * @return APP_ERROR error code.
     */
    APP_ERROR Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) override;

    /*
     * @description: Do nothing temporarily.
     * @return APP_ERROR error code.
     */
    APP_ERROR DeInit();

    /*
     * @description: Get the index and confidence of the most possible class.
     * @param featLayerData  Vector of output feature data.
     * @param argmaxIndex  index of the most possible class.
     * @param confidence  confidence of the most possible class.
     * @return: ErrorCode.
     */
    APP_ERROR Process(std::vector<std::shared_ptr<void>>& featLayerData, int& argmaxIndex, float& confidence);

private:
    APP_ERROR CheckModelCompatibility();

    int classNum_ = 0;

    bool softmax_ = false;

    uint32_t classifierTensor_ = 0;
};
}

#endif // MXBASE_RESNET50POSTPROCESSOR_H
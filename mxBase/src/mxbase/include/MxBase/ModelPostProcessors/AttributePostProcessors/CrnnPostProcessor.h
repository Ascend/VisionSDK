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

#ifndef MXBASE_CRNNPOSTPROCESSOR_H
#define MXBASE_CRNNPOSTPROCESSOR_H

#include "MxBase/ModelPostProcessors/ModelPostProcessorBase/ModelPostProcessorBase.h"

namespace {
const float DEFAULT_CLASS_NUM = 36;
const float DEFAULT_OBJECT_NUM = 32;
const int MIN_SIZE = 2;
}

namespace MxBase {
class SDK_DEPRECATED_FOR() CrnnPostProcessor : public MxBase::ModelPostProcessorBase {
public:
    APP_ERROR Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) override;
    APP_ERROR DeInit();
    APP_ERROR Process(std::vector<std::shared_ptr<void>>& featLayerData, std::string& resultString);

private:
    APP_ERROR CheckModelCompatibility();
    std::string CrnnDetectionOutput(std::vector<std::shared_ptr<void>>& featLayerData);
private:
    int classNum_ = DEFAULT_CLASS_NUM;
    // length of model output.
    int objectNum_ = DEFAULT_OBJECT_NUM;
};
}

#endif // MXBASE_CRNNPOSTPROCESSOR_H
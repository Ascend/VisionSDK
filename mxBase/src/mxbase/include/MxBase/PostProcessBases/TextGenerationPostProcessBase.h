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
 * Description: Accepts the model inference output tensor and outputs the text generation result.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef TEXTGENERATION_POST_PROCESS_H
#define TEXTGENERATION_POST_PROCESS_H
#include "MxBase/PostProcessBases/PostProcessBase.h"

namespace MxBase {
class TextGenerationPostProcessBase : public PostProcessBase {
public:
    TextGenerationPostProcessBase() = default;

    TextGenerationPostProcessBase(const TextGenerationPostProcessBase &other) = default;

    virtual ~TextGenerationPostProcessBase() = default;

    TextGenerationPostProcessBase& operator=(const TextGenerationPostProcessBase &other);

    APP_ERROR Init(const std::map<std::string, std::string> &postConfig) override;

    APP_ERROR DeInit() override;

    virtual APP_ERROR Process(const std::vector<TensorBase>& tensors,
                              std::vector<TextsInfo>& textsInfos,
                              const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});

protected:
    uint32_t classNum_ = 0;
};

using GetTextGenerationInstanceFunc = std::shared_ptr<TextGenerationPostProcessBase>(*)();
}


#endif
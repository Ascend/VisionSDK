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
 * Description: Transformer model post-processing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef TRANSFORMER_POST_PROCESS_H
#define TRANSFORMER_POST_PROCESS_H
#include "MxBase/PostProcessBases/TextGenerationPostProcessBase.h"
#include "MxBase/Common/HiddenAttr.h"

namespace MxBase {
class TransformerPostProcessDptr;
class SDK_AVAILABLE_FOR_OUT TransformerPostProcess : public TextGenerationPostProcessBase {
public:
    TransformerPostProcess();

    ~TransformerPostProcess() = default;

    TransformerPostProcess(const TransformerPostProcess &other);

    APP_ERROR Init(const std::map<std::string, std::string> &postConfig) override;

    APP_ERROR DeInit() override;

    APP_ERROR Process(const std::vector<TensorBase>& tensors, std::vector<TextsInfo>& translationInfos,
                      const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});

    TransformerPostProcess &operator=(const TransformerPostProcess &other);

    uint64_t GetCurrentVersion() override;

private:
    friend class TransformerPostProcessDptr;
    std::shared_ptr<MxBase::TransformerPostProcessDptr> dPtr_;
    APP_ERROR CheckDptr();
};
#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
    std::shared_ptr<MxBase::TransformerPostProcess> GetTextGenerationInstance();
}
#endif
}
#endif
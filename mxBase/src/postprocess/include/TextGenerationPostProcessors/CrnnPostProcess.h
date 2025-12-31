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
 * Create: 2021
 * History: NA
 */

#ifndef CRNN_POST_PROCESS_H
#define CRNN_POST_PROCESS_H
#include "MxBase/PostProcessBases/TextGenerationPostProcessBase.h"
#include "MxBase/Common/HiddenAttr.h"

namespace MxBase {
class CrnnPostProcessDptr;
class SDK_AVAILABLE_FOR_OUT CrnnPostProcess : public TextGenerationPostProcessBase {
public:
    CrnnPostProcess();

    ~CrnnPostProcess() = default;

    CrnnPostProcess(const CrnnPostProcess &other);

    APP_ERROR Init(const std::map<std::string, std::string> &postConfig) override;

    APP_ERROR DeInit() override;

    APP_ERROR Process(const std::vector<TensorBase>& tensors, std::vector<TextsInfo>& translationInfos,
                      const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});

    CrnnPostProcess &operator=(const CrnnPostProcess &other);

    uint64_t GetCurrentVersion() override;
private:
    friend class CrnnPostProcessDptr;
    APP_ERROR CheckDptr();
    std::shared_ptr<MxBase::CrnnPostProcessDptr> dPtr_;
};

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::CrnnPostProcess> GetTextGenerationInstance();
}
#endif
}
#endif
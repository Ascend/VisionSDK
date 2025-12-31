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
 * Create: 2021
 * History: NA
 */

#ifndef RESNET50_POST_PROCESS_H
#define RESNET50_POST_PROCESS_H
#include "MxBase/PostProcessBases/ClassPostProcessBase.h"
#include "MxBase/Common/HiddenAttr.h"

namespace MxBase {
constexpr uint32_t MAX_BATCH_SIZE = 2048;
class Resnet50PostProcessDptr;
class SDK_AVAILABLE_FOR_OUT Resnet50PostProcess : public ClassPostProcessBase {
public:
    Resnet50PostProcess();

    ~Resnet50PostProcess() = default;

    Resnet50PostProcess(const Resnet50PostProcess &other);

    APP_ERROR Init(const std::map<std::string, std::string> &postConfig) override;

    APP_ERROR DeInit() override;

    APP_ERROR Process(const std::vector<TensorBase> &tensors, std::vector<std::vector<ClassInfo>> &classInfos,
                      const std::map<std::string, std::shared_ptr<void>> &configParamMap = {}) override;

    Resnet50PostProcess &operator=(const Resnet50PostProcess &other);

    uint64_t GetCurrentVersion() override;

private:
    friend class Resnet50PostProcessDptr;
    APP_ERROR CheckDptr();
    std::shared_ptr<MxBase::Resnet50PostProcessDptr> dPtr_;
};

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::Resnet50PostProcess> GetClassInstance();
}
#endif
}
#endif
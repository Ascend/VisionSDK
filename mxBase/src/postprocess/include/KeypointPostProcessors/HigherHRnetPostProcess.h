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
 * Description: HigherHRnet model post-processing.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef HIGHER_HRNET_POST_PROCESS_H
#define HIGHER_HRNET_POST_PROCESS_H

#include "MxBase/Common/HiddenAttr.h"
#include "MxBase/PostProcessBases/KeypointPostProcessBase.h"


namespace MxBase {
class HigherHRnetPostProcessDptr;
class SDK_AVAILABLE_FOR_OUT HigherHRnetPostProcess : public KeypointPostProcessBase {
public:
    HigherHRnetPostProcess();

    ~HigherHRnetPostProcess() = default;

    HigherHRnetPostProcess(const HigherHRnetPostProcess &other);

    HigherHRnetPostProcess &operator=(const HigherHRnetPostProcess &other);

    APP_ERROR Init(const std::map<std::string, std::string> &postConfig) override;

    APP_ERROR DeInit() override;

    APP_ERROR Process(const std::vector<TensorBase>& tensors,
                      std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos,
                      const std::vector<ResizedImageInfo>& resizedImageInfos,
                      const std::map<std::string, std::shared_ptr<void>> &paramMap = {}) override;

    uint64_t GetCurrentVersion() override;

private:
    friend class HigherHRnetPostProcessDptr;
    APP_ERROR CheckDptr();
    std::shared_ptr<MxBase::HigherHRnetPostProcessDptr> dPtr_;
    
};

#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::HigherHRnetPostProcess> GetKeypointInstance();
}
#endif
}
#endif
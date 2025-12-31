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
 * Description: Deeplabv3 model post-processing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef DEEPLAB_V3_POST_H
#define DEEPLAB_V3_POST_H
#include "MxBase/PostProcessBases/SemanticSegPostProcessBase.h"
#include "MxBase/Common/HiddenAttr.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

namespace MxBase {
class Deeplabv3PostDptr;
class SDK_AVAILABLE_FOR_OUT Deeplabv3Post : public SemanticSegPostProcessBase {
public:
    Deeplabv3Post();

    ~Deeplabv3Post();

    Deeplabv3Post(const Deeplabv3Post &other);

    APP_ERROR Init(const std::map<std::string, std::string> &postConfig) override;

    APP_ERROR DeInit() override;

    APP_ERROR Process(const std::vector<TensorBase> &tensors, std::vector<SemanticSegInfo> &semanticSegInfos,
        const std::vector<ResizedImageInfo> &resizedImageInfos = {},
        const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});

    Deeplabv3Post &operator=(const Deeplabv3Post &other);

    uint64_t GetCurrentVersion() override;
private:
    friend class Deeplabv3PostDptr;
    APP_ERROR CheckDptr();
    std::shared_ptr<MxBase::Deeplabv3PostDptr> dPtr_;
};
#ifdef ENABLE_POST_PROCESS_INSTANCE
extern "C" {
std::shared_ptr<MxBase::Deeplabv3Post> GetSemanticSegInstance();
}
#endif
}
#endif
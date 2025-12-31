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
 * Description: Classification detection post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef CLASS_POST_PROCESS_H
#define CLASS_POST_PROCESS_H
#include "MxBase/PostProcessBases/PostProcessBase.h"

namespace MxBase {
class ClassPostProcessBase : public PostProcessBase {
public:
    ClassPostProcessBase() = default;

    ClassPostProcessBase(const ClassPostProcessBase &other) = default;

    virtual ~ClassPostProcessBase() = default;

    ClassPostProcessBase& operator=(const ClassPostProcessBase &other);

    APP_ERROR Init(const std::map<std::string, std::string> &postConfig) override;

    APP_ERROR DeInit() override;
    
    APP_ERROR virtual Process(const std::vector<TensorBase>& tensors, std::vector<std::vector<ClassInfo>>& classInfos,
                              const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});

protected:
    uint32_t classNum_ = 0;
    uint32_t topK_ = 1;
};

using GetClassInstanceFunc = std::shared_ptr<ClassPostProcessBase>(*)();
}


#endif
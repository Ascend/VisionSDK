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
 * Description: Accepts the model inference output tensor and outputs the target detection result.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef TEXTDETECTPOSTPROCESSBASE_H
#define TEXTDETECTPOSTPROCESSBASE_H

#include "MxBase/PostProcessBases/ImagePostProcessBase.h"

namespace MxBase {
class TextObjectPostProcessBase : public ImagePostProcessBase {
public:
    TextObjectPostProcessBase() = default;

    TextObjectPostProcessBase(const TextObjectPostProcessBase &other) = default;

    virtual ~TextObjectPostProcessBase() = default;

    TextObjectPostProcessBase &operator = (const TextObjectPostProcessBase &other);

    APP_ERROR Init(const std::map<std::string, std::string> &postConfig) override;

    APP_ERROR DeInit() override;

    virtual APP_ERROR Process(const std::vector<TensorBase> &tensors,
        std::vector<std::vector<TextObjectInfo>> &textObjectInfos,
        const std::vector<ResizedImageInfo> &resizedImageInfos = {},
        const std::map<std::string, std::shared_ptr<void>> &configParamMap = {});

protected:
    void ResizeReduction(const ResizedImageInfo &resizedImageInfo, TextObjectInfo &textObjInfo);

    void FixCoords(uint32_t scrData, float &desData);

protected:
    bool checkModelFlag_ = true;
};
using GetTextObjectInstanceFunc = std::shared_ptr<TextObjectPostProcessBase>(*)();
}

#endif // TEXTDETECTPOSTPROCESSBASE_H

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
 * Description: Resize Param Config.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_RESIZECONFIG_H
#define MXBASE_RESIZECONFIG_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/Resize/ResizeChecker.h"
#include "ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h"
#include "ConfigUtils.h"

namespace MxBase {
class ResizeParamConfig {
public:
    ResizeParamConfig()
    {
        checkPtr_ = std::make_shared<ResizeChecker>();
    }
    virtual ~ResizeParamConfig() = default;
    virtual APP_ERROR ResizeCheck(const DvppDataInfo& inputData, const ResizeConfig& resizeConfig) = 0;
    virtual APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize) = 0;
public:
    HeightAndWidthConstrainInfo resizeScaleConstrain_ = { .wMin = 32, .wMax = 4096, .hMin = 6, .hMax = 4096 };
    float ratioMin_ = 1.0 / 32.f;
    float ratioMax_ = 16.f;
    uint32_t inputHeightAlign_ = 2;
    uint32_t inputWidthAlign_ = 2;
    uint32_t resizeAreaHeightAlign_ = 2;
    uint32_t resizeAreaWidthAlign_ = 2;
    HeightAndWidthConstrainInfo strideShapeConstrain_ = { .wMin = 32, .wMax = 16384, .hMin = 6, .hMax = 16384 };
public:
    std::shared_ptr<ResizeChecker> checkPtr_;
};
}
#endif

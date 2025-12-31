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
 * Description: Padding Param Config.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef MXBASE_PADDINGCONFIG_H
#define MXBASE_PADDINGCONFIG_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/Padding/PaddingChecker.h"
#include "ConfigUtils.h"

namespace MxBase {
class PaddingConfig {
public:
    PaddingConfig()
    {
        checkPtr_ = std::make_shared<PaddingChecker>();
    }
    virtual ~PaddingConfig() = default;
    virtual APP_ERROR PaddingCheck(const DvppDataInfo& inputData, const MakeBorderConfig& borderConfig) = 0;
    virtual APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize) = 0;
public:
    std::shared_ptr<PaddingChecker> checkPtr_;
public:
    uint32_t colorMin_ = 0;
    uint32_t colorMax_ = 255;
    HeightAndWidthConstrainInfo inputShapeConstrain_ = {.wMin = 18, .wMax = 4096, .hMin = 6, .hMax = 4096};
    HeightAndWidthConstrainInfo outputShapeConstrain_ = {.wMin = 18, .wMax = 4096, .hMin = 6, .hMax = 4096};
    HeightAndWidthConstrainInfo strideShapeConstrain_ = {.wMin = 32, .wMax = 16384, .hMin = 6, .hMax = 16384};
    std::vector<BorderType> borderTypeRange_ = {BorderType::BORDER_CONSTANT};
};
}
#endif

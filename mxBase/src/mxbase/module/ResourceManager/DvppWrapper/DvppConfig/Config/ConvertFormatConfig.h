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
 * Description: ConvertFormat Param Config.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_CONVERTFORMATCONFIG_H
#define MXBASE_CONVERTFORMATCONFIG_H
#include "MxBase/E2eInfer/DataType.h"
#include "ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/ConvertFormat/ConvertFormatChecker.h"
#include "ConfigUtils.h"

namespace MxBase {
class ConvertFormatConfig {
public:
    HeightAndWidthConstrainInfo inputShapeConstrain_ = { .wMin = 32, .wMax = 4096, .hMin = 6, .hMax = 4096 };
    HeightAndWidthConstrainInfo strideShapeConstrain_ = { .wMin = 32, .wMax = 16384, .hMin = 6, .hMax = 16384 };
public:
    std::shared_ptr<ConvertFormatChecker> checkPtr_;
public:
    virtual ~ConvertFormatConfig() = default;
    ConvertFormatConfig()
    {
        checkPtr_ = std::make_shared<ConvertFormatChecker>();
    }
    virtual APP_ERROR ConvertFormatCheck(const DvppDataInfo& inputInfo, const DvppDataInfo& outputInfo) = 0;
    virtual APP_ERROR GetOutputDataSize(DvppDataInfo& outputData, uint32_t& outputDataSize) = 0;
};
}
#endif

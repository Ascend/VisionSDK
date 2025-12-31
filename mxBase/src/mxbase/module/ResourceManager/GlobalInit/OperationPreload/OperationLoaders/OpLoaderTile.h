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
 * Description: Load Tile operation by given json config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#ifndef MXBASE_OPLOADERTILE_H
#define MXBASE_OPLOADERTILE_H

#include "OpLoader.h"

namespace MxBase {
    class OpLoaderTile : public OpLoader {
    public:
        OpLoaderTile();

        explicit OpLoaderTile(std::string opType);

        APP_ERROR CheckOpCustom(std::string inputShape, std::string outputShape);

        APP_ERROR OpCreateParamTensor(const std::string inputShape, std::string inputDataType, OperatorDesc &opDesc);
    };
}

#endif

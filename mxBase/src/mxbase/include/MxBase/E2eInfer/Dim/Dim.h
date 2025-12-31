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
 * Description: Constructing Rect Class.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef MX_DIM_H
#define MX_DIM_H

namespace MxBase {
    struct Dim {
        Dim() : left(0), right(0), top(0), bottom(0) {};

        Dim(const uint32_t inputDim) : left(inputDim), right(inputDim), top(inputDim), bottom(inputDim) {};

        Dim(const uint32_t inputLeft, const uint32_t inputRight, const uint32_t inputTop, const uint32_t inputBottom)
            : left(inputLeft), right(inputRight), top(inputTop), bottom(inputBottom) {};
        uint32_t left;
        uint32_t right;
        uint32_t top;
        uint32_t bottom;
    };
}

#endif

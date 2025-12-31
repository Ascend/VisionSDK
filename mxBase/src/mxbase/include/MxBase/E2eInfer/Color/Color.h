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

#ifndef MX_COLOR_H
#define MX_COLOR_H

namespace MxBase {
    struct Color {
        Color() : channel_zero(0), channel_one(0), channel_two(0) {};

        Color(const uint32_t inputRed, const uint32_t inputGreen, const uint32_t inputBlue)
            : channel_zero(inputRed), channel_one(inputGreen), channel_two(inputBlue) {};
        uint32_t channel_zero;
        uint32_t channel_one;
        uint32_t channel_two;
    };
}

#endif

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
 * Description: Data type related function processing.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include <cstdint>
#include "MxBase/Utils/DataTypeUtils.h"

namespace MxBase {

void DataTypeUtils::Float32ToFloat16(uint16_t *__restrict out, float &in)
{
    uint32_t inu = *((uint32_t *)(&in));
    uint32_t value;
    uint32_t flag;
    uint32_t exponent;

    value = inu & 0x7fffffffu;
    flag = inu & 0x80000000u;
    exponent = inu & 0x7f800000u;

    value >>= 13u;
    flag >>= 16u;

    value -= 0x1c000;

    value = (exponent < 0x38800000u) ? 0 : value;
    value = (exponent > 0x8e000000u) ? 0x7bff : value;
    value = (exponent == 0 ? 0 : value);

    value |= flag;

    *((uint16_t *) out) = value;
}
}

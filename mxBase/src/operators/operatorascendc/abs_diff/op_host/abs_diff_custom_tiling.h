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
 * Description: absdiff operator host file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef ABS_DIFF_CUSTOM_TILING_H
#define ABS_DIFF_CUSTOM_TILING_H

#include "register/tilingdata_base.h"

namespace optiling {
    BEGIN_TILING_DATA_DEF(TilingDataAbsDiff)
        TILING_DATA_FIELD_DEF(uint32_t, size);
    END_TILING_DATA_DEF;

    REGISTER_TILING_DATA_CLASS(AbsDiffCustom, TilingDataAbsDiff)
}
#endif
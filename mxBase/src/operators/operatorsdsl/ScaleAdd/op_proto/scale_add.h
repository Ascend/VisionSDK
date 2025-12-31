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
 * Description: Header of ScaleAdd op.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#ifndef GE_OP_SCALE_ADD_H
#define GE_OP_SCALE_ADD_H
#include "graph/operator_reg.h"
namespace ge {

REG_OP(ScaleAdd)
    .INPUT(x1, TensorType({DT_UINT8, DT_FLOAT16, DT_FLOAT32}))
    .INPUT(x2, TensorType({DT_UINT8, DT_FLOAT16, DT_FLOAT32}))
    .OUTPUT(y, TensorType({DT_UINT8, DT_FLOAT16, DT_FLOAT32}))
    .REQUIRED_ATTR(scale, Float)
    .OP_END_FACTORY_REG(ScaleAdd)
}
#endif // GE_OP_SCALE_ADD_H

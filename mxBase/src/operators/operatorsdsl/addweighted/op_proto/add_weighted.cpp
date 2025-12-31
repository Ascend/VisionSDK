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
 * Description: Operator AddWeighted Proto File.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "add_weighted.h"
namespace ge {

IMPLEMT_COMMON_INFERFUNC(AddWeightedInferShape)
{
    TensorDesc tensordesc_output = op.GetOutputDescByName("y");

    tensordesc_output.SetShape(op.GetInputDescByName("x1").GetShape());
    tensordesc_output.SetDataType(op.GetInputDescByName("x1").GetDataType());
    tensordesc_output.SetFormat(op.GetInputDescByName("x1").GetFormat());
    (void)op.UpdateOutputDesc("y", tensordesc_output);
    return GRAPH_SUCCESS;
}

IMPLEMT_VERIFIER(AddWeighted, AddWeightedVerify)
{
    if (op.GetInputDescByName("x1").GetDataType() != op.GetInputDescByName("x2").GetDataType()) {
        return GRAPH_FAILED;
    }
    return GRAPH_SUCCESS;
}

COMMON_INFER_FUNC_REG(AddWeighted, AddWeightedInferShape);
VERIFY_FUNC_REG(AddWeighted, AddWeightedVerify);
}

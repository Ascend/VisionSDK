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
 * Description: Compute AbsDiff of each element in input.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "abs_diff.h"
namespace ge {

IMPLEMT_COMMON_INFERFUNC(AbsDiffInferShape)
{
    TensorDesc tensordesc_output = op.GetOutputDescByName("y");
    tensordesc_output.SetShape(op.GetInputDescByName("x1").GetShape());
    tensordesc_output.SetDataType(op.GetInputDescByName("x1").GetDataType());
    tensordesc_output.SetFormat(op.GetInputDescByName("x1").GetFormat());

    std::vector<std::pair<int64_t, int64_t>> input_range;
    op.GetInputDescByName("x1").GetShapeRange(input_range);
    tensordesc_output.SetShapeRange(input_range);

    (void)op.UpdateOutputDesc("y", tensordesc_output);
    return GRAPH_SUCCESS;
}

IMPLEMT_VERIFIER(AbsDiff, AbsDiffVerify)
{
    return GRAPH_SUCCESS;
}

COMMON_INFER_FUNC_REG(AbsDiff, AbsDiffInferShape);
VERIFY_FUNC_REG(AbsDiff, AbsDiffVerify);

}  // namespace ge

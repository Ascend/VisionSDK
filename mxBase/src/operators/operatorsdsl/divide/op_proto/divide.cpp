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
 * Description: Operator Divide Proto File.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "divide.h"
namespace ge {

IMPLEMT_COMMON_INFERFUNC(DivideInferShape)
{
    TensorDesc tensordesc_output = op.GetOutputDescByName("y");
    tensordesc_output.SetShape(op.GetInputDescByName("x1").GetShape());
    DataType x1_type = op.GetInputDescByName("x1").GetDataType();
    DataType x2_type = op.GetInputDescByName("x2").GetDataType();
    DataType y_type = x1_type;
    if (x1_type == DataType::DT_FLOAT || x2_type == DataType::DT_FLOAT) {
        y_type = DataType::DT_FLOAT;
    } else if (x1_type == DataType::DT_FLOAT16 || x2_type == DataType::DT_FLOAT16) {
        y_type = DataType::DT_FLOAT16;
    } else {
        y_type = DataType::DT_UINT8;
    }
    tensordesc_output.SetDataType(y_type);
    tensordesc_output.SetFormat(op.GetInputDescByName("x1").GetFormat());

    std::vector<std::pair<int64_t, int64_t>> input_range;
    op.GetInputDescByName("x1").GetShapeRange(input_range);
    tensordesc_output.SetShapeRange(input_range);

    (void)op.UpdateOutputDesc("y", tensordesc_output);
    return GRAPH_SUCCESS;
}

IMPLEMT_VERIFIER(Divide, DivideVerify)
{
    return GRAPH_SUCCESS;
}

COMMON_INFER_FUNC_REG(Divide, DivideInferShape);
VERIFY_FUNC_REG(Divide, DivideVerify);

}  // namespace ge

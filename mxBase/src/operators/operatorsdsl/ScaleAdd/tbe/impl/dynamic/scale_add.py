#!/usr/bin/env python3
# -*- coding: UTF-8 -*-

# -------------------------------------------------------------------------
#  This file is part of the Vision SDK project.
# Copyright (c) 2025 Huawei Technologies Co.,Ltd.
#
# Vision SDK is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#
#           http://license.coscl.org.cn/MulanPSL2
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.
# -------------------------------------------------------------------------
# Description: Implement file of ScaleAdd op.
# Author: MindX SDK

import tbe.dsl as tbe
from tbe import tvm
from tbe.common.register import register_op_compute
from tbe.common.utils import para_check

from impl.util.platform_adapter import classify
from impl.util.platform_adapter import OpPatternMode
from impl.util.platform_adapter import para_check
from impl.util.platform_adapter import shape_util
from impl.util.platform_adapter import register_operator
from impl.util.platform_adapter import register_operator_compute


@register_op_compute("scale_add", op_mode="dynamic", support_fusion=False)
def scale_add_compute(x1, x2, y, scale, kernel_name="scale_add"):
    """
    To do: Implement the operator by referring to the
           TBE Operator Development Guide.
    """
    if x1.dtype == "uint8":
        x1 = tbe.cast_to(x1, "float16")
        x2 = tbe.cast_to(x2, "float16")
        res = tbe.vadd(tbe.vmuls(x1, scale), x2)
        res = tbe.cast_to(res, "uint8")
        return res
    res = tbe.vadd(tbe.vmuls(x1, scale), x2)
    return res


@register_operator("scale_add")
@para_check.check_op_params(para_check.REQUIRED_INPUT, para_check.REQUIRED_INPUT, para_check.REQUIRED_OUTPUT,
                            para_check.REQUIRED_ATTR_FLOAT, para_check.KERNEL_NAME)
def scale_add(x1, x2, y, scale, kernel_name="scale_add"):
    """
    To do: Implement the operator by referring to the
           TBE Operator Development Guide.
    """
    check_tuple = ("uint8", "float16", "float32")
    x1_data_type = x1.get("dtype").lower()
    para_check.check_dtype(x1_data_type, check_tuple, param_name="x1")

    x2_data_type = x2.get("dtype").lower()
    para_check.check_dtype(x2_data_type, check_tuple, param_name="x2")

    ins = classify([x1, x2], OpPatternMode.ELEWISE)
    schedules, tensors = [], []

    for _x1, _x2 in ins:
        with tbe.compute():
            shape_x1, shape_x2 = shape_util.variable_shape([_x1, _x2])
            x1_input = tvm.placeholder(shape_x1, name="x1", dtype=x1_data_type)
            x2_input = tvm.placeholder(shape_x2, name="x2", dtype=x2_data_type)
            res = scale_add_compute(x1_input, x2_input, y, scale, kernel_name)
            tensors.append([x1_input, x2_input, res])
        with tvm.target.cce():
            sch = tbe.auto_schedule(res)
        schedules.append(sch)

    # operator build
    config = {"name": kernel_name, "tensor_list": tensors}
    tbe.build(schedules, config)
    

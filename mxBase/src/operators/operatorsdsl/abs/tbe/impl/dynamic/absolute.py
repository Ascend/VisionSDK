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
from impl.util.platform_adapter import classify
from impl.util.platform_adapter import OpPatternMode
from impl.util.platform_adapter import para_check
from impl.util.platform_adapter import shape_util
from impl.util.platform_adapter import register_operator
from impl.util.platform_adapter import register_operator_compute
from tbe.common.register import register_op_compute
from tbe.common.utils import para_check


@register_op_compute("absolute", op_mode="dynamic", support_fusion=False)
def absolute_compute(x, y, kernel_name="absolute"):
    """
    To do: Implement the operator by referring to the
           TBE Operator Development Guide.
    """
    if x.dtype == 'uint8':
        x = tbe.cast_to(x, 'float16')
        res = tbe.vabs(x)
        res = tbe.cast_to(res, 'uint8')
        return res
    res = tbe.vabs(x)
    return res


@register_operator("absolute")
@para_check.check_op_params(para_check.REQUIRED_INPUT, para_check.REQUIRED_OUTPUT, para_check.KERNEL_NAME)
def absolute(x, y, kernel_name="absolute"):
    """
    To do: Implement the operator by referring to the
           TBE Operator Development Guide.
    """
    check_tuple = ("uint8", "float16", "float32")
    x_data_type = x.get("dtype").lower()
    para_check.check_dtype(x_data_type, check_tuple, param_name="x")

    ins = classify([x], OpPatternMode.ELEWISE)
    schedules, tensors = [], []

    for (_x,) in ins:
        with tbe.compute():
            shape_x = shape_util.variable_shape([_x])
            data_x = tvm.placeholder(shape_x[0], dtype=x_data_type, name="x")
            res = absolute_compute(data_x, y, kernel_name)
            tensors.append([data_x, res])
        with tvm.target.cce():
            sch = tbe.auto_schedule(res)
        schedules.append(sch)

    # operator build
    config = {"name": kernel_name,
              "tensor_list": tensors}
    tbe.build(schedules, config)
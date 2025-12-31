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
# Description: Operator Multiply Implement File.
# Author: MindX SDK

import tbe.dsl as tbe
from tbe import tvm
from tbe.common.register import register_op_compute
from tbe.common.utils import para_check, shape_util
from impl.util.platform_adapter import classify, OpPatternMode


@register_op_compute("multiply")
def multiply_compute(x1, x2, y, scale, kernel_name="multiply"):
    """
    To do: Implement the operator by referring to the
           TBE Operator Development Guide.
    """

    dtype_list = {"uint8": 0, "float16": 1, "float32": 2}
    ori_dtype = x1.dtype
    if x1.dtype in dtype_list.keys() and x2.dtype in dtype_list.keys():
        if dtype_list[x2.dtype] > dtype_list[x1.dtype]:
            ori_dtype = x2.dtype

    if ori_dtype == "uint8":
        x1 = tbe.cast_to(x1, "float32")
        x2 = tbe.cast_to(x2, "float32")
    else:
        x1 = tbe.cast_to(x1, ori_dtype)
        x2 = tbe.cast_to(x2, ori_dtype)

    res = tbe.vmul(x1, x2)
    res = tbe.vmuls(res, scale)
    res = tbe.cast_to(res, ori_dtype)
    return res


@para_check.check_op_params(para_check.REQUIRED_INPUT, para_check.REQUIRED_INPUT, para_check.REQUIRED_OUTPUT,
                            para_check.REQUIRED_ATTR_FLOAT, para_check.KERNEL_NAME)
def multiply(x1, x2, y, scale, kernel_name="multiply"):
    """
    To do: Implement the operator by referring to the
           TBE Operator Development Guide.
    """

    # 1. check dtypes of inputs in the list
    dtype_x1 = x1.get("dtype").lower()
    dtype_x2 = x2.get("dtype").lower()

    check_list = ("uint8", "float16", "float32")
    para_check.check_dtype(dtype_x1, check_list, param_name="x1")
    para_check.check_dtype(dtype_x2, check_list, param_name="x2")

    ins = classify([x1, x2], OpPatternMode.ELEWISE)
    schedules, tensors = [], []

    for (_x1, _x2) in ins:
        with tbe.compute():
            shape_list = shape_util.variable_shape([_x1, _x2])
            shape_x1 = shape_list[0]
            shape_x2 = shape_list[1]

            # 2. get input data
            data_x1 = tvm.placeholder(shape_x1, dtype=dtype_x1, name="data_x1")
            data_x2 = tvm.placeholder(shape_x2, dtype=dtype_x2, name="data_x2")

            # 3. compute multiply
            res = multiply_compute(data_x1, data_x2, y, scale, kernel_name)
            tensors.append([data_x1, data_x2, res])

        # auto schedule
        with tvm.target.cce():
            schedule = tbe.auto_schedule(res)
        schedules.append(schedule)

    # operator build
    config = {"name": kernel_name, "tensor_list": tensors}
    tbe.build(schedules, config)

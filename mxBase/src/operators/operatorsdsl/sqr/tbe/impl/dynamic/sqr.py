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
# Description: Compute Square of each element in input.
# Author: MindX SDK

import tbe.dsl as tbe
from tbe import tvm
from impl.util.platform_adapter import classify
from impl.util.platform_adapter import OpPatternMode
from impl.util.platform_adapter import para_check
from impl.util.platform_adapter import shape_util
from impl.util.platform_adapter import tbe_platform
from impl.util.platform_adapter import register_operator
from impl.util.platform_adapter import register_operator_compute
from tbe.common.register import register_op_compute
from tbe.common.utils import para_check


@register_operator_compute("sqr", op_mode="dynamic", support_fusion=True)
def sqr_compute(x1, y, kernel_name="sqr"):
    """
    algorithm: sqr
    calculating data's sqr,y= x*x

    Parameters
    ----------
    x1: TVM tensor
        the placeholder of input data
    y: dict
        shape and dtype of output, should be same shape and type as input
    kernel_name: str
        cce kernel name, default value is sqr

    Returns
    -------
    res : tvm.tensor
        the result of sqr
    """
    api_check = tbe_platform.api_check_support("tbe.dsl.vmul",
                                               "float32")
    input_dtype = x1.dtype
    if not (api_check) and input_dtype == "float32":
        x1 = tbe.cast_to(x1, "float16")
        res = tbe.vmul(x1, x1)
        res = tbe.cast_to(res, "float32")
    elif input_dtype == "uint8": 
        x1 = tbe.cast_to(x1, "float16")
        res = tbe.vmul(x1, x1)
        res = tbe.cast_to(res, "uint8")
    else:
        res = tbe.vmul(x1, x1)

    return res


@register_operator("sqr")
@para_check.check_op_params(para_check.REQUIRED_INPUT, para_check.REQUIRED_OUTPUT, para_check.KERNEL_NAME)
def sqr(x1, y, kernel_name="sqr"):
    """
    To do: Implement the operator by referring to the
           TBE Operator Development Guide.
    """

    x_dtype = x1.get("dtype").lower()
    check_list = ("float16", "float32", "uint8")
    para_check.check_dtype(x_dtype, check_list, param_name="x1")

    ins = classify([x1], OpPatternMode.ELEWISE)
    schedules, tensors = [], []
    for (x1,) in ins:
        with tbe.compute():
            # shape
            x_shape = shape_util.variable_shape([x1])
            # square_compute
            data_x = tvm.placeholder(x_shape[0], x_dtype, name="x1")
            res = sqr_compute(data_x, y, kernel_name)

            tensors.append((data_x, res))
        with tvm.target.cce():
            sch = tbe.auto_schedule(res)
        schedules.append(sch)

    # build
    config = {"name": kernel_name, "tensor_list": tensors}
    tbe.build(schedules, config)
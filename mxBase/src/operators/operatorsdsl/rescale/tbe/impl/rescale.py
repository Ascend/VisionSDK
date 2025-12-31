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
# Description: Operator Rescale Implement File.
# Author: MindX SDK

import tbe.dsl as tbe
from tbe import tvm
from tbe.common.register import register_op_compute
from tbe.common.utils import para_check


@register_op_compute("rescale")
def rescale_compute(x, y, scale, bias, kernel_name="rescale"):
    """
    To do: Implement the operator by referring to the
           TBE Operator Development Guide.
    """

    if x.dtype == "uint8":
        x = tbe.cast_to(x, "float16")
        res = tbe.vadds(tbe.vmuls(x, scale), bias)
        res = tbe.cast_to(res, "uint8")
        return res
    res = tbe.vadds(tbe.vmuls(x, scale), bias)
    return res


@para_check.check_op_params(para_check.REQUIRED_INPUT, para_check.REQUIRED_OUTPUT, para_check.REQUIRED_ATTR_FLOAT,
                            para_check.REQUIRED_ATTR_FLOAT, para_check.KERNEL_NAME)
def rescale(x, y, scale, bias, kernel_name="rescale"):
    para_check.check_dtype(x.get("dtype").lower(), ["uint8", "float16", "float32"])
    data_x = tvm.placeholder(x.get("shape"), dtype=x.get("dtype"), name="data_x")
    res = rescale_compute(data_x, y, scale, bias, kernel_name)

    # auto schedule
    with tvm.target.cce():
        schedule = tbe.auto_schedule(res)

    # operator build
    config = {"name": kernel_name,
              "tensor_list": [data_x, res]}
    tbe.build(schedule, config)

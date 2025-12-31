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
# Description: Operator AddWeighted Implement File.
# Author: MindX SDK

import tbe.dsl as tbe
from tbe import tvm
from tbe.common.register import register_op_compute
from tbe.common.utils import para_check


@register_op_compute("add_weighted")
def add_weighted_compute(x1, x2, y, alpha, beta, gamma, kernel_name="add_weighted"):
    """
    To do: Implement the operator by referring to the
           TBE Operator Development Guide.
    """

    ori_dtype = x1.dtype
    if ori_dtype == "uint8":
        x1 = tbe.cast_to(x1, "float32")
        x2 = tbe.cast_to(x2, "float32")
    res = tbe.vadds(tbe.vadd(tbe.vmuls(x1, alpha), tbe.vmuls(x2, beta)), gamma)
    res = tbe.cast_to(res, ori_dtype)
    return res


@para_check.check_op_params(para_check.REQUIRED_INPUT, para_check.REQUIRED_INPUT, para_check.REQUIRED_OUTPUT,
                            para_check.REQUIRED_ATTR_FLOAT, para_check.REQUIRED_ATTR_FLOAT,
                            para_check.REQUIRED_ATTR_FLOAT, para_check.KERNEL_NAME)
def add_weighted(x1, x2, y, alpha, beta, gamma, kernel_name="add_weighted"):
    """
    To do: Implement the operator by referring to the
           TBE Operator Development Guide.
    """
    data_x1 = tvm.placeholder(x1.get("shape"), dtype=x1.get("dtype"), name="data_x1")
    data_x2 = tvm.placeholder(x2.get("shape"), dtype=x2.get("dtype"), name="data_x2")

    res = add_weighted_compute(data_x1, data_x2, y, alpha, beta, gamma, kernel_name)

    # auto schedule
    with tvm.target.cce():
        schedule = tbe.auto_schedule(res)

    # operator build
    config = {"name": kernel_name,
              "tensor_list": [data_x1, data_x2, res]}
    tbe.build(schedule, config)
    

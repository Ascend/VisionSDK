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
"""
Description: python core operations on tensors api test.
Author: Vision SDK
Create: 2025
History: NA
"""

import os
import sys
import unittest

import numpy as np

from vision_pytest import BaseTestCase
import base
from base import Tensor, cvt_color, transpose_operator


class TestMatricesCoreOperationsOnTensors(BaseTestCase):
    def setUp(self):
        self.test_array = np.ones([300, 256, 4], dtype=np.uint8)

    def test_cvt_color_to_one_channel_return_success(self):
        try:
            test_tensor = Tensor(self.test_array)
            test_tensor.to_device(0)
            self.assertEqual(test_tensor.shape, [300, 256, 4])
            self.assertEqual(str(test_tensor.dtype), "dtype.uint8")
            result_tensor = cvt_color(test_tensor, base.color_rgba2gray)
            self.assertEqual(result_tensor.shape, [300, 256, 1])
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_cvt_color_to_three_channel_return_success(self):
        try:
            test_tensor = Tensor(self.test_array)
            test_tensor.to_device(0)
            self.assertEqual(test_tensor.shape, [300, 256, 4])
            self.assertEqual(str(test_tensor.dtype), "dtype.uint8")
            result_tensor = cvt_color(test_tensor, base.color_rgba2rgb)
            self.assertEqual(result_tensor.shape, [300, 256, 3])
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_transpose_operator_whc_to_hwc_return_success(self):
        try:
            test_tensor = Tensor(self.test_array)
            test_tensor.to_device(0)
            self.assertEqual(test_tensor.shape, [300, 256, 4])
            self.assertEqual(str(test_tensor.dtype), "dtype.uint8")
            result_tensor = transpose_operator(test_tensor, [1, 0, 2])
            self.assertEqual(result_tensor.shape, [256, 300, 4])
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")


if __name__ == '__main__':
    base.mx_init()
    failed = TestMatricesCoreOperationsOnTensors.run_tests()
    base.mx_deinit()
    sys.exit(1 if failed > 0 else 0)
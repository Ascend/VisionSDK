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
Description: python Tensor api test.
Author: Vision SDK
Create: 2024
History: NA
"""

import os
import sys
import unittest

import numpy as np

from vision_pytest import BaseTestCase
import base
from base import Tensor


class TestTensor(BaseTestCase):
    def setUp(self):
        self.test_array = np.ones([10, 3, 4, 3], dtype=np.float32)

    def test_tensor_init_return_success(self):
        try:
            test_tensor = Tensor(self.test_array)
            self.assertEqual(test_tensor.shape, [10, 3, 4, 3])
            self.assertEqual(str(test_tensor.dtype), "dtype.float32")
            arr1 = np.array(test_tensor)
            arr2 = np.array(test_tensor)
            self.assertTrue(np.array_equal(arr1, arr2))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_tensor_to_host_to_device_return_success(self):
        try:
            test_tensor = Tensor(self.test_array)
            self.assertEqual(test_tensor.device, -1)
            test_tensor.to_device(0)
            self.assertEqual(test_tensor.device, 0)
            self.assertEqual(test_tensor.shape, [10, 3, 4, 3])
            self.assertEqual(str(test_tensor.dtype), "dtype.float32")
            test_tensor.to_host()
            self.assertEqual(test_tensor.shape, [10, 3, 4, 3])
            self.assertEqual(str(test_tensor.dtype), "dtype.float32")
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_tensor_set_tensor_value_uint8_return_success(self):
        try:
            test_tensor = Tensor(np.ones([10, 3, 4], dtype=np.uint8))
            self.assertEqual(test_tensor.device, -1)
            test_tensor.to_device(0)
            test_tensor.set_tensor_value(200, base.uint8)
            self.assertEqual(test_tensor.shape, [10, 3, 4])
            self.assertEqual(str(test_tensor.dtype), "dtype.uint8")
            test_tensor.to_host()
            result_array = np.array(test_tensor)
            expected_result = np.full([10, 3, 4], [200, 200, 200, 200], dtype=np.uint8)
            self.assertTrue(np.array_equal(result_array, expected_result))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_tensor_set_tensor_value_int32_return_success(self):
        try:
            test_tensor = Tensor(np.ones([10, 3, 4], dtype=np.int32))
            self.assertEqual(test_tensor.device, -1)
            test_tensor.to_device(0)
            test_tensor.set_tensor_value(200, base.int32)
            self.assertEqual(test_tensor.shape, [10, 3, 4])
            self.assertEqual(str(test_tensor.dtype), "dtype.int32")
            test_tensor.to_host()
            result_array = np.array(test_tensor)
            expected_result = np.full([10, 3, 4], [200, 200, 200, 200], dtype=np.int32)
            self.assertTrue(np.array_equal(result_array, expected_result))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_tensor_set_tensor_value_float16_return_success(self):
        try:
            test_tensor = Tensor(np.ones([10, 3, 4], dtype=np.float16))
            self.assertEqual(test_tensor.device, -1)
            test_tensor.to_device(0)
            test_tensor.set_tensor_value(200, base.float16)
            self.assertEqual(test_tensor.shape, [10, 3, 4])
            self.assertEqual(str(test_tensor.dtype), "dtype.float16")
            test_tensor.to_host()
            result_array = np.array(test_tensor)
            expected_result = np.full([10, 3, 4], [200, 200, 200, 200], dtype=np.float16)
            self.assertTrue(np.array_equal(result_array, expected_result))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_tensor_set_tensor_value_float32_return_success(self):
        try:
            test_tensor = Tensor(np.ones([10, 3, 4], dtype=np.float32))
            self.assertEqual(test_tensor.device, -1)
            test_tensor.to_device(0)
            test_tensor.set_tensor_value(200, base.float32)
            self.assertEqual(test_tensor.shape, [10, 3, 4])
            self.assertEqual(str(test_tensor.dtype), "dtype.float32")
            test_tensor.to_host()
            result_array = np.array(test_tensor)
            expected_result = np.full([10, 3, 4], [200, 200, 200, 200], dtype=np.float32)
            self.assertTrue(np.array_equal(result_array, expected_result))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_tensor_set_tensor_value_unsupported_tensor_datatype_return_fail(self):
        test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.int64)
        test_tensor1 = Tensor(test_array1)
        test_tensor1.to_device(0)
        try:
            result_tensor = test_tensor1.set_tensor_value(200, base.float32)
            self.fail("Expected an exception due to dtype unsupported, but none was raised.")
        except Exception as e:
            self.assertIn(" (Code = 1004, Message = \"Invalid parameter\")", str(e))

    def test_tensor_set_tensor_value_unsupported_value_datatype_return_fail(self):
        test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.int64)
        test_tensor1 = Tensor(test_array1)
        test_tensor1.to_device(0)
        try:
            result_tensor = test_tensor1.set_tensor_value(200, base.undefined)
            self.fail("Expected an exception due to dtype unsupported, but none was raised.")
        except Exception as e:
            self.assertIn("Invalid Input Tensor dtype, should be one of dtype.uint8,dtype.int32, "
                          "dtype.float16, dtype.float32", str(e))

    def test_tensor_set_tensor_value_with_different_datatype_return_fail(self):
        test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.uint8)
        test_tensor1 = Tensor(test_array1)
        test_tensor1.to_device(0)
        try:
            result_tensor = test_tensor1.set_tensor_value(200, base.float32)
            self.fail("Expected an exception due to different dtype, but none was raised.")
        except Exception as e:
            self.assertIn(" (Code = 1004, Message = \"Invalid parameter\")", str(e))

    def test_tensor_set_tensor_value_with_tensor_on_host_return_fail(self):
        test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.float32)
        test_tensor1 = Tensor(test_array1)
        try:
            result_tensor = test_tensor1.set_tensor_value(200, base.float32)
            self.fail("Expected an exception due to tensor on host not on device, but none was raised.")
        except Exception as e:
            self.assertIn(" (Code = 1001, Message = \"General Failed\")", str(e))

    def test_tensor_set_tensor_value_with_value_exceeded_range_return_fail(self):
        test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.uint8)
        test_tensor1 = Tensor(test_array1)
        try:
            result_tensor = test_tensor1.set_tensor_value(20000000000, base.uint8)
            self.fail("Expected an exception due to exceeded data, but none was raised.")
        except Exception as e:
            self.assertIn("Input value exceeds the range of the target data type.", str(e))



if __name__ == '__main__':
    base.mx_init()
    failed = TestTensor.run_tests()
    base.mx_deinit()
    sys.exit(1 if failed > 0 else 0)
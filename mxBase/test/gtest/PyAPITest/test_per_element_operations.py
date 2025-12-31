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
Description: python per element operations api test.
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
from base import (Tensor, convert_to, clip, add, subtract, multiply,
                  divide, min_operator, max_operator)


class TestPerElementOperations(BaseTestCase):
    def setUp(self):
        self.test_array = np.ones([300, 256, 4], dtype=np.uint8)

    def test_convert_to_return_success(self):
        try:
            test_tensor = Tensor(self.test_array)
            test_tensor.to_device(0)
            self.assertEqual(test_tensor.shape, [300, 256, 4])
            self.assertEqual(str(test_tensor.dtype), "dtype.uint8")
            result_tensor = convert_to(test_tensor, base.int16)
            self.assertEqual(result_tensor.shape, [300, 256, 4])
            self.assertEqual(str(result_tensor.dtype), "dtype.int16")
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_clip_return_success(self):
        try:
            test_array = np.full([300, 256, 4], [1, 7, 4, 5], dtype=np.uint8)
            test_tensor = Tensor(test_array)
            test_tensor.to_device(0)
            self.assertEqual(test_tensor.shape, [300, 256, 4])
            self.assertEqual(str(test_tensor.dtype), "dtype.uint8")
            result_tensor = clip(test_tensor, 3, 6)
            self.assertEqual(result_tensor.shape, [300, 256, 4])
            self.assertEqual(str(result_tensor.dtype), "dtype.uint8")
            result_tensor.to_host()
            result_array = np.array(result_tensor)
            expected_result = np.full([300, 256, 4], [3, 6, 4, 5], dtype=np.uint8)
            self.assertTrue(np.array_equal(result_array, expected_result))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_add_return_success(self):
        try:
            test_array = np.full([300, 256, 4], [1, 7, 4, 5], dtype=np.uint8)
            test_tensor1 = Tensor(test_array)
            test_tensor1.to_device(0)
            test_tensor2 = Tensor(test_array)
            test_tensor2.to_device(0)
            result_tensor = add(test_tensor1, test_tensor2)
            self.assertEqual(result_tensor.shape, [300, 256, 4])
            self.assertEqual(str(result_tensor.dtype), "dtype.uint8")
            result_tensor.to_host()
            result_array = np.array(result_tensor)
            expected_result = np.full([300, 256, 4], [2, 14, 8, 10], dtype=np.uint8)
            self.assertTrue(np.array_equal(result_array, expected_result))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_subtract_return_success(self):
        try:
            test_array1 = np.full([300, 256, 4], [5, 6, 7, 8], dtype=np.uint8)
            test_array2 = np.full([300, 256, 4], [1, 3, 2, 2], dtype=np.uint8)
            test_tensor1 = Tensor(test_array1)
            test_tensor1.to_device(0)
            test_tensor2 = Tensor(test_array2)
            test_tensor2.to_device(0)
            result_tensor = subtract(test_tensor1, test_tensor2)
            self.assertEqual(result_tensor.shape, [300, 256, 4])
            self.assertEqual(str(result_tensor.dtype), "dtype.uint8")
            result_tensor.to_host()
            result_array = np.array(result_tensor)
            expected_result = np.full([300, 256, 4], [4, 3, 5, 6], dtype=np.uint8)
            self.assertTrue(np.array_equal(result_array, expected_result))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_multiply_return_success(self):
        try:
            test_array1 = np.full([300, 256, 4], [2, 2, 2, 2], dtype=np.uint8)
            test_array2 = np.full([300, 256, 4], [1, 2, 3, 4], dtype=np.uint8)
            test_tensor1 = Tensor(test_array1)
            test_tensor1.to_device(0)
            test_tensor2 = Tensor(test_array2)
            test_tensor2.to_device(0)
            result_tensor = multiply(test_tensor1, test_tensor2)
            self.assertEqual(result_tensor.shape, [300, 256, 4])
            self.assertEqual(str(result_tensor.dtype), "dtype.uint8")
            result_tensor.to_host()
            result_array = np.array(result_tensor)
            expected_result = np.full([300, 256, 4], [2, 4, 6, 8], dtype=np.uint8)
            self.assertTrue(np.array_equal(result_array, expected_result))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_multiply_with_scale_return_success(self):
        try:
            test_array1 = np.full([300, 256, 4], [2, 2, 2, 2], dtype=np.uint8)
            test_array2 = np.full([300, 256, 4], [1, 2, 3, 4], dtype=np.uint8)
            test_tensor1 = Tensor(test_array1)
            test_tensor1.to_device(0)
            test_tensor2 = Tensor(test_array2)
            test_tensor2.to_device(0)
            result_tensor = multiply(test_tensor1, test_tensor2, 2)
            self.assertEqual(result_tensor.shape, [300, 256, 4])
            self.assertEqual(str(result_tensor.dtype), "dtype.uint8")
            result_tensor.to_host()
            result_array = np.array(result_tensor)
            expected_result = np.full([300, 256, 4], [4, 8, 12, 16], dtype=np.uint8)
            self.assertTrue(np.array_equal(result_array, expected_result))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_divide_return_success(self):
        try:
            test_array1 = np.full([300, 256, 4], [6, 9, 12, 10], dtype=np.uint8)
            test_array2 = np.full([300, 256, 4], [3, 3, 2, 5], dtype=np.uint8)
            test_tensor1 = Tensor(test_array1)
            test_tensor1.to_device(0)
            test_tensor2 = Tensor(test_array2)
            test_tensor2.to_device(0)
            result_tensor = divide(test_tensor1, test_tensor2)
            self.assertEqual(result_tensor.shape, [300, 256, 4])
            self.assertEqual(str(result_tensor.dtype), "dtype.uint8")
            result_tensor.to_host()
            result_array = np.array(result_tensor)
            expected_result = np.full([300, 256, 4], [2, 3, 6, 2], dtype=np.uint8)
            self.assertTrue(np.array_equal(result_array, expected_result))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_divide_with_scale_return_success(self):
        try:
            test_array1 = np.full([300, 256, 4], [6, 9, 12, 10], dtype=np.uint8)
            test_array2 = np.full([300, 256, 4], [3, 3, 2, 5], dtype=np.uint8)
            test_tensor1 = Tensor(test_array1)
            test_tensor1.to_device(0)
            test_tensor2 = Tensor(test_array2)
            test_tensor2.to_device(0)
            result_tensor = divide(test_tensor1, test_tensor2, 2)
            self.assertEqual(result_tensor.shape, [300, 256, 4])
            self.assertEqual(str(result_tensor.dtype), "dtype.uint8")
            result_tensor.to_host()
            result_array = np.array(result_tensor)
            expected_result = np.full([300, 256, 4], [4, 6, 12, 4], dtype=np.uint8)
            self.assertTrue(np.array_equal(result_array, expected_result))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_min_return_success(self):
        try:
            test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.uint8)
            test_array2 = np.full([300, 256, 4], [3, 3, 2, 5], dtype=np.uint8)
            test_tensor1 = Tensor(test_array1)
            test_tensor1.to_device(0)
            test_tensor2 = Tensor(test_array2)
            test_tensor2.to_device(0)
            result_tensor = min_operator(test_tensor1, test_tensor2)
            self.assertEqual(result_tensor.shape, [300, 256, 4])
            self.assertEqual(str(result_tensor.dtype), "dtype.uint8")
            result_tensor.to_host()
            result_array = np.array(result_tensor)
            expected_result = np.full([300, 256, 4], [3, 1, 0, 5], dtype=np.uint8)
            self.assertTrue(np.array_equal(result_array, expected_result))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_max_return_success(self):
        try:
            test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.uint8)
            test_array2 = np.full([300, 256, 4], [3, 3, 2, 5], dtype=np.uint8)
            test_tensor1 = Tensor(test_array1)
            test_tensor1.to_device(0)
            test_tensor2 = Tensor(test_array2)
            test_tensor2.to_device(0)
            result_tensor = max_operator(test_tensor1, test_tensor2)
            self.assertEqual(result_tensor.shape, [300, 256, 4])
            self.assertEqual(str(result_tensor.dtype), "dtype.uint8")
            result_tensor.to_host()
            result_array = np.array(result_tensor)
            expected_result = np.full([300, 256, 4], [6, 3, 2, 10], dtype=np.uint8)
            self.assertTrue(np.array_equal(result_array, expected_result))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_max_with_unsupported_datatype_return_fail(self):
        test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.int64)
        test_array2 = np.full([300, 256, 4], [3, 3, 2, 5], dtype=np.int64)
        test_tensor1 = Tensor(test_array1)
        test_tensor1.to_device(0)
        test_tensor2 = Tensor(test_array2)
        test_tensor2.to_device(0)
        try:
            result_tensor = max_operator(test_tensor1, test_tensor2)
            self.fail("Expected an exception due to dtype unsupported, but none was raised.")
        except Exception as e:
            self.assertIn(" (Code = 1004, Message = \"Invalid parameter\")", str(e))

    def test_min_with_unsupported_datatype_return_fail(self):
        test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.int64)
        test_array2 = np.full([300, 256, 4], [3, 3, 2, 5], dtype=np.int64)
        test_tensor1 = Tensor(test_array1)
        test_tensor1.to_device(0)
        test_tensor2 = Tensor(test_array2)
        test_tensor2.to_device(0)
        try:
            result_tensor = min_operator(test_tensor1, test_tensor2)
            self.fail("Expected an exception due to dtype unsupported, but none was raised.")
        except Exception as e:
            self.assertIn(" (Code = 1004, Message = \"Invalid parameter\")", str(e))

    def test_max_with_different_datatype_return_fail(self):
        test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.float16)
        test_array2 = np.full([300, 256, 4], [3, 3, 2, 5], dtype=np.uint8)
        test_tensor1 = Tensor(test_array1)
        test_tensor1.to_device(0)
        test_tensor2 = Tensor(test_array2)
        test_tensor2.to_device(0)
        try:
            result_tensor = max_operator(test_tensor1, test_tensor2)
            self.fail("Expected an exception due to dtype unsupported, but none was raised.")
        except Exception as e:
            self.assertIn(" (Code = 1004, Message = \"Invalid parameter\")", str(e))

    def test_min_with_different_datatype_return_fail(self):
        test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.float16)
        test_array2 = np.full([300, 256, 4], [3, 3, 2, 5], dtype=np.uint8)
        test_tensor1 = Tensor(test_array1)
        test_tensor1.to_device(0)
        test_tensor2 = Tensor(test_array2)
        test_tensor2.to_device(0)
        try:
            result_tensor = min_operator(test_tensor1, test_tensor2)
            self.fail("Expected an exception due to dtype unsupported, but none was raised.")
        except Exception as e:
            self.assertIn(" (Code = 1004, Message = \"Invalid parameter\")", str(e))

    def test_min_with_host_tensor_return_fail(self):
        test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.float16)
        test_array2 = np.full([300, 256, 4], [3, 3, 2, 5], dtype=np.uint8)
        test_tensor1 = Tensor(test_array1)
        test_tensor1.to_device(0)
        test_tensor2 = Tensor(test_array2)
        test_tensor2.to_device(0)
        try:
            result_tensor = min_operator(test_tensor1, test_tensor2)
            self.fail("Expected an exception due to tensor on host not on device, but none was raised.")
        except Exception as e:
            self.assertIn(" (Code = 1004, Message = \"Invalid parameter\")", str(e))

    def test_max_with_host_tensor_return_fail(self):
        test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.float16)
        test_array2 = np.full([300, 256, 4], [3, 3, 2, 5], dtype=np.uint8)
        test_tensor1 = Tensor(test_array1)
        test_tensor1.to_device(0)
        test_tensor2 = Tensor(test_array2)
        test_tensor2.to_device(0)
        try:
            result_tensor = max_operator(test_tensor1, test_tensor2)
            self.fail("Expected an exception due to tensor on host not on device, but none was raised.")
        except Exception as e:
            self.assertIn(" (Code = 1004, Message = \"Invalid parameter\")", str(e))

    def test_min_with_invalid_input_return_fail(self):
        test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.float16)
        test_tensor1 = Tensor(test_array1)
        test_tensor1.to_device(0)
        try:
            result_tensor = min_operator(test_tensor1, [])
            self.fail("Expected an exception due to invalid input, but none was raised.")
        except Exception as e:
            self.assertIn("in method 'min_operator', argument 2 of type 'PyBase::Tensor const &'", str(e))

    def test_max_with_invalid_input_return_fail(self):
        test_array1 = np.full([300, 256, 4], [6, 1, 0, 10], dtype=np.float16)
        test_tensor1 = Tensor(test_array1)
        test_tensor1.to_device(0)
        try:
            result_tensor = max_operator(test_tensor1, [])
            self.fail("Expected an exception due to invalid input, but none was raised.")
        except Exception as e:
            self.assertIn("in method 'max_operator', argument 2 of type 'PyBase::Tensor const &'", str(e))






if __name__ == '__main__':
    base.mx_init()
    failed = TestPerElementOperations.run_tests()
    base.mx_deinit()
    sys.exit(1 if failed > 0 else 0)
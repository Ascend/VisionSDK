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
Description: python Image api test.
Author: MindX SDK
Create: 2024
History: NA
"""

import os
import sys
import unittest

import numpy as np

from vision_pytest import BaseTestCase
import base
from base import Tensor, Image, Model, ImageProcessor, log, Size, Rect, Point


class TestImage(BaseTestCase):
    def setUp(self):
        self.image_path = "./data/test.jpg"
        self.device_id = 0
        self.image_processor = ImageProcessor(self.device_id)
        self.image_array = np.zeros((63, 63, 3), dtype=np.uint8)
        self.origin_size = Size(63, 63)
        self.aligned_size = Size(160, 160)

    def test_image_init_return_success(self):
        try:
            image = base.Image()
            self.assertEqual(image.device, -1)
            self.assertEqual(image.width, 0)
            self.assertEqual(image.height, 0)
            self.assertEqual(image.original_height, 0)
            self.assertEqual(image.original_width, 0)
            self.assertEqual(image.format, base.image_format.nv12)
            image = base.Image(self.image_array, base.rgb, (self.origin_size, self.aligned_size))
            self.assertEqual(image.device, -1)
            self.assertEqual(image.width, 160)
            self.assertEqual(image.height, 160)
            self.assertEqual(image.original_height, 63)
            self.assertEqual(image.original_width, 63)
            self.assertEqual(image.format, base.rgb)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_to_tensor_return_success(self):
        try:
            image = base.Image(self.image_array, base.rgb, (self.origin_size, self.aligned_size))
            image_to_tensor = image.to_tensor()
            self.assertEqual([1, 160, 160, 3], image_to_tensor.shape)
            image_to_tensor = image.get_tensor()
            self.assertEqual([1, 160, 160, 3], image_to_tensor.shape)
            image_to_tensor = image.get_original_tensor()
            self.assertEqual([1, 63, 63, 3], image_to_tensor.shape)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_to_host_to_device_return_success(self):
        try:
            image = base.Image(self.image_array, base.rgb, (self.origin_size, self.aligned_size))
            self.assertEqual(image.device, -1)
            image.to_device(0)
            self.assertEqual(image.device, 0)
            image.to_host()
            self.assertEqual(image.device, -1)
            self.assertEqual(image.width, 160)
            self.assertEqual(image.height, 160)
            self.assertEqual(image.original_height, 63)
            self.assertEqual(image.original_width, 63)
            self.assertEqual(image.format, base.image_format.rgb)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_serialize_and_unserialize_return_success(self):
        try:
            image = base.Image(self.image_array, base.rgb, (self.origin_size, self.aligned_size))
            image.serialize("./data/decodeDataImage.dat", True)
            load_image = Image()
            load_image.unserialize("./data/decodeDataImage.dat")
            self.assertEqual(image.width, 160)
            self.assertEqual(image.height, 160)
            self.assertEqual(image.original_height, 63)
            self.assertEqual(image.original_width, 63)
            self.assertEqual(image.format, base.image_format.rgb)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_dump_buffer_return_success(self):
        try:
            image = base.Image(self.image_array, base.rgb, (self.origin_size, self.aligned_size))
            image.dump_buffer("./data/decodeDumpBuffer.dat", True)
            self.assertTrue(os.path.exists("./data/decodeDumpBuffer.dat"))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_tensor_to_image_return_success(self):
        try:
            image = base.Image(self.image_array, base.rgb, (self.origin_size, self.aligned_size))
            tensor = image.to_tensor()
            converted_image = base.tensor_to_image(tensor, base.rgb)
            self.assertEqual(image.width, converted_image.width)
            self.assertEqual(image.height, converted_image.height)
            self.assertEqual(image.format, converted_image.format)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

if __name__ == '__main__':
    base.mx_init()
    failed = TestImage.run_tests()
    base.mx_deinit()
    sys.exit(1 if failed > 0 else 0)
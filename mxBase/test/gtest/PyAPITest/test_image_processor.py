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
Description: python ImageProcessor api test.
Author: MindX SDK
Create: 2024
History: NA
"""

import random
import os
import sys
import unittest

from vision_pytest import BaseTestCase
import base
from base import Tensor, Image, Model, ImageProcessor, log, Size, Rect, Point, ModelLoadOptV2, Dim, Color


class TestImageProcessor(BaseTestCase):
    def setUp(self):
        self.device_id = 0
        try:
            self.image_processor = ImageProcessor(self.device_id)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")
        self.image_path = "./data/test.jpg"
        self.save_path = "./data/result.jpg"
        self.image_format = base.bgr

    def test_image_processor_decode_return_success(self):
        try:
            decoded_img = self.image_processor.decode(self.image_path, base.nv12)
            self.assertEqual(decoded_img.format, base.nv12)
            self.assertEqual(decoded_img.height, 720)
            self.assertEqual(decoded_img.width, 1280)
            self.assertEqual(decoded_img.original_height, 720)
            self.assertEqual(decoded_img.original_width, 1280)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_processor_encode_return_fail_with_invalid_level(self):
        decoded_img = self.image_processor.decode(self.image_path, base.nv12)
        with self.assertRaises(Exception):
            self.image_processor.encode(decoded_img, self.save_path, 0)
        with self.assertRaises(Exception):
            self.image_processor.encode(decoded_img, self.save_path, 101)
    
    def test_image_processor_encode_return_success(self):
        try:
            decoded_img = self.image_processor.decode(self.image_path, base.nv12)
            self.image_processor.encode(decoded_img, self.save_path, 100)
            self.assertTrue(os.path.exists(self.save_path))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_processor_decode_bytes_return_success(self):
        try:
            with open(self.image_path, "rb") as f:
                image_bytes = f.read()
            decoded_img = self.image_processor.decode_bytes(image_bytes, len(image_bytes), base.nv21)
            self.assertEqual(decoded_img.format, base.nv21)
            self.assertEqual(decoded_img.height, 720)
            self.assertEqual(decoded_img.width, 1280)
            self.assertEqual(decoded_img.original_height, 720)
            self.assertEqual(decoded_img.original_width, 1280)
            decoded_img = self.image_processor.decode_bytes(image_bytes, len(image_bytes))
            self.assertEqual(decoded_img.format, base.nv12)
            self.assertEqual(decoded_img.height, 720)
            self.assertEqual(decoded_img.width, 1280)
            self.assertEqual(decoded_img.original_height, 720)
            self.assertEqual(decoded_img.original_width, 1280)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_processor_decode_bytes_return_fail(self):
        data = str("test")
        with self.assertRaises(Exception):
            decoded_img = self.image_processor.decode_bytes(data, len(data), base.nv21)
        data = b"test"
        with self.assertRaises(Exception):
            decoded_img = self.image_processor.decode_bytes(data, len(data) + 1, base.nv21)

    def test_image_processor_encode_bytes_return_success(self):
        try:
            decoded_img = self.image_processor.decode(self.image_path, base.nv12)
            image_data = self.image_processor.encode_bytes(decoded_img)
            with open("./data/copy.jpg", "wb") as f:
                f.write(image_data)
            decoded_img = self.image_processor.decode_bytes(image_data, len(image_data))
            self.assertEqual(decoded_img.format, base.nv12)
            self.assertEqual(decoded_img.height, 720)
            self.assertEqual(decoded_img.width, 1280)
            self.assertEqual(decoded_img.original_height, 720)
            self.assertEqual(decoded_img.original_width, 1280)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_processor_encode_bytes_return_fail_with_invalid_level(self):
        decoded_img = self.image_processor.decode(self.image_path, base.nv12)
        with self.assertRaises(Exception):
            image_data = self.image_processor.encode_bytes(decoded_img, 0)
        with self.assertRaises(Exception):
            image_data = self.image_processor.encode_bytes(decoded_img, 101)

    def test_image_processor_resize_return_success(self):
        try:
            decoded_img = self.image_processor.decode(self.image_path, base.nv12)
            size_conf = Size(512, 512)
            resized_img = self.image_processor.resize(decoded_img, size_conf, base.huaweiu_high_order_filter)
            self.assertEqual(resized_img.format, base.nv12)
            self.assertEqual(resized_img.width, size_conf.width)
            self.assertEqual(resized_img.height, size_conf.height)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_processor_padding_return_success(self):
        pad_dim = Dim(0, 0, 1, 1)
        pad_color = Color(random.randint(1, 254), random.randint(1, 254), random.randint(1, 254))
        try:
            decoded_img = self.image_processor.decode(self.image_path, base.nv12)
            padded_img = self.image_processor.padding(decoded_img, pad_dim, pad_color, base.border_constant)
            self.assertEqual(padded_img.format, base.nv12)
            self.assertEqual(padded_img.width, 1280)
            self.assertEqual(padded_img.height, 724)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_processor_crop_return_success(self):
        crop_para = [Rect(0, 0, 18, 6), Rect(0, 0, 18, 6)]
        try:
            decoded_img = self.image_processor.decode(self.image_path, base.nv12)
            croped_img = self.image_processor.crop(decoded_img, crop_para)
            self.assertEqual(len(croped_img), 2)
            self.assertEqual(croped_img[0].width, 32)
            self.assertEqual(croped_img[0].height, 6)
            decoded_imgs = []
            for _ in range(10):
                decoded_imgs.append(decoded_img)
            croped_img = self.image_processor.crop(decoded_imgs, crop_para)
            self.assertEqual(len(croped_img), 20)
            self.assertEqual(croped_img[0].width, 32)
            self.assertEqual(croped_img[0].height, 6)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_processor_crop_resize_return_success(self):
        crop_size = [(Rect(0, 0, 128, 128), Size(512, 512))]
        try:
            decoded_img = self.image_processor.decode(self.image_path, base.nv12)
            crop_resized_img = self.image_processor.crop_resize(decoded_img, crop_size)
            self.assertEqual(crop_resized_img[0].width, 512)
            self.assertEqual(crop_resized_img[0].height, 512)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_processor_crop_paste_return_success(self):
        try:
            decoded_img = self.image_processor.decode(self.image_path, base.nv12)
            paste_img = self.image_processor.decode(self.image_path, base.nv12)
            crop_pasted_img = self.image_processor.crop_paste(decoded_img, 
                                                              (Rect(0, 0, 640, 512), Rect(0, 0, 320, 320)), paste_img)
            self.assertEqual(paste_img.width, 1280)
            self.assertEqual(paste_img.height, 720)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_processor_convert_format_return_success(self):
        try:
            decoded_img = self.image_processor.decode(self.image_path, base.nv12)
            rgb_img = self.image_processor.convert_format(decoded_img, base.rgb)
            self.assertEqual(rgb_img.format, base.rgb)
            self.assertEqual(rgb_img.height, 720)
            self.assertEqual(rgb_img.width, 1280)
            self.assertEqual(rgb_img.original_height, 720)
            self.assertEqual(rgb_img.original_width, 1280)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_image_processor_convert_format_throw_exception(self):
        with self.assertRaises(RuntimeError) as context:
            decoded_img = self.image_processor.decode(self.image_path, base.nv12)
            yuv420_img = self.image_processor.convert_format(decoded_img, base.nv12)
        self.assertEqual(str(context.exception), ' (Code = 1004, Message = "Invalid parameter") ')


if __name__ == '__main__':
    base.mx_init()
    failed = TestImageProcessor.run_tests()
    base.mx_deinit()
    sys.exit(1 if failed > 0 else 0)
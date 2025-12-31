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
Description: python dvpp api test.
Author: MindX SDK
Create: 2024
History: NA
"""
import sys
import unittest

from vision_pytest import BaseTestCase
import base
from base import dvpp, Image, Size


class TestDvpp(BaseTestCase):
    def setUp(self):
        self.device_id = 0

    def test_dvpp_read_image_return_success(self):
        image_path = "./data/test.jpg"
        image = Image()
        device_id = self.device_id
        decode_format = image.format
        output_image = dvpp.read_image(image_path, device_id, decode_format)
        self.assertEqual(output_image.device, 0)
        self.assertEqual(output_image.width, 1280)
        self.assertEqual(output_image.height, 720)
        self.assertEqual(output_image.original_height, 720)
        self.assertEqual(output_image.original_width, 1280)
        self.assertEqual(output_image.format, base.image_format.nv12)

    def test_dvpp_resize_return_success(self):
        image_path = "./data/test.jpg"
        image = Image()
        device_id = self.device_id
        decode_format = image.format
        output_image = dvpp.read_image(image_path, device_id, decode_format)
        size_conf = Size(416, 416)
        resized_image = dvpp.resize(output_image, size_conf, base.huaweiu_high_order_filter)
        self.assertEqual(resized_image.device, 0)
        self.assertEqual(resized_image.width, 416)
        self.assertEqual(resized_image.height, 416)
        self.assertEqual(resized_image.original_height, 416)
        self.assertEqual(resized_image.original_width, 416)
        self.assertEqual(resized_image.format, base.image_format.nv12)

if __name__ == '__main__':
    cfg = base.AppGlobalCfgExtra()
    cfg.vpcChnNum = 1
    cfg.jpegdChnNum = 1
    cfg.pngdChnNum = 1
    cfg.jpegeChnNum = 1
    base.mx_init(cfg)
    failed = TestDvpp.run_tests()
    base.mx_deinit()
    sys.exit(1 if failed > 0 else 0)
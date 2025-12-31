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
Description: python Model api test.
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
from base import Tensor, Image, Model, ImageProcessor, log, Size, Rect, Point, DeviceMemory, ModelLoadOptV2
from base import visionDataFormat


class TestModel(BaseTestCase):
    def setUp(self):
        self.device_id = 0
        self.image_processor = ImageProcessor(self.device_id)
        self.image_path = "./data/test.jpg"
        self.model_path = "../../../models/yolov3/yolov3_tf_bs1_fp16.om"
        self.size_cof = Size(416, 416)

    def test_device_memory_return_success(self):
        try:
            device_momory = base.DeviceMemory(1)
            device_momory = DeviceMemory(20)
            ptr_val = device_momory.get_data()
            self.assertNotEqual(ptr_val, 0)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_model_init_with_path_return_success(self):
        try:
            model1 = base.model(self.model_path, deviceId=self.device_id)
            self.assertEqual(model1.input_format, visionDataFormat.NHWC)
            self.assertEqual(model1.input_num, 1)
            self.assertEqual(model1.output_num, 3)
            model2 = base.Model(self.model_path, deviceId=self.device_id)
            self.assertEqual(model1.input_format, visionDataFormat.NHWC)
            self.assertEqual(model1.input_num, 1)
            self.assertEqual(model1.output_num, 3)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_model_init_with_opt_return_success(self):
        mdl_load_opt = ModelLoadOptV2()
        mdl_load_opt.loadType = base.load_model_from_mem
        np_data = np.fromfile(self.model_path, dtype="int8")
        try:
            modelptr = base.bytes_to_ptr(np_data)
            self.assertNotEqual(modelptr, 0)
            mdl_load_opt.modelPtr = modelptr
            mdl_load_opt.modelSize = np_data.size
            m = base.Model(mdl_load_opt, deviceId=self.device_id)
            self.assertEqual(m.input_format, visionDataFormat.NHWC)
            self.assertEqual(m.input_num, 1)
            self.assertEqual(m.output_num, 3)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_model_infer_return_success(self):
        try:
            decoded_img = self.image_processor.decode(self.image_path, base.nv12)
            resized_img = self.image_processor.resize(decoded_img, self.size_cof, base.huaweiu_high_order_filter)
            tensors = [resized_img.to_tensor()]
            m = base.model(self.model_path, deviceId=self.device_id)
            outputs = m.infer(tensors)
            self.assertEqual(str(outputs[0].dtype), 'dtype.float32')
            self.assertEqual(outputs[0].shape, [1, 13, 13, 255])
            outputs = m.infer(resized_img.to_tensor())
            self.assertEqual(str(outputs[0].dtype), 'dtype.float32')
            self.assertEqual(outputs[0].shape, [1, 13, 13, 255])
            self.assertEqual("visionDataFormat.NHWC", str(m.input_format))
            self.assertEqual("dtype.uint8", str(m.input_dtype(0)))
            self.assertEqual([1, 624, 416, 1], m.input_shape(0))
            self.assertEqual("dtype.float32", str(m.output_dtype(0)))
            self.assertEqual([1, 13, 13, 255], m.output_shape(0))
            self.assertEqual("[]", str(m.model_gear()))
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

if __name__ == '__main__':
    base.mx_init()
    failed = TestModel.run_tests()
    base.mx_deinit()
    sys.exit(1 if failed > 0 else 0)
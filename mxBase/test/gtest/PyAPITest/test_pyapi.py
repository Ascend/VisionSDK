#!/usr/bin/env python3
# coding=utf-8
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
-------------------------------------------------------------------------
 This file is part of the Vision SDK project.
Copyright (c) 2025 Huawei Technologies Co.,Ltd.

Vision SDK is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:

          http://license.coscl.org.cn/MulanPSL2

THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details.
-------------------------------------------------------------------------
Description: python api test.
Author: Vision SDK
Create: 2022
History: NA
"""

import sys
import os
import unittest
import numpy as np
import cv2

from vision_pytest import BaseTestCase
import base
from base import Tensor, Image, Model, ImageProcessor, log, Size, Rect, Point


class TestPostprocess(BaseTestCase):

    def setUp(self):
        base.mx_init()
        self.device_id = 0
        self.image_processor2 = ImageProcessor(self.device_id)
        self.image_processor2 = ImageProcessor(self.device_id)

    def tearDown(self):
        pass

    def test_tensor_dtype(self):
        dtypes = [np.uint8, np.int8, np.int16, np.uint16, np.uint32, np.int32, np.int64, np.uint64,
            np.float16, np.float32, np.double, bool]
        shape = [10, 3, 4, 3]
        for d in dtypes:
            a = np.ones(shape, dtype=d)
            t = Tensor(a)
            b = np.array(t)
            self.assertEqual(t.shape, shape)
            self.assertTrue((a == b).all())
            self.assertEqual(a.dtype, b.dtype)

    def test_tensor_deploy(self):
        shape = (224, 224, 3)
        a = np.array(np.random.rand(*shape), dtype=np.float32)
        t = Tensor(a)
        self.assertEqual(t.device, -1)
        t.to_device(0)
        self.assertEqual(t.device, 0)
        self.assertEqual(t.dtype, base.float32)
        self.assertEqual(t.shape, list(shape))
        t.to_host()
        b = np.array(t)
        self.assertTrue((a == b).all())

    def test_tensor_batch(self):
        item_size = 3
        shape = [224, 224, 3]
        concat_shape = [672, 224, 3]
        stack_shape = [3, 224, 224, 3]

        inputs = []
        tensors = []
        for _ in range(item_size):
            a = np.array(np.random.rand(*shape), dtype=np.float32)
            inputs.append(a)
            t = Tensor(a)
            t.to_device(0)
            tensors.append(t)
        concat_tensor = base.batch_concat(tensors)
        concat_tensor.to_host()
        a = np.concatenate(inputs, axis=0)
        b = np.array(concat_tensor)
        self.assertTrue((a == b).all())

    
    def test_image_processor(self):
        device_id = 0
        resize_height = 224
        resize_width = 224
        original_jpg_path = "./data/test.jpg"
        self.decoded_img = self.image_processor2.decode(original_jpg_path, base.nv12)
        self.assertEqual(self.decoded_img.device, device_id)
        self.assertEqual(self.decoded_img.width, 1280)
        self.assertEqual(self.decoded_img.height, 720)

        size_cof = Size(resize_width, resize_height)
        self.resized_img = self.image_processor2.resize(self.decoded_img, size_cof, base.huaweiu_high_order_filter)
        self.assertEqual(self.resized_img.device, device_id)
        self.assertEqual(self.resized_img.width, 224)
        self.assertEqual(self.resized_img.height, 224)
        self.assertEqual(self.resized_img.format, base.nv12)

        crop_rect_list = [Rect(0, 0, 60, 60), Rect(70, 70, 224, 224)]
        croped_img_list = self.image_processor2.crop(self.resized_img, crop_rect_list)
        self.assertEqual(croped_img_list[0].height, 60)
        self.assertEqual(croped_img_list[1].height, 154)

        crop_input_image_list = [self.resized_img, self.resized_img]
        croped_img_list2 = self.image_processor2.crop(crop_input_image_list, crop_rect_list)
        self.assertEqual(croped_img_list2[0].height, 60)
        self.assertEqual(croped_img_list2[1].height, 154)
        self.assertEqual(croped_img_list2[2].height, 60)
        self.assertEqual(croped_img_list2[3].height, 154)

        rect_size1 = (Rect(0, 0, 600, 600), Size(200, 150))
        rect_size2 = (Rect(200, 200, 1200, 700), Size(300, 200))
        crop_resize_tuple_list = [rect_size1, rect_size2]
        crop_resized_img_list = self.image_processor2.crop_resize(self.decoded_img, crop_resize_tuple_list)
        self.assertEqual(crop_resized_img_list[0].height, 150)
        self.assertEqual(crop_resized_img_list[1].height, 200)


    def test_image_construct(self):
        image_path = "../../../models/imageRaw/test637.jpg"
        decode_image = cv2.imread(image_path)
        original_size = Size(629, 637)
        aligned_size = Size(640, 638)
        image_ori = np.array(decode_image)
        image_ori.transpose((1, 0, 2))
        construct_image_rgb_auto = Image(image_ori, base.rgb)
        tensor = np.array(construct_image_rgb_auto.get_tensor())
        shape = tensor.shape
        self.assertEqual(aligned_size.width, shape[2])
        self.assertEqual(aligned_size.height, shape[1])
        size_info = (original_size, aligned_size)
        construct_image_rgb = Image(image_ori, base.rgb, size_info)
        original_tensor = np.array(construct_image_rgb.get_original_tensor())
        shape = original_tensor.shape
        self.assertEqual(original_size.width, shape[2])
        self.assertEqual(original_size.height, shape[1])
        return True

    def test_model(self):
        device_id = 0
        resize_height = 224
        resize_width = 224
        image_path = "./data/test.jpg"
        model_path = "../../../models/resnet50/resnet50_aipp_tf.om"
        self.decoded_img = self.image_processor2.decode(image_path, base.nv12)
        size_cof = Size(224, 224)
        self.resized_image = self.image_processor2.resize(self.decoded_img, size_cof, base.huaweiu_high_order_filter)

        t = [self.resized_image.to_tensor()]
        
        m = base.model(model_path, deviceId=device_id)
        try:
            outputs = m.infer(t)
        except (AttributeError, RuntimeError) as error_info:
            log.error(error_info)
            return False

        log.info("input_format: %s" % (m.input_format))
        log.info("input_dtype: %s" % (m.input_dtype(0)))
        log.info("output_dtype: %s" % (m.output_dtype(0)))
        log.info("input_shape: %s" % (m.input_shape(0)))
        log.info("output_shape: %s" % (m.output_shape(0)))
        return True


if __name__ == '__main__':
    failed = TestPostprocess.run_tests()
    sys.exit(1 if failed > 0 else 0)
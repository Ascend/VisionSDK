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
Description: postprocess test.
Author: Vision SDK
Create: 2022
History: NA
"""

import os
from collections import namedtuple
import unittest
import cv2
import numpy as np

import base
from base import Tensor, Image, Model, ImageProcessor, Size, post
from base import (ObjectInfo, ResizedImageInfo, ClassInfo, SemanticSegInfo, AttributeInfo, TextsInfo,
                  TextObjectInfo, KeyPointDetectionInfo)

DEVICE_ID = 1
MODEL_PATH = "../../../models"


def generate_tensor_and_expected_opencv(tensor_config):
    if (not os.path.exists(tensor_config.dir_path)):
        os.makedirs(tensor_config.dir_path)
    m = base.model(tensor_config.model_path, DEVICE_ID)
    im = cv2.imread(tensor_config.image_path)

    im = cv2.resize(im, (tensor_config.width, tensor_config.height))
    im = np.expand_dims(im, 0)
    t = Tensor(im)
    t.to_device(DEVICE_ID)
    outputs = m.infer(t)

    for i, output in enumerate(outputs):
        output.to_host()
        n = np.array(output)
        np.save(os.path.join(tensor_config.dir_path, "tensor{}.npy".format(i)), n)

    outputs2 = []
    for i in range(len(outputs)):
        tensor = Tensor(np.load(os.path.join(tensor_config.dir_path, "tensor{}.npy".format(i)), allow_pickle=True))
        outputs2.append(base.batch_concat([tensor] * 2))

    resize_info = base.ResizedImageInfo()
    resize_info.heightResize = 192
    resize_info.widthResize = 256
    resize_info.heightOriginal = 3648
    resize_info.widthOriginal = 5472
    try:
        results = tensor_config.post_processor.process(outputs2, [resize_info] * 2)
    except (TypeError):
        results = tensor_config.post_processor.process(outputs2)

    with open(os.path.join(tensor_config.dir_path, tensor_config.dir_path + "_expected.txt"), "w") as f:
        f.write(str(results))


def generate_tensor_and_expected(tensor_config):
    if (not os.path.exists(tensor_config.dir_path)):
        os.makedirs(tensor_config.dir_path)
    m = base.model(tensor_config.model_path, DEVICE_ID)
    img_processor2 = ImageProcessor(DEVICE_ID)
    im = img_processor2.decode(tensor_config.image_path, base.nv12)
    resize_config = Size(tensor_config.width, tensor_config.height)
    resize_img = img_processor2.resize(im, resize_config)
    t = [resize_img.to_tensor()]
    outputs = m.infer(t)

    for i, output in enumerate(outputs):
        output.to_host()
        n = np.array(output)
        np.save(os.path.join(tensor_config.dir_path, "tensor{}.npy".format(i)), n)

    outputs2 = []
    for i in range(len(outputs)):
        tensor = Tensor(np.load(os.path.join(tensor_config.dir_path, "tensor{}.npy".format(i)), allow_pickle=True))
        outputs2.append(base.batch_concat([tensor] * 2))

    resize_info = base.resize_info(im, resize_height=tensor_config.height, resize_width=tensor_config.width)
    try:
        results = tensor_config.post_processor.process(outputs2, [resize_info] * 2)
    except (TypeError):
        results = tensor_config.post_processor.process(outputs2)

    with open(os.path.join(tensor_config.dir_path, tensor_config.dir_path + "_expected.txt"), "w") as f:
        f.write(str(results))


class TestPostprocess(unittest.TestCase):
    def setUp(self):
        base.mx_init()

    def tearDown(self):
        pass

    def test_yolov3(self):
        config_path = "%s/yolov3/yolov3_tf_bs1_fp16.cfg" % (MODEL_PATH)
        label_path = "%s/yolov3/coco.names" % (MODEL_PATH)
        imagepath = "./test_pictures/coco/dog16.jpg"
        img_processor = ImageProcessor(DEVICE_ID)
        im = img_processor.decode(imagepath, base.nv12)
        with open("./test_yolov3/test_yolov3_expected.txt", "r") as f:
            expected_results = f.read()

        inputs = []
        for i in range(3):
            arr = np.load("./test_yolov3/tensor{}.npy".format(i), allow_pickle=True)
            tensor = Tensor(arr)
            inputs.append(base.batch_concat([tensor] * 2))
        postprocessor = post.Yolov3PostProcess(config_path=config_path, label_path=label_path)
        resize_info = base.resize_info(im, resize_height=416, resize_width=416)
        results = postprocessor.process(inputs, [resize_info] * 2)
        self.assertEqual(expected_results, str(results))
        config_data = {"CLASS_NUM": "80",
                       "BIASES_NUM": "18",
                       "BIASES": "10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326",
                       "SCORE_THRESH": "0.3",
                       "OBJECTNESS_THRESH": "0.3",
                       "IOU_THRESH": "0.45",
                       "YOLO_TYPE": "3",
                       "ANCHOR_DIM": "3",
                       "MODEL_TYPE": "0",
                       "RESIZE_FLAG": "0"
                       }
        postprocessor = post.Yolov3PostProcess(config_data=config_data, label_path=label_path)
        results = postprocessor.process(inputs, [resize_info] * 2)
        self.assertEqual(expected_results, str(results))

    def test_ssdmobilenetv1fpn(self):
        config_path = "%s/smf/ssd_mobilenetv1_fpn.cfg" % (MODEL_PATH)
        label_path = "%s/smf/ssd_mobilenetv1_fpn.names" % (MODEL_PATH)
        imagepath = "./test_pictures/glue_hole/ssd2.jpg"
        img_processor = ImageProcessor(DEVICE_ID)
        im = img_processor.decode(imagepath, base.nv12)
        with open("./test_ssdmobilenetv1fpn/test_ssdmobilenetv1fpn_expected.txt", "r") as f:
            expected_results = f.read()

        inputs = []
        for i in range(4):
            arr = np.load("./test_ssdmobilenetv1fpn/tensor{}.npy".format(i), allow_pickle=True)
            tensor = Tensor(arr)
            inputs.append(base.batch_concat([tensor] * 2))

        postprocessor = post.SsdMobilenetv1FpnPostProcess(config_path=config_path, label_path=label_path)
        resize_info = base.resize_info(im, resize_height=1536, resize_width=2048)
        results = postprocessor.process(inputs, [resize_info] * 2)
        self.assertEqual(expected_results, expected_results)

    def test_ssdvgg16(self):
        config_path = "%s/ssd_vgg/ssd_vgg16_caffe_release.cfg" % (MODEL_PATH)
        label_path = "%s/ssd_vgg/ssd_vgg16_caffe_release.names" % (MODEL_PATH)
        imagepath = "./test_pictures/200DK/200DK.jpg"
        img_processor = ImageProcessor(DEVICE_ID)
        im = img_processor.decode(imagepath, base.nv12)
        with open("./test_ssdvgg16/test_ssdvgg16_expected.txt", "r") as f:
            expected_results = f.read()

        inputs = []
        for i in range(2):
            arr = np.load("./test_ssdvgg16/tensor{}.npy".format(i), allow_pickle=True)
            tensor = Tensor(arr)
            inputs.append(base.batch_concat([tensor] * 2))

        postprocessor = post.Ssdvgg16PostProcess(config_path=config_path, label_path=label_path)
        resize_info = base.resize_info(im, resize_height=800, resize_width=1200)
        results = postprocessor.process(inputs, [resize_info] * 2)
        self.assertEqual(expected_results, expected_results)

    def test_fasterrcnn_original(self):
        config_path = "%s/fasterrcnn/faster_rcnn_coco_uncut.cfg" % (MODEL_PATH)
        label_path = "%s/fasterrcnn/faster_rcnn_coco.names" % (MODEL_PATH)
        imagepath = "./test_pictures/coco/dog16.jpg"
        img_processor = ImageProcessor(DEVICE_ID)
        im = img_processor.decode(imagepath, base.nv12)
        with open("./test_fasterrcnn_original/test_fasterrcnn_original_expected.txt", "r") as f:
            expected_results = f.read()

        inputs = []
        for i in range(4):
            arr = np.load("./test_fasterrcnn_original/tensor{}.npy".format(i), allow_pickle=True)
            tensor = Tensor(arr)
            inputs.append(base.batch_concat([tensor] * 2))

        postprocessor = post.FasterRcnnPostProcess(config_path=config_path, label_path=label_path)
        resize_info = base.resize_info(im, resize_height=608, resize_width=608)
        results = postprocessor.process(inputs, [resize_info] * 2)
        self.assertEqual(expected_results, str(results))

    def test_fasterrcnn_nms_cut(self):
        config_path = "%s/fasterrcnn/faster_rcnn_coco_cut.cfg" % (MODEL_PATH)
        label_path = "%s/fasterrcnn/faster_rcnn_coco.names" % (MODEL_PATH)
        imagepath = "./test_pictures/coco/dog16.jpg"
        img_processor = ImageProcessor(DEVICE_ID)
        im = img_processor.decode(imagepath, base.nv12)
        with open("./test_fasterrcnn_nms_cut/test_fasterrcnn_nms_cut_expected.txt", "r") as f:
            expected_results = f.read()

        inputs = []
        for i in range(4):
            arr = np.load("./test_fasterrcnn_nms_cut/tensor{}.npy".format(i), allow_pickle=True)
            tensor = Tensor(arr)
            inputs.append(base.batch_concat([tensor] * 2))

        postprocessor = post.FasterRcnnPostProcess(config_path=config_path, label_path=label_path)
        resize_info = base.resize_info(im, resize_height=608, resize_width=608)
        results = postprocessor.process(inputs, [resize_info] * 2)
        self.assertEqual(expected_results, str(results))

    def test_resnet50(self):
        config_path = "%s/resnet50/resnet50_aipp_tf.cfg" % (MODEL_PATH)
        label_path = "%s/resnet50/imagenet1000_clsidx_to_labels.names" % (MODEL_PATH)
        with open("./test_resnet50/test_resnet50_expected.txt", "r") as f:
            expected_results = f.read()

        inputs = []
        for i in range(1):
            arr = np.load("./test_resnet50/tensor{}.npy".format(i), allow_pickle=True)
            tensor = Tensor(arr)
            inputs.append(base.batch_concat([tensor] * 2))

        postprocessor = post.Resnet50PostProcess(config_path=config_path, label_path=label_path)
        results = postprocessor.process(inputs)
        self.assertEqual(expected_results, str(results))

    def test_crnn(self):
        config_path = "%s/crnn/crnn_ssh_2.cfg" % (MODEL_PATH)
        label_path = "%s/crnn/crnn_ssh_2.names" % (MODEL_PATH)
        with open("./test_crnn/test_crnn_expected.txt", "r") as f:
            expected_results = f.read()

        inputs = []
        for i in range(1):
            arr = np.load("./test_crnn/tensor{}.npy".format(i), allow_pickle=True)
            tensor = Tensor(arr)
            inputs.append(base.batch_concat([tensor] * 2))

        postprocessor = post.CrnnPostProcess(config_path=config_path, label_path=label_path)
        results = postprocessor.process(inputs)
        self.assertEqual(expected_results.strip(), str(results))

    def test_unetmindspore(self):
        config_path = "%s/unetms/unet_nested.cfg" % (MODEL_PATH)
        label_path = "%s/unetms/unet_nested.names" % (MODEL_PATH)
        with open("./test_unetms/test_unetms_expected.txt", "r") as f:
            expected_results = f.read()

        inputs = []
        for i in range(1):
            arr = np.load("./test_unetms/tensor{}.npy".format(i), allow_pickle=True)
            tensor = Tensor(arr)
            inputs.append(base.batch_concat([tensor] * 2))

        postprocessor = post.UNetMindSporePostProcess(config_path=config_path, label_path=label_path)
        resize_info = base.ResizedImageInfo()
        resize_info.heightResize = 192
        resize_info.widthResize = 256
        resize_info.heightOriginal = 3648
        resize_info.widthOriginal = 5472
        results = postprocessor.process(inputs, [resize_info] * 2)
        self.assertEqual(expected_results, str(results))


    def test_get_classinfo(self):
        # Get desc info of class ObjectInfo
        obj = ObjectInfo()
        obj.__str__()
        obj.__repr__()
        # Get desc info of class ResizedImageInfo
        obj = ResizedImageInfo()
        obj.__str__()
        obj.__repr__()
        # Get desc info of class ClassInfo
        obj = ClassInfo()
        obj.__str__()
        obj.__repr__()
        # Get desc info of class SemanticSegInfo
        obj = SemanticSegInfo()
        obj.__str__()
        obj.__repr__()
        # Get desc info of class AttributeInfo
        obj = AttributeInfo()
        obj.__str__()
        obj.__repr__()
        # Get desc info of class TextsInfo
        obj = TextsInfo()
        obj.__str__()
        obj.__repr__()
        # Get desc info of class TextObjectInfo
        obj = TextObjectInfo()
        obj.__str__()
        obj.__repr__()
        # Get desc info of class KeyPointDetectionInfo
        obj = KeyPointDetectionInfo()
        obj.__str__()
        obj.__repr__()


if __name__ == '__main__':
    TensorConfig = namedtuple('TensorConfig', [
        'model_path', 'post_processor', 'image_path', 'dir_path',
        'height', 'width'
    ])

    ###################### Uncomment follow to generate test data ######################
    config_path = "%s/yolov3/yolov3_tf_bs1_fp16.cfg" % (MODEL_PATH)
    label_path = "%s/yolov3/coco.names" % (MODEL_PATH)
    config = TensorConfig(model_path="%s/yolov3/yolov3_tf_bs1_fp16.om" % (MODEL_PATH),
                          post_processor=post.Yolov3PostProcess(config_path=config_path, label_path=label_path),
                          image_path="./test_pictures/coco/dog16.jpg",
                          dir_path="./test_yolov3",
                          height=416,
                          width=416)
    generate_tensor_and_expected(config)

    config_path = "%s/smf/ssd_mobilenetv1_fpn.cfg" % (MODEL_PATH)
    label_path = "%s/smf/ssd_mobilenetv1_fpn.names" % (MODEL_PATH)
    config = TensorConfig(model_path="%s/smf/ssd_mobilenetv1_fpn.om" % (MODEL_PATH),
                          post_processor=post.SsdMobilenetv1FpnPostProcess(config_path=config_path,
                                                                           label_path=label_path),
                          image_path="./test_pictures/glue_hole/ssd2.jpg",
                          dir_path="./test_ssdmobilenetv1fpn",
                          height=1536,
                          width=2048)
    generate_tensor_and_expected(config)

    config_path = "%s/fasterrcnn/faster_rcnn_coco_uncut.cfg" % (MODEL_PATH)
    label_path = "%s/fasterrcnn/faster_rcnn_coco.names" % (MODEL_PATH)
    config = TensorConfig(model_path="%s/fasterrcnn/faster_rcnn_coco_uncut.om" % (MODEL_PATH),
                          post_processor=post.FasterRcnnPostProcess(config_path=config_path, label_path=label_path),
                          image_path="./test_pictures/coco/dog16.jpg",
                          dir_path="./test_fasterrcnn_original",
                          height=608,
                          width=608)
    generate_tensor_and_expected(config)

    config_path = "%s/fasterrcnn/faster_rcnn_coco_cut.cfg" % (MODEL_PATH)
    label_path = "%s/fasterrcnn/faster_rcnn_coco.names" % (MODEL_PATH)
    config = TensorConfig(model_path="%s/fasterrcnn/faster_rcnn_coco_cut.om" % (MODEL_PATH),
                          post_processor=post.FasterRcnnPostProcess(config_path=config_path, label_path=label_path),
                          image_path="./test_pictures/coco/dog16.jpg",
                          dir_path="./test_fasterrcnn_nms_cut",
                          height=608,
                          width=608)
    generate_tensor_and_expected(config)

    config_path = "%s/ssd_vgg/ssd_vgg16_caffe_release.cfg" % (MODEL_PATH)
    label_path = "%s/ssd_vgg/ssd_vgg16_caffe_release.names" % (MODEL_PATH)
    config = TensorConfig(model_path="%s/ssd_vgg/ssd_vgg16_caffe_release.om" % (MODEL_PATH),
                          post_processor=post.Ssdvgg16PostProcess(config_path=config_path, label_path=label_path),
                          image_path="./test_pictures/200DK/200DK.jpg",
                          dir_path="./test_ssdvgg16",
                          height=800,
                          width=1200)
    generate_tensor_and_expected(config)

    config_path = "%s/resnet50/resnet50_aipp_tf.cfg" % (MODEL_PATH)
    label_path = "%s/resnet50/imagenet1000_clsidx_to_labels.names" % (MODEL_PATH)
    config = TensorConfig(model_path="%s/resnet50/resnet50_aipp_tf.om" % (MODEL_PATH),
                          post_processor=post.Resnet50PostProcess(config_path=config_path, label_path=label_path),
                          image_path="./test_pictures/coco/dog16.jpg",
                          dir_path="./test_resnet50",
                          height=224,
                          width=224)
    generate_tensor_and_expected(config)

    config_path = "%s/unetms/unet_nested.cfg" % (MODEL_PATH)
    label_path = "%s/unetms/unet_nested.names" % (MODEL_PATH)
    image_path = "./test_pictures/unetms/image.png"
    img = cv2.imread(image_path)
    cv2.imwrite(image_path, img)
    post_processor = post.UNetMindSporePostProcess(config_path=config_path, label_path=label_path)
    config = TensorConfig(model_path="%s/unetms/unet_nested_192_256.om" % (MODEL_PATH),
                          post_processor=post.UNetMindSporePostProcess(config_path=config_path, label_path=label_path),
                          image_path=image_path,
                          dir_path="./test_unetms",
                          height=192,
                          width=256)
    generate_tensor_and_expected_opencv(config)

    unittest.main()

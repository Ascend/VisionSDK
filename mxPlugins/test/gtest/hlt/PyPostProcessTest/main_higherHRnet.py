#!/usr/bin/env python
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
Description: main of HigherHRnet
Author: Vision SDK
Create: 2022
History: NA
"""

import sys
import logging

import numpy as np
import MxpiDataType_pb2 as MxpiDataType
from StreamManagerApi import StreamManagerApi, MxProtobufIn, InProtobufVector

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
    if (len(sys.argv) < 2):
        logging.error("Please input full path of pipeline.")
        exit()

    pipeline_path = sys.argv[1]
    resize_img_path = b'/home/simon/models/higherHRnet/ResizedImg.npy'
    img_shape_path = b'/home/simon/models/higherHRnet/ImgShape.npy'
    stream_name = b'pose'

    stream_manager_api = StreamManagerApi()
    ret = stream_manager_api.InitManager()
    if ret != 0:
        logging.error("Failed to init Stream manager, ret=%s" % str(ret))
        exit()

    with open(pipeline_path, 'rb') as f:
        pipeline_str = f.read()
    ret = stream_manager_api.CreateMultipleStreams(pipeline_str)
    if ret != 0:
        logging.error("Failed to create Stream, ret=%s" % str(ret))
        exit()
    scale_array = (512, 576, 640, 704, 768, 832, 896, 960, 1024)

    img_resized, img_shape = np.load(resize_img_path), np.load(img_shape_path)
    
    vision_list = MxpiDataType.MxpiVisionList()
    vision_vec = vision_list.visionVec.add()
    vision_vec.visionInfo.format = 0
    vision_vec.visionInfo.width = img_shape[1]
    vision_vec.visionInfo.height = img_shape[0]
    vision_vec.visionInfo.widthAligned = img_resized.shape[2]
    vision_vec.visionInfo.heightAligned = img_resized.shape[1]
    
    vision_vec.visionData.memType = 0
    vision_vec.visionData.dataStr = img_resized.tobytes()
    vision_vec.visionData.dataSize = len(img_resized)
    buf_type = b"MxTools.MxpiVisionList"
    
    in_plugin_id = 0
    protobuf = MxProtobufIn()
    element_name = "appsrc0"
    protobuf.key = element_name.encode("utf-8")
    protobuf.type = buf_type
    protobuf.protobuf = vision_list.SerializeToString()
    protobuf_vec = InProtobufVector()
    protobuf_vec.push_back(protobuf)
    stream_manager_api.SendProtobuf(stream_name, in_plugin_id, protobuf_vec)

    infer_result = stream_manager_api.GetResult(stream_name, in_plugin_id, 3000)
    if infer_result.errorCode != 0:
        logging.error("GetResult failed.")
        exit()

    logging.info(infer_result.data.decode())

    # destroy streams
    stream_manager_api.DestroyAllStreams()
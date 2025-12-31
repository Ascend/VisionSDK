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
Description: class list test.
Author: Vision SDK
Create: 2021
History: NA
"""

import json
import logging
from StreamManagerApi import StreamManagerApi, MxDataInput, MxProtobufIn, InProtobufVector, StringVector
import MxpiDataType_pb2 as MxpiDataType
if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
    # init stream manager
    streamManagerApi = StreamManagerApi()
    ret = streamManagerApi.InitManager()
    if ret != 0:
        logging.error("Failed to init Stream manager, ret=%s" % str(ret))
        exit()

    # create streams by pipeline config file
    pipeline = {
        "classification+detection": {
            "stream_config": {
                "deviceId": "0"
            },
            "mxpi_imagedecoder0": {
                "factory": "mxpi_imagedecoder",
                "next": "mxpi_imageresize0"
            },
            "mxpi_imageresize0": {
                "props": {
                    "dataSource": "mxpi_imagedecoder0",
                    "resizeHeight": "416",
                    "resizeWidth": "416"
                },
                "factory": "mxpi_imageresize",
                "next": "mxpi_modelinfer0"
            },
            "mxpi_modelinfer0": {
                "props": {
                    "dataSource": "mxpi_imageresize0",
                    "modelPath": "../models/yolov3/yolov3_tf_bs1_fp16.om",
                    "postProcessConfigPath": "../models/yolov3/yolov3_tf_bs1_fp16.cfg",
                    "labelPath": "../models/yolov3/coco.names",
                    "postProcessLibPath": "../../../lib/libMpYOLOv3PostProcessor.so"
                },
                "factory": "mxpi_modelinfer",
                "next": "mxpi_imagecrop0"
            },
            "mxpi_imagecrop0": {
                "props": {
                    "dataSource": "mxpi_modelinfer0"
                },
                "factory": "mxpi_imagecrop",
                "next": "mxpi_imageresize1"
            },
            "mxpi_imageresize1": {
                "props": {
                    "dataSource": "mxpi_imagecrop0"
                },
                "factory": "mxpi_imageresize",
                "next": "mxpi_modelinfer1"
            },
            "mxpi_modelinfer1": {
                "props": {
                    "dataSource": "mxpi_imageresize1",
                    "modelPath": "../models/resnet50/resnet50_aipp_tf.om",
                    "postProcessConfigPath": "../models/resnet50/resnet50_aipp_tf.cfg",
                    "labelPath": "../models/resnet50/imagenet1000_clsidx_to_labels.names",
                    "postProcessLibPath": "../../../lib/libresnet50postprocessor.so"
                },
                "factory": "mxpi_modelinfer",
                "next": "mxpi_dataserialize0"
            },
            "mxpi_dataserialize0": {
                "props": {
                    "outputDataKeys": "mxpi_modelinfer0,mxpi_modelinfer1"
                },
                "factory": "mxpi_dataserialize",
                "next": "appsink0"
            },
            "appsrc0": {
                "props": {
                    "blocksize": "409600"
                },
                "factory": "appsrc",
                "next": "mxpi_imagedecoder0"
            },
            "appsink0": {
                "props": {
                    "blocksize": "4096000"
                },
                "factory": "appsink"
            }
        }
    }
    pipelineStr = json.dumps(pipeline).encode()	
    ret = streamManagerApi.CreateMultipleStreams(pipelineStr)
    if ret != 0:
        logging.error("Failed to create Stream, ret=%s" % str(ret))
        exit()

    # Construct the input of the stream
    dataInput = MxDataInput()
    with open("test.jpg", 'rb') as f:
        dataInput.data = f.read()

    # The following is how to set the dataInput.roiBoxs
    """
    roiVector = RoiBoxVector()
    roi = RoiBox()
    roi.x0 = 100
    roi.y0 = 100
    roi.x1 = 200
    roi.y1 = 200
    roiVector.push_back(roi)
    dataInput.roiBoxs = roiVector
    """
    # Inputs data to a specified stream based on streamName.
    streamName = b'classification+detection'
    inPluginId = 0
    key = b'mxpi_modelinfer1'    
    objectList = MxpiDataType.MxpiClassList()
    mxpiClass = objectList.classVec.add()
    mxpiMetaHeader = mxpiClass.headerVec.add()
    mxpiMetaHeader.memberId = 1;
    mxpiClass.classId = 1
    mxpiClass.className = b'people'
    mxpiClass.confidence = 0.1
    protobuf = MxProtobufIn()
    protobuf.key = key
    protobuf.type = b'MxTools.MxpiClassList'
    protobuf.protobuf = objectList.SerializeToString()
    logging.info(protobuf.key)
    logging.info(protobuf.protobuf)
   
    protobufVec = InProtobufVector()
    protobufVec.push_back(protobuf)
    logging.info(streamName)
    
    # Inputs data to a specified stream based on streamName.
    uniqueId = streamManagerApi.SendData(streamName, inPluginId, dataInput)
    if uniqueId < 0:
        logging.error("Failed to send data to stream.")
        exit()
    keyVec = StringVector()
    keyVec.push_back(key)
    inferResult = streamManagerApi.GetProtobuf(streamName, inPluginId, keyVec)
    if inferResult.size() == 0:
        logging.error("inferResult is null")
        exit()
    if inferResult[0].errorCode != 0:
        logging.error("GetResultWithUniqueId error. errorCode=%d" % (
        inferResult[0].errorCode))
        exit()
    #print the infer result
    logging.info("GetProtobuf errorCode=%d" % (inferResult[0].errorCode))
    logging.info("key:" + str(inferResult[0].messageName))
    result = MxpiDataType.MxpiClassList()
    result.ParseFromString(inferResult[0].messageBuf)
    logging.info("value:" + str(result))

    # destroy streams
    streamManagerApi.DestroyAllStreams()

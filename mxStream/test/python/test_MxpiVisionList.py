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
Description: visionlist test.
Author: Vision SDK
Create: 2021
History: NA
"""

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
    with open("../pipeline/test.pipeline", 'rb') as f:
        pipelineStr = f.read()
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
    key = b'mxpi_modelinfer2'    
    visionList = MxpiDataType.MxpiVisionList()
    visionVec = visionList.visionVec.add() 
    visionVec.visionData.deviceId = 0
    visionVec.visionData.memType = 0
    visionVec.visionData.dataStr = dataInput.data
    protobuf = MxProtobufIn()
    protobuf.key = key
    protobuf.type = b'MxTools.MxpiVisionList'
    protobuf.protobuf = visionList.SerializeToString()
   
    protobufVec = InProtobufVector()
    protobufVec.push_back(protobuf)
    errorCode = streamManagerApi.SendProtobuf(streamName, inPluginId, protobufVec)
    if errorCode < 0:
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
    logging.info("key:"+str(inferResult[0].messageName))
    result = MxpiDataType.MxpiVisionList()
    result.ParseFromString(inferResult[0].messageBuf)
    with open("result.jpg", "wb") as fo:
        fo.write(result.visionVec[0].visionData.dataStr)
    # Inputs data to a specified stream based on streamName.
    uniqueId = streamManagerApi.SendData(streamName, inPluginId, dataInput)
    if uniqueId < 0:
        logging.error("Failed to send data to stream.")
        exit()
    # Obtain the inference result by specifying streamName and uniqueId.
    inferResult = streamManagerApi.GetResult(streamName, inPluginId)
    if inferResult.errorCode != 0:
        logging.error("GetResultWithUniqueId error. errorCode=%d, errorMsg=%s" % (
            inferResult.errorCode, inferResult.data.decode()))
        exit()


    # destroy streams
    streamManagerApi.DestroyAllStreams()

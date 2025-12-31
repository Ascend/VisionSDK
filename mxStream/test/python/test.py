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
Description: test.
Author: Vision SDK
Create: 2021
History: NA
"""

import logging
import json
from StreamManagerApi import StreamManagerApi, MxDataInput, RoiBoxVector, RoiBox


logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
pipeline = {
    "detection": {
        "appsrc0": {
            "props": {
                "blocksize": "409600"
            },
            "factory": "appsrc",
            "next": "appsink0"
        },
        "appsink0": {
            "factory": "appsink"
        }
    }
}
pipelineStr = json.dumps(pipeline).encode()
STREAM_NAME = b'detection'

moderInferApi = StreamManagerApi()
moderInferApi.InitManager()
moderInferApi.CreateMultipleStreams(pipelineStr)

dataInput = MxDataInput()
dataInput.data = b'hahahahahahaahahha'
dataInput.fragmentId = 2

roiVector = RoiBoxVector()
roi = RoiBox()
roi.x0 = 100
roi.y0 = 100
roi.x1 = 200
roi.y1 = 200
roiVector.push_back(roi)
dataInput.roiBoxs = roiVector



for _ in range(1, 5):
    uniqueId = moderInferApi.SendDataWithUniqueId(STREAM_NAME, 0, dataInput)
    if uniqueId < 0:
        logging.error("SendDataWithUniqueId error.")
        continue

    dataOutPut = moderInferApi.GetResultWithUniqueId(STREAM_NAME, uniqueId, 5000)
    if dataOutPut.errorCode != 0:
        logging.error("GetResultWithUniqueId error. errorCode=%d, errorMsg=%s" %
              (dataOutPut.errorCode, dataOutPut.data.decode()))
    logging.info(dataOutPut.data.decode())

moderInferApi.DestroyAllStreams()

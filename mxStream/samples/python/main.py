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
Description: main.
Author: Vision SDK
Create: 2021
History: NA
"""

import os
import logging

from StreamManagerApi import StreamManagerApi, MxDataInput


def check_file(file_path):
    if os.path.islink(file_path):
        logging.error("Error", 'Path of file cannot be a soft link!')
        return False
    real_file_path = os.path.realpath(file_path)
    if not isinstance(real_file_path, str) or not real_file_path:
        logging.error("Error", 'Path of file is not a valid path!')
        return False
    if not os.path.exists(real_file_path):
        logging.error("Error", 'The file not exists!')
        return False
    if not os.access(real_file_path, mode=os.R_OK):
        logging.error("Error", 'Please check if the file is readable.')
        return False
    file_size = os.path.getsize(real_file_path)
    if file_size <= 0 or file_size > 10 * 1024 * 1024:
        logging.error("Error", 'The file is empty or too large.')
        return False
    return True

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
    # init stream manager
    try:
        streamManagerApi = StreamManagerApi()
        ret = streamManagerApi.InitManager()
        if ret != 0:
            logging.error("Failed to init Stream manager, ret=%s" % str(ret))
            exit()

        # create streams by pipeline config file
        if not check_file("../pipeline/Sample.pipeline"):
            streamManagerApi.DestroyAllStreams()
            logging.error("Invalid pipeline file.")
            exit()
        with open("../pipeline/Sample.pipeline", 'rb') as f:
            pipelineStr = f.read()
        ret = streamManagerApi.CreateMultipleStreams(pipelineStr)
        if ret != 0:
            streamManagerApi.DestroyAllStreams()
            logging.error("Failed to create Stream, ret=%s" % str(ret))
            exit()

        # Construct the input of the stream
        dataInput = MxDataInput()
        if not check_file("test.jpg"):
            logging.error("Invalid image file.")
            streamManagerApi.DestroyAllStreams()
            exit()
        with open("test.jpg", 'rb') as f:
            dataInput.data = f.read()

        # Inputs data to a specified stream based on streamName.
        streamName = b'classification+detection'
        inPluginId = 0
        uniqueId = streamManagerApi.SendDataWithUniqueId(streamName, inPluginId, dataInput)
        if uniqueId < 0:
            logging.error("Failed to send data to stream.")
            streamManagerApi.DestroyAllStreams()
            exit()

        # Obtain the inference result by specifying streamName and uniqueId.
        inferResult = streamManagerApi.GetResultWithUniqueId(streamName, uniqueId, 3000)
        if inferResult.errorCode != 0:
            logging.error("GetResultWithUniqueId error. errorCode=%d, errorMsg=%s" % (
                inferResult.errorCode, inferResult.data.decode()))
            streamManagerApi.DestroyAllStreams()
            exit()

        # print the infer result
        logging.info(inferResult.data.decode())

        # destroy streams
        streamManagerApi.DestroyAllStreams()
    except Exception as e:
        logging.error('Exception has been catched from this demo.')

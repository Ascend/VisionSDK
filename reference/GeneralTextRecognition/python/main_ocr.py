#!/usr/bin/env python
# coding=utf-8

"""
Copyright 2021 Huawei Technologies Co., Ltd

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
"""

import os
import json
import sys

from StreamManagerApi import StreamManagerApi, MxDataInput

MAX_FILE_SIZE = 1024 * 1024 * 10  # 10MB
MAX_IMAGE_SIZE = 1024 * 1024 * 1024  # 1G
PATH = "../data/OCR.pipeline"

if __name__ == '__main__':
    # init stream manager
    STREAM_NAME = b'OCR'

    # pylint: disable=duplicate-code
    streamManagerApi = StreamManagerApi()
    ret = streamManagerApi.InitManager()
    if ret != 0:
        print("Failed to init Stream manager, ret=%s" % str(ret))
        sys.exit(-1)

    # create streams by pipeline config file
    if os.path.getsize(PATH) <= 0 or os.path.getsize(PATH) > MAX_FILE_SIZE:
        print("Pipeline file size invalid!")
        sys.exit(-1)
    with open(PATH, 'rb') as f:
        pipelineStr = f.read()
    ret = streamManagerApi.CreateMultipleStreams(pipelineStr)
    if ret != 0:
        print("Failed to create Stream, ret=%s" % str(ret))
        sys.exit(-1)
    # pylint: enable=duplicate-code

    # Inputs data to a specified stream based on streamName.
    IN_PLUGIN_ID = 0
    # Construct the input of the stream
    dataInput = MxDataInput()

    DIR_NAME = '../input_data/'

    file_list = os.listdir(DIR_NAME)
    file_list.sort()
    for file_name in file_list:
        lower_file_name = file_name.lower()
        if lower_file_name.endswith(".jpeg") or lower_file_name.endswith(".jpg") or lower_file_name.endswith(".png"):
            img_path = os.path.join(DIR_NAME, file_name)
            print("img_path: ", img_path)
            if os.path.getsize(img_path) <= 0 or os.path.getsize(img_path) > MAX_IMAGE_SIZE:
                print("Image file size invalid!")
                sys.exit(-1)
            with open(img_path, 'rb') as f:
                dataInput.data = f.read()

            # Inputs data to a specified stream based on streamName.
            IN_PLUGIN_ID = 0
            uniqueId = streamManagerApi.SendDataWithUniqueId(STREAM_NAME, IN_PLUGIN_ID, dataInput)
            if uniqueId < 0:
                print("Failed to send data to stream.")
                sys.exit(-1)

            # Obtain the inference result by specifying streamName and uniqueId.
            inferResult = streamManagerApi.GetResultWithUniqueId(STREAM_NAME, uniqueId, 30000)
            if inferResult.errorCode != 0:
                print(
                    "GetResultWithUniqueId error. errorCode=%d, errorMsg=%s"
                    % (inferResult.errorCode, inferResult.data.decode())
                )
                sys.exit(-1)

            # print the infer result
            print(inferResult.data.decode())
            results = json.loads(inferResult.data.decode())

    # destroy streams
    streamManagerApi.DestroyAllStreams()

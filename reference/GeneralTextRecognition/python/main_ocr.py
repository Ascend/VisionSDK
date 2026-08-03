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
import sys

from StreamManagerApi import StreamManagerApi, MxDataInput

MAX_FILE_SIZE = 1024 * 1024 * 10  # 10MB
MAX_IMAGE_SIZE = 1024 * 1024 * 1024  # 1G
SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
PROJECT_ROOT = os.path.dirname(SCRIPT_DIR)
PIPELINE_PATH = os.path.join(PROJECT_ROOT, "data", "OCR.pipeline")
DIR_NAME = os.path.join(PROJECT_ROOT, "input_data")
SUPPORTED_IMAGE_SUFFIXES = (".jpg", ".jpeg", ".png")

if __name__ == '__main__':
    # init stream manager
    STREAM_NAME = b'OCR'

    # pylint: disable=duplicate-code
    streamManagerApi = StreamManagerApi()
    ret = streamManagerApi.InitManager()
    if ret != 0:
        print("Failed to init Stream manager, ret=%s" % str(ret))
        sys.exit(1)

    # create streams by pipeline config file
    if not os.path.isfile(PIPELINE_PATH):
        print("Pipeline file does not exist: %s" % PIPELINE_PATH)
        sys.exit(1)
    if os.path.getsize(PIPELINE_PATH) <= 0 or os.path.getsize(PIPELINE_PATH) > MAX_FILE_SIZE:
        print("Pipeline file size invalid!")
        sys.exit(1)
    with open(PIPELINE_PATH, 'rb') as f:
        pipelineStr = f.read()
    # replace project root placeholder in pipeline config
    pipelineStr = pipelineStr.replace(b"<Project_Root>", PROJECT_ROOT.encode())
    ret = streamManagerApi.CreateMultipleStreams(pipelineStr)
    if ret != 0:
        print("Failed to create Stream, ret=%s" % str(ret))
        sys.exit(1)
    # pylint: enable=duplicate-code

    # Inputs data to a specified stream based on streamName.
    IN_PLUGIN_ID = 0
    # Construct the input of the stream
    dataInput = MxDataInput()

    try:
        if not os.path.isdir(DIR_NAME):
            print("Input directory does not exist: %s" % DIR_NAME)
            sys.exit(1)
        file_list = os.listdir(DIR_NAME)
        file_list.sort()
        image_list = [file_name for file_name in file_list if file_name.lower().endswith(SUPPORTED_IMAGE_SUFFIXES)]
        if not image_list:
            print("No supported JPG, JPEG or PNG images found in %s" % DIR_NAME)
            sys.exit(1)

        for file_name in image_list:
            img_path = os.path.join(DIR_NAME, file_name)
            print("img_path: ", img_path)
            if os.path.getsize(img_path) <= 0 or os.path.getsize(img_path) > MAX_IMAGE_SIZE:
                print("Image file size invalid!")
                sys.exit(1)
            with open(img_path, 'rb') as f:
                dataInput.data = f.read()

            uniqueId = streamManagerApi.SendDataWithUniqueId(STREAM_NAME, IN_PLUGIN_ID, dataInput)
            if uniqueId < 0:
                print("Failed to send data to stream.")
                sys.exit(1)

            # Obtain the inference result by specifying streamName and uniqueId.
            inferResult = streamManagerApi.GetResultWithUniqueId(STREAM_NAME, uniqueId, 30000)
            if inferResult.errorCode != 0:
                print(
                    "GetResultWithUniqueId error. errorCode=%d, errorMsg=%s"
                    % (inferResult.errorCode, inferResult.data.decode())
                )
                sys.exit(1)

            # print the infer result
            print(inferResult.data.decode())
    finally:
        streamManagerApi.DestroyAllStreams()

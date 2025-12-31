#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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
Description: Stream server derived from StreamManagerApi.
Author: Vision SDK
Create: 2024
History: NA
"""
import os
import time
from StreamManagerApi import StreamManagerApi, MxDataInput

from server_options_and_logger import logger as logging
from server_options_and_logger import server_option_instance
from tensor import Tensor
from utils import file_base_check


class StreamServer:
    def __init__(self):
        self.mx_stream_manager = StreamManagerApi()
        if self.mx_stream_manager.InitManager() != 0:
            logging.error("Failed to init Stream manager.")
            raise Exception("Failed to init Stream manager.")
        self.infer_configs = {}

    @staticmethod
    def _inference(server_manager, request, task_id):
        mx_stream_manager = server_manager.stream_server.mx_stream_manager
        infer_config = server_manager.stream_server.infer_configs[task_id]
        stream_name = infer_config["name"].encode("utf-8")
        input_ids = request.get_input_ids()
        if len(input_ids) == 0:
            logging.error("There is no input of stream!")
            raise Exception("There is no input of stream!")
        # Construct the input of the stream
        for i in input_ids:
            data_input = MxDataInput()
            data_input.data = request.get_input(i).data_
            unique_id = mx_stream_manager.SendDataWithUniqueId(stream_name, 0, data_input)
            if unique_id < -1:
                logging.error("Failed to send data to stream.")
                raise Exception("Failed to send data to stream.")
            infer_result = mx_stream_manager.GetResultWithUniqueId(
                stream_name, unique_id, server_option_instance.infer_configs[task_id]["timeoutMs"])
            if infer_result.errorCode != 0:
                logging.error("GetResultWithUniqueId error. errorCode=%d", infer_result.errorCode)
                raise Exception("GetResultWithUniqueId error. errorCode=%d", infer_result.errorCode)
            output_tensor = Tensor()
            if not output_tensor.set_data(len(infer_result.data), infer_result.data):
                logging.error("Tensor set data failed!")
                raise Exception("Tensor set data failed!")
            request.add_output(i, output_tensor)
        return

    def de_init(self):
        if self.mx_stream_manager.DestroyAllStreams() != 0:
            logging.error("Failed to destroy stream manager.")
            raise Exception("Failed to destroy stream manager.")

    def add_stream(self, infer_config):
        if infer_config["id"] in self.infer_configs:
            logging.error("Infer Stream already exist!")
            raise Exception("Infer Stream already exist!")
        if "path" not in infer_config:
            logging.error("Infer config file not exist.")
            raise Exception("Infer config file not exist.")
        file_base_check(os.path.realpath(infer_config["path"]))
        with open(os.path.realpath(infer_config["path"]), 'rb') as f:
            pipeline_str = f.read()
            if self.mx_stream_manager.CreateMultipleStreams(pipeline_str) != 0:
                logging.error("Failed to create Stream.")
                raise Exception("Failed to create Stream.")
        self.infer_configs[infer_config["id"]] = infer_config

    def run(self, infer_server_manager, task_id):
        if task_id not in infer_server_manager.request_queues:
            logging.error("Can't find Request Queue of request.")
            raise Exception("Can't find Request Queue of request.")
        while not infer_server_manager.stop:
            if infer_server_manager.request_queues[task_id].empty():
                time.sleep(1)
                continue
            infer_request = infer_server_manager.request_queues[task_id].pop()
            try:
                self._inference(infer_server_manager, infer_request, task_id)
            except Exception as err_message:
                infer_request.is_error = True
                logging.error(err_message)
                logging.error("Fail to infer, request id is %s", task_id)
            finally:
                infer_request.is_processed = True

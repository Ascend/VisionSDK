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
Description: ai_server flask app.
Author: Vision SDK
Create: 2024
History: NA
"""
import base64
import os
import time
import uuid
from http import HTTPStatus
from functools import wraps

from flask import Flask
from flask import jsonify
from flask import request

from error_code import ErrorCode, ErrorMessage
from infer_request import InferRequest
from server_options_and_logger import logger as logging
from server_options_and_logger import server_option_instance
from tensor import Tensor, TensorKind
from token_bucket import token_bucket_instance

INVALID_CHARS = [
    "\n", "\f", "\r", "\b", "\t", "\v", "\u000D", "\u000A", "\u000C", "\u000B", "\u0009", "\u0008", "\u007F"
]
BYTE_RATE = 1024
MAX_BYTE_SIZE = server_option_instance.max_content_length * BYTE_RATE
TIME_OUT = 60


class StreamServerServiceApp:
    EXPECT_JSON_SIZE = 6

    def __init__(self, infer_server_manager, server_options):
        self.infer_server_manager = infer_server_manager
        self.server_options = server_options
        self.max_request_rate = "%s/minute" % str(server_options.max_request_rate)

    @staticmethod
    def fail_reply(error_code, status_code):
        response = jsonify(
            {'isSuccess': False, 'errorCode': error_code, "errorMsg": ErrorMessage[error_code]})
        response.status_code = status_code
        return response

    @staticmethod
    def _success_reply(output):
        response = jsonify(
            {'isSuccess': True, 'errorCode': ErrorCode.SUCCESS, "errorMsg": ErrorMessage[ErrorCode.SUCCESS],
             "outputs": output})
        response.status_code = HTTPStatus.OK
        return response

    def base_app(self):
        app = Flask(__name__)
        app.config["MAX_CONTENT_LENGTH"] = MAX_BYTE_SIZE
        app.config['SERVER_NAME'] = None

        @app.before_request
        def ip_check():
            res = None
            remote_addr = request.remote_addr
            for invalid_char in INVALID_CHARS:
                if invalid_char in remote_addr:
                    logging.error(ErrorMessage[ErrorCode.INTERNAL_ERROR])
                    return StreamServerServiceApp.fail_reply(ErrorCode.INTERNAL_ERROR, HTTPStatus.BAD_REQUEST)
            return res

        @app.route('/v2', methods=['GET'])
        @app.route('/v2/', methods=['GET'])
        @log_request_info
        @content_length_limit
        @request_rate_limit
        def get_server_options():
            return self._success_reply(self.server_options.server_info_json)

        @app.route('/v2/live', methods=['GET'])
        @app.route('/v2/live/', methods=['GET'])
        @log_request_info
        @content_length_limit
        @request_rate_limit
        def get_server_live():
            return self._success_reply({"isLive": self.infer_server_manager.get_server_live()})

        @app.route('/v2/ready', methods=['GET'])
        @app.route('/v2/ready/', methods=['GET'])
        @log_request_info
        @content_length_limit
        @request_rate_limit
        def get_server_ready():
            return self._success_reply({"isReady": self.infer_server_manager.get_server_ready()})

        @app.route('/v2/streams/<stream_name>/ready', methods=['GET'])
        @log_request_info
        @content_length_limit
        @request_rate_limit
        def get_stream_ready(stream_name):
            task_id = "streams%s" % stream_name
            if task_id not in self.server_options.infer_configs:
                logging.error("The task dose not exist")
                return self.fail_reply(ErrorCode.TARGET_NOT_EXISTS, HTTPStatus.BAD_REQUEST)
            return self._success_reply(self.infer_server_manager.get_stream_model_ready(task_id))

        @app.route('/v2/streams/<stream_name>/config', methods=['GET'])
        @log_request_info
        @content_length_limit
        @request_rate_limit
        def get_stream_config(stream_name):
            task_id = "streams%s" % stream_name
            if task_id not in self.server_options.infer_configs:
                logging.error("The task dose not exist")
                return self.fail_reply(ErrorCode.TARGET_NOT_EXISTS, HTTPStatus.BAD_REQUEST)
            return self._success_reply(self.server_options.infer_configs[task_id])

        @app.route('/v2/models/<model_name>/ready', methods=['GET'])
        @log_request_info
        @content_length_limit
        @request_rate_limit
        def get_model_ready(model_name):
            task_id = "models%s" % model_name
            if task_id not in self.server_options.infer_configs:
                logging.error("The task dose not exist")
                self.fail_reply(ErrorCode.TARGET_NOT_EXISTS, HTTPStatus.BAD_REQUEST)
            return self._success_reply(self.infer_server_manager.get_stream_model_ready(task_id))

        @app.route('/v2/models/<model_name>/config', methods=['GET'])
        @log_request_info
        @content_length_limit
        @request_rate_limit
        def get_model_config(model_name):
            task_id = "models%s" % model_name
            if task_id not in self.server_options.infer_configs:
                logging.error("The task dose not exist")
                self.fail_reply(ErrorCode.TARGET_NOT_EXISTS, HTTPStatus.BAD_REQUEST)
            return self._success_reply(self.server_options.infer_configs[task_id])
        return app

    def create_app(self):
        app = self.base_app()

        @app.route('/v2/<infer_type>/<name>/infer', methods=['POST'])
        @log_request_info
        @content_length_limit
        @request_rate_limit
        def post_stream_infer(infer_type, name):
            if infer_type != "streams" and infer_type != "models":
                logging.error("The task is not stream infer or model infer.")
                return self.fail_reply(ErrorCode.TARGET_NOT_EXISTS, HTTPStatus.BAD_REQUEST)
            task_id = "%s%s" % (infer_type, name)
            if task_id not in self.infer_server_manager.infer_configs:
                logging.error("The task dose not exist")
                return self.fail_reply(ErrorCode.TARGET_NOT_EXISTS, HTTPStatus.BAD_REQUEST)
            if request.headers.get("Content-Type") != "application/json":
                logging.error("The content type of request must be json")
                return self.fail_reply(ErrorCode.TARGET_NOT_EXISTS, HTTPStatus.BAD_REQUEST)
            json_data = request.json
            try:
                input_request = self._extract_input_json(json_data, task_id)
            except Exception as err_message:
                logging.error("Invalid Http request body, extract inputs item failed! Error message is %s", err_message)
                return self.fail_reply(ErrorCode.INVALID_BODY, HTTPStatus.BAD_REQUEST)
            if self.infer_server_manager.request_queues[task_id].full():
                logging.error("The stream infer queue is full, task is %s", task_id)
                return self.fail_reply(ErrorCode.CACHE_FULL, HTTPStatus.BAD_REQUEST)
            self.infer_server_manager.request_queues[task_id].push(input_request)
            start_time = time.time()
            while not input_request.is_processed:
                time.sleep(1)
                if time.time() - start_time > TIME_OUT:
                    logging.error("Input request process time out!")
                    input_request.is_processed = True
            if input_request.is_error:
                return self.fail_reply(ErrorCode.INFER_FAILED, HTTPStatus.BAD_REQUEST)
            return self._success_reply(input_request.get_output_json())

        return app

    def _extract_input_json(self, json_data, task_id):
        if "inputs" not in json_data or not isinstance(json_data["inputs"], list) or len(json_data["inputs"]) == 0:
            logging.error("Invalid Http request body, the request must has input fields and "
                          "inputs should be an valid array.")
            raise Exception("Invalid Http request body, the request must has input fields and inputs should"
                            " be an valid array.")
        inputs = json_data["inputs"][0]
        if len(inputs) != self.EXPECT_JSON_SIZE:
            logging.error("Invalid input field, the input field can only contain 6 fields: name, id, shape, format,"
                          " dataType and data.")
            raise Exception("Invalid input field, the input field can only contain 6 fields: name, id, shape, format,"
                            " dataType and data.")
        if "name" not in inputs or not isinstance(inputs["name"], str):
            logging.error("Invalid input field, no name field is included or name field is not string type.")
            raise Exception("Invalid input field, no name field is included or name field is not string type.")
        if "dataType" not in inputs or not isinstance(inputs["dataType"], str):
            logging.error("Invalid input field, no dataType field is included or dataType field is not string type.")
            raise Exception("Invalid input field, no dataType field is included or dataType field is not string type.")
        if "id" not in inputs or not isinstance(inputs["id"], int):
            logging.error("Invalid input field, no id field is included or format field is not string type.")
            raise Exception("Invalid input field, no id field is included or format field is not string type.")
        if "format" not in inputs or not isinstance(inputs["format"], str):
            logging.error("Invalid input field, no format field is included or format field is not string type.")
            raise Exception("Invalid input field, no format field is included or format field is not string type.")
        if "shape" not in inputs or not isinstance(inputs["shape"], list):
            logging.error("No shape field or shape field is not list type.")
            raise Exception("No shape field or shape field is not list type.")
        for shape in inputs["shape"]:
            if not isinstance(shape, int):
                logging.error("Shape field is not integer type.")
                raise Exception("Shape field is not integer type.")
        if "data" not in inputs:
            logging.error("Invalid input field, no data field is included or data field is not string type.")
            raise Exception("Invalid input field, no data field is included or data field is not string type.")
        decoded_data = base64.b64decode(inputs["data"])
        input_tensor = Tensor(name=inputs["name"], id=inputs["id"], format=inputs["format"], shape=inputs["shape"],
                              data_type_str=inputs["dataType"])
        if not input_tensor.check_tensor_data(TensorKind.CLIENT_KIND):
            logging.error("Check input tensor data type failed!")
            raise Exception("Check input tensor data type failed!")
        if not input_tensor.check_tensor_data_size(len(decoded_data)):
            logging.error("Check input tensor data size failed!")
            raise Exception("Check input tensor data size failed!")
        input_tensor.set_data(len(decoded_data), decoded_data)
        input_request = InferRequest(self.server_options.infer_configs[task_id])
        input_request.add_input(inputs["id"], input_tensor)
        return input_request


def content_length_limit(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        length = 0
        for header in request.headers:
            for content in header:
                length += len(content)
        data_str = request.get_data()
        length += len(data_str)
        if length > MAX_BYTE_SIZE:
            logging.error("Illegal request, content length with head is larger than %s byte.", str(MAX_BYTE_SIZE))
            return StreamServerServiceApp.fail_reply(ErrorCode.LARGE_CONTENT, HTTPStatus.REQUEST_ENTITY_TOO_LARGE)
        return func(*args, **kwargs)
    return wrapper


def request_rate_limit(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        if not token_bucket_instance.consume_token(1):
            logging.error("Too many request.")
            return StreamServerServiceApp.fail_reply(ErrorCode.RATE_EXCEEDED, HTTPStatus.BAD_REQUEST)
        return func(*args, **kwargs)
    return wrapper


def log_request_info(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        remote_addr = request.remote_addr
        uid = request.headers.get('uuid')
        client_id = None
        if uid:
            try:
                client_id = uuid.UUID(uid)
            except ValueError:
                logging.error('The client id format is wrong.')
        logging.info("Remote IP: %s Client ID: %s - - %s %s", remote_addr, client_id, request.method, request.path)
        return func(*args, **kwargs)
    return wrapper

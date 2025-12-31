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
Description: Start restful service with flask.
Author: Vision SDK
Create: 2024
History: NA
"""

from http.server import BaseHTTPRequestHandler
from utils import get_sys_info
from server_options_and_logger import server_option_instance
from server_options_and_logger import logger as logging
from create_app import StreamServerServiceApp
from infer_server_manager import InferServerManager


def version_string(self):
    return 'server'


BaseHTTPRequestHandler.version_string = version_string


def start_service(infer_server_manager):
    curr_user, hostname, user_ip = get_sys_info()
    logging.info("starting stream server: [%s], [%s], [%s]", user_ip, curr_user, hostname)
    infer_server_manager.init_infer_server()
    infer_server_manager.start_infer_server()
    stream_service = StreamServerServiceApp(infer_server_manager, server_option_instance)
    app = stream_service.create_app()
    app.run(host="127.0.0.1", port=server_option_instance.port, debug=False)


def main():
    infer_server_manager = InferServerManager(server_option_instance)
    try:
        start_service(infer_server_manager)
    except Exception as err_message:
        logging.error(err_message)
        logging.error('stream_server startup failed.')
    finally:
        infer_server_manager.stop_service()


if __name__ == '__main__':
    main()
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
Description: Infer Server Manager.
Author: Vision SDK
Create: 2024
History: NA
"""
import threading
from queue import Queue

from model_server import ModelServer
from server_options_and_logger import logger as logging
from stream_server import StreamServer

REQUEST_CACHE_UPPER_LIMIT = 1000
REQUEST_CACHE_LOWER_LIMIT = 1


class RequestQueue:
    def __init__(self, size):
        if size < REQUEST_CACHE_LOWER_LIMIT:
            size = REQUEST_CACHE_LOWER_LIMIT
        if size > REQUEST_CACHE_UPPER_LIMIT:
            size = REQUEST_CACHE_UPPER_LIMIT
        self.queue = Queue(size)

    def push(self, request):
        self.queue.put(request)

    def pop(self):
        return self.queue.get()

    def size(self):
        return self.queue.qsize()

    def full(self):
        return self.queue.full()

    def empty(self):
        return self.queue.empty()


class InferServerManager:
    def __init__(self, server_options: dict):
        self.request_queues = {}
        self.infer_configs = server_options.infer_configs
        self.project_status_map = {}
        self.stream_server = None
        self.model_servers = {}
        self.server_options = server_options
        self.stop = True
        self.live = True
        self.ready = False
        self.infer_server_count = 0
        self.thread_pool = []
        self.request_cache_size = server_options.request_cache_size

    def init_infer_server(self):
        self.stream_server = StreamServer()
        for task_id, json_data in self.infer_configs.items():
            if json_data["inferType"] == "streams":
                self.stream_server.add_stream(json_data)
            elif json_data["inferType"] == "models":
                model_server_instance = ModelServer(json_data)
                self.model_servers[json_data["id"]] = model_server_instance
            else:
                logging.error('Unsupported inferType: %s.', json_data["inferType"])
                continue
            self.infer_server_count += 1
            self.request_queues[task_id] = RequestQueue(self.request_cache_size)
        if self.infer_server_count == 0:
            logging.error("Init infer server error.")
            raise Exception("Init infer server error.")
        self.stop = False
        self.ready = True

    def start_infer_server(self):
        for task_id, _ in self.stream_server.infer_configs.items():
            t1 = threading.Thread(target=self.stream_server.run, args=(self, task_id))
            t1.start()
            if not t1.is_alive():
                logging.error("Failed to create stream infer:%s.", task_id)
                raise Exception(f"Failed to create stream infer:{task_id}.")
            self.thread_pool.append(t1)

        for task_id in self.model_servers:
            t1 = threading.Thread(target=self.model_servers[task_id].run, args=(self, self.model_servers[task_id]))
            t1.start()
            if not t1.is_alive():
                logging.error("Failed to create model infer:%s.", task_id)
                raise Exception(f"Failed to create model infer:{task_id}.")
            self.thread_pool.append(t1)

        if not self.thread_pool:
            logging.error("Unable to start any inference service.")
            raise Exception("Unable to start any inference service.")
        return

    def get_server_live(self):
        return self.live

    def get_server_ready(self):
        return self.ready

    def get_stream_model_ready(self, task_id: str):
        if task_id in self.request_queues:
            return {"isReady": True}
        return {"isReady": False}

    def get_infer_configs(self, task_id: str):
        return self.infer_configs[task_id]

    def stop_service(self):
        for _, model in self.model_servers.items():
            model.de_init()
        self.stream_server.de_init()
        self.stop = True
        for t in self.thread_pool:
            t.join()
        logging.info("Exit infer server manager all thread.")
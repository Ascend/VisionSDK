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
Description: Model server derived from FunctionalStream.
Author: Vision SDK
Create: 2024
History: NA
"""

import os
from time import sleep

import MxpiDataType_pb2 as MxpiDataType
from mindx.sdk.stream import BufferInput, PluginNode, FunctionalStream, MetadataInput

from server_options_and_logger import logger as logging
from tensor import Tensor


class ModelServer:
    def __init__(self, infer_config):
        self.infer_config = infer_config
        props = {
            "modelPath": "", "dataSource": "appsrc0", "outputDeviceId": "-1", "waitingTime": "5000",
            "dynamicStrategy": "Nearest", "singleBatchInfer": "0", "outputHasBatchDim": "1"
        }
        input_size = len(self.infer_config["inputs"])
        if input_size == 0:
            logging.error("Inputs tensor Size = 0.")
            raise Exception("Inputs tensor Size = 0.")
        inputs = []
        data_source_str = ""
        # Construct appsrc plugin node
        for i in range(input_size):
            appsrc = PluginNode("appsrc")
            inputs.append(appsrc)
            data_source_str = data_source_str + "appsrc" + str(i) + ","
        props["dataSource"] = data_source_str[0:-1]

        # Construct mxpi_tensorinfer plugin node
        props["modelPath"] = os.path.realpath(self.infer_config["path"])
        if "outputDeviceId" in self.infer_config:
            props["outputDeviceId"] = str(self.infer_config["outputDeviceId"])
        if "outputHasBatchDim" in self.infer_config:
            props["outputHasBatchDim"] = str(self.infer_config["outputHasBatchDim"])
        if "dynamicBatching" in self.infer_config:
            if "waitingTime" in self.infer_config["dynamicBatching"]:
                props["waitingTime"] = str(self.infer_config["dynamicBatching"]["waitingTime"])
            if "dynamicStrategy" in self.infer_config["dynamicBatching"]:
                props["dynamicStrategy"] = self.infer_config["dynamicBatching"]["dynamicStrategy"]
            if "singleBatchInfer" in self.infer_config["dynamicBatching"]:
                props["singleBatchInfer"] = str(self.infer_config["dynamicBatching"]["singleBatchInfer"])
        mxpi_tensorinfer0 = PluginNode("mxpi_tensorinfer", props)(*inputs)

        # Construct appsink plugin node
        appsink0 = PluginNode("appsink")(mxpi_tensorinfer0)
        outputs = [appsink0, ]
        model_name = self.infer_config["name"]

        self.stream_ = FunctionalStream(model_name, inputs, outputs)
        self.stream_.set_device_id(str(self.infer_config["deviceId"]))
        self.stream_.build()

    @staticmethod
    def send_inputs(infer_request, model_server_instance):
        for i, tensor_id in enumerate(infer_request.get_input_ids()):
            mxpi_tensor_package_list = MxpiDataType.MxpiTensorPackageList()
            tensor_package_vec = mxpi_tensor_package_list.tensorPackageVec.add()
            tensor_vec = tensor_package_vec.tensorVec.add()
            tensor_vec.dataStr = infer_request.get_input(tensor_id).data_
            tensor_vec.deviceId = 0
            tensor_vec.memType = 0
            metadata_input = MetadataInput()
            metadata_input.data_source = "appsrc0"
            metadata_input.data_type = "MxTools.MxpiTensorPackageList"
            metadata_input.serialized_metadata = mxpi_tensor_package_list.SerializeToString()
            buffer_input = BufferInput()
            model_server_instance.stream_.send("appsrc%s" % str(i), [metadata_input], buffer_input)
        return

    @staticmethod
    def get_output(infer_request, model_server_instance):
        model_result = model_server_instance.stream_.get_result("appsink0", ["mxpi_tensorinfer0"], 9000)
        for i, metadata in enumerate(model_result.metadata_list):
            mxpi_tensor = MxpiDataType.MxpiTensorPackageList()
            mxpi_tensor.ParseFromString(metadata.get_byte_data())
            output_tensor = Tensor()
            if not output_tensor.set_data(len(mxpi_tensor.tensorPackageVec[0].tensorVec[0].dataStr),
                                          mxpi_tensor.tensorPackageVec[0].tensorVec[0].dataStr):
                logging.error("Tensor set data failed!")
                return False
            infer_request.add_output(i, output_tensor)
        return True

    def de_init(self):
        self.stream_.stop()

    def inference(self, infer_request, model_server_instance):
        try:
            self.send_inputs(infer_request, model_server_instance)
        except Exception as err_message:
            logging.error("Fail to send input ", err_message)
            infer_request.is_error = True
            infer_request.is_processed = True
            return
        try:
            self.get_output(infer_request, model_server_instance)
        except Exception as err_message:
            logging.error("Fail to get output. ", err_message)
            infer_request.is_error = True
        finally:
            infer_request.is_processed = True
        return

    def dynamic_batch_inference(self, infer_server_manager, model_server_instance, task_id):
        queue_size = infer_server_manager.request_queues[task_id].size()
        preferred_batch_size_list = model_server_instance.infer_config["dynamicBatching"].get("preferredBatchSize", [])
        batch_size = 1
        if preferred_batch_size_list:
            preferred_batch_size_list = sorted(preferred_batch_size_list)
            for temp_batch in preferred_batch_size_list:
                if temp_batch <= queue_size:
                    batch_size = temp_batch
                    break
        else:
            logging.warning("preferredBatchSizeVec is empty! Batch_size is set as default value 1.")
        infer_requests = []
        for _ in range(batch_size):
            try:
                infer_request = infer_server_manager.request_queues[task_id].pop()
                self.send_inputs(infer_request, model_server_instance)
                infer_requests.append(infer_request)
            except Exception as err_message:
                logging.error(err_message)
                logging.error("Fail to send input")
                infer_request.is_processed = True
                infer_request.is_error = True

        for infer_request in infer_requests:
            try:
                self.get_output(infer_request, model_server_instance)
            except Exception as err_message:
                infer_request.is_error = True
                logging.error(err_message)
                logging.error("Fail to get output")
            finally:
                infer_request.is_processed = True

    def run(self, infer_server_manager, model_server_instance):
        task_id = model_server_instance.infer_config["id"]
        if task_id not in infer_server_manager.request_queues:
            logging.error("Can't find Request Queue of request.")
            raise Exception("Can't find Request Queue of request.")
        while not infer_server_manager.stop:
            try:
                if infer_server_manager.request_queues[task_id].empty():
                    sleep(1)
                    continue
                if "dynamicBatching" in model_server_instance.infer_config:
                    self.dynamic_batch_inference(infer_server_manager, model_server_instance, task_id)
                else:
                    infer_request = infer_server_manager.request_queues[task_id].pop()
                    self.inference(infer_request, model_server_instance)
            except Exception as err_message:
                logging.error(err_message)


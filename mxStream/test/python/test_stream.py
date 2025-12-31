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
Description: test stream
Author: Vision SDK
Create: 2024
History: NA
"""

import unittest
from stream import (BufferInput, BufferOutput, MetadataInput, MetadataOutput, DataOutput, PluginNode, SequentialStream,
                    FunctionalStream)
import mindx.sdk.MxpiDataType_pb2 as MxpiDataType


class MyTestCase(unittest.TestCase):

    def setUp(self):
        self.props = {
            "modelPath": "", "dataSource": "appsrc0", "outputDeviceId": "-1", "waitingTime": "5000",
            "dynamicStrategy": "Nearest", "singleBatchInfer": "0", "outputHasBatchDim": "1" }
        self.props0 = {"blocksize": "4096000"}

    def test_stream_buffer_io_set_and_get_success(self):
        try:
            buffer_input = BufferInput()
            buffer_input.data = "haha"
            buffer_input.frame_info = "1"
            buffer_input.vision_info = "2"
            self.assertEqual(buffer_input.data, "haha")
            self.assertEqual(buffer_input.frame_info, "1")
            self.assertEqual(buffer_input.vision_info, "2")
            buffer_output = BufferOutput()
            buffer_output.error_code = 1
            buffer_output.error_msg = "ok"
            buffer_output.data = "ahah"
            self.assertEqual(buffer_output.data, "ahah")
            self.assertEqual(buffer_output.error_msg, "ok")
            self.assertEqual(buffer_output.error_code, 1)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_stream_metadata_io_set_and_get_success(self):
        try:
            metadata_input = MetadataInput()
            metadata_input.data_source = "1"
            metadata_input.data_type = "1"
            metadata_input.serialized_metadata = "1"
            self.assertEqual(metadata_input.data_source, "1")
            self.assertEqual(metadata_input.data_type, "1")
            self.assertEqual(metadata_input.serialized_metadata, "1")
            metadata_output = MetadataOutput()
            metadata_output.error_code = 1
            metadata_output.error_msg = "ok"
            metadata_output.data_type = "ahah"
            metadata_output.serialized_metadata = "ahah"
            metadata_output.set_error_info(1, "f")
            metadata_output.get_byte_data()
            self.assertEqual(metadata_output.error_code, 1)
            self.assertEqual(metadata_output.error_msg, "f")
            self.assertEqual(metadata_output.data_type, "ahah")
            self.assertEqual(metadata_output.serialized_metadata, "ahah")
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_stream_data_output_set_and_get_success(self):
        try:
            data_output = DataOutput()
            data_output.error_code = 1
            data_output.error_msg = "ok"
            data_output.buffer_output = BufferOutput()
            data_output.metadata_list = [MetadataOutput()]
            data_output.set_error_info(1, "f")
            buffer_output = data_output.buffer_output
            metadata_list = data_output.metadata_list
            self.assertEqual(data_output.error_msg, "f")
            self.assertEqual(data_output.error_code, 1)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_plugin_node_plugin_name_return_success(self):
        try:
            appsrc = PluginNode("appsrc", self.props, "test")
            test_name = appsrc.plugin_name()
            self.assertEqual(test_name, "test")
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_plugin_node_factory_return_success(self):
        try:
            appsrc = PluginNode("appsrc", self.props, "test")
            test_factory = appsrc.factory()
            self.assertEqual(test_factory, "appsrc")
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_plugin_node_to_json_return_success(self):
        expect_json="{\"factory\":\"appsrc\",\"props\":{\"dataSource\":\"appsrc0\",\"dynamicStrategy\":\"Nearest\"," \
                     "\"modelPath\":\"\",\"outputDeviceId\":\"-1\",\"outputHasBatchDim\":\"1\"," \
                     "\"singleBatchInfer\":\"0\",\"waitingTime\":\"5000\"}}"
        try:
            appsrc = PluginNode("appsrc", self.props, "test")
            test_json = appsrc.to_json()
            self.assertEqual(test_json, expect_json)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_plugin_node_call_return_success(self):
        appsrc = PluginNode("appsrc", self.props, "test")
        try:
            test_call = appsrc(appsrc)
            test_call = test_call(appsrc, appsrc)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_sequential_stream_combination_interface_return_success(self):
        expect_json = "{\"appsink0\":{\"factory\":\"appsink\",\"props\":{\"blocksize\":\"4096000\"}}," \
        "\"appsrc0\":{\"factory\":\"appsrc\",\"next\":\"appsink0\",\"props\":{\"blocksize\":\"4096000\"}}," \
        "\"stream_config\":{\"deviceId\":\"0\"}}"
        try:
            sequential_stream = SequentialStream("stream")
            sequential_stream.set_device_id("0")
            sequential_stream.add(PluginNode("appsrc", self.props0))
            sequential_stream.add(PluginNode("appsink", self.props0))
            sequential_stream.build()
            element_name0 = "appsrc0"
            element_name1 = "appsink0"
            time_out = 3
            buffer_input = BufferInput()
            buffer_input.data = "hahahahahaha"
            sequential_stream.send(element_name0, [], buffer_input)
            data_output = sequential_stream.get_result(element_name1, [], time_out)
            self.assertEqual(data_output.error_code, 0)
            test_json = sequential_stream.to_json()
            self.assertEqual(test_json, expect_json)
            sequential_stream.stop()
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_functional_stream_combination_interface_return_success(self):
        expect_json = "{\"appsink0\":{\"factory\":\"appsink\"}," \
        "\"appsrc0\":{\"factory\":\"appsrc\",\"next\":\"appsink0\",\"props\":{\"blocksize\":\"4096000\"}}," \
        "\"stream_config\":{\"deviceId\":\"0\"}}"
        try:
            functional_stream_0 = FunctionalStream("test")
            appsrc0 = PluginNode("appsrc", self.props0, "appsrc0")
            appsink0 = PluginNode("appsink")(appsrc0)
            functional_stream = FunctionalStream("test", [appsrc0], [appsink0])
            functional_stream.set_device_id("0")
            functional_stream.build()
            element_name0 = "appsrc0"
            element_name1 = "appsink0"
            time_out = 3
            buffer_input = BufferInput()
            buffer_input.data = "hahahahahaha"
            functional_stream.send(element_name0, [], buffer_input)
            data_output = functional_stream.get_result(element_name1, [], time_out)
            self.assertEqual(data_output.error_code, 0)
            test_json = functional_stream.to_json()
            self.assertEqual(test_json, expect_json)
            functional_stream.stop()
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

if __name__ == '__main__':
    unittest.main()
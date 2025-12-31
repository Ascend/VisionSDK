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
import json
from StreamManagerApi import (StreamManagerApi, MxProtobufIn, MxProtobufOut, InProtobufVector,
                              OutProtobufVector, MxMetadataInput, MxMetadataOutput, MetadataInputVector,
                              MetadataOutputVector, MxBufferAndMetadataOutput, MxBufferOutput,
                              RoiBox, RoiBoxVector, MxDataInput, MxDataOutput, MxDataInputExt,
                              MxBufferInput, StringVector)
import mindx.sdk.MxpiDataType_pb2 as MxpiDataType


class MyTestCase(unittest.TestCase):
    def setUp(self):
        self.pipeline = {
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
        self.pipeline_str = json.dumps(self.pipeline).encode()
        self.stream_manager_api = StreamManagerApi()
        self.assertEqual(self.stream_manager_api.InitManager(), 0)

    def tearDown(self):
        self.assertEqual(self.stream_manager_api.DestroyAllStreams(), 0)

    def test_stream_manager_api_mx_protobuf_io_set_and_get_success(self):
        try:
            mx_protobuf_in = MxProtobufIn()
            mx_protobuf_in.key = b'1'
            mx_protobuf_in.type = b'1'
            mx_protobuf_in.protobuf = b'1'
            self.assertEqual(mx_protobuf_in.key, b'1')
            self.assertEqual(mx_protobuf_in.type, b'1')
            self.assertEqual(mx_protobuf_in.protobuf, b'1')
            mx_protobuf_out = MxProtobufOut()
            mx_protobuf_out.errorCode = 1
            mx_protobuf_out.messageName = b'1'
            mx_protobuf_out.messageBuf = b'1'
            self.assertEqual(mx_protobuf_out.errorCode, 1)
            self.assertEqual(mx_protobuf_out.messageName, b'1')
            self.assertEqual(mx_protobuf_out.messageBuf, b'1')
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_stream_manager_api_protobuf_vector_append_and_pop_success(self):
        try:
            in_protobuf_vector = InProtobufVector()
            in_protobuf_vector.append(MxProtobufIn())
            in_protobuf_vector.pop()
            vec_size = in_protobuf_vector.size()
            self.assertEqual(vec_size, 0)
            out_protobuf_vector = OutProtobufVector()
            out_protobuf_vector.append(MxProtobufOut())
            out_protobuf_vector.pop()
            vec_size = out_protobuf_vector.size()
            self.assertEqual(vec_size, 0)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_stream_manager_api_mx_met_data_io_set_and_get_success(self):
        try:
            mx_metadata_input = MxMetadataInput()
            mx_metadata_input.datasource = b'1'
            mx_metadata_input.dataType = b'1'
            mx_metadata_input.serializedMetadata = b'1'
            self.assertEqual(mx_metadata_input.datasource, b'1')
            self.assertEqual(mx_metadata_input.dataType, b'1')
            self.assertEqual(mx_metadata_input.serializedMetadata, b'1')
            mx_metadata_output = MxMetadataOutput()
            mx_metadata_output.errorCode = 1
            mx_metadata_output.errorMsg = b'1'
            mx_metadata_output.dataType = b'1'
            mx_metadata_output.serializedMetadata = b'1'
            self.assertEqual(mx_metadata_output.errorCode, 1)
            self.assertEqual(mx_metadata_output.errorMsg, b'1')
            self.assertEqual(mx_metadata_output.dataType, b'1')
            self.assertEqual(mx_metadata_output.serializedMetadata, b'1')
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")
    
    def test_stream_manager_api_metadata_vector_set_and_get_success(self):
        try:
            metadata_input_vector = MetadataInputVector()
            metadata_input_vector.append(MxMetadataInput())
            metadata_input_vector.pop()
            vec_size = metadata_input_vector.size()
            self.assertEqual(vec_size, 0)
            metadata_output_vector = MetadataOutputVector()
            metadata_output_vector.append(MxMetadataOutput())
            metadata_output_vector.pop()
            vec_size = metadata_output_vector.size()
            self.assertEqual(vec_size, 0)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")
    
    def test_stream_manager_api_mx_buffer_and_metadata_output_set_and_get_success(self):
        try:
            mx_buffer_and_metadata_output = MxBufferAndMetadataOutput()
            mx_buffer_and_metadata_output.errorCode = 1
            mx_buffer_and_metadata_output.errorMsg = b'1'
            mx_buffer_output = MxBufferOutput()
            mx_buffer_and_metadata_output.bufferOutput = mx_buffer_output
            mx_buffer_and_metadata_output.metadataVec = MetadataOutputVector()
            self.assertEqual(mx_buffer_and_metadata_output.errorCode, 1)
            self.assertEqual(mx_buffer_and_metadata_output.errorMsg, b'1')
            buffer_output = mx_buffer_and_metadata_output.bufferOutput
            meta_data_vec = mx_buffer_and_metadata_output.metadataVec
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_stream_manager_api_roi_box_set_and_get_success(self):
        try:
            roi_box = RoiBox()
            roi_box.x0 = 1
            roi_box.x1 = 2
            roi_box.y0 = 3
            roi_box.y1 = 4
            self.assertEqual(roi_box.x0, 1)
            self.assertEqual(roi_box.x1, 2)
            self.assertEqual(roi_box.y0, 3)
            self.assertEqual(roi_box.y1, 4)
            roi_box_vector = RoiBoxVector()
            roi_box_vector.push_back(roi_box)
            roi_box_vector.pop_back()
            roi_box_vector.push_back(roi_box)
            roi_box_vector.pop()
            self.assertEqual(roi_box_vector.size(), 0)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_stream_manager_api_mxdata_io_set_and_get_success(self):
        try:
            mx_data_input = MxDataInput()
            mx_data_input.data = b'1'
            mx_data_input.fragmentId = 1
            mx_data_input.customParam = b'1'
            mx_data_input.roiBoxs = RoiBoxVector()
            self.assertEqual(mx_data_input.data, b'1')
            self.assertEqual(mx_data_input.fragmentId, 1)
            self.assertEqual(mx_data_input.customParam, b'1')
            mx_data_output = MxDataOutput()
            mx_data_output.data = b'1'
            mx_data_output.errorCode = 1
            self.assertEqual(mx_data_output.data, b'1')
            self.assertEqual(mx_data_output.errorCode, 1)
            mx_data_input_ext = MxDataInputExt()
            mx_data_input_ext.data = b'1'
            mx_data_input_ext.fragmentId = 1
            mx_data_input_ext.customParam = b'1'
            mx_data_input_ext.roiBoxs = RoiBoxVector()
            mx_data_input_ext.mxpiFrameInfo = b'1'
            mx_data_input_ext.mxpiVisionInfo = b'1'
            self.assertEqual(mx_data_input_ext.data, b'1')
            self.assertEqual(mx_data_input_ext.fragmentId, 1)
            self.assertEqual(mx_data_input_ext.customParam, b'1')
            self.assertEqual(mx_data_input_ext.mxpiFrameInfo, b'1')
            self.assertEqual(mx_data_input_ext.mxpiVisionInfo, b'1')
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_stream_manager_api_buffer_io_set_and_get_success(self):
        try:
            mx_buffer_input = MxBufferInput()
            mx_buffer_input.data = b'1'
            mx_buffer_input.mxpiFrameInfo = b'1'
            mx_buffer_input.mxpiVisionInfo = b'1'
            self.assertEqual(mx_buffer_input.data, b'1')
            self.assertEqual(mx_buffer_input.mxpiFrameInfo, b'1')
            self.assertEqual(mx_buffer_input.mxpiVisionInfo, b'1')
            mx_buffer_output = MxBufferOutput()
            mx_buffer_output.errorCode = 1
            mx_buffer_output.errorMsg = b'1'
            mx_buffer_output.data = b'1'
            self.assertEqual(mx_buffer_output.errorCode, 1)
            self.assertEqual(mx_buffer_output.errorMsg, b'1')
            self.assertEqual(mx_buffer_output.data, b'1')
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_stream_manager_api_send_data_return_success(self):
        try:
            sub_ret = self.stream_manager_api.CreateMultipleStreams(self.pipeline_str)
            self.assertEqual(sub_ret, 0)
            data_input = MxDataInput()
            data_input.data = b'hahahahahahaahahha'
            sub_ret = self.stream_manager_api.SendData(b'detection', 0, data_input)
            self.assertEqual(sub_ret, 0)
            sub_ret = self.stream_manager_api.SendData(b'detection', b'appsrc0', data_input)
            self.assertEqual(sub_ret, 0)
            vision_list = MxpiDataType.MxpiVisionList()
            vision_vec = vision_list.visionVec.add()
            vision_vec.visionData.dataStr = data_input.data
            metadata = MxMetadataInput()
            metadata.dataSource = b"appsrc0"
            metadata.dataType = b"MxTools.MxpiVisionList"
            metadata.serializedMetadata = vision_list.SerializeToString()
            metadata_vec = MetadataInputVector()
            metadata_vec.push_back(metadata)
            data_buffer = MxBufferInput()
            sub_ret = self.stream_manager_api.SendData(b'detection', b'appsrc0', metadata_vec, data_buffer)
            self.assertEqual(sub_ret, 0)
            data_output = self.stream_manager_api.GetResult(b'detection', 0, 3)
            self.assertEqual(data_output.errorCode, 0)
            data_source = b"hahahahahahaahahha"
            data_source_vec = StringVector()
            data_source_vec.push_back(data_source)
            data_output = self.stream_manager_api.GetResult(b'detection', b'appsink0', data_source_vec, 3)
            self.assertEqual(data_output.errorCode, 0)
            protobuf = MxProtobufIn()
            object_list = MxpiDataType.MxpiClassList()
            protobuf.key = b'mxpi_modelinfer1'
            protobuf.type = b'MxTools.MxpiClassList'
            protobuf.protobuf = object_list.SerializeToString()
            protobuf_vec = InProtobufVector()
            protobuf_vec.push_back(protobuf)
            sub_ret = self.stream_manager_api.SendProtobuf(b'detection', b'appsrc0', protobuf_vec)
            self.assertEqual(sub_ret, 0)
            sub_ret = self.stream_manager_api.SendProtobuf(b'detection', 0, protobuf_vec)
            self.assertEqual(sub_ret, 0)
            keys = [b'mxpi_modelinfer1']
            key_vec = StringVector()
            for key in keys:
                key_vec.push_back(key)
            protobuf_vec = OutProtobufVector()
            output_vec = self.stream_manager_api.GetProtobuf(b'detection', 0, key_vec)
            self.assertEqual(output_vec.size(), 0)
            sub_ret = self.stream_manager_api.StopStream(b'detection')
            self.assertEqual(sub_ret, 0)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

    def test_stream_manager_api_send_data_with_unique_id_return_success(self):
        path = b"test.pipline"
        try:
            sub_ret = self.stream_manager_api.CreateMultipleStreamsFromFile(path)
            self.assertEqual(sub_ret, 0)
            data_input = MxDataInput()
            data_input.data = b'hahahahahahaahahha'
            sub_ret = self.stream_manager_api.SendDataWithUniqueId(b'detection', 0, data_input)
            self.assertEqual(sub_ret, 0)
            data_output = self.stream_manager_api.GetResultWithUniqueId(b'detection', 0, 3)
            self.assertEqual(data_output.errorCode, 0)
        except Exception as e:
            self.fail(f"Expected no exception, but got {e}")

if __name__ == '__main__':
    unittest.main()
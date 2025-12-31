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
Description: Tensor.
Author: Vision SDK
Create: 2024
History: NA
"""

import base64
import logging
from enum import Enum

MAX_NAME_LEN = 100
MIN_TENSOR_ID = 0
MAX_TENSOR_ID = 10000
SHAPE_SIZE_MIN = 1
SHAPE_SIZE_MAX = 10000
MIN_DATA_TYPE_STR_LEN = 4
MAX_DATA_TYPE_STR_LEN = 8
MAX_DATA_SIZE = 51200 * 1024
MAX_DATA_SIZE_INT = 51200 * 1024
LEGAL_TENSOR_NAME_CHAR = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_+-/:"


class TensorKind(Enum):
    SERVER_KIND = 0
    CLIENT_KIND = 1


class DataType(Enum):
    UNDEFINED = -1
    FLOAT32 = 0
    FLOAT16 = 1
    INT8 = 2
    INT32 = 3
    UINT8 = 4
    UINT16 = 5
    UINT32 = 6
    INT64 = 7
    UINT64 = 8
    DOUBLE64 = 9
    BOOL = 10
    STRING = 11
    BINARY = 12


class Tensor:
    def __init__(self, **kwargs):
        self.name_ = kwargs.get("name")
        self.id_ = kwargs.get("id")
        self.format_ = kwargs.get("format")
        self.shape_ = kwargs.get("shape")
        self.data_type_size_ = 0
        self.data_size_ = 0
        self.data_ = None
        self.data_type_str_ = ""
        self.data_type_ = DataType.UNDEFINED
        if kwargs.get("data_type_str"):
            self.data_type_str_ = kwargs.get("data_type_str")
        elif kwargs.get("data_type"):
            self.data_type_ = kwargs.get("data_type")

    def check_data_size(self, data_size):
        if data_size == 0 or data_size > MAX_DATA_SIZE:
            logging.error("Tensor data size is invalid.")
            return False
        if data_size != self.data_size_:
            logging.error("The length of the received data does not match the dimension of the tensor. "
                          "The length of the received data = %s", data_size)
            self.print_tensor_info()
            return False
        return True

    def set_data(self, data_size, data_ptr):
        if not data_ptr:
            logging.error("Data is None.")
            return False
        self.data_ = data_ptr
        return True

    def name(self):
        return self.name_

    def id(self):
        return self.id_

    def data(self):
        return self.data_

    def data_size(self):
        return len(self.data_)

    def get_data_type(self):
        return self.data_type_

    def get_tensor_json(self):
        return {
            "name": self.name_,
            "data_type": self.data_type_str_,
            "format": self.format_,
            "shape": self.shape_,
            "data": base64.b64encode(self.data_).decode(),
        }

    def check_tensor_data_size(self, kind):
        if kind == TensorKind.CLIENT_KIND:
            count = 1
            for s in self.shape_:
                if s <= 0 or s > MAX_DATA_SIZE_INT or MAX_DATA_SIZE / s < count:
                    logging.error("Tensor shape dim is invalid.")
                    return False
                count *= s
            if count == 0:
                logging.error("Tensor shape dim count is invalid.")
                return False
            self.data_size_ = count * self.data_type_size_
        elif kind == TensorKind.SERVER_KIND:
            count = 1
            for s in self.shape_:
                if s == -1:
                    continue
                if s <= 0 or s > MAX_DATA_SIZE_INT or MAX_DATA_SIZE / s < count:
                    logging.error("Tensor shape dim is invalid.")
                    return False
                count *= s
        return True

    def check_tensor_data(self, kind):
        if len(self.name_) == 0 or len(self.name_) > MAX_NAME_LEN:
            logging.error("Tensor name length is invalid.")
            return False
        for i in self.name_:
            if i not in LEGAL_TENSOR_NAME_CHAR:
                logging.error("Tensor name has invalid char.")
                return False
        if self.id_ < MIN_TENSOR_ID or self.id_ > MAX_TENSOR_ID:
            logging.error("Tensor id is out of range.")
            return False
        if len(self.data_type_str_) < MIN_DATA_TYPE_STR_LEN or len(self.data_type_str_) > MAX_DATA_TYPE_STR_LEN:
            logging.error("DataType string len out of range.")
            return False
        if not (self.format_ == "FORMAT_NONE" or self.format_ == "FORMAT_NHWC" or self.format_ == "FORMAT_NCWH"):
            logging.error("Tensor format is invaid.")
            return False
        if len(self.shape_) < SHAPE_SIZE_MIN or len(self.shape_) > SHAPE_SIZE_MAX:
            logging.error("Tensor shape size is out of range.")
            return False
        return self.check_tensor_data_size(kind)

    def print_tensor_info(self):
        shape_str = ",".join([str(i) for i in self.shape_])
        logging.info("Tensor info: dataTypeStr=%s, dataType=%s, dataTypeSize=%s, format=%s, shape=%s, dataSize=%s",
                     self.data_type_str_, self.data_type_, self.data_type_size_, self.format_, shape_str,
                     self.data_size_)

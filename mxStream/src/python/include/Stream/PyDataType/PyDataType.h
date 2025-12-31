/*
* -------------------------------------------------------------------------
*  This file is part of the Vision SDK project.
* Copyright (c) 2025 Huawei Technologies Co.,Ltd.
*
* Vision SDK is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*
*           http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
* See the Mulan PSL v2 for more details.
* -------------------------------------------------------------------------
 * Description: Input data. Including image storage and info. As one input param of SenData interface.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef PY_DATA_TYPE_H
#define PY_DATA_TYPE_H

#include <string>
#include <Python.h>
#include <vector>
namespace PyStream {
struct BufferInput {
public:
    BufferInput() = default;
    ~BufferInput() = default;

    std::string data;
    std::string frame_info;
    std::string vision_info;
};

struct BufferOutput {
public:
    BufferOutput() = default;
    ~BufferOutput() = default;
    PyObject *get_byte_data()
    {
        return PyBytes_FromStringAndSize(data.data(), data.size());
    }

    int error_code = 0;
    std::string error_msg;
    std::string data;
};

struct MetadataOutput {
public:
    MetadataOutput() = default;
    ~MetadataOutput() = default;
    void set_error_info(int errorCodeIn, const std::string &errorMsgIn)
    {
        error_code = errorCodeIn;
        error_msg = errorMsgIn;
    }
    PyObject *get_byte_data()
    {
        return PyBytes_FromStringAndSize(serialized_metadata.data(), serialized_metadata.size());
    }

    int error_code = 0;
    std::string error_msg;
    std::string data_type;
    std::string serialized_metadata;
};

struct DataOutput {
public:
    DataOutput() = default;
    ~DataOutput() = default;
    void set_error_info(int errorCodeIn, const std::string &errorMsgIn)
    {
        error_code = errorCodeIn;
        error_msg = errorMsgIn;
    }

    int error_code = 0;
    std::string error_msg;
    BufferOutput buffer_output;
    std::vector<MetadataOutput> metadata_list = {};
};

struct MetadataInput {
public:
    MetadataInput() = default;
    ~MetadataInput() = default;

    std::string data_source;
    std::string data_type;
    std::string serialized_metadata;
};

struct CropRoiBox {
    float x0;
    float y0;
    float x1;
    float y1;
};

struct MxDataInput {
public:
    MxDataInput() = default;
    ~MxDataInput() = default;

    std::string data;
    int fragmentId;
    std::string customParam;
    std::vector<CropRoiBox> roiBoxs;
};

struct MxDataOutput {
public:
    MxDataOutput() = default;
    ~MxDataOutput() = default;

    int errorCode = 0;
    int dataSize = 0;
    std::string data;
};
}  // namespace PyStream
#endif
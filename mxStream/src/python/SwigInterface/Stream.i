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
 * Description: Python version variable definition.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

/* Stream.i */
%module stream
%{
#define SWIG_PYTHON_STRICT_BYTE_CHAR
#include "Stream/PyDataType/PyDataType.h"
#include "Stream/PyPluginNode/PluginNode.h"
#include "Stream/PyStream/FunctionalStream.h"
#include "Stream/PyStream/SequentialStream.h"
%}
%include <std_string.i>
%include <std_vector.i>
%include <std_map.i>
%include <std_shared_ptr.i>

namespace std {
    %template(CropRoiBoxVector) vector<PyStream::CropRoiBox>;
    %template(StringVector) vector<std::string>;
    %template(MxDataInputVector) vector<PyStream::MxDataInput>;
    %template(MxDataOutputVector) vector<PyStream::MxDataOutput>;
    %template(MetadataInputVector) vector<PyStream::MetadataInput>;
    %template(MetadataOutputVector) vector<PyStream::MetadataOutput>;
    %template(PluginNodeVector) vector<PyStream::PluginNode>;
    %template(StringMap) map<string,string>;
    %shared_ptr(MxStream::Stream);
    %shared_ptr(MxStream::PluginNode);
}
%naturalvar PyStream::MxDataInput::roiBoxs;
%naturalvar PyStream::DataOutput::metadata_list;
%ignore PyStream::PluginNode::GetNode() const;
%ignore PyStream::PluginNode::PluginNode();
%ignore PyStream::PluginNode::PluginNode(const PluginNode& other);

%include "Stream/PyDataType/PyDataType.h"
%include "Stream/PyPluginNode/PluginNode.h"
%include "Stream/PyStream/FunctionalStream.h"
%include "Stream/PyStream/SequentialStream.h"
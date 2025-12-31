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

/* Post.i */
%module post
%{
#define SWIG_PYTHON_STRICT_BYTE_CHAR
#include "PyPostProcess/PyPostProcess.h"
#include "PyTensor/PyTensor.h"
#include "PyPostProcessDataType/PyPostProcessDataType.h"
%}
%include <std_string.i>
%include <std_vector.i>
%include <std_map.i>
%include <std_pair.i>
namespace std {
    %template(StringMap) map<std::string, std::string>;
    %template(TensorVector) vector<PyBase::Tensor>;
    %template(ResizedImageInfoVector) vector<PyBase::ResizedImageInfo>;
    %template(ObjectInfoVector) vector<PyBase::ObjectInfo>;
    %template(ClassInfoVector) vector<PyBase::ClassInfo>;
    %template(TextsInfoVector) vector<PyBase::TextsInfo>;
    %template(TextObjectInfoVector) vector<PyBase::TextObjectInfo>;
    %template(KeyPointDetectionInfoVector) vector<PyBase::KeyPointDetectionInfo>;
    %template(SemanticSegInfoVector) vector<PyBase::SemanticSegInfo>;
    %template(ObjectInfoVecVector) vector<vector<PyBase::ObjectInfo>>;
    %template(ClassInfoVecVector) vector<vector<PyBase::ClassInfo>>;
    %template(TextObjectInfoVecVector) vector<vector<PyBase::TextObjectInfo>>;
    %template(KeyPointDetectionInfoVecVector) vector<vector<PyBase::KeyPointDetectionInfo>>;
}
%exception {
    try {
        $action;
    } catch (const std::runtime_error &e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        SWIG_fail;
    }
}
%include "PyPostProcess/PyPostProcess.h"
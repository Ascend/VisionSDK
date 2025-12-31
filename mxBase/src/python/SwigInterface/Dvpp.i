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

/* Dvpp.i */
%module dvpp
%{
#define SWIG_PYTHON_STRICT_BYTE_CHAR
#include "MxBase/E2eInfer/DataType.h"
#include "PyDvpp/PyDvpp.h"
#include "PyImage/PyImage.h"
%}
%exception {
    try {
        $action;
    } catch (const std::runtime_error &e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        SWIG_fail;
    }
}
%include <std_string.i>
%ignore MxBase::TensorDType;
%ignore MxBase::VisionDataFormat;
%ignore MxBase::StreamFormat;
%ignore MxBase::BorderType;
%ignore MxBase::CvtColorMode;
%ignore MxBase::ReduceDim;
%ignore MxBase::ReduceType;
%ignore MxBase::CmpOp;
%ignore MxBase::IMAGE_FORMAT_STRING;
%ignore MxBase::MorphShape;
%ignore MxBase::ThresholdType;
%ignore MxBase::BlurConfig;
%ignore MxBase::ModelLoadOptV2;
%ignore MxBase::Interpolation;
%ignore MxBase::ImageFormat;
%include "PyDvpp/PyDvpp.h"
%include "MxBase/E2eInfer/DataType.h"
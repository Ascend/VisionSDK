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
 * Author: Vision SDK
 * Create: 2025
 * History: NA
 */

/* Base.i */
%module(directors="1") base
%{
#define SWIG_PYTHON_STRICT_BYTE_CHAR
#include "PyImage/PyImage.h"
#include "PyImageProcessor/PyImageProcessor.h"
#include "PyInit/PyInit.h"
#include "PyModel/PyModel.h"
#include "PyPostProcessDataType/PyPostProcessDataType.h"
#include "PyTensor/PyTensor.h"
#include "PyVideoDecoder/PyVideoDecoder.h"
#include "PyTensorOperations/PyMatricesCoreOperationsOnTensors.h"
#include "PyVideoEncoder/PyVideoEncoder.h"
#include "PyTensorOperations/PyPerElementOperations.h"

#include "MxBase/E2eInfer/Rect/Rect.h"
#include "MxBase/E2eInfer/Color/Color.h"
#include "MxBase/E2eInfer/Size/Size.h"
#include "MxBase/E2eInfer/Dim/Dim.h"
#include "MxBase/E2eInfer/DataType.h"
#include "MxBase/E2eInfer/Point/Point.h"
%}
%include <std_string.i>
%include <std_vector.i>
%include <std_map.i>
%include <std_shared_ptr.i>
%include <std_pair.i>
%include <stdint.i>
%include <attribute.i>
namespace std {
    %template(RectVector) vector<MxBase::Rect>;
    %template(ImageVector) vector<PyBase::Image>;
    %template(RectPair) pair<MxBase::Rect, MxBase::Rect>;
    %template(RectSizePair) pair<MxBase::Rect, MxBase::Size>;
    %template(RectSizePairVector) vector<std::pair<MxBase::Rect, MxBase::Size>>;
    %template(TensorVector) vector<PyBase::Tensor>;
    %template(Uint32Vector) vector<uint32_t>;
    %template(Uint32VectorVector) vector<vector<uint32_t>>;
    %template(IntVector) vector<int>;
    %template(IntVectorVector) vector<vector<int>>;
    %template(StringVector) vector<std::string>;
    %template(SizePair) pair<MxBase::Size, MxBase::Size>;
    %template(StringMap) map<std::string, std::string>;
    %template(IntFloatMap) map<int, float>;
    %template(FloatVector) vector<float>;
    %template(IntFloatVectorMap) map<int, vector<float>>;
    %template(Uint8Vector) vector<uint8_t>;
    %template(Uint8VectorVector) vector<vector<uint8_t>>;
}
%exception {
    try {
        $action;
    } catch (const std::runtime_error &e) {
        PyErr_SetString(PyExc_RuntimeError, e.what());
        SWIG_fail;
    }
}
// ignore DataType Enum
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
%ignore MxBase::Rect::Rect(const Point leftTop, const Point rightBottom);

// Image wrapper
%attribute(PyBase::Image, int, device, PyBase::Image::GetDeviceId);
%attribute(PyBase::Image, int, original_height, PyBase::Image::GetImageOriginalHeight);
%attribute(PyBase::Image, int, original_width, PyBase::Image::GetImageOriginalWidth);
%attribute(PyBase::Image, int, height, PyBase::Image::GetImageHeight);
%attribute(PyBase::Image, int, width, PyBase::Image::GetImageWidth);
%attribute(PyBase::Image, MxBase::ImageFormat, format, PyBase::Image::GetFormat);
%ignore PyBase::Image::Image(const Image &other);
%ignore PyBase::Image::GetDeviceId() const;
%ignore PyBase::Image::GetImageHeight() const;
%ignore PyBase::Image::GetImageWidth() const;
%ignore PyBase::Image::GetFormat() const;
%ignore PyBase::Image::GetImageOriginalHeight() const;
%ignore PyBase::Image::GetImageOriginalWidth() const;
%ignore PyBase::Image::GetImagePtr() const;
%ignore PyBase::DEFAULT_ENCODE_LEVEL;

// Tensor wrapper
%attribute(PyBase::Tensor, int, device, PyBase::Tensor::GetDeviceId);
%attribute(PyBase::Tensor, MxBase::TensorDType, dtype, PyBase::Tensor::GetDataType);
%attributeval(PyBase::Tensor, std::vector<uint32_t>, shape, PyBase::Tensor::GetShape);
%ignore PyBase::Tensor::Tensor();
%ignore PyBase::Tensor::Tensor(const Tensor &other);
%ignore PyBase::Tensor::GetDeviceId() const;
%ignore PyBase::Tensor::GetDataType() const;
%ignore PyBase::Tensor::GetShape() const;
%ignore PyBase::Tensor::SetTensor(const MxBase::Tensor &src);
%ignore PyBase::Tensor::GetTensorPtr() const;

// Vdec wrapper
%ignore PyBase::CallBackVdec(MxBase::Image &decodedImage, uint32_t channelId, uint32_t frameId, void *userData);
%ignore PyBase::VideoDecoder::VideoDecoder(const VideoDecoder &vdec);
%ignore PyBase::VdecCallBacker::vdecHelper_;
%ignore PyBase::VdecCallBackerHelper::HandleEntry(PyBase::Image decodedImage, int channelId, int frameId);
%feature("director") PyBase::VdecCallBackerHelper;

// Venc wrapper
%ignore PyBase::CallBackVenc(
    std::shared_ptr<uint8_t> &outDataPtr, uint32_t &outDataSize, uint32_t &channelId, uint32_t &frameId, void *userData);
%ignore PyBase::VideoEncoder::VideoEncoder(const VideoEncoder &venc);
%ignore PyBase::VideoEncodeConfig::thresholdI;
%ignore PyBase::VideoEncodeConfig::thresholdB;
%ignore PyBase::VideoEncodeConfig::thresholdP;
%ignore PyBase::VencCallBacker::vencHelper_;
%ignore PyBase::VencCallBackerHelper::HandleEntry(PyObject *pyBytes, int outDataSize, int channelId, int frameId);
%feature("director") PyBase::VencCallBackerHelper;

// model wrapper
%attribute(PyBase::Model, MxBase::VisionDataFormat, input_format, PyBase::Model::GetInputFormat);
%attribute(PyBase::Model, uint32_t, input_num, PyBase::Model::GetInputNum);
%attribute(PyBase::Model, uint32_t, output_num, PyBase::Model::GetOutputNum);
%ignore PyBase::DeviceMemory::DeviceMemory(const DeviceMemory &other);
%ignore PyBase::Model::Model(const Model &other);
%ignore PyBase::ModelType;
%ignore PyBase::ModelLoadType;
%ignore PyBase::Model::GetInputFormat() const;
%ignore PyBase::Model::GetInputNum() const;
%ignore PyBase::Model::GetOutputNum() const;

// model post struct wrapper
%naturalvar PyBase::KeyPointDetectionInfo::keyPointMap;
%naturalvar PyBase::KeyPointDetectionInfo::scoreMap;
%naturalvar PyBase::TextsInfo::text;
%naturalvar PyBase::SemanticSegInfo::pixels;
%naturalvar PyBase::SemanticSegInfo::labelMap;
%naturalvar PyBase::ObjectInfo::mask;
%ignore PyBase::ResizeType;
%extend PyBase::KeyPointDetectionInfo {
    std::string __str__() {
        return $self->Print();
    }
    std::string __repr__() {
        return $self->Print();
    }
}
%extend PyBase::TextObjectInfo {
    std::string __str__() {
        return $self->Print();
    }
    std::string __repr__() {
        return $self->Print();
    }
}
%extend PyBase::TextsInfo {
    std::string __str__() {
        return $self->Print();
    }
    std::string __repr__() {
        return $self->Print();
    }
}
%extend PyBase::AttributeInfo {
    std::string __str__() {
        return $self->Print();
    }
    std::string __repr__() {
        return $self->Print();
    }
}
%extend PyBase::SemanticSegInfo {
    std::string __str__() {
        return $self->Print();
    }
    std::string __repr__() {
        return $self->Print();
    }
}
%extend PyBase::ClassInfo {
    std::string __str__() {
        return $self->Print();
    }
    std::string __repr__() {
        return $self->Print();
    }
}
%extend PyBase::ResizedImageInfo {
    std::string __str__() {
        return $self->Print();
    }
    std::string __repr__() {
        return $self->Print();
    }
}
%extend PyBase::ObjectInfo {
    std::string __str__() {
        return $self->Print();
    }
    std::string __repr__() {
        return $self->Print();
    }
}
%ignore PyBase::KeyPointDetectionInfo::Print();
%ignore PyBase::TextObjectInfo::Print();
%ignore PyBase::TextsInfo::Print();
%ignore PyBase::AttributeInfo::Print();
%ignore PyBase::SemanticSegInfo::Print();
%ignore PyBase::ClassInfo::Print();
%ignore PyBase::ResizedImageInfo::Print();
%ignore PyBase::ObjectInfo::Print();
%ignore PyBase::KeyPointDetectionInfo::FromBase(const MxBase::KeyPointDetectionInfo &other);
%ignore PyBase::TextObjectInfo::FromBase(const MxBase::TextObjectInfo &other);
%ignore PyBase::TextsInfo::FromBase(const MxBase::TextsInfo &other);
%ignore PyBase::AttributeInfo::FromBase(const MxBase::AttributeInfo &other);
%ignore PyBase::SemanticSegInfo::FromBase(const MxBase::SemanticSegInfo &other);
%ignore PyBase::ClassInfo::FromBase(const MxBase::ClassInfo &other);
%ignore PyBase::ResizedImageInfo::ToBase() const;
%ignore PyBase::ObjectInfo::FromBase(const MxBase::ObjectInfo &other);

// enum wrapper
%pythoncode {
from enum import Enum

class dtype(Enum):
    undefined = -1
    float32 = 0
    float16 = 1
    int8 = 2
    int32 = 3
    uint8 = 4
    int16 = 6
    uint16 = 7
    uint32 = 8
    int64 = 9
    uint64 = 10
    double = 11
    bool = 12

undefined = dtype.undefined
float32 = dtype.float32
float16 = dtype.float16
int8 = dtype.int8
int32 = dtype.int32
uint8 = dtype.uint8
int16 = dtype.int16
uint16 = dtype.uint16
uint32 = dtype.uint32
int64 = dtype.int64
uint64 = dtype.uint64
double = dtype.double
bool = dtype.bool

class ResizeType(Enum):
    RESIZER_STRETCHING = 0
    RESIZER_TF_KEEP_ASPECT_RATIO = 1
    RESIZER_MS_KEEP_ASPECT_RATIO = 2

resize_stretching = ResizeType.RESIZER_STRETCHING
resize_tf_keep_ratio = ResizeType.RESIZER_TF_KEEP_ASPECT_RATIO
resize_ms_keep_ratio = ResizeType.RESIZER_MS_KEEP_ASPECT_RATIO

class StreamFormat(Enum):
    h265_main_level = 0
    h264_baseline_level = 1
    h264_main_level = 2
    h264_high_level = 3

h265_main_level = StreamFormat.h265_main_level
h264_baseline_level = StreamFormat.h264_baseline_level
h264_main_level = StreamFormat.h264_main_level
h264_high_level = StreamFormat.h264_high_level

class interpolation(Enum):
    huaweiu_high_order_filter = 0
    bilinear_similar_opencv = 1
    nearest_neighbor_opencv = 2
    bilinear_similar_tensorflow = 3
    nearestneighbor_tensorflow = 4

huaweiu_high_order_filter = interpolation.huaweiu_high_order_filter
bilinear_similar_opencv = interpolation.bilinear_similar_opencv
nearest_neighbor_opencv = interpolation.nearest_neighbor_opencv
bilinear_similar_tensorflow = interpolation.bilinear_similar_tensorflow
nearestneighbor_tensorflow = interpolation.nearestneighbor_tensorflow

class borderType(Enum):
    border_constant = 0
    border_replicate = 1
    border_reflect = 2
    border_reflect_101 = 3

border_constant = borderType.border_constant
border_replicate = borderType.border_replicate
border_reflect = borderType.border_reflect
border_reflect_101 = borderType.border_reflect_101

class image_format(Enum):
    yuv_400 = 0
    nv12 = 1
    nv21 = 2
    yuv_sp_422 = 3
    yvu_sp_422 = 4
    yuv_sp_444 = 5
    yvu_sp_444 = 6
    yuyv_packed_422 = 7
    uyvy_packed_422 = 8
    yvyu_packed_422 = 9
    vyuy_packed_422 = 10
    yuv_packed_444 = 11
    rgb = 12
    bgr = 13
    argb = 14
    abgr = 15
    rgba = 16
    bgra = 17

yuv_400 = image_format.yuv_400
nv12 = image_format.nv12
nv21 = image_format.nv21
yuv_sp_422 = image_format.yuv_sp_422
yvu_sp_422 = image_format.yvu_sp_422
yuv_sp_444 = image_format.yuv_sp_444
yvu_sp_444 = image_format.yvu_sp_444
yuyv_packed_422 = image_format.yuyv_packed_422
uyvy_packed_422 = image_format.uyvy_packed_422
yvyu_packed_422 = image_format.yvyu_packed_422
vyuy_packed_422 = image_format.vyuy_packed_422
yuv_packed_444 = image_format.yuv_packed_444
rgb = image_format.rgb
bgr = image_format.bgr
argb = image_format.argb
abgr = image_format.abgr
rgba = image_format.rgba
bgra = image_format.bgra

class cvt_color_mode(Enum):
    color_yuvsp4202gray = 0
    color_yvusp4202gray = 1
    color_yuvsp4202rgb = 2
    color_yvusp4202rgb = 3
    color_yuvsp4202bgr = 4
    color_yvusp4202bgr = 5
    color_rgb2gray = 6
    color_bgr2gray = 7
    color_bgr2rgb = 8
    color_rgb2bgr = 9
    color_rgb2rgba = 10
    color_rgba2gray = 11
    color_rgba2rgb = 12
    color_gray2rgb = 13
    color_rgba2mrgba = 14
    color_bgr2yuvsp420 = 15
    color_rgb2yuvsp420 = 16
    color_rgb2yvusp420 = 17
    color_bgr2yvusp420 = 18

color_yuvsp4202gray = cvt_color_mode.color_yuvsp4202gray
color_yvusp4202gray = cvt_color_mode.color_yvusp4202gray
color_yuvsp4202rgb = cvt_color_mode.color_yuvsp4202rgb
color_yvusp4202rgb = cvt_color_mode.color_yvusp4202rgb
color_yuvsp4202bgr = cvt_color_mode.color_yuvsp4202bgr
color_yvusp4202bgr = cvt_color_mode.color_yvusp4202bgr
color_rgb2gray = cvt_color_mode.color_rgb2gray
color_bgr2gray = cvt_color_mode.color_bgr2gray
color_bgr2rgb = cvt_color_mode.color_bgr2rgb
color_rgb2bgr = cvt_color_mode.color_rgb2bgr
color_rgb2rgba = cvt_color_mode.color_rgb2rgba
color_rgba2gray = cvt_color_mode.color_rgba2gray
color_rgba2rgb = cvt_color_mode.color_rgba2rgb
color_gray2rgb = cvt_color_mode.color_gray2rgb
color_rgba2mrgba = cvt_color_mode.color_rgba2mrgba
color_bgr2yuvsp420 = cvt_color_mode.color_bgr2yuvsp420
color_rgb2yuvsp420 = cvt_color_mode.color_rgb2yuvsp420
color_rgb2yvusp420 = cvt_color_mode.color_rgb2yvusp420
color_bgr2yvusp420 = cvt_color_mode.color_bgr2yvusp420

class ModelLoadType(Enum):
    load_model_from_file = 1
    load_model_from_file_with_mem = 2
    load_model_from_mem = 3
    load_model_from_mem_with_mem = 4

load_model_from_file = ModelLoadType.load_model_from_file
load_model_from_file_with_mem = ModelLoadType.load_model_from_file_with_mem
load_model_from_mem = ModelLoadType.load_model_from_mem
load_model_from_mem_with_mem = ModelLoadType.load_model_from_mem_with_mem

class ModelType(Enum):
    model_type_om = 0
    model_type_mindir = 1

model_type_om = ModelType.model_type_om
model_type_mindir = ModelType.model_type_mindir

class visionDataFormat(Enum):
    NCHW = 0
    NHWC = 1
NCHW = visionDataFormat.NCHW
NHWC = visionDataFormat.NHWC
}
%include "PyImage/PyImage.h"
%include "PyImageProcessor/PyImageProcessor.h"
%include "PyInit/PyInit.h"
%include "PyModel/PyModel.h"
%include "PyPostProcessDataType/PyPostProcessDataType.h"
%include "PyTensor/PyTensor.h"
%include "PyVideoDecoder/PyVideoDecoder.h"
%include "PyVideoEncoder/PyVideoEncoder.h"
%include "MxBase/E2eInfer/Rect/Rect.h"
%include "MxBase/E2eInfer/Color/Color.h"
%include "MxBase/E2eInfer/Size/Size.h"
%include "MxBase/E2eInfer/Dim/Dim.h"
%include "MxBase/E2eInfer/DataType.h"
%include "MxBase/E2eInfer/Point/Point.h"
%include "PyTensorOperations/PyMatricesCoreOperationsOnTensors.h"
%include "PyTensorOperations/PyPerElementOperations.h"
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
 * Description: Gtest unit cases.
 * Author: Mind SDK
 * Create: 2020
 * History: NA
 */
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <random>
#include "MxBase/CV/ObjectDetection/Nms/Nms.h"
#include "MxBase/Maths/FastMath.h"

#define private public
#define protected public
#include "mxbase/include/MxBase/ConfigUtil/ConfigUtil.h"
#include "mxbase/include/MxBase/PostProcessBases/PostProcessBase.h"
#include "postprocess/module/ObjectPostProcessors/Yolov3PostProcess/Yolov3PostProcessDptr.hpp"
#include "mxbase/include/MxBase/ModelPostProcessors/ModelPostProcessorBase/ModelPostProcessorBase.h"
#undef private
#undef protected

#include "MxBase/Log/Log.h"
#include "ObjectPostProcessors/Yolov3PostProcess.h"
#include "PostProcessUtils.h"

namespace {
using namespace MxBase;
const std::vector<uint32_t> SHAPE1 = {1, 255, 255};
const std::vector<uint32_t> SHAPE2 = {1, 255, 255, 4};
const std::vector<uint32_t> SHAPE3 = {1, 255, 255, 60};
const std::vector<uint32_t> SHAPE4 = {1, 60, 255, 255};
const std::vector<uint32_t> SHAPE5 = {1, 20, 255, 255, 3};

const int HEIGHT = 32;
const int WIDTH = 16;
const int ANCHORDIM = 3;
const int BBOXDIM = 4;
const int CLASSNUM = 10;
const int LAYER_HEIGHT = 2;
const int LAYER_WIDTH = 2;
const int NETOUT_DATA_LEN = 100;
const int NETOUT_LEN = 600;
const int DETBOXES_RESULT_0 = 0;
const float MIN_VALUE = 0.;
const float MAX_VALUE = 10.;
const float SEPARATESCORE_THRESH = 0.5;
const float SEPARATESCORE_THRESH_MIN = 0.0001;
const ResizedImageInfo RESIZED_IMAGE_INFO = {WIDTH, WIDTH, HEIGHT, HEIGHT, ResizeType::RESIZER_STRETCHING, 1.0};

NetInfo g_netInfo = {
    ANCHORDIM,
    BBOXDIM,
    CLASSNUM,
    WIDTH,
    HEIGHT,
};

OutputLayer g_layer = {
    LAYER_WIDTH,
    LAYER_HEIGHT
};

OutputLayer g_layer_zero = {
    0,
    0
};

std::vector<std::vector<ObjectInfo>> g_objectInfos = {};
std::vector<ResizedImageInfo> g_resizedImageInfos = {RESIZED_IMAGE_INFO};
std::map<std::string, std::shared_ptr<void>> g_paramMap;
int g_stride = g_layer.width * g_layer.height;

class Yolov3PostProcessTest : public testing::Test {
public:
    std::map<std::string, std::string> postConfig = {
        {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                     "\"BIASES_NUM\": \"18\","
                                     "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                     "\"SCORE_THRESH\": \"0.3\","
                                     "\"OBJECTNESS_THRESH\": \"0.3\","
                                     "\"IOU_THRESH\": \"0.45\","
                                     "\"YOLO_TYPE\": \"3\","
                                     "\"ANCHOR_DIM\": \"3\","
                                     "\"MODEL_TYPE\": \"0\","
                                     "\"RESIZE_FLAG\": \"0\"}"}
    };
};

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess)
{
    LogInfo << "****************case TestYolov3PostProcess***************";
    Yolov3PostProcess yolov3PostProcess;
    Yolov3PostProcess yolov3PostProcess1(yolov3PostProcess);
    yolov3PostProcess = yolov3PostProcess1;
    std::vector<TensorBase> tensors = {};
    std::vector<ResizedImageInfo> resizedImageInfos = {};
    std::map<std::string, std::shared_ptr<void>> config = {};
    APP_ERROR ret = yolov3PostProcess.Process(tensors, g_objectInfos, resizedImageInfos, config);
    EXPECT_NE(ret, APP_ERR_OK);
    yolov3PostProcess.GetCurrentVersion();
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Fail_when_Config_is_Invalid)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "\"CLASS_NUM\": \"80\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"0\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Fail_when_Config_without_Bias)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"80\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIA\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"0\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Fail_when_Tensor_Dim_is_3)
{
    Yolov3PostProcess yolov3PostProcess;

    TensorBase tensor(SHAPE1, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {tensor, tensor, tensor};
    APP_ERROR ret = yolov3PostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Fail_when_Tensors_size_is_1)
{
    Yolov3PostProcess yolov3PostProcess;

    TensorBase tensor(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {tensor};
    APP_ERROR ret = yolov3PostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Fail_when_Channel_Num_is_4)
{
    Yolov3PostProcess yolov3PostProcess;
    TensorBase tensor1(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor2(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase tensor3(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor3};
    APP_ERROR ret = yolov3PostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Success_when_MODEL_TYPE_is_0)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"0\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor2(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase tensor3(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor3};
    PostProcessUtils::SetRandomTensorValue<float>(tensors, MIN_VALUE, MAX_VALUE);
    ret = yolov3PostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Success_when_MODEL_TYPE_is_1)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"1\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(SHAPE4, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor2(SHAPE4, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase tensor3(SHAPE4, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor3};
    PostProcessUtils::SetRandomTensorValue<float>(tensors, MIN_VALUE, MAX_VALUE);
    ret = yolov3PostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Success_when_MODEL_TYPE_is_2)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"YOLO_VERSION\": \"5\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"2\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(SHAPE5, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor2(SHAPE5, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase tensor3(SHAPE5, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor3};
    PostProcessUtils::SetRandomTensorValue<float>(tensors, MIN_VALUE, MAX_VALUE);
    ret = yolov3PostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(Yolov3PostProcessTest, Test_Yolov3PostProcess_DeInit_Should_Success)
{
    std::shared_ptr<Yolov3PostProcess> instanceSrc = GetObjectInstance();
    EXPECT_NE(instanceSrc, nullptr);
    APP_ERROR ret = instanceSrc->DeInit();
    EXPECT_EQ(ret, 0);
}


TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Return_Fail_when_strBiases_is_inValid)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \"{{{{{{{{{{{1}}}}}}}}}}} \","
                                         "\"BIASES\": \" \","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"YOLO_VERSION\": \"5\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"2\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}


TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Return_Fail_when_strBiases_is_inValid2)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \" 1 \","
                                         "\"BIASES\": \" {{{{{{{{{{{1}}}}}}}}}}} \","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"YOLO_VERSION\": \"5\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"2\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}


TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Warning_but_Return_Success_when_BIASES_NUM_is_inValid)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \" \","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"YOLO_VERSION\": \"5\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"2\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}


TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Warning_but_Return_Success_when_IOU_THRESH_is_inValid)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \" \","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"YOLO_VERSION\": \"5\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"2\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}


TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Return_Fail_when_YOLO_TYPE_is_inValid)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \" \","
                                         "\"YOLO_VERSION\": \"5\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"0\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Return_Fail_when_MODEL_TYPE_is_inValid)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"YOLO_VERSION\": \"5\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"15\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}


TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Success_when_YOLO_VERSION_is_4_MindSpore)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"YOLO_VERSION\": \"4\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"0\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(Yolov3PostProcessTest,
    TestYolov3PostProcess_Should_Return_Success_when_MODEL_TYPE_is_None_YOLO_VERSION_is_5)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"YOLO_VERSION\": \"5\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \" \","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}


TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Success_when_MODEL_TYPE_is_2_YOLO_VERSION_is_4)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"4\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"2\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}


TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Fail_when_ANCHOR_DIM_is_None)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"YOLO_VERSION\": \"3\","
                                         "\"ANCHOR_DIM\": \" \","
                                         "\"MODEL_TYPE\": \"1\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}


TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Return_Fail_when_OBJECTNESS_THRESH_is_inValid)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \"18 \","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"1.1\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"YOLO_VERSION\": \"5\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"2\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}


TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Fail_when_TestCheckDptr_Nullptr)
{
    Yolov3PostProcess yolov3PostProcess;
    yolov3PostProcess.dPtr_ = nullptr;
    APP_ERROR ret = yolov3PostProcess.CheckDptr();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}


TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Success_when_isInitConfig_is_true)
{
    Yolov3PostProcess yolov3PostProcess;
    yolov3PostProcess.isInitConfig_ = true;
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Fail_when_inputs_tensor_is_None)
{
    Yolov3PostProcess yolov3PostProcess;
    TensorBase tensor(SHAPE1, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {};
    APP_ERROR ret = yolov3PostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_NE(ret, APP_ERR_OK);
}


TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Fail_when_tensor1_size_is_4)
{
    Yolov3PostProcess yolov3PostProcess;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"15\","
                                         "\"BIASES_NUM\": \"18\","
                                         "\"BIASES\": \"10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326\","
                                         "\"SCORE_THRESH\": \"0.3\","
                                         "\"OBJECTNESS_THRESH\": \"0.3\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"YOLO_TYPE\": \"3\","
                                         "\"ANCHOR_DIM\": \"3\","
                                         "\"MODEL_TYPE\": \"2\","
                                         "\"RESIZE_FLAG\": \"0\"}"}
    };
    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    TensorBase tensor2(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor3(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor2, tensor2, tensor3};
    PostProcessUtils::SetRandomTensorValue<float>(tensors, MIN_VALUE, MAX_VALUE);
    ret = yolov3PostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Fail_when_tensor_type_Mismatched)
{
    Yolov3PostProcess yolov3PostProcess;

    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase invalidTensor;
    std::vector<TensorBase> tensors = {invalidTensor, invalidTensor, invalidTensor};
    PostProcessUtils::SetRandomTensorValue<float>(tensors, MIN_VALUE, MAX_VALUE);
    ret = yolov3PostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Fail_when_resizedImageInfos_not_equal_batchSize)
{
    Yolov3PostProcess yolov3PostProcess;

    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);

    const std::vector<uint32_t> SHAPE3_BS2 = {2, 255, 255, 60};
    TensorBase tensor1(SHAPE3_BS2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor2(SHAPE3_BS2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor3(SHAPE3_BS2, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor3};
    PostProcessUtils::SetRandomTensorValue<float>(tensors, MIN_VALUE, MAX_VALUE);
    ret = yolov3PostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_Should_Return_Fail_when_tensor_is_empty)
{
    Yolov3PostProcess yolov3PostProcess;

    APP_ERROR ret = yolov3PostProcess.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor2(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase tensor3(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensors = {tensor1, tensor2, tensor3};
    ret = yolov3PostProcess.Process(tensors, g_objectInfos, g_resizedImageInfos, g_paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
}


TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_CompareProbTest_classProb_greater_maxProb)
{
    Yolov3PostProcess* postProcess = new Yolov3PostProcess();
    Yolov3PostProcessDptr dptr(postProcess);
    int classID = 0;
    float maxProb = 0.5f;
    float classProb = 0.7f;
    dptr.CompareProb(classID, maxProb, classProb, CLASSNUM);
    EXPECT_FLOAT_EQ(maxProb, 0.7f);
    EXPECT_EQ(classID, CLASSNUM);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_CompareProbTest_classProb_less_maxProb)
{
    Yolov3PostProcess* postProcess = new Yolov3PostProcess();
    Yolov3PostProcessDptr dptr(postProcess);
    int classID = 2;
    float maxProb = 0.8f;
    float classProb = 0.5f;
    dptr.CompareProb(classID, maxProb, classProb, CLASSNUM);
    EXPECT_FLOAT_EQ(maxProb, 0.8f);
    EXPECT_EQ(classID, classID);
}

TEST_F(Yolov3PostProcessTest, TestYolov3PostProcess_GetInfo)
{
    Yolov3PostProcess* postProcess = new Yolov3PostProcess();
    Yolov3PostProcessDptr dptr(postProcess);

    ObjectInfo det;
    float x = 0.5f;
    float y = 0.5f;
    float width = 0.2f;
    float height = 0.2f;

    ObjectInfo result = dptr.GetInfo(det, x, y, width, height);

    EXPECT_FLOAT_EQ(result.x0, 0.4f);  // x - width / 2 = 0.5 - 0.1 = 0.4
    EXPECT_FLOAT_EQ(result.x1, 0.6f);  // x + width / 2 = 0.5 + 0.1 = 0.6
    EXPECT_FLOAT_EQ(result.y0, 0.4f);  // y - height / 2 = 0.5 - 0.1 = 0.4
    EXPECT_FLOAT_EQ(result.y1, 0.6f);  // y + height / 2 = 0.5 + 0.1 = 0.6
}

TEST_F(Yolov3PostProcessTest, SelectClassNCHW_divided_value_inValid)
{
    Yolov3PostProcess* postProcess = new Yolov3PostProcess();
    Yolov3PostProcessDptr dptr(postProcess);

    std::shared_ptr<std::vector<float>> netout = std::make_shared<std::vector<float>>(NETOUT_LEN);
    float* data = netout->data();
    for (int i = 0; i < NETOUT_DATA_LEN; ++i) {
        data[i] = 0.1f;
    }
    std::vector<MxBase::ObjectInfo> detBoxes;
    dptr.SelectClassNCHW(netout, g_netInfo, detBoxes, g_stride, g_layer_zero);
    EXPECT_EQ(detBoxes.size(), DETBOXES_RESULT_0);
}

TEST_F(Yolov3PostProcessTest, SelectClassNHWC_divided_value_inValid)
{
    Yolov3PostProcess* postProcess = new Yolov3PostProcess();
    Yolov3PostProcessDptr dptr(postProcess);

    std::shared_ptr<std::vector<float>> netout = std::make_shared<std::vector<float>>(NETOUT_LEN);
    float* data = netout->data();

    for (int i = 0; i < NETOUT_DATA_LEN; ++i) {
        data[i] = 0.1f;
    }
    std::vector<MxBase::ObjectInfo> detBoxes;
    dptr.SelectClassNHWC(netout, g_netInfo, detBoxes, g_stride, g_layer_zero);
    EXPECT_EQ(detBoxes.size(), DETBOXES_RESULT_0);
}

TEST_F(Yolov3PostProcessTest, SelectClassNCHWC_divided_value_inValid)
{
    Yolov3PostProcess* postProcess = new Yolov3PostProcess();
    Yolov3PostProcessDptr dptr(postProcess);

    std::shared_ptr<std::vector<float>> netout = std::make_shared<std::vector<float>>(NETOUT_LEN);
    float* data = netout->data();

    for (int i = 0; i < NETOUT_DATA_LEN; ++i) {
        data[i] = 0.0f;
    }
    std::vector<MxBase::ObjectInfo> detBoxes;
    dptr.SelectClassNCHWC(netout, g_netInfo, detBoxes, g_stride, g_layer_zero);
    EXPECT_EQ(detBoxes.size(), DETBOXES_RESULT_0);
}

TEST_F(Yolov3PostProcessTest, SelectClassNCHW)
{
    Yolov3PostProcess* postProcess = new Yolov3PostProcess();
    Yolov3PostProcessDptr dptr(postProcess);

    std::shared_ptr<std::vector<float>> netout = std::make_shared<std::vector<float>>(NETOUT_LEN);
    float* data = netout->data();

    for (int i = 0; i < NETOUT_DATA_LEN; ++i) {
        data[i] = 0.0f;
    }
    postProcess->separateScoreThresh_.push_back(SEPARATESCORE_THRESH_MIN);
    for (int i = 0; i < CLASSNUM; ++i) {
        postProcess->configData_.labelVec_.push_back("0");
        postProcess->separateScoreThresh_.push_back(SEPARATESCORE_THRESH);
    }
    std::vector<MxBase::ObjectInfo> detBoxes;
    dptr.SelectClassNCHW(netout, g_netInfo, detBoxes, g_stride, g_layer);
    EXPECT_NE(detBoxes.size(), DETBOXES_RESULT_0);
}

TEST_F(Yolov3PostProcessTest, SelectClassNHWC)
{
    Yolov3PostProcess* postProcess = new Yolov3PostProcess();
    Yolov3PostProcessDptr dptr(postProcess);

    std::shared_ptr<std::vector<float>> netout = std::make_shared<std::vector<float>>(NETOUT_LEN);
    float* data = netout->data();

    for (int i = 0; i < NETOUT_DATA_LEN; ++i) {
        data[i] = 0.0f;
    }
    postProcess->separateScoreThresh_.push_back(SEPARATESCORE_THRESH_MIN);
    for (int i = 0; i < CLASSNUM; ++i) {
        postProcess->configData_.labelVec_.push_back("0");
        postProcess->separateScoreThresh_.push_back(SEPARATESCORE_THRESH);
    }

    std::vector<MxBase::ObjectInfo> detBoxes;
    dptr.SelectClassNHWC(netout, g_netInfo, detBoxes, g_stride, g_layer);
    EXPECT_NE(detBoxes.size(), DETBOXES_RESULT_0);
}

TEST_F(Yolov3PostProcessTest, SelectClassNCHWC)
{
    Yolov3PostProcess* postProcess = new Yolov3PostProcess();
    Yolov3PostProcessDptr dptr(postProcess);

    std::shared_ptr<std::vector<float>> netout = std::make_shared<std::vector<float>>(NETOUT_LEN);
    float* data = netout->data();

    for (int i = 0; i < NETOUT_DATA_LEN; ++i) {
        data[i] = 0.0f;
    }
    postProcess->separateScoreThresh_.push_back(SEPARATESCORE_THRESH_MIN);
    for (int i = 0; i < CLASSNUM; ++i) {
        postProcess->configData_.labelVec_.push_back("0");
        postProcess->separateScoreThresh_.push_back(SEPARATESCORE_THRESH);
    }

    std::vector<MxBase::ObjectInfo> detBoxes;
    dptr.SelectClassNCHWC(netout, g_netInfo, detBoxes, g_stride, g_layer);
    EXPECT_NE(detBoxes.size(), DETBOXES_RESULT_0);
}

TEST_F(Yolov3PostProcessTest, DISABLED_SelectClassYoloV4MS)
{
    Yolov3PostProcess* postProcess = new Yolov3PostProcess();
    Yolov3PostProcessDptr dptr(postProcess);

    std::shared_ptr<std::vector<float>> netout = std::make_shared<std::vector<float>>(NETOUT_LEN);
    float* data = netout->data();

    for (int i = 0; i < NETOUT_DATA_LEN; ++i) {
        data[i] = 0.0f;
    }
    postProcess->separateScoreThresh_.push_back(SEPARATESCORE_THRESH_MIN);
    for (int i = 0; i < CLASSNUM; ++i) {
        postProcess->configData_.labelVec_.push_back("0");
        postProcess->configData_.labelVec_.push_back("1");
        postProcess->separateScoreThresh_.push_back(SEPARATESCORE_THRESH);
    }

    std::vector<MxBase::ObjectInfo> detBoxes;
    dptr.SelectClassYoloV4MS(netout, g_netInfo, detBoxes, g_stride, g_layer);
    EXPECT_NE(detBoxes.size(), DETBOXES_RESULT_0);
}

} // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

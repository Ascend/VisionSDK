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
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <new>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "MxBase/Log/Log.h"
#ifndef ENABLE_POST_PROCESS_INSTANCE
#define ENABLE_POST_PROCESS_INSTANCE
#endif
#define private public
#define protected public
#include "ObjectPostProcessors/FasterRcnnPostProcess.h"
#undef protected
#undef private


static bool g_forceMallocFailure = false;

void* operator new(size_t size)
{
    if (g_forceMallocFailure) {
        throw std::bad_alloc();
    } else {
        void *ptr = std::malloc(size);
        if (!ptr) {
            throw std::bad_alloc();
        }
        return ptr;
    }
}

void operator delete(void* ptr) noexcept
{
std::free(ptr);
}


namespace {
using namespace MxBase;

class FasterRcnnPostProcessTest : public testing::Test {};

TEST_F(FasterRcnnPostProcessTest, FasterRcnnTest)
{
    FasterRcnnPostProcess instanceSrc;
    FasterRcnnPostProcess instanceDst(instanceSrc);
    instanceSrc = instanceDst;
    EXPECT_TRUE(1);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Init_Should_Success)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    EXPECT_EQ(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Init_Should_Fail_When_Json_Format_Is_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Init_Should_Fail_When_Json_Data_Is_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_ssNUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_Wrong_Tensor_Num)
{
    FasterRcnnPostProcess instanceSrc;
    std::vector<uint32_t> shape = {1, 255, 255};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_BboxTensor_Shape_is_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::vector<uint32_t> shape = {1, 255, 255};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor, tensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Success_When_Config_Is_Default)
{
    FasterRcnnPostProcess instanceSrc;
    std::vector<uint32_t> shape = {2, 100, 255};
    std::vector<uint32_t> shape1 = {2, 100, 4};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    std::vector<TensorBase> vec = {tensor, tensor, bboxTensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_EQ(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_BboxCutTensor_Shape_Size_Is_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {2, 255, 255};
    std::vector<uint32_t> shape1 = {2, 255, 4};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    std::vector<TensorBase> vec = {tensor, bboxTensor, tensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_BboxCutTensor_Shape2_Is_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {2, 255, 255};
    std::vector<uint32_t> shape1 = {2, 255, 4, 1};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    std::vector<TensorBase> vec = {tensor, bboxTensor, tensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_BboxCutTensor_Shape3_Is_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {2, 255, 255};
    std::vector<uint32_t> shape1 = {2, 255, 91, 1};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    std::vector<TensorBase> vec = {tensor, bboxTensor, tensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Success_When_Model_Type_Is_NMS_CUT)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {2, 100, 255};
    std::vector<uint32_t> shape1 = {2, 100, 90, 4};
    std::vector<uint32_t> shape2 = {2, 100, 90, 4};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    TensorBase confidenceTensor(shape2, type);
    TensorBase::TensorBaseMalloc(confidenceTensor);
    std::vector<TensorBase> vec = {tensor, bboxTensor, confidenceTensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_EQ(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_BboxShape_is_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\","
                                         "\"FRAMEWORK\": \"mindspore\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {2, 255, 255};
    std::vector<uint32_t> shape1 = {2, 255, 4};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    std::vector<TensorBase> vec = {tensor, tensor, bboxTensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_BboxShape2_is_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\","
                                         "\"FRAMEWORK\": \"mindspore\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {2, 90000, 255};
    std::vector<uint32_t> shape1 = {2, 255, 4};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    std::vector<TensorBase> vec = {tensor, tensor, bboxTensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_ClassTensor_Shape_is_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\","
                                         "\"FRAMEWORK\": \"mindspore\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {2, 90000, 5};
    std::vector<uint32_t> shape1 = {2, 255, 4};
    std::vector<uint32_t> shape2 = {2, 255, 4};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    TensorBase classTensor(shape2, type);
    TensorBase::TensorBaseMalloc(classTensor);
    std::vector<TensorBase> vec = {classTensor, tensor, bboxTensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_BboxShape2_is_Wrong_And_Framework_Is_Ms)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\","
                                         "\"FRAMEWORK\": \"mindspore\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {2, 90000, 5};
    std::vector<uint32_t> shape1 = {2, 90000, 4};
    std::vector<uint32_t> shape2 = {2, 90000, 4};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    TensorBase classTensor(shape2, type);
    TensorBase::TensorBaseMalloc(classTensor);
    std::vector<TensorBase> vec = {classTensor, tensor, bboxTensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Success_When_Frame_Work_Is_MS)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\","
                                         "\"FRAMEWORK\": \"mindspore\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {2, 90000, 5};
    std::vector<uint32_t> shape1 = {2, 90000, 5};
    std::vector<uint32_t> shape2 = {2, 90000, 5};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    TensorBase classTensor(shape2, type);
    TensorBase::TensorBaseMalloc(classTensor);
    std::vector<TensorBase> vec = {classTensor, tensor, bboxTensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_EQ(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_ObjectTensor_Shape_Is_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"2\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {2, 255, 255};
    std::vector<uint32_t> shape1 = {2, 255, 4};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    std::vector<TensorBase> vec = {tensor, tensor, bboxTensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_BBoxCutTensor_Size_Is_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"2\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {2, 5};
    std::vector<uint32_t> shape1 = {2, 5};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    std::vector<TensorBase> vec = {tensor, tensor, bboxTensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_BBoxCutTensor_Shape_Is_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"2\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {2, 5};
    std::vector<uint32_t> shape1 = {5};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    std::vector<TensorBase> vec = {tensor, tensor, bboxTensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_GetCurrentVersion)
{
    FasterRcnnPostProcess instanceSrc;
    auto version = instanceSrc.GetCurrentVersion();
    EXPECT_EQ(version, MINDX_SDK_VERSION);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_CheckDptr_Should_Fail_When_Nullptr)
{
    FasterRcnnPostProcess instanceSrc;
    instanceSrc.dPtr_ = nullptr;
    APP_ERROR ret = instanceSrc.CheckDptr();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Deinit)
{
    FasterRcnnPostProcess instanceSrc;
    APP_ERROR ret = instanceSrc.DeInit();
    EXPECT_EQ(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Init_Repeatedly)
{
    FasterRcnnPostProcess instanceSrc;
    // wrong json format
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\"}"}
    };
    instanceSrc.isInitConfig_ = true;
    APP_ERROR ret = instanceSrc.Init(postConfig);
    EXPECT_EQ(ret, 0);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_ResizedImageInfos_Is_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\","
                                         "\"FRAMEWORK\": \"mindspore\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {2, 90000, 5};
    std::vector<uint32_t> shape1 = {2, 90000, 5};
    std::vector<uint32_t> shape2 = {2, 90000, 5};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    TensorBase classTensor(shape2, type);
    TensorBase::TensorBaseMalloc(classTensor);
    std::vector<TensorBase> vec = {classTensor, tensor, bboxTensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    std::vector<ResizedImageInfo> resizedImageInfos = {};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_ObjectInfosSize_Less_Than_ResizedImageInfosSize)
{
    FasterRcnnPostProcess instanceSrc;
    std::vector<uint32_t> shape = {2, 100, 255};
    std::vector<uint32_t> shape1 = {2, 100, 4};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    std::vector<TensorBase> vec = {tensor, tensor, bboxTensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {};
    int resizedImageInfosSize = 256;
    std::vector<ResizedImageInfo> resizedImageInfos(resizedImageInfosSize, resizedImageInfo);
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_COMM_OUT_OF_RANGE);
}

#ifdef ENABLE_POST_PROCESS_INSTANCE
TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_GetObjectInstance)
{
    std::shared_ptr<MxBase::FasterRcnnPostProcess> instance = GetObjectInstance();
    EXPECT_NE(instance, nullptr);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_GetObjectInstance_Should_Fail_When_MakeShared_Wrong)
{
    std::shared_ptr<MxBase::FasterRcnnPostProcess> ptr = nullptr;
    g_forceMallocFailure = true;
    try {
        std::shared_ptr <MxBase::FasterRcnnPostProcess> instance = GetObjectInstance();
        ptr = instance;
    } catch (const std::exception& ex) {
        ptr = nullptr;
    }
    EXPECT_EQ(ptr, nullptr);
    g_forceMallocFailure = false;
}
#undef ENABLE_POST_PROCESS_INSTANCE
#endif

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnnPostProcess_Create_Should_Fail_When_MakeShared_Wrong)
{
    g_forceMallocFailure = true;
    APP_ERROR ret;
    try {
        FasterRcnnPostProcess instanceSrc;
    } catch (const std::exception& ex) {
        ret = APP_ERR_ACL_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
    g_forceMallocFailure = false;
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnnPostProcess_Copy_Should_Fail_When_MakeShared_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    g_forceMallocFailure = true;
    std::shared_ptr<MxBase::FasterRcnnPostProcessDptr> ptr = nullptr;
    try {
        FasterRcnnPostProcess instanceSrc1(instanceSrc);
        ptr = instanceSrc1.dPtr_;
    } catch (const std::exception& ex) {
        ptr = nullptr;
    }
    EXPECT_EQ(ptr, nullptr);
    g_forceMallocFailure = false;
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnnPostProcess_Assign_To_Self_Should_Success)
{
    FasterRcnnPostProcess instanceSrc1;
    FasterRcnnPostProcess instanceSrc2;
    instanceSrc2 = instanceSrc1;
    EXPECT_NE(instanceSrc2.dPtr_, nullptr);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnnPostProcess_Init_Should_Fail_When_CheckcDptr_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"91\","
                                         "\"SCORE_THRESH\": \"0.5\","
                                         "\"IOU_THRESH\": \"0.45\","
                                         "\"NMS_FINISHED\": \"false\","
                                         "\"MODEL_TYPE\": \"1\"}"}
    };
    instanceSrc.dPtr_ = nullptr;
    APP_ERROR ret = instanceSrc.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(FasterRcnnPostProcessTest, Test_FasterRcnn_Should_Fail_When_CheckDptr_Wrong)
{
    FasterRcnnPostProcess instanceSrc;
    std::vector<uint32_t> shape = {2, 100, 255};
    std::vector<uint32_t> shape1 = {2, 100, 4};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase bboxTensor(shape1, type);
    TensorBase::TensorBaseMalloc(bboxTensor);
    std::vector<TensorBase> vec = {tensor, tensor, bboxTensor, tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    instanceSrc.dPtr_ = nullptr;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

} // namespace

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}
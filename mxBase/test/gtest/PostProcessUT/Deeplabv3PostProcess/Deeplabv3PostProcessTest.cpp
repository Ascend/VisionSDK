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

#include <glog/logging.h>
#include <gtest/gtest.h>
#include "MxBase/Log/Log.h"
#include "SegmentPostProcessors/Deeplabv3Post.h"
#include "MxBase/MxBase.h"

namespace {
using namespace MxBase;

const std::vector<uint32_t> SHAPE1 = {1, 255, 255};
const std::vector<uint32_t> SHAPE2 = {1, 255, 255, 21};
const std::vector<uint32_t> SHAPE3 = {2, 255, 255, 21};
const int HEIGHT = 32;
const int WIDTH = 16;

class Deeplabv3PostProcessTest : public testing::Test {};

TEST_F(Deeplabv3PostProcessTest, Deeplabv3Test)
{
    Deeplabv3Post instanceSrc;
    Deeplabv3Post instanceDst(instanceSrc);
    instanceSrc = instanceDst;
    EXPECT_TRUE(1);
}

TEST_F(Deeplabv3PostProcessTest, TestDeeplabv3_Should_Return_Success_When_Config_is_Ok)
{
    Deeplabv3Post deeplabv3Post;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"21\","
                                         "\"CHECK_MODEL\": \"true\","
                                         "\"MODEL_TYPE\": \"0\"}"}
    };
    APP_ERROR ret = deeplabv3Post.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(Deeplabv3PostProcessTest, TestDeeplabv3_Should_Return_Fail_When_Config_is_Invalid)
{
    Deeplabv3Post deeplabv3Post;
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
    APP_ERROR ret = deeplabv3Post.Init(postConfig);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(Deeplabv3PostProcessTest, TestDeeplabv3_Should_Return_Fail_When_TensorBase_Dim_is_3)
{
    Deeplabv3Post deeplabv3Post;

    TensorBase tensor(SHAPE1, TensorDataType::TENSOR_DTYPE_FLOAT32, 0);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {tensor};
    std::vector<SemanticSegInfo> semanticSegInfos;
    ResizedImageInfo resizedImageInfo = {WIDTH, WIDTH, HEIGHT, HEIGHT, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    APP_ERROR ret = deeplabv3Post.Process(tensors, semanticSegInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(Deeplabv3PostProcessTest, TestDeeplabv3_Should_Return_Fail_When_Process_before_Init)
{
    Deeplabv3Post deeplabv3Post;

    TensorBase tensor(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32, 0);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {tensor};
    std::vector<SemanticSegInfo> semanticSegInfos;
    ResizedImageInfo resizedImageInfo = {WIDTH, WIDTH, HEIGHT, HEIGHT, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    APP_ERROR ret = deeplabv3Post.Process(tensors, semanticSegInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(Deeplabv3PostProcessTest, TestDeeplabv3_Should_Return_Success_when_FRAMEWORK_TYPE_is_0)
{
    Deeplabv3Post deeplabv3Post;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"21\","
                                         "\"CHECK_MODEL\": \"true\","
                                         "\"MODEL_TYPE\": \"0\","
                                         "\"FRAMEWORK_TYPE\": \"0\"}"},
            {"labelPath", "/home/simon/models/deeplabv3/deeplabv3.names"}
    };
    APP_ERROR ret = deeplabv3Post.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);

    TensorBase tensor(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32, 0);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {tensor};
    std::vector<SemanticSegInfo> semanticSegInfos;
    ResizedImageInfo resizedImageInfo = {WIDTH, WIDTH, HEIGHT, HEIGHT, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    ret = deeplabv3Post.Process(tensors, semanticSegInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}
TEST_F(Deeplabv3PostProcessTest, TestDeeplabv3_Should_Return_Success_when_FRAMEWORK_TYPE_is_1)
{
    Deeplabv3Post deeplabv3Post;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"21\","
                                         "\"CHECK_MODEL\": \"true\","
                                         "\"MODEL_TYPE\": \"0\","
                                         "\"FRAMEWORK_TYPE\": \"1\"}"},
            {"labelPath", "/home/simon/models/deeplabv3/deeplabv3.names"}
    };
    APP_ERROR ret = deeplabv3Post.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);

    TensorBase tensor(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32, 0);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {tensor};
    std::vector<SemanticSegInfo> semanticSegInfos;
    ResizedImageInfo resizedImageInfo = {WIDTH, WIDTH, HEIGHT, HEIGHT, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    ret = deeplabv3Post.Process(tensors, semanticSegInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(Deeplabv3PostProcessTest, TestDeeplabv3_Should_Return_Failed_When_BatchSize_Is_2)
{
    Deeplabv3Post deeplabv3Post;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"21\","
                                         "\"CHECK_MODEL\": \"true\","
                                         "\"MODEL_TYPE\": \"0\","
                                         "\"FRAMEWORK_TYPE\": \"1\"}"},
            {"labelPath", "/home/simon/models/deeplabv3/deeplabv3.names"}
    };
    APP_ERROR ret = deeplabv3Post.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);

    TensorBase tensor(SHAPE3, TensorDataType::TENSOR_DTYPE_FLOAT32, 0);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {tensor};
    std::vector<SemanticSegInfo> semanticSegInfos;
    ResizedImageInfo resizedImageInfo = {WIDTH, WIDTH, HEIGHT, HEIGHT, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    ret = deeplabv3Post.Process(tensors, semanticSegInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(Deeplabv3PostProcessTest, TestDeeplabv3_Should_Return_Success_when_FRAMEWORK_TYPE_is_2)
{
    Deeplabv3Post deeplabv3Post;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"21\","
                                         "\"CHECK_MODEL\": \"true\","
                                         "\"MODEL_TYPE\": \"0\","
                                         "\"FRAMEWORK_TYPE\": \"2\"}"},
            {"labelPath", "/home/simon/models/deeplabv3/deeplabv3.names"}
    };
    APP_ERROR ret = deeplabv3Post.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);

    TensorBase tensor(SHAPE2, TensorDataType::TENSOR_DTYPE_FLOAT32, 0);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> tensors = {tensor};
    std::vector<SemanticSegInfo> semanticSegInfos;
    ResizedImageInfo resizedImageInfo = {WIDTH, WIDTH, HEIGHT, HEIGHT, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> paramMap;
    ret = deeplabv3Post.Process(tensors, semanticSegInfos, resizedImageInfos, paramMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}
} // namespace

int main(int argc, char* argv[])
{
    MxInit();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    MxDeInit();
    return ret;
}
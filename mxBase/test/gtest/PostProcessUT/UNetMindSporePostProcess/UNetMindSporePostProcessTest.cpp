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
#include "SegmentPostProcessors/UNetMindSporePostProcess.h"
#include "MxBase/Log/Log.h"

namespace {
using namespace MxBase;

class UNetMindSporePostProcessTest : public testing::Test {};

TEST_F(UNetMindSporePostProcessTest, Test_UNetMindSporePostProcess_Construct_Should_Success)
{
    UNetMindSporePostProcess unetPostSrc;
    UNetMindSporePostProcess unetPostDst(unetPostSrc);
    unetPostDst = unetPostSrc;
    EXPECT_TRUE(1);
}

TEST_F(UNetMindSporePostProcessTest, Test_UNetMindSporePostProcess_Init_Should_Success)
{
    UNetMindSporePostProcess unetPostSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"3\","
                                         "\"MODEL_TYPE\": \"2\","
                                         "\"POST_TYPE\": \"1\","
                                         "\"RESIZE_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = unetPostSrc.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(UNetMindSporePostProcessTest, Test_UNetMindSporePostProcess_Init_Should_Success_When_CLASS_NUM_Is_NULL)
{
    UNetMindSporePostProcess unetPostSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"RESIZE_TYPE\": \"3\","
                                         "\"MODEL_TYPE\": \"2\","
                                         "\"POST_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = unetPostSrc.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(UNetMindSporePostProcessTest, Test_UNetMindSporePostProcess_Init_Should_Success_When_MODEL_TYPE_Is_NULL)
{
    UNetMindSporePostProcess unetPostSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"3\","
                                         "\"POST_TYPE\": \"1\","
                                         "\"RESIZE_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = unetPostSrc.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(UNetMindSporePostProcessTest, Test_UNetMindSporePostProcess_Init_Should_Success_When_POST_TYPE_Is_NULL)
{
    UNetMindSporePostProcess unetPostSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"3\","
                                         "\"MODEL_TYPE\": \"2\","
                                         "\"RESIZE_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = unetPostSrc.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(UNetMindSporePostProcessTest, Test_UNetMindSporePostProcess_Init_Should_Success_When_RESIZE_TYPE_Is_NULL)
{
    UNetMindSporePostProcess unetPostSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"3\","
                                         "\"MODEL_TYPE\": \"2\","
                                         "\"POST_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = unetPostSrc.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(UNetMindSporePostProcessTest,
    Test_UNetMindSporePostProcess_Init_Should_Success_When_PostConfig_Is_Json_Format)
{
    UNetMindSporePostProcess unetPostSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"3\","
                                         "\"MODEL_TYPE\": \"2\","
                                         "\"POST_TYPE\": "}
    };
    APP_ERROR ret = unetPostSrc.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(UNetMindSporePostProcessTest,
    Test_UNetMindSporePostProcess_Process_Should_Failed_When_Size_Of_ResizedImageInfo_Is_Wrong)
{
    UNetMindSporePostProcess unetPostSrc;
    std::vector<SemanticSegInfo> semanticSegInfos;
    std::vector<uint32_t> shape = {1, 80000, 5, 1};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor, tensor, tensor};
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = unetPostSrc.Process(vec, semanticSegInfos, resizedImageInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(UNetMindSporePostProcessTest, Test_UNetMindSporePostProcess_Process_Should_Success_When_PostType_Is_0)
{
    UNetMindSporePostProcess unetPostSrc;

    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"3\","
                                         "\"MODEL_TYPE\": \"0\","
                                         "\"POST_TYPE\": \"0\","
                                         "\"RESIZE_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = unetPostSrc.Init(postConfig);

    std::vector<SemanticSegInfo> semanticSegInfos;
    std::vector<uint32_t> shape = {2, 80000, 5, 3};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor, tensor, tensor};
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = unetPostSrc.Process(vec, semanticSegInfos, resizedImageInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(UNetMindSporePostProcessTest,
    Test_UNetMindSporePostProcess_Process_Should_Failed_When_Post_Type_Is_Not_Match)
{
    UNetMindSporePostProcess unetPostSrc;

    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"3\","
                                         "\"MODEL_TYPE\": \"0\","
                                         "\"POST_TYPE\": \"1\","
                                         "\"RESIZE_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = unetPostSrc.Init(postConfig);

    std::vector<SemanticSegInfo> semanticSegInfos;
    std::vector<uint32_t> shape = {2, 80000, 5, 3};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor, tensor, tensor};
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = unetPostSrc.Process(vec, semanticSegInfos, resizedImageInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(UNetMindSporePostProcessTest, Test_UNetMindSporePostProcess_Process_Should_Success_When_PostType_Is_1)
{
    UNetMindSporePostProcess unetPostSrc;

    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"3\","
                                         "\"MODEL_TYPE\": \"1\","
                                         "\"POST_TYPE\": \"1\","
                                         "\"RESIZE_TYPE\": \"1\"}"}
    };
    APP_ERROR ret = unetPostSrc.Init(postConfig);

    std::vector<SemanticSegInfo> semanticSegInfos;
    std::vector<uint32_t> shape = {2, 3, 5};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor, tensor, tensor};
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = unetPostSrc.Process(vec, semanticSegInfos, resizedImageInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}
} // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
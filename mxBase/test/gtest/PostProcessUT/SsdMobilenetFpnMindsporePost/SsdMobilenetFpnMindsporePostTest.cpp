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

#include <random>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include "MxBase/Log/Log.h"
#include "PostProcessUtils.h"
#include "ObjectPostProcessors/SsdMobilenetFpnMindsporePost.h"

namespace {
using namespace MxBase;
const float MIN_VALUE = 0.;
const float MAX_VALUE = 10.;
class SsdMobilenetFpnMindsporePostTest : public testing::Test {
public:
    std::map<std::string, std::string> postConfig_ = {{"postProcessConfigContent",
        "{\"CLASS_NUM\": \"80\","
        "\"SCORE_THRESH\": \"0.7\","
        "\"IOU_THRESH\": \"0.5\","
        "\"RPN_MAX_NUM\": \"1000\","
        "\"MAX_PER_IMG\": \"128\","
        "\"MASK_THREAD_BINARY\": \"0.5\","
        "\"MASK_SHAPE_SIZE\": \"28\"}"}};
};

TEST_F(SsdMobilenetFpnMindsporePostTest, SsdMobilenetFpnMindsporeTest)
{
    SsdMobilenetFpnMindsporePost instanceSrc;
    SsdMobilenetFpnMindsporePost instanceDst(instanceSrc);
    instanceSrc = instanceDst;
    EXPECT_TRUE(1);
}

TEST_F(SsdMobilenetFpnMindsporePostTest, Test_SsdMobilenetFpnMindsporeTest_Init_Should_Success)
{
    SsdMobilenetFpnMindsporePost instanceSrc;
    APP_ERROR ret = instanceSrc.Init(postConfig_);
    EXPECT_EQ(ret, 0);
}

TEST_F(SsdMobilenetFpnMindsporePostTest, Test_SsdMobilenetFpnMindsporeTest_Should_Fail_When_Wrong_Json_Format)
{
    SsdMobilenetFpnMindsporePost instanceSrc;
    std::map<std::string, std::string> postConfig = {{"postProcessConfigContent",
        "\"KEYPOINT_NUM\": \"17\","
        "\"SCORE_THRESH\": \"0.1\"}"}};
    APP_ERROR ret = instanceSrc.Init(postConfig);
    EXPECT_NE(ret, 0);
}

TEST_F(SsdMobilenetFpnMindsporePostTest, Test_SsdMobilenetFpnMindsporeTest_Should_Fail_When_Wrong_Tensor_Shape)
{
    LogInfo << "****************case SsdMobilenetFpnMindsporeTest***************";
    SsdMobilenetFpnMindsporePost instanceSrc;
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

TEST_F(SsdMobilenetFpnMindsporePostTest, Test_SsdMobilenetFpnMindsporeTest_Should_Fail_When_Wrong_Tensor_Num)
{
    LogInfo << "****************case SsdMobilenetFpnMindsporeTest***************";
    SsdMobilenetFpnMindsporePost instanceSrc;
    std::vector<uint32_t> shape = {1, 255, 4};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(SsdMobilenetFpnMindsporePostTest, Test_SsdMobilenetFpnMindsporeTest_Should_Fail_When_Wrong_Tensor_DataType)
{
    LogInfo << "****************case SsdMobilenetFpnMindsporeTest***************";
    SsdMobilenetFpnMindsporePost instanceSrc;
    std::vector<uint32_t> shape = {1, 255, 4};
    auto type = TensorDataType::TENSOR_DTYPE_UINT8;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    auto ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(SsdMobilenetFpnMindsporePostTest, Test_SsdMobilenetFpnMindsporeTest_Should_Fail_When_Wrong_Tensor_Shape1)
{
    LogInfo << "****************case SsdMobilenetFpnMindsporeTest***************";
    SsdMobilenetFpnMindsporePost instanceSrc;
    std::vector<uint32_t> shape = {1, 255, 4};
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

TEST_F(SsdMobilenetFpnMindsporePostTest, Test_SsdMobilenetFpnMindsporeTest_Should_Fail_When_Wrong_Tensor_BatchSize)
{
    LogInfo << "****************case SsdMobilenetFpnMindsporeTest***************";
    SsdMobilenetFpnMindsporePost instanceSrc;
    APP_ERROR ret = instanceSrc.Init(postConfig_);
    EXPECT_EQ(ret, 0);
    std::vector<uint32_t> shape = {1, 255, 4};
    std::vector<uint32_t> shape2 = {1, 255, 80};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    TensorBase tensor2(shape2, type);
    TensorBase::TensorBaseMalloc(tensor2);
    std::vector<TensorBase> vec = {tensor, tensor2};
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_NE(ret, 0);
}

TEST_F(SsdMobilenetFpnMindsporePostTest, Test_SsdMobilenetFpnMindsporeTest_Should_Success)
{
    LogInfo << "****************case SsdMobilenetFpnMindsporeTest***************";
    SsdMobilenetFpnMindsporePost instanceSrc;
    APP_ERROR ret = instanceSrc.Init(postConfig_);
    EXPECT_EQ(ret, 0);
    std::vector<uint32_t> shape = {2, 255, 4};
    std::vector<uint32_t> shape2 = {2, 255, 80};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor1(shape, type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor2(shape2, type);
    TensorBase::TensorBaseMalloc(tensor2);
    std::vector<TensorBase> vec = {tensor1, tensor2};
    PostProcessUtils::SetRandomTensorValue<float>(vec, MIN_VALUE, MAX_VALUE);
    std::vector<std::vector<ObjectInfo>> objectInfos;
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfos = {resizedImageInfo, resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, objectInfos, resizedImageInfos, configParamMap);
    EXPECT_EQ(ret, 0);
}

TEST_F(SsdMobilenetFpnMindsporePostTest, Test_SsdMobilenetFpnMindsporeTest_DeInit_Should_Success)
{
    std::shared_ptr<SsdMobilenetFpnMindsporePost> instanceSrc = GetObjectInstance();
    EXPECT_NE(instanceSrc, nullptr);
    APP_ERROR ret = instanceSrc->DeInit();
    EXPECT_EQ(ret, 0);
}
}  // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}
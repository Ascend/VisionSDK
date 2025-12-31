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
#include "TextGenerationPostProcessors/CrnnPostProcess.h"
#include "MxBase/Log/Log.h"

namespace {
using namespace MxBase;

class CrnnPostProcessTest : public testing::Test {};

TEST_F(CrnnPostProcessTest, Test_CrnnPostProcess_Constructor_Should_Success)
{
    CrnnPostProcess crnnPostProcess;
    CrnnPostProcess crnnPostProcess1(crnnPostProcess);   // 覆盖拷贝构造函数
    crnnPostProcess = crnnPostProcess1;                  // 覆盖赋值重载函数
}

TEST_F(CrnnPostProcessTest, Test_CrnnPostProcessTest_Init_Should_Success)
{
    CrnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"37\","
                                         "\"OBJECT_NUM\": \"24\","
                                         "\"BLANK_INDEX\": \"36\","
                                         "\"WITH_ARGMAX\": \"false\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(CrnnPostProcessTest, Test_CrnnPostProcessTest_Init_Should_Success_With_Default)
{
    CrnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {};
    APP_ERROR ret = instanceSrc.Init(postConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(CrnnPostProcessTest, Test_CrnnPostProcessTest_Should_Fail_When_Num_Of_Input_Is_Wrong)
{
    CrnnPostProcess instanceSrc;
    std::vector<uint32_t> shape = {1, 255, 255};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<TextsInfo> textInfos;
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    APP_ERROR ret = instanceSrc.Process(vec, textInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CrnnPostProcessTest, Test_CrnnPostProcessTest_Should_Fail_When_OBJECT_NUM_Is_0)
{
    CrnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"37\","
                                         "\"OBJECT_NUM\": \"0\","
                                         "\"BLANK_INDEX\": \"36\","
                                         "\"WITH_ARGMAX\": \"false\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {1, 255, 255};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor};
    std::vector<TextsInfo> textInfos;
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, textInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CrnnPostProcessTest, Test_CrnnPostProcessTest_Should_Fail_When_ClassNum_Is_Not_Match)
{
    CrnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"37\","
                                         "\"OBJECT_NUM\": \"24\","
                                         "\"BLANK_INDEX\": \"36\","
                                         "\"WITH_ARGMAX\": \"false\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {1, 24, 24};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor};
    std::vector<TextsInfo> textInfos;
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, textInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CrnnPostProcessTest, Test_CrnnPostProcessTest_Process_Should_Success_In_WithArgMax_Is_False)
{
    CrnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"37\","
                                         "\"OBJECT_NUM\": \"24\","
                                         "\"BLANK_INDEX\": \"36\","
                                         "\"WITH_ARGMAX\": \"false\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {1, 24, 37};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor};
    std::vector<TextsInfo> textInfos;
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, textInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(CrnnPostProcessTest,
    Test_CrnnPostProcessTest_Process_Should_Failed_When_Input_Shape_Wrong_In_WithArgMax_Is_True)
{
    CrnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"37\","
                                         "\"OBJECT_NUM\": \"24\","
                                         "\"BLANK_INDEX\": \"36\","
                                         "\"WITH_ARGMAX\": \"true\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {1, 24, 37};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor};
    std::vector<TextsInfo> textInfos;
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, textInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CrnnPostProcessTest,
    Test_CrnnPostProcessTest_Process_Should_Failed_When_ObjectNum_Is_Not_Match_In_WithArgMax_Is_True)
{
    CrnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"37\","
                                         "\"OBJECT_NUM\": \"24\","
                                         "\"BLANK_INDEX\": \"36\","
                                         "\"WITH_ARGMAX\": \"true\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {24, 37};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor};
    std::vector<TextsInfo> textInfos;
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, textInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CrnnPostProcessTest, Test_CrnnPostProcessTest_Process_Should_Success_In_WithArgMax_Is_True)
{
    CrnnPostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"37\","
                                         "\"OBJECT_NUM\": \"24\","
                                         "\"BLANK_INDEX\": \"36\","
                                         "\"WITH_ARGMAX\": \"true\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);
    std::vector<uint32_t> shape = {37, 24};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor};
    std::vector<TextsInfo> textInfos;
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, textInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}
} // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
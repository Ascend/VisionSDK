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
#include "ClassPostProcessors/Resnet50PostProcess.h"

namespace {
using namespace MxBase;

class Resnet50PostProcessTest : public testing::Test {};

TEST_F(Resnet50PostProcessTest, Resnet50Test)
{
    Resnet50PostProcess instanceSrc;
    Resnet50PostProcess instanceDst(instanceSrc);
    instanceSrc = instanceDst;
    instanceSrc.GetCurrentVersion();
    EXPECT_TRUE(1);
}

TEST_F(Resnet50PostProcessTest, Resnet50_Test_Init_Should_Success)
{
    Resnet50PostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"1001\","
                                         "\"SOFTMAX\": \"false\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);

    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(Resnet50PostProcessTest, Resnet50_Test_Init_Should_Success_With_Default)
{
    Resnet50PostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {};
    APP_ERROR ret = instanceSrc.Init(postConfig);

    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(Resnet50PostProcessTest, Resnet50_Test_Process_Should_Failed_When_ClassNum_Not_Match)
{
    Resnet50PostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"1001\","
                                         "\"SOFTMAX\": \"false\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);

    std::vector<uint32_t> shape = {1, 255, 255};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<std::vector<ClassInfo>> classInfos;
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, classInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(Resnet50PostProcessTest, Resnet50_Test_Process_Should_Success)
{
    Resnet50PostProcess instanceSrc;
    std::map<std::string, std::string> postConfig = {
            {"postProcessConfigContent", "{\"CLASS_NUM\": \"1001\","
                                         "\"SOFTMAX\": \"false\"}"}
    };
    APP_ERROR ret = instanceSrc.Init(postConfig);

    std::vector<uint32_t> shape = {1, 1001, 255};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor(shape, type);
    TensorBase::TensorBaseMalloc(tensor);
    std::vector<TensorBase> vec = {tensor, tensor};
    std::vector<std::vector<ClassInfo>> classInfos;
    std::map<std::string, std::shared_ptr<void>> configParamMap;
    ret = instanceSrc.Process(vec, classInfos, configParamMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}
} // namespace

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}
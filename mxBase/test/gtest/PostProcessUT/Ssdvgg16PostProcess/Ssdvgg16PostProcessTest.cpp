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
#include "PostProcessUtils.h"
#include "ObjectPostProcessors/Ssdvgg16PostProcess.h"
#include "MxBase/Log/Log.h"

namespace {
using namespace MxBase;
const float MIN_VALUE = 0.;
const float MAX_VALUE = 10.;
class Ssdvgg16PostProcessTest : public testing::Test {
public:
    std::map<std::string, std::string> postConfig_ = {{"postProcessConfigContent",
        "{\"CLASS_NUM\": \"80\","
        "\"SCORE_THRESH\": \"0.7\"}"}};
};

TEST_F(Ssdvgg16PostProcessTest, TestSsdvgg16PostProcess)
{
    LogInfo << "****************case TestSsdvgg16PostProcess***************";
    Ssdvgg16PostProcess ssdvgg16PostProcess;
    Ssdvgg16PostProcess ssdvgg16PostProcess1(ssdvgg16PostProcess);
    ssdvgg16PostProcess = ssdvgg16PostProcess1;
    std::vector<TensorBase> tensors = {};
    std::vector<std::vector<ObjectInfo>> objectInfos = {};
    std::vector<ResizedImageInfo> resizedImageInfos = {};
    std::map<std::string, std::shared_ptr<void>> config = {};
    APP_ERROR ret = ssdvgg16PostProcess.Process(tensors, objectInfos, resizedImageInfos, config);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(Ssdvgg16PostProcessTest, Test_Ssdvgg16PostProcess_Init_Should_Success)
{
    Ssdvgg16PostProcess ssdvgg16PostProcess;
    APP_ERROR ret = ssdvgg16PostProcess.Init(postConfig_);
    Ssdvgg16PostProcess ssdvgg16PostProcessTmp(ssdvgg16PostProcess);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(Ssdvgg16PostProcessTest, Test_Ssdvgg16PostProcess_Process_Should_Success)
{
    Ssdvgg16PostProcess ssdvgg16PostProcess;
    APP_ERROR ret = ssdvgg16PostProcess.Init(postConfig_);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<uint32_t> shape1 = {1, 8};
    std::vector<uint32_t> shape2 = {1, 200, 8};
    auto type = TensorDataType::TENSOR_DTYPE_FLOAT32;
    TensorBase tensor1(shape1, type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor2(shape2, type);
    TensorBase::TensorBaseMalloc(tensor2);
    std::vector<TensorBase> tensorVec = {tensor1, tensor2};
    PostProcessUtils::SetRandomTensorValue<float>(tensorVec, MIN_VALUE, MAX_VALUE);
    std::vector<std::vector<ObjectInfo>> objectInfos = {};
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfoVec = {resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configMap = {};
    ret = ssdvgg16PostProcess.Process(tensorVec, objectInfos, resizedImageInfoVec, configMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(Ssdvgg16PostProcessTest, Test_Ssdvgg16PostProcess_DeInit_Should_Success)
{
    std::shared_ptr<Ssdvgg16PostProcess> instanceSrc = GetObjectInstance();
    EXPECT_NE(instanceSrc, nullptr);
    APP_ERROR ret = instanceSrc->DeInit();
    EXPECT_EQ(ret, 0);
}
}  // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
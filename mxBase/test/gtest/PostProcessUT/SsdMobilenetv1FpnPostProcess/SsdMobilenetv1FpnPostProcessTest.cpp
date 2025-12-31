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

#include <random>
#include <glog/logging.h>
#include <gtest/gtest.h>
#include <cstring>

#define private public
#define protected public
#include "mxbase/include/MxBase/PostProcessBases/PostProcessBase.h"
#include "postprocess/module/ObjectPostProcessors/SsdMobilenetv1FpnPostProcess/SsdMobilenetv1FpnPostProcessDptr.hpp"
#undef private
#undef protected

#include "PostProcessUtils.h"
#include "ObjectPostProcessors/SsdMobilenetv1FpnPostProcess.h"
#include "MxBase/Log/Log.h"

namespace {
using namespace MxBase;
const float MIN_VALUE = 0.;
const float MAX_VALUE = 10.;
const ResizedImageInfo RESIZED_IMAGE_INFO = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};

std::vector<uint32_t> g_shape1 = {1};
std::vector<uint32_t> g_shape2 = {1, 300};
std::vector<uint32_t> g_shape3 = {1, 300, 4};
auto g_type = TensorDataType::TENSOR_DTYPE_FLOAT32;
std::vector<ResizedImageInfo> g_resizedImageInfoVec = {RESIZED_IMAGE_INFO};
std::map<std::string, std::shared_ptr<void>> g_configMap = {};
std::vector<std::vector<ObjectInfo>> g_objectInfos = {};

class SsdMobilenetv1FpnPostProcessTest : public testing::Test {
public:
    std::map<std::string, std::string> postConfig_ = {{"postProcessConfigContent",
        "{\"CLASS_NUM\": \"80\","
        "\"SCORE_THRESH\": \"0.7\"}"}};
};

TEST_F(SsdMobilenetv1FpnPostProcessTest, Test_SsdMobilenetv1FpnPostProcess_Process_Should_Fail_When_Tensor_Empty)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess1(ssdMobilenetv1FpnPostProcess);
    ssdMobilenetv1FpnPostProcess = ssdMobilenetv1FpnPostProcess1;
    std::vector<TensorBase> tensors = {};
    std::vector<std::vector<ObjectInfo>> objectInfos = {};
    std::vector<ResizedImageInfo> resizedImageInfos = {};
    std::map<std::string, std::shared_ptr<void>> config = {};
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensors, objectInfos, resizedImageInfos, config);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest, Test_SsdMobilenetv1FpnPostProcess_Init_Should_Success)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Init(postConfig_);
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcessTmp(ssdMobilenetv1FpnPostProcess);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest, Test_SsdMobilenetv1FpnPostProcess_Process_Should_Success)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Init(postConfig_);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase tensor1(g_shape1, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase tensor2(g_shape2, g_type);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase tensor3(g_shape3, g_type);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor3, tensor2};
    PostProcessUtils::SetRandomTensorValue<float>(tensorVec, MIN_VALUE, MAX_VALUE);
    ResizedImageInfo resizedImageInfo = {16, 16, 32, 32, ResizeType::RESIZER_STRETCHING, 1.0};
    std::vector<ResizedImageInfo> resizedImageInfoVec = {resizedImageInfo};
    std::map<std::string, std::shared_ptr<void>> configMap = {};
    std::vector<std::vector<ObjectInfo>> objectInfos = {};
    ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, objectInfos, resizedImageInfoVec, configMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest, Test_SsdMobilenetv1FpnPostProcess_DeInit_Should_Success)
{
    std::shared_ptr<SsdMobilenetv1FpnPostProcess> instanceSrc = GetObjectInstance();
    EXPECT_NE(instanceSrc, nullptr);
    APP_ERROR ret = instanceSrc->DeInit();
    EXPECT_EQ(ret, 0);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Fail_when_postConfig_is_None)
{
    postConfig_ = {};
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Init(postConfig_);
    EXPECT_EQ(ret, APP_ERR_COMM_NO_EXIST);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Fail_when_isInitConfig_is_true)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;
    ssdMobilenetv1FpnPostProcess.isInitConfig_ = true;
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Init(postConfig_);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_CheckDptr_nullptr)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;
    ssdMobilenetv1FpnPostProcess.dPtr_ = nullptr;
    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(g_shape2, g_type);
    TensorBase tensor3(g_shape3, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor3, tensor2};
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest,
    Test_SsdMobilenetv1FpnPostProcess_Process_Should_Fail_when_tensor_type_Mismatched)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Init(postConfig_);
    EXPECT_EQ(ret, APP_ERR_OK);
    TensorBase invalidTensor;
    std::vector<TensorBase> tensorVec = {invalidTensor, invalidTensor, invalidTensor};
    ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_tensor_size_less_4)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;
    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(g_shape2, g_type);
    TensorBase tensor3(g_shape3, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor3};
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}


TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_tensor3_size_not_equal_BBOX_POOLSIZE)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;
    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(g_shape2, g_type);
    TensorBase tensor3(g_shape3, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor2, tensor2};
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}


TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_tensor3_shape2_not_equal_BOX_DIM)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;

    std::vector<uint32_t> shape3 = {1, 300, 5};

    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(g_shape2, g_type);
    TensorBase tensor3(shape3, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);

    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor3, tensor2};
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_tensor3_size_is_0)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;
    TensorBase invalidTensor;
    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(g_shape2, g_type);
    TensorBase tensor3(g_shape3, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensorVec = {tensor1, tensor2, invalidTensor, tensor2};
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_tensor1_and_tensor2_shape0_not_equal)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;

    std::vector<uint32_t> shape4 = {2, 300};
    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(shape4, g_type);
    TensorBase tensor3(g_shape3, g_type);
    TensorBase tensor4(g_shape2, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    TensorBase::TensorBaseMalloc(tensor4);
    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor3, tensor4};
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_tensor1_and_tensor3_shape0_not_equal)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;

    std::vector<uint32_t> shape3 = {2, 300, 4};

    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(g_shape2, g_type);
    TensorBase tensor3(shape3, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor3, tensor2};
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_tensor1_and_tensor4_shape0_not_equal)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;

    std::vector<uint32_t> shape4 = {2, 300};
    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(g_shape2, g_type);
    TensorBase tensor3(g_shape3, g_type);
    TensorBase tensor4(shape4, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    TensorBase::TensorBaseMalloc(tensor4);
    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor3, tensor4};
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}


TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_tensor2_shape_sub1)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;

    std::vector<uint32_t> shape2 = {300};
    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(shape2, g_type);
    TensorBase tensor3(g_shape3, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor3, tensor2};
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_tensor1_and_tensor4_empty)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;

    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(g_shape2, g_type);
    TensorBase tensor3(g_shape3, g_type);
    TensorBase tensor4(g_shape2, g_type);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);

    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor3, tensor4};
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
}

TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_tensor2_tensor3_and_tensor4_batchsize_is_0)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;
    std::vector<uint32_t> shape2 = {0, 300};
    std::vector<uint32_t> shape3 = {0, 300, 4};

    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(shape2, g_type);
    TensorBase tensor3(shape3, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);

    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor3, tensor2};
    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}


TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_tensor4_value_greater_80)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;

    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(g_shape2, g_type);
    TensorBase tensor3(g_shape3, g_type);
    TensorBase tensor4(g_shape2, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);
    TensorBase::TensorBaseMalloc(tensor4);

    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor3, tensor4};
    PostProcessUtils::SetRandomTensorValue<float>(tensorVec, MIN_VALUE, MAX_VALUE);

    size_t totalElements = tensor4.GetShape()[0] * tensor4.GetShape()[1];
    std::vector<float> data(totalElements, 81.0f);
    void* ptr = tensor4.GetBuffer();
    float* floatPtr = static_cast<float*>(ptr);
    std::copy(data.begin(), data.end(), floatPtr);

    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}


TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_tensor2_value_less_3)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;

    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(g_shape2, g_type);
    TensorBase tensor3(g_shape3, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);

    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor3, tensor2};
    PostProcessUtils::SetRandomTensorValue<float>(tensorVec, MIN_VALUE, MAX_VALUE);

    size_t totalElements = tensor2.GetShape()[0] * tensor2.GetShape()[1];
    std::vector<float> data(totalElements, 1.0f);
    void* ptr = tensor2.GetBuffer();
    float* floatPtr = static_cast<float*>(ptr);
    std::copy(data.begin(), data.end(), floatPtr);

    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}


TEST_F(SsdMobilenetv1FpnPostProcessTest,
    TestSsdMobilenetv1FpnPostProcess_Process_Should_Return_Fail_when_tensor1_value_greater_100)
{
    SsdMobilenetv1FpnPostProcess ssdMobilenetv1FpnPostProcess;

    TensorBase tensor1(g_shape1, g_type);
    TensorBase tensor2(g_shape2, g_type);
    TensorBase tensor3(g_shape3, g_type);
    TensorBase::TensorBaseMalloc(tensor1);
    TensorBase::TensorBaseMalloc(tensor2);
    TensorBase::TensorBaseMalloc(tensor3);

    std::vector<TensorBase> tensorVec = {tensor1, tensor2, tensor3, tensor2};
    PostProcessUtils::SetRandomTensorValue<float>(tensorVec, MIN_VALUE, MAX_VALUE);

    size_t totalElements = tensor1.GetShape()[0];
    std::vector<float> data(totalElements, 101.0f);
    void* ptr = tensor1.GetBuffer();
    float* floatPtr = static_cast<float*>(ptr);
    std::copy(data.begin(), data.end(), floatPtr);

    APP_ERROR ret = ssdMobilenetv1FpnPostProcess.Process(tensorVec, g_objectInfos, g_resizedImageInfoVec, g_configMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

} // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
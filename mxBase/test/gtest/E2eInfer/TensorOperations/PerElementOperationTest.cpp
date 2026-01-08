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
 * Create: 2023
 * History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <vector>
#include "ResourceManager/HAL/AclApi.h"
#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/PerElementOperations.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/CommonUtils.h"
#include "MxBase/MxBase.h"
#include "MxBase/DeviceManager/DeviceManager.h"

namespace {
using namespace MxBase;
constexpr size_t DATA_LEN = 6;
constexpr int IMAGE_WIDTH = 480;
constexpr int IMAGE_HEIGHT = 640;
constexpr int CHANNEL = 3;
constexpr int GRAY_CHANNEL = 1;
constexpr size_t CONVERTTO_ROI_DST_H = 2;
constexpr size_t CONVERTTO_ROI_DST_LEN = 4;
constexpr float EPSILON = 1e-5f;
const std::vector<uint32_t> SHAPE3 = {0x1, 0x2, 0x3};
const std::vector<uint32_t> SHAPE3_DIFFC = {0x1, 0x2, 0x4};
const std::vector<uint32_t> SHAPE4 = {0x1, 0x2, 0x3, 0x1};
const std::vector<uint32_t> SHAPE5 = {0x1, 0x2, 0x3, 0x1, 0x1};
const std::vector<uint32_t> shapeHW = {IMAGE_HEIGHT, IMAGE_WIDTH};
const std::vector<uint32_t> shapeHWC = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
const std::vector<uint32_t> shapeHWC2 = {IMAGE_WIDTH, IMAGE_WIDTH, CHANNEL};
const std::vector<uint32_t> shapeNHWC = {GRAY_CHANNEL, IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
const std::vector<uint32_t> invalidShape = {GRAY_CHANNEL, IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL, CHANNEL};
const std::vector<uint32_t> CONVERTTO_ROI_SRC_HWSHAPE = {0x2, 0x3};
const std::vector<uint32_t> CONVERTTO_ROI_DST_HWSHAPE = {0x2, 0x2};
const std::vector<uint32_t> CONVERTTO_ROI_SRC_HWCSHAPE = {0x2, 0x3, 0x3};
const std::vector<uint32_t> CONVERTTO_ROI_DST_HWCSHAPE = {0x2, 0x2, 0x3};
const std::vector<uint32_t> CONVERTTO_ROI_SRC_NHWCSHAPE = {0x1, 0x2, 0x3, 0x3};
const std::vector<uint32_t> CONVERTTO_ROI_DST_NHWCSHAPE = {0x1, 0x2, 0x2, 0x3};
const Rect CONVERTTO_ROI_RECT = {0, 0, 2, 2};
uint8_t g_data1[DATA_LEN] = {0};
uint8_t g_data2[DATA_LEN] = {0};
float g_data3[DATA_LEN] = {0.f};
uint8_t g_data4[DATA_LEN] = {1, 2, 3, 3, 4, 5};
uint8_t g_data5[DATA_LEN] = {5, 4, 3, 3, 2, 1};
uint8_t g_absdiffU8Result[DATA_LEN] = {4, 2, 0, 0, 2, 4};
float g_data6[DATA_LEN] = {1.0f, 2.0f, 3.0f, 3.0f, 4.0f, 5.0f};
float g_data7[DATA_LEN] = {5.0f, 4.0f, 3.0f, 3.0f, 2.0f, 1.0f};
float g_data8[DATA_LEN] = {-1.0f, -2.0f, -3.0f, -3.0f, -4.0f, -5.0f};
float g_data9[DATA_LEN] = {1.0f, 2.0f, 3.0f, 3.0f, 4.0f, 5.0f};
float g_data10[CONVERTTO_ROI_DST_LEN] = {1.0f, 2.0f, 3.0f, 4.0f};
float g_data11[DATA_LEN] = {1.1f, 2.2f, 3.3f, 3.7f, 4.8f, 5.9f};
float g_absdiffFloatResult[DATA_LEN] = {4.0f, 2.0f, 0.0f, 0.0f, 2.0f, 4.0f};
uint8_t g_sqrResult1[DATA_LEN] = {1, 4, 9, 9, 16, 25};
float g_sqrResult2[DATA_LEN] = {25, 16, 9, 9, 4, 1};

constexpr float MUL_SCALE = 1.5;
constexpr float ALPHA = 1.0;
constexpr float BETA = 2.0;
constexpr float GAMMA = 3.0;
constexpr float MIN_VAL = 1.f;
constexpr float MAX_VAL = 10.f;
constexpr uint8_t VALID_BITWISE_OP_SRC_VALUE1 = 7;
constexpr uint8_t VALID_BITWISE_OP_SRC_VALUE2 = 17;
constexpr uint8_t VALID_BITWISEAND_DST_VALUE = 1;
constexpr uint8_t VALID_BITWISEOR_DST_VALUE = 23;
constexpr uint8_t VALID_BITWISEXOR_DST_VALUE = 22;
constexpr uint8_t VALID_MULTIPLY_OP_SRC_VALUE1 = 2;
constexpr uint8_t VALID_MULTIPLY_OP_SRC_VALUE2 = 3;
constexpr uint8_t VALID_MULTIPLY_OP_DST_VALUE1 = 6;
constexpr uint8_t VALID_MULTIPLY_OP_DST_VALUE2 = 9;
constexpr uint8_t ROI_X0_1 = 1;
constexpr uint8_t ROI_Y0_1 = 1;
constexpr uint8_t ROI_X1_2 = 2;
constexpr uint8_t ROI_Y1_2 = 2;
constexpr uint8_t ROI_X1_3 = 3;
constexpr uint8_t ROI_Y1_3 = 3;
constexpr uint8_t VALID_ADD_INPLACE_RESULT_INDEX_5 = 5;
constexpr uint8_t VALID_ADD_INPLACE_RESULT_INDEX_15 = 15;
constexpr uint8_t VALID_ADD_INPLACE_RESULT_INDEX_16 = 16;
constexpr uint8_t VALID_ADD_INPLACE_RESULT_INDEX_17 = 17;
constexpr uint8_t VALID_ADD_INPLACE_RESULT = 2;
constexpr uint32_t SHAPE_DIMENSION_ZERO = 0;
constexpr uint32_t SHAPE_DIMENSION_ONE = 1;
constexpr uint32_t SHAPE_DIMENSION_TWO = 2;
constexpr uint32_t SHAPE_DIMENSION_THREE = 3;
constexpr uint32_t SHAPE_DIMENSION_FOUR = 4;
constexpr uint32_t SHAPE_DIMENSION_FIVE = 5;
constexpr int COMPARE_SATISFIED = 255;
constexpr int COMPARE_NOT_SATISFIED = 0;
const std::vector<uint32_t> shapeHW_add = {5, 4};
const std::vector<uint32_t> shapeHW_add2 = {10, 10};
const std::vector<uint32_t> shapeHWC_add = {5, 4, 3};
const std::vector<uint32_t> shapeNHWC_add = {1, 5, 4, 3};
constexpr uint8_t VALID_SUBTRACT_OP_SRC_VALUE1 = 6;
constexpr uint8_t VALID_SUBTRACT_OP_SRC_VALUE2 = 2;
constexpr uint8_t VALID_SUBTRACT_OP_DST_VALUE = 4;

class PerElementOperationTest : public testing::Test {
public:
    void TearDown() override
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

template<typename T>
bool EXPECT_EQ_VECTOR(std::vector<T> t1, std::vector<T> t2)
{
    return t1 == t2;
}

bool AreFloatValueEqual(float a, float b)
{
    return std::fabs(a - b) < EPSILON;
}

TEST_F(PerElementOperationTest, TestAdd)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(&g_data2, SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor3);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    APP_ERROR ret = Add(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor4(&g_data2, SHAPE4, TensorDType::UINT8);
    ret = Add(tensor1, tensor2, tensor4);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor5(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor tensor6(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(tensor6);
    tensor5.ToDevice(0);
    tensor6.ToDevice(0);
    ret = Add(tensor1, tensor5, tensor6);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, TestImplicitTensorMalloc1)
{
    Tensor tensor1(SHAPE3, TensorDType::UINT8, 0, true);
    Tensor tensor2(SHAPE3, TensorDType::UINT8, 0, true);
    Tensor::TensorMalloc(tensor1);
    Tensor::TensorMalloc(tensor2);

    Tensor tensor4(SHAPE4, TensorDType::UINT8, 0, true);
    Tensor::TensorMalloc(tensor4);
    APP_ERROR ret = Add(tensor1, tensor2, tensor4);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor5(SHAPE3, TensorDType::FLOAT32, 0, true);
    Tensor::TensorMalloc(tensor5);
    ret = Add(tensor1, tensor2, tensor5);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, TestSub)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(&g_data2, SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor3);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor4(&g_data2, SHAPE4, TensorDType::UINT8);
    ret = Subtract(tensor1, tensor2, tensor4);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor5(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor tensor6(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(tensor6);
    tensor5.ToDevice(0);
    tensor6.ToDevice(0);
    ret = Subtract(tensor1, tensor5, tensor6);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, TestBitwiseAnd)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(&g_data2, SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor3);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    APP_ERROR ret = BitwiseAnd(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor4(&g_data2, SHAPE4, TensorDType::UINT8);
    ret = BitwiseAnd(tensor1, tensor2, tensor4);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseAnd_Should_Return_Success_When_Src_is_Uint8_W_480_H_640)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHW, TensorDType::UINT8, 0);
        Tensor src2(shapeHW, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor::TensorMalloc(src2);
        src1.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE1);
        src2.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE2);
        Tensor dst;
        APP_ERROR ret = BitwiseAnd(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(static_cast<uint8_t *>(dst.GetData())[0], VALID_BITWISEAND_DST_VALUE);
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseAnd_Should_Return_Fail_When_aclnnBitwiseAndTensorGetWorkspaceSize_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(AclApi::aclnnBitwiseAndTensorGetWorkspaceSize).stubs().will(returnValue(1));
        Tensor src1(shapeHW, TensorDType::UINT8, 0);
        Tensor src2(shapeHW, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor::TensorMalloc(src2);
        src1.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE1);
        src2.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE2);
        Tensor dst;
        APP_ERROR ret = BitwiseAnd(src1, src2, dst);
        EXPECT_EQ(ret, 1);
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseAnd_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src1;
    Tensor src2;
    Tensor dst;
    APP_ERROR ret = BitwiseAnd(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseAnd_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, -1);
    Tensor src2(SHAPE3, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseAnd(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseAnd_Should_Return_Fail_When_Src_Type_Is_Different)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseAnd(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseAnd_Should_Return_Fail_When_Src_Datatype_Is_Float32)
{
    Tensor src1(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor src2(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseAnd(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseAnd_Should_Return_Fail_When_Src_Shape_Is_Different)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3_DIFFC, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseAnd(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseAnd_Should_Return_Fail_When_Dst_Shape_Is_Different_With_Src)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseAnd(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseAnd_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src1(SHAPE5, TensorDType::UINT8, 0);
    Tensor src2(SHAPE5, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseAnd(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseAnd_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseAnd(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseAnd_Should_Return_Fail_When_Dst_Datatype_Is_Float32)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseAnd(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseAnd_Should_Return_Fail_When_Src_DeviceId_Is_Different)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseAnd(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_BitwiseAnd_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseAnd(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_BitwiseAnd_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    AscendStream stream(1);
    stream.CreateAscendStream();
    APP_ERROR ret = BitwiseAnd(src1, src2, dst, stream);
    stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest, TestBitwiseOr)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(&g_data2, SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor3);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    APP_ERROR ret = BitwiseOr(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor4(&g_data2, SHAPE4, TensorDType::UINT8);
    ret = BitwiseOr(tensor1, tensor2, tensor4);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Success_When_Src_is_Uint8_W_480_H_640)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHW, TensorDType::UINT8, 0);
        Tensor src2(shapeHW, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor::TensorMalloc(src2);
        src1.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE1);
        src2.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE2);
        Tensor dst;
        APP_ERROR ret = BitwiseOr(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(static_cast<uint8_t *>(dst.GetData())[0], VALID_BITWISEOR_DST_VALUE);
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Fail_When_aclnnBitwiseOrTensorGetWorkspaceSize_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(AclApi::aclnnBitwiseOrTensorGetWorkspaceSize).stubs().will(returnValue(1));
        Tensor src1(shapeHW, TensorDType::UINT8, 0);
        Tensor src2(shapeHW, TensorDType::UINT8, 0);
        src1.Malloc();
        src2.Malloc();
        src1.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE1);
        src2.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE2);
        Tensor dst;
        APP_ERROR ret = BitwiseOr(src1, src2, dst);
        EXPECT_EQ(ret, 1);
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src1;
    Tensor src2;
    Tensor dst;
    APP_ERROR ret = BitwiseOr(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, -1);
    Tensor src2(SHAPE3, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseOr(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Fail_When_Src_Type_Is_Different)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseOr(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Fail_When_Src_Datatype_Is_Float32)
{
    Tensor src1(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor src2(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseOr(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Fail_When_Src_Shape_Is_Different)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3_DIFFC, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseOr(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Fail_When_Dst_Shape_Is_Different_With_Src)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseOr(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src1(SHAPE5, TensorDType::UINT8, 0);
    Tensor src2(SHAPE5, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseOr(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseOr(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Fail_When_Dst_Datatype_Is_Float32)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseOr(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Fail_When_Src_DeviceId_Is_Different)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseOr(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseOr(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_BitwiseOr_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    AscendStream stream(1);
    stream.CreateAscendStream();
    APP_ERROR ret = BitwiseOr(src1, src2, dst, stream);
    stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest, TestBitwiseXor)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(&g_data2, SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor3);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    APP_ERROR ret = BitwiseXor(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor4(&g_data2, SHAPE4, TensorDType::UINT8);
    ret = BitwiseXor(tensor1, tensor2, tensor4);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Success_When_Src_is_Uint8_W_480_H_640)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHW, TensorDType::UINT8, 0);
        Tensor src2(shapeHW, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor::TensorMalloc(src2);
        src1.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE1);
        src2.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE2);
        Tensor dst;
        APP_ERROR ret = BitwiseXor(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(static_cast<uint8_t *>(dst.GetData())[0], VALID_BITWISEXOR_DST_VALUE);
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Fail_When_aclnnBitwiseXorTensorGetWorkspaceSize_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(AclApi::aclnnBitwiseXorTensorGetWorkspaceSize).stubs().will(returnValue(1));
        Tensor src1(shapeHW, TensorDType::UINT8, 0);
        Tensor src2(shapeHW, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor::TensorMalloc(src2);
        src1.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE1);
        src2.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE2);
        Tensor dst;
        APP_ERROR ret = BitwiseXor(src1, src2, dst);
        EXPECT_EQ(ret, 1);
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Fail_When_CreateAclTensor_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(&CreateAclTensor).stubs().will(returnValue(1));
        Tensor src1(shapeHW, TensorDType::UINT8, 0);
        Tensor src2(shapeHW, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor::TensorMalloc(src2);
        src1.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE1);
        src2.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE2);
        Tensor dst;
        APP_ERROR ret = BitwiseXor(src1, src2, dst);
        EXPECT_EQ(ret, 1);
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src1;
    Tensor src2;
    Tensor dst;
    APP_ERROR ret = BitwiseXor(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, -1);
    Tensor src2(SHAPE3, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseXor(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Fail_When_Src_Type_Is_Different)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseXor(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Fail_When_Src_Datatype_Is_Float32)
{
    Tensor src1(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor src2(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseXor(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Fail_When_Src_Shape_Is_Different)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3_DIFFC, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseXor(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Fail_When_Dst_Shape_Is_Different_With_Src)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseXor(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src1(SHAPE5, TensorDType::UINT8, 0);
    Tensor src2(SHAPE5, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = BitwiseXor(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseXor(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Fail_When_Dst_Datatype_Is_Float32)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseXor(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Fail_When_Src_DeviceId_Is_Different)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseXor(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_BitwiseXor_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseXor(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_BitwiseXor_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    AscendStream stream(1);
    stream.CreateAscendStream();
    APP_ERROR ret = BitwiseXor(src1, src2, dst, stream);
    stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest, Test_BitwiseNot_Should_Return_Success_When_Input_Parameters_OK)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(SHAPE3, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        uint8_t valueForBitwiseNot = 100;
        src.SetTensorValue(valueForBitwiseNot);
        Tensor dst;
        APP_ERROR ret = BitwiseNot(src, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        uint8_t expectBitwiseNotResult = 155;
        EXPECT_EQ(static_cast<uint8_t *>(dst.GetData())[0], expectBitwiseNotResult);
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseNot_Should_Return_Success_When_Async)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(SHAPE3, TensorDType::UINT8, 0);
        src.Malloc();
        uint8_t valueForBitwiseNot = 100;
        src.SetTensorValue(valueForBitwiseNot);
        Tensor dst;
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = BitwiseNot(src, dst, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        uint8_t expectBitwiseNotResult = 155;
        EXPECT_EQ(static_cast<uint8_t *>(dst.GetData())[0], expectBitwiseNotResult);
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseNot_Should_Return_Fail_When_aclnnBitwiseNotGetWorkspaceSize_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(AclApi::aclnnBitwiseNotGetWorkspaceSize).stubs().will(returnValue(1));
        Tensor src(SHAPE3, TensorDType::UINT8, 0);
        src.Malloc();
        uint8_t valueForBitwiseNot = 100;
        src.SetTensorValue(valueForBitwiseNot);
        Tensor dst;
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = BitwiseNot(src, dst, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, 1);
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseNot_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src;
    Tensor dst;
    APP_ERROR ret = BitwiseNot(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseNot_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src(SHAPE3, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = BitwiseNot(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseNot_Should_Return_Fail_When_Src_Datatype_Is_Float32)
{
    Tensor src(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = BitwiseNot(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseNot_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src(SHAPE5, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = BitwiseNot(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseNot_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE3, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseNot(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_BitwiseNot_Should_Return_Fail_When_Dst_Datatype_Is_Float32)
{
    Tensor src(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseNot(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_BitwiseNot_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE3, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseNot(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_BitwiseNot_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(SHAPE3, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        AscendStream stream(1);
        stream.CreateAscendStream();
        APP_ERROR ret = BitwiseNot(src, dst, stream);
        stream.Synchronize();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        stream.DestroyAscendStream();
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseNot_Should_Return_Fail_When_Dst_Shape_Is_Different_With_Src)
{
    Tensor src(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = BitwiseNot(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, TestPow)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(&g_data2, SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(tensor3);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    APP_ERROR ret = Pow(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, TestExp)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::FLOAT32);
    Tensor tensor2(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    APP_ERROR ret = Exp(tensor1, tensor2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor3(&g_data2, SHAPE4, TensorDType::FLOAT32);
    ret = Exp(tensor1, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, TestLog)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(SHAPE3, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    APP_ERROR ret = Log(tensor1, tensor2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor3(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(tensor3);
    tensor3.ToDevice(0);
    ret = Log(tensor1, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor5(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor5);
    tensor5.ToDevice(0);
    ret = Log(tensor1, tensor5);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqr_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src;
    Tensor dst;
    APP_ERROR ret = Sqr(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqr_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst;
    APP_ERROR ret = Sqr(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqr_Should_Return_Fail_When_Src_Datatype_Is_Int32)
{
    Tensor src(&g_data1, SHAPE3, TensorDType::INT32, 0);
    Tensor dst;
    APP_ERROR ret = Sqr(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqr_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src(&g_data3, SHAPE5, TensorDType::FLOAT32, 0);
    Tensor dst;
    APP_ERROR ret = Sqr(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqr_Should_Return_Fail_When_Dst_Shape_Different)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32, 0);
    Tensor dst(SHAPE3_DIFFC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Sqr(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqr_Should_Return_Fail_When_Dst_ShapeDim_Different)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32, 0);
    Tensor dst(SHAPE4, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Sqr(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqr_Should_Return_Fail_When_Dst_Datatype_Different)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32, 0);
    Tensor dst(SHAPE3, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Sqr(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqr_Should_Return_Fail_When_Dst_Device_Different)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32, 0);
    Tensor dst(SHAPE3, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Sqr(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqr_Should_Return_Fail_When_Stream_Device_Different)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32, 0);
    Tensor dst;
    AscendStream stream(1);
    stream.CreateAscendStream();
    APP_ERROR ret = Sqr(src, dst, stream);
    stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest, Test_Sqr_Should_Return_Success_When_Dst_is_Uint8_Src_is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data4, SHAPE3, TensorDType::UINT8);
        src.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = Sqr(src, dst);
        ASSERT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        for (size_t i = 0; i < DATA_LEN; i++) {
            EXPECT_EQ(static_cast<uint8_t *>(dst.GetData())[i], g_sqrResult1[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_Sqr_Should_Return_Success_When_Dst_is_Float16_Src_is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data7, SHAPE3, TensorDType::FLOAT32);
        Tensor src16;
        src.ToDevice(0);
        Tensor dst;
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = ConvertTo(src, src16, TensorDType::FLOAT16, stream);
        ASSERT_EQ(ret, APP_ERR_OK);
        ret = Sqr(src16, dst, stream);
        ASSERT_EQ(ret, APP_ERR_OK);
        Tensor dst32;
        ConvertTo(dst, dst32, TensorDType::FLOAT32, stream);
        ASSERT_EQ(ret, APP_ERR_OK);
        stream.Synchronize();
        dst32.ToHost();
        for (size_t i = 0; i < DATA_LEN; i++) {
            EXPECT_EQ(static_cast<float *>(dst32.GetData())[i], g_sqrResult2[i]);
        }
        stream.DestroyAscendStream();
    }
}

TEST_F(PerElementOperationTest, Test_Sqr_Should_Return_Success_When_Dst_is_Float32_Src_is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data7, SHAPE3, TensorDType::FLOAT32);
        src.ToDevice(0);
        Tensor dst;
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = Sqr(src, dst, stream);
        stream.Synchronize();
        ASSERT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        for (size_t i = 0; i < DATA_LEN; i++) {
            EXPECT_EQ(static_cast<float *>(dst.GetData())[i], g_sqrResult2[i]);
        }
        stream.DestroyAscendStream();
    }
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Failed_When_Dype_is_different)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    APP_ERROR ret = ThresholdBinary(tensor1, tensor2, 1.0f, 2.0f);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Threshold(tensor1, tensor2, 1.0f, 2.0f);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor4(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor4);
    ret = ThresholdBinary(tensor1, tensor4, 1.0f, 2.0f);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Threshold(tensor1, tensor4, 1.0f, 2.0f);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Threshold_Inv_Should_Return_Success_When_Dtype_Is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
    Tensor tensor1(&g_data6, SHAPE3, TensorDType::FLOAT32);
    Tensor tensor2;
    tensor1.ToDevice(0);
    APP_ERROR ret = Threshold(tensor1, tensor2, 3.0f, MAX_VAL, ThresholdType::THRESHOLD_BINARY_INV);
    EXPECT_EQ(ret, APP_ERR_OK);
    tensor2.ToHost();
    EXPECT_EQ((static_cast<float *>(tensor2.GetData()))[SHAPE_DIMENSION_ZERO], MAX_VAL);
    EXPECT_EQ((static_cast<float *>(tensor2.GetData()))[SHAPE_DIMENSION_ONE], MAX_VAL);
    EXPECT_EQ((static_cast<float *>(tensor2.GetData()))[SHAPE_DIMENSION_TWO], MAX_VAL);
    EXPECT_EQ((static_cast<float *>(tensor2.GetData()))[SHAPE_DIMENSION_THREE], MAX_VAL);
    EXPECT_EQ((static_cast<float *>(tensor2.GetData()))[SHAPE_DIMENSION_FOUR], 0);
    EXPECT_EQ((static_cast<float *>(tensor2.GetData()))[SHAPE_DIMENSION_FIVE], 0);
    }
}

TEST_F(PerElementOperationTest, Test_Threshold_Should_Return_Failed_When_Dtype_Is_Different)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    APP_ERROR ret = Threshold(tensor1, tensor2, 1.0f, 2.0f, ThresholdType::THRESHOLD_BINARY);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Threshold_Should_Return_Success_When_Dtype_Is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
    Tensor src(&g_data6, SHAPE3, TensorDType::FLOAT32);
    src.ToDevice(0);
    Tensor dst;
    float thresh = 3.0f;
    AscendStream stream(0);
    stream.CreateAscendStream();
    APP_ERROR ret = Threshold(src, dst, thresh, MAX_VAL);
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    dst.ToHost();
    EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], 0);
    EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE], 0);
    EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO], 0);
    EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_THREE], 0);
    EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], MAX_VAL);
    EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], MAX_VAL);
    }
}

TEST_F(PerElementOperationTest, TestAddWeighted)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(&g_data2, SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(tensor3);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    APP_ERROR ret = AddWeighted(tensor1, 1.0f, tensor2, 2.0f, 3.0f, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor4(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor4);
    ret = AddWeighted(tensor1, 1.0f, tensor2, 2.0f, 3.0f, tensor4);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src1_Is_Empty)
{
    Tensor src1;
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src1_Is_Host)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src1_Shape_Size_Is_5)
{
    Tensor src1(SHAPE5, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src1_Datatype_Is_Int8)
{
    Tensor src1(SHAPE3, TensorDType::INT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src2_Is_Empty)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2;
    Tensor dst;
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src2_Is_Host)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src2_Shape_Size_Is_5)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE5, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src2_Datatype_Is_Int8)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::INT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Dst_Shape_Size_Is_5)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE5, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Dst_Datatype_Is_Int8)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::INT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_AddWeighted_Should_Return_Fail_When_Src1_DeviceId_Is_Different_With_Src2)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_AddWeighted_Should_Return_Fail_When_Src1_DeviceId_Is_Different_With_Dst)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_AddWeighted_Should_Return_Fail_When_Src1_DeviceId_Is_Different_With_StreamId)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    AscendStream stream(1);
    stream.CreateAscendStream();
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst, stream);
    stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest,
    Test_AddWeighted_Should_Return_Fail_When_Src2_DeviceId_Is_Different_With_Dst)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_AddWeighted_Should_Return_Fail_When_Src2_DeviceId_Is_Different_With_StreamId)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    AscendStream stream(0);
    stream.CreateAscendStream();
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst, stream);
    stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src1_Shape_Is_Different_With_Src2)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src1_Shape_Is_Different_With_Dst)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src2_Shape_Is_Different_With_Dst)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src1_Datatype_Is_Different_With_Src2)
{
    Tensor src1(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src1_Datatype_Is_Different_With_Dst)
{
    Tensor src1(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src2_Datatype_Is_Different_With_Dst)
{
    Tensor src1(SHAPE3, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst(SHAPE3, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Success_When_Input_Parameters_OK)
{
    if (DeviceManager::IsAscend310P()) {
        uint8_t data1[DATA_LEN] = {1};
        uint8_t data2[DATA_LEN] = {2};
        Tensor src1(data1, SHAPE3, TensorDType::UINT8);
        src1.ToDevice(0);
        Tensor src2(data2, SHAPE3, TensorDType::UINT8);
        src2.ToDevice(0);
        Tensor dst;
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = AddWeighted(src1, ALPHA, src2, BETA, GAMMA, dst, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        dst.ToHost();
        ASSERT_EQ(ret, APP_ERR_OK);
        const int expectValue = 8;
        EXPECT_EQ((static_cast<uint8_t*>(dst.GetData()))[0], expectValue);
    }
}

TEST_F(PerElementOperationTest, TestAbsDiff)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(&g_data2, SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor3);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    APP_ERROR ret = AbsDiff(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor5(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor tensor6(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(tensor6);
    tensor5.ToDevice(0);
    tensor6.ToDevice(0);
    ret = AbsDiff(tensor1, tensor5, tensor6);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, TestMultiply)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(&g_data2, SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor3);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor4(&g_data2, SHAPE4, TensorDType::UINT8);
    ret = Multiply(tensor1, tensor2, tensor4);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor5(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor tensor6(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(tensor6);
    tensor5.ToDevice(0);
    tensor6.ToDevice(0);
    ret = Multiply(tensor1, tensor5, tensor6);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, TestMultiplyScale)
{
    float scale = MUL_SCALE;
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(&g_data2, SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor3);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3, scale);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor4(&g_data2, SHAPE4, TensorDType::UINT8);
    ret = Multiply(tensor1, tensor2, tensor4, scale);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor5(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor tensor6(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(tensor6);
    tensor5.ToDevice(0);
    tensor6.ToDevice(0);
    ret = Multiply(tensor1, tensor5, tensor6);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, TestDiv)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(&g_data2, SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor3);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor4(&g_data2, SHAPE4, TensorDType::UINT8);
    ret = Divide(tensor1, tensor2, tensor4);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, TestAbs)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::FLOAT32);
    Tensor tensor2(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    APP_ERROR ret = Abs(tensor1, tensor2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor3(SHAPE4, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    ret = Abs(tensor1, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor4(SHAPE4, TensorDType::INT16, 0);
    Tensor::TensorMalloc(tensor4);
    Tensor tensor5(SHAPE4, TensorDType::INT16, 0);
    Tensor::TensorMalloc(tensor5);
    ret = Abs(tensor4, tensor5);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src1;
    Tensor src2;
    Tensor dst;
    APP_ERROR ret = ScaleAdd(src1, ALPHA, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor src2(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor dst;
    APP_ERROR ret = ScaleAdd(src1, ALPHA, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Fail_When_Src_Datatype_Is_Int32)
{
    Tensor src1(&g_data1, SHAPE3, TensorDType::INT32);
    Tensor src2(&g_data1, SHAPE3, TensorDType::INT32);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = ScaleAdd(src1, ALPHA, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src1(&g_data1, SHAPE5, TensorDType::UINT8);
    Tensor src2(&g_data1, SHAPE5, TensorDType::UINT8);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = ScaleAdd(src1, ALPHA, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Fail_When_Dst_Shape_Different)
{
    Tensor src1(&g_data1, SHAPE5, TensorDType::UINT8);
    Tensor src2(&g_data1, SHAPE3, TensorDType::UINT8);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = ScaleAdd(src1, ALPHA, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Fail_When_Dst_ShapeDim_Different)
{
    Tensor src1(&g_data1, SHAPE3_DIFFC, TensorDType::UINT8);
    Tensor src2(&g_data1, SHAPE3, TensorDType::UINT8);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = ScaleAdd(src1, ALPHA, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Fail_When_Dst_Datatype_Different)
{
    Tensor src1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor src2(&g_data1, SHAPE3, TensorDType::UINT8);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst(SHAPE3, TensorDType::FLOAT16);
    Tensor::TensorMalloc(dst);
    dst.ToDevice(0);
    APP_ERROR ret = ScaleAdd(src1, ALPHA, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Fail_When_Stream_Device_Different)
{
    Tensor src1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor src2(&g_data1, SHAPE3, TensorDType::UINT8);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst;
    MxBase::AscendStream stream = MxBase::AscendStream(1);
    stream.CreateAscendStream();
    APP_ERROR ret = ScaleAdd(src1, ALPHA, src2, dst, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Fail_When_Dst_Device_Different)
{
    Tensor src1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor src2(&g_data1, SHAPE3, TensorDType::UINT8);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst(SHAPE3, TensorDType::UINT8);
    Tensor::TensorMalloc(dst);
    dst.ToDevice(1);
    APP_ERROR ret = ScaleAdd(src1, ALPHA, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Fail_When_src1_src2_dtype_Different)
{
    Tensor src1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor src2(&g_data1, SHAPE3, TensorDType::FLOAT16);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = ScaleAdd(src1, ALPHA, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Success_When_Dst_is_Uint8_Src_is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data4, SHAPE3, TensorDType::UINT8);
        Tensor src2(&g_data5, SHAPE3, TensorDType::UINT8);
        src1.ToDevice(0);
        src2.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = ScaleAdd(src1, ALPHA, src2, dst);
        ASSERT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        uint8_t scaleAddResult = 6;
        for (size_t i = 0; i < DATA_LEN; i++) {
            EXPECT_EQ(scaleAddResult, static_cast<uint8_t *>(dst.GetData())[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Success_When_Dst_is_Float32_Src_is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        src1.ToDevice(0);
        src2.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = ScaleAdd(src1, ALPHA, src2, dst);
        ASSERT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        float scaleAddResult = 6.f;
        for (size_t i = 0; i < DATA_LEN; i++) {
            EXPECT_EQ(scaleAddResult, static_cast<float *>(dst.GetData())[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Success_When_Dst_is_Float16_Src_is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        src1.ToDevice(0);
        src2.ToDevice(0);
        Tensor dst;
        Tensor src1fp16;
        Tensor src2fp16;
        APP_ERROR ret = ConvertTo(src1, src1fp16, TensorDType::FLOAT16);
        ASSERT_EQ(ret, APP_ERR_OK);
        ret = ConvertTo(src2, src2fp16, TensorDType::FLOAT16);
        ASSERT_EQ(ret, APP_ERR_OK);
        ret = ScaleAdd(src1fp16, ALPHA, src2fp16, dst);
        ASSERT_EQ(ret, APP_ERR_OK);
        Tensor dst32;
        ret = ConvertTo(dst, dst32, TensorDType::FLOAT32);
        ASSERT_EQ(ret, APP_ERR_OK);
        dst32.ToHost();
        float scaleAddResult = 6.f;
        for (size_t i = 0; i < DATA_LEN; i++) {
            EXPECT_EQ(scaleAddResult, static_cast<float *>(dst32.GetData())[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Fail_When_TensorDType_Is_Undefined)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2;
    tensor1.ToDevice(0);
    APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::UNDEFINED);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor tensor1;
    Tensor tensor2;
    APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::FLOAT32);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2;
    APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::FLOAT32);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Fail_When_SrcDtype_And_DataType_Are_Same)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2;
    APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::UINT8);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Success_When_Dst_Is_Empty)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
        tensor1.ToDevice(0);
        Tensor tensor2;
        APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::FLOAT32);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(tensor2.GetDataType(), MxBase::TensorDType::FLOAT32);
    }
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2(SHAPE3, TensorDType::FLOAT32, 1);
    MxBase::Tensor::TensorMalloc(tensor2);
    APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::FLOAT32);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Fail_When_Src_ShapeSize_Is_Different_With_Dst)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2(SHAPE4, TensorDType::FLOAT32, 0);
    MxBase::Tensor::TensorMalloc(tensor2);
    APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::FLOAT32);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Fail_When_DstDtype_And_DataType_Not_Match)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2(SHAPE3, TensorDType::FLOAT32, 0);
    MxBase::Tensor::TensorMalloc(tensor2);
    APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::FLOAT16);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Fail_When_Src_Shape_Is_Different_With_Dst)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2(SHAPE3_DIFFC, TensorDType::FLOAT32, 0);
    MxBase::Tensor::TensorMalloc(tensor2);
    APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::FLOAT32);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Success_When_Src_Is_Uint8_And_Dst_Is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
        tensor1.ToDevice(0);
        Tensor tensor2(SHAPE3, TensorDType::FLOAT32, 0);
        MxBase::Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::FLOAT32);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(tensor2.GetDataType(), MxBase::TensorDType::FLOAT32);
    }
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Success_When_Src_Is_Uint8_And_Dst_Is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3, TensorDType::UINT8, 0);
        MxBase::Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE3, TensorDType::FLOAT16, 0);
        MxBase::Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::FLOAT16);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(tensor2.GetDataType(), MxBase::TensorDType::FLOAT16);
    }
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Success_When_Src_Is_Float16_And_Dst_Is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3, TensorDType::FLOAT16, 0);
        MxBase::Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE3, TensorDType::FLOAT32, 0);
        MxBase::Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::FLOAT32);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(tensor2.GetDataType(), MxBase::TensorDType::FLOAT32);
    }
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Success_When_Src_Is_Float16_And_Dst_Is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3, TensorDType::FLOAT16, 0);
        MxBase::Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE3, TensorDType::UINT8, 0);
        MxBase::Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::UINT8);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(tensor2.GetDataType(), MxBase::TensorDType::UINT8);
    }
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Success_When_Src_Is_Float32_And_Dst_Is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3, TensorDType::FLOAT32, 0);
        MxBase::Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE3, TensorDType::UINT8, 0);
        MxBase::Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::UINT8);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(tensor2.GetDataType(), MxBase::TensorDType::UINT8);
    }
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Success_When_Src_Is_Float32_And_Dst_Is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3, TensorDType::FLOAT32, 0);
        MxBase::Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE3, TensorDType::FLOAT16, 0);
        MxBase::Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::FLOAT16);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(tensor2.GetDataType(), MxBase::TensorDType::FLOAT16);
    }
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Success_When_Src_Is_Uint16_And_Dst_Is_Uint32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3, TensorDType::UINT16, 0);
        MxBase::Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE3, TensorDType::UINT32, 0);
        MxBase::Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::UINT32);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(tensor2.GetDataType(), MxBase::TensorDType::UINT32);
    }
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Success_When_Src_Is_Uint16_And_Dst_Is_Uint64)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3, TensorDType::UINT16, 0);
        MxBase::Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE3, TensorDType::UINT64, 0);
        MxBase::Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::UINT64);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(tensor2.GetDataType(), MxBase::TensorDType::UINT64);
    }
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Success_When_Src_Is_Uint32_And_Dst_Is_Uint16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3, TensorDType::UINT32, 0);
        MxBase::Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE3, TensorDType::UINT16, 0);
        MxBase::Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::UINT16);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(tensor2.GetDataType(), MxBase::TensorDType::UINT16);
    }
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Success_When_Src_Is_Uint32_And_Dst_Is_Uint64)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3, TensorDType::UINT32, 0);
        MxBase::Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE3, TensorDType::UINT64, 0);
        MxBase::Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::UINT64);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(tensor2.GetDataType(), MxBase::TensorDType::UINT64);
    }
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Success_When_Src_Is_Uint64_And_Dst_Is_Uint16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3, TensorDType::UINT64, 0);
        MxBase::Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE3, TensorDType::UINT16, 0);
        MxBase::Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::UINT16);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(tensor2.GetDataType(), MxBase::TensorDType::UINT16);
    }
}

TEST_F(PerElementOperationTest, Test_ConvertTo_Should_Return_Success_When_Src_Is_Uint64_And_Dst_Is_Uint32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3, TensorDType::UINT64, 0);
        MxBase::Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPE3, TensorDType::UINT32, 0);
        MxBase::Tensor::TensorMalloc(tensor2);
        APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::UINT32);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(tensor2.GetDataType(), MxBase::TensorDType::UINT32);
    }
}

TEST_F(PerElementOperationTest,
    Test_ConvertTo_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Stream)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2;
    MxBase::AscendStream stream = MxBase::AscendStream(1);
    stream.CreateAscendStream();
    APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::FLOAT32, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest,
    Test_ConvertTo_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_Stream)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2(SHAPE3_DIFFC, TensorDType::FLOAT32, 1);
    MxBase::Tensor::TensorMalloc(tensor2);
    MxBase::AscendStream stream = MxBase::AscendStream(0);
    stream.CreateAscendStream();
    APP_ERROR ret = ConvertTo(tensor1, tensor2, MxBase::TensorDType::FLOAT32, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest, Test_ConvertToRoi_Should_Return_Success_When_Dst_Is_Empty)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data4, CONVERTTO_ROI_SRC_HWSHAPE, TensorDType::UINT8);
        src.ToDevice(0);
        src = Tensor(src, CONVERTTO_ROI_RECT);
        Tensor dst;
        APP_ERROR ret = ConvertTo(src, dst, MxBase::TensorDType::FLOAT32);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(),
            std::vector<uint32_t>{CONVERTTO_ROI_DST_H, CONVERTTO_ROI_DST_H}), true);
        dst.ToHost();
        for (size_t i = 0; i < CONVERTTO_ROI_DST_LEN; i++) {
            EXPECT_EQ(g_data10[i], static_cast<float *>(dst.GetData())[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_ConvertToRoi_Should_Return_Fail_When_Dst_Shape_Not_Match_Src_Roi)
{
    Tensor src(&g_data4, CONVERTTO_ROI_SRC_HWSHAPE, TensorDType::UINT8);
    src.ToDevice(0);
    src = Tensor(src, CONVERTTO_ROI_RECT);
    Tensor dst(shapeHW, TensorDType::FLOAT32, 0);
    dst.Malloc();
    APP_ERROR ret = ConvertTo(src, dst, MxBase::TensorDType::FLOAT32);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ConvertToRoi_Should_Return_Fail_When_Dst_With_Roi)
{
    Tensor src(&g_data4, CONVERTTO_ROI_SRC_HWSHAPE, TensorDType::UINT8);
    src.ToDevice(0);
    src = Tensor(src, CONVERTTO_ROI_RECT);
    Tensor dst(CONVERTTO_ROI_DST_HWSHAPE, TensorDType::FLOAT32, 0);
    dst.Malloc();
    const Rect dstRect = {0, 0, 1, 1};
    dst = Tensor(dst, dstRect);
    APP_ERROR ret = ConvertTo(src, dst, MxBase::TensorDType::FLOAT32);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ConvertToRoi_Should_Return_Success_When_Dst_Shape_Match_Src_Roi)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data4, CONVERTTO_ROI_SRC_HWSHAPE, TensorDType::UINT8);
        src.ToDevice(0);
        src = Tensor(src, CONVERTTO_ROI_RECT);
        Tensor dst(CONVERTTO_ROI_DST_HWSHAPE, TensorDType::FLOAT32, 0);
        dst.Malloc();
        APP_ERROR ret = ConvertTo(src, dst, MxBase::TensorDType::FLOAT32);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(),
            std::vector<uint32_t>{CONVERTTO_ROI_DST_H, CONVERTTO_ROI_DST_H}), true);
        dst.ToHost();
        for (size_t i = 0; i < CONVERTTO_ROI_DST_LEN; i++) {
            EXPECT_EQ(g_data10[i], static_cast<float *>(dst.GetData())[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_ConvertToRoi_Should_Return_Fail_When_Src_Fp32_Dst_Fp16)
{
    Tensor src(CONVERTTO_ROI_SRC_HWSHAPE, TensorDType::FLOAT32, 0);
    src.Malloc();
    src = Tensor(src, CONVERTTO_ROI_RECT);
    Tensor dst;
    APP_ERROR ret = ConvertTo(src, dst, MxBase::TensorDType::FLOAT16);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ConvertToRoi_Should_Return_Fail_When_Src_Fp16_Dst_Fp32)
{
    Tensor src(CONVERTTO_ROI_SRC_HWSHAPE, TensorDType::FLOAT16, 0);
    src.Malloc();
    src = Tensor(src, CONVERTTO_ROI_RECT);
    Tensor dst;
    APP_ERROR ret = ConvertTo(src, dst, MxBase::TensorDType::FLOAT32);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ConvertToRoi_Should_Return_Success_When_Src_U8_Dst_Fp32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data4, CONVERTTO_ROI_SRC_HWSHAPE, TensorDType::UINT8);
        src.ToDevice(0);
        src = Tensor(src, CONVERTTO_ROI_RECT);
        Tensor dst;
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = ConvertTo(src, dst, MxBase::TensorDType::FLOAT32, stream);
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(),
            std::vector<uint32_t>{CONVERTTO_ROI_DST_H, CONVERTTO_ROI_DST_H}), true);
        dst.ToHost();
        for (size_t i = 0; i < CONVERTTO_ROI_DST_LEN; i++) {
            EXPECT_EQ(g_data10[i], static_cast<float *>(dst.GetData())[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_ConvertToRoi_Should_Return_Success_When_Src_U8_Dst_Fp16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data4, CONVERTTO_ROI_SRC_HWSHAPE, TensorDType::UINT8);
        src.ToDevice(0);
        src = Tensor(src, CONVERTTO_ROI_RECT);
        Tensor dst;
        APP_ERROR ret = ConvertTo(src, dst, MxBase::TensorDType::FLOAT16);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(),
            std::vector<uint32_t>{CONVERTTO_ROI_DST_H, CONVERTTO_ROI_DST_H}), true);

        Tensor dst2;
        ret = ConvertTo(dst, dst2, MxBase::TensorDType::FLOAT32);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst2.ToHost();
        for (size_t i = 0; i < CONVERTTO_ROI_DST_LEN; i++) {
            EXPECT_EQ(g_data10[i], static_cast<float *>(dst2.GetData())[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_ConvertToRoi_Should_Return_Success_When_Src_Fp32_Dst_U8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data11, CONVERTTO_ROI_SRC_HWSHAPE, TensorDType::FLOAT32);
        src.ToDevice(0);
        src = Tensor(src, CONVERTTO_ROI_RECT);
        Tensor dst;
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = ConvertTo(src, dst, MxBase::TensorDType::UINT8, stream);
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(),
            std::vector<uint32_t>{CONVERTTO_ROI_DST_H, CONVERTTO_ROI_DST_H}), true);
        dst.ToHost();
        for (size_t i = 0; i < CONVERTTO_ROI_DST_LEN; i++) {
            EXPECT_EQ(g_data10[i], static_cast<uint8_t *>(dst.GetData())[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_ConvertToRoi_Should_Return_Success_When_Src_Fp16_Dst_U8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data11, CONVERTTO_ROI_SRC_HWSHAPE, TensorDType::FLOAT32);
        src.ToDevice(0);
        Tensor srcFp16;
        APP_ERROR ret = ConvertTo(src, srcFp16, MxBase::TensorDType::FLOAT16);
        EXPECT_EQ(ret, APP_ERR_OK);
        srcFp16 = Tensor(srcFp16, CONVERTTO_ROI_RECT);
        Tensor dst;
        ret = ConvertTo(srcFp16, dst, MxBase::TensorDType::UINT8);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(),
            std::vector<uint32_t>{CONVERTTO_ROI_DST_H, CONVERTTO_ROI_DST_H}), true);
        dst.ToHost();
        for (size_t i = 0; i < CONVERTTO_ROI_DST_LEN; i++) {
            EXPECT_EQ(g_data10[i], static_cast<uint8_t *>(dst.GetData())[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_ConvertToRoi_Should_Return_Success_When_Tensor_HW)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data4, CONVERTTO_ROI_SRC_HWSHAPE, TensorDType::UINT8);
        src.ToDevice(0);
        src = Tensor(src, CONVERTTO_ROI_RECT);
        Tensor dst;
        APP_ERROR ret = ConvertTo(src, dst, MxBase::TensorDType::FLOAT32);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(),
            std::vector<uint32_t>{CONVERTTO_ROI_DST_H, CONVERTTO_ROI_DST_H}), true);
        dst.ToHost();
        for (size_t i = 0; i < CONVERTTO_ROI_DST_LEN; i++) {
            EXPECT_EQ(g_data10[i], static_cast<float *>(dst.GetData())[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_ConvertToRoi_Should_Return_Success_When_Tensor_HWC)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(CONVERTTO_ROI_SRC_HWCSHAPE, TensorDType::UINT8, 0);
        src.Malloc();
        src = Tensor(src, CONVERTTO_ROI_RECT);
        Tensor dst;
        APP_ERROR ret = ConvertTo(src, dst, MxBase::TensorDType::FLOAT32);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{CONVERTTO_ROI_DST_H,
            CONVERTTO_ROI_DST_H, SHAPE_DIMENSION_THREE}), true);
    }
}

TEST_F(PerElementOperationTest, Test_ConvertToRoi_Should_Return_Success_When_Tensor_NHWC)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(CONVERTTO_ROI_SRC_NHWCSHAPE, TensorDType::UINT8, 0);
        src.Malloc();
        src = Tensor(src, CONVERTTO_ROI_RECT);
        Tensor dst;
        APP_ERROR ret = ConvertTo(src, dst, MxBase::TensorDType::FLOAT32);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{SHAPE_DIMENSION_ONE,
            CONVERTTO_ROI_DST_H, CONVERTTO_ROI_DST_H, SHAPE_DIMENSION_THREE}), true);
    }
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_minVal_Greater_than_maxVal)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::FLOAT32);
    Tensor tensor2(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(tensor2);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    const float minVal = 11.0f;
    const float maxVal = 3.0f;
    APP_ERROR ret = Clip(tensor1, tensor2, minVal, maxVal);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src1;
    Tensor src2;
    Tensor dst;
    APP_ERROR ret = Min(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst;
    APP_ERROR ret = Min(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Fail_When_Src_Datatype_Is_Int)
{
    Tensor src1(SHAPE3, TensorDType::INT32);
    Tensor src2(SHAPE3, TensorDType::INT32);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = Min(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src1(&g_data3, SHAPE5, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE5, TensorDType::FLOAT32);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = Min(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Fail_When_Src_Dst_Shape_Is_Different)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE4, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(0);
    APP_ERROR ret = Min(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Fail_When_Src_Datatype_Is_Different_With_Dst)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT16);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(0);
    APP_ERROR ret = Min(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    APP_ERROR ret = Min(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(1);
    APP_ERROR ret = Min(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst;
    src1.ToDevice(1);
    src2.ToDevice(1);
    APP_ERROR ret = Min(src1, src2, dst, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(1);
    APP_ERROR ret = Min(src1, src2, dst, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Success_When_Dst_Is_Float_Src_Is_Float)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        Tensor dst;
        src1.ToDevice(0);
        src2.ToDevice(0);
        APP_ERROR ret = Min(src1, src2, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], g_data6[SHAPE_DIMENSION_ZERO]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE], g_data6[SHAPE_DIMENSION_ONE]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO], g_data6[SHAPE_DIMENSION_TWO]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_THREE], g_data6[SHAPE_DIMENSION_THREE]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], g_data7[SHAPE_DIMENSION_FOUR]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], g_data7[SHAPE_DIMENSION_FIVE]);
    }
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Success_When_Dst_Is_Float16_Src_Is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        Tensor dst;
        src1.ToDevice(0);
        src2.ToDevice(0);
        Tensor src1Fp16;
        Tensor src2Fp16;
        ConvertTo(src1, src1Fp16, TensorDType::FLOAT16);
        ConvertTo(src2, src2Fp16, TensorDType::FLOAT16);
        APP_ERROR ret = Min(src1Fp16, src2Fp16, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        Tensor dst_fp32;
        ConvertTo(dst, dst_fp32, TensorDType::FLOAT32);
        dst_fp32.ToHost();
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_ZERO], g_data6[SHAPE_DIMENSION_ZERO]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_ONE], g_data6[SHAPE_DIMENSION_ONE]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_TWO], g_data6[SHAPE_DIMENSION_TWO]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_THREE], g_data6[SHAPE_DIMENSION_THREE]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_FOUR], g_data7[SHAPE_DIMENSION_FOUR]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_FIVE], g_data7[SHAPE_DIMENSION_FIVE]);
    }
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Success_When_Dst_Is_Uint8_Src_Is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(&g_data4, SHAPE3, TensorDType::UINT8);
        Tensor src2(&g_data5, SHAPE3, TensorDType::UINT8);
        Tensor dst;
        src1.ToDevice(0);
        src2.ToDevice(0);
        APP_ERROR ret = Min(src1, src2, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], g_data4[SHAPE_DIMENSION_ZERO]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_ONE], g_data4[SHAPE_DIMENSION_ONE]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_TWO], g_data4[SHAPE_DIMENSION_TWO]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_THREE], g_data4[SHAPE_DIMENSION_THREE]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], g_data5[SHAPE_DIMENSION_FOUR]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], g_data5[SHAPE_DIMENSION_FIVE]);
    }
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src1;
    Tensor src2;
    Tensor dst;
    APP_ERROR ret = Max(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst;
    APP_ERROR ret = Max(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Fail_When_Src_Datatype_Is_Int)
{
    Tensor src1(SHAPE3, TensorDType::INT32);
    Tensor src2(SHAPE3, TensorDType::INT32);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = Max(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src1(&g_data3, SHAPE5, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE5, TensorDType::FLOAT32);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = Max(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Fail_When_Src_Dst_Shape_Is_Different)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE4, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(0);
    APP_ERROR ret = Max(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Fail_When_Src_Datatype_Is_Different_With_Dst)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT16);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(0);
    APP_ERROR ret = Max(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    APP_ERROR ret = Max(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(1);
    APP_ERROR ret = Max(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst;
    src1.ToDevice(1);
    src2.ToDevice(1);
    APP_ERROR ret = Max(src1, src2, dst, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(1);
    APP_ERROR ret = Max(src1, src2, dst, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Success_When_Dst_Is_Float_Src_Is_Float)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        Tensor dst;
        src1.ToDevice(0);
        src2.ToDevice(0);
        APP_ERROR ret = Max(src1, src2, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], g_data7[SHAPE_DIMENSION_ZERO]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE], g_data7[SHAPE_DIMENSION_ONE]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO], g_data7[SHAPE_DIMENSION_TWO]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_THREE], g_data7[SHAPE_DIMENSION_THREE]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], g_data6[SHAPE_DIMENSION_FOUR]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], g_data6[SHAPE_DIMENSION_FIVE]);
    }
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Success_When_Dst_Is_Float16_Src_Is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        Tensor dst;
        src1.ToDevice(0);
        src2.ToDevice(0);
        Tensor src1Fp16;
        Tensor src2Fp16;
        ConvertTo(src1, src1Fp16, TensorDType::FLOAT16);
        ConvertTo(src2, src2Fp16, TensorDType::FLOAT16);
        APP_ERROR ret = Max(src1Fp16, src2Fp16, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        Tensor dst_fp32;
        ConvertTo(dst, dst_fp32, TensorDType::FLOAT32);
        dst_fp32.ToHost();
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_ZERO], g_data7[SHAPE_DIMENSION_ZERO]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_ONE], g_data7[SHAPE_DIMENSION_ONE]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_TWO], g_data7[SHAPE_DIMENSION_TWO]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_THREE], g_data7[SHAPE_DIMENSION_THREE]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_FOUR], g_data6[SHAPE_DIMENSION_FOUR]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_FIVE], g_data6[SHAPE_DIMENSION_FIVE]);
    }
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Success_When_Dst_Is_Uint8_Src_Is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(&g_data4, SHAPE3, TensorDType::UINT8);
        Tensor src2(&g_data5, SHAPE3, TensorDType::UINT8);
        Tensor dst;
        src1.ToDevice(0);
        src2.ToDevice(0);
        APP_ERROR ret = Max(src1, src2, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], g_data5[SHAPE_DIMENSION_ZERO]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_ONE], g_data5[SHAPE_DIMENSION_ONE]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_TWO], g_data5[SHAPE_DIMENSION_TWO]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_THREE], g_data5[SHAPE_DIMENSION_THREE]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], g_data4[SHAPE_DIMENSION_FOUR]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], g_data4[SHAPE_DIMENSION_FIVE]);
    }
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src;
    Tensor dst;
    APP_ERROR ret = Abs(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst;
    APP_ERROR ret = Abs(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Fail_When_Src_Datatype_Is_Int)
{
    Tensor src(SHAPE3, TensorDType::INT32);
    Tensor::TensorMalloc(src);
    src.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = Abs(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src(&g_data3, SHAPE5, TensorDType::FLOAT32);
    src.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = Abs(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Fail_When_Src_Dst_Shape_Is_Different)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE4, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src.ToDevice(0);
    dst.ToDevice(0);
    APP_ERROR ret = Abs(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Fail_When_Src_Datatype_Is_Different_With_Dst)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT16);
    Tensor::TensorMalloc(dst);
    src.ToDevice(0);
    dst.ToDevice(0);
    APP_ERROR ret = Abs(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src.ToDevice(0);
    APP_ERROR ret = Abs(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src.ToDevice(0);
    dst.ToDevice(1);
    APP_ERROR ret = Abs(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst;
    src.ToDevice(1);
    APP_ERROR ret = Abs(src, dst, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src.ToDevice(0);
    dst.ToDevice(1);
    APP_ERROR ret = Abs(src, dst, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Success_When_Dst_Is_Float_Src_Is_Float)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src(&g_data8, SHAPE3, TensorDType::FLOAT32);
        Tensor dst;
        src.ToDevice(0);
        APP_ERROR ret = Abs(src, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], g_data9[SHAPE_DIMENSION_ZERO]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE], g_data9[SHAPE_DIMENSION_ONE]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO], g_data9[SHAPE_DIMENSION_TWO]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_THREE], g_data9[SHAPE_DIMENSION_THREE]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], g_data9[SHAPE_DIMENSION_FOUR]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], g_data9[SHAPE_DIMENSION_FIVE]);
    }
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Success_When_Dst_Is_Float16_Src_Is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src(&g_data8, SHAPE3, TensorDType::FLOAT32);
        Tensor dst;
        src.ToDevice(0);
        Tensor srcFp16;
        ConvertTo(src, srcFp16, TensorDType::FLOAT16);
        APP_ERROR ret = Abs(srcFp16, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        Tensor dst_fp32;
        ConvertTo(dst, dst_fp32, TensorDType::FLOAT32);
        dst_fp32.ToHost();
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_ZERO], g_data9[SHAPE_DIMENSION_ZERO]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_ONE], g_data9[SHAPE_DIMENSION_ONE]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_TWO], g_data9[SHAPE_DIMENSION_TWO]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_THREE], g_data9[SHAPE_DIMENSION_THREE]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_FOUR], g_data9[SHAPE_DIMENSION_FOUR]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_FIVE], g_data9[SHAPE_DIMENSION_FIVE]);
    }
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Success_When_Dst_Is_Uint8_Src_Is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src(&g_data4, SHAPE3, TensorDType::UINT8);
        Tensor dst;
        src.ToDevice(0);
        APP_ERROR ret = Abs(src, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], g_data4[SHAPE_DIMENSION_ZERO]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_ONE], g_data4[SHAPE_DIMENSION_ONE]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_TWO], g_data4[SHAPE_DIMENSION_TWO]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_THREE], g_data4[SHAPE_DIMENSION_THREE]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], g_data4[SHAPE_DIMENSION_FOUR]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], g_data4[SHAPE_DIMENSION_FIVE]);
    }
}

TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Success_When_Input_Parameters_OK)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(SHAPE3, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        uint8_t valueForRescale = 100;
        src.SetTensorValue(valueForRescale);
        Tensor dst;
        APP_ERROR ret = Rescale(src, dst, 2.0f, 1.0f);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        uint8_t expectRescaletResult = 201;
        EXPECT_EQ(static_cast<uint8_t *>(dst.GetData())[0], expectRescaletResult);
    }
}

TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src;
    Tensor dst;
    APP_ERROR ret = Rescale(src, dst, 3.0f, 1.0f);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src(SHAPE3, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Rescale(src, dst, 3.0f, 1.0f);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Fail_When_Src_Datatype_Is_Int32)
{
    Tensor src(SHAPE3, TensorDType::INT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Rescale(src, dst, 3.0f, 1.0f);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src(SHAPE5, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Rescale(src, dst, 3.0f, 1.0f);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE3, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Rescale(src, dst, 3.0f, 1.0f);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Fail_When_Dst_Datatype_Is_Int32)
{
    Tensor src(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE3, TensorDType::INT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Rescale(src, dst, 3.0f, 1.0f);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Fail_When_Src_Datatype_Is_Different_With_Dst)
{
    Tensor src(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE3, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Rescale(src, dst, 3.0f, 1.0f);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE3, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Rescale(src, dst, 3.0f, 1.0f);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_Rescale_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    Tensor src(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE3, TensorDType::UINT8, 1);
    AscendStream stream(1);
    stream.CreateAscendStream();
    APP_ERROR ret = Rescale(src, dst, 3.0f, 1.0f, stream);
    stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Fail_When_Dst_Shape_Is_Different_With_Src)
{
    Tensor src(SHAPE3, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Rescale(src, dst, 3.0f, 1.0f);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src1;
    Tensor src2;
    Tensor dst;
    APP_ERROR ret = Add(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor src2(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor dst;
    APP_ERROR ret = Add(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_Src_Datatype_Is_Int)
{
    Tensor src1(SHAPE3, TensorDType::INT32);
    Tensor src2(SHAPE3, TensorDType::INT32);
    Tensor dst;
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);

    src1.ToDevice(0);
    src2.ToDevice(0);
    APP_ERROR ret = Add(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src1(&g_data3, SHAPE5, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE5, TensorDType::FLOAT32);
    Tensor dst;
    src1.ToDevice(0);
    src2.ToDevice(0);
    APP_ERROR ret = Add(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_Src_Shape_Different)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE4, TensorDType::FLOAT32);
    Tensor dst;
    src1.ToDevice(0);
    src2.ToDevice(0);
    APP_ERROR ret = Add(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_Src_Dtype_Different)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor dst;
    src1.ToDevice(0);
    src2.ToDevice(0);
    APP_ERROR ret = Add(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_Dst_Dtype_Is_Different_with_Src)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT16);
    Tensor::TensorMalloc(dst);

    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(0);
    APP_ERROR ret = Add(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_Src_Shape_Is_Different_With_Dst)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE4, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);

    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(0);
    APP_ERROR ret = Add(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);

    src1.ToDevice(0);
    src2.ToDevice(0);
    APP_ERROR ret = Add(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);

    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(1);
    APP_ERROR ret = Add(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst;

    src1.ToDevice(1);
    src2.ToDevice(1);

    APP_ERROR ret = Add(src1, src2, dst, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_src1_DeviceId_Is_Different_With_src2)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst;

    src1.ToDevice(0);
    src2.ToDevice(1);

    APP_ERROR ret = Add(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);

    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(1);

    APP_ERROR ret = Add(src1, src2, dst, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Success_When_Dst_is_Float_Src_is_Float)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
        Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
        Tensor dst(SHAPE3, TensorDType::FLOAT32);
        Tensor::TensorMalloc(dst);

        src1.ToDevice(0);
        src2.ToDevice(0);
        dst.ToDevice(0);

        APP_ERROR ret = Add(src1, src2, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_aclCreateScalar_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        aclScalar *alpha = nullptr;
        MOCKER_CPP(AclApi::aclCreateScalar).stubs().will(returnValue(alpha));
        Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
        Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
        Tensor dst(SHAPE3, TensorDType::FLOAT32);
        Tensor::TensorMalloc(dst);

        src1.ToDevice(0);
        src2.ToDevice(0);
        dst.ToDevice(0);

        APP_ERROR ret = Add(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
    }
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Fail_When_aclnnAddGetWorkspaceSize_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(AclApi::aclnnAddGetWorkspaceSize).stubs().will(returnValue(1));
        Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
        Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
        Tensor dst(SHAPE3, TensorDType::FLOAT32);
        Tensor::TensorMalloc(dst);

        src1.ToDevice(0);
        src2.ToDevice(0);
        dst.ToDevice(0);

        APP_ERROR ret = Add(src1, src2, dst);
        EXPECT_EQ(ret, 1);
    }
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Failed_When_Src1_RoiShape_IsNot_Equal_To_Src2_RoiShape)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(shapeHWC_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);

        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_3, ROI_Y1_3);
        src2 = Tensor(src2, roi_src2);

        APP_ERROR ret = Add(src1, src2, src2, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Failed_When_Src1_RoiShape_IsNot_Equal_To_Dst_RoiShape)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(shapeHWC_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);

        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_3, ROI_Y1_3);
        dst = Tensor(dst, roi_dst);

        APP_ERROR ret = Add(src1, src2, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Failed_When_Src1_SetRoi_Src2_SetRoi_Dst_Is_Empty)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(shapeHWC_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst;

        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);

        APP_ERROR ret = Add(src1, src2, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

static void Expect_AddInplaceResult(Tensor &src)
{
    src.ToHost();
    auto shapes = src.GetShape();
    auto totalElements = 1;
    for (auto const s : shapes) {
        totalElements *= s;
    }
    /*
     * src1 shape(1,5,4,3), roi(1,1,2,2)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     *
     * src2 shape(1,5,4,3), roi(1,1,2,2)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     *
     * dst shape(1,5,4,3), roi(1,1,2,2)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     * (1,1,1) (2,2,2) (1,1,1) (1,1,1)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     * (1,1,1) (1,1,1) (1,1,1) (1,1,1)
     */
    for (int i = 0; i < totalElements; i++) {
        if (i == VALID_ADD_INPLACE_RESULT_INDEX_15 || i == VALID_ADD_INPLACE_RESULT_INDEX_16 ||
            i == VALID_ADD_INPLACE_RESULT_INDEX_17) {
            EXPECT_EQ(static_cast<uint8_t *>(src.GetData())[(int64_t)i], VALID_ADD_INPLACE_RESULT);
        } else {
            EXPECT_EQ(static_cast<uint8_t *>(src.GetData())[(int64_t)i], 1);
        }
    }
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Success_When_Datatype_Is_U8_Shape_Is_NHWC_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(shapeNHWC_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeNHWC_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        src1.SetTensorValue((uint8_t)1);
        src2.SetTensorValue((uint8_t)1);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);

        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        APP_ERROR ret = Add(src1, src2, src2, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        Expect_AddInplaceResult(src2);
    }
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Success_When_Datatype_Is_U8_Shape_Is_HWC_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(shapeHWC_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        src1.SetTensorValue((uint8_t)1);
        src2.SetTensorValue((uint8_t)1);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        APP_ERROR ret = Add(src1, src2, src2, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        Expect_AddInplaceResult(src2);
    }
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Success_When_Datatype_Is_U8_Shape_Is_HW_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(shapeHW_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHW_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        src1.SetTensorValue((uint8_t)1);
        src2.SetTensorValue((uint8_t)1);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        APP_ERROR ret = Add(src1, src2, src2, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        src2.ToHost();
        auto shapes = src2.GetShape();
        auto totalElements = 1;
        for (auto const s : shapes) {
            totalElements *= s;
        }
        for (int i = 0; i < totalElements; i++) {
            if (i == VALID_ADD_INPLACE_RESULT_INDEX_5) {
                EXPECT_EQ(static_cast<uint8_t *>(src2.GetData())[(int64_t)i], VALID_ADD_INPLACE_RESULT);
            } else {
                EXPECT_EQ(static_cast<uint8_t *>(src2.GetData())[(int64_t)i], 1);
            }
        }
    }
}

TEST_F(PerElementOperationTest, Test_Add_Should_Return_Success_When_Datatype_Is_U8_Only_Roishape_Is_Equal)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(shapeHW_add, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHW_add2, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        src1.SetTensorValue((uint8_t)1);
        src2.SetTensorValue((uint8_t)1);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        APP_ERROR ret = Add(src1, src2, src1, stream0);
        EXPECT_EQ(ret, APP_ERR_OK);
        ret = stream0.Synchronize();
        EXPECT_EQ(ret, APP_ERR_OK);
        src1.ToHost();
        auto shapes = src1.GetShape();
        auto totalElements = 1;
        for (auto const s : shapes) {
            totalElements *= s;
        }
        for (int i = 0; i < totalElements; i++) {
            if (i == VALID_ADD_INPLACE_RESULT_INDEX_5) {
                EXPECT_EQ(static_cast<uint8_t *>(src1.GetData())[(int64_t)i], VALID_ADD_INPLACE_RESULT);
            } else {
                EXPECT_EQ(static_cast<uint8_t *>(src1.GetData())[(int64_t)i], 1);
            }
        }
        stream0.DestroyAscendStream();
    }
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = Subtract(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = Multiply(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = Divide(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Sum_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst(std::vector<uint32_t> {CHANNEL}, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src = Tensor(src, roi_src);
        APP_ERROR ret = Sum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseAnd_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = BitwiseAnd(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseOr_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = BitwiseOr(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}


TEST_F(PerElementOperationTest, Test_BitwiseXor_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = BitwiseXor(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_BitwiseNot_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src = Tensor(src, roi_src);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = BitwiseNot(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Cilp_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(src);
        Tensor dst(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src = Tensor(src, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = Pow(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Exp_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(src);
        Tensor dst(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src = Tensor(src, roi_src);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = Exp(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Sqrt_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(src);
        Tensor dst(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src = Tensor(src, roi_src);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = Sqrt(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Sqrt_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src;
    Tensor dst;
    APP_ERROR ret = Sqrt(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqrt_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src(shapeHWC, TensorDType::FLOAT16);
    src.Malloc();
    Tensor dst;
    APP_ERROR ret = Sqrt(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqrt_Should_Return_Fail_When_Src_Datatype_Is_Uint8)
{
    Tensor src(shapeHWC, TensorDType::UINT8, 0);
    src.Malloc();
    Tensor dst;
    APP_ERROR ret = Sqrt(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqrt_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src(invalidShape, TensorDType::FLOAT16, 0);
    src.Malloc();
    Tensor dst;
    APP_ERROR ret = Sqrt(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqrt_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src(shapeHWC, TensorDType::FLOAT16, 0);
    src.Malloc();
    Tensor dst(shapeHWC, TensorDType::FLOAT16);
    dst.Malloc();
    APP_ERROR ret = Sqrt(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqrt_Should_Return_Fail_When_Src_Datatype_Is_Different_With_Dst)
{
    Tensor src(shapeHWC, TensorDType::FLOAT16, 0);
    src.Malloc();
    Tensor dst(shapeHWC, TensorDType::UINT8, 0);
    dst.Malloc();
    APP_ERROR ret = Sqrt(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqrt_Should_Return_Fail_When_Src_Shape_Is_Different_With_Dst)
{
    Tensor src(shapeHWC, TensorDType::FLOAT16, 0);
    src.Malloc();
    Tensor dst(shapeNHWC, TensorDType::FLOAT16, 0);
    dst.Malloc();
    APP_ERROR ret = Sqrt(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqrt_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src(shapeHWC, TensorDType::FLOAT16, 0);
    src.Malloc();
    Tensor dst(shapeNHWC, TensorDType::FLOAT16, 1);
    dst.Malloc();
    APP_ERROR ret = Sqrt(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sqrt_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    MxBase::AscendStream stream(1);
    stream.CreateAscendStream();
    Tensor src(shapeHWC, TensorDType::FLOAT16, 0);
    src.Malloc();
    Tensor dst(shapeNHWC, TensorDType::FLOAT16, 1);
    dst.Malloc();
    APP_ERROR ret = Sqrt(src, dst, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.Synchronize();
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest, Test_Sqrt_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_StreamId)
{
    MxBase::AscendStream stream(1);
    stream.CreateAscendStream();
    Tensor src(shapeHWC, TensorDType::FLOAT16, 1);
    src.Malloc();
    Tensor dst(shapeNHWC, TensorDType::FLOAT16, 0);
    dst.Malloc();
    APP_ERROR ret = Sqrt(src, dst, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.Synchronize();
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest, Test_Sqrt_Should_Return_Success_When_Src_Is_Float32)
{
    MxBase::AscendStream stream(0);
    stream.CreateAscendStream();
    Tensor src(&g_sqrResult2, SHAPE3, TensorDType::FLOAT32);
    src.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = Sqrt(src, dst, stream);
    ASSERT_EQ(ret, APP_ERR_OK);
    stream.Synchronize();
    stream.DestroyAscendStream();
    dst.ToHost();
    for (size_t i = 0; i < DATA_LEN; i++) {
        EXPECT_EQ(static_cast<float *>(dst.GetData())[i], g_data7[i]);
    }
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src1;
    Tensor src2;
    Tensor dst;
    APP_ERROR ret = Pow(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src1(shapeHWC, TensorDType::FLOAT16);
    src1.Malloc();
    Tensor src2(shapeHWC, TensorDType::FLOAT16);
    src2.Malloc();
    Tensor dst;
    APP_ERROR ret = Pow(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Fail_When_Src_Datatype_Is_Int32)
{
    Tensor src1(shapeHWC, TensorDType::INT32, 0);
    src1.Malloc();
    Tensor src2(shapeHWC, TensorDType::INT32, 0);
    src2.Malloc();
    Tensor dst;
    APP_ERROR ret = Pow(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src1(invalidShape, TensorDType::FLOAT16, 0);
    src1.Malloc();
    Tensor src2(invalidShape, TensorDType::FLOAT16, 0);
    src2.Malloc();
    Tensor dst;
    APP_ERROR ret = Pow(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src1(shapeHWC, TensorDType::FLOAT16, 0);
    src1.Malloc();
    Tensor src2(shapeHWC, TensorDType::FLOAT16, 0);
    src2.Malloc();
    Tensor dst(shapeHWC, TensorDType::FLOAT16);
    dst.Malloc();
    APP_ERROR ret = Pow(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Fail_When_Src_Datatype_Is_Different_With_Dst)
{
    Tensor src1(shapeHWC, TensorDType::FLOAT16, 0);
    src1.Malloc();
    Tensor src2(shapeHWC, TensorDType::FLOAT16, 0);
    src2.Malloc();
    Tensor dst(shapeHWC, TensorDType::UINT8, 0);
    dst.Malloc();
    APP_ERROR ret = Pow(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Fail_When_Src_Shape_Is_Different_With_Dst)
{
    Tensor src1(shapeHWC, TensorDType::FLOAT16, 0);
    src1.Malloc();
    Tensor src2(shapeHWC, TensorDType::FLOAT16, 0);
    src2.Malloc();
    Tensor dst(shapeNHWC, TensorDType::FLOAT16, 0);
    dst.Malloc();
    APP_ERROR ret = Pow(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src1(shapeHWC, TensorDType::FLOAT16, 0);
    src1.Malloc();
    Tensor src2(shapeHWC, TensorDType::FLOAT16, 0);
    src2.Malloc();
    Tensor dst(shapeNHWC, TensorDType::FLOAT16, 1);
    dst.Malloc();
    APP_ERROR ret = Pow(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    MxBase::AscendStream stream(1);
    stream.CreateAscendStream();
    Tensor src1(shapeHWC, TensorDType::FLOAT16, 0);
    src1.Malloc();
    Tensor src2(shapeHWC, TensorDType::FLOAT16, 0);
    src2.Malloc();
    Tensor dst(shapeNHWC, TensorDType::FLOAT16, 1);
    dst.Malloc();
    APP_ERROR ret = Pow(src1, src2, dst, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.Synchronize();
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_StreamId)
{
    MxBase::AscendStream stream(1);
    stream.CreateAscendStream();
    Tensor src1(shapeHWC, TensorDType::FLOAT16, 1);
    src1.Malloc();
    Tensor src2(shapeHWC, TensorDType::FLOAT16, 1);
    src2.Malloc();
    Tensor dst(shapeNHWC, TensorDType::FLOAT16, 0);
    dst.Malloc();
    APP_ERROR ret = Pow(src1, src2, dst, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.Synchronize();
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Success_When_DataType_Is_UINT8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data4, SHAPE3, TensorDType::UINT8);
        src1.ToDevice(0);
        Tensor src2(SHAPE3, TensorDType::UINT8, 0);
        src2.Malloc();
        const uint8_t value = 2;
        src2.SetTensorValue(value);
        Tensor dst;
        APP_ERROR ret = Pow(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        for (size_t i = 0; i < DATA_LEN; i++) {
            EXPECT_EQ(static_cast<uint8_t *>(dst.GetData())[i], g_sqrResult1[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Success_When_DataType_Is_FLOAT32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data7, SHAPE3, TensorDType::FLOAT32);
        src1.ToDevice(0);
        Tensor src2(SHAPE3, TensorDType::FLOAT32, 0);
        src2.Malloc();
        const float value = 2.0f;
        src2.SetTensorValue(value);
        Tensor dst;
        APP_ERROR ret = Pow(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        for (size_t i = 0; i < DATA_LEN; i++) {
            EXPECT_EQ(AreFloatValueEqual(static_cast<float *>(dst.GetData())[i], g_sqrResult2[i]), true);
        }
    }
}

TEST_F(PerElementOperationTest, Test_Pow_Should_Return_Success_When_DataType_Is_FLOAT16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data7, SHAPE3, TensorDType::FLOAT32);
        src1.ToDevice(0);
        Tensor src1Fp16;
        ConvertTo(src1, src1Fp16, TensorDType::FLOAT16);
        Tensor src2(SHAPE3, TensorDType::FLOAT16, 0);
        src2.Malloc();
        const float value = 2.0f;
        src2.SetTensorValue(value, true);
        Tensor dst;
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = Pow(src1Fp16, src2, dst, stream);
        stream.Synchronize();
        EXPECT_EQ(ret, APP_ERR_OK);
        Tensor dstFp32;
        ConvertTo(dst, dstFp32, TensorDType::FLOAT32);
        dstFp32.ToHost();
        for (size_t i = 0; i < DATA_LEN; i++) {
            EXPECT_EQ(AreFloatValueEqual(static_cast<float *>(dstFp32.GetData())[i], g_sqrResult2[i]), true);
        }
        stream.DestroyAscendStream();
    }
}

TEST_F(PerElementOperationTest, Test_Log_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(src);
        Tensor dst(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src = Tensor(src, roi_src);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = Log(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Sqr_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(src);
        Tensor dst(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src = Tensor(src, roi_src);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = Sqr(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_AddWeighted_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = AddWeighted(src1, 1.0, src2, 2.0, 3.0, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src;
    Tensor dst;
    APP_ERROR ret = ThresholdBinary(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Threshold(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor dst;
    APP_ERROR ret = ThresholdBinary(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Threshold(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Fail_When_Src_Datatype_Is_Int32)
{
    Tensor src(SHAPE3, TensorDType::INT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = ThresholdBinary(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Threshold(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src(&g_data3, SHAPE5, TensorDType::FLOAT32);
    src.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = ThresholdBinary(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Threshold(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Fail_When_Dst_Shape_Different)
{
    Tensor src(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(shapeHWC2, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = ThresholdBinary(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Threshold(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Fail_When_Dst_ShapeDim_Different)
{
    Tensor src(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(shapeNHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = ThresholdBinary(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Threshold(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Fail_When_Dst_Datatype_Different)
{
    Tensor src(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(shapeHWC, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = ThresholdBinary(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Threshold(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Fail_When_Dst_Device_Different)
{
    Tensor src(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(shapeHWC, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = ThresholdBinary(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Threshold(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Fail_When_Stream_Device_Different)
{
    Tensor src(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    MxBase::AscendStream stream(1);
    stream.CreateAscendStream();
    APP_ERROR ret = ThresholdBinary(src, dst, MIN_VAL, MAX_VAL, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Threshold(src, dst, MIN_VAL, MAX_VAL, ThresholdType::THRESHOLD_BINARY, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Success_When_Dtype_Is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data4, SHAPE3, TensorDType::UINT8);
        src.ToDevice(0);
        Tensor dst;
        float thresh = 3.0f;
        APP_ERROR ret = ThresholdBinary(src, dst, thresh, MAX_VAL);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], 0);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_ONE], 0);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_TWO], 0);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_THREE], 0);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], MAX_VAL);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], MAX_VAL);
    }
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Success_When_Dtype_Is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data4, SHAPE3, TensorDType::UINT8);
        src.ToDevice(0);
        Tensor srcFp16;
        ConvertTo(src, srcFp16, TensorDType::FLOAT16);
        Tensor dst;
        float thresh = 3.0f;
        APP_ERROR ret = ThresholdBinary(src, dst, thresh, MAX_VAL);
        EXPECT_EQ(ret, APP_ERR_OK);
        Tensor dst_fp32;
        ConvertTo(dst, dst_fp32, TensorDType::FLOAT32);
        dst_fp32.ToHost();
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_ZERO], 0);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_ONE], 0);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_TWO], 0);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_THREE], 0);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_FOUR], MAX_VAL);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_FIVE], MAX_VAL);
    }
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Success_When_Dtype_Is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data6, SHAPE3, TensorDType::FLOAT32);
        src.ToDevice(0);
        Tensor dst;
        float thresh = 3.0f;
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = ThresholdBinary(src, dst, thresh, MAX_VAL);
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], 0);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE], 0);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO], 0);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_THREE], 0);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], MAX_VAL);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], MAX_VAL);
    }
}

TEST_F(PerElementOperationTest, Test_ThresholdBinary_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(src);
        Tensor dst(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src = Tensor(src, roi_src);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = ThresholdBinary(src, dst, MIN_VAL, MAX_VAL);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        ret = Threshold(src, dst, MIN_VAL, MAX_VAL);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Abs_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(src);
        Tensor dst(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src = Tensor(src, roi_src);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = Abs(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_AbsDiff_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = AbsDiff(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_AbsDiff_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src1;
    Tensor src2;
    Tensor dst;
    APP_ERROR ret = AbsDiff(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AbsDiff_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst;
    APP_ERROR ret = AbsDiff(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AbsDiff_Should_Return_Fail_When_Src_Datatype_Is_Int)
{
    Tensor src1(SHAPE3, TensorDType::INT32);
    Tensor src2(SHAPE3, TensorDType::INT32);
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = AbsDiff(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AbsDiff_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src1(&g_data3, SHAPE5, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE5, TensorDType::FLOAT32);
    src1.ToDevice(0);
    src2.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = AbsDiff(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AbsDiff_Should_Return_Fail_When_Src_Dst_Shape_Is_Different)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE4, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(0);
    APP_ERROR ret = AbsDiff(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AbsDiff_Should_Return_Fail_When_Src_Datatype_Is_Different_With_Dst)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT16);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(0);
    APP_ERROR ret = AbsDiff(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AbsDiff_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    APP_ERROR ret = AbsDiff(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AbsDiff_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(1);
    APP_ERROR ret = AbsDiff(src1, src2, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_AbsDiff_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst;
    src1.ToDevice(1);
    src2.ToDevice(1);
    APP_ERROR ret = AbsDiff(src1, src2, dst, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest,
    Test_AbsDiff_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);
    src1.ToDevice(0);
    src2.ToDevice(0);
    dst.ToDevice(1);
    APP_ERROR ret = AbsDiff(src1, src2, dst, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_AbsDiff_Should_Return_Success_When_Dst_is_Float_Src_is_Float)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        Tensor dst;
        src1.ToDevice(0);
        src2.ToDevice(0);
        APP_ERROR ret = AbsDiff(src1, src2, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO],
                  g_absdiffFloatResult[SHAPE_DIMENSION_ZERO]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE],
                  g_absdiffFloatResult[SHAPE_DIMENSION_ONE]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO],
                  g_absdiffFloatResult[SHAPE_DIMENSION_TWO]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_THREE],
                  g_absdiffFloatResult[SHAPE_DIMENSION_THREE]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FOUR],
                  g_absdiffFloatResult[SHAPE_DIMENSION_FOUR]);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FIVE],
                  g_absdiffFloatResult[SHAPE_DIMENSION_FIVE]);
    }
}

TEST_F(PerElementOperationTest, Test_AbsDiff_Should_Return_Success_When_Dst_Is_Float16_Src_Is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        Tensor dst;
        src1.ToDevice(0);
        src2.ToDevice(0);
        Tensor src1Fp16;
        Tensor src2Fp16;
        ConvertTo(src1, src1Fp16, TensorDType::FLOAT16);
        ConvertTo(src2, src2Fp16, TensorDType::FLOAT16);
        APP_ERROR ret = AbsDiff(src1Fp16, src2Fp16, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        Tensor dst_fp32;
        ConvertTo(dst, dst_fp32, TensorDType::FLOAT32);
        dst_fp32.ToHost();
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_ZERO],
                  g_absdiffFloatResult[SHAPE_DIMENSION_ZERO]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_ONE],
                  g_absdiffFloatResult[SHAPE_DIMENSION_ONE]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_TWO],
                  g_absdiffFloatResult[SHAPE_DIMENSION_TWO]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_THREE],
                  g_absdiffFloatResult[SHAPE_DIMENSION_THREE]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_FOUR],
                  g_absdiffFloatResult[SHAPE_DIMENSION_FOUR]);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_FIVE],
                  g_absdiffFloatResult[SHAPE_DIMENSION_FIVE]);
    }
}

TEST_F(PerElementOperationTest, Test_AbsDiff_Should_Return_Success_When_Dst_Is_Uint8_Src_Is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src1(&g_data4, SHAPE3, TensorDType::UINT8);
        Tensor src2(&g_data5, SHAPE3, TensorDType::UINT8);
        Tensor dst;
        src1.ToDevice(0);
        src2.ToDevice(0);
        APP_ERROR ret = AbsDiff(src1, src2, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_ZERO],
                  g_absdiffU8Result[SHAPE_DIMENSION_ZERO]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_ONE],
                  g_absdiffU8Result[SHAPE_DIMENSION_ONE]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_TWO],
                  g_absdiffU8Result[SHAPE_DIMENSION_TWO]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_THREE],
                  g_absdiffU8Result[SHAPE_DIMENSION_THREE]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_FOUR],
                  g_absdiffU8Result[SHAPE_DIMENSION_FOUR]);
        EXPECT_EQ((static_cast<uint8_t  *>(dst.GetData()))[SHAPE_DIMENSION_FIVE],
                  g_absdiffU8Result[SHAPE_DIMENSION_FIVE]);
    }
}

TEST_F(PerElementOperationTest, Test_ScaleAdd_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = ScaleAdd(src1, 1.0, src2, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Max_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = Max(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Min_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = Min(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Reduce_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src = Tensor(src, roi_src);
        APP_ERROR ret = Reduce(src, dst, ReduceDim::REDUCE_HEIGHT, ReduceType::REDUCE_SUM);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Sort_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHW, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src = Tensor(src, roi_src);
        APP_ERROR ret = Sort(src, dst, 1, false);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_SortIdx_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHW, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src = Tensor(src, roi_src);
        APP_ERROR ret = SortIdx(src, dst, 1, false);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src1;
    Tensor src2;
    Tensor dst;
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor src2(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor dst;
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Src_Datatype_Is_Int)
{
    Tensor src1(SHAPE3, TensorDType::INT32, 0);
    Tensor src2(SHAPE3, TensorDType::INT32, 0);
    Tensor dst;
    Tensor::TensorMalloc(src1);
    Tensor::TensorMalloc(src2);
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src1(&g_data3, SHAPE5, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE5, TensorDType::FLOAT32);
    Tensor dst;
    src1.ToDevice(0);
    src2.ToDevice(0);
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Src_ShapeDim_Different)
{
    Tensor src1(shapeNHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Src_Shape_Different)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data3, SHAPE4, TensorDType::FLOAT32);
    Tensor dst;
    src1.ToDevice(0);
    src2.ToDevice(0);
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Src_Dtype_Different)
{
    Tensor src1(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor src2(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor dst;
    src1.ToDevice(0);
    src2.ToDevice(0);
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Src_Device_Different)
{
    Tensor src1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(shapeHWC, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(src2);
    Tensor dst;
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Srcs_Contain_Empty)
{
    Tensor src1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor dst;
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Dst_Shape_Different)
{
    Tensor src1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst(shapeHWC2, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Dst_ShapeDim_Different)
{
    Tensor src1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst(shapeNHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Dst_Datatype_Different)
{
    Tensor src1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst(shapeHWC, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Dst_Device_Different)
{
    Tensor src1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst(shapeHWC, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Stream_Device_Different)
{
    Tensor src1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src1);
    Tensor src2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src2);
    Tensor dst(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    MxBase::AscendStream stream = MxBase::AscendStream(1);
    stream.CreateAscendStream();
    APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Success_When_Dtype_Is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data4, SHAPE3, TensorDType::UINT8);
        src1.ToDevice(0);
        Tensor src2(&g_data5, SHAPE3, TensorDType::UINT8);
        src2.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_NE);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_ONE], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_TWO], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_THREE], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], COMPARE_SATISFIED);
    }
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Success_When_Dtype_Is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data4, SHAPE3, TensorDType::UINT8);
        src1.ToDevice(0);
        Tensor src2(&g_data5, SHAPE3, TensorDType::UINT8);
        src2.ToDevice(0);
        Tensor src1Fp16;
        Tensor src2Fp16;
        ConvertTo(src1, src1Fp16, TensorDType::FLOAT16);
        ConvertTo(src2, src2Fp16, TensorDType::FLOAT16);
        Tensor dst;
        APP_ERROR ret = Compare(src1Fp16, src2Fp16, dst, CmpOp::CMP_EQ);
        EXPECT_EQ(ret, APP_ERR_OK);
        Tensor dst_fp32;
        ConvertTo(dst, dst_fp32, TensorDType::FLOAT32);
        dst_fp32.ToHost();
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_ZERO], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_ONE], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_TWO], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_THREE], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_FOUR], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[SHAPE_DIMENSION_FIVE], COMPARE_NOT_SATISFIED);
    }
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Success_When_Dtype_Is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        src1.ToDevice(0);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        src2.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_THREE], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], COMPARE_NOT_SATISFIED);
    }
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Success_When_CmpOp_Is_EQ)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data4, SHAPE3, TensorDType::UINT8);
        src1.ToDevice(0);
        Tensor src2(&g_data5, SHAPE3, TensorDType::UINT8);
        src2.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_EQ);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_ONE], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_TWO], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_THREE], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], COMPARE_NOT_SATISFIED);
    }
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Success_When_CmpOp_Is_NE)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        src1.ToDevice(0);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        src2.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_NE);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_THREE], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], COMPARE_SATISFIED);
    }
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Success_When_CmpOp_Is_LT)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        src1.ToDevice(0);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        src2.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_LT);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_THREE], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], COMPARE_NOT_SATISFIED);
    }
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Success_When_CmpOp_Is_GT)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        src1.ToDevice(0);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        src2.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_GT);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_THREE], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], COMPARE_SATISFIED);
    }
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Success_When_CmpOp_Is_LE)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        src1.ToDevice(0);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        src2.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_LE);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_THREE], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], COMPARE_NOT_SATISFIED);
    }
}

TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Success_When_CmpOp_Is_GE)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(&g_data6, SHAPE3, TensorDType::FLOAT32);
        src1.ToDevice(0);
        Tensor src2(&g_data7, SHAPE3, TensorDType::FLOAT32);
        src2.ToDevice(0);
        Tensor dst;
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = Compare(src1, src2, dst, CmpOp::CMP_GE, stream);
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE], COMPARE_NOT_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_THREE], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FOUR], COMPARE_SATISFIED);
        EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_FIVE], COMPARE_SATISFIED);
    }
}


TEST_F(PerElementOperationTest, Test_Compare_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src1);
        Tensor src2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src2);
        Tensor dst(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        MxBase::Rect roi_src1(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src1 = Tensor(src1, roi_src1);
        MxBase::Rect roi_src2(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src2 = Tensor(src2, roi_src2);
        MxBase::Rect roi_dst(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        dst = Tensor(dst, roi_dst);
        APP_ERROR ret = Compare(src1, src2, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}


TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Fail_When_Src_Is_Inplace)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHW, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
        src = Tensor(src, roi_src);
        APP_ERROR ret = Rescale(src, dst, ALPHA, BETA);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Success_When_Dtype_Is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data4, SHAPE3, TensorDType::UINT8);
        src.ToDevice(0);
        Tensor dst;
        float scale = 1.1f;
        float bias = 9.85f;
        APP_ERROR ret = Rescale(src, dst, scale, bias);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        uint8_t rescaleResult[DATA_LEN] = {10, 12, 13, 13, 14, 15};
        for (size_t i = 0; i < DATA_LEN; i++) {
            EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[i], rescaleResult[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Success_When_Dtype_Is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data4, SHAPE3, TensorDType::UINT8);
        src.ToDevice(0);
        Tensor srcFp16;
        ConvertTo(src, srcFp16, TensorDType::FLOAT16);
        Tensor dst;
        float scale = 1.0f;
        float bias = 5.0f;
        APP_ERROR ret = Rescale(srcFp16, dst, scale, bias);
        EXPECT_EQ(ret, APP_ERR_OK);
        Tensor dst_fp32;
        ConvertTo(dst, dst_fp32, TensorDType::FLOAT32);
        dst_fp32.ToHost();
        float rescaleResult[DATA_LEN] = {6.f, 7.f, 8.f, 8.f, 9.f, 10.f};
        for (size_t i = 0; i < DATA_LEN; i++) {
            EXPECT_EQ((static_cast<float *>(dst_fp32.GetData()))[i], rescaleResult[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_Rescale_Should_Return_Success_When_Dtype_Is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data6, SHAPE3, TensorDType::FLOAT32);
        src.ToDevice(0);
        Tensor dst;
        float scale = 10.33f;
        float bias = 5.98f;
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = Rescale(src, dst, scale, bias, stream);
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        float rescaleResult[DATA_LEN] = {16.31, 26.64, 36.97, 36.97, 47.3, 57.63};
        for (size_t i = 0; i < DATA_LEN; i++) {
            EXPECT_EQ((static_cast<float *>(dst.GetData()))[i], rescaleResult[i]);
        }
    }
}

TEST_F(PerElementOperationTest, Test_Cilp_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src;
    Tensor dst;
    APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst;
    APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Src_Datatype_Is_Int)
{
    Tensor src(SHAPE3, TensorDType::INT32);
    Tensor::TensorMalloc(src);
    src.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src(&g_data3, SHAPE5, TensorDType::FLOAT32);
    src.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Src_Dst_shape_different)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE4, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);

    src.ToDevice(0);
    dst.ToDevice(0);

    APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Src_Datatype_Is_Different_With_Dst)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT16);
    Tensor::TensorMalloc(dst);

    src.ToDevice(0);
    dst.ToDevice(0);

    APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);

    src.ToDevice(0);

    APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);

    src.ToDevice(0);
    dst.ToDevice(1);
    APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst;

    src.ToDevice(1);
    APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_StreamId)
{
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    Tensor::TensorMalloc(dst);

    src.ToDevice(0);
    dst.ToDevice(1);

    APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Success_When_Dst_is_Float_Src_is_Float)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
        Tensor dst;

        src.ToDevice(0);
        APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Float_aclCreateScalar_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        aclScalar *min = nullptr;
        MOCKER_CPP(AclApi::aclCreateScalar).stubs().will(returnValue(min));
        Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
        Tensor dst;
        src.ToDevice(0);
        APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
        EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
    }
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Float_aclCreateScalar_Second_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        float minVal = 1;
        aclScalar *min = AclApi::aclCreateScalar(&minVal, static_cast<aclDataType>(1));
        aclScalar *max = nullptr;
        int mockTime = 2;
        MOCKER_CPP(AclApi::aclCreateScalar).times(mockTime).will(returnValue(min)).then(returnValue(max));
        Tensor src(&g_data3, SHAPE3, TensorDType::FLOAT32);
        Tensor dst;
        src.ToDevice(0);
        APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
        EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
    }
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Success_When_Dst_is_Float16_Src_is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src(SHAPE3, TensorDType::FLOAT16);
        Tensor dst(SHAPE3, TensorDType::FLOAT16);
        Tensor::TensorMalloc(src);
        Tensor::TensorMalloc(dst);

        src.ToDevice(0);
        dst.ToDevice(0);
        APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Success_When_Dst_is_uint8_Src_is_uint8)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst;

        src.ToDevice(0);
        APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Uint8_aclCreateScalar_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        aclScalar *min = nullptr;
        MOCKER_CPP(AclApi::aclCreateScalar).stubs().will(returnValue(min));
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst;
        src.ToDevice(0);
        APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
        EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
    }
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Uint8_Second_aclCreateScalar_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        int32_t minValInt32 = 1;
        aclScalar *min = AclApi::aclCreateScalar(&minValInt32, static_cast<aclDataType>(3));
        aclScalar *max = nullptr;
        int mockTime = 2;
        MOCKER_CPP(AclApi::aclCreateScalar).times(mockTime).will(returnValue(min)).then(returnValue(max));
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst;
        src.ToDevice(0);
        APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
        EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
    }
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Fail_When_Uint8_aclnnClampGetWorkspaceSize_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(AclApi::aclnnClampGetWorkspaceSize).stubs().will(returnValue(1));
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst;
        src.ToDevice(0);
        APP_ERROR ret = Clip(src, dst, MIN_VAL, MAX_VAL);
        EXPECT_EQ(ret, 1);
    }
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor tensor1;
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor tensor1(shapeHWC, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Src_Datatype_Is_Int32)
{
    Tensor tensor1(shapeHWC, TensorDType::INT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor tensor1(invalidShape, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Src_ShapeDim_Different)
{
    Tensor tensor1(shapeNHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Src_Shape_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC2, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Success_When_Src_Dtype_Different)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Src_Device_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Srcs_Contain_Empty)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Dst_Shape_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC2, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Dst_ShapeDim_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeNHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Dst_Datatype_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Dst_Device_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Fail_When_Stream_Device_Different)
{
    MxBase::AscendStream stream = MxBase::AscendStream(1);
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Success_When_Dst_is_Uint8_Src_is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        tensor1.SetTensorValue(VALID_MULTIPLY_OP_DST_VALUE1);
        Tensor tensor2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor2);
        tensor2.SetTensorValue(VALID_MULTIPLY_OP_SRC_VALUE2);
        Tensor tensor3(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
        EXPECT_EQ(ret, APP_ERR_OK);
        tensor3.ToHost();
        EXPECT_EQ(static_cast<uint8_t *>(tensor3.GetData())[0], VALID_MULTIPLY_OP_SRC_VALUE1);
    }
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Success_When_Dst_is_Float16_Src_is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(PerElementOperationTest, Test_Divide_Should_Return_Success_When_Dst_is_Float32_Src_is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor1);
        tensor1.SetTensorValue(static_cast<float>(VALID_MULTIPLY_OP_DST_VALUE1), false);
        Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor2);
        tensor2.SetTensorValue(static_cast<float>(VALID_MULTIPLY_OP_SRC_VALUE2), false);
        Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
        EXPECT_EQ(ret, APP_ERR_OK);
        tensor3.ToHost();
        EXPECT_EQ(static_cast<float *>(tensor3.GetData())[0], static_cast<float>(VALID_MULTIPLY_OP_SRC_VALUE1));
    }
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor tensor1;
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor tensor1(shapeHWC, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Fail_When_Src_Datatype_Is_Int32)
{
    Tensor tensor1(shapeHWC, TensorDType::INT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor tensor1(invalidShape, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Fail_When_Src_ShapeDim_Different)
{
    Tensor tensor1(shapeNHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Fail_When_Src_Shape_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC2, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Success_When_Src_Dtype_Different)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Fail_When_Src_Device_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Fail_When_Srcs_Contain_Empty)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor tensor3;
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Fail_When_Dst_Shape_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC2, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Fail_When_Dst_ShapeDim_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeNHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Fail_When_Dst_Datatype_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Fail_When_Dst_Device_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Fail_When_Stream_Device_Different)
{
    MxBase::AscendStream stream = MxBase::AscendStream(1);
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Success_When_Dst_is_Uint8_Src_is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        tensor1.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE2);
        Tensor tensor2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor2);
        tensor2.SetTensorValue(VALID_BITWISE_OP_SRC_VALUE1);
        Tensor tensor3(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
        EXPECT_EQ(ret, APP_ERR_OK);
        tensor3.ToHost();
        EXPECT_EQ(static_cast<uint8_t *>(tensor3.GetData())[0], VALID_MULTIPLY_OP_SRC_VALUE2);
    }
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Success_When_Dst_is_Float16_Src_is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Divide(tensor1, tensor2, tensor3);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(PerElementOperationTest, Test_Divide_with_Scale_Should_Return_Success_When_Dst_is_Float32_Src_is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor1);
        tensor1.SetTensorValue(static_cast<float>(VALID_MULTIPLY_OP_DST_VALUE1), false);
        Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor2);
        tensor2.SetTensorValue(static_cast<float>(VALID_MULTIPLY_OP_SRC_VALUE2), false);
        Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Divide(tensor1, tensor2, tensor3, MUL_SCALE);
        EXPECT_EQ(ret, APP_ERR_OK);
        tensor3.ToHost();
        EXPECT_EQ(static_cast<float *>(tensor3.GetData())[0], static_cast<float>(VALID_MULTIPLY_OP_SRC_VALUE2));
    }
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor tensor1;
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor tensor1(shapeHWC, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Fail_When_Src_Datatype_Is_Int32)
{
    Tensor tensor1(shapeHWC, TensorDType::INT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor tensor1(invalidShape, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Fail_When_Src_ShapeDim_Different)
{
    Tensor tensor1(shapeNHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3;
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Fail_When_Src_Shape_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC2, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3;
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Success_When_Src_Dtype_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    if (DeviceManager::IsAscend310P()) {
        APP_ERROR ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Fail_When_Srcs_Contain_Empty)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor tensor3;
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Fail_When_Dst_Shape_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC2, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Fail_When_Dst_ShapeDim_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeNHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Fail_When_Dst_Datatype_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Fail_When_Dst_Device_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Fail_When_Stream_Device_Different)
{
    MxBase::AscendStream stream = MxBase::AscendStream(1);
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Multiply(tensor1, tensor2, tensor3, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}
TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Success_When_Dst_is_Uint8_Src_is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        tensor1.SetTensorValue(VALID_MULTIPLY_OP_SRC_VALUE1);
        Tensor tensor2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor2);
        tensor2.SetTensorValue(VALID_MULTIPLY_OP_SRC_VALUE2);
        Tensor tensor3(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
        EXPECT_EQ(ret, APP_ERR_OK);
        tensor3.ToHost();
        EXPECT_EQ(static_cast<uint8_t *>(tensor3.GetData())[0], VALID_MULTIPLY_OP_DST_VALUE1);

        tensor3 = Tensor();
        ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
        EXPECT_EQ(ret, APP_ERR_OK);
        tensor3.ToHost();
        EXPECT_EQ(static_cast<uint8_t *>(tensor3.GetData())[0], VALID_MULTIPLY_OP_DST_VALUE2);
    }
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Success_When_Dst_is_Float16_Src_is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
        ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(PerElementOperationTest, Test_Multiply_Should_Return_Success_When_Dst_is_Float32_Src_is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor1);
        tensor1.SetTensorValue(static_cast<float>(VALID_MULTIPLY_OP_SRC_VALUE1), false);
        Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor2);
        tensor2.SetTensorValue(static_cast<float>(VALID_MULTIPLY_OP_SRC_VALUE2), false);
        Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Multiply(tensor1, tensor2, tensor3);
        EXPECT_EQ(ret, APP_ERR_OK);
        tensor3.ToHost();
        EXPECT_EQ(static_cast<float *>(tensor3.GetData())[0], static_cast<float>(VALID_MULTIPLY_OP_DST_VALUE1));

        tensor3 = Tensor();
        ret = Multiply(tensor1, tensor2, tensor3, MUL_SCALE);
        EXPECT_EQ(ret, APP_ERR_OK);
        tensor3.ToHost();
        EXPECT_EQ(static_cast<float *>(tensor3.GetData())[0], static_cast<float>(VALID_MULTIPLY_OP_DST_VALUE2));
    }
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor tensor1;
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor tensor1(shapeHWC, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Src_Datatype_Is_Int32)
{
    Tensor tensor1(shapeHWC, TensorDType::INT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor tensor1(invalidShape, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Src_ShapeDim_Different)
{
    Tensor tensor1(shapeNHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3;
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Src_Shape_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC2, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3;
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Src_Dtype_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3;
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Src_Device_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3;
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Srcs_Contain_Empty)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor tensor3;
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Dst_Shape_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC2, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Dst_ShapeDim_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeNHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Dst_Datatype_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Dst_Device_Different)
{
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_Stream_Device_Different)
{
    MxBase::AscendStream stream = MxBase::AscendStream(1);
    Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor3);
    APP_ERROR ret = Subtract(tensor1, tensor2, tensor3, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Success_When_Dst_is_Uint8_Src_is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        tensor1.SetTensorValue(VALID_SUBTRACT_OP_SRC_VALUE1);
        Tensor tensor2(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor2);
        tensor2.SetTensorValue(VALID_SUBTRACT_OP_SRC_VALUE2);
        Tensor tensor3(shapeHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
        EXPECT_EQ(ret, APP_ERR_OK);
        tensor3.ToHost();
        EXPECT_EQ(reinterpret_cast<uint8_t *>(tensor3.GetData())[0], VALID_SUBTRACT_OP_DST_VALUE);
    }
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Success_When_Dst_is_Float16_Src_is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_aclCreateScalar_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        aclScalar *alpha = nullptr;
        MOCKER_CPP(AclApi::aclCreateScalar).stubs().will(returnValue(alpha));
        Tensor tensor1(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
        EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
    }
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Fail_When_aclnnSubGetWorkspaceSize_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(AclApi::aclnnSubGetWorkspaceSize).stubs().will(returnValue(1));
        Tensor tensor1(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(shapeHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
        EXPECT_EQ(ret, 1);
    }
}

TEST_F(PerElementOperationTest, Test_Subtract_Should_Return_Success_When_Dst_is_Float32_Src_is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor1);
        tensor1.SetTensorValue(static_cast<float>(VALID_SUBTRACT_OP_SRC_VALUE1), false);
        Tensor tensor2(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor2);
        tensor2.SetTensorValue(static_cast<float>(VALID_SUBTRACT_OP_SRC_VALUE2), false);
        Tensor tensor3(shapeHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor3);
        APP_ERROR ret = Subtract(tensor1, tensor2, tensor3);
        EXPECT_EQ(ret, APP_ERR_OK);
        tensor3.ToHost();
        EXPECT_EQ(reinterpret_cast<float *>(tensor3.GetData())[0], static_cast<float>(VALID_SUBTRACT_OP_DST_VALUE));
    }
}

TEST_F(PerElementOperationTest, Test_Exp_Should_Return_Fail_With_Invalid_Device)
{
    MOCKER_CPP(&DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    Tensor src(SHAPE3, TensorDType::UINT8);
    Tensor dst(SHAPE3, TensorDType::FLOAT32);
    APP_ERROR ret = Exp(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Log_Exp_Should_Return_Fail_With_Invalid_Stream)
{
    MxBase::AscendStream stream = MxBase::AscendStream(1);
    stream.CreateAscendStream();
    Tensor src(SHAPE3, TensorDType::UINT8, 0);
    Tensor dst(SHAPE3, TensorDType::FLOAT32, 0);
    src.Malloc();
    APP_ERROR ret = Exp(src, dst, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    ret = Log(src, dst, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Log_Exp_Should_Return_Fail_When_Dst_Shape_Is_Invalid)
{
    Tensor src(SHAPE3, TensorDType::FLOAT32, 0);
    src.Malloc();
    Tensor dst(SHAPE4, TensorDType::UINT8);
    dst.Malloc();
    APP_ERROR ret = Log(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    ret = Exp(src, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}


TEST_F(PerElementOperationTest, Test_ConverTo_Should_Return_Fail_With_Invalid_Device)
{
    MOCKER_CPP(&DeviceManager::IsAscend310B).stubs().will(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    Tensor src;
    Tensor dst;
    APP_ERROR ret = ConvertTo(src, dst, TensorDType::FLOAT16);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_ConverTo_Should_Return_Fail_When_Src_Is_Invalid)
{
    Tensor src({}, TensorDType::FLOAT16, 0);
    Tensor dst({}, TensorDType::FLOAT16, 0);
    APP_ERROR ret = ConvertTo(src, dst, TensorDType::FLOAT16);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    src.Malloc();
    ret = ConvertTo(src, dst, TensorDType::FLOAT16);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sort_SortIdx_Should_Return_Fail_When_Device_Is_Invalid)
{
    MOCKER_CPP(&DeviceManager::IsAscend310B).stubs().will(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    Tensor src(shapeHW, TensorDType::FLOAT16, 0);
    src.Malloc();
    Tensor dst;
    APP_ERROR ret = Sort(src, dst, 1, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = SortIdx(src, dst, 1, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, Test_Sort_Should_Return_Fail_When_Src_Is_Invalid)
{
    Tensor src({}, TensorDType::FLOAT16);
    Tensor dst;
    APP_ERROR ret = Sort(src, dst, 2, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Sort(src, dst, 1, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    src.Malloc();
    ret = Sort(src, dst, 1, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    src.ToDevice(0);
    ret = Sort(src, dst, 1, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor src2(shapeHWC, TensorDType::FLOAT16, 0);
    src2.Malloc();
    ret = Sort(src2, dst, 1, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor src3(shapeHW, TensorDType::INT8, 0);
    src3.Malloc();
    ret = Sort(src3, dst, 1, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(PerElementOperationTest, DISABLED_Test_Sort_SortIdx_Should_Return_Success_With_Axis_0)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHW, TensorDType::FLOAT16, 0);
        src.Malloc();
        Tensor dst;
        APP_ERROR ret = Sort(src, dst, 0, false);
        EXPECT_EQ(ret, APP_ERR_OK);

        Tensor dstIdx;
        ret = SortIdx(src, dstIdx, 0, false);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(PerElementOperationTest, Test_Sort_Should_Return_Fail_When_Transpose_Fail)
{
    MOCKER_CPP(&Transpose).stubs().will(returnValue(-1));
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHW, TensorDType::FLOAT16, 0);
        src.Malloc();
        Tensor dst;
        APP_ERROR ret = Sort(src, dst, 0, false);
        EXPECT_EQ(ret, -1);
    }
}

TEST_F(PerElementOperationTest, Test_Sort_Should_Return_Fail_When_ConvertTo_Fail)
{
    MOCKER_CPP(&ConvertTo).stubs().will(returnValue(-1));
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHW, TensorDType::FLOAT16, 0);
        src.Malloc();
        Tensor dst(shapeHW, TensorDType::FLOAT16);
        dst.Malloc();
        APP_ERROR ret = Sort(src, dst, 0, false);
        EXPECT_EQ(ret, -1);
    }
}

TEST_F(PerElementOperationTest, DISABLED_Test_Sort_Should_Return_Fail_When_Dst_Device_Is_Host_And_Axis_0)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHW, TensorDType::FLOAT16, 0);
        src.Malloc();
        Tensor dst(shapeHW, TensorDType::FLOAT16);
        dst.Malloc();
        APP_ERROR ret = Sort(src, dst, 0, false);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Sort_SortIdx_Should_Return_Fail_When_Dst_Device_Is_Host_And_Axis_1)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(shapeHW, TensorDType::FLOAT16, 0);
        src.Malloc();
        Tensor dst(shapeHW, TensorDType::FLOAT16);
        dst.Malloc();
        APP_ERROR ret = Sort(src, dst, 1, false);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

        ret = SortIdx(src, dst, 1, false);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(PerElementOperationTest, Test_Clip_Should_Return_Success_With_Device_310B)
{
    MOCKER_CPP(&DeviceManager::IsAscend310B).stubs().will(returnValue(true));
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    APP_ERROR ret = APP_ERR_OK;
    std::vector<TensorDType> dtypes = {TensorDType::UINT8, TensorDType::FLOAT32, TensorDType::FLOAT16};
    for (auto dtype: dtypes) {
        Tensor src(&g_data1, SHAPE3, dtype);
        src.ToDevice(0);
        Tensor dst;
        ret = Clip(src, dst, MIN_VAL, MAX_VAL, stream0);
        stream0.Synchronize();
        EXPECT_EQ(ret, APP_ERR_OK);
    }
    stream0.DestroyAscendStream();
}

}

int main(int argc, char *argv[])
{
    MxInit();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    MxDeInit();
    return ret;
}
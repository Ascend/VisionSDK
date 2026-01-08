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
#include "ResourceManager/HAL/AclApi.h"
#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/CoreOperationsOnTensors.h"
#include "MxBase/MxBase.h"
#include "MxBase/DeviceManager/DeviceManager.h"

namespace {
using namespace MxBase;
const std::vector<uint32_t> SHAPE1 = {0x2, 0x2, 0x2};
const size_t DATA_LEN = 6;
const uint32_t DIM = 0x1;
const std::vector<uint32_t> SHAPE3 = {0x1, 0x2, 0x3};
const std::vector<uint32_t> SHAPE4 = {0x1, 0x2, 0x3, 0x1};
const std::vector<uint32_t> SHAPE5 = {0x1, 0x2, 0x3, 0x1, 0x1};
const std::vector<uint32_t> SHAPE_SINGLE = {0x1, 0x2, 0x1};
const int IMAGE_WIDTH = 480;
const int IMAGE_HEIGHT = 640;
const int MAX_IMAGE_HEIGHT = 4096;
const int CHANNEL = 3;
const int GRAY = 1;
const int INVALID_CHANNEL = 5;
const std::vector<uint32_t> SHAPEHW = {IMAGE_HEIGHT, IMAGE_WIDTH};
const std::vector<uint32_t> SHAPEHW1 = {IMAGE_HEIGHT, IMAGE_WIDTH, GRAY};
const std::vector<uint32_t> SHAPEHWC = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
const std::vector<uint32_t> INVALID_SHAPEHWC = {IMAGE_HEIGHT, IMAGE_WIDTH, INVALID_CHANNEL};
const std::vector<uint32_t> SHAPEHWC_TRANSPOSE = {IMAGE_WIDTH, IMAGE_HEIGHT, CHANNEL};
const std::vector<uint32_t> SHAPEHWC2 = {IMAGE_WIDTH, IMAGE_WIDTH, CHANNEL};
const std::vector<uint32_t> SHAPENHWC = {GRAY, IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
const std::vector<uint32_t> SHAPENHW1 = {GRAY, IMAGE_HEIGHT, IMAGE_WIDTH, GRAY};
const std::vector<uint32_t> INVALIDSHAPE = {GRAY, IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL, CHANNEL};
const std::vector<uint32_t> INVALID_SHAPEHW = {MAX_IMAGE_HEIGHT + 1, IMAGE_WIDTH};
const std::vector<int> TRANS_AXIS = {1, 0, 2};
uint8_t g_data1[DATA_LEN] = {0};
uint8_t g_data2[DATA_LEN] = {0};
uint8_t g_data3[DATA_LEN] = {0};
const int MAX_ERODE_KERNEL = 9;
const int MIN_ERODE_KERNEL = 3;
const int MAX_ERODE_ITER = 100;
const int MAX_U8 = 255;

class CoreOperationsOnTensorsTest : public testing::Test {
public:
    void TearDown() override
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

template<typename T>
bool EXPECT_EQ_VECTOR(const std::vector<T>& t1, const std::vector<T>& t2)
{
    return t1 == t2;
}

TEST_F(CoreOperationsOnTensorsTest, TestBatchSplit)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::Tensor src(SHAPE1, MxBase::TensorDType::UINT8);
        MxBase::Tensor::TensorMalloc(src);
        src.ToDevice(0);
        std::vector<Tensor> dst;
        APP_ERROR ret = BatchSplit(src, dst, true);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(CoreOperationsOnTensorsTest, TestHstack)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(&g_data2, SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor3);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    std::vector<Tensor> srcVec = {tensor1, tensor2};
    APP_ERROR ret = Hstack(srcVec, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, DISABLED_TestVstack)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2(&g_data2, SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE4, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor3);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    std::vector<Tensor> srcVec = {tensor1, tensor2};
    APP_ERROR ret = Vstack(srcVec, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor tensor4;
    ret = Vstack(srcVec, tensor4);
    if (DeviceManager::IsAscend310P()) {
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor tensor1;
    Tensor tensor2;
    APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor tensor1(SHAPEHWC, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_When_Src_Datatype_Is_Int32)
{
    Tensor tensor1(SHAPEHWC, TensorDType::INT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor tensor1(INVALIDSHAPE, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_When_Dst_Shape_Different)
{
    Tensor tensor1(SHAPEHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHWC2, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_When_Dst_ShapeDim_Different)
{
    Tensor tensor1(SHAPEHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPENHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_When_Dst_Datatype_Different)
{
    Tensor tensor1(SHAPEHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHWC, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(tensor2);
    APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_When_Dst_Shape_From_Axis)
{
    Tensor tensor1(SHAPEHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_When_Dst_Device_Different)
{
    Tensor tensor1(SHAPEHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHWC, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(tensor2);
    APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_When_Stream_Device_Different)
{
    Tensor tensor1(SHAPEHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    MxBase::AscendStream stream = MxBase::AscendStream(1);
    stream.CreateAscendStream();
    APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS, stream);
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Success_When_Dtype_is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPEHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(tensor2.GetShape(), SHAPEHWC_TRANSPOSE), true);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Success_When_Dtype_is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPEHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(tensor2.GetShape(), SHAPEHWC_TRANSPOSE), true);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Success_When_Dtype_is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPEHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS, stream);
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(tensor2.GetShape(), SHAPEHWC_TRANSPOSE), true);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_When_aclCreateIntArray_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        aclIntArray *axisArray = nullptr;
        MOCKER_CPP(AclApi::aclCreateIntArray).stubs().will(returnValue(axisArray));
        Tensor tensor1(SHAPEHWC, TensorDType::FLOAT32, 0);
        tensor1.Malloc();
        Tensor tensor2;
        APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
        EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_When_aclnnPermuteGetWorkspaceSize_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(AclApi::aclnnPermuteGetWorkspaceSize).stubs().will(returnValue(1));
        Tensor tensor1(SHAPEHWC, TensorDType::FLOAT32, 0);
        tensor1.Malloc();
        Tensor tensor2;
        APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
        EXPECT_EQ(ret, 1);
    }
}

TEST_F(CoreOperationsOnTensorsTest, TestSplit)
{
    Tensor tensor1(&g_data1, SHAPE5, TensorDType::UINT8);
    Tensor tensor2(SHAPE3, TensorDType::UINT8);
    Tensor tensor3(SHAPE3, TensorDType::UINT8);
    Tensor tensor4(SHAPE3, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor2);
    Tensor::TensorMalloc(tensor3);
    Tensor::TensorMalloc(tensor4);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    tensor4.ToDevice(0);
    std::vector<Tensor> tv = {tensor2, tensor3, tensor4};
    APP_ERROR ret = Split(tensor1, tv);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, TestMerge)
{
    Tensor tensor2(&g_data1, SHAPE_SINGLE, TensorDType::UINT8);
    Tensor tensor3(&g_data2, SHAPE_SINGLE, TensorDType::UINT8);
    Tensor tensor4(&g_data3, SHAPE_SINGLE, TensorDType::UINT8);
    Tensor tensor1(SHAPE5, TensorDType::UINT8);
    Tensor::TensorMalloc(tensor1);
    tensor1.ToDevice(0);
    tensor2.ToDevice(0);
    tensor3.ToDevice(0);
    tensor4.ToDevice(0);
    std::vector<Tensor> tv = {tensor2, tensor3, tensor4};
    APP_ERROR ret = Merge(tv, tensor1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_Srcs_Contains_Empty)
{
    Tensor tensor1;
    Tensor tensor2;
    Tensor tensor3;
    std::vector<Tensor> tv = {tensor1, tensor2};
    APP_ERROR ret = Merge(tv, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_Srcs_Size_Is_0)
{
    Tensor tensor1;
    std::vector<Tensor> tv = {};
    APP_ERROR ret = Merge(tv, tensor1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor tensor1(SHAPEHW1, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    std::vector<Tensor> tv = {tensor1, tensor2};
    APP_ERROR ret = Merge(tv, tensor3);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_Src_Datatype_Is_Int32)
{
    Tensor tensor1(SHAPEHW1, TensorDType::INT32, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHW1, TensorDType::INT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(SHAPEHW1, TensorDType::INT32, 0);
    Tensor::TensorMalloc(tensor3);
    Tensor dst;
    std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
    APP_ERROR ret = Merge(tv, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_Src_Shape_Not_Same)
{
    Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(SHAPEHW, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor3);
    Tensor dst;
    std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
    APP_ERROR ret = Merge(tv, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_Src_Shape_Size_Is_2)
{
    Tensor tensor1(SHAPEHW, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHW, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(SHAPEHW, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor3);
    Tensor dst;
    std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
    APP_ERROR ret = Merge(tv, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_Src_Channel_Sum_Is_5)
{
    Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(SHAPEHWC, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor3);
    Tensor dst;
    std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
    APP_ERROR ret = Merge(tv, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_Src_Device_Different)
{
    Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHW1, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(SHAPEHW1, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(tensor3);
    Tensor dst;
    std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
    APP_ERROR ret = Merge(tv, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_Src_Device_Different_With_Dst)
{
    Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor3);
    Tensor dst(SHAPEHWC, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dst);
    std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
    APP_ERROR ret = Merge(tv, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_Src_Device_Diff_With_Stream)
{
    Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor3);
    Tensor dst;
    MxBase::AscendStream stream = MxBase::AscendStream(1);
    std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
    APP_ERROR ret = Merge(tv, dst, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_Srcs_With_Diff_Datatype)
{
    Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHW1, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor3);
    Tensor dst;
    std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
    APP_ERROR ret = Merge(tv, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_Src_Shape_size_Is_5)
{
    Tensor tensor1(INVALIDSHAPE, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2;
    Tensor tensor3;
    std::vector<Tensor> tv = {tensor1, tensor2};
    APP_ERROR ret = Merge(tv, tensor2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_Dst_Channel_Dim_Not_Right)
{
    Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHWC, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor dst(SHAPEHWC, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    std::vector<Tensor> tv = {tensor1, tensor2};
    APP_ERROR ret = Merge(tv, dst);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Success_When_Src_Is_Uint8_And_Dst_Is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPEHW1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(SHAPEHW1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor3);
        Tensor dst(SHAPEHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        std::vector <Tensor> tv = {tensor1, tensor2, tensor3};
        APP_ERROR ret = Merge(tv, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_aclCreateTensorList_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        aclTensorList* srcList = nullptr;
        MOCKER_CPP(AclApi::aclCreateTensorList).stubs().will(returnValue(srcList));
        Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPEHW1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(SHAPEHW1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor3);
        Tensor dst(SHAPEHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        std::vector <Tensor> tv = {tensor1, tensor2, tensor3};
        APP_ERROR ret = Merge(tv, dst);
        EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Fail_When_aclnnCatGetWorkspaceSize_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(AclApi::aclnnCatGetWorkspaceSize).stubs().will(returnValue(1));
        MOCKER_CPP(AclApi::aclDestroyTensorList).stubs().will(returnValue(1));
        Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPEHW1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(SHAPEHW1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor3);
        Tensor dst(SHAPEHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        std::vector <Tensor> tv = {tensor1, tensor2, tensor3};
        APP_ERROR ret = Merge(tv, dst);
        EXPECT_EQ(ret, 1);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Success_When_Src_Is_Float16_And_Dst_Is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPEHW1, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPEHW1, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(SHAPEHW1, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor3);
        Tensor dst(SHAPEHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(dst);
        std::vector <Tensor> tv = {tensor1, tensor2, tensor3};
        APP_ERROR ret = Merge(tv, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Success_When_Src_Is_Float32_And_Dst_Is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPENHW1, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPENHW1, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(SHAPENHW1, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor3);
        Tensor dst(SHAPENHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(dst);
        std::vector <Tensor> tv = {tensor1, tensor2, tensor3};
        APP_ERROR ret = Merge(tv, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Merge_Should_Return_Success_When_Src_Is_Float32_And_Dst_Is_Empty)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPENHW1, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPENHW1, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(SHAPENHW1, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor3);
        Tensor dst;
        std::vector <Tensor> tv = {tensor1, tensor2, tensor3};
        APP_ERROR ret = Merge(tv, dst);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src;
    std::vector<Tensor> tv = {};
    APP_ERROR ret = Split(src, tv);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_Src_Shape_Size_Is_2)
{
    Tensor src(SHAPEHW, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> tv = {};
    APP_ERROR ret = Split(src, tv);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_Src_Shape_Size_Is_5)
{
    Tensor src(INVALIDSHAPE, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> tv = {};
    APP_ERROR ret = Split(src, tv);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src(SHAPEHW1, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> tv = {};
    APP_ERROR ret = Split(src, tv);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_Src_Datatype_Is_Int32)
{
    Tensor src(SHAPEHW1, TensorDType::INT32, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> tv = {};
    APP_ERROR ret = Split(src, tv);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_Src_Channel_Is_1)
{
    Tensor src(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> tv = {};
    APP_ERROR ret = Split(src, tv);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_Src_Channel_Is_5)
{
    Tensor src({IMAGE_HEIGHT, IMAGE_WIDTH, 5}, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> tv = {};
    APP_ERROR ret = Split(src, tv);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_Dst_Device_Different)
{
    Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHW1, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(SHAPEHW1, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(tensor3);
    Tensor src(SHAPEHWC, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
    APP_ERROR ret = Split(src, tv);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_Src_Device_Different_With_Dst)
{
    Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor3);
    Tensor src(SHAPEHWC, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
    APP_ERROR ret = Split(src, tv);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_Src_Device_Diff_With_Stream)
{
    Tensor src(SHAPENHWC, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    MxBase::AscendStream stream = MxBase::AscendStream(1);
    std::vector<Tensor> tv = {};
    APP_ERROR ret = Split(src, tv, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_Dst_With_Diff_Datatype)
{
    Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHW1, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor3);
    Tensor src(SHAPEHWC, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
    APP_ERROR ret = Split(src, tv);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_Dst_Shape_Size_Not_Same)
{
    Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHW, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(SHAPEHWC, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor3);
    Tensor src(SHAPEHWC, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
    APP_ERROR ret = Split(src, tv);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_Dst_Shape_Not_Right)
{
    Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor1);
    Tensor tensor2(SHAPEHW1, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor2);
    Tensor tensor3(SHAPEHWC, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(tensor3);
    Tensor src(SHAPEHWC, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
    APP_ERROR ret = Split(src, tv);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Success_When_Src_Is_Uint8_And_Dst_Is_Uint8)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPEHW1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPEHW1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(SHAPEHW1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(tensor3);
        Tensor src(SHAPEHWC, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
        APP_ERROR ret = Split(src, tv);
        EXPECT_EQ(ret, APP_ERR_OK);
        for (uint32_t i = 0; i < tv.size(); i++) {
            EXPECT_EQ(tv[i].GetShape(), SHAPEHW1);
        }
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Success_When_Src_Is_Float16_And_Dst_Is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPENHW1, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPENHW1, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(SHAPENHW1, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor3);
        Tensor src(SHAPENHWC, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(src);
        std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
        APP_ERROR ret = Split(src, tv);
        EXPECT_EQ(ret, APP_ERR_OK);
        for (uint32_t i = 0; i < tv.size(); i++) {
            EXPECT_EQ(tv[i].GetShape(), SHAPENHW1);
        }
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Success_When_Src_Is_Float32_And_Dst_Is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPENHW1, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2(SHAPENHW1, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor2);
        Tensor tensor3(SHAPENHW1, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor3);
        Tensor src(SHAPENHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(src);
        std::vector<Tensor> tv = {tensor1, tensor2, tensor3};
        APP_ERROR ret = Split(src, tv);
        EXPECT_EQ(ret, APP_ERR_OK);
        for (uint32_t i = 0; i < tv.size(); i++) {
            EXPECT_EQ(tv[i].GetShape(), SHAPENHW1);
        }
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Success_When_Src_Is_Float32_And_Dst_Is_Empty)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(SHAPEHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(src);
        std::vector<Tensor> tv = {};
        APP_ERROR ret = Split(src, tv);
        EXPECT_EQ(ret, APP_ERR_OK);
        for (uint32_t i = 0; i < tv.size(); i++) {
            EXPECT_EQ(tv[i].GetShape(), SHAPEHW1);
        }
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_aclCreateTensorList_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        aclTensorList* outList = nullptr;
        MOCKER_CPP(AclApi::aclCreateTensorList).stubs().will(returnValue(outList));
        Tensor src(SHAPEHWC, TensorDType::FLOAT32, 0);
        src.Malloc();
        std::vector<Tensor> tv = {};
        APP_ERROR ret = Split(src, tv);
        EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Fail_When_aclnnSplitTensorGetWorkspaceSize_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(AclApi::aclnnSplitTensorGetWorkspaceSize).stubs().will(returnValue(1));
        MOCKER_CPP(AclApi::aclDestroyTensorList).stubs().will(returnValue(1));
        Tensor src(SHAPEHWC, TensorDType::FLOAT32, 0);
        src.Malloc();
        std::vector<Tensor> tv = {};
        APP_ERROR ret = Split(src, tv);
        EXPECT_EQ(ret, 1);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Split_Should_Return_Success_When_Src_Is_Float32_With_Stream)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(SHAPEHWC, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(src);
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        std::vector<Tensor> tv = {};
        APP_ERROR ret = Split(src, tv, stream0);
        stream0.Synchronize();
        EXPECT_EQ(ret, APP_ERR_OK);
        for (uint32_t i = 0; i < tv.size(); i++) {
            EXPECT_EQ(tv[i].GetShape(), SHAPEHW1);
        }
        stream0.DestroyAscendStream();
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src;
    Tensor dst;
    BlurConfig blur;
    APP_ERROR ret = Erode(src, dst, blur);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src(SHAPEHWC, TensorDType::FLOAT32);
    Tensor::TensorMalloc(src);
    Tensor dst;
    BlurConfig blur;
    APP_ERROR ret = Erode(src, dst, blur);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_Src_Is_NHWC)
{
    Tensor src(SHAPENHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    BlurConfig blur;
    APP_ERROR ret = Erode(src, dst, blur);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_Src_Channel_Is_Five)
{
    Tensor src(INVALID_SHAPEHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    BlurConfig blur;
    APP_ERROR ret = Erode(src, dst, blur);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_Src_Shape_Different_With_Dst)
{
    Tensor src(SHAPEHW1, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPEHWC, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    BlurConfig blur;
    APP_ERROR ret = Erode(src, dst, blur);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_Kernel_Size_Large_Than_MaxKernel)
{
    Tensor src(SHAPEHW1, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    BlurConfig blur;
    blur.kernelSize = Size(MAX_ERODE_KERNEL + 1, MAX_ERODE_KERNEL + 1);
    APP_ERROR ret = Erode(src, dst, blur);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_Kernel_Size_Smaller_Than_MinKernel)
{
    Tensor src(SHAPEHW1, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    BlurConfig blur;
    blur.kernelSize = Size(MIN_ERODE_KERNEL - 1, MIN_ERODE_KERNEL - 1);
    APP_ERROR ret = Erode(src, dst, blur);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_borderType_Is_Constant)
{
    Tensor src(SHAPEHW1, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    BlurConfig blur;
    blur.borderType = BorderType::BORDER_CONSTANT;
    APP_ERROR ret = Erode(src, dst, blur);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_anchor_Is_Invalid)
{
    Tensor src(SHAPEHW1, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    BlurConfig blur;
    blur.anchor = std::make_pair(0, 0);
    APP_ERROR ret = Erode(src, dst, blur);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_MorphType_Is_MORPH_CROSS)
{
    Tensor src(SHAPEHW1, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    BlurConfig blur;
    blur.morphShape = MorphShape::MORPH_CROSS;
    APP_ERROR ret = Erode(src, dst, blur);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_Src_Device_Is_Different_With_Dst)
{
    Tensor src(SHAPEHW1, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPEHW1, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(dst);
    BlurConfig blur;
    APP_ERROR ret = Erode(src, dst, blur);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_Src_DataType_Is_Different_With_Dst)
{
    Tensor src(SHAPEHW1, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPEHW1, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(dst);
    BlurConfig blur;
    APP_ERROR ret = Erode(src, dst, blur);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_Src_Device_Is_Different_With_Stream)
{
    Tensor src(SHAPEHW1, TensorDType::FLOAT32, 1);
    Tensor::TensorMalloc(src);
    Tensor dst;
    BlurConfig blur;
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    APP_ERROR ret = Erode(src, dst, blur, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_Src_Is_Int32)
{
    Tensor src(SHAPEHW1, TensorDType::INT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    BlurConfig blur;
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    APP_ERROR ret = Erode(src, dst, blur, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_Src_Height_Exceed_Max)
{
    Tensor src(INVALID_SHAPEHW, TensorDType::INT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    BlurConfig blur;
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    APP_ERROR ret = Erode(src, dst, blur, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Fail_When_Src_Height_Exceed_Min)
{
    Tensor src(SHAPE3, TensorDType::INT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    BlurConfig blur;
    AscendStream stream0 = AscendStream(0);
    stream0.CreateAscendStream();
    APP_ERROR ret = Erode(src, dst, blur, stream0);
    stream0.Synchronize();
    stream0.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Success_When_Kernel_Nine_Src_U8)
{
    if (DeviceManager::IsAscend310P()) {
        uint8_t erodeData[IMAGE_HEIGHT][IMAGE_WIDTH];
        for (size_t i = 0; i < IMAGE_HEIGHT; i++)
            for (size_t j = 0; j < IMAGE_WIDTH; j++) {
                erodeData[i][j] = i % MAX_U8;
        }
        Tensor src(&erodeData, SHAPEHW, MxBase::TensorDType::UINT8);
        src.ToDevice(0);
        Tensor dst;
        BlurConfig blur;
        blur.kernelSize = Size(MAX_ERODE_KERNEL, MAX_ERODE_KERNEL);
        APP_ERROR ret = Erode(src, dst, blur);
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(static_cast<uint8_t *>(dst.GetData())[IMAGE_WIDTH], 0);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Success_When_Src_Float)
{
    if (DeviceManager::IsAscend310P()) {
        float erodeData[IMAGE_HEIGHT][IMAGE_WIDTH];
        for (size_t i = 0; i < IMAGE_HEIGHT; i++)
            for (size_t j = 0; j < IMAGE_WIDTH; j++) {
                erodeData[i][j] = i % MAX_U8;
        }
        Tensor src(&erodeData, SHAPEHW1, MxBase::TensorDType::FLOAT32);
        src.ToDevice(0);
        Tensor dst;
        BlurConfig blur;
        blur.iterations = MAX_ERODE_ITER;
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        APP_ERROR ret = Erode(src, dst, blur, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        dst.ToHost();
        EXPECT_EQ(static_cast<float *>(dst.GetData())[IMAGE_WIDTH], 0);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Erode_Should_Return_Success_When_Src_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        float erodeData[IMAGE_HEIGHT][IMAGE_WIDTH];
        for (size_t i = 0; i < IMAGE_HEIGHT; i++)
            for (size_t j = 0; j < IMAGE_WIDTH; j++) {
                erodeData[i][j] = i % MAX_U8;
        }
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src(&erodeData, SHAPEHW1, MxBase::TensorDType::FLOAT32);
        src.ToDevice(0);
        Tensor srcFP16;
        APP_ERROR ret = MxBase::ConvertTo(src, srcFP16, MxBase::TensorDType::FLOAT16, stream0);
        Tensor dst;
        BlurConfig blur;
        blur.kernelSize = MxBase::Size(MAX_ERODE_KERNEL, MAX_ERODE_KERNEL);
        ret = Erode(srcFP16, dst, blur, stream0);
        EXPECT_EQ(ret, APP_ERR_OK);
        stream0.Synchronize();
        ret = MxBase::ConvertTo(dst, src, MxBase::TensorDType::FLOAT32, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        src.ToHost();
        EXPECT_EQ(static_cast<float *>(src.GetData())[IMAGE_WIDTH], 0);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_BatchSplit_Should_Return_Fail_When_Src_Is_None)
{
    MxBase::Tensor src(SHAPE1, MxBase::TensorDType::UINT8);
    std::vector<Tensor> dst;
    APP_ERROR ret = BatchSplit(src, dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_BatchSplit_Should_Return_Fail_When_Device_Is_Not_310P)
{
    MOCKER_CPP(&DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    MxBase::Tensor src(SHAPE1, MxBase::TensorDType::UINT8);
    std::vector<Tensor> dst;
    APP_ERROR ret = BatchSplit(src, dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_BatchSplit_Should_Return_Fail_When_Src_Shape_Is_Not_NHWC_or_HWC)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::Tensor src(SHAPE5, MxBase::TensorDType::UINT8);
        src.Malloc();
        src.ToDevice(0);
        std::vector <Tensor> dst;
        APP_ERROR ret = BatchSplit(src, dst, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_BatchSplit_Should_Return_Fail_When_Src_DataType_Is_Invalid)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::Tensor src(SHAPE1, MxBase::TensorDType::INT32);
        src.Malloc();
        src.ToDevice(0);
        std::vector<Tensor> dst;
        APP_ERROR ret = BatchSplit(src, dst, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_BatchSplit_Should_Return_Fail_When_Dst_Is_Empty_And_NoReplace)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::Tensor src(SHAPE1, MxBase::TensorDType::UINT8);
        src.Malloc();
        src.ToDevice(0);

        MxBase::Tensor tensor1(SHAPE1, TensorDType::UINT8);
        std::vector<Tensor> dst = {tensor1};
        APP_ERROR ret = BatchSplit(src, dst, false);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_BatchSplit_Should_Return_Fail_When_Dst_Shape_Is_InValid)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::Tensor src(SHAPE1, MxBase::TensorDType::UINT8);
        src.Malloc();
        src.ToDevice(0);

        MxBase::Tensor tensor1(SHAPE1, TensorDType::UINT8);
        MxBase::Tensor tensor2(SHAPE1, TensorDType::UINT8);
        tensor1.Malloc();
        tensor2.Malloc();
        tensor1.ToDevice(0);
        tensor2.ToDevice(0);
        std::vector<Tensor> dst = {tensor1, tensor2};
        APP_ERROR ret = BatchSplit(src, dst, false);
        EXPECT_EQ(ret, APP_ERR_COMM_ALLOC_MEM);

        dst = {tensor1};
        ret = BatchSplit(src, dst, false);
        EXPECT_EQ(ret, APP_ERR_COMM_ALLOC_MEM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_BatchSplit_Should_Return_Fail_When_Src_Device_Is_Invalid)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::AscendStream stream = MxBase::AscendStream(1);
        stream.CreateAscendStream();
        MxBase::Tensor src(SHAPE1, MxBase::TensorDType::UINT8);
        src.Malloc();
        src.ToDevice(0);

        size_t dstShapeSize = SHAPE1.size() - 1;
        std::vector<uint32_t> dstShape(SHAPE1.end() - dstShapeSize, SHAPE1.end());
        MxBase::Tensor tensor1(dstShape, TensorDType::UINT8);
        MxBase::Tensor tensor2(dstShape, TensorDType::UINT8);
        tensor1.Malloc();
        tensor2.Malloc();
        tensor1.ToDevice(0);
        tensor2.ToDevice(0);
        std::vector<Tensor> dst = {tensor1, tensor2};
        APP_ERROR ret = BatchSplit(src, dst, false, stream);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_BatchSplit_Should_Return_Fail_With_NoReplace_And_Stream)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::AscendStream stream = MxBase::AscendStream(0);
        stream.CreateAscendStream();
        MxBase::Tensor src(SHAPE1, MxBase::TensorDType::UINT8);
        src.Malloc();

        size_t dstShapeSize = SHAPE1.size() - 1;
        std::vector<uint32_t> dstShape(SHAPE1.end() - dstShapeSize, SHAPE1.end());
        MxBase::Tensor tensor1(dstShape, TensorDType::UINT8);
        MxBase::Tensor tensor2(dstShape, TensorDType::UINT8);
        tensor1.Malloc();
        tensor2.Malloc();
        std::vector<Tensor> dst = {tensor1, tensor2};
        MOCKER_CPP(&aclrtLaunchCallback).stubs().will(returnValue(-1));
        APP_ERROR ret = BatchSplit(src, dst, false, stream);
        EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_BatchSplit_Should_Return_Success_With_NoReplace_And_Stream)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::AscendStream stream = MxBase::AscendStream(0);
        stream.CreateAscendStream();
        MxBase::Tensor src(SHAPE1, MxBase::TensorDType::UINT8);
        src.Malloc();
        src.ToDevice(0);

        size_t dstShapeSize = SHAPE1.size() - 1;
        std::vector<uint32_t> dstShape(SHAPE1.end() - dstShapeSize, SHAPE1.end());
        MxBase::Tensor tensor1(dstShape, TensorDType::UINT8);
        MxBase::Tensor tensor2(dstShape, TensorDType::UINT8);
        tensor1.Malloc();
        tensor2.Malloc();
        tensor1.ToDevice(0);
        tensor2.ToDevice(0);
        std::vector<Tensor> dst = {tensor1, tensor2};
        APP_ERROR ret = BatchSplit(src, dst, false, stream);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Hstack_Should_Return_Fail_When_Src_Shape_Is_Invalid)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, {DIM}, TensorDType::UINT8);
        Tensor tensor2(&g_data2, {DIM}, TensorDType::UINT8);
        Tensor tensor3(SHAPE4, TensorDType::UINT8);
        tensor1.ToDevice(0);
        tensor2.ToDevice(0);
        std::vector <Tensor> srcVec = {tensor1};
        APP_ERROR ret = Hstack(srcVec, tensor3);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

        srcVec = {tensor1, tensor2};
        ret = Hstack(srcVec, tensor3);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Hstack_Vstack_Should_Return_Fail_With_Invalid_Dst_Device)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor tensor2(&g_data1, SHAPE3, TensorDType::UINT8);
        tensor1.ToDevice(0);
        tensor2.ToDevice(0);
        std::vector<Tensor> srcVec = {tensor1, tensor2};
        Tensor tensor4(SHAPE4, TensorDType::FLOAT16);
        tensor4.Malloc();
        APP_ERROR ret = Hstack(srcVec, tensor4);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

        ret = Vstack(srcVec, tensor4);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Tile_Should_Return_Fail_With_InValid_DeviceType)
{
    MOCKER_CPP(&DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    Tensor src(&g_data1, SHAPE1, TensorDType::UINT8);
    Tensor dst(SHAPE1, TensorDType::UINT8);
    std::vector<uint32_t> multiple = {1, 1, 1};
    APP_ERROR ret = Tile(src, dst, multiple);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Tile_Should_Return_Fail_With_Invalid_Multiple)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data1, SHAPE1, TensorDType::UINT8);
        src.ToDevice(0);
        Tensor dst(SHAPE1, TensorDType::UINT8);
        std::vector <uint32_t> multiple = {1, 1};
        APP_ERROR ret = Tile(src, dst, multiple);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Tile_Should_Return_Fail_With_Invalid_Dst_Device)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(std::vector<uint32_t>{DIM}, TensorDType::UINT8, 0);
        src.Malloc();
        Tensor dst(std::vector<uint32_t>{DIM}, TensorDType::UINT8);
        dst.Malloc();
        std::vector <uint32_t> multiple = {DIM};
        APP_ERROR ret = Tile(src, dst, multiple);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Tile_Should_Return_Fail_When_Tensor_To_Device_Fail)
{
    MOCKER_CPP(&Tensor::ToDevice).stubs().will(returnValue(-1));
    if (DeviceManager::IsAscend310P()) {
        Tensor src(&g_data1, SHAPE1, TensorDType::UINT8, 0);
        Tensor dst(SHAPE1, TensorDType::UINT8, 0);
        std::vector <uint32_t> multiple = {1, 1, 1};
        APP_ERROR ret = Tile(src, dst, multiple);
        EXPECT_EQ(ret, -1);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Vstack_Should_Return_Fail_With_Incorrect_Device)
{
    MOCKER_CPP(&DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    Tensor tensor1(&g_data1, {DIM}, TensorDType::UINT8);
    Tensor tensor2(SHAPE4, TensorDType::UINT8);
    std::vector<Tensor> srcVec = {tensor1};
    APP_ERROR ret = Vstack(srcVec, tensor2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(CoreOperationsOnTensorsTest, Test_Vstack_Should_Return_Fail_With_Invalid_Src_Shape)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, {DIM}, TensorDType::UINT8);
        Tensor tensor2(&g_data2, {DIM}, TensorDType::UINT8);
        Tensor tensor3(SHAPE4, TensorDType::UINT8);
        tensor1.ToDevice(0);
        tensor2.ToDevice(0);
        std::vector <Tensor> srcVec = {tensor1};
        APP_ERROR ret = Vstack(srcVec, tensor3);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

        srcVec = {tensor1, tensor2};
        ret = Vstack(srcVec, tensor3);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Vstack_Should_Return_Fail_With_Invalid_Stream)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor tensor2(&g_data1, SHAPE3, TensorDType::UINT8);
        tensor1.ToDevice(0);
        tensor2.ToDevice(0);
        std::vector<Tensor> srcVec = {tensor1, tensor2};
        Tensor tensor4;
        MxBase::AscendStream stream = MxBase::AscendStream(1);
        stream.CreateAscendStream();
        APP_ERROR ret = Vstack(srcVec, tensor4, stream);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_With_Invalid_Src_Shape)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPEHW, TensorDType::FLOAT32, 0);
        tensor1.Malloc();
        Tensor tensor2;
        APP_ERROR ret = Transpose(tensor1, tensor2, TRANS_AXIS);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_With_Invalid_Dst_Device)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPEHW, TensorDType::FLOAT32, 0);
        tensor1.Malloc();
        Tensor tensor2(SHAPEHW, TensorDType::FLOAT32);
        tensor2.Malloc();

        std::vector<int> axes;
        APP_ERROR ret = Transpose(tensor1, tensor2, axes);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Faild_With_Invalid_Axes)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPEHW, TensorDType::FLOAT32, 0);
        tensor1.Malloc();
        Tensor tensor2;

        std::vector<int> axes = {1, 3};
        APP_ERROR ret = Transpose(tensor1, tensor2, axes);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Fail_When_To_Device_Is_Fail)
{
    MOCKER_CPP(&Tensor::ToDevice).stubs().will(returnValue(-1));
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPEHW, TensorDType::FLOAT32, 0);
        tensor1.Malloc();
        Tensor tensor2;

        std::vector<int> axes;
        APP_ERROR ret = Transpose(tensor1, tensor2, axes);
        EXPECT_EQ(ret, -1);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_Transpose_Should_Return_Success_With_Empty_Axes)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPEHW, TensorDType::FLOAT32, 0);
        tensor1.Malloc();
        Tensor tensor2;

        std::vector<int> axes;
        APP_ERROR ret = Transpose(tensor1, tensor2, axes);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(CoreOperationsOnTensorsTest, DISABLED_Test_Tile_Should_Return_Success)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor src(std::vector<uint32_t>{DIM}, TensorDType::UINT8, 0);
        src.Malloc();
        Tensor dst;
        std::vector <uint32_t> multiple = {DIM};
        APP_ERROR ret = Tile(src, dst, multiple);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(CoreOperationsOnTensorsTest, Test_BatchSplit_Should_Return_Success_When_Dst_Is_Valid_And_CPU)
{
    if (DeviceManager::IsAscend310P()) {
        MxBase::Tensor src(SHAPE1, MxBase::TensorDType::UINT8);
        src.Malloc();

        size_t dstShapeSize = SHAPE1.size() - 1;
        std::vector<uint32_t> dstShape(SHAPE1.end() - dstShapeSize, SHAPE1.end());
        MxBase::Tensor tensor1(dstShape, TensorDType::UINT8);
        MxBase::Tensor tensor2(dstShape, TensorDType::UINT8);
        tensor1.Malloc();
        tensor2.Malloc();
        std::vector<Tensor> dst = {tensor1, tensor2};
        APP_ERROR ret = BatchSplit(src, dst, false);
        EXPECT_EQ(ret, APP_ERR_OK);

        src.ToDevice(0);
        tensor1.ToDevice(0);
        tensor2.ToDevice(0);
        dst = {tensor1, tensor2};
        ret = BatchSplit(src, dst, true);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}


TEST_F(CoreOperationsOnTensorsTest, DISABLED_Test_Hstack_Should_Return_Success)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor tensor2(&g_data1, SHAPE3, TensorDType::UINT8);
        tensor1.ToDevice(0);
        tensor2.ToDevice(0);
        std::vector<Tensor> srcVec = {tensor1, tensor2};
        Tensor tensor4;
        APP_ERROR ret = Hstack(srcVec, tensor4);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
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
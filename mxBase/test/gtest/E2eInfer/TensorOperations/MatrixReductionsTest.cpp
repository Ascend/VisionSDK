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
#include "MxBase/E2eInfer/TensorOperation/MatricesOperation/MatrixReductions.h"
#include "MxBase/MxBase.h"
#include "MxBase/DeviceManager/DeviceManager.h"

namespace {
    using namespace MxBase;
    const size_t DATA_LEN = 6;
    const size_t CHANNEL_NUM = 3;
    const std::vector<uint32_t> SHAPE1 = {0x1};
    const std::vector<uint32_t> SHAPE2 = {0x1, 0x2};
    const std::vector<uint32_t> SHAPE3 = {0x1, 0x2, 0x3};
    const std::vector<uint32_t> SHAPE4 = {0x1, 0x2, 0x3, 0x1};
    const std::vector<uint32_t> SHAPE5 = {0x3};
    const std::vector<uint32_t> SHAPE6 = {0x1, 0x2, 0x1};
    const std::vector<uint32_t> SHAPE7 = {0x2};
    const std::vector<uint32_t> SHAPE8 = {0x2, 0x3};
    const std::vector<uint32_t> SHAPE9 = {0x2, 0x3, 0x5};
    uint8_t g_data1[DATA_LEN] = {0};
    uint8_t g_data2[DATA_LEN] = {0};
    uint8_t g_data3[DATA_LEN] = {1, 2, 3, 4, 5, 6};
    float g_data4[DATA_LEN] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    float g_data5[CHANNEL_NUM] = {17.0f, 29.0f, 45.0f};
    float g_data6[DATA_LEN] = {-1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f};
    float g_data7[CHANNEL_NUM] = {5.0f, 7.0f, 9.0f};
    uint8_t g_data8[CHANNEL_NUM] = {5, 7, 9};

    constexpr uint32_t SHAPE_DIMENSION_ZERO = 0;
    constexpr uint32_t SHAPE_DIMENSION_ONE = 1;
    constexpr uint32_t SHAPE_DIMENSION_TWO = 2;
    constexpr uint8_t ROI_X0_1 = 0;
    constexpr uint8_t ROI_Y0_1 = 0;
    constexpr uint8_t ROI_X1_2 = 1;
    constexpr uint8_t ROI_Y1_2 = 1;
    constexpr uint8_t G_DATA3_MIN_VAL = 1;
    constexpr uint8_t G_DATA3_MAX_VAL = 6;
    constexpr float G_DATA4_MIN_VAL = 1.0f;
    constexpr float G_DATA4_MAX_VAL = 6.0f;
    constexpr uint32_t MIN_LOC_H = 0;
    constexpr uint32_t MIN_LOC_W = 0;
    constexpr uint32_t MAX_LOC_H = 1;
    constexpr uint32_t MAX_LOC_W = 2;

    class MatrixReductionsTest : public testing::Test {
    public:
        void TearDown() override
        {
            // clear mock
            GlobalMockObject::verify();
        }
    };

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Fail_When_Src_Is_Empty)
    {
        Tensor src;
        Tensor dst;
        APP_ERROR ret = Sum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Fail_When_Src_Is_Host)
    {
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst;
        APP_ERROR ret = Sum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Fail_When_Src_Datatype_Is_Int)
    {
        Tensor src(SHAPE3, TensorDType::INT32);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = Sum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Fail_When_Src_Shape_Is_Not_NHWC_Or_HWC)
    {
        Tensor src(&g_data1, SHAPE5, TensorDType::UINT8);
        src.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = Sum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Fail_When_Src_Shape_Channel_Is_Not_1_To_4)
    {
        Tensor src(SHAPE9, TensorDType::UINT8);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = Sum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Fail_When_Dst_Shape_Is_Not_Equal_Channel_Num)
    {
        Tensor src(&g_data1, SHAPE6, TensorDType::UINT8);
        Tensor dst(SHAPE5, TensorDType::UINT8);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        dst.ToDevice(0);
        APP_ERROR ret = Sum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Fail_When_Dst_Is_Host)
    {
        Tensor src(&g_data1, SHAPE3, TensorDType::FLOAT32);
        Tensor dst(SHAPE5, TensorDType::FLOAT32);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        APP_ERROR ret = Sum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
    {
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst(SHAPE5, TensorDType::UINT8);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        dst.ToDevice(1);
        APP_ERROR ret = Sum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
    {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst;
        src.ToDevice(1);
        APP_ERROR ret = Sum(src, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Fail_When_Src_Datatype_Is_Different_With_Dst)
    {
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst(SHAPE5, TensorDType::FLOAT32);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        dst.ToDevice(0);
        APP_ERROR ret = Sum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Fail_When_Src_Is_Inplace)
    {
        if (DeviceManager::IsAscend310P()) {
            Tensor src(SHAPE3, TensorDType::UINT8, 0);
            MxBase::Rect roiSrc(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
            src.SetReferRect(roiSrc);
            Tensor::TensorMalloc(src);
            Tensor dst(SHAPE5, TensorDType::UINT8, 0);
            Tensor::TensorMalloc(dst);
            APP_ERROR ret = Sum(src, dst);
            EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        }
    }

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Success_When_Src_Is_Uint8_Asyn)
    {
        if (DeviceManager::IsAscend310P()) {
            AscendStream stream0 = AscendStream(0);
            stream0.CreateAscendStream();
            Tensor src(&g_data3, SHAPE3, TensorDType::UINT8);
            Tensor dst;
            src.ToDevice(0);
            APP_ERROR ret = Sum(src, dst, stream0);
            stream0.Synchronize();
            stream0.DestroyAscendStream();
            EXPECT_EQ(ret, APP_ERR_OK);
            dst.ToHost();
            for (size_t i = 0; i < CHANNEL_NUM; i++) {
                EXPECT_EQ((static_cast<uint8_t *>(dst.GetData()))[i], g_data8[i]);
            }
        }
    }

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Success_When_Src_Is_FLOAT16_Asyn)
    {
        if (DeviceManager::IsAscend310P()) {
            AscendStream stream0 = AscendStream(0);
            stream0.CreateAscendStream();
            Tensor src(&g_data4, SHAPE3, TensorDType::FLOAT32);
            src.ToDevice(0);
            Tensor srcFp16;
            ConvertTo(src, srcFp16, TensorDType::FLOAT16);
            Tensor dst;
            APP_ERROR ret = Sum(srcFp16, dst, stream0);
            stream0.Synchronize();
            stream0.DestroyAscendStream();
            EXPECT_EQ(ret, APP_ERR_OK);
            Tensor dstFp32;
            ConvertTo(dst, dstFp32, TensorDType::FLOAT32);
            dstFp32.ToHost();
            for (size_t i = 0; i < CHANNEL_NUM; i++) {
                EXPECT_EQ((static_cast<float *>(dstFp32.GetData()))[i], g_data7[i]);
            }
        }
    }

    TEST_F(MatrixReductionsTest, Test_Sum_Should_Return_Success_When_Src_Is_FLOAT32_Sync)
    {
        if (DeviceManager::IsAscend310P()) {
            Tensor src(&g_data4, SHAPE3, TensorDType::FLOAT32);
            Tensor dst;
            src.ToDevice(0);
            APP_ERROR ret = Sum(src, dst);
            EXPECT_EQ(ret, APP_ERR_OK);
            dst.ToHost();
            for (size_t i = 0; i < CHANNEL_NUM; i++) {
                EXPECT_EQ((static_cast<float *>(dst.GetData()))[i], g_data7[i]);
            }
        }
    }

    TEST_F(MatrixReductionsTest, TestReduce)
    {
        Tensor tensor1(SHAPE1, TensorDType::UINT8);
        Tensor tensor2(SHAPE2, TensorDType::UINT8);
        Tensor::TensorMalloc(tensor1);
        Tensor::TensorMalloc(tensor2);
        tensor1.ToDevice(0);
        tensor2.ToDevice(0);

        APP_ERROR ret = Reduce(tensor1, tensor2, MxBase::ReduceDim::REDUCE_HEIGHT, MxBase::ReduceType::REDUCE_SUM);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

        Tensor tensor4(&g_data2, SHAPE3, TensorDType::UINT8);
        tensor4.ToDevice(0);
        ret = Reduce(tensor2, tensor4, MxBase::ReduceDim::REDUCE_HEIGHT, MxBase::ReduceType::REDUCE_MAX);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_SqrSum_Should_Return_Fail_When_Src_Is_Empty)
    {
        Tensor src;
        Tensor dst;
        APP_ERROR ret = SqrSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_SqrSum_Should_Return_Fail_When_Src_Is_Host)
    {
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst;
        APP_ERROR ret = SqrSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_SqrSum_Should_Return_Fail_When_Src_Datatype_Is_Int)
    {
        Tensor src(SHAPE3, TensorDType::INT32);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = SqrSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_SqrSum_Should_Return_Fail_When_Src_Shape_Is_Not_HWC)
    {
        Tensor src(&g_data1, SHAPE4, TensorDType::UINT8);
        src.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = SqrSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_SqrSum_Should_Return_Fail_When_Dst_Shape_Is_Not_Equal_Channel_Num)
    {
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst(SHAPE1, TensorDType::FLOAT32);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        dst.ToDevice(0);
        APP_ERROR ret = SqrSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_SqrSum_Should_Return_Fail_When_Dst_Is_Uint8)
    {
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst(SHAPE5, TensorDType::UINT8);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        dst.ToDevice(0);
        APP_ERROR ret = SqrSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_SqrSum_Should_Return_Fail_When_Dst_Is_Host)
    {
        Tensor src(&g_data1, SHAPE3, TensorDType::FLOAT32);
        Tensor dst(SHAPE5, TensorDType::FLOAT32);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        APP_ERROR ret = SqrSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_SqrSum_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
    {
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst(SHAPE5, TensorDType::FLOAT32);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        dst.ToDevice(1);
        APP_ERROR ret = SqrSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_SqrSum_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
    {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst;
        src.ToDevice(1);
        APP_ERROR ret = SqrSum(src, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_SqrSum_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_StreamId)
    {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst(SHAPE5, TensorDType::FLOAT32);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        dst.ToDevice(1);
        APP_ERROR ret = SqrSum(src, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_SqrSum_Should_Return_Fail_When_Src_Is_Inplace)
    {
        if (DeviceManager::IsAscend310P()) {
            Tensor src(SHAPE3, TensorDType::UINT8, 0);
            Tensor::TensorMalloc(src);
            Tensor dst(SHAPE5, TensorDType::FLOAT32, 0);
            Tensor::TensorMalloc(dst);
            MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
            src = Tensor(src, roi_src);
            APP_ERROR ret = SqrSum(src, dst);
            EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        }
    }

    TEST_F(MatrixReductionsTest, Test_SqrSum_Should_Return_Success_When_Src_Is_Uint8)
    {
        if (DeviceManager::IsAscend310P()) {
            AscendStream stream0 = AscendStream(0);
            stream0.CreateAscendStream();
            Tensor src(&g_data3, SHAPE3, TensorDType::UINT8);
            Tensor dst;
            src.ToDevice(0);
            APP_ERROR ret = SqrSum(src, dst, stream0);
            stream0.Synchronize();
            stream0.DestroyAscendStream();
            EXPECT_EQ(ret, APP_ERR_OK);
            dst.ToHost();
            EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], g_data5[SHAPE_DIMENSION_ZERO]);
            EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE], g_data5[SHAPE_DIMENSION_ONE]);
            EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO], g_data5[SHAPE_DIMENSION_TWO]);
        }
    }

    TEST_F(MatrixReductionsTest, Test_SqrSum_Should_Return_Success_When_Src_Is_Float)
    {
        if (DeviceManager::IsAscend310P()) {
            AscendStream stream0 = AscendStream(0);
            stream0.CreateAscendStream();
            Tensor src(&g_data4, SHAPE3, TensorDType::FLOAT32);
            Tensor dst;
            src.ToDevice(0);
            APP_ERROR ret = SqrSum(src, dst, stream0);
            stream0.Synchronize();
            stream0.DestroyAscendStream();
            EXPECT_EQ(ret, APP_ERR_OK);
            dst.ToHost();
            EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ZERO], g_data5[SHAPE_DIMENSION_ZERO]);
            EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_ONE], g_data5[SHAPE_DIMENSION_ONE]);
            EXPECT_EQ((static_cast<float *>(dst.GetData()))[SHAPE_DIMENSION_TWO], g_data5[SHAPE_DIMENSION_TWO]);
        }
    }

    TEST_F(MatrixReductionsTest, Test_AbsSum_Should_Return_Fail_When_Src_Is_Empty)
    {
        Tensor src;
        Tensor dst;
        APP_ERROR ret = AbsSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_AbsSum_Should_Return_Fail_When_Src_Is_Host)
    {
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst;
        APP_ERROR ret = AbsSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_AbsSum_Should_Return_Fail_When_Src_Datatype_Is_Int)
    {
        Tensor src(SHAPE3, TensorDType::INT32);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = AbsSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_AbsSum_Should_Return_Fail_When_Src_Shape_Is_Not_HWC)
    {
        Tensor src(&g_data1, SHAPE4, TensorDType::UINT8);
        src.ToDevice(0);
        Tensor dst;
        APP_ERROR ret = AbsSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_AbsSum_Should_Return_Fail_When_Dst_Shape_Is_Not_Equal_Channel_Num)
    {
        Tensor src(&g_data1, SHAPE6, TensorDType::UINT8);
        Tensor dst(SHAPE5, TensorDType::FLOAT32);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        dst.ToDevice(0);
        APP_ERROR ret = AbsSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_AbsSum_Should_Return_Fail_When_Dst_Is_Uint8)
    {
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst(SHAPE5, TensorDType::UINT8);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        dst.ToDevice(0);
        APP_ERROR ret = AbsSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_AbsSum_Should_Return_Fail_When_Dst_Is_Host)
    {
        Tensor src(&g_data1, SHAPE3, TensorDType::FLOAT32);
        Tensor dst(SHAPE5, TensorDType::FLOAT32);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        APP_ERROR ret = AbsSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_AbsSum_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
    {
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst(SHAPE5, TensorDType::FLOAT32);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        dst.ToDevice(1);
        APP_ERROR ret = AbsSum(src, dst);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_AbsSum_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
    {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst;
        src.ToDevice(1);
        APP_ERROR ret = AbsSum(src, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_AbsSum_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_StreamId)
    {
        AscendStream stream0 = AscendStream(0);
        stream0.CreateAscendStream();
        Tensor src(&g_data1, SHAPE3, TensorDType::UINT8);
        Tensor dst(SHAPE5, TensorDType::FLOAT32);
        Tensor::TensorMalloc(dst);
        src.ToDevice(0);
        dst.ToDevice(1);
        APP_ERROR ret = AbsSum(src, dst, stream0);
        stream0.Synchronize();
        stream0.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_AbsSum_Should_Return_Fail_When_Src_Is_Inplace)
    {
        if (DeviceManager::IsAscend310P()) {
            Tensor src(SHAPE3, TensorDType::UINT8, 0);
            Tensor::TensorMalloc(src);
            Tensor dst(SHAPE5, TensorDType::FLOAT32, 0);
            Tensor::TensorMalloc(dst);
            MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
            src = Tensor(src, roi_src);
            APP_ERROR ret = AbsSum(src, dst);
            EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        }
    }

    TEST_F(MatrixReductionsTest, Test_AbsSum_Should_Return_Success_When_Src_Is_Uint8)
    {
        if (DeviceManager::IsAscend310P()) {
            AscendStream stream0 = AscendStream(0);
            stream0.CreateAscendStream();
            Tensor src(&g_data3, SHAPE3, TensorDType::UINT8);
            Tensor dst;
            src.ToDevice(0);
            APP_ERROR ret = AbsSum(src, dst, stream0);
            stream0.Synchronize();
            stream0.DestroyAscendStream();
            EXPECT_EQ(ret, APP_ERR_OK);
            dst.ToHost();
            for (size_t i = 0; i < CHANNEL_NUM; i++) {
                EXPECT_EQ((static_cast<float *>(dst.GetData()))[i], g_data7[i]);
            }
        }
    }

    TEST_F(MatrixReductionsTest, Test_AbsSum_Should_Return_Success_When_Src_Is_Float)
    {
        if (DeviceManager::IsAscend310P()) {
            AscendStream stream0 = AscendStream(0);
            stream0.CreateAscendStream();
            Tensor src(&g_data6, SHAPE3, TensorDType::FLOAT32);
            Tensor dst;
            src.ToDevice(0);
            APP_ERROR ret = AbsSum(src, dst, stream0);
            stream0.Synchronize();
            stream0.DestroyAscendStream();
            EXPECT_EQ(ret, APP_ERR_OK);
            dst.ToHost();
            for (size_t i = 0; i < CHANNEL_NUM; i++) {
                EXPECT_EQ((static_cast<float *>(dst.GetData()))[i], g_data7[i]);
            }
        }
    }
    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_Src_Is_Empty)
    {
        Tensor src;
        Tensor minVal;
        Tensor maxVal;
        Tensor minLoc;
        Tensor maxLoc;
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_Src_Is_Host)
    {
        Tensor src(&g_data1, SHAPE2, TensorDType::UINT8);
        Tensor minVal;
        Tensor maxVal;
        Tensor minLoc;
        Tensor maxLoc;
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_Src_Datatype_Is_Int)
    {
        Tensor src(SHAPE2, TensorDType::INT32);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor minVal;
        Tensor maxVal;
        Tensor minLoc;
        Tensor maxLoc;
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_Src_Shape_Is_Not_HW_OR_HWC)
    {
        Tensor src(SHAPE4, TensorDType::UINT8);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor minVal;
        Tensor maxVal;
        Tensor minLoc;
        Tensor maxLoc;
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_Src_Shape_Is_HWC_But_C_Is_Not_One)
    {
        Tensor src(SHAPE3, TensorDType::UINT8);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor minVal;
        Tensor maxVal;
        Tensor minLoc;
        Tensor maxLoc;
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_MinVal_Or_MaxVal_Dimension_Is_Not_One)
    {
        Tensor src(SHAPE2, TensorDType::UINT8);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor minVal(SHAPE2, TensorDType::UINT8);
        Tensor::TensorMalloc(minVal);
        minVal.ToDevice(0);
        Tensor maxVal;
        Tensor minLoc;
        Tensor maxLoc;
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_MinLoc_Or_MaxLoc_Dimension_Is_Not_One)
    {
        Tensor src(SHAPE2, TensorDType::UINT8);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor minVal;
        Tensor maxVal;
        Tensor minLoc(SHAPE3, TensorDType::UINT32);
        Tensor::TensorMalloc(minLoc);
        minLoc.ToDevice(0);
        Tensor maxLoc;
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_MinVal_Or_MaxVal_Type_Is_Different_With_src)
    {
        Tensor src(SHAPE2, TensorDType::UINT8);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor minVal(SHAPE1, TensorDType::FLOAT32);
        Tensor maxVal(SHAPE1, TensorDType::FLOAT32);
        Tensor minLoc(SHAPE7, TensorDType::UINT32);
        Tensor maxLoc(SHAPE7, TensorDType::UINT32);
        Tensor::TensorMalloc(minVal);
        Tensor::TensorMalloc(maxVal);
        Tensor::TensorMalloc(minLoc);
        Tensor::TensorMalloc(maxLoc);
        minVal.ToDevice(0);
        maxVal.ToDevice(0);
        minLoc.ToDevice(0);
        maxLoc.ToDevice(0);
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_MinLoc_Or_MaxLoc_Type_Is_Not_UINT32)
    {
        Tensor src(SHAPE2, TensorDType::UINT8);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor minVal(SHAPE1, TensorDType::FLOAT32);
        Tensor maxVal(SHAPE1, TensorDType::FLOAT32);
        Tensor minLoc(SHAPE7, TensorDType::FLOAT32);
        Tensor maxLoc(SHAPE7, TensorDType::FLOAT32);
        Tensor::TensorMalloc(minVal);
        Tensor::TensorMalloc(maxVal);
        Tensor::TensorMalloc(minLoc);
        Tensor::TensorMalloc(maxLoc);
        minVal.ToDevice(0);
        maxVal.ToDevice(0);
        minLoc.ToDevice(0);
        maxLoc.ToDevice(0);
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_Dst_Is_Host)
    {
        Tensor src(SHAPE2, TensorDType::UINT8);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor minVal(SHAPE1, TensorDType::FLOAT32);
        Tensor maxVal;
        Tensor minLoc;
        Tensor maxLoc;
        Tensor::TensorMalloc(minVal);
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
    {
        Tensor src(SHAPE2, TensorDType::UINT8);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor minVal(SHAPE1, TensorDType::FLOAT32);
        Tensor::TensorMalloc(minVal);
        minVal.ToDevice(1);
        Tensor maxVal;
        Tensor minLoc;
        Tensor maxLoc;
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_Dst_DeviceId_Is_Different)
    {
        Tensor src(SHAPE2, TensorDType::UINT8);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor minVal(SHAPE1, TensorDType::FLOAT32);
        Tensor::TensorMalloc(minVal);
        minVal.ToDevice(0);
        Tensor maxVal(SHAPE1, TensorDType::FLOAT32);
        Tensor::TensorMalloc(minVal);
        minVal.ToDevice(1);
        Tensor minLoc;
        Tensor maxLoc;
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest,
           Test_MinMaxLoc_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
    {
        AscendStream stream = AscendStream(0);
        stream.CreateAscendStream();
        Tensor src(SHAPE2, TensorDType::UINT8);
        Tensor::TensorMalloc(src);
        src.ToDevice(1);
        Tensor minVal;
        Tensor maxVal;
        Tensor minLoc;
        Tensor maxLoc;
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest,
           Test_MinMaxLoc_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_StreamId)
    {
        AscendStream stream = AscendStream(0);
        stream.CreateAscendStream();
        Tensor src(SHAPE2, TensorDType::UINT8);
        Tensor::TensorMalloc(src);
        src.ToDevice(0);
        Tensor minVal(SHAPE1, TensorDType::FLOAT32);
        Tensor::TensorMalloc(minVal);
        minVal.ToDevice(1);
        Tensor maxVal;
        Tensor minLoc;
        Tensor maxLoc;
        APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_Src_Is_Inplace)
    {
        if (DeviceManager::IsAscend310P()) {
            Tensor src(SHAPE3, TensorDType::UINT8, 0);
            Tensor::TensorMalloc(src);
            MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
            src = Tensor(src, roi_src);
            Tensor minVal;
            Tensor maxVal;
            Tensor minLoc;
            Tensor maxLoc;
            APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
            EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        }
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Success_When_Src_Is_Uint8)
    {
        if (DeviceManager::IsAscend310P()) {
            Tensor src(&g_data3, SHAPE8, TensorDType::UINT8);
            src.ToDevice(0);
            Tensor minVal;
            Tensor maxVal;
            Tensor minLoc;
            Tensor maxLoc;
            APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
            EXPECT_EQ(ret, APP_ERR_OK);
            minVal.ToHost();
            maxVal.ToHost();
            minLoc.ToHost();
            maxLoc.ToHost();
            EXPECT_EQ((static_cast<uint8_t *>(minVal.GetData()))[SHAPE_DIMENSION_ZERO], G_DATA3_MIN_VAL);
            EXPECT_EQ((static_cast<uint8_t *>(maxVal.GetData()))[SHAPE_DIMENSION_ZERO], G_DATA3_MAX_VAL);
            EXPECT_EQ((static_cast<uint32_t *>(minLoc.GetData()))[SHAPE_DIMENSION_ZERO], MIN_LOC_H);
            EXPECT_EQ((static_cast<uint32_t *>(minLoc.GetData()))[SHAPE_DIMENSION_ONE], MIN_LOC_W);
            EXPECT_EQ((static_cast<uint32_t *>(maxLoc.GetData()))[SHAPE_DIMENSION_ZERO], MAX_LOC_H);
            EXPECT_EQ((static_cast<uint32_t *>(maxLoc.GetData()))[SHAPE_DIMENSION_ONE], MAX_LOC_W);
        }
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Fail_When_aclnnMinMaxLocGetWorkspaceSize_Fail)
    {
        if (DeviceManager::IsAscend310P()) {
            MOCKER_CPP(AclApi::aclnnMinMaxLocGetWorkspaceSize).stubs().will(returnValue(1));
            Tensor src(&g_data3, SHAPE8, TensorDType::UINT8);
            src.ToDevice(0);
            Tensor minVal;
            Tensor maxVal;
            Tensor minLoc;
            Tensor maxLoc;
            APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc);
            EXPECT_EQ(ret, 1);
        }
    }

    TEST_F(MatrixReductionsTest, Test_MinMaxLoc_Should_Return_Success_When_Src_Is_Float32)
    {
        if (DeviceManager::IsAscend310P()) {
            AscendStream stream = AscendStream(0);
            stream.CreateAscendStream();
            Tensor src(&g_data4, SHAPE8, TensorDType::FLOAT32);
            src.ToDevice(0);
            Tensor minVal;
            Tensor maxVal;
            Tensor minLoc;
            Tensor maxLoc;
            APP_ERROR ret = MinMaxLoc(src, minVal, maxVal, minLoc, maxLoc, stream);
            stream.Synchronize();
            stream.DestroyAscendStream();
            EXPECT_EQ(ret, APP_ERR_OK);
            minVal.ToHost();
            maxVal.ToHost();
            minLoc.ToHost();
            maxLoc.ToHost();
            EXPECT_EQ((static_cast<float *>(minVal.GetData()))[SHAPE_DIMENSION_ZERO], G_DATA4_MIN_VAL);
            EXPECT_EQ((static_cast<float *>(maxVal.GetData()))[SHAPE_DIMENSION_ZERO], G_DATA4_MAX_VAL);
            EXPECT_EQ((static_cast<uint32_t *>(minLoc.GetData()))[SHAPE_DIMENSION_ZERO], MIN_LOC_H);
            EXPECT_EQ((static_cast<uint32_t *>(minLoc.GetData()))[SHAPE_DIMENSION_ONE], MIN_LOC_W);
            EXPECT_EQ((static_cast<uint32_t *>(maxLoc.GetData()))[SHAPE_DIMENSION_ZERO], MAX_LOC_H);
            EXPECT_EQ((static_cast<uint32_t *>(maxLoc.GetData()))[SHAPE_DIMENSION_ONE], MAX_LOC_W);
        }
    }

    TEST_F(MatrixReductionsTest, Test_MinMax_Should_Return_Fail_When_Src_Is_Empty)
    {
        Tensor src;
        Tensor minVal;
        Tensor maxVal;
        APP_ERROR ret = MinMax(src, minVal, maxVal);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMax_Should_Return_Fail_When_Src_Is_Host)
    {
        Tensor src(&g_data1, SHAPE2, TensorDType::UINT8);
        Tensor minVal(SHAPE1, TensorDType::UINT8, 0);
        Tensor maxVal(SHAPE1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(minVal);
        Tensor::TensorMalloc(maxVal);
        APP_ERROR ret = MinMax(src, minVal, maxVal);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMax_Should_Return_Fail_When_Src_Datatype_Is_Int32)
    {
        Tensor src(&g_data1, SHAPE2, TensorDType::INT32);
        src.ToDevice(0);
        Tensor minVal(SHAPE1, TensorDType::UINT8, 0);
        Tensor maxVal(SHAPE1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(minVal);
        Tensor::TensorMalloc(maxVal);
        APP_ERROR ret = MinMax(src, minVal, maxVal);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMax_Should_Return_Fail_When_Src_Shape_Is_Not_HW_OR_HWC)
    {
        Tensor src(SHAPE4, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor minVal(SHAPE1, TensorDType::UINT8, 0);
        Tensor maxVal(SHAPE1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(minVal);
        Tensor::TensorMalloc(maxVal);
        APP_ERROR ret = MinMax(src, minVal, maxVal);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMax_Should_Return_Fail_When_MinVal_Or_MaxVal_Type_Is_Different_With_src)
    {
        Tensor src(&g_data1, SHAPE2, TensorDType::UINT8);
        src.ToDevice(0);
        Tensor minVal(SHAPE1, TensorDType::FLOAT32, 0);
        Tensor maxVal(SHAPE1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(minVal);
        Tensor::TensorMalloc(maxVal);
        APP_ERROR ret = MinMax(src, minVal, maxVal);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest,
           Test_MinMax_Should_Return_Fail_When_MinVal_Or_MaxVal_Device_Is_Different_With_src)
    {
        Tensor src(&g_data1, SHAPE2, TensorDType::UINT8);
        src.ToDevice(0);
        Tensor minVal(SHAPE1, TensorDType::UINT8, 0);
        Tensor maxVal(SHAPE1, TensorDType::UINT8, 1);
        Tensor::TensorMalloc(minVal);
        Tensor::TensorMalloc(maxVal);
        APP_ERROR ret = MinMax(src, minVal, maxVal);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMax_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
    {
        AscendStream stream = AscendStream(1);
        Tensor src(&g_data1, SHAPE2, TensorDType::UINT8);
        src.ToDevice(0);
        Tensor minVal(SHAPE1, TensorDType::UINT8, 0);
        Tensor maxVal(SHAPE1, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(minVal);
        Tensor::TensorMalloc(maxVal);
        APP_ERROR ret = MinMax(src, minVal, maxVal, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }

    TEST_F(MatrixReductionsTest, Test_MinMax_Should_Return_Success_When_MinVal_And_MaxVal_Do_Not_Malloc)
    {
        if (DeviceManager::IsAscend310P()) {
            Tensor src(&g_data1, SHAPE2, TensorDType::UINT8);
            src.ToDevice(0);
            Tensor minVal(SHAPE1, TensorDType::UINT8, 0);
            Tensor maxVal(SHAPE1, TensorDType::UINT8, 0);
            APP_ERROR ret = MinMax(src, minVal, maxVal);
            EXPECT_EQ(ret, APP_ERR_OK);
        }
    }

    TEST_F(MatrixReductionsTest, Test_MinMax_Should_Return_Fail_When_Src_Is_Inplace)
    {
        if (DeviceManager::IsAscend310P()) {
            Tensor src(SHAPE3, TensorDType::UINT8, 0);
            Tensor::TensorMalloc(src);
            MxBase::Rect roi_src(ROI_X0_1, ROI_Y0_1, ROI_X1_2, ROI_Y1_2);
            src = Tensor(src, roi_src);
            Tensor minVal(SHAPE1, TensorDType::UINT8, 0);
            Tensor maxVal(SHAPE1, TensorDType::UINT8, 0);
            Tensor::TensorMalloc(minVal);
            Tensor::TensorMalloc(maxVal);
            APP_ERROR ret = MinMax(src, minVal, maxVal);
            EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        }
    }

    TEST_F(MatrixReductionsTest, Test_MinMax_Should_Return_Success_When_Src_Is_Uint8_For_Synchronous_Scene)
    {
        if (DeviceManager::IsAscend310P()) {
            Tensor src(&g_data3, SHAPE8, TensorDType::UINT8);
            src.ToDevice(0);
            Tensor minVal(SHAPE1, TensorDType::UINT8, 0);
            Tensor maxVal(SHAPE1, TensorDType::UINT8, 0);
            Tensor::TensorMalloc(minVal);
            Tensor::TensorMalloc(maxVal);
            APP_ERROR ret = MinMax(src, minVal, maxVal);
            EXPECT_EQ(ret, APP_ERR_OK);
            minVal.ToHost();
            maxVal.ToHost();
            EXPECT_EQ((static_cast<uint8_t *>(minVal.GetData()))[SHAPE_DIMENSION_ZERO], G_DATA3_MIN_VAL);
            EXPECT_EQ((static_cast<uint8_t *>(maxVal.GetData()))[SHAPE_DIMENSION_ZERO], G_DATA3_MAX_VAL);
        }
    }

    TEST_F(MatrixReductionsTest, Test_MinMax_Should_Return_Success_When_Src_Is_Uint8_For_Asynchronous_Scene)
    {
        if (DeviceManager::IsAscend310P()) {
            AscendStream stream = AscendStream(0);
            Tensor src(&g_data3, SHAPE8, TensorDType::UINT8);
            src.ToDevice(0);
            Tensor minVal(SHAPE1, TensorDType::UINT8, 0);
            Tensor maxVal(SHAPE1, TensorDType::UINT8, 0);
            Tensor::TensorMalloc(minVal);
            Tensor::TensorMalloc(maxVal);
            APP_ERROR ret = MinMax(src, minVal, maxVal, stream);
            stream.Synchronize();
            stream.DestroyAscendStream();
            EXPECT_EQ(ret, APP_ERR_OK);
            minVal.ToHost();
            maxVal.ToHost();
            EXPECT_EQ((static_cast<uint8_t *>(minVal.GetData()))[SHAPE_DIMENSION_ZERO], G_DATA3_MIN_VAL);
            EXPECT_EQ((static_cast<uint8_t *>(maxVal.GetData()))[SHAPE_DIMENSION_ZERO], G_DATA3_MAX_VAL);
        }
    }

    TEST_F(MatrixReductionsTest, Test_MinMax_Should_Return_Success_When_Src_Is_Float32_For_Synchronous_Scene)
    {
        if (DeviceManager::IsAscend310P()) {
            Tensor src(&g_data4, SHAPE8, TensorDType::FLOAT32);
            src.ToDevice(0);
            Tensor minVal(SHAPE1, TensorDType::FLOAT32, 0);
            Tensor maxVal(SHAPE1, TensorDType::FLOAT32, 0);
            Tensor::TensorMalloc(minVal);
            Tensor::TensorMalloc(maxVal);
            APP_ERROR ret = MinMax(src, minVal, maxVal);
            EXPECT_EQ(ret, APP_ERR_OK);
            minVal.ToHost();
            maxVal.ToHost();
            EXPECT_EQ((static_cast<float *>(minVal.GetData()))[SHAPE_DIMENSION_ZERO], G_DATA4_MIN_VAL);
            EXPECT_EQ((static_cast<float *>(maxVal.GetData()))[SHAPE_DIMENSION_ZERO], G_DATA4_MAX_VAL);
        }
    }

    TEST_F(MatrixReductionsTest, Test_MinMax_Should_Return_Success_When_Src_Is_Float32_For_Asynchronous_Scene)
    {
        if (DeviceManager::IsAscend310P()) {
            AscendStream stream = AscendStream(0);
            Tensor src(&g_data4, SHAPE8, TensorDType::FLOAT32);
            src.ToDevice(0);
            Tensor minVal(SHAPE1, TensorDType::FLOAT32, 0);
            Tensor maxVal(SHAPE1, TensorDType::FLOAT32, 0);
            Tensor::TensorMalloc(minVal);
            Tensor::TensorMalloc(maxVal);
            APP_ERROR ret = MinMax(src, minVal, maxVal, stream);
            stream.Synchronize();
            stream.DestroyAscendStream();
            EXPECT_EQ(ret, APP_ERR_OK);
            minVal.ToHost();
            maxVal.ToHost();
            EXPECT_EQ((static_cast<float *>(minVal.GetData()))[SHAPE_DIMENSION_ZERO], G_DATA4_MIN_VAL);
            EXPECT_EQ((static_cast<float *>(maxVal.GetData()))[SHAPE_DIMENSION_ZERO], G_DATA4_MAX_VAL);
        }
    }
}

int main(int argc, char *argv[])
{
    if (DeviceManager::IsAscend310P()) {
        MxInit();
        testing::InitGoogleTest(&argc, argv);
        int ret = RUN_ALL_TESTS();
        MxDeInit();
        return ret;
    }
}
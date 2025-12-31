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
 * Create: 2022
 * History: NA
 */

#include <gtest/gtest.h>
#include <vector>
#include "MxBase/MxBase.h"
#include "MxBase/DeviceManager/DeviceManager.h"

namespace {
using namespace MxBase;
class TensorTikTest : public testing::Test {};
const std::vector<uint32_t> SHAPE4 = {1, 32, 32, 3};
const std::vector<std::vector<float>> TRANS_MATRIX = {{2, 2, 2}, {1, 2, 1}};
const float BORDER_VALUE = 10;
const PaddingMode PADDING_MODE = PaddingMode::PADDING_CONST;
const WarpAffineMode WARP_AFFINE_MODE = WarpAffineMode::INTER_LINEAR;

TEST_F(TensorTikTest, TestCaption)
{
    const size_t largeSize = 4096;
    const size_t height = 4096;
    const size_t width = 400;
    const float op = 0.5;
    const size_t channelThree = 3;
    MxBase::Tensor caption(std::vector<uint32_t>{height, width, channelThree}, MxBase::TensorDType::UINT8, 0);
    MxBase::Tensor frame(std::vector<uint32_t>{height, width, channelThree}, MxBase::TensorDType::UINT8, 0);
    MxBase::Tensor captionBg(std::vector<uint32_t>{height, width, channelThree}, MxBase::TensorDType::UINT8, 0);
    MxBase::Tensor captionAlpha(std::vector<uint32_t>{height, width, 1}, MxBase::TensorDType::UINT8, 0);
    MxBase::Tensor captionAlphaFP(std::vector<uint32_t>{height, width, 1}, MxBase::TensorDType::FLOAT16, 0);
    MxBase::Tensor::TensorMalloc(caption);
    MxBase::Tensor::TensorMalloc(frame);
    MxBase::Tensor::TensorMalloc(captionBg);
    MxBase::Tensor::TensorMalloc(captionAlpha);
    auto ret = BlendImageCaption(frame, caption, captionAlpha, captionBg, op);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorTikTest, TestWarpAffine)
{
    const size_t maxH = 3840;
    const size_t maxW = 2160;
    const size_t maxC = 4;
    const size_t minH = 32;
    const size_t maxB = 16;
    const size_t matSize = 6;
    const float color = 255.00001;
    std::vector<std::vector<float>> maxtrix {{0, 0, 0}, {0, 0, 0}};

    MxBase::Tensor src1(std::vector<uint32_t>{1, minH, minH, 1}, MxBase::TensorDType::UINT8, 0);
    MxBase::Tensor dst1;
    auto ret = MxBase::WarpAffineHiper(src1, dst1, maxtrix, (MxBase::PaddingMode) 0, 0, (MxBase::WarpAffineMode) 0);
    EXPECT_NE(ret, APP_ERR_OK);
    MxBase::Tensor::TensorMalloc(src1);
    src1.ToHost();
    ret = MxBase::WarpAffineHiper(src1, dst1, maxtrix, (MxBase::PaddingMode) 0, 0, (MxBase::WarpAffineMode) 0);
    EXPECT_NE(ret, APP_ERR_OK);
    MxBase::Tensor src2(std::vector<uint32_t>{maxB + 1, minH, minH, 1}, MxBase::TensorDType::UINT8, 0);
    MxBase::Tensor::TensorMalloc(src2);
    ret = MxBase::WarpAffineHiper(src2, dst1, maxtrix, (MxBase::PaddingMode) 0, 0, (MxBase::WarpAffineMode) 0);
    EXPECT_NE(ret, APP_ERR_OK);
    MxBase::Tensor src3(std::vector<uint32_t>{1, maxH + 1, minH, 1}, MxBase::TensorDType::UINT8, 0);
    MxBase::Tensor::TensorMalloc(src3);
    ret = MxBase::WarpAffineHiper(src3, dst1, maxtrix, (MxBase::PaddingMode) 0, 0, (MxBase::WarpAffineMode) 0);
    EXPECT_NE(ret, APP_ERR_OK);
    MxBase::Tensor src4(std::vector<uint32_t>{1, minH, maxW + 1, 1}, MxBase::TensorDType::UINT8, 0);
    MxBase::Tensor::TensorMalloc(src4);
    ret = MxBase::WarpAffineHiper(src4, dst1, maxtrix, (MxBase::PaddingMode) 0, 0, (MxBase::WarpAffineMode) 0);
    EXPECT_NE(ret, APP_ERR_OK);
    MxBase::Tensor src5(std::vector<uint32_t>{1, minH, minH, maxC + 1}, MxBase::TensorDType::UINT8, 0);
    MxBase::Tensor::TensorMalloc(src5);
    ret = MxBase::WarpAffineHiper(src5, dst1, maxtrix, (MxBase::PaddingMode) 0, 0, (MxBase::WarpAffineMode) 0);
    EXPECT_NE(ret, APP_ERR_OK);
    src1.ToDvpp(0);
    ret = MxBase::WarpAffineHiper(src1, dst1, maxtrix, (MxBase::PaddingMode) 1, 0, (MxBase::WarpAffineMode) 0);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::WarpAffineHiper(src1, dst1, maxtrix, (MxBase::PaddingMode) 0, 0, (MxBase::WarpAffineMode) 1);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = MxBase::WarpAffineHiper(src1, dst1, maxtrix, (MxBase::PaddingMode) 0, color, (MxBase::WarpAffineMode) 0);
    EXPECT_NE(ret, APP_ERR_OK);
    MxBase::Tensor dst2(std::vector<uint32_t>{1, minH, minH, 1}, MxBase::TensorDType::FLOAT32, 0);
    MxBase::Tensor::TensorMalloc(dst2);
    ret = MxBase::WarpAffineHiper(src1, dst2, maxtrix, (MxBase::PaddingMode) 0, 0, (MxBase::WarpAffineMode) 0);
    EXPECT_NE(ret, APP_ERR_OK);
    MxBase::Tensor dst3(std::vector<uint32_t>{1, minH, minH, maxC + 1}, MxBase::TensorDType::UINT8, 0);
    MxBase::Tensor::TensorMalloc(dst3);
    ret = MxBase::WarpAffineHiper(src1, dst3, maxtrix, (MxBase::PaddingMode) 0, 0, (MxBase::WarpAffineMode) 0);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src;
    Tensor dst;
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor src(SHAPE4, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Success_When_Src_Datatype_Is_Float16)
{
    Tensor src(SHAPE4, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Src_Shape_Size_Is_3)
{
    std::vector<uint32_t> shape = {32, 32, 3};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Src_Dim_N_Is_17)
{
    std::vector<uint32_t> shape = {17, 32, 32, 3};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Src_Dim_H_Exceed_Min)
{
    std::vector<uint32_t> shape = {1, 31, 32, 3};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Src_Dim_H_Exceed_Max)
{
    std::vector<uint32_t> shape = {1, 3841, 32, 3};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Src_Dim_W_Exceed_Min)
{
    std::vector<uint32_t> shape = {1, 32, 31, 3};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Src_Dim_W_Exceed_Max)
{
    std::vector<uint32_t> shape = {1, 32, 2161, 3};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Dst_Is_Host)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE4, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Dst_Datatype_Is_Float16)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE4, TensorDType::FLOAT16, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Dst_Shape_Size_Is_3)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> shape = {32, 32, 3};
    Tensor dst(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Dst_Dim_N_Is_17)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> shape = {17, 32, 32, 3};
    Tensor dst(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Dst_Dim_H_Exceed_Min)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> shape = {1, 31, 32, 3};
    Tensor dst(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Dst_Dim_H_Exceed_Max)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> shape = {1, 3841, 32, 3};
    Tensor dst(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Dst_Dim_W_Exceed_Min)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> shape = {1, 32, 31, 3};
    Tensor dst(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Dst_Dim_W_Exceed_Max)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> shape = {1, 32, 2161, 3};
    Tensor dst(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_TransMatrix_Size_Is_2x2)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    std::vector<std::vector<float>> transMatrix = {{2, 2}, {1, 2}};
    APP_ERROR ret = WarpAffineHiper(src, dst, transMatrix, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Value_Of_TransMatrix_Determinant_Is_0)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    std::vector<std::vector<float>> transMatrix = {{2, 2, 2}, {2, 2, 2}};
    APP_ERROR ret = WarpAffineHiper(src, dst, transMatrix, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_PaddingMode_Is_Not_PADDING_CONST)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    PaddingMode paddingMode = static_cast<PaddingMode>(1);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, paddingMode, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_BorderValue_Exceed_Min)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    float borderValue = -1;
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, borderValue, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_BorderValue_Exceed_Max)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    float borderValue = 256;
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, borderValue, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_WarpAffineMode_Is_Not_INTER_LINEAR)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    WarpAffineMode warpAffineMode = static_cast<WarpAffineMode>(1);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, warpAffineMode);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE4, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest,
    Test_WarpAffineHiper_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_StreamId)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(src);
    Tensor dst;
    AscendStream stream(0);
    stream.CreateAscendStream();
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE, stream);
    stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(TensorTikTest,
    Test_WarpAffineHiper_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_StreamId)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE4, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dst);
    AscendStream stream(0);
    stream.CreateAscendStream();
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE, stream);
    stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Dst_Datatype_Is_Different_With_Src)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst(SHAPE4, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Dst_Dim_N_Is_Different_With_Src)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> shape = {2, 32, 32, 3};
    Tensor dst(shape, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Fail_When_Dst_Dim_C_Is_Different_With_Src)
{
    Tensor src(SHAPE4, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> shape = {1, 32, 32, 1};
    Tensor dst(shape, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = WarpAffineHiper(src, dst, TRANS_MATRIX, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorTikTest, Test_WarpAffineHiper_Should_Return_Success_When_Input_Parameters_OK)
{
    if (DeviceManager::IsAscend310P()) {
        const int dataLen = 1 * 32 * 32 * 3;
        uint8_t data[dataLen] = {0};
        for (int i = 0; i < dataLen; i++) {
            data[i] = 1;
        }
        Tensor src(data, SHAPE4, TensorDType::UINT8);
        src.ToDevice(0);
        Tensor dst;
        std::vector<std::vector<float>> transMatrix = {{0.5, 0.8660254, -5.85640646}, {-0.8660254, 0.5, 21.85640646}};
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = WarpAffineHiper(src, dst, transMatrix, PADDING_MODE, BORDER_VALUE, WARP_AFFINE_MODE, stream);
        stream.Synchronize();
        stream.DestroyAscendStream();
        dst.ToHost();
        ASSERT_EQ(ret, APP_ERR_OK);
        const int expectValue = 10;
        EXPECT_EQ((static_cast<uint8_t*>(dst.GetData()))[0], expectValue);
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
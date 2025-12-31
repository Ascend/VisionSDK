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
#include <mockcpp/mockcpp.hpp>
#include <vector>
#include "acl/dvpp/hi_dvpp.h"
#include "ResourceManager/HAL/AclApi.h"
#include "ResourceManager/DvppPool/DvppPool.h"
#include "MxBase/E2eInfer/TensorOperation/TensorWarping.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/CommonUtils.h"
#include "MxBase/MxBase.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/E2eInfer/Tensor/CallBack.h"

namespace {
    using namespace MxBase;
    constexpr size_t ROTATE_DATA_LEN = 6;
    constexpr size_t WARP_PERSPECTIVE_DATA_LEN = 1024;
    constexpr size_t ROTATE_DATA_LEN2 = 921600;
    constexpr size_t ROTATE_DATA_LEN3 = 307200;
    const std::vector<uint32_t> SHAPE1 = {0x1};
    const std::vector<uint32_t> SHAPE2 = {0x64, 0x64};
    const std::vector<uint32_t> SHAPE3 = {0x1, 0x2, 0x3};
    const std::vector<uint32_t> SHAPE3_DIFFC = {0x1, 0x2, 0x4};
    const std::vector<uint32_t> SHAPE3_FOUR_CHANNEL = {0x100, 0x200, 0x4};
    const std::vector<uint32_t> SHAPE4 = {0x1, 0x2, 0x3, 0x1};
    const std::vector<uint32_t> SHAPE5 = {0x1, 0x2, 0x3, 0x1, 0x1};
    const std::vector<uint32_t> SHAPE6 = {0x512, 0x200, 0x3};
    const std::vector<uint32_t> SHAPE_SINGLE = {0x1, 0x2, 0x1};
    const std::vector<std::vector<float>> DETERMINANT_ZERO_MATRIX = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    const std::vector<std::vector<float>> CHANGE_NOTHING_MATRIX = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    uint8_t g_data1[ROTATE_DATA_LEN] = {0};
    uint8_t g_data2[ROTATE_DATA_LEN] = {0};
    uint8_t g_data3[ROTATE_DATA_LEN] = {0};
    uint8_t g_data4[ROTATE_DATA_LEN2] = {0};
    uint8_t g_data5[ROTATE_DATA_LEN3] = {0};
    constexpr int IMAGE_SIZE = 389;
    constexpr int IMAGE_SIZE_ALIGN = 400;
    constexpr int IMAGE_RECT_ONE = 16;
    constexpr int IMAGE_RECT_TWO = 32;
    constexpr int IMAGE_SHAPE = 640;
    constexpr int CHANNEL = 3;
    constexpr int GRAY_CHANNEL = 1;
    constexpr int RGBA_CHANNEL = 4;
    constexpr int IMAGE_INVLID_WIDTH = 15;
    constexpr int IMAGE_INVLID_HEIGHT = 15;
    constexpr int IMAGE_WIDTH = 480;
    constexpr int IMAGE_HEIGHT = 640;
    constexpr int DATA_LEN = 307200;
    constexpr int CONST_0 = 0;
    constexpr int CONST_6 = 6;
    constexpr int CONST_32 = 32;
    constexpr int CONST_64 = 64;
    constexpr int CONST_128 = 128;
    constexpr int CONST_3000 = 3000;
    class TensorWarpingTest : public testing::Test {
    public:
        void TearDown() override
        {
            // clear mock
            GlobalMockObject::verify();
            LogDebug << "TearDown()";
        }
    };

    template<typename T>
    bool EXPECT_EQ_VECTOR(std::vector<T> t1, std::vector<T> t2)
    {
        return t1 == t2;
    }

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src;
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Src_Is_Host)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Src_Datatype_Is_Float32)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_YuvSrc_Height_Exceed_Min)
{
    const int yuvMinH = 17;
    std::vector<uint32_t> shape = {yuvMinH, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_YuvSrc_Height_Exceed_Max)
{
    const int yuvMaxH = 4097;
    std::vector<uint32_t> shape = {yuvMaxH, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_YuvSrc_Width_Exceed_Min)
{
    const int yuvMinW = 5;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, yuvMinW};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_YuvSrc_Width_Exceed_Max)
{
    const int yuvMaxW = 4097;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, yuvMaxW};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_RgbSrc_Height_Exceed_Min)
{
    const int rgbMinH = 9;
    std::vector<uint32_t> shape = {rgbMinH, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_RgbSrc_Height_Exceed_Max)
{
    const int rgbMaxH = 4097;
    std::vector<uint32_t> shape = {rgbMaxH, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_RgbSrc_Width_Exceed_Min)
{
    const int rgbMinW = 5;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, rgbMinW, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_RgbSrc_Width_Exceed_Max)
{
    const int rgbMaxW = 4097;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, rgbMaxW, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Rgb_Crop_Rect_Height_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    const int cropX1 = 4096;
    const int cropY1 = 4097;
    Rect rect1(CONST_0, CONST_0, cropX1, cropY1);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Rgb_Crop_Rect_Width_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    const int cropX1 = 4097;
    const int cropY1 = 4096;
    Rect rect1(CONST_0, CONST_0, cropX1, cropY1);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Rgb_Crop_Rect_Height_Exceed_Min)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    const int cropX1 = 6;
    const int cropY1 = 9;
    Rect rect1(CONST_0, CONST_0, cropX1, cropY1);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Rgb_Crop_Rect_Width_Exceed_Min)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    const int cropX1 = 5;
    const int cropY1 = 10;
    Rect rect1(CONST_0, CONST_0, cropX1, cropY1);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Yuv_Crop_Rect_Height_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    const int cropX1 = 4096;
    const int cropY1 = 4097;
    Rect rect1(CONST_0, CONST_0, cropX1, cropY1);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Yuv_Crop_Rect_Width_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    const int cropX1 = 4097;
    const int cropY1 = 4096;
    Rect rect1(CONST_0, CONST_0, cropX1, cropY1);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Yuv_Crop_Rect_Height_Exceed_Min)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    const int cropX1 = 6;
    const int cropY1 = 17;
    Rect rect1(CONST_0, CONST_0, cropX1, cropY1);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Yuv_Crop_Rect_Width_Exceed_Min)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    const int cropX1 = 5;
    const int cropY1 = 18;
    Rect rect1(CONST_0, CONST_0, cropX1, cropY1);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Crop_Rect_Height_Exceed_ValidRoi_Height)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    src.SetValidRoi(Rect(CONST_0, CONST_0, CONST_64, CONST_32));
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Crop_Rect_Width_Exceed_ValidRoi_Width)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    src.SetValidRoi(Rect(CONST_0, CONST_0, CONST_32, CONST_64));
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Rgb_Resize_Height_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, IMAGE_WIDTH, IMAGE_WIDTH);
    std::vector<Rect> cropRecVec{rect1};
    const int resizeW = 4096;
    const int resizeH = 4097;
    Size size1(resizeW, resizeH);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Rgb_Resize_Width_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, IMAGE_WIDTH, IMAGE_WIDTH);
    std::vector<Rect> cropRecVec{rect1};
    const int resizeW = 4097;
    const int resizeH = 4096;
    Size size1(resizeW, resizeH);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Rgb_Resize_Height_Exceed_Min)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_32, CONST_32);
    std::vector<Rect> cropRecVec{rect1};
    const int resizeW = 6;
    const int resizeH = 9;
    Size size1(resizeW, resizeH);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Rgb_Resize_Width_Exceed_Min)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_32, CONST_32);
    std::vector<Rect> cropRecVec{rect1};
    const int resizeW = 5;
    const int resizeH = 10;
    Size size1(resizeW, resizeH);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Yuv_Resize_Height_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, IMAGE_WIDTH, IMAGE_WIDTH);
    std::vector<Rect> cropRecVec{rect1};
    const int resizeW = 4097;
    const int resizeH = 4096;
    Size size1(resizeW, resizeH);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Yuv_Resize_Width_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, IMAGE_WIDTH, IMAGE_WIDTH);
    std::vector<Rect> cropRecVec{rect1};
    const int resizeW = 4096;
    const int resizeH = 4097;
    Size size1(resizeW, resizeH);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Yuv_Resize_Height_Exceed_Min)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_32, CONST_32);
    std::vector<Rect> cropRecVec{rect1};
    const int resizeW = 6;
    const int resizeH = 17;
    Size size1(resizeW, resizeH);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Yuv_Resize_Width_Exceed_Min)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_32, CONST_32);
    std::vector<Rect> cropRecVec{rect1};
    const int resizeW = 5;
    const int resizeH = 18;
    Size size1(resizeW, resizeH);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Resize_Ratios_Exceed_Min)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, IMAGE_HEIGHT, IMAGE_HEIGHT);
    std::vector<Rect> cropRecVec{rect1};
    const int resizeW = 19;
    const int resizeH = 19;
    Size size1(resizeW, resizeH);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Resize_Ratios_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    const int cropX1 = 20;
    const int cropY1 = 20;
    Rect rect1(CONST_0, CONST_0, cropX1, cropY1);
    std::vector<Rect> cropRecVec{rect1};
    const int resizeW = 321;
    const int resizeH = 321;
    Size size1(resizeW, resizeH);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_With_Batch_Should_Return_Fail_When_CropRectVec_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    std::vector<Rect> cropRecVec;
    std::vector<Size> sizeVec;
    const uint32_t vecSize = 257;
    for (uint32_t i = 0; i < vecSize; i++) {
        Rect rec(CONST_0, CONST_0, CONST_32, CONST_32);
        Size size(CONST_32, CONST_32);
        cropRecVec.push_back(rec);
        sizeVec.push_back(size);
    }
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Stream)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    AscendStream stream(1);
    stream.CreateAscendStream();
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false, stream);
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> dstShape = {CONST_32, CONST_32, CHANNEL};
    Tensor dstTensor(dstShape, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dstTensor);
    std::vector<Tensor> dst{dstTensor};
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_Stream)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> dstShape = {CONST_32, CONST_32, CHANNEL};
    Tensor dstTensor(dstShape, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dstTensor);
    std::vector<Tensor> dst{dstTensor};
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    AscendStream stream(0);
    stream.CreateAscendStream();
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false, stream);
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Src_ShapeSize_Is_Different_With_Dst)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> dstShape = {CONST_32, CONST_32};
    Tensor dstTensor(dstShape, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dstTensor);
    std::vector<Tensor> dst{dstTensor};
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    AscendStream stream(0);
    stream.CreateAscendStream();
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false, stream);
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Src_ShapeSize_Exceed_Min)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Src_ShapeSize_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Use_UserStream_WithNoThread)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(CONST_32, CONST_32);
    std::vector<Size> sizeVec{size1};
    AscendStream stream(0);
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false, stream);
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Success_When_Dst_IsWithMargin_And_NonkeepMargin)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor dstTensor(dstShape, TensorDType::UINT8, 0);
        dstTensor.SetValidRoi(Rect{0, 0, CONST_32, CONST_32});
        Tensor::TensorMalloc(dstTensor);
        std::vector<Tensor> dst{dstTensor};
        Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
        std::vector<Rect> cropRecVec{rect1};
        Size size1(CONST_32, CONST_32);
        std::vector<Size> sizeVec{size1};
        APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst[0].GetShape(), std::vector<uint32_t>{CONST_32, CONST_32, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Success_When_Dst_IsMemoryReusable)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {CONST_32, CONST_32, CHANNEL};
        Tensor dstTensor(dstShape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dstTensor);
        std::vector<Tensor> dst{dstTensor};
        Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
        std::vector<Rect> cropRecVec{rect1};
        Size size1(CONST_32, CONST_32);
        std::vector<Size> sizeVec{size1};
        APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::HUAWEI_HIGH_ORDER_FILTER, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst[0].GetShape(), std::vector<uint32_t>{CONST_32, CONST_32, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Success_When_Dst_IsNonMemoryReusable_And_keepMargin)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {IMAGE_SIZE, IMAGE_SIZE, CHANNEL};
        Tensor dstTensor(dstShape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dstTensor);
        std::vector<Tensor> dst{dstTensor};
        Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
        std::vector<Rect> cropRecVec{rect1};
        Size size1(IMAGE_SIZE, IMAGE_SIZE);
        std::vector<Size> sizeVec{size1};
        APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::HUAWEI_HIGH_ORDER_FILTER, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst[0].GetShape(),
            std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE_ALIGN, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Success_When_Dst_IsNonMemoryReusable_And_NonkeepMargin)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {IMAGE_SIZE, IMAGE_SIZE, CHANNEL};
        Tensor dstTensor(dstShape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dstTensor);
        std::vector<Tensor> dst{dstTensor};
        Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
        std::vector<Rect> cropRecVec{rect1};
        Size size1(IMAGE_SIZE, IMAGE_SIZE);
        std::vector<Size> sizeVec{size1};
        APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::HUAWEI_HIGH_ORDER_FILTER, false);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst[0].GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Success_When_Use_DefaultStream)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<Tensor> dst;
        Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
        std::vector<Rect> cropRecVec{rect1};
        Size size1(IMAGE_SIZE, IMAGE_SIZE);
        std::vector<Size> sizeVec{size1};
        APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst[0].GetShape(),
            std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE_ALIGN, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Success_When_Use_UserStream_WithThread)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<Tensor> dst;
        Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
        std::vector<Rect> cropRecVec{rect1};
        Size size1(IMAGE_SIZE, IMAGE_SIZE);
        std::vector<Size> sizeVec{size1};
        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream);
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst[0].GetShape(),
            std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE_ALIGN, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Success_When_Dst_Is_Empty_Vector)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<Tensor> dst;
        Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
        std::vector<Rect> cropRecVec{rect1};
        Size size1(IMAGE_SIZE, IMAGE_SIZE);
        std::vector<Size> sizeVec{size1};
        APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst[0].GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_DstVec_Has_Empty_Tensor)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> dst1Shape = {IMAGE_SIZE, IMAGE_SIZE, CHANNEL};
    std::vector<uint32_t> dst2Shape = {IMAGE_SIZE, IMAGE_SIZE, CHANNEL};
    Tensor dst1(dst1Shape, TensorDType::UINT8, 0);
    Tensor dst2(dst2Shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst1);
    std::vector<Tensor> dst{dst1, dst2};
    Rect rect1(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE);
    std::vector<Rect> cropRecVec{rect1, rect1};
    Size size1(IMAGE_SIZE, IMAGE_SIZE);
    std::vector<Size> sizeVec{size1, size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Rec_Size_Invalid)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Tensor> dst;
    Rect rect1(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE);
    std::vector<Rect> cropRecVec{rect1};
    Size size1(IMAGE_SIZE, IMAGE_SIZE);
    std::vector<Size> sizeVec{size1, size1};
    APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Src_Is_Empty)
{
    Tensor src;
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Src_Is_Host)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Src_Datatype_Is_Float32)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_YuvSrc_Height_Exceed_Min)
{
    const int yuvMinH = 17;
    std::vector<uint32_t> shape = {yuvMinH, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_YuvSrc_Height_Exceed_Max)
{
    const int yuvMaxH = 4097;
    std::vector<uint32_t> shape = {yuvMaxH, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_YuvSrc_Width_Exceed_Min)
{
    const int yuvMinW = 5;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, yuvMinW};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_YuvSrc_Width_Exceed_Max)
{
    const int yuvMaxW = 4097;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, yuvMaxW};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_RgbSrc_Height_Exceed_Min)
{
    const int rgbMinH = 9;
    std::vector<uint32_t> shape = {rgbMinH, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_RgbSrc_Height_Exceed_Max)
{
    const int rgbMaxH = 4097;
    std::vector<uint32_t> shape = {rgbMaxH, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_RgbSrc_Width_Exceed_Min)
{
    const int rgbMinW = 5;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, rgbMinW, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_RgbSrc_Width_Exceed_Max)
{
    const int rgbMaxW = 4097;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, rgbMaxW, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Rgb_Crop_Rect_Height_Exceed_Max)
{
    const int cropX1 = 4096;
    const int cropY1 = 4097;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, cropX1, cropY1), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Rgb_Crop_Rect_Width_Exceed_Max)
{
    const int cropX1 = 4097;
    const int cropY1 = 4096;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, cropX1, cropY1), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Rgb_Crop_Rect_Height_Exceed_Min)
{
    const int cropX1 = 6;
    const int cropY1 = 9;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, cropX1, cropY1), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Rgb_Crop_Rect_Width_Exceed_Min)
{
    const int cropX1 = 5;
    const int cropY1 = 10;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, cropX1, cropY1), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Yuv_Crop_Rect_Height_Exceed_Max)
{
    const int cropX1 = 4096;
    const int cropY1 = 4097;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, cropX1, cropY1), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Yuv_Crop_Rect_Width_Exceed_Max)
{
    const int cropX1 = 4097;
    const int cropY1 = 4096;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, cropX1, cropY1), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Yuv_Crop_Rect_Height_Exceed_Min)
{
    const int cropX1 = 6;
    const int cropY1 = 17;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, cropX1, cropY1), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Yuv_Crop_Rect_Width_Exceed_Min)
{
    const int cropX1 = 5;
    const int cropY1 = 18;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, cropX1, cropY1), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Crop_Rect_Height_Exceed_ValidRoi_Height)
{
    const int validX1 = 64;
    const int validY1 = 32;
    const int cropX1 = 64;
    const int cropY1 = 64;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    src.SetValidRoi(Rect(CONST_0, CONST_0, validX1, validY1));
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, cropX1, cropY1), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Crop_Rect_Width_Exceed_ValidRoi_Width)
{
    const int validX1 = 32;
    const int validY1 = 64;
    const int cropX1 = 64;
    const int cropY1 = 64;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    src.SetValidRoi(Rect(CONST_0, CONST_0, validX1, validY1));
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, cropX1, cropY1), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Stream)
{
    AscendStream stream(1);
    stream.CreateAscendStream();
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> dstShape = {CONST_64, CONST_64, CHANNEL};
    Tensor dst(dstShape, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_Stream)
{
    AscendStream stream(1);
    stream.CreateAscendStream();
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> dstShape = {CONST_64, CONST_64, CHANNEL};
    Tensor dst(dstShape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Src_ShapeSize_Is_Different_With_Dst)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> dstShape = {CONST_64, CONST_64, CHANNEL};
    Tensor dst(dstShape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Src_ShapeSize_Exceed_Min)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Src_ShapeSize_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Fail_When_Use_UserStream_WithNoThread)
{
    AscendStream stream(0);
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, CONST_64, CONST_64), dst, true, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
    stream.DestroyAscendStream();
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Success_When_Dst_IsWithMargin_And_NonkeepMargin)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL};
        Tensor dst(dstShape, TensorDType::UINT8, 0);
        dst.SetValidRoi(Rect{0, 0, IMAGE_SIZE, IMAGE_SIZE});
        Tensor::TensorMalloc(dst);
        APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE), dst, false);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Success_When_Dst_IsMemoryReusable)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor dst(dstShape, TensorDType::UINT8, 0, true);
        Tensor::TensorMalloc(dst);
        APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, IMAGE_HEIGHT, IMAGE_WIDTH), dst, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Success_When_Dst_IsNonMemoryReusable_And_keepMargin)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL};
        Tensor dst(dstShape, TensorDType::UINT8, 0);
        dst.SetValidRoi(Rect{0, 0, IMAGE_SIZE, IMAGE_SIZE});
        Tensor::TensorMalloc(dst);
        APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE), dst, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(),
            std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE_ALIGN, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Success_When_Dst_IsNonMemoryReusable_And_NonkeepMargin)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL};
        Tensor dst(dstShape, TensorDType::UINT8, 0);
        dst.SetValidRoi(Rect{0, 0, IMAGE_SIZE, IMAGE_SIZE});
        Tensor::TensorMalloc(dst);
        APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE), dst, false);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(),
            std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Success_When_Use_DefaultStream)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE), dst, false);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(),
            std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Crop_Should_Return_Success_When_Use_UserStream_WithThread)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream;
        stream.CreateAscendStream();
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        APP_ERROR ret = Crop(src, Rect(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE), dst, false, stream);
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(),
            std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Crop_With_Batch_Should_Return_Success_When_Dst_Is_Empty_Vector)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<Tensor> dst;
        Rect rec1(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE);
        Rect rec2(CONST_0, CONST_0, IMAGE_SIZE_ALIGN, IMAGE_SIZE_ALIGN);
        APP_ERROR ret = Crop(src, std::vector<Rect>{rec1, rec2}, dst, false);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst[0].GetShape(),
            std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst[1].GetShape(),
            std::vector<uint32_t>{IMAGE_SIZE_ALIGN, IMAGE_SIZE_ALIGN, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Crop_With_Batch_Should_Return_Fail_When_DstVec_Has_Empty_Tensor)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Rect rect1(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE);
    Rect rect2(CONST_0, CONST_0, IMAGE_SIZE_ALIGN, IMAGE_SIZE_ALIGN);
    std::vector<uint32_t> dst1Shape = {IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL};
    std::vector<uint32_t> dst2Shape = {IMAGE_SIZE_ALIGN, IMAGE_SIZE_ALIGN, CHANNEL};
    Tensor dst1(dst1Shape, TensorDType::UINT8, 0);
    Tensor dst2(dst2Shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst1);
    std::vector<Tensor> dst{dst1, dst2};
    APP_ERROR ret = Crop(src, std::vector<Rect>{rect1, rect2}, dst, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_With_Batch_Should_Return_Fail_When_CropRectVec_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<Rect> rectVec;
    const uint32_t rectVecSize = 257;
    for (uint32_t i = 0; i < rectVecSize; i++) {
        Rect rec{0, 0, i, i};
        rectVec.push_back(rec);
    }
    std::vector<Tensor> dst;
    APP_ERROR ret = Crop(src, rectVec, dst, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_With_Batch_Should_Return_Fail_When_DstVec_Size_Invalid)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Rect rect1(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE);
    Rect rect2(CONST_0, CONST_0, IMAGE_SIZE_ALIGN, IMAGE_SIZE_ALIGN);
    std::vector<uint32_t> dst1Shape = {IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL};
    std::vector<uint32_t> dst2Shape = {IMAGE_SIZE_ALIGN, IMAGE_SIZE_ALIGN, CHANNEL};
    std::vector<uint32_t> dst3Shape = {IMAGE_SIZE_ALIGN, IMAGE_SIZE_ALIGN, CHANNEL};
    Tensor dst1(dst1Shape, TensorDType::UINT8, 0);
    Tensor dst2(dst2Shape, TensorDType::UINT8, 0);
    Tensor dst3(dst3Shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst1);
    Tensor::TensorMalloc(dst2);
    Tensor::TensorMalloc(dst3);
    std::vector<Tensor> dst{dst1, dst2, dst3};
    APP_ERROR ret = Crop(src, std::vector<Rect>{rect1, rect2}, dst, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Crop_With_Batch_Should_Return_Success_When_Use_DefaultStream)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<Tensor> dst;
        Rect rec1(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE);
        APP_ERROR ret = Crop(src, std::vector<Rect>{rec1}, dst, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst[0].GetShape(),
            std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE_ALIGN, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Crop_With_Batch_Should_Return_Success_When_Use_UserStream_WithThread)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream;
        stream.CreateAscendStream();
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<Tensor> dst;
        Rect rec1(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE);
        APP_ERROR ret = Crop(src, std::vector<Rect>{rec1}, dst, false, stream);
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst[0].GetShape(),
            std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
    }
}


TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_Src_Is_Empty)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src;
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::BILINEAR_SIMILAR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_Src_Is_Host)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, -1);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_Src_Datatype_Is_Float32)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::FLOAT32, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_YuvSrc_Height_Exceed_Min)
{
    const int yuvMinH = 5;
    std::vector<uint32_t> shape = {yuvMinH, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_YuvSrc_Height_Exceed_Max)
{
    const int yuvMaxH = 4097;
    std::vector<uint32_t> shape = {yuvMaxH, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_YuvSrc_Width_Exceed_Min)
{
    const int yuvMinW = 17;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, yuvMinW};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_YuvSrc_Width_Exceed_Max)
{
    const int yuvMaxW = 4097;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, yuvMaxW};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_RgbSrc_Height_Exceed_Min)
{
    const int yuvMinH = 5;
    std::vector<uint32_t> shape = {yuvMinH, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_RgbSrc_Height_Exceed_Max)
{
    const int yuvMaxH = 4097;
    std::vector<uint32_t> shape = {yuvMaxH, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_RgbSrc_Width_Exceed_Min)
{
    const int yuvMinW = 9;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, yuvMinW, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_RgbSrc_Width_Exceed_Max)
{
    const int yuvMaxW = 4097;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, yuvMaxW, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_RgbResize_Height_Exceed_Max)
{
    const int resizeH = 4097;
    const int resizeW = 4096;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_RgbResize_Width_Exceed_Max)
{
    const int resizeH = 4096;
    const int resizeW = 4097;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_RgbResizeH_Exceed_Min)
{
    const int resizeH = 9;
    const int resizeW = 6;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_RgbResizeW_Exceed_Min)
{
    const int resizeH = 10;
    const int resizeW = 5;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_YuvResizeH_Exceed_Max)
{
    const int resizeH = 4097;
    const int resizeW = 4096;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_YuvResizeW_Exceed_Max)
{
    const int resizeH = 4096;
    const int resizeW = 4097;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_YuvResizeH_Exceed_Min)
{
    const int resizeH = 17;
    const int resizeW = 6;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_YuvResizeW_Exceed_Min)
{
    const int resizeH = 18;
    const int resizeW = 5;
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_ResizeRatios_Exceed_Min)
{
    const int resizeH = 19;
    const int resizeW = 19;
    std::vector<uint32_t> shape = {IMAGE_SHAPE, IMAGE_SHAPE};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_ResizeRatios_Exceed_Max)
{
    const int srcH = 20;
    const int srcW = 20;
    const int resizeH = 641;
    const int resizeW = 641;
    std::vector<uint32_t> shape = {srcH, srcH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(resizeW, resizeH), Interpolation::NEAREST_NEIGHBOR_OPENCV, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> dstshape = {IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL};
    Tensor dst(dstshape, TensorDType::UINT8, 1);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::BILINEAR_SIMILAR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_Src_ShapeSize_Is_Different_With_Dst)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    std::vector<uint32_t> dstshape = {IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL};
    Tensor dst(dstshape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(dst);
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::BILINEAR_SIMILAR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_Src_ShapeSize_Exceed_Min)
{
    std::vector<uint32_t> shape = {IMAGE_HEIGHT};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::BILINEAR_SIMILAR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_Src_ShapeSize_Exceed_Max)
{
    std::vector<uint32_t> shape = {IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(src);
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::BILINEAR_SIMILAR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Src_Is_RGBA_And_Using_Bilinear)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, RGBA_CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::BILINEAR_SIMILAR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Src_Is_RGBA_And_Using_Bilinear_Async)
{
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, RGBA_CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    src.Malloc();
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::BILINEAR_SIMILAR_OPENCV, true);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Src_Is_RGB_Set_Margin_false_Async)
{
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_SIZE_ALIGN, CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    src.Malloc();
    src.ToDevice(0);
    Tensor dst({IMAGE_SHAPE, IMAGE_SIZE, CHANNEL}, TensorDType::UINT8, -1, false);
    dst.Malloc();
    dst.ToDevice(0);
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::BILINEAR_SIMILAR_OPENCV, false);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Src_Is_RGBA_And_Using_NearestNeighbor)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, RGBA_CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Stream)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        AscendStream stream(1);
        stream.CreateAscendStream();
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE),
                               Interpolation::BILINEAR_SIMILAR_OPENCV, true, stream);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE),
                     Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream);
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_Stream)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 1);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL};
        Tensor dst(dstShape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        AscendStream stream(1);
        stream.CreateAscendStream();
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE),
                               Interpolation::BILINEAR_SIMILAR_OPENCV, true, stream);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE),
                     Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream);
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_Use_UserStream_WithNoThread)
{
    if (DeviceManager::IsAtlas800IA2()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        AscendStream stream(0);
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE),
                               Interpolation::BILINEAR_SIMILAR_OPENCV, true, stream);
        EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
        ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE),
                     Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream);
        EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Dst_IsWithMargin_And_NonkeepMargin)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {IMAGE_SIZE, IMAGE_SIZE, CHANNEL};
        Tensor dst(dstShape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE),
                               Interpolation::BILINEAR_SIMILAR_OPENCV, false);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}),
                  true);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Dst_IsMemoryReusable)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL};
        Tensor dst(dstShape, TensorDType::UINT8, 0, true);
        Tensor::TensorMalloc(dst);
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE),
                               Interpolation::BILINEAR_SIMILAR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL}),
                  true);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Dst_IsNonMemoryReusable_And_keepMargin)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {IMAGE_SIZE, IMAGE_SIZE, CHANNEL};
        Tensor dst(dstShape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE),
                               Interpolation::BILINEAR_SIMILAR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Dst_IsNonMemoryReusable_And_NonkeepMargin)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {IMAGE_SIZE, IMAGE_SIZE, CHANNEL};
        Tensor dst(dstShape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(dst);
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE),
                               Interpolation::BILINEAR_SIMILAR_OPENCV, false);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Use_DefaultStream)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE),
                               Interpolation::BILINEAR_SIMILAR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Use_UserStream_WithThread)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;

        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE),
                               Interpolation::BILINEAR_SIMILAR_OPENCV, true, stream);
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_TensorDtype_Is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;

        AscendStream stream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE),
                               Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream);
        stream.DestroyAscendStream();
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Src_Is_Gray)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, GRAY_CHANNEL};
        Tensor src(shape, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE),
                               Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, GRAY_CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Src_Is_With_Roi)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(g_data4, shape, TensorDType::FLOAT16, 0);
        Rect rect(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE);
        src = Tensor(src, rect);
        Tensor::TensorMalloc(src);
        Tensor dst;
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE),
                               Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
        ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE),
                     Interpolation::BILINEAR_SIMILAR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Src_Is_With_Roi_GRAY)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, GRAY_CHANNEL};
        Tensor src(g_data5, shape, TensorDType::FLOAT16, 0);
        Rect rect(CONST_0, CONST_0, IMAGE_SIZE, IMAGE_SIZE);
        src = Tensor(src, rect);
        Tensor::TensorMalloc(src);
        Tensor dst;
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE),
                               Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, GRAY_CHANNEL}), true);
        ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE),
                     Interpolation::BILINEAR_SIMILAR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(dst.GetShape(), std::vector<uint32_t>{IMAGE_SIZE, IMAGE_SIZE, GRAY_CHANNEL}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Failed_When_Src_Is_With_Invalid_Roi)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(g_data4, shape, TensorDType::FLOAT16, 0);
        Rect rect(CONST_0, CONST_0, CONST_6, CONST_6);
        src = Tensor(src, rect);
        Tensor::TensorMalloc(src);
        Tensor dst;
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE),
                               Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        ret = Resize(src, dst, Size(IMAGE_SIZE, IMAGE_SIZE),
                     Interpolation::BILINEAR_SIMILAR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(TensorWarpingTest, TestResizePaste)
{
    ImageProcessor imageProcessor(0);
    std::vector<uint32_t> shape = { IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL };

    Tensor tensor(shape, TensorDType::UINT8);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    tensor.ToDvpp(0);

    Tensor inputPicsTensor2(shape, TensorDType::UINT8);
    Tensor inputPicsTensor3(shape, TensorDType::UINT8);
    ret = Tensor::TensorMalloc(inputPicsTensor2);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = Tensor::TensorMalloc(inputPicsTensor3);
    EXPECT_EQ(ret, APP_ERR_OK);
    inputPicsTensor2.ToDvpp(0);
    inputPicsTensor3.ToDvpp(0);
    std::vector<Tensor> inputPics = {inputPicsTensor2, inputPicsTensor3};

    Rect rect2(IMAGE_RECT_ONE, IMAGE_RECT_TWO, IMAGE_RECT_ONE, IMAGE_RECT_TWO);
    Rect rect3(IMAGE_RECT_TWO, IMAGE_RECT_ONE, IMAGE_RECT_TWO, IMAGE_RECT_ONE);
    std::vector<Rect> pasteRects = {rect2, rect3};

    Tensor outputTensor;
    ret = ResizePaste(tensor, inputPics, pasteRects, outputTensor, false);
    if (!DeviceManager::IsAscend310P()) {
        EXPECT_NE(ret, APP_ERR_OK);
    }

    AscendStream stream(0);
    stream.CreateAscendStream();
    ret = ResizePaste(tensor, inputPics, pasteRects, outputTensor, false, stream);
    if (!DeviceManager::IsAscend310P()) {
        EXPECT_NE(ret, APP_ERR_OK);
    }
    stream.Synchronize();
    stream.DestroyAscendStream();
}

TEST_F(TensorWarpingTest, Test_ResizePaste_Should_Fail_When_Resize_Ratio_Wrong_Sync)
{
    ImageProcessor imageProcessor(0);
    std::vector<uint32_t> shape = { IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL };

    Tensor tensor(shape, TensorDType::UINT8);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    tensor.ToDvpp(0);

    Tensor inputPicsTensor2(shape, TensorDType::UINT8);
    Tensor inputPicsTensor3(shape, TensorDType::UINT8);
    ret = Tensor::TensorMalloc(inputPicsTensor2);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = Tensor::TensorMalloc(inputPicsTensor3);
    EXPECT_EQ(ret, APP_ERR_OK);
    inputPicsTensor2.ToDvpp(0);
    inputPicsTensor3.ToDvpp(0);
    std::vector<Tensor> inputPics = {inputPicsTensor2, inputPicsTensor3};

    Rect rect2(IMAGE_RECT_ONE, IMAGE_RECT_ONE, IMAGE_RECT_TWO, IMAGE_RECT_TWO);
    Rect rect3(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> pasteRects = {rect2, rect3};

    Tensor outputTensor;
    ret = ResizePaste(tensor, inputPics, pasteRects, outputTensor, false);
    if (!DeviceManager::IsAscend310P()) {
        EXPECT_NE(ret, APP_ERR_OK);
    }
}

TEST_F(TensorWarpingTest, Test_ResizePaste_Should_Fail_When_Resize_Ratio_Wrong_Async)
{
    ImageProcessor imageProcessor(0);
    std::vector<uint32_t> shape = { IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL };

    Tensor tensor(shape, TensorDType::UINT8);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    tensor.ToDvpp(0);

    Tensor inputPicsTensor2(shape, TensorDType::UINT8);
    Tensor inputPicsTensor3(shape, TensorDType::UINT8);
    ret = Tensor::TensorMalloc(inputPicsTensor2);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = Tensor::TensorMalloc(inputPicsTensor3);
    EXPECT_EQ(ret, APP_ERR_OK);
    inputPicsTensor2.ToDvpp(0);
    inputPicsTensor3.ToDvpp(0);
    std::vector<Tensor> inputPics = {inputPicsTensor2, inputPicsTensor3};

    Rect rect2(IMAGE_RECT_ONE, IMAGE_RECT_ONE, IMAGE_RECT_TWO, IMAGE_RECT_TWO);
    Rect rect3(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> pasteRects = {rect2, rect3};

    Tensor outputTensor;
    AscendStream stream(0);
    stream.CreateAscendStream();
    ret = ResizePaste(tensor, inputPics, pasteRects, outputTensor, false, stream);
    if (!DeviceManager::IsAscend310P()) {
        EXPECT_NE(ret, APP_ERR_OK);
    }
    stream.Synchronize();
    stream.DestroyAscendStream();
}

TEST_F(TensorWarpingTest, Test_ResizePaste_Should_Fail_When_Resize_Ratio_Wrong_And_Output_Is_Not_Null)
{
    ImageProcessor imageProcessor(0);
    std::vector<uint32_t> shape = { IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL };

    Tensor tensor(shape, TensorDType::UINT8);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    tensor.ToDvpp(0);

    Tensor inputPicsTensor2(shape, TensorDType::UINT8);
    Tensor inputPicsTensor3(shape, TensorDType::UINT8);
    ret = Tensor::TensorMalloc(inputPicsTensor2);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = Tensor::TensorMalloc(inputPicsTensor3);
    EXPECT_EQ(ret, APP_ERR_OK);
    inputPicsTensor2.ToDvpp(0);
    inputPicsTensor3.ToDvpp(0);
    std::vector<Tensor> inputPics = {inputPicsTensor2, inputPicsTensor3};

    Rect rect2(IMAGE_RECT_ONE, IMAGE_RECT_ONE, IMAGE_RECT_TWO, IMAGE_RECT_TWO);
    Rect rect3(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> pasteRects = {rect2, rect3};

    Tensor outputTensor;
    ret = ResizePaste(tensor, inputPics, pasteRects, outputTensor, false);
    if (!DeviceManager::IsAscend310P()) {
        EXPECT_NE(ret, APP_ERR_OK);
    }

    AscendStream stream(0);
    stream.CreateAscendStream();
    ret = ResizePaste(tensor, inputPics, pasteRects, outputTensor, false, stream);
    if (!DeviceManager::IsAscend310P()) {
        EXPECT_NE(ret, APP_ERR_OK);
    }
    stream.Synchronize();
    stream.DestroyAscendStream();
}

TEST_F(TensorWarpingTest, Test_ResizePaste_Should_Success_Sync)
{
    ImageProcessor imageProcessor(0);
    std::vector<uint32_t> shape = { IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL };

    Tensor tensor(shape, TensorDType::UINT8);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    tensor.ToDvpp(0);

    Tensor inputPicsTensor2(shape, TensorDType::UINT8);
    Tensor inputPicsTensor3(shape, TensorDType::UINT8);
    ret = Tensor::TensorMalloc(inputPicsTensor2);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = Tensor::TensorMalloc(inputPicsTensor3);
    EXPECT_EQ(ret, APP_ERR_OK);
    inputPicsTensor2.ToDvpp(0);
    inputPicsTensor3.ToDvpp(0);
    std::vector<Tensor> inputPics = {inputPicsTensor2, inputPicsTensor3};

    Rect rect2(CONST_0, CONST_0, CONST_128, CONST_128);
    Rect rect3(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> pasteRects = {rect2, rect3};

    Tensor outputTensor;
    ret = ResizePaste(tensor, inputPics, pasteRects, outputTensor, false);
    if (!DeviceManager::IsAscend310P()) {
        EXPECT_NE(ret, APP_ERR_OK);
    }
}

TEST_F(TensorWarpingTest, Test_ResizePaste_Should_Success_When_Output_is_Not_None)
{
    ImageProcessor imageProcessor(0);
    std::vector<uint32_t> shape = { IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL };

    Tensor tensor(shape, TensorDType::UINT8);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    tensor.ToDvpp(0);

    Tensor inputPicsTensor2(shape, TensorDType::UINT8);
    Tensor inputPicsTensor3(shape, TensorDType::UINT8);
    ret = Tensor::TensorMalloc(inputPicsTensor2);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = Tensor::TensorMalloc(inputPicsTensor3);
    EXPECT_EQ(ret, APP_ERR_OK);
    inputPicsTensor2.ToDvpp(0);
    inputPicsTensor3.ToDvpp(0);
    std::vector<Tensor> inputPics = {inputPicsTensor2, inputPicsTensor3};

    Rect rect2(CONST_0, CONST_0, CONST_128, CONST_128);
    Rect rect3(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> pasteRects = {rect2, rect3};

    Tensor outputTensor;
    ret = ResizePaste(tensor, inputPics, pasteRects, outputTensor, false);
    if (!DeviceManager::IsAscend310P()) {
        EXPECT_NE(ret, APP_ERR_OK);
    }

    AscendStream stream(0);
    stream.CreateAscendStream();
    ret = ResizePaste(tensor, inputPics, pasteRects, outputTensor, false, stream);
    if (!DeviceManager::IsAscend310P()) {
        EXPECT_NE(ret, APP_ERR_OK);
    }
    stream.Synchronize();
    stream.DestroyAscendStream();
}

TEST_F(TensorWarpingTest, Test_ResizePaste_Should_Success_Async)
{
    ImageProcessor imageProcessor(0);
    std::vector<uint32_t> shape = { IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL };

    Tensor tensor(shape, TensorDType::UINT8);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    EXPECT_EQ(ret, APP_ERR_OK);
    tensor.ToDvpp(0);

    Tensor inputPicsTensor2(shape, TensorDType::UINT8);
    Tensor inputPicsTensor3(shape, TensorDType::UINT8);
    ret = Tensor::TensorMalloc(inputPicsTensor2);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = Tensor::TensorMalloc(inputPicsTensor3);
    EXPECT_EQ(ret, APP_ERR_OK);
    inputPicsTensor2.ToDvpp(0);
    inputPicsTensor3.ToDvpp(0);
    std::vector<Tensor> inputPics = {inputPicsTensor2, inputPicsTensor3};

    Rect rect2(CONST_0, CONST_0, CONST_128, CONST_128);
    Rect rect3(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> pasteRects = {rect2, rect3};

    Tensor outputTensor;
    ret = ResizePaste(tensor, inputPics, pasteRects, outputTensor, false);
    if (!DeviceManager::IsAscend310P()) {
    EXPECT_NE(ret, APP_ERR_OK);
    }

    AscendStream stream(0);
    stream.CreateAscendStream();
    ret = ResizePaste(tensor, inputPics, pasteRects, outputTensor, false, stream);
    if (!DeviceManager::IsAscend310P()) {
    EXPECT_NE(ret, APP_ERR_OK);
    }
    stream.Synchronize();
    stream.DestroyAscendStream();
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_TensorDType_Is_Undefined)
{
    Tensor tensor1;
    Tensor tensor2;
    APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Src_Is_Host)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    Tensor tensor2;
    APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Dst)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor2.ToDevice(1);
    APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Src_ShapeSize_Is_Different_With_Dst)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2(&g_data1, SHAPE4, TensorDType::UINT8);
    tensor2.ToDevice(0);
    APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Src_Shape_Is_Different_With_Dst)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2(&g_data1, SHAPE3_DIFFC, TensorDType::UINT8);
    tensor2.ToDevice(0);
    APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Src_ShapeSize_Exceed_Min)
{
    Tensor tensor1(&g_data1, SHAPE1, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2;
    APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Src_ShapeSize_Exceed_Max)
{
    Tensor tensor1(&g_data1, SHAPE4, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2;
    APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Src_DeviceId_Is_Different_With_Stream)
{
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2;
    MxBase::AscendStream stream = MxBase::AscendStream(1);
    stream.CreateAscendStream();
    APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    stream.DestroyAscendStream();
}

TEST_F(TensorWarpingTest,
    Test_Rotate_Should_Return_Fail_When_Dst_DeviceId_Is_Different_With_Stream)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
        tensor1.ToDevice(1);
        Tensor tensor2(&g_data1, SHAPE3, TensorDType::UINT8);
        tensor2.ToDevice(0);
        MxBase::AscendStream stream = MxBase::AscendStream(1);
        stream.CreateAscendStream();
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
        stream.DestroyAscendStream();
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Success_When_Rotate_90)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
        tensor1.ToDevice(0);
        Tensor tensor2;
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(tensor2.GetShape(), std::vector<uint32_t>{0x2, 0x1, 0x3}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Reverse_aclCreateIntArray_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(&Transpose).stubs().will(returnValue(0));
        aclIntArray *axisArray = nullptr;
        MOCKER_CPP(AclApi::aclCreateIntArray).stubs().will(returnValue(axisArray));
        Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
        tensor1.ToDevice(0);
        Tensor tensor2;
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
        EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Reverse_aclnnFlipGetWorkspaceSize_Fail)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(&Transpose).stubs().will(returnValue(0));
        MOCKER_CPP(AclApi::aclnnFlipGetWorkspaceSize).stubs().will(returnValue(1));
        Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
        tensor1.ToDevice(0);
        Tensor tensor2;
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
        EXPECT_EQ(ret, 1);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Success_When_Rotate_90_Mock_Atlas800IA2)
{
    MOCKER_CPP(DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    MOCKER_CPP(DeviceManager::IsAtlas800IA2).stubs().will(returnValue(true));
    MOCKER_CPP(&hi_mpi_vpc_rotate).stubs().will(returnValue(0));
    MOCKER_CPP(&hi_mpi_vpc_get_process_result).stubs().will(returnValue(0));
    Tensor tensor1(&g_data5, SHAPE6, TensorDType::UINT8, 0);
    tensor1.ToDevice(0);
    Tensor tensor2(SHAPE6, TensorDType::UINT8, 0);
    APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_180);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Success_When_Rotate_180)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
        tensor1.ToDevice(0);
        Tensor tensor2;
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_180);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(tensor2.GetShape(), std::vector<uint32_t>{0x1, 0x2, 0x3}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Success_When_Rotate_270)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
        tensor1.ToDevice(0);
        Tensor tensor2;
        MxBase::AscendStream stream = MxBase::AscendStream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_270, stream);
        EXPECT_EQ(ret, APP_ERR_OK);
        stream.DestroyAscendStream();
        EXPECT_EQ(EXPECT_EQ_VECTOR(tensor2.GetShape(), std::vector<uint32_t>{0x2, 0x1, 0x3}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Success_When_TensorDType_Is_Float32)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        MxBase::AscendStream stream = MxBase::AscendStream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90, stream);
        EXPECT_EQ(ret, APP_ERR_OK);
        stream.DestroyAscendStream();
        EXPECT_EQ(EXPECT_EQ_VECTOR(tensor2.GetShape(), std::vector<uint32_t>{0x2, 0x1, 0x3}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Success_When_TensorDType_Is_Float16)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        MxBase::AscendStream stream = MxBase::AscendStream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_270, stream);
        EXPECT_EQ(ret, APP_ERR_OK);
        stream.DestroyAscendStream();
        EXPECT_EQ(EXPECT_EQ_VECTOR(tensor2.GetShape(), std::vector<uint32_t>{0x2, 0x1, 0x3}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Success_When_Rotate_90_Four_Channel)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3_FOUR_CHANNEL, TensorDType::UINT8);
        Tensor::TensorMalloc(tensor1);
        tensor1.ToDevice(0);
        Tensor tensor2;
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(tensor2.GetShape(), std::vector<uint32_t>{0x200, 0x100, 0x4}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Success_When_Rotate_180_Four_Channel)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3_FOUR_CHANNEL, TensorDType::UINT8);
        Tensor::TensorMalloc(tensor1);
        tensor1.ToDevice(0);
        Tensor tensor2;
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_180);
        EXPECT_EQ(ret, APP_ERR_OK);
        EXPECT_EQ(EXPECT_EQ_VECTOR(tensor2.GetShape(), std::vector<uint32_t>{0x100, 0x200, 0x4}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Success_When_Rotate_270_Four_Channel)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3_FOUR_CHANNEL, TensorDType::UINT8);
        Tensor::TensorMalloc(tensor1);
        tensor1.ToDevice(0);
        Tensor tensor2;
        MxBase::AscendStream stream = MxBase::AscendStream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_270, stream);
        EXPECT_EQ(ret, APP_ERR_OK);
        stream.DestroyAscendStream();
        EXPECT_EQ(EXPECT_EQ_VECTOR(tensor2.GetShape(), std::vector<uint32_t>{0x200, 0x100, 0x4}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Success_When_TensorDType_Is_Float32_Four_Channel)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3_FOUR_CHANNEL, TensorDType::FLOAT32, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        MxBase::AscendStream stream = MxBase::AscendStream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90, stream);
        EXPECT_EQ(ret, APP_ERR_OK);
        stream.DestroyAscendStream();
        EXPECT_EQ(EXPECT_EQ_VECTOR(tensor2.GetShape(), std::vector<uint32_t>{0x200, 0x100, 0x4}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Success_When_TensorDType_Is_Float16_Four_Channel)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(SHAPE3_FOUR_CHANNEL, TensorDType::FLOAT16, 0);
        Tensor::TensorMalloc(tensor1);
        Tensor tensor2;
        MxBase::AscendStream stream = MxBase::AscendStream(0);
        stream.CreateAscendStream();
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_270, stream);
        EXPECT_EQ(ret, APP_ERR_OK);
        stream.DestroyAscendStream();
        EXPECT_EQ(EXPECT_EQ_VECTOR(tensor2.GetShape(), std::vector<uint32_t>{0x200, 0x100, 0x4}), true);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Failed_Mock_310B_isGray_And_Illegal_resizeW)
{
    MOCKER_CPP(DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    Tensor tensor1({IMAGE_SIZE, IMAGE_SIZE_ALIGN, GRAY_CHANNEL}, TensorDType::FLOAT16, 0);
    tensor1.Malloc();
    tensor1.SetTensorValue(0.2f, true);
    tensor1.SetValidRoi(Rect(CONST_0, CONST_0, CONST_64, CONST_32));
    Size size1(IMAGE_INVLID_WIDTH, IMAGE_INVLID_WIDTH);
    Tensor dst;
    APP_ERROR ret = Resize(tensor1, dst, Size(IMAGE_INVLID_WIDTH, IMAGE_INVLID_WIDTH),
                           Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Failed_Mock_310B_isGray_And_Illegal_resize_Ratio)
{
    MOCKER_CPP(DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    Tensor tensor1({IMAGE_SIZE, IMAGE_SIZE_ALIGN, GRAY_CHANNEL}, TensorDType::FLOAT16, 0);
    tensor1.Malloc();
    tensor1.SetTensorValue(0.2f, true);
    tensor1.SetValidRoi(Rect(CONST_0, CONST_0, CONST_64, CONST_32));
    Tensor dst;
    APP_ERROR ret = Resize(tensor1, dst, Size(CONST_3000, CONST_128), Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Failed_Mock_800IA2_isFloat16)
{
    MOCKER_CPP(DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    MOCKER_CPP(DeviceManager::IsAtlas800IA2).stubs().will(returnValue(true));
    Tensor tensor1({IMAGE_SIZE, IMAGE_SIZE_ALIGN, CHANNEL}, TensorDType::FLOAT16, 0);
    tensor1.Malloc();
    tensor1.SetTensorValue(0.2f, true);
    tensor1.SetValidRoi(Rect(CONST_0, CONST_0, CONST_64, CONST_32));
    Tensor dst;
    APP_ERROR ret = Resize(tensor1, dst, Size(CONST_128, CONST_128), Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Failed_Mock_800IA2_isRGBA)
{
    MOCKER_CPP(DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    MOCKER_CPP(DeviceManager::IsAtlas800IA2).stubs().will(returnValue(true));
    Tensor tensor1({IMAGE_SIZE, IMAGE_SIZE_ALIGN, RGBA_CHANNEL}, TensorDType::UINT8, 0);
    tensor1.Malloc();
    uint8_t value = 1;
    tensor1.SetTensorValue(value);
    tensor1.SetValidRoi(Rect(CONST_0, CONST_0, CONST_64, CONST_32));
    Tensor dst;
    APP_ERROR ret = Resize(tensor1, dst, Size(CONST_128, CONST_128), Interpolation::NEAREST_NEIGHBOR_OPENCV, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_WarpPerspective_Should_Return_Failed_When_Mat_Determinant_Is_Zero)
{
    Tensor src(&g_data5, {1, IMAGE_HEIGHT, IMAGE_WIDTH, 1}, TensorDType::UINT8, 0);
    src.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = WarpPerspective(src, dst, DETERMINANT_ZERO_MATRIX,
                                    (MxBase::PaddingMode) 0, 0, (MxBase::WarpPerspectiveMode) 0);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}


TEST_F(TensorWarpingTest, Test_WarpPerspective_Should_Return_Success_When_Mat_Determinant_Is_Not_Zero)
{
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor src(&g_data5, {1, IMAGE_HEIGHT, IMAGE_WIDTH, 1}, TensorDType::UINT8, 0);
    src.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = WarpPerspective(src, dst, CHANGE_NOTHING_MATRIX,
                                    (MxBase::PaddingMode) 0, 0, (MxBase::WarpPerspectiveMode) 0, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_WarpPerspective_Should_Return_Success_When_Src_Is_FP16)
{
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor src({1, IMAGE_HEIGHT, IMAGE_WIDTH, 1}, TensorDType::FLOAT16, 0);
    src.Malloc();
    src.SetTensorValue(0.2f, true);
    src.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = WarpPerspective(src, dst, CHANGE_NOTHING_MATRIX,
                                    (MxBase::PaddingMode) 0, 0, (MxBase::WarpPerspectiveMode) 0, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_WarpPerspective_Should_Return_Success_When_Src_Is_FP32)
{
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor src({1, IMAGE_HEIGHT, IMAGE_WIDTH, 1}, TensorDType::FLOAT32, 0);
    src.Malloc();
    src.SetTensorValue(0.2f);
    src.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = WarpPerspective(src, dst, CHANGE_NOTHING_MATRIX,
                                    (MxBase::PaddingMode) 0, 0, (MxBase::WarpPerspectiveMode) 0, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_WarpPerspective_Should_Return_Success_When_Src_Is_FLOAT32_Sync)
{
    Tensor src({1, IMAGE_HEIGHT, IMAGE_WIDTH, 1}, TensorDType::FLOAT32, 0);
    src.Malloc();
    src.SetTensorValue(1.0, false);
    src.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = WarpPerspective(src, dst, CHANGE_NOTHING_MATRIX,
                                    (MxBase::PaddingMode) 0, 0, (MxBase::WarpPerspectiveMode) 0);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_WarpPerspective_Should_Return_Failed_When_Src_Is_INT32)
{
    Tensor src({1, IMAGE_HEIGHT, IMAGE_WIDTH, 1}, TensorDType::INT32, 0);
    src.Malloc();
    src.SetTensorValue(1);
    src.ToDevice(0);
    Tensor dst;
    APP_ERROR ret = WarpPerspective(src, dst, CHANGE_NOTHING_MATRIX,
                                    (MxBase::PaddingMode) 0, 0, (MxBase::WarpPerspectiveMode) 0);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_With_Invalid_Interpolation)
{
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, RGBA_CHANNEL};
    Tensor src(shape, TensorDType::UINT8, 0);
    src.Malloc();
    Tensor dst;
    APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::NEAREST_NEIGHBOR_TENSORFLOW, true);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_ResizeRgba_CallBackFunc_Success)
{
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    std::vector<Tensor> src;
    std::vector<Tensor> dst;
    Size resize;
    ResizeRgbaCallbackParam* resizeCallbackParam =
            new ResizeRgbaCallbackParam{src, dst, resize, Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream};
    CallBack dvppCallback;
    auto ret = dvppCallback.ResizeRgbaCallback(resizeCallbackParam, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_ResizeRgba_CallBackFunc_Failed_Mock_aclrtLaunchCallback)
{
    MOCKER_CPP(&aclrtLaunchCallback).stubs().will(returnValue(1));
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    std::vector<Tensor> src;
    std::vector<Tensor> dst;
    Size resize;
    ResizeRgbaCallbackParam* resizeCallbackParam =
            new ResizeRgbaCallbackParam{src, dst, resize, Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream};
    CallBack dvppCallback;
    auto ret = dvppCallback.ResizeRgbaCallback(resizeCallbackParam, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(TensorWarpingTest, Test_ResizeRgba_CallBackFunc_Failed_Return_APP_ERR_OK)
{
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor src1(std::vector<uint32_t>{40, 40, 4}, MxBase::TensorDType::UINT8, 0);
    src1.Malloc();
    src1.ToDevice(0);
    std::vector<Tensor> srcVec = {src1, src1};
    std::vector<Tensor> dst = {src1};
    Size resize;
    ResizeRgbaCallbackParam* resizeCallbackParam =
            new ResizeRgbaCallbackParam{srcVec, dst, resize, Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream};
    CallBack dvppCallback;
    auto ret = dvppCallback.ResizeRgbaCallback(resizeCallbackParam, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_ResizeRgba_CallBackFunc_Success_Mock_aclrtMemcpy)
{
    MOCKER_CPP(&aclrtMemcpy).stubs().will(returnValue(0));
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor src1(std::vector<uint32_t>{40, 40, 4}, MxBase::TensorDType::UINT8, 0);
    src1.Malloc();
    src1.ToDevice(0);
    std::vector<Tensor> srcVec = {src1};
    std::vector<Tensor> dst = {src1};
    Size resize;
    ResizeRgbaCallbackParam* resizeCallbackParam =
            new ResizeRgbaCallbackParam{srcVec, dst, resize, Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream};
    CallBack dvppCallback;
    auto ret = dvppCallback.ResizeRgbaCallback(resizeCallbackParam, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_ResizeRgba_CallBackFunc_Failed_Mock_GetChn)
{
    MOCKER_CPP(&aclrtMemcpy).stubs().will(returnValue(0));
    MOCKER_CPP(&hi_mpi_dvpp_malloc).stubs().will(returnValue(0));
    MOCKER_CPP(&DvppPool::GetChn).stubs().will(returnValue(1));
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor src1(std::vector<uint32_t>{40, 40, 4}, MxBase::TensorDType::UINT8, 0);
    src1.Malloc();
    src1.ToDevice(0);
    Tensor dst1(std::vector<uint32_t>{40, 40, 4}, MxBase::TensorDType::UINT8, 0);
    src1.Malloc();
    src1.ToDevice(0);
    std::vector<Tensor> srcVec = {src1};
    std::vector<Tensor> dst = {dst1};
    Size resize;
    ResizeRgbaCallbackParam* resizeCallbackParam =
            new ResizeRgbaCallbackParam{srcVec, dst, resize, Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream};
    CallBack dvppCallback;
    auto ret = dvppCallback.ResizeRgbaCallback(resizeCallbackParam, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_ResizeRgba_CallBackFunc_Failed_Mock_Himpi_Resize)
{
    MOCKER_CPP(&aclrtMemcpy).stubs().will(returnValue(0));
    MOCKER_CPP(&hi_mpi_dvpp_malloc).stubs().will(returnValue(0));
    MOCKER_CPP(&DvppPool::GetChn).stubs().will(returnValue(0));
    MOCKER_CPP(&hi_mpi_vpc_resize).stubs().will(returnValue(1));
    MOCKER_CPP(&DvppPool::PutChn).stubs().will(returnValue(1));
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor src1(std::vector<uint32_t>{40, 40, 4}, MxBase::TensorDType::UINT8, 0);
    src1.Malloc();
    src1.ToDevice(0);
    Tensor dst1(std::vector<uint32_t>{40, 40, 4}, MxBase::TensorDType::UINT8, 0);
    src1.Malloc();
    src1.ToDevice(0);
    std::vector<Tensor> srcVec = {src1};
    std::vector<Tensor> dst = {dst1};
    Size resize;
    ResizeRgbaCallbackParam* resizeCallbackParam =
            new ResizeRgbaCallbackParam{srcVec, dst, resize, Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream};
    CallBack dvppCallback;
    auto ret = dvppCallback.ResizeRgbaCallback(resizeCallbackParam, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_ResizeRgba_CallBackFunc_Failed_Mock_Himpi_GetResult_Failed)
{
    MOCKER_CPP(&aclrtMemcpy).stubs().will(returnValue(0));
    MOCKER_CPP(&hi_mpi_dvpp_malloc).stubs().will(returnValue(0));
    MOCKER_CPP(&DvppPool::GetChn).stubs().will(returnValue(0));
    MOCKER_CPP(&hi_mpi_vpc_resize).stubs().will(returnValue(0));
    MOCKER_CPP(&hi_mpi_vpc_get_process_result).stubs().will(returnValue(1));
    MOCKER_CPP(&DvppPool::PutChn).stubs().will(returnValue(1));
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor src1(std::vector<uint32_t>{40, 40, 4}, MxBase::TensorDType::UINT8, 0);
    src1.Malloc();
    src1.ToDevice(0);
    Tensor dst1(std::vector<uint32_t>{40, 40, 4}, MxBase::TensorDType::UINT8, 0);
    src1.Malloc();
    src1.ToDevice(0);
    std::vector<Tensor> srcVec = {src1};
    std::vector<Tensor> dst = {dst1};
    Size resize;
    ResizeRgbaCallbackParam* resizeCallbackParam =
            new ResizeRgbaCallbackParam{srcVec, dst, resize, Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream};
    CallBack dvppCallback;
    auto ret = dvppCallback.ResizeRgbaCallback(resizeCallbackParam, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_ResizeRgba_CallBackFunc_Failed_Mock_Himpi_GetResult_Success)
{
    MOCKER_CPP(&aclrtMemcpy).stubs().will(returnValue(0));
    MOCKER_CPP(&hi_mpi_dvpp_malloc).stubs().will(returnValue(0));
    MOCKER_CPP(&DvppPool::GetChn).stubs().will(returnValue(0));
    MOCKER_CPP(&hi_mpi_vpc_resize).stubs().will(returnValue(0));
    MOCKER_CPP(&hi_mpi_vpc_get_process_result).stubs().will(returnValue(0));
    MOCKER_CPP(&DvppPool::PutChn).stubs().will(returnValue(0));
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor src1(std::vector<uint32_t>{40, 40, 4}, MxBase::TensorDType::UINT8, 0);
    src1.Malloc();
    src1.ToDevice(0);
    Tensor dst1(std::vector<uint32_t>{40, 40, 4}, MxBase::TensorDType::UINT8, 0);
    src1.Malloc();
    src1.ToDevice(0);
    std::vector<Tensor> srcVec = {src1};
    std::vector<Tensor> dst = {dst1};
    Size resize;
    ResizeRgbaCallbackParam* resizeCallbackParam =
            new ResizeRgbaCallbackParam{srcVec, dst, resize, Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream};
    CallBack dvppCallback;
    auto ret = dvppCallback.ResizeRgbaCallback(resizeCallbackParam, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_ResizeCallBack_Success)
{
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor src;
    Tensor dst;
    Size resize;
    ResizeCallbackParam* resizeCallbackParam =
            new ResizeCallbackParam{src, dst, resize, Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream};
    CallBack dvppCallback;
    auto ret = dvppCallback.ResizeCallback(resizeCallbackParam, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_ResizeCallBack_Fail)
{
    MOCKER_CPP(&aclrtLaunchCallback).stubs().will(returnValue(-1));
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor src;
    Tensor dst;
    Size resize;
    ResizeCallbackParam* resizeCallbackParam =
            new ResizeCallbackParam{src, dst, resize, Interpolation::NEAREST_NEIGHBOR_OPENCV, true, stream};
    CallBack dvppCallback;
    auto ret = dvppCallback.ResizeCallback(resizeCallbackParam, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(TensorWarpingTest, Test_RotateCallback_Success)
{
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor src;
    Tensor dst;
    RotateAngle angle = RotateAngle::ROTATE_90;
    RotateCallbackParam* rotateCallbackParam =
            new RotateCallbackParam{src, dst, angle};
    CallBack dvppCallback;
    auto ret = dvppCallback.RotateCallback(rotateCallbackParam, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TensorWarpingTest, Test_RotateCallbackFunc_Success)
{
    MOCKER_CPP(&aclrtLaunchCallback).stubs().will(returnValue(-1));
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor src;
    Tensor dst;
    RotateAngle angle = RotateAngle::ROTATE_90;
    RotateCallbackParam* rotateCallbackParam =
            new RotateCallbackParam{src, dst, angle};
    CallBack dvppCallback;
    auto ret = dvppCallback.RotateCallback(rotateCallbackParam, stream);
    stream.Synchronize();
    stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(TensorWarpingTest, Test_CropResize_Should_Return_Fail_When_Dst_Is_Invalid)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        std::vector<uint32_t> dstShape = {IMAGE_HEIGHT, IMAGE_WIDTH, CHANNEL};
        Rect rect1(CONST_0, CONST_0, CONST_64, CONST_64);
        std::vector<Rect> cropRecVec{rect1};
        Size size1(CONST_32, CONST_32);
        std::vector<Size> sizeVec{size1};

        Tensor dstTensor(dstShape, TensorDType::FLOAT32, 0);
        dstTensor.SetValidRoi(Rect{0, 0, CONST_32, CONST_32});
        Tensor::TensorMalloc(dstTensor);
        std::vector<Tensor> dst{dstTensor};
        APP_ERROR ret = CropResize(src, cropRecVec, sizeVec, dst, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

        Tensor dstTensor2(dstShape, TensorDType::UINT8);
        dstTensor2.SetValidRoi(Rect{0, 0, CONST_32, CONST_32});
        Tensor::TensorMalloc(dstTensor2);
        std::vector<Tensor> dst2{dstTensor2};
        ret = CropResize(src, cropRecVec, sizeVec, dst2, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

        ret = CropResize(src, cropRecVec, sizeVec, dst2, Interpolation::BILINEAR_SIMILAR_TENSORFLOW, false);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

        std::vector<Tensor> dst3{dstTensor2, dstTensor2};
        ret = CropResize(src, cropRecVec, sizeVec, dst3, Interpolation::BILINEAR_SIMILAR_OPENCV, false);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_Src_Is_RGBA_And_Using_Bilinear_With_Stream)
{
    if (DeviceManager::IsAscend310P()) {
        MOCKER_CPP(&aclrtLaunchCallback).stubs().will(returnValue(-1));
        AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
        stream.CreateAscendStream();
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, RGBA_CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::BILINEAR_SIMILAR_OPENCV,
                               true, stream);
        EXPECT_EQ(ret, -1);
        stream.Synchronize();
        stream.DestroyAscendStream();
    }
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Success_When_Src_Is_RGBA_And_Using_Bilinear_With_Stream)
{
    if (DeviceManager::IsAscend310P()) {
        AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
        stream.CreateAscendStream();
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, RGBA_CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst;
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::BILINEAR_SIMILAR_OPENCV,
                               true, stream);
        EXPECT_EQ(ret, APP_ERR_OK);
        stream.Synchronize();
        stream.DestroyAscendStream();
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Success_When_Device_Is_800IA2)
{
    MOCKER_CPP(&DeviceManager::IsAtlas800IA2).stubs().will(returnValue(true));
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2;
    APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90, stream);
    EXPECT_EQ(ret, APP_ERR_OK);
    stream.Synchronize();
    stream.DestroyAscendStream();
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Device_Is_800IA2)
{
    MOCKER_CPP(&DeviceManager::IsAtlas800IA2).stubs().will(returnValue(true));
    MOCKER_CPP(&CallBack::RotateCallback).stubs().will(returnValue(-1));
    AscendStream stream = AscendStream(0, AscendStream::FlagType::FAST_LAUNCH);
    stream.CreateAscendStream();
    Tensor tensor1(&g_data1, SHAPE3, TensorDType::UINT8);
    tensor1.ToDevice(0);
    Tensor tensor2;
    APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90, stream);
    EXPECT_EQ(ret, -1);
    stream.Synchronize();
    stream.DestroyAscendStream();
}

TEST_F(TensorWarpingTest, Test_ResizePaste_Should_Return_Faile_With_InputPics_Items_Is_Invalid)
{
    ImageProcessor imageProcessor(0);
    std::vector<uint32_t> shape = { IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL };

    Tensor tensor(shape, TensorDType::UINT8);
    APP_ERROR ret = Tensor::TensorMalloc(tensor);
    tensor.ToDvpp(0);

    Tensor inputPicsTensor2(shape, TensorDType::UINT8);
    Tensor inputPicsTensor3(shape, TensorDType::UINT8);
    std::vector<Tensor> inputPics = {inputPicsTensor2, inputPicsTensor3};

    Rect rect2(CONST_0, CONST_0, CONST_128, CONST_128);
    Rect rect3(CONST_0, CONST_0, CONST_64, CONST_64);
    std::vector<Rect> pasteRects = {rect2, rect3};
    Tensor outputTensor;
    ret = ResizePaste(tensor, inputPics, pasteRects, outputTensor, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor inputPicsTensor4(shape, TensorDType::FLOAT32);
    Tensor::TensorMalloc(inputPicsTensor4);
    Tensor inputPicsTensor5(shape, TensorDType::UINT8);
    Tensor::TensorMalloc(inputPicsTensor5);
    std::vector<Tensor> inputPics2 = {inputPicsTensor4, inputPicsTensor5};
    ret = ResizePaste(tensor, inputPics2, pasteRects, outputTensor, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor inputPicsTensor6({ IMAGE_SHAPE }, TensorDType::UINT8);
    Tensor::TensorMalloc(inputPicsTensor6);
    std::vector<Tensor> inputPics3 = {inputPicsTensor6, inputPicsTensor5};
    ret = ResizePaste(tensor, inputPics3, pasteRects, outputTensor, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    std::vector<Tensor> inputPics4 = {inputPicsTensor5, inputPicsTensor6};
    ret = ResizePaste(tensor, inputPics4, pasteRects, outputTensor, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    Tensor inputPicsTensor7({ IMAGE_SHAPE, IMAGE_SHAPE, GRAY_CHANNEL }, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(inputPicsTensor7);
    Tensor inputPicsTensor8({ IMAGE_SHAPE, IMAGE_SHAPE, CHANNEL }, TensorDType::UINT8, 0);
    Tensor::TensorMalloc(inputPicsTensor8);
    std::vector<Tensor> inputPics5 = {inputPicsTensor7, inputPicsTensor8};
    ret = ResizePaste(tensor, inputPics5, pasteRects, outputTensor, false);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TensorWarpingTest, Test_Resize_Should_Return_Fail_When_Dst_Is_Invalid)
{
    if (DeviceManager::IsAscend310P()) {
        std::vector<uint32_t> shape = {IMAGE_HEIGHT, IMAGE_WIDTH, RGBA_CHANNEL};
        Tensor src(shape, TensorDType::UINT8, 0);
        Tensor::TensorMalloc(src);
        Tensor dst({IMAGE_SHAPE, IMAGE_SHAPE}, TensorDType::UINT8);
        Tensor::TensorMalloc(dst);
        APP_ERROR ret = Resize(src, dst, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::BILINEAR_SIMILAR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

        Tensor dst2(shape, TensorDType::UINT8, 1);
        Tensor::TensorMalloc(dst2);
        ret = Resize(src, dst2, Size(IMAGE_SHAPE, IMAGE_SHAPE), Interpolation::BILINEAR_SIMILAR_OPENCV, true);
        EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    }
}


TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Rotate_90_And_Transpose_Fail)
{
    MOCKER_CPP(&Transpose).stubs().will(returnValue(-1));
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE_SINGLE, TensorDType::UINT8);
        tensor1.ToDevice(0);
        Tensor tensor2;
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
        EXPECT_EQ(ret, -1);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Rotate_270_And_Transpose_Fail)
{
    MOCKER_CPP(&Transpose).stubs().will(returnValue(-1));
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE_SINGLE, TensorDType::UINT8);
        tensor1.ToDevice(0);
        Tensor tensor2;
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_270);
        EXPECT_EQ(ret, -1);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Rotate_90_And_ConvertTo_Fail)
{
    MOCKER_CPP(&ConvertTo).stubs().will(returnValue(-1));
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE2, TensorDType::UINT8);
        tensor1.ToDevice(0);
        Tensor tensor2;
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
        EXPECT_EQ(ret, -1);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Rotate_90_And_Split_Fail)
{
    MOCKER_CPP(&Split).stubs().will(returnValue(-1));
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE3_FOUR_CHANNEL, TensorDType::UINT8);
        tensor1.ToDevice(0);
        Tensor tensor2;
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
        EXPECT_EQ(ret, -1);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Fail_When_Rotate_90_And_Split_Fail_And_NotConvert)
{
    MOCKER_CPP(&Split).stubs().will(returnValue(-1));
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE3_FOUR_CHANNEL, TensorDType::FLOAT16);
        tensor1.ToDevice(0);
        Tensor tensor2;
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
        EXPECT_EQ(ret, -1);
    }
}

TEST_F(TensorWarpingTest, Test_Rotate_Should_Return_Success_When_Rotate_90_And_NotConvert)
{
    if (DeviceManager::IsAscend310P()) {
        Tensor tensor1(&g_data1, SHAPE2, TensorDType::FLOAT16);
        tensor1.ToDevice(0);
        Tensor tensor2;
        APP_ERROR ret = Rotate(tensor1, tensor2, MxBase::RotateAngle::ROTATE_90);
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
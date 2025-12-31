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
 * Create: 2024
 * History: NA
 */
#include <gtest/gtest.h>
#include "ResourceManager/DvppWrapper/DvppConfig/Config/Config.h"
#include "EncodeConfigWithAtlas800IA2.h"
#include "ConvertFormatConfigWithAtlas800IA2.h"
#include "ResizeConfigWithAtlas800IA2.h"
#include "CropConfigWithAtlas800IA2.h"
#include "CropPasteConfigWithAtlas800IA2.h"
#include "CropResizeConfigWithAtlas800IA2.h"
#include "PaddingConfigWithAtlas800IA2.h"
using namespace MxBase;
namespace {
constexpr size_t DATA_LENTH = 6;
constexpr uint32_t SHAPE_NORMAL_VALUE1 = 200;
constexpr uint32_t NOT_ALIGNED = 251;
constexpr uint32_t YUV_OVERSIZE = 8193;
constexpr uint32_t RGB_OVERSIZE = 4097;
constexpr uint32_t STRIDE_SHAPE_OVERSIZE = 500000;
constexpr uint32_t SHAPE_NORMAL_VALUE2 = 300;
constexpr uint32_t ALIGNED = 256;
constexpr uint32_t PADDING_COLOR = 255;
constexpr uint32_t PADDING_DIM = 10;
constexpr uint32_t NORMAL_ENCODE_LEVEL = 50;
constexpr uint32_t NORMAL_RESIZE_WIDTH = 20;
constexpr uint32_t NORMAL_RESIZE_HEIGHT = 80;
constexpr float NORMAL_RESIZE_WIDTH_RATIO = 2.0f;
constexpr float NORMAL_RESIZE_HEIGHT_RATIO = 2.0f;
constexpr uint32_t NORMAL_RESIZE_INTERPOLATION = 0;
constexpr uint32_t NORMAL_X0 = 20;
constexpr uint32_t NORMAL_X1 = 80;
constexpr uint32_t NORMAL_Y0 = 30;
constexpr uint32_t NORMAL_Y1 = 90;
constexpr uint32_t CONST_NUMBER_ONE = 1;

uint8_t g_data1[DATA_LENTH] = {0};
class DvppConfigTest : public testing::Test {};

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_Is_Null_On_310)
{
    DvppDataInfo input;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_Is_Null_On_310B)
{
    DvppDataInfo input;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310B", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_Is_Null_On_310P)
{
    DvppDataInfo input;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_Width_Is_Oversize_On_310)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.height = SHAPE_NORMAL_VALUE1;
    input.width = YUV_OVERSIZE;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_Width_Is_Oversize_On_310B)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.height = SHAPE_NORMAL_VALUE1;
    input.width = YUV_OVERSIZE;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310B", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_Width_Is_Oversize_On_310P_YUV)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.height = SHAPE_NORMAL_VALUE1;
    input.width = YUV_OVERSIZE;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_Width_Is_Oversize_On_310P_RGB)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_RGB_888;
    input.height = SHAPE_NORMAL_VALUE1;
    input.width = RGB_OVERSIZE;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_Height_Is_Oversize_On_310)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = YUV_OVERSIZE;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_Height_Is_Oversize_On_310B)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = YUV_OVERSIZE;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310B", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_Height_Is_Oversize_On_310P_YUV)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = YUV_OVERSIZE;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_Height_Is_Oversize_On_310P_RGB)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_RGB_888;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = RGB_OVERSIZE;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_HeightStride_Is_Oversize)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = SHAPE_NORMAL_VALUE1;
    input.widthStride = ALIGNED;
    input.heightStride = STRIDE_SHAPE_OVERSIZE;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_WidthStride_Is_Not_Aligned)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = SHAPE_NORMAL_VALUE1;
    input.widthStride = NOT_ALIGNED;
    input.heightStride = ALIGNED;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_HeightStride_Is_Not_Aligned)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = SHAPE_NORMAL_VALUE1;
    input.widthStride = ALIGNED;
    input.heightStride = NOT_ALIGNED;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_CropAndPaste_Should_Return_Failed_When_Input_HeightStride_Is_lower_Than_Height)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = SHAPE_NORMAL_VALUE2;
    input.widthStride = ALIGNED;
    input.heightStride = ALIGNED;
    std::vector <DvppDataInfo> inputDataVec = {input};

    std::vector <DvppDataInfo> pasteDataVec;
    std::vector <CropRoiConfig> cropAreaVec;
    std::vector <CropRoiConfig> pasteAreaVec;
    std::tuple <std::string, std::string, ImageFormat> cropResizeConfigKey = {
            "310", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropPasteConfig(cropResizeConfigKey);
    APP_ERROR ret = config->CropPasteCheck(inputDataVec, pasteDataVec, cropAreaVec, pasteAreaVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}
TEST_F(DvppConfigTest, Test_Padding_Should_Return_Failed_When_Input_Is_Null_On_310P)
{
    DvppDataInfo input;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    MakeBorderConfig borderConfig;
    borderConfig.borderType = MxBase::MakeBorderConfig::BORDER_CONSTANT;
    borderConfig.left = PADDING_DIM;
    borderConfig.right = PADDING_DIM;
    borderConfig.top = PADDING_DIM;
    borderConfig.bottom = PADDING_DIM;
    borderConfig.channel_zero = PADDING_COLOR;
    borderConfig.channel_one = PADDING_COLOR;
    borderConfig.channel_two = PADDING_COLOR;

    std::tuple <std::string, std::string, ImageFormat> paddingConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetPaddingConfig(paddingConfigKey);
    APP_ERROR ret = config->PaddingCheck(input, borderConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Padding_Should_Return_Failed_When_Input_Width_Is_Oversize_On_310P)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.height = SHAPE_NORMAL_VALUE1;
    input.width = YUV_OVERSIZE;
    MakeBorderConfig borderConfig;
    borderConfig.borderType = MxBase::MakeBorderConfig::BORDER_CONSTANT;
    borderConfig.left = PADDING_DIM;
    borderConfig.right = PADDING_DIM;
    borderConfig.top = PADDING_DIM;
    borderConfig.bottom = PADDING_DIM;
    borderConfig.channel_zero = PADDING_COLOR;
    borderConfig.channel_one = PADDING_COLOR;
    borderConfig.channel_two = PADDING_COLOR;
    std::tuple <std::string, std::string, ImageFormat> paddingConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetPaddingConfig(paddingConfigKey);
    APP_ERROR ret = config->PaddingCheck(input, borderConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Padding_Should_Return_Failed_When_Input_Height_Is_Oversize_On_310P)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = YUV_OVERSIZE;
    MakeBorderConfig borderConfig;
    borderConfig.borderType = MxBase::MakeBorderConfig::BORDER_CONSTANT;
    borderConfig.left = PADDING_DIM;
    borderConfig.right = PADDING_DIM;
    borderConfig.top = PADDING_DIM;
    borderConfig.bottom = PADDING_DIM;
    borderConfig.channel_zero = PADDING_COLOR;
    borderConfig.channel_one = PADDING_COLOR;
    borderConfig.channel_two = PADDING_COLOR;
    std::tuple <std::string, std::string, ImageFormat> paddingConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetPaddingConfig(paddingConfigKey);
    APP_ERROR ret = config->PaddingCheck(input, borderConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Padding_Should_Return_Failed_When_Input_HeightStride_Is_Oversize)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = SHAPE_NORMAL_VALUE1;
    input.widthStride = ALIGNED;
    input.heightStride = STRIDE_SHAPE_OVERSIZE;
    MakeBorderConfig borderConfig;
    borderConfig.borderType = MxBase::MakeBorderConfig::BORDER_CONSTANT;
    borderConfig.left = PADDING_DIM;
    borderConfig.right = PADDING_DIM;
    borderConfig.top = PADDING_DIM;
    borderConfig.bottom = PADDING_DIM;
    borderConfig.channel_zero = PADDING_COLOR;
    borderConfig.channel_one = PADDING_COLOR;
    borderConfig.channel_two = PADDING_COLOR;
    std::tuple <std::string, std::string, ImageFormat> paddingConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetPaddingConfig(paddingConfigKey);
    APP_ERROR ret = config->PaddingCheck(input, borderConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Padding_Should_Return_Failed_When_Input_WidthStride_Is_Not_Aligned)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = SHAPE_NORMAL_VALUE1;
    input.widthStride = NOT_ALIGNED;
    input.heightStride = ALIGNED;
    MakeBorderConfig borderConfig;
    borderConfig.borderType = MxBase::MakeBorderConfig::BORDER_CONSTANT;
    borderConfig.left = PADDING_DIM;
    borderConfig.right = PADDING_DIM;
    borderConfig.top = PADDING_DIM;
    borderConfig.bottom = PADDING_DIM;
    borderConfig.channel_zero = PADDING_COLOR;
    borderConfig.channel_one = PADDING_COLOR;
    borderConfig.channel_two = PADDING_COLOR;
    std::tuple <std::string, std::string, ImageFormat> paddingConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetPaddingConfig(paddingConfigKey);
    APP_ERROR ret = config->PaddingCheck(input, borderConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Padding_Should_Return_Failed_When_Input_HeightStride_Is_Not_Aligned)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = SHAPE_NORMAL_VALUE1;
    input.widthStride = ALIGNED;
    input.heightStride = NOT_ALIGNED;
    MakeBorderConfig borderConfig;
    borderConfig.borderType = MxBase::MakeBorderConfig::BORDER_CONSTANT;
    borderConfig.left = PADDING_DIM;
    borderConfig.right = PADDING_DIM;
    borderConfig.top = PADDING_DIM;
    borderConfig.bottom = PADDING_DIM;
    borderConfig.channel_zero = PADDING_COLOR;
    borderConfig.channel_one = PADDING_COLOR;
    borderConfig.channel_two = PADDING_COLOR;
    std::tuple <std::string, std::string, ImageFormat> paddingConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetPaddingConfig(paddingConfigKey);
    APP_ERROR ret = config->PaddingCheck(input, borderConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Padding_Should_Return_Failed_When_Input_HeightStride_Is_lower_Than_Height)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = SHAPE_NORMAL_VALUE2;
    input.widthStride = ALIGNED;
    input.heightStride = ALIGNED;
    MakeBorderConfig borderConfig;
    borderConfig.borderType = MxBase::MakeBorderConfig::BORDER_CONSTANT;
    borderConfig.left = PADDING_DIM;
    borderConfig.right = PADDING_DIM;
    borderConfig.top = PADDING_DIM;
    borderConfig.bottom = PADDING_DIM;
    borderConfig.channel_zero = PADDING_COLOR;
    borderConfig.channel_one = PADDING_COLOR;
    borderConfig.channel_two = PADDING_COLOR;
    std::tuple <std::string, std::string, ImageFormat> paddingConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetPaddingConfig(paddingConfigKey);
    APP_ERROR ret = config->PaddingCheck(input, borderConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Padding_Should_Return_Failed_When_Input_WidthStride_Is_lower_Than_Width)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE2;
    input.height = SHAPE_NORMAL_VALUE1;
    input.widthStride = ALIGNED;
    input.heightStride = ALIGNED;
    MakeBorderConfig borderConfig;
    borderConfig.borderType = MxBase::MakeBorderConfig::BORDER_CONSTANT;
    borderConfig.left = PADDING_DIM;
    borderConfig.right = PADDING_DIM;
    borderConfig.top = PADDING_DIM;
    borderConfig.bottom = PADDING_DIM;
    borderConfig.channel_zero = PADDING_COLOR;
    borderConfig.channel_one = PADDING_COLOR;
    borderConfig.channel_two = PADDING_COLOR;
    std::tuple <std::string, std::string, ImageFormat> paddingConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetPaddingConfig(paddingConfigKey);
    APP_ERROR ret = config->PaddingCheck(input, borderConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_ConvertFormat_Should_Return_Failed_When_Input_Is_Null_On_310P)
{
    DvppDataInfo input;
    DvppDataInfo output;
    std::tuple <std::string, std::string, ImageFormat> convertFormatConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetConvertFormatConfig(convertFormatConfigKey);
    APP_ERROR ret = config->ConvertFormatCheck(input, output);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_ConvertFormat_Should_Return_Failed_When_Input_Width_Is_Oversize_On_310P)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.height = SHAPE_NORMAL_VALUE1;
    input.width = YUV_OVERSIZE;
    DvppDataInfo output;
    std::tuple <std::string, std::string, ImageFormat> convertFormatConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetConvertFormatConfig(convertFormatConfigKey);
    APP_ERROR ret = config->ConvertFormatCheck(input, output);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_ConvertFormat_Should_Return_Failed_When_Input_Height_Is_Oversize_On_310P)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = YUV_OVERSIZE;
    DvppDataInfo output;
    std::tuple <std::string, std::string, ImageFormat> convertFormatConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetConvertFormatConfig(convertFormatConfigKey);
    APP_ERROR ret = config->ConvertFormatCheck(input, output);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_ConvertFormat_Should_Return_Failed_When_Input_HeightStride_Is_Oversize)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = SHAPE_NORMAL_VALUE1;
    input.widthStride = ALIGNED;
    input.heightStride = STRIDE_SHAPE_OVERSIZE;
    DvppDataInfo output;
    std::tuple <std::string, std::string, ImageFormat> convertFormatConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetConvertFormatConfig(convertFormatConfigKey);
    APP_ERROR ret = config->ConvertFormatCheck(input, output);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_ConvertFormat_Should_Return_Failed_When_Input_WidthStride_Is_Not_Aligned)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = SHAPE_NORMAL_VALUE1;
    input.widthStride = NOT_ALIGNED;
    input.heightStride = ALIGNED;
    DvppDataInfo output;
    std::tuple <std::string, std::string, ImageFormat> convertFormatConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetConvertFormatConfig(convertFormatConfigKey);
    APP_ERROR ret = config->ConvertFormatCheck(input, output);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_ConvertFormat_Should_Return_Failed_When_Input_HeightStride_Is_Not_Aligned)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = SHAPE_NORMAL_VALUE1;
    input.widthStride = ALIGNED;
    input.heightStride = NOT_ALIGNED;
    DvppDataInfo output;
    std::tuple <std::string, std::string, ImageFormat> convertFormatConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetConvertFormatConfig(convertFormatConfigKey);
    APP_ERROR ret = config->ConvertFormatCheck(input, output);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_ConvertFormat_Should_Return_Failed_When_Input_HeightStride_Is_lower_Than_Height)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.width = SHAPE_NORMAL_VALUE1;
    input.height = SHAPE_NORMAL_VALUE2;
    input.widthStride = ALIGNED;
    input.heightStride = ALIGNED;
    DvppDataInfo output;
    std::tuple <std::string, std::string, ImageFormat> convertFormatConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetConvertFormatConfig(convertFormatConfigKey);
    APP_ERROR ret = config->ConvertFormatCheck(input, output);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Crop_Should_Return_Failed_When_Input_Is_Null_On_310P)
{
    DvppDataInfo input;
    CropRoiConfig cropRoiConfig = {NORMAL_X0, NORMAL_X1, NORMAL_Y0, NORMAL_Y1};
    std::vector<CropRoiConfig> cropAreaVec = {cropRoiConfig};
    uint32_t cropAreaNum = CONST_NUMBER_ONE;
    uint32_t inputImageNum = CONST_NUMBER_ONE;
    uint32_t outputImageNum = CONST_NUMBER_ONE;
    std::tuple <std::string, std::string, ImageFormat> cropConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropConfig(cropConfigKey);
    APP_ERROR ret = config->CropCheck(input, cropAreaVec, cropAreaNum, inputImageNum, outputImageNum);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Crop_Should_Return_Failed_When_Input_Is_Null_On_310)
{
    DvppDataInfo input;
    CropRoiConfig cropRoiConfig = {NORMAL_X0, NORMAL_X1, NORMAL_Y0, NORMAL_Y1};
    std::vector<CropRoiConfig> cropAreaVec = {cropRoiConfig};
    uint32_t cropAreaNum = CONST_NUMBER_ONE;
    uint32_t inputImageNum = CONST_NUMBER_ONE;
    uint32_t outputImageNum = CONST_NUMBER_ONE;
    std::tuple <std::string, std::string, ImageFormat> cropConfigKey = {
            "310", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropConfig(cropConfigKey);
    APP_ERROR ret = config->CropCheck(input, cropAreaVec, cropAreaNum, inputImageNum, outputImageNum);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Crop_Should_Return_Failed_When_Input_Is_Null_On_310B)
{
    DvppDataInfo input;
    CropRoiConfig cropRoiConfig = {NORMAL_X0, NORMAL_X1, NORMAL_Y0, NORMAL_Y1};
    std::vector<CropRoiConfig> cropAreaVec = {cropRoiConfig};
    uint32_t cropAreaNum = CONST_NUMBER_ONE;
    uint32_t inputImageNum = CONST_NUMBER_ONE;
    uint32_t outputImageNum = CONST_NUMBER_ONE;
    std::tuple <std::string, std::string, ImageFormat> cropConfigKey = {
            "310B", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropConfig(cropConfigKey);
    APP_ERROR ret = config->CropCheck(input, cropAreaVec, cropAreaNum, inputImageNum, outputImageNum);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Crop_Should_Return_Failed_When_Input_Width_Is_Oversize_On_310P)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.height = SHAPE_NORMAL_VALUE1;
    input.width = YUV_OVERSIZE;
    CropRoiConfig cropRoiConfig = {NORMAL_X0, NORMAL_X1, NORMAL_Y0, NORMAL_Y1};
    std::vector<CropRoiConfig> cropAreaVec = {cropRoiConfig};
    uint32_t cropAreaNum = CONST_NUMBER_ONE;
    uint32_t inputImageNum = CONST_NUMBER_ONE;
    uint32_t outputImageNum = CONST_NUMBER_ONE;
    std::tuple <std::string, std::string, ImageFormat> cropConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropConfig(cropConfigKey);
    APP_ERROR ret = config->CropCheck(input, cropAreaVec, cropAreaNum, inputImageNum, outputImageNum);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Crop_Should_Return_Failed_When_Input_Height_Is_Oversize_On_310P)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.height = YUV_OVERSIZE;
    input.width = SHAPE_NORMAL_VALUE1;
    CropRoiConfig cropRoiConfig = {NORMAL_X0, NORMAL_X1, NORMAL_Y0, NORMAL_Y1};
    std::vector<CropRoiConfig> cropAreaVec = {cropRoiConfig};
    uint32_t cropAreaNum = CONST_NUMBER_ONE;
    uint32_t inputImageNum = CONST_NUMBER_ONE;
    uint32_t outputImageNum = CONST_NUMBER_ONE;
    std::tuple <std::string, std::string, ImageFormat> cropConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetCropConfig(cropConfigKey);
    APP_ERROR ret = config->CropCheck(input, cropAreaVec, cropAreaNum, inputImageNum, outputImageNum);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Resize_Should_Return_Failed_When_Input_Is_Null_On_310P)
{
    DvppDataInfo input;
    ResizeConfig resizeConfig = {NORMAL_RESIZE_WIDTH, NORMAL_RESIZE_HEIGHT, NORMAL_RESIZE_WIDTH_RATIO,
                                 NORMAL_RESIZE_HEIGHT_RATIO, NORMAL_RESIZE_INTERPOLATION};
    std::tuple <std::string, std::string, ImageFormat> resizeConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetResizeConfig(resizeConfigKey);
    APP_ERROR ret = config->ResizeCheck(input, resizeConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Resize_Should_Return_Failed_When_Input_Is_Null_On_310)
{
    DvppDataInfo input;
    ResizeConfig resizeConfig = {NORMAL_RESIZE_WIDTH, NORMAL_RESIZE_HEIGHT, NORMAL_RESIZE_WIDTH_RATIO,
                                 NORMAL_RESIZE_HEIGHT_RATIO, NORMAL_RESIZE_INTERPOLATION};
    std::tuple <std::string, std::string, ImageFormat> resizeConfigKey = {
            "310", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetResizeConfig(resizeConfigKey);
    APP_ERROR ret = config->ResizeCheck(input, resizeConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Resize_Should_Return_Failed_When_Input_Is_Null_On_310B)
{
    DvppDataInfo input;
    ResizeConfig resizeConfig = {NORMAL_RESIZE_WIDTH, NORMAL_RESIZE_HEIGHT, NORMAL_RESIZE_WIDTH_RATIO,
                                 NORMAL_RESIZE_HEIGHT_RATIO, NORMAL_RESIZE_INTERPOLATION};
    std::tuple <std::string, std::string, ImageFormat> resizeConfigKey = {
            "310B", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetResizeConfig(resizeConfigKey);
    APP_ERROR ret = config->ResizeCheck(input, resizeConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Resize_Should_Return_Failed_When_Input_Width_Is_Oversize_On_310P)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.height = SHAPE_NORMAL_VALUE1;
    input.width = YUV_OVERSIZE;
    ResizeConfig resizeConfig = {NORMAL_RESIZE_WIDTH, NORMAL_RESIZE_HEIGHT, NORMAL_RESIZE_WIDTH_RATIO,
                                 NORMAL_RESIZE_HEIGHT_RATIO, NORMAL_RESIZE_INTERPOLATION};
    std::tuple <std::string, std::string, ImageFormat> resizeConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetResizeConfig(resizeConfigKey);
    APP_ERROR ret = config->ResizeCheck(input, resizeConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Resize_Should_Return_Failed_When_Input_Height_Is_Oversize_On_310P)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.height = YUV_OVERSIZE;
    input.width = SHAPE_NORMAL_VALUE1;
    ResizeConfig resizeConfig = {NORMAL_RESIZE_WIDTH, NORMAL_RESIZE_HEIGHT, NORMAL_RESIZE_WIDTH_RATIO,
                                 NORMAL_RESIZE_HEIGHT_RATIO, NORMAL_RESIZE_INTERPOLATION};
    std::tuple <std::string, std::string, ImageFormat> resizeConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetResizeConfig(resizeConfigKey);
    APP_ERROR ret = config->ResizeCheck(input, resizeConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Encode_Should_Return_Failed_When_Input_Is_Null_On_310P)
{
    DvppDataInfo input;
    uint32_t encodeLevel = NORMAL_ENCODE_LEVEL;
    std::tuple <std::string, std::string, ImageFormat> encodeConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetEncodeConfig(encodeConfigKey);
    APP_ERROR ret = config->EncodeCheck(input, encodeLevel);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Encode_Should_Return_Failed_When_Input_Is_Null_On_310)
{
    DvppDataInfo input;
    uint32_t encodeLevel = NORMAL_ENCODE_LEVEL;
    std::tuple <std::string, std::string, ImageFormat> encodeConfigKey = {
            "310", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetEncodeConfig(encodeConfigKey);
    APP_ERROR ret = config->EncodeCheck(input, encodeLevel);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Encode_Should_Return_Failed_When_Input_Is_Null_On_310B)
{
    DvppDataInfo input;
    uint32_t encodeLevel = NORMAL_ENCODE_LEVEL;
    std::tuple <std::string, std::string, ImageFormat> encodeConfigKey = {
            "310B", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetEncodeConfig(encodeConfigKey);
    APP_ERROR ret = config->EncodeCheck(input, encodeLevel);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Encode_Should_Return_Failed_When_Input_Width_Is_Oversize_On_310P)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.height = SHAPE_NORMAL_VALUE1;
    input.width = YUV_OVERSIZE;
    uint32_t encodeLevel = NORMAL_ENCODE_LEVEL;
    std::tuple <std::string, std::string, ImageFormat> encodeConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetEncodeConfig(encodeConfigKey);
    APP_ERROR ret = config->EncodeCheck(input, encodeLevel);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_Encode_Should_Return_Failed_When_Input_Height_Is_Oversize_On_310P)
{
    DvppDataInfo input;
    input.data = g_data1;
    input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    input.height = YUV_OVERSIZE;
    input.width = SHAPE_NORMAL_VALUE1;
    uint32_t encodeLevel = NORMAL_ENCODE_LEVEL;
    std::tuple <std::string, std::string, ImageFormat> encodeConfigKey = {
            "310P", "*",  static_cast<ImageFormat>(input.format)
    };
    auto config = MxBase::Config::GetInstance()->GetEncodeConfig(encodeConfigKey);
    APP_ERROR ret = config->EncodeCheck(input, encodeLevel);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppConfigTest, Test_ConvertFormatConfigForYUVAndYVUWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    DvppDataInfo input = {};
    DvppDataInfo output = {};
    ConvertFormatConfigForYUVAndYVUWithAtlas800IA2 config;
    APP_ERROR ret = config.ConvertFormatCheck(input, output);
    EXPECT_NE(ret, APP_ERR_OK);
    uint32_t size = 0;
    ret = config.GetOutputDataSize(output, size);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppConfigTest, Test_ConvertFormatConfigForRGBAndBGRWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    DvppDataInfo input = {};
    DvppDataInfo output = {};
    ConvertFormatConfigForRGBAndBGRWithAtlas800IA2 config;
    APP_ERROR ret = config.ConvertFormatCheck(input, output);
    EXPECT_NE(ret, APP_ERR_OK);
    uint32_t size = 0;
    ret = config.GetOutputDataSize(output, size);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppConfigTest, Test_CropConfigForYUVAndYVUWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    DvppDataInfo input = {};
    DvppDataInfo output = {};
    std::vector<CropRoiConfig> cropAreaVec = {};
    CropConfigForYUVAndYVUWithAtlas800IA2 config;
    APP_ERROR ret = config.CropCheck(input, cropAreaVec, 0, 0, 0);
    EXPECT_NE(ret, APP_ERR_OK);
    uint32_t size = 0;
    ret = config.GetOutputDataSize(output, size);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppConfigTest, Test_CropConfigForRGBAndBGRWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    DvppDataInfo input = {};
    DvppDataInfo output = {};
    std::vector<CropRoiConfig> cropAreaVec = {};
    CropConfigForRGBAndBGRWithAtlas800IA2 config;
    APP_ERROR ret = config.CropCheck(input, cropAreaVec, 0, 0, 0);
    EXPECT_NE(ret, APP_ERR_OK);
    uint32_t size = 0;
    ret = config.GetOutputDataSize(output, size);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppConfigTest, Test_CropPasteConfigForYUVAndYVUWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    std::vector<DvppDataInfo> inputDataVec = {};
    std::vector<CropRoiConfig> cropAreaVec = {};
    CropPasteConfigForYUVAndYVUWithAtlas800IA2 config;
    APP_ERROR ret = config.CropPasteCheck(inputDataVec, inputDataVec, cropAreaVec, cropAreaVec);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppConfigTest, Test_CropPasteConfigForRGBAndBGRWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    std::vector<DvppDataInfo> inputDataVec = {};
    std::vector<CropRoiConfig> cropAreaVec = {};
    CropPasteConfigForRGBAndBGRWithAtlas800IA2 config;
    APP_ERROR ret = config.CropPasteCheck(inputDataVec, inputDataVec, cropAreaVec, cropAreaVec);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppConfigTest, Test_CropResizeConfigForYUVAndYVUWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    DvppDataInfo input = {};
    CropRoiConfig cropRect = {};
    ResizeConfig resizeScale = {};
    CropResizeConfigForYUVAndYVUWithAtlas800IA2 config;
    APP_ERROR ret = config.CropResizeCheck(input, cropRect, resizeScale);
    EXPECT_NE(ret, APP_ERR_OK);
    uint32_t size = 0;
    ret = config.GetOutputDataSize(input, size);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppConfigTest, Test_CropResizeConfigForRGBAndBGRWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    DvppDataInfo input = {};
    CropRoiConfig cropRect = {};
    ResizeConfig resizeScale = {};
    CropResizeConfigForRGBAndBGRWithAtlas800IA2 config;
    APP_ERROR ret = config.CropResizeCheck(input, cropRect, resizeScale);
    EXPECT_NE(ret, APP_ERR_OK);
    uint32_t size = 0;
    ret = config.GetOutputDataSize(input, size);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppConfigTest, Test_EncodeConfigForYUVAndYVUWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    DvppDataInfo input = {};
    EncodeConfigForYUVAndYVUWithAtlas800IA2 config;
    APP_ERROR ret = config.EncodeCheck(input, 0);
    EXPECT_NE(ret, APP_ERR_OK);
    uint32_t size = 0;
    ret = config.GetOutputDataSize(input, size);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppConfigTest, Test_EncodeConfigForRBGAndBGRWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    DvppDataInfo input = {};
    EncodeConfigForRBGAndBGRWithAtlas800IA2 config;
    APP_ERROR ret = config.EncodeCheck(input, 0);
    EXPECT_NE(ret, APP_ERR_OK);
    uint32_t size = 0;
    ret = config.GetOutputDataSize(input, size);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppConfigTest, Test_PaddingConfigForYUVAndYVUWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    DvppDataInfo input = {};
    MakeBorderConfig borderConfig = {};
    PaddingConfigForYUVAndYVUWithAtlas800IA2 config;
    APP_ERROR ret = config.PaddingCheck(input, borderConfig);
    EXPECT_NE(ret, APP_ERR_OK);
    uint32_t size = 0;
    ret = config.GetOutputDataSize(input, size);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppConfigTest, Test_PaddingConfigForRGBAndBGRWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    DvppDataInfo input = {};
    MakeBorderConfig borderConfig = {};
    PaddingConfigForRGBAndBGRWithAtlas800IA2 config;
    APP_ERROR ret = config.PaddingCheck(input, borderConfig);
    EXPECT_NE(ret, APP_ERR_OK);
    uint32_t size = 0;
    ret = config.GetOutputDataSize(input, size);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppConfigTest, Test_ResizeConfigForYUVAndYVUWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    DvppDataInfo input = {};
    ResizeConfig resizeConfig = {};
    ResizeConfigForYUVAndYVUWithAtlas800IA2 config;
    APP_ERROR ret = config.ResizeCheck(input, resizeConfig);
    EXPECT_NE(ret, APP_ERR_OK);
    uint32_t size = 0;
    ret = config.GetOutputDataSize(input, size);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppConfigTest, Test_ResizeConfigForRGBAndBGRWithAtlas800IA2_Should_Return_Failed_With_Valid_Input)
{
    DvppDataInfo input = {};
    ResizeConfig resizeConfig = {};
    ResizeConfigForRGBAndBGRWithAtlas800IA2 config;
    APP_ERROR ret = config.ResizeCheck(input, resizeConfig);
    EXPECT_NE(ret, APP_ERR_OK);
    uint32_t size = 0;
    ret = config.GetOutputDataSize(input, size);
    EXPECT_EQ(ret, APP_ERR_OK);
}
}  // namespace

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
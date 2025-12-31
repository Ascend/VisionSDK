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
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#define private public
#include "MbCV/Image/ImageProcessor/ImageProcessorDptr.hpp"
#include "MxBase/E2eInfer/ImageProcessor/ImageProcessor.h"
#undef private
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MxBase.h"
#include "MxBase/Asynchron/AscendStream.h"

using namespace std;
using namespace MxBase;
namespace {
const long EXCEED_MAX_DATA_SIZE = 1073741825;
const uint32_t IMG_SIZE1 = 10;
const uint32_t IMG_SIZE2 = 20;
const uint32_t IMG_SIZE3 = 25;
const uint32_t IMG_SIZE4 = 30;
const uint32_t INVALID_BORDERTYPE = 4;
const uint32_t INVALID_COMPONENT = 5;
const int CROP_INPUT_SIZE = 1;
const int CROP_CONFIG_SIZE = 10;
const uint32_t PADDING_DIM = 10;
const uint32_t PADDING_COLOR = 255;
const uint32_t DEVICE_ID_ZERO = 0;
const uint32_t DEVICE_ID_ONE = 1;
const Size RESIZED_SIZE(100, 100);
const uint32_t LEFT_POS = 201;
const uint32_t UP_POS = 1;
const uint32_t STEP = 10;
const uint32_t IMAGE_VECTOR_SIZE = 10;
const uint32_t STRIDE = 5;
std::shared_ptr<uint8_t> g_encodedJPGDataPtr;
std::shared_ptr<uint8_t> g_encodedPNGDataPtr;
uint32_t g_encodedJPGDataSize;
uint32_t g_encodedPNGDataSize;
Image g_decodedYUV420Image;
ImageProcessor g_imageProcessor;
std::vector<MxBase::Rect> g_cropRectVec(IMAGE_VECTOR_SIZE);
std::vector<std::pair<MxBase::Rect, MxBase::Size>> g_cropResizeVec(IMAGE_VECTOR_SIZE);
const Size CROP_RELATED_SIZE(416, 416);
const Rect CROP_RECT1(0, 0, 416, 416);
const Rect CROP_RECT2(1, 1, 100, 100);
const Rect PASTE_RECT(100, 100, 288, 288);
const std::pair<Rect, Rect> CROP_PASTE_PAIR = std::make_pair(CROP_RECT1, PASTE_RECT);
const std::string JPG_IMAGE_PATH = "./test.JPG";
const std::string PNG_IMAGE_PATH = "/home/simon/models/imageRaw/dog.png";


class ImageProcessorTest : public testing::Test {
protected:
    static void SetUpTestCase()
    {
        g_imageProcessor = ImageProcessor(DEVICE_ID_ZERO);
        APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, g_decodedYUV420Image);
        ret = g_imageProcessor.Encode(g_decodedYUV420Image, g_encodedPNGDataPtr, g_encodedPNGDataSize);
        g_decodedYUV420Image.ToHost();

        Image decodedImage2;
        ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage2);
        ret = g_imageProcessor.Encode(decodedImage2, g_encodedJPGDataPtr, g_encodedJPGDataSize);

        for (size_t i = 0; i < g_cropRectVec.size(); i++) {
            Rect cropRectSub(LEFT_POS, UP_POS, STEP * (i + STRIDE) + LEFT_POS, STEP * (i + STRIDE));
            g_cropRectVec[i] = cropRectSub;
            g_cropResizeVec[i].first = cropRectSub;
            g_cropResizeVec[i].second = CROP_RELATED_SIZE;
        }
    }

    void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

TEST_F(ImageProcessorTest, Test_CheckImageWH_Should_Return_Fail_When_imgSize_Small)
{
    Size imgSize1(IMG_SIZE1, IMG_SIZE3);
    Size imgSize2(IMG_SIZE3, IMG_SIZE1);
    Size minSize(IMG_SIZE2, IMG_SIZE2);
    Size maxSize(IMG_SIZE4, IMG_SIZE4);
    APP_ERROR ret = g_imageProcessor.imageProcessorDptr_->CheckImageWH(imgSize1, minSize, maxSize);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    ret = g_imageProcessor.imageProcessorDptr_->CheckImageWH(imgSize2, minSize, maxSize);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_CheckOutputImage_Should_Return_Success_When_Param_Valid)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.imageProcessorDptr_->CheckOutputImage(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_CheckOutputImage_Should_Return_Fail_When_Host)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
    decodedImage1.ToHost();
    ret = g_imageProcessor.imageProcessorDptr_->CheckOutputImage(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_CheckOutputImage_Should_Return_Fail_When_GetDeviceId_Invalid)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
    MOCKER_CPP(&Image::GetDeviceId).stubs().will(returnValue(1));
    ret = g_imageProcessor.imageProcessorDptr_->CheckOutputImage(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_CheckOutputImage_Should_Return_Success_When_RGB_888)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1, ImageFormat::RGB_888);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.imageProcessorDptr_->CheckOutputImage(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImageInput_Should_Return_Success_When_Param_Valid)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.imageProcessorDptr_->CheckVPCImageInput(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImageInput_Should_Return_Success_When_RGB_888)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1, ImageFormat::RGB_888);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.imageProcessorDptr_->CheckVPCImageInput(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImageInput_Should_Return_Fail_When_Input_Host)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
    decodedImage1.ToHost();
    ret = g_imageProcessor.imageProcessorDptr_->CheckVPCImageInput(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImageInput_Should_Return_Fail_When_CheckImageWH_Fail)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
    MOCKER_CPP(&ImageProcessorDptr::CheckImageWH).stubs().will(returnValue(1));
    ret = g_imageProcessor.imageProcessorDptr_->CheckVPCImageInput(decodedImage1);
    EXPECT_EQ(ret, 1);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImageInput_Should_Return_Fail_When_When_GetDeviceId_Invalid)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
    MOCKER_CPP(&Image::GetDeviceId).stubs().will(returnValue(1));
    ret = g_imageProcessor.imageProcessorDptr_->CheckVPCImageInput(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImageInput_Should_Return_Fail_When_When_GetData_Nullptr)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::shared_ptr<uint8_t> data = nullptr;
    MOCKER_CPP(&Image::GetData).stubs().will(returnValue(data));
    ret = g_imageProcessor.imageProcessorDptr_->CheckVPCImageInput(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImageInput_Should_Return_Success_When_Not_310P)
{
    ImageProcessor imageProcessor1;
    Image decodedImage1;
    APP_ERROR ret = imageProcessor1.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);

    MOCKER_CPP(&ImageProcessorDptr::IsAscend310POrAtlas800IA2).stubs().will(returnValue(false));
    imageProcessor1.imageProcessorDptr_->vpcMode_ = false;
    ret = imageProcessor1.imageProcessorDptr_->CheckVPCImageInput(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImageOutput_Should_Return_Success_When_Param_Valid)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.imageProcessorDptr_->CheckVPCImageOutput(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImageOutput_Should_Return_Fail_When_CheckImageWH_Fail)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
    MOCKER_CPP(&ImageProcessorDptr::CheckImageWH).stubs().will(returnValue(1));
    ret = g_imageProcessor.imageProcessorDptr_->CheckVPCImageOutput(decodedImage1);
    EXPECT_EQ(ret, 1);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImageOutput_Should_Return_Success_When_RGB_888)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1, ImageFormat::RGB_888);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_imageProcessor.imageProcessorDptr_->CheckVPCImageOutput(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImageOutput_Should_Return_Fail_When_Input_Host)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
    decodedImage1.ToHost();
    ret = g_imageProcessor.imageProcessorDptr_->CheckVPCImageOutput(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImageOutput_Should_Return_Success_When_Not_310P)
{
    ImageProcessor imageProcessor1;
    Image decodedImage1;
    APP_ERROR ret = imageProcessor1.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);

    MOCKER_CPP(&ImageProcessorDptr::IsAscend310POrAtlas800IA2).stubs().will(returnValue(false));
    imageProcessor1.imageProcessorDptr_->vpcMode_ = false;
    ret = imageProcessor1.imageProcessorDptr_->CheckVPCImageOutput(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImage_Should_Return_Success_When_Param_Valid)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
    Image decodedImage2;
    ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage2);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.imageProcessorDptr_->CheckVPCImage(decodedImage1, decodedImage2);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_CheckVPCImage_Should_Return_Fail_When_input_Host)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
    decodedImage1.ToHost();
    Image decodedImage2;
    ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage2);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.imageProcessorDptr_->CheckVPCImage(decodedImage1, decodedImage2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_Decode_Should_Return_Fail_When_Path_Invalid)
{
    std::string path = "";
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(path, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_Decode_Should_Return_Fail_When_310)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).stubs().will(returnValue(true));
    MOCKER_CPP(&DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);

    Image decodedImage2;
    ret = g_imageProcessor.Decode(PNG_IMAGE_PATH, decodedImage2);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(ImageProcessorTest, Test_CheckFormat_Should_Return_Fail_When_GetSocName_Invalid)
{
    Image decodedImage1;
    std::string result = "310X";
    MOCKER_CPP(&DeviceManager::GetSocName).stubs().will(returnValue(result));
    APP_ERROR ret = g_imageProcessor.imageProcessorDptr_->CheckFormat(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_InitEncodeChannel_Should_Return_Fail_When_GetSocName_Invalid)
{
    ImageProcessor imageProcessor;
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->InitEncodeChannel(0);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_DecodePrototype1_Should_Return_Success_When_ImageFormat_Is_YUV420)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);

    Image decodedImage2;
    ret = g_imageProcessor.Decode(PNG_IMAGE_PATH, decodedImage2);
    EXPECT_EQ(ret, APP_ERR_OK);
}


TEST_F(ImageProcessorTest, Test_DecodePrototype1_Should_Return_Success_When_ImageFormat_Is_YVU420)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1, ImageFormat::YVU_SP_420);
    EXPECT_EQ(ret, APP_ERR_OK);

    Image decodedImage2;
    ret = g_imageProcessor.Decode(PNG_IMAGE_PATH, decodedImage2, ImageFormat::YVU_SP_420);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_DecodePrototype1_Should_Return_Success_When_ImageFormat_Is_RGB888)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1, ImageFormat::RGB_888);

    if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    Image decodedImage2;
    ret = g_imageProcessor.Decode(PNG_IMAGE_PATH, decodedImage2, ImageFormat::RGB_888);
    if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(ImageProcessorTest, Test_DecodePrototype1_Should_Return_Success_When_ImageFormat_Is_BGR888)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1, ImageFormat::BGR_888);
    if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    Image decodedImage2;
    ret = g_imageProcessor.Decode(PNG_IMAGE_PATH, decodedImage2, ImageFormat::BGR_888);
    if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(ImageProcessorTest, Test_DecodePrototype2_Should_Return_Success_When_ImageFormat_Is_YUV420)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(g_encodedJPGDataPtr, g_encodedJPGDataSize, decodedImage1,
                                            ImageFormat::YUV_SP_420);
    EXPECT_EQ(ret, APP_ERR_OK);

    Image decodedImage2;
    ret = g_imageProcessor.Decode(g_encodedPNGDataPtr, g_encodedPNGDataSize, decodedImage2, ImageFormat::YUV_SP_420);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_DecodePrototype2_Should_Return_Success_When_ImageFormat_Is_YVU420)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(g_encodedJPGDataPtr, g_encodedJPGDataSize, decodedImage1,
                                            ImageFormat::YVU_SP_420);
    EXPECT_EQ(ret, APP_ERR_OK);

    Image decodedImage2;
    ret = g_imageProcessor.Decode(g_encodedPNGDataPtr, g_encodedPNGDataSize, decodedImage2, ImageFormat::YVU_SP_420);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_DecodePrototype2_Should_Return_Success_When_ImageFormat_Is_RGB888)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(g_encodedJPGDataPtr, g_encodedJPGDataSize, decodedImage1,
                                            ImageFormat::RGB_888);
    if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    Image decodedImage2;
    ret = g_imageProcessor.Decode(g_encodedPNGDataPtr, g_encodedPNGDataSize, decodedImage2, ImageFormat::RGB_888);
    if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(ImageProcessorTest, Test_DecodePrototype2_Should_Return_Success_When_ImageFormat_Is_BGR888)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(g_encodedJPGDataPtr, g_encodedJPGDataSize, decodedImage1,
                                            ImageFormat::BGR_888);
    if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    Image decodedImage2;
    ret = g_imageProcessor.Decode(g_encodedPNGDataPtr, g_encodedPNGDataSize, decodedImage2, ImageFormat::BGR_888);
    if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(ImageProcessorTest, Test_Encode_Should_Return_Fail_When_savePath_Empty)
{
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.Encode(g_decodedYUV420Image, "");
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_Encode_Should_Return_Fail_When_savePath_Without_Point)
{
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.Encode(g_decodedYUV420Image, "Encode_YUV420jpg");
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_Encode_Should_Return_Fail_When_savePath_Png)
{
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.Encode(g_decodedYUV420Image, "Encode_YUV420.png");
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_EncodePrototype1_Should_Return_Success_When_All_Prepared)
{
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.Encode(g_decodedYUV420Image, "Encode_YUV420.jpg");
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_EncodePrototype1_Should_Return_Fail_When_Image_Is_On_Host)
{
    APP_ERROR ret = g_imageProcessor.Encode(g_decodedYUV420Image, "Encode_YUV420.jpg");
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_EncodePrototype1_Should_Return_Fail_When_Image_Is_Not_On_ImageProcessor_Device)
{
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ONE);
    APP_ERROR ret = g_imageProcessor.Encode(g_decodedYUV420Image, "Encode_YUV420.jpg");
    EXPECT_NE(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_EncodePrototype2_Should_Return_Success_When_All_Prepared)
{
    std::shared_ptr<uint8_t> outDataPtr;
    uint32_t outDataSize;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.Encode(g_decodedYUV420Image, outDataPtr, outDataSize);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_NE(outDataPtr, nullptr);
    EXPECT_NE(outDataSize, 0);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_EncodePrototype2_Should_Return_Success_When_Image_Is_On_Host)
{
    std::shared_ptr<uint8_t> outDataPtr;
    uint32_t outDataSize;
    APP_ERROR ret = g_imageProcessor.Encode(g_decodedYUV420Image, outDataPtr, outDataSize);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_EncodePrototype2_Should_Return_Success_When_Image_Is_Not_On_ImageProcessor_Device)
{
    std::shared_ptr<uint8_t> outDataPtr;
    uint32_t outDataSize;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ONE);
    APP_ERROR ret = g_imageProcessor.Encode(g_decodedYUV420Image, outDataPtr, outDataSize);
    EXPECT_NE(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_EncodePrototype1_Should_Return_Success_When_Decode_RGB888)
{
    ImageProcessor imageProcessor;
    Image decodeImage;

    APP_ERROR ret = imageProcessor.Decode("./test.JPG", decodeImage, ImageFormat::RGB_888);
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = imageProcessor.Encode(decodeImage, "./out.jpg");
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_EncodePrototype2_Should_Return_Success_When_Decode_RGB888)
{
    ImageProcessor imageProcessor;
    Image decodeImage;

    APP_ERROR ret = imageProcessor.Decode("./test.JPG", decodeImage, ImageFormat::RGB_888);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::shared_ptr<uint8_t> outDataPtr;
    uint32_t outDataSize;
    ret = imageProcessor.Encode(decodeImage, outDataPtr, outDataSize);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_ResizePrototype1_Should_Return_Success_When_All_Prepared)
{
    Image resizedImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.Resize(g_decodedYUV420Image, RESIZED_SIZE, resizedImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_Resize_Should_Return_Success_When_Output_Not_Empty)
{
    Image resizedImage;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, resizedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    ret = g_imageProcessor.Resize(g_decodedYUV420Image, RESIZED_SIZE, resizedImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_ResizePrototype1_Should_Return_Fail_When_Image_Is_On_Host)
{
    Image resizedImage;
    APP_ERROR ret = g_imageProcessor.Resize(g_decodedYUV420Image, RESIZED_SIZE, resizedImage);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_ResizePrototype1_Should_Return_Fail_When_Image_Is_Not_On_ImageProcessor_Device)
{
    Image resizedImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ONE);
    APP_ERROR ret = g_imageProcessor.Resize(g_decodedYUV420Image, RESIZED_SIZE, resizedImage);
    EXPECT_NE(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_ResizePrototype2_Should_Return_Success_When_All_Prepared)
{
    AscendStream ascendStream(DEVICE_ID_ZERO);
    ascendStream.CreateAscendStream();
    Image resizedImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.Resize(g_decodedYUV420Image, RESIZED_SIZE, resizedImage,
                                            Interpolation::HUAWEI_HIGH_ORDER_FILTER, ascendStream);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = ascendStream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
    ascendStream.DestroyAscendStream();
}

TEST_F(ImageProcessorTest, Test_ResizePrototype2_Should_Return_Fail_When_Image_Is_On_Host)
{
    AscendStream ascendStream(DEVICE_ID_ZERO);
    ascendStream.CreateAscendStream();
    Image resizedImage;
    APP_ERROR ret = g_imageProcessor.Resize(g_decodedYUV420Image, RESIZED_SIZE, resizedImage,
                                            Interpolation::HUAWEI_HIGH_ORDER_FILTER, ascendStream);
    EXPECT_NE(ret, APP_ERR_OK);
    ascendStream.DestroyAscendStream();
}

TEST_F(ImageProcessorTest, Test_ResizePrototype2_Should_Return_Fail_When_Image_Is_Not_On_ImageProcessor_Device)
{
    AscendStream ascendStream(DEVICE_ID_ZERO);
    ascendStream.CreateAscendStream();
    Image resizedImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ONE);
    APP_ERROR ret = g_imageProcessor.Resize(g_decodedYUV420Image, RESIZED_SIZE, resizedImage,
                                            Interpolation::HUAWEI_HIGH_ORDER_FILTER, ascendStream);
    EXPECT_NE(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
    ascendStream.DestroyAscendStream();
}

TEST_F(ImageProcessorTest, Test_Padding_Should_Return_Success_When_All_Prepared)
{
    Image paddedImage;
    Dim dim(PADDING_DIM, PADDING_DIM, PADDING_DIM, PADDING_DIM);
    Color color(PADDING_COLOR, PADDING_COLOR, PADDING_COLOR);
    BorderType borderType = MxBase::BorderType::BORDER_CONSTANT;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.Padding(g_decodedYUV420Image, dim, color, borderType, paddedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    borderType = MxBase::BorderType::BORDER_REPLICATE;
    ret = g_imageProcessor.Padding(g_decodedYUV420Image, dim, color, borderType, paddedImage);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    borderType = MxBase::BorderType::BORDER_REFLECT;
    ret = g_imageProcessor.Padding(g_decodedYUV420Image, dim, color, borderType, paddedImage);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    borderType = MxBase::BorderType::BORDER_REFLECT_101;
    ret = g_imageProcessor.Padding(g_decodedYUV420Image, dim, color, borderType, paddedImage);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    borderType = static_cast<MxBase::BorderType>(INVALID_BORDERTYPE);
    ret = g_imageProcessor.Padding(g_decodedYUV420Image, dim, color, borderType, paddedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_ConvertDvppDataInfoToImage_Should_Return_Success_When_Image_Not_Empty)
{
    Image decodedImage1;
    APP_ERROR ret = g_imageProcessor.Decode(JPG_IMAGE_PATH, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);

    DvppDataInfo inputDvppDataInfo;
    ret = g_imageProcessor.imageProcessorDptr_->ConvertDvppDataInfoToImage(inputDvppDataInfo, decodedImage1);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_CropPrototype1_Should_Return_Success_When_All_Prepared)
{
    Image croppedImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.Crop(g_decodedYUV420Image, CROP_RECT2, croppedImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropPrototype2_Should_Return_Success_When_All_Prepared)
{
    Image croppedImage;
    MxBase::AscendStream stream = MxBase::AscendStream(DEVICE_ID_ZERO);
    stream.CreateAscendStream();
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);

    APP_ERROR ret = g_imageProcessor.Crop(g_decodedYUV420Image, CROP_RECT2, croppedImage, stream);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropPrototype3_Should_Return_Success_When_All_Prepared)
{
    std::vector<Image> croppedImageVec;
    croppedImageVec.resize(CROP_CONFIG_SIZE);
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.Crop(g_decodedYUV420Image, g_cropRectVec, croppedImageVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropPrototype4_Should_Return_Success_When_All_Prepared)
{
    std::vector<Image> croppedImageVec;
    croppedImageVec.resize(CROP_CONFIG_SIZE);
    MxBase::AscendStream stream = MxBase::AscendStream(DEVICE_ID_ZERO);
    stream.CreateAscendStream();
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);

    APP_ERROR ret = g_imageProcessor.Crop(g_decodedYUV420Image, g_cropRectVec, croppedImageVec, stream);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropPrototype5_Should_Return_Success_When_All_Prepared)
{
    std::vector<Image>inputImageVec;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    inputImageVec.resize(CROP_INPUT_SIZE);
    for (size_t i = 0; i < inputImageVec.size(); i++) {
        inputImageVec[i] = g_decodedYUV420Image;
    }
    std::vector<Image> croppedImageVec;
    croppedImageVec.resize(inputImageVec.size() * g_cropRectVec.size());

    APP_ERROR ret = g_imageProcessor.Crop(inputImageVec, g_cropRectVec, croppedImageVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_Crop_Should_Return_Fail_When_VecSize_Not_Same)
{
    std::vector<Image>inputImageVec;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    inputImageVec.resize(CROP_INPUT_SIZE);
    for (size_t i = 0; i < inputImageVec.size(); i++) {
        inputImageVec[i] = g_decodedYUV420Image;
    }
    std::vector<Image> croppedImageVec;
    croppedImageVec.resize(inputImageVec.size());

    APP_ERROR ret = g_imageProcessor.Crop(inputImageVec, g_cropRectVec, croppedImageVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropPrototype6_Should_Return_Success_When_All_Prepared)
{
    std::vector<Image>inputImageVec;
    inputImageVec.resize(CROP_INPUT_SIZE);
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    for (size_t i = 0; i < inputImageVec.size(); i++) {
        inputImageVec[i] = g_decodedYUV420Image;
    }
    std::vector<Image> croppedImageVec;
    croppedImageVec.resize(inputImageVec.size() * g_cropRectVec.size());
    AscendStream ascendStream(DEVICE_ID_ZERO);
    ascendStream.CreateAscendStream();

    APP_ERROR ret = g_imageProcessor.Crop(inputImageVec, g_cropRectVec, croppedImageVec, ascendStream);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = ascendStream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = ascendStream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropResizePrototype1_Should_Return_Success_When_All_Prepared)
{
    std::vector<Image> cropResizedImageVec0(IMAGE_VECTOR_SIZE);
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);

    APP_ERROR ret = g_imageProcessor.CropResize(g_decodedYUV420Image, g_cropRectVec, CROP_RELATED_SIZE,
                                                cropResizedImageVec0);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropResizePrototype2_Should_Return_Success_When_All_Prepared)
{
    std::vector<Image> cropResizedImageVec0(IMAGE_VECTOR_SIZE);
    MxBase::AscendStream stream = MxBase::AscendStream(DEVICE_ID_ZERO);
    stream.CreateAscendStream();
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);

    APP_ERROR ret = g_imageProcessor.CropResize(g_decodedYUV420Image, g_cropRectVec, CROP_RELATED_SIZE,
                                                cropResizedImageVec0, stream);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropResizePrototype3_Should_Return_Success_When_All_Prepared)
{
    std::vector<Image> cropResizedImageVec(IMAGE_VECTOR_SIZE);
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);

    APP_ERROR ret = g_imageProcessor.CropResize(g_decodedYUV420Image, g_cropResizeVec, cropResizedImageVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropResizePrototype4_Should_Return_Success_When_All_Prepared)
{
    std::vector<Image> cropResizedImageVec(IMAGE_VECTOR_SIZE);
    MxBase::AscendStream stream = MxBase::AscendStream(DEVICE_ID_ZERO);
    stream.CreateAscendStream();
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);

    APP_ERROR ret = g_imageProcessor.CropResize(g_decodedYUV420Image, g_cropResizeVec, cropResizedImageVec, stream);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropResizePrototype5_Should_Return_Success_When_All_Prepared)
{
    std::vector<Image> inputImageVec(IMAGE_VECTOR_SIZE);
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    for (size_t i = 0; i < inputImageVec.size(); i++) {
        inputImageVec[i] = g_decodedYUV420Image;
    }
    std::vector<Image> cropResizedImageVec2(IMAGE_VECTOR_SIZE);

    APP_ERROR ret = g_imageProcessor.CropResize(inputImageVec, g_cropResizeVec, cropResizedImageVec2);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropResize_Should_Return_Fail_When_VecSize_Not_Same)
{
    std::vector<Image> inputImageVec(IMAGE_VECTOR_SIZE);
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    for (size_t i = 0; i < inputImageVec.size(); i++) {
    inputImageVec[i] = g_decodedYUV420Image;
    }
    std::vector<Image> cropResizedImageVec2(1);

    APP_ERROR ret = g_imageProcessor.CropResize(inputImageVec, g_cropResizeVec, cropResizedImageVec2);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropResizePrototype6_Should_Return_Success_When_All_Prepared)
{
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    std::vector<Image> inputImageVec(IMAGE_VECTOR_SIZE);
    for (size_t i = 0; i < inputImageVec.size(); i++) {
        inputImageVec[i] = g_decodedYUV420Image;
    }
    std::vector<Image> cropResizedImageVec2(IMAGE_VECTOR_SIZE);
    AscendStream ascendStream(DEVICE_ID_ZERO);
    ascendStream.CreateAscendStream();

    APP_ERROR ret = g_imageProcessor.CropResize(inputImageVec, g_cropResizeVec, cropResizedImageVec2, ascendStream);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = ascendStream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = ascendStream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropAndPastePrototype1_Should_Return_Success_When_All_Prepared)
{
    Image resizedImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.Resize(g_decodedYUV420Image, CROP_RELATED_SIZE, resizedImage);

    ret = g_imageProcessor.CropAndPaste(g_decodedYUV420Image, CROP_PASTE_PAIR, resizedImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_CropAndPastePrototype2_Should_Return_Success_When_All_Prepared)
{
    AscendStream Stream = AscendStream(DEVICE_ID_ZERO);
    Stream.CreateAscendStream();
    Image resizedImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.Resize(g_decodedYUV420Image, CROP_RELATED_SIZE, resizedImage);

    ret = g_imageProcessor.CropAndPaste(g_decodedYUV420Image, CROP_PASTE_PAIR, resizedImage, Stream);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = Stream.Synchronize();
    EXPECT_EQ(ret, APP_ERR_OK);
    Stream.DestroyAscendStream();
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_ConvertFormat_Should_Return_Success_From_YUV420_When_All_Prepared)
{
    Image YUVImage;
    Image RGBImage;
    Image BGRImage;
    Image YVUImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.ConvertFormat(g_decodedYUV420Image, ImageFormat::RGB_888, RGBImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(g_decodedYUV420Image, ImageFormat::BGR_888, BGRImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(g_decodedYUV420Image, ImageFormat::YVU_SP_420, YVUImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_ConvertFormat_Should_Return_Fail_From_YUV420_When_Dst_Image_Format_Is_The_Same)
{
    Image sameFormatImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.ConvertFormat(g_decodedYUV420Image, ImageFormat::YUV_SP_420, sameFormatImage);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_ConvertFormat_Should_Return_Success_From_YVU420_When_All_Prepared)
{
    Image YUVImage;
    Image RGBImage;
    Image BGRImage;
    Image YVUImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.ConvertFormat(g_decodedYUV420Image, ImageFormat::YVU_SP_420, YVUImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(YVUImage, ImageFormat::YUV_SP_420, YUVImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(YVUImage, ImageFormat::BGR_888, BGRImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(YVUImage, ImageFormat::RGB_888, RGBImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_ConvertFormat_Should_Return_Fail_From_YVU420_When_Dst_Image_Format_Is_The_Same)
{
    Image YVUImage;
    Image sameFormatImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);
    APP_ERROR ret = g_imageProcessor.ConvertFormat(g_decodedYUV420Image, ImageFormat::YVU_SP_420, YVUImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(YVUImage, ImageFormat::YVU_SP_420, sameFormatImage);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_ConvertFormat_Should_Return_Success_From_RGB888_When_All_Prepared)
{
    Image YUVImage;
    Image RGBImage;
    Image BGRImage;
    Image YVUImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);

    APP_ERROR ret = g_imageProcessor.ConvertFormat(g_decodedYUV420Image, ImageFormat::RGB_888, RGBImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(RGBImage, ImageFormat::YUV_SP_420, YUVImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(RGBImage, ImageFormat::BGR_888, BGRImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(RGBImage, ImageFormat::YVU_SP_420, YVUImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_ConvertFormat_Should_Return_Fail_From_RGB888_When_Dst_Image_Format_Is_The_Same)
{
    Image sameFormatImage;
    Image RGBImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);

    APP_ERROR ret = g_imageProcessor.ConvertFormat(g_decodedYUV420Image, ImageFormat::RGB_888, RGBImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(RGBImage, ImageFormat::RGB_888, sameFormatImage);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_ConvertFormat_Should_Return_Success_From_BGR888_When_All_Prepared)
{
    Image YUVImage;
    Image RGBImage;
    Image BGRImage;
    Image YVUImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);

    APP_ERROR ret = g_imageProcessor.ConvertFormat(g_decodedYUV420Image, ImageFormat::BGR_888, BGRImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(BGRImage, ImageFormat::YUV_SP_420, YUVImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(BGRImage, ImageFormat::RGB_888, RGBImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(BGRImage, ImageFormat::YVU_SP_420, YVUImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_ConvertFormat_Should_Return_Fail_From_BGR888_When_Dst_Image_Format_Is_The_Same)
{
    Image sameFormatImage;
    Image BGRImage;
    g_decodedYUV420Image.ToDevice(DEVICE_ID_ZERO);

    APP_ERROR ret = g_imageProcessor.ConvertFormat(g_decodedYUV420Image, ImageFormat::BGR_888, BGRImage);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_imageProcessor.ConvertFormat(BGRImage, ImageFormat::BGR_888, sameFormatImage);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    g_decodedYUV420Image.ToHost();
}

TEST_F(ImageProcessorTest, Test_InitVpcChannel_Should_Return_Success_When_Not_310P)
{
    ImageProcessor imageProcessor;
    imageProcessor.imageProcessorDptr_->vpcMode_ = false;
    MOCKER_CPP(&ImageProcessorDptr::IsAscend310POrAtlas800IA2).stubs().will(returnValue(false));
    VpcChnConfig config = VPC_CHN_CONFIG;
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->InitVpcChannel(config);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_PngPredictDecSize_Should_Return_Fail_When_imageData_null)
{
    ImageProcessor imageProcessor;
    uint32_t decSize = 0;
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->PngPredictDecSize(nullptr,
        0, MxbasePixelFormat::MXBASE_PIXEL_FORMAT_ABGR_8888, decSize);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
}

TEST_F(ImageProcessorTest, Test_CheckJpegDecodeFormat_Should_Return_Fail_When_YUV_400)
{
    ImageProcessor imageProcessor;
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->CheckJpegDecodeFormat(ImageFormat::YUV_400);
    EXPECT_EQ(ret, APP_ERR_DVPP_INVALID_FORMAT);
}

TEST_F(ImageProcessorTest, Test_CheckJpegDecodeFormat_Should_Return_Fail_When_YUV_400_Not_310P)
{
    ImageProcessor imageProcessor;
    MOCKER_CPP(&ImageProcessorDptr::IsAscend310POrAtlas800IA2).stubs().will(returnValue(false));
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->CheckJpegDecodeFormat(ImageFormat::YUV_400);
    EXPECT_EQ(ret, APP_ERR_DVPP_INVALID_FORMAT);
}

TEST_F(ImageProcessorTest, Test_GetPngDec_Should_Return_Fail_When_Input_Empty)
{
    DvppImageInfo imageInfo;
    DvppImageOutput imageOutput;
    ImageProcessor imageProcessor;
    ImageFormat decodeFormat = ImageFormat::YUV_400;
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->GetPngDec(imageInfo, imageOutput, decodeFormat);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_GetPngDec_Should_Return_Fail_When_Input_Empty_Not_310P)
{
    DvppImageInfo imageInfo;
    DvppImageOutput imageOutput;
    ImageProcessor imageProcessor;
    ImageFormat decodeFormat = ImageFormat::YUV_400;
    MOCKER_CPP(&ImageProcessorDptr::IsAscend310POrAtlas800IA2).stubs().will(returnValue(false));
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->GetPngDec(imageInfo, imageOutput, decodeFormat);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_GetPngDec_Should_Return_Success_When_GRAY_CHANNEL)
{
    DvppImageInfo imageInfo;
    DvppImageOutput imageOutput;
    ImageProcessor imageProcessor;
    ImageFormat decodeFormat = ImageFormat::YUV_400;
    imageOutput.components = static_cast<int32_t>(PngChannelType::GRAY_CHANNEL);
    MOCKER_CPP(&DvppWrapper::GetPictureDec).stubs().will(returnValue(0));
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->GetPngDec(imageInfo, imageOutput, decodeFormat);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_GetPngDec_Should_Return_Success_When_AGRAY_CHANNEL)
{
    DvppImageInfo imageInfo;
    DvppImageOutput imageOutput;
    ImageProcessor imageProcessor;
    ImageFormat decodeFormat = ImageFormat::YUV_400;
    imageOutput.components = static_cast<int32_t>(PngChannelType::AGRAY_CHANNEL);
    MOCKER_CPP(&DvppWrapper::GetPictureDec).stubs().will(returnValue(0));
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->GetPngDec(imageInfo, imageOutput, decodeFormat);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_GetPngDec_Should_Return_Success_When_RGBA_COLOR_CHANNEL)
{
    DvppImageInfo imageInfo;
    DvppImageOutput imageOutput;
    ImageProcessor imageProcessor;
    ImageFormat decodeFormat = ImageFormat::YUV_400;
    imageOutput.components = static_cast<int32_t>(PngChannelType::RGBA_COLOR_CHANNEL);
    MOCKER_CPP(&DvppWrapper::GetPictureDec).stubs().will(returnValue(0));
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->GetPngDec(imageInfo, imageOutput, decodeFormat);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageProcessorTest, Test_GetPngDec_Should_Return_Fail_When_components_Invalid)
{
    DvppImageInfo imageInfo;
    DvppImageOutput imageOutput;
    ImageProcessor imageProcessor;
    ImageFormat decodeFormat = ImageFormat::YUV_400;
    imageOutput.components = INVALID_COMPONENT;
    MOCKER_CPP(&DvppWrapper::GetPictureDec).stubs().will(returnValue(0));
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->GetPngDec(imageInfo, imageOutput, decodeFormat);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_CheckEncodeFormat_Should_Return_Fail_When_Img_Empty)
{
    ImageProcessor imageProcessor;
    Image decodedImage1;

    APP_ERROR ret = imageProcessor.imageProcessorDptr_->CheckEncodeFormat(decodedImage1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_CheckRawFormat_Should_Return_Fail_When_dataPtr_Nullptr)
{
    ImageProcessor imageProcessor;
    Image decodedImage1;

    std::shared_ptr<uint8_t> dataPtr = nullptr;
    uint32_t dataSize = 1;
    RawFormat rawFormat;
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->CheckRawFormat(dataPtr, dataSize, rawFormat);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_CheckRawFormat_Should_Return_Fail_When_dataSize_Too_Large)
{
    ImageProcessor imageProcessor;
    Image decodedImage1;

    std::shared_ptr<uint8_t> dataPtr = std::make_shared<uint8_t>(1);
    uint32_t dataSize = EXCEED_MAX_DATA_SIZE;
    RawFormat rawFormat;
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->CheckRawFormat(dataPtr, dataSize, rawFormat);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_ConvertColor_Should_Return_Fail_When_Input_Empty)
{
    ImageProcessor imageProcessor;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    MOCKER_CPP(&ImageProcessorDptr::IsAscend310POrAtlas800IA2).stubs().will(returnValue(false));
    imageProcessor.imageProcessorDptr_->vpcMode_ = false;
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->ConvertColor(inputDataInfo, outputDataInfo);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, Test_ConvertColor_Should_Return_Fail_When_Input_Empty_Not_310P)
{
    ImageProcessor imageProcessor;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    MOCKER_CPP(&ImageProcessorDptr::IsAscend310POrAtlas800IA2).stubs().will(returnValue(false));
    imageProcessor.imageProcessorDptr_->vpcMode_ = false;
    APP_ERROR ret = imageProcessor.imageProcessorDptr_->ConvertColor(inputDataInfo, outputDataInfo);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageProcessorTest, TestImageProcessorInit)
{
    ImageProcessor imageProcessorDecode(DEVICE_ID_ZERO);
    ImageProcessor imageProcessorVpc(DEVICE_ID_ZERO);
    ImageProcessor imageProcessorEncode(DEVICE_ID_ZERO);
    ImageProcessor imageProcessorPng(DEVICE_ID_ZERO);

    Image decodeImage;
    APP_ERROR ret = imageProcessorDecode.InitJpegDecodeChannel();
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = imageProcessorDecode.Decode("./test.JPG", decodeImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = imageProcessorEncode.InitJpegEncodeChannel();
    ret = imageProcessorEncode.Encode(decodeImage, "./out.jpg");
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = imageProcessorVpc.InitVpcChannel();
    EXPECT_EQ(ret, APP_ERR_OK);
    Image resizeImage;
    const int sizeResize = 320;
    ret = imageProcessorVpc.Resize(decodeImage, Size(sizeResize, sizeResize), resizeImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = imageProcessorPng.InitPngDecodeChannel();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = imageProcessorDecode.InitJpegDecodeChannel();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = imageProcessorEncode.InitJpegEncodeChannel();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = imageProcessorVpc.InitVpcChannel();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = imageProcessorPng.InitPngDecodeChannel();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = imageProcessorVpc.InitJpegDecodeChannel();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = imageProcessorVpc.InitJpegEncodeChannel();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = imageProcessorVpc.InitVpcChannel();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = imageProcessorVpc.InitPngDecodeChannel();
    EXPECT_EQ(ret, APP_ERR_OK);
}
}

int main(int argc, char *argv[])
{
    MxInit();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
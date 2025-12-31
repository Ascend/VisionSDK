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
#include <cstring>
#include <fstream>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <opencv2/opencv.hpp>
#include "acl/acl.h"
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#define private public
#include "MxBase/MxBase.h"
#undef private

using namespace std;
using namespace MxBase;
namespace {
class ImageTest : public testing::Test {
public:
    void TearDown() override
    {
        // clear mock
        GlobalMockObject::verify();
    }
};
const int ALIGNED_IMG_SIZE = 32;
const int UNALIGNED_IMG_SIZE = 33;
const uint32_t EXCEED_MAX_DATA_SIZE = 268435457;
const int ALIGNED_SIZE = 640;
const int ORIGINAL_SIZE = 635;
const int RGB_CHANNEL = 3;
const int DATA_SIZE = 96;
const int VALID_DATA_SIZE = 12;
const int HEIGHT1 = 2;
const int WIDTH1 = 2;
const int CHANNEL1 = 3;
const int ALIGN_HEIGHT = 2;
const int ALIGN_WIDTH = 16;
APP_ERROR ReadBinFromFile(const std::string path, std::shared_ptr<uint8_t>& data, size_t& dataSize)
{
    std::ifstream file(path.c_str(), std::ifstream::binary);
    if (!file) {
        LogError << "Cannot open the image file." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    file.seekg(0, std::ifstream::end);
    uint32_t fileSize = file.tellg();
    file.seekg(0);
    auto dataPtr = new (std::nothrow) char[fileSize + 1];
    if (dataPtr == nullptr) {
        LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        file.close();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    auto deleter = [](uint8_t* p) { delete[] p; };
    data.reset((uint8_t*)dataPtr, deleter);
    file.read(dataPtr, fileSize);
    file.close();
    dataSize = fileSize;
    return APP_ERR_OK;
}

TEST_F(ImageTest, Test_ImageConstructor_With_imageSize_Should_Return_Fail_When_Input_Nullptr)
{
    APP_ERROR ret = APP_ERR_OK;
    std::shared_ptr<uint8_t> data = nullptr;
    uint32_t dataSize = 1;
    int32_t deviceId = 0;
    Size imageSize;
    try {
        Image image(data, dataSize, deviceId, imageSize, ImageFormat::RGB_888);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_ALLOC_MEM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_ALLOC_MEM);
}

TEST_F(ImageTest, Test_ImageConstructor_With_imageSizeInfo_Should_Return_Fail_When_Input_Nullptr)
{
    APP_ERROR ret = APP_ERR_OK;
    std::shared_ptr<uint8_t> data = nullptr;
    uint32_t dataSize = 1;
    int32_t deviceId = 0;
    Size imageSize;
    std::pair<Size, Size> imageSizeInfo = std::make_pair(imageSize, imageSize);
    try {
        Image image(data, dataSize, deviceId, imageSizeInfo, ImageFormat::RGB_888);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_ALLOC_MEM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_ALLOC_MEM);
}

TEST_F(ImageTest, Test_ImageConstructor_With_imageSizeInfo_Should_Return_Fail_When_aclrtGetDeviceCount_Fail)
{
    MOCKER_CPP(&aclrtGetDeviceCount).stubs().will(returnValue(1));
    APP_ERROR ret = APP_ERR_OK;
    std::shared_ptr<uint8_t> data = std::make_shared<uint8_t>(1);
    uint32_t dataSize = 1;
    int32_t deviceId = 0;
    Size imageSize;
    std::pair<Size, Size> imageSizeInfo = std::make_pair(imageSize, imageSize);
    try {
        Image image(data, dataSize, deviceId, imageSizeInfo, ImageFormat::RGB_888);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_ALLOC_MEM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_ALLOC_MEM);
}

TEST_F(ImageTest, Test_ImageConstructor_With_imageSizeInfo_Should_Return_Fail_When_DeviceId_Valid)
{
    APP_ERROR ret = APP_ERR_OK;
    std::shared_ptr<uint8_t> data = std::make_shared<uint8_t>(1);
    uint32_t dataSize = 1;
    int32_t deviceId = -2;
    Size imageSize;
    std::pair<Size, Size> imageSizeInfo = std::make_pair(imageSize, imageSize);
    try {
        Image image(data, dataSize, deviceId, imageSizeInfo, ImageFormat::RGB_888);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_ALLOC_MEM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_ALLOC_MEM);
}

TEST_F(ImageTest, Test_ImageConstructor_With_imageSizeInfo_Should_Return_Fail_When_AlignedWidth_Invalid)
{
    APP_ERROR ret = APP_ERR_OK;
    std::shared_ptr<uint8_t> data = std::make_shared<uint8_t>(1);
    uint32_t dataSize = 1;
    int32_t deviceId = 0;
    Size imageSize1(ALIGNED_IMG_SIZE, ALIGNED_IMG_SIZE);
    Size imageSize2(UNALIGNED_IMG_SIZE, ALIGNED_IMG_SIZE);
    std::pair<Size, Size> imageSizeInfo = std::make_pair(imageSize1, imageSize2);
    try {
        Image image(data, dataSize, deviceId, imageSizeInfo, ImageFormat::RGB_888);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageTest, Test_ImageConstructor_With_imageSizeInfo_Should_Return_Fail_When_AlignedHeight_Invalid)
{
    APP_ERROR ret = APP_ERR_OK;
    std::shared_ptr<uint8_t> data = std::make_shared<uint8_t>(1);
    uint32_t dataSize = 1;
    int32_t deviceId = 0;
    Size imageSize1(ALIGNED_IMG_SIZE, ALIGNED_IMG_SIZE);
    Size imageSize2(ALIGNED_IMG_SIZE, UNALIGNED_IMG_SIZE);
    std::pair<Size, Size> imageSizeInfo = std::make_pair(imageSize1, imageSize2);
    try {
        Image image(data, dataSize, deviceId, imageSizeInfo, ImageFormat::RGB_888);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageTest, Test_ImageConstructor_With_imageSizeInfo_Should_Return_Fail_When_OriginalWidth_Invalid)
{
    APP_ERROR ret = APP_ERR_OK;
    std::shared_ptr<uint8_t> data = std::make_shared<uint8_t>(1);
    uint32_t dataSize = 1;
    int32_t deviceId = 0;
    Size imageSize1(UNALIGNED_IMG_SIZE, ALIGNED_IMG_SIZE);
    Size imageSize2(ALIGNED_IMG_SIZE, ALIGNED_IMG_SIZE);
    std::pair<Size, Size> imageSizeInfo = std::make_pair(imageSize1, imageSize2);
    try {
        Image image(data, dataSize, deviceId, imageSizeInfo, ImageFormat::RGB_888);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageTest, Test_ImageConstructor_With_imageSizeInfo_Should_Return_Fail_When_OriginalHeight_Invalid)
{
    APP_ERROR ret = APP_ERR_OK;
    std::shared_ptr<uint8_t> data = std::make_shared<uint8_t>(1);
    uint32_t dataSize = 1;
    int32_t deviceId = 0;
    Size imageSize1(ALIGNED_IMG_SIZE, UNALIGNED_IMG_SIZE);
    Size imageSize2(ALIGNED_IMG_SIZE, ALIGNED_IMG_SIZE);
    std::pair<Size, Size> imageSizeInfo = std::make_pair(imageSize1, imageSize2);
    try {
        Image image(data, dataSize, deviceId, imageSizeInfo, ImageFormat::RGB_888);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageTest, Test_ImageConstructor_With_imageSizeInfo_Should_Return_Fail_When_DataSize_Exceed)
{
    APP_ERROR ret = APP_ERR_OK;
    std::shared_ptr<uint8_t> data = std::make_shared<uint8_t>(1);
    uint32_t dataSize = EXCEED_MAX_DATA_SIZE;
    int32_t deviceId = 0;
    Size imageSize1(ALIGNED_IMG_SIZE, ALIGNED_IMG_SIZE);
    Size imageSize2(ALIGNED_IMG_SIZE, ALIGNED_IMG_SIZE);
    std::pair<Size, Size> imageSizeInfo = std::make_pair(imageSize1, imageSize2);
    try {
        Image image(data, dataSize, deviceId, imageSizeInfo, ImageFormat::RGB_888);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageTest, Test_ImageConstructor_With_imageSizeInfo_Should_Return_Fail_When_DataSize_Invalid)
{
    APP_ERROR ret = APP_ERR_OK;
    std::shared_ptr<uint8_t> data = std::make_shared<uint8_t>(1);
    uint32_t dataSize = 1;
    int32_t deviceId = 0;
    Size imageSize1(ALIGNED_IMG_SIZE, ALIGNED_IMG_SIZE);
    Size imageSize2(ALIGNED_IMG_SIZE, ALIGNED_IMG_SIZE);
    std::pair<Size, Size> imageSizeInfo = std::make_pair(imageSize1, imageSize2);
    try {
        Image image(data, dataSize, deviceId, imageSizeInfo, ImageFormat::RGB_888);
    } catch (const std::exception& ex) {
        ret = APP_ERR_COMM_INVALID_PARAM;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageTest, Test_TensorCheck_Should_Return_Fail_When_InputTensor_Empty)
{
    Image image;
    Tensor inputTensor;
    APP_ERROR ret = image.TensorCheck(inputTensor);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageTest, Test_TensorCheck_Should_Return_Fail_When_InputTensor_FLOAT)
{
    Image image;
    Tensor inputTensor(std::vector<uint32_t>{1}, TensorDType::FLOAT32, 0);
    inputTensor.Malloc();
    APP_ERROR ret = image.TensorCheck(inputTensor);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageTest, Test_TensorCheck_Should_Return_Fail_When_InputTensor_ShapeSize_Invalid)
{
    Image image;
    Tensor inputTensor(std::vector<uint32_t>{1}, TensorDType::UINT8, 0);
    inputTensor.Malloc();
    APP_ERROR ret = image.TensorCheck(inputTensor);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageTest, Test_TensorCheck_Should_Return_Fail_When_InputTensor_NHWCShape_Invalid)
{
    Image image;
    Tensor inputTensor(std::vector<uint32_t>{2, 2, 2, 2}, TensorDType::UINT8, 0);
    inputTensor.Malloc();
    APP_ERROR ret = image.TensorCheck(inputTensor);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageTest, Test_TensorToImageCheck_Should_Return_Fail_When_InputTensor_Nullptr)
{
    Image image;
    Tensor inputTensor;
    APP_ERROR ret = image.TensorToImageCheck(inputTensor, ImageFormat::RGB_888);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageTest, Test_TensorToImageCheck_Should_Return_Fail_When_2Dim_RGB)
{
    Image image;
    Tensor inputTensor(std::vector<uint32_t>{2, 2}, TensorDType::UINT8, 0);
    inputTensor.Malloc();
    APP_ERROR ret = image.TensorToImageCheck(inputTensor, ImageFormat::RGB_888);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageTest, Test_TensorToImageCheck_Should_Return_Fail_When_3Dim_YUV)
{
    Image image;
    Tensor inputTensor(std::vector<uint32_t>{2, 2, 2}, TensorDType::UINT8, 0);
    inputTensor.Malloc();
    APP_ERROR ret = image.TensorToImageCheck(inputTensor, ImageFormat::YUV_400);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(ImageTest, Test_TensorToImageCheck_Should_Return_Success_When_Param_Valid)
{
    Image image;
    Tensor inputTensor(std::vector<uint32_t>{2, 2}, TensorDType::UINT8, 0);
    inputTensor.Malloc();
    APP_ERROR ret = image.TensorToImageCheck(inputTensor, ImageFormat::YUV_400);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageTest, TestImageTrans)
{
    uint32_t deviceId = 0;
    ImageProcessor imageProcessor(deviceId);
    std::string imagePath = "./test.JPG";

    Image decodedImage;
    APP_ERROR ret = imageProcessor.Decode(imagePath, decodedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    Image image(decodedImage.GetData(), decodedImage.GetDataSize());
    ret = image.ToHost();
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = image.ToDevice(0);
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = image.ToHost();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageTest, TestConvertTensor)
{
    uint32_t deviceId = 0;
    ImageProcessor imageProcessor(deviceId);
    std::string imagePath = "./test.JPG";

    Image decodedImage;
    APP_ERROR ret = imageProcessor.Decode(imagePath, decodedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    Image image(decodedImage.GetData(), decodedImage.GetDataSize());
    EXPECT_EQ(ret, APP_ERR_OK);

    Tensor tensor = image.ConvertToTensor();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageTest, ConstructImageRGB)
{
    uint32_t deviceId = 0;
    std::shared_ptr<uint8_t> data = nullptr;
    size_t dataSize = 0;
    APP_ERROR ret = ReadBinFromFile("/home/simon/models/imageRaw/decodeRGB640X640.dat", data, dataSize);
    EXPECT_EQ(ret, APP_ERR_OK);

    Size alignedSize(ALIGNED_SIZE, ALIGNED_SIZE);
    Size originalSize(ORIGINAL_SIZE, ORIGINAL_SIZE);

    std::pair<Size, Size> imageSizeInfoEqual(alignedSize, alignedSize);
    Image imageAligned(data, dataSize, -1, imageSizeInfoEqual, ImageFormat::RGB_888);
    EXPECT_EQ(imageAligned.GetOriginalSize().width, ALIGNED_SIZE);
    imageAligned.SetImageOriginalSize(Size(ORIGINAL_SIZE, ORIGINAL_SIZE));
    auto tempData = imageAligned.GetOriginalData();

    std::pair<Size, Size> imageSizeInfoNotEqual(originalSize, alignedSize);
    Image imageNotAligned(data, dataSize, -1, imageSizeInfoNotEqual, ImageFormat::RGB_888);
    EXPECT_EQ(imageNotAligned.GetOriginalSize().width, ORIGINAL_SIZE);
    tempData = imageNotAligned.GetOriginalData();

    imageNotAligned.ToDevice(deviceId);
    tempData = imageNotAligned.GetOriginalData();

    Image imageDvpp(tempData, ORIGINAL_SIZE * ORIGINAL_SIZE * RGB_CHANNEL, deviceId, imageSizeInfoNotEqual,
        ImageFormat::RGB_888);
    EXPECT_EQ(imageDvpp.GetDataSize(), dataSize);
}

TEST_F(ImageTest, ConstructImageBGRA)
{
    std::shared_ptr<uint8_t> data = nullptr;
    size_t dataSize = 0;
    APP_ERROR ret = ReadBinFromFile("/home/simon/models/imageRaw/decodeBGRA640X640.dat", data, dataSize);
    EXPECT_EQ(ret, APP_ERR_OK);

    Size alignedSize(ALIGNED_SIZE, ALIGNED_SIZE);
    Size originalSize(ORIGINAL_SIZE, ORIGINAL_SIZE);

    std::pair<Size, Size> imageSizeInfoEqual(alignedSize, alignedSize);

    Image imageAligned(data, dataSize, -1, imageSizeInfoEqual, ImageFormat::BGRA_8888);
    EXPECT_EQ(imageAligned.GetOriginalSize().width, ALIGNED_SIZE);
    imageAligned.SetImageOriginalSize(originalSize);
    auto tempData = imageAligned.GetData();
    EXPECT_NE(tempData.get(), nullptr);
}

TEST_F(ImageTest, ConstructImageGray)
{
    std::shared_ptr<uint8_t> data = nullptr;
    size_t dataSize = 0;
    APP_ERROR ret = ReadBinFromFile("/home/simon/models/imageRaw/decodeGray640X640.dat", data, dataSize);
    EXPECT_EQ(ret, APP_ERR_OK);

    Size alignedSize(ALIGNED_SIZE, ALIGNED_SIZE);
    Size originalSize(ORIGINAL_SIZE, ORIGINAL_SIZE);

    std::pair<Size, Size> imageSizeInfoEqual(alignedSize, alignedSize);

    Image imageAligned(data, dataSize, -1, imageSizeInfoEqual, ImageFormat::YUV_400);
    EXPECT_EQ(imageAligned.GetOriginalSize().width, ALIGNED_SIZE);
    imageAligned.SetImageOriginalSize(originalSize);
    auto tempData = imageAligned.GetData();
    EXPECT_NE(tempData.get(), nullptr);

    const int resizeSize = 320;
    Size size(resizeSize, resizeSize);
    ImageProcessor imageProcessor(0);
    Image resizeImage;
    imageAligned.ToDevice(0);
    ret = imageProcessor.Resize(imageAligned, size, resizeImage);
    EXPECT_NE(ret, APP_ERR_OK);
    const int margin = 240;
    Rect rect(0, 0, margin, margin);
    Dim padDim(0, 0, margin, margin);
    Color color(0, 0, 0);
    std::vector<Rect> rects = {rect};
    std::vector<Image> outputImageVec = {resizeImage};
    ret = imageProcessor.CropResize(imageAligned, rects, size, outputImageVec);
    EXPECT_NE(ret, APP_ERR_OK);
    std::vector<Image> inputImageVec = {imageAligned};
    std::vector<std::pair<Rect, Size>> cropResizeVec = { { rect, size } };
    ret = imageProcessor.CropResize(inputImageVec, cropResizeVec, outputImageVec);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = imageProcessor.Crop(imageAligned, rect, resizeImage);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = imageProcessor.Padding(imageAligned, padDim, color, BorderType::BORDER_CONSTANT, resizeImage);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(ImageTest, ConstructImageError)
{
    Size alignedSize(ALIGNED_SIZE, ALIGNED_SIZE);
    Size originalSize(ORIGINAL_SIZE, ORIGINAL_SIZE);
    Image image;
    APP_ERROR ret = image.SetImageOriginalSize(originalSize);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = image.SetImageAlignedSize(alignedSize);
    EXPECT_NE(ret, APP_ERR_OK);
    uint32_t deviceId = 0;
    std::shared_ptr<uint8_t> data = nullptr;
    size_t dataSize = 0;
    ret = ReadBinFromFile("/home/simon/models/imageRaw/decodeRGB640X640.dat", data, dataSize);
    EXPECT_EQ(ret, APP_ERR_OK);
    const int maxSize = 8192;
    const int minSize = 6;
    const int alignedWidth = 16;
    const int alignedHeight = 2;
    std::pair<Size, Size> imageSizeInfoEqual(alignedSize, alignedSize);
    Image imageConstruct(data, dataSize, -1, imageSizeInfoEqual, ImageFormat::RGB_888);
    ret = imageConstruct.SetImageAlignedSize(Size(ALIGNED_SIZE + 1, ALIGNED_SIZE));
    EXPECT_NE(ret, APP_ERR_OK);
    ret = imageConstruct.SetImageOriginalSize(Size(ALIGNED_SIZE + 1, ALIGNED_SIZE));
    EXPECT_NE(ret, APP_ERR_OK);
    ret = imageConstruct.SetImageAlignedSize(Size(ALIGNED_SIZE - alignedWidth, ALIGNED_SIZE));
    EXPECT_NE(ret, APP_ERR_OK);
    ret = imageConstruct.SetImageAlignedSize(Size(ALIGNED_SIZE, ALIGNED_SIZE - 1));
    EXPECT_NE(ret, APP_ERR_OK);
    ret = imageConstruct.SetImageOriginalSize(Size(minSize - 1, ORIGINAL_SIZE));
    EXPECT_NE(ret, APP_ERR_OK);
    ret = imageConstruct.SetImageOriginalSize(Size(ORIGINAL_SIZE, minSize - 1));
    EXPECT_NE(ret, APP_ERR_OK);
    ret = imageConstruct.SetImageOriginalSize(Size(maxSize + 1, ORIGINAL_SIZE));
    EXPECT_NE(ret, APP_ERR_OK);
    ret = imageConstruct.SetImageOriginalSize(Size(ORIGINAL_SIZE, maxSize + 1));
    EXPECT_NE(ret, APP_ERR_OK);
    ret = imageConstruct.SetImageOriginalSize(Size(minSize, minSize));
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = imageConstruct.SetImageOriginalSize(Size(minSize, minSize));
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = imageConstruct.SetImageAlignedSize(Size(alignedWidth, alignedWidth));
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = imageConstruct.SetImageAlignedSize(Size(alignedWidth, alignedWidth - alignedHeight));
    EXPECT_NE(ret, APP_ERR_OK);
    ret = imageConstruct.SetImageAlignedSize(Size(maxSize + alignedWidth, maxSize));
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(ImageTest, TestImageDumpBuffer)
{
    uint32_t deviceId = 0;
    ImageProcessor imageProcessor(deviceId);
    std::string imagePath = "./test.JPG";

    Image decodedImage;
    APP_ERROR ret = imageProcessor.Decode(imagePath, decodedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = decodedImage.DumpBuffer("./res.dat", true);
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = decodedImage.DumpBuffer("./res.dat");
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}
TEST_F(ImageTest, TestImageSeriAndUnseri)
{
    uint32_t deviceId = 0;
    ImageProcessor imageProcessor(deviceId);
    std::string imagePath = "./test.JPG";

    Image decodedImage;
    APP_ERROR ret = imageProcessor.Decode(imagePath, decodedImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = decodedImage.Serialize("./res.dat", true);
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = decodedImage.Serialize("./res.dat");
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    ret = decodedImage.Unserialize("./res.dat");
    EXPECT_EQ(ret, APP_ERR_OK);
}
TEST_F(ImageTest, TestTensorToImage)
{
// Test TensorToImage function in condition that tensor shape does not meet the requirements(H % 2 = 0; W % 16 = 0).
    std::vector<uint32_t> shape1 = {HEIGHT1, WIDTH1, CHANNEL1};
    uint8_t input1[HEIGHT1][WIDTH1][CHANNEL1] = {1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4};
    MxBase::Tensor tensor1(&input1, shape1, MxBase::TensorDType::UINT8, -1);
    tensor1.ToDevice(0);
    MxBase::Image image1;
    APP_ERROR ret = Image::TensorToImage(tensor1, image1, ImageFormat::RGB_888);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_EQ(image1.GetDataSize(), DATA_SIZE);
    image1.ToHost();
    tensor1.ToHost();
    for (int h = 0; h < HEIGHT1; h++) {
        for (int w = 0; w < WIDTH1; w++) {
            for (int c = 0; c < CHANNEL1; c++) {
                EXPECT_EQ(((uint8_t*)(tensor1.GetData()))[h * WIDTH1 * CHANNEL1  + w * CHANNEL1 + c],
                          ((uint8_t*)(image1.GetData().get()))[h * ALIGN_WIDTH * CHANNEL1 + w * CHANNEL1 + c]);
            }
        }
    }
// Test TensorToImage function in condition that tensor shape meets the requirements(H % 2 = 0; W % 16 = 0).
    std::vector<uint32_t> shape2 = {ALIGN_HEIGHT, ALIGN_WIDTH, CHANNEL1};
    uint8_t input2[ALIGN_HEIGHT][ALIGN_WIDTH][CHANNEL1] = {1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4,
                                                           1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4,
                                                           1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4,
                                                           1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4,
                                                           1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4,
                                                           1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4,
                                                           1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4,
                                                           1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4 };
    MxBase::Tensor tensor2(&input2, shape2, MxBase::TensorDType::UINT8, -1);
    tensor2.ToDevice(0);
    MxBase::Image image2;
    ret = Image::TensorToImage(tensor2, image2, ImageFormat::RGB_888);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_EQ(image2.GetDataSize(), DATA_SIZE);
    image2.ToHost();
    tensor2.ToHost();
    for (int i = 0; i < DATA_SIZE; i++) {
        EXPECT_EQ(((uint8_t*)(tensor2.GetData()))[i], ((uint8_t*)(image2.GetData().get()))[i]);
    }
// Test TensorToImage function with ImageFormat YUV_SP_444 and matched height
    std::vector<uint32_t> shape4 = {3, 2, 1};
    uint8_t input4[3][2][1] = {1, 1, 1, 2, 2, 2};
    MxBase::Tensor tensor4(&input4, shape4, MxBase::TensorDType::UINT8, -1);
    tensor4.ToDevice(0);
    MxBase::Image image4;
    ret = Image::TensorToImage(tensor4, image4, ImageFormat::YUV_SP_444);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(ImageTest, TestConvertToTensor)
{
    // Image is on device
    std::string imagePath = "./test.JPG";
    cv::Mat vhead = cv::imread(imagePath, cv::IMREAD_UNCHANGED);
    int64_t cSize = vhead.channels();
    int64_t hSize = vhead.rows;
    int64_t wSize = vhead.cols;
    std::vector<uint32_t> sshape = {hSize, wSize, 1};
    std::vector<uint32_t> sshape2 = {hSize, wSize, 3};
    MxBase::Tensor tensor1(vhead.data, sshape2, MxBase::TensorDType::UINT8);
    // conduct TensorToImage function
    tensor1.ToDevice(0);
    MxBase::Image image1;
    APP_ERROR ret = Image::TensorToImage(tensor1, image1, ImageFormat::RGB_888);
    EXPECT_EQ(ret, APP_ERR_OK);
    // conduct ConvertToTensor function without stride
    image1.ToDevice(0);
    Tensor tensor2 = image1.ConvertToTensor(false, true);
    tensor2.ToHost();
    tensor1.ToHost();
    int dataSize = hSize * wSize;
    for (int i = 0; i < dataSize; i++) {
        EXPECT_EQ(((uint8_t*)tensor2.GetData())[i], ((uint8_t*)tensor1.GetData())[i]);
    }
    // conduct ConvertToTensor function with stride
    Tensor tensor3 = image1.ConvertToTensor(true, true);
    tensor3.ToHost();
    auto shape3 = tensor3.GetShape();
    size_t widthDim = shape3.size() - 0x2;
    for (int h = 0; h < hSize; h++) {
        for (int w = 0; w < wSize; w++) {
            for (int c = 0; c < cSize; c++) {
                EXPECT_EQ(((uint8_t*)tensor3.GetData())[h * shape3[widthDim] * cSize + w * cSize + c],
                          ((uint8_t*)tensor1.GetData())[h * wSize * cSize + w * cSize + c]);
            }
        }
    }

    // Image is on host
    Image image;
    Tensor tensor = image.ConvertToTensor(true, true);

    image1.ToHost();
    Tensor tensor4 = image1.ConvertToTensor(false, true);
    tensor4.ToHost();
    for (int i; i < dataSize; i++) {
        EXPECT_EQ(((uint8_t*)tensor2.GetData())[i], ((uint8_t*)tensor1.GetData())[i]);
    }
}
}

int main(int argc, char *argv[])
{
    MxInit();
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
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

#include <map>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#define private public
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/DvppWrapper/DvppWrapperBase.h"
#undef private
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Utils/FileUtils.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Checker/CheckUtils.h"
#include "ResourceManager/DvppWrapper/DvppConfig/Config/ConfigUtils.h"

namespace {
using namespace MxBase;
std::shared_ptr<DvppWrapper> g_dvppCommon;
std::shared_ptr<DvppWrapper> g_dvppPngDecoder;
std::shared_ptr<DvppWrapper> g_dvppVpcProcessor;
std::shared_ptr<DvppWrapper> g_dvppJpegEncoder;
std::shared_ptr<DvppWrapper> g_dvppJpegDecoder;
const std::string YUV_FILE_SUFFIX = ".yuv";
const std::string JPG_FILE_SUFFIX = ".jpg";
const uint32_t IMAGEWIDTH = 156;
const uint32_t IMAGEHEIGTH = 138;
const uint32_t OFFSET = 100000;
const int VECSIZEINPUT = 1;
const int VECSIZE = 5;
const uint32_t RESIZEWIDTH = 110;
const uint32_t RESIZEHEIGHT = 130;
const uint32_t CROPWIDTH = 122;
const uint32_t CROPHEIGHT = 126;
const uint32_t PADDINGDIM = 10;
const uint32_t PADDINGCOLOR = 255;
const uint32_t DVPP_CONSTANT_15 = 15;
const uint32_t DVPP_CONSTANT_16 = 16;
const uint32_t DVPP_CONSTANT_17 = 17;
const uint32_t DVPP_CONSTANT_32 = 17;
const uint32_t DVPP_CONSTANT_12345 = 12345;
const uint32_t WIDTH_1080P = 1920;
const uint32_t HEIGHT_1080P = 1080;
const uint32_t LEVEL = 100;
const uint32_t VEC_SIZE_5 = 5;
const uint32_t VEC_SIZE_4 = 4;
const uint32_t SIZE_512 = 512;
enum class IMAGEINFO {
    ERROR_HW,
    ERROR_SIZE,
    ERROR_PATH,
    ERROR_TYPE,
    ERROR_HIGHT,
    ERROR_WIDTH,
    ERROR_MEMORY,
    ERROR_ALIGN_HW,
    ERROR_CONFIG,
    ERROR_OUTOFRANGE,
    ERROR_SMALLER,
    ERROR_IMAGESIZE_NE_CONFIGSIZE,
    ERROR_IMAGESIZE_NE_RESIZESIZE,
    ERROR_OUTPUTSIZE_NE_CONFIGSIZE,
};
struct YuvFileInfo {
    uint32_t width;
    uint32_t height;
    MxbasePixelFormat format;
};
const std::map<std::string, YuvFileInfo> YUV_PICTURES_INFO = {
    {"440_1280x720_nv12", {1280, 720, MXBASE_PIXEL_FORMAT_YUV_400}},
    {"yuv420_1280x720_nv12", {1280, 720, MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420}},
    {"yuv420_1280x720_nv21", {1280, 720, MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420}},
    {"yuv444_1920x1080_yuv444sp_vu", {1920, 1080, MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_444}},
    {"yuv422_720x1280_yuv422sp_vu", {720, 1280, MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_422}},
    {"1280x720_yuyv_422P", {1280, 720, MXBASE_PIXEL_FORMAT_YUYV_PACKED_422}},
    {"1280x720_uyvy_422P", {1280, 720, MXBASE_PIXEL_FORMAT_UYVY_PACKED_422}},
    {"1280x720_yvyu_422P", {1280, 720, MXBASE_PIXEL_FORMAT_YVYU_PACKED_422}},
    {"1280x720_vyuy_422P", {1280, 720, MXBASE_PIXEL_FORMAT_VYUY_PACKED_422}},
    {"1280x720_yuv444pkt", {1280, 720, MXBASE_PIXEL_FORMAT_YUV_PACKED_444}}
};
const std::map<std::string, MxbasePixelFormat> JPG_PICTURES_INFO = {
    {"yuv400_1080x1920", MXBASE_PIXEL_FORMAT_YUV_400},
    {"yuv420_1280x720", MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420},
    {"yuv444_1280x720", MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_444},
    {"yuv422_1280x720", MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_422}
};
CropRoiConfig CROP_ROI_CONFIG{22, 226, 230, 30};


bool ReadYuvFileToDeviceMemory(const std::string& fileName, MxBase::MemoryData& deviceMemoryData)
{
    if (!MxBase::FileUtils::CheckFileExists(fileName)) {
        LogError << "File not exists. fileName:" << fileName << "." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return false;
    }
    auto fileBuffer = MxBase::FileUtils::ReadFileContent(fileName);
    MxBase::MemoryData memoryData((void*)fileBuffer.c_str(), fileBuffer.size(), MxBase::MemoryData::MEMORY_HOST);
    deviceMemoryData.size = fileBuffer.size();
    deviceMemoryData.type = MxBase::MemoryData::MEMORY_DVPP;
    auto ret = MemoryHelper::MxbsMallocAndCopy(deviceMemoryData, memoryData);
    if (ret != APP_ERR_OK) {
        LogError << "MemoryHelper MxbsMemcpy failed."<< GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    return true;
}

bool WriteYuvFileFromDeviceMemory(const std::string& filePath, const std::string& fileName,
    MxBase::MemoryData& deviceMemoryData)
{
    if (!MxBase::FileUtils::CheckDirectoryExists(filePath)) {
        if (!MxBase::FileUtils::CreateDirectories(filePath)) {
            LogError << "Failed to create result directory: " << fileName << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
    }
    MxBase::MemoryData hostMemoryData(deviceMemoryData.size, MxBase::MemoryData::MEMORY_HOST);
    auto ret = MxBase::MemoryHelper::MxbsMallocAndCopy(hostMemoryData, deviceMemoryData);
    if (ret != APP_ERR_OK) {
        return false;
    }
    MxBase::FileUtils::WriteFileContent(filePath + "/" + fileName,
        std::string((char *)hostMemoryData.ptrData, hostMemoryData.size));
    ret = MemoryHelper::MxbsFree(hostMemoryData);
    if (ret != APP_ERR_OK) {
        return false;
    }
    return true;
}

class DvppWrapperTest : public testing::Test {
protected:
    void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

class DeviceGuard {
public:
    DeviceGuard()
    {
        InitDevice();
        InitResource();
    }
    ~DeviceGuard()
    {
        DeInitDevice();
    }

private:
    APP_ERROR InitDevice();
    void DeInitDevice() const;
    APP_ERROR InitResource() const;
    DeviceContext deviceContext_ = {};
};

APP_ERROR DeviceGuard::InitDevice()
{
    APP_ERROR result = APP_ERR_OK;
    result = DeviceManager::GetInstance()->InitDevices();
    if (result != APP_ERR_OK) {
        return result;
    }
    deviceContext_.devId = 0;
    result = DeviceManager::GetInstance()->SetDevice(deviceContext_);
    if (result != APP_ERR_OK) {
        return result;
    }
    return result;
}

void DeviceGuard::DeInitDevice() const
{
    APP_ERROR result = DeviceManager::GetInstance()->DestroyDevices();
    if (result != APP_ERR_OK) {
    }
    g_dvppJpegDecoder->DeInit();
    g_dvppPngDecoder->DeInit();
    g_dvppVpcProcessor->DeInit();
    g_dvppJpegEncoder->DeInit();
    g_dvppJpegDecoder.reset();
    g_dvppPngDecoder.reset();
    g_dvppVpcProcessor.reset();
    g_dvppJpegEncoder.reset();
}

APP_ERROR DeviceGuard::InitResource() const
{
    APP_ERROR ret = APP_ERR_OK;
    g_dvppCommon = MemoryHelper::MakeShared<DvppWrapper>();
    if (g_dvppCommon == nullptr) {
        LogError << "Failed to create g_dvppCommon object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    g_dvppJpegDecoder = MemoryHelper::MakeShared<DvppWrapper>();
    if (g_dvppJpegDecoder == nullptr) {
        LogError << "Failed to create JpegDecoder object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    JpegDecodeChnConfig jpegConfig;
    g_dvppJpegDecoder->InitJpegDecodeChannel(jpegConfig);

    g_dvppPngDecoder = MemoryHelper::MakeShared<DvppWrapper>();
    if (g_dvppPngDecoder == nullptr) {
        LogError << "Failed to create PngDecoder object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    PngDecodeChnConfig pngDecodeChnConfig;
    g_dvppPngDecoder->InitPngDecodeChannel(pngDecodeChnConfig);

    g_dvppVpcProcessor = MemoryHelper::MakeShared<DvppWrapper>();
    if (g_dvppVpcProcessor == nullptr) {
        LogError << "Failed to create Vpc object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    VpcChnConfig vpcChnConfig;
    g_dvppVpcProcessor->InitVpcChannel(vpcChnConfig);

    g_dvppJpegEncoder = MemoryHelper::MakeShared<DvppWrapper>();
    if (g_dvppJpegEncoder == nullptr) {
        LogError << "Failed to create JpegEncoder object." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    JpegEncodeChnConfig jpegEncodeChnConfig;
    g_dvppJpegEncoder->InitJpegEncodeChannel(jpegEncodeChnConfig);
    LogInfo << "DvppCommon object created successfully";

    return APP_ERR_OK;
}

APP_ERROR GetDvppDataInfo(const std::string path, DvppDataInfo& dataInfo)
{
    std::string content = MxBase::FileUtils::ReadFileContent(path);
    // Allocate memory on the device
    MemoryData data(content.size(), MemoryData::MEMORY_DVPP);
    MemoryData src((void *)const_cast<char *>(content.c_str()), content.size(), MemoryData::MEMORY_HOST);
    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(data, src);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    DvppImageOutput imageOutput;
    DvppImageInfo imageInfo;
    imageInfo.data = src.ptrData;
    imageInfo.size = src.size;
    imageInfo.pictureType = imageInfo.PIXEL_FORMAT_JPEG;
    ret = g_dvppJpegDecoder->GetPictureDec(imageInfo, imageOutput);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    dataInfo.width = imageOutput.width;
    dataInfo.height = imageOutput.height;

    dataInfo.destory = [](void *dataPtr) -> void {
        MemoryData data;
        data.type = MemoryData::MEMORY_DVPP;
        data.ptrData = dataPtr;
        MemoryHelper::MxbsFree(data);
        data.ptrData = nullptr;
    };
    dataInfo.dataSize = content.size();
    dataInfo.data = static_cast<uint8_t*>(data.ptrData);
    return APP_ERR_OK;
}

std::string CopyDataToHost(DvppDataInfo output)
{
    DvppDataInfo dataInfo;
    const uint32_t level = 100;
    if (output.data == nullptr) {
        return "";
    }
    APP_ERROR ret = g_dvppJpegEncoder->DvppJpegEncode(output, dataInfo, level);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to DvppJpegEncode." << GetErrorInfo(APP_ERR_DVPP_JPEG_ENCODE_FAIL);
        return "";
    }
    MemoryData data(dataInfo.dataSize, MemoryData::MEMORY_HOST);
    MemoryData src(static_cast<void*>(dataInfo.data), dataInfo.dataSize, MemoryData::MEMORY_DVPP);
    ret = MemoryHelper::MxbsMallocAndCopy(data, src);
    if (ret != APP_ERR_OK) {
        return "";
    }
    std::string result(static_cast<char*>(data.ptrData), data.size);
    ret = data.free(data.ptrData);
    if (ret != APP_ERR_OK) {
        return "";
    }
    dataInfo.destory(dataInfo.data);
    output.destory(output.data);
    return result;
}

void SetCropRoiConfig(std::vector<CropRoiConfig>& vecCfg)
{
    CropRoiConfig config1 {50, 249, 299, 0};
    CropRoiConfig config2 {20, 156, 156, 20};
    CropRoiConfig config3 {10, 156, 156, 10};
    CropRoiConfig config4 {40, 156, 156, 40};
    CropRoiConfig config5 {30, 156, 156, 30};
    vecCfg.push_back(config1);
    vecCfg.push_back(config2);
    vecCfg.push_back(config3);
    vecCfg.push_back(config4);
    vecCfg.push_back(config5);
}

APP_ERROR SetCropResizeConfig(std::vector<DvppDataInfo>& vecIn)
{
    std::string filepath = "./test1.jpg";
    std::string filepath1 = "./test2.jpg";
    std::string filepath2 = "./test3.jpg";
    std::string filepath3 = "./test4.jpg";
    std::string filepath4 = "./test5.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    DvppDataInfo input1;
    ret = g_dvppJpegDecoder->DvppJpegDecode(filepath1, input1);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    DvppDataInfo input2;
    ret = g_dvppJpegDecoder->DvppJpegDecode(filepath2, input2);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    DvppDataInfo input3;
    ret = g_dvppJpegDecoder->DvppJpegDecode(filepath3, input3);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    DvppDataInfo input4;
    ret = g_dvppJpegDecoder->DvppJpegDecode(filepath4, input4);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    vecIn.push_back(input);
    vecIn.push_back(input1);
    vecIn.push_back(input2);
    vecIn.push_back(input3);
    vecIn.push_back(input4);
    return APP_ERR_OK;
}

TEST_F(DvppWrapperTest, TestDvppJpegDecodeWithPathNormal)
{
    std::string filepath = "./test5.jpg";
    DvppDataInfo output;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, output);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::string content = MxBase::FileUtils::ReadFileContent("./TestImageDecoder/decode_dvpp.jpg");
    std::string result = CopyDataToHost(output);
    EXPECT_EQ(content, result);
}

TEST_F(DvppWrapperTest, TestDvppPngDecodeWithPathNormal)
{
    std::string filepath = "./test.png";
    DvppDataInfo output;
    output.format = MXBASE_PIXEL_FORMAT_RGB_888;
    APP_ERROR ret = g_dvppPngDecoder->DvppPngDecode(filepath, output);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestDvppJpegEncodeWithPathNormal)
{
    std::string filepath = "./test5.jpg";
    DvppDataInfo output;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, output);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_dvppJpegEncoder->DvppJpegEncode(output, "./Encode", "encode", LEVEL);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::string result = MxBase::FileUtils::ReadFileContent("./Encode/encode.jpg");
    std::string image = MxBase::FileUtils::ReadFileContent("./TestEncode/encode_dvpp.jpg");
    EXPECT_EQ(result, image);
}

TEST_F(DvppWrapperTest, TestDvppJpegEncodeWithDataInfoNormal)
{
    std::string filepath = "./test5.jpg";
    DvppDataInfo output;
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_dvppJpegEncoder->DvppJpegEncode(input, output, LEVEL);
    EXPECT_EQ(ret, APP_ERR_OK);
    input.destory(input.data);
    MemoryData data(output.dataSize, MemoryData::MEMORY_HOST);
    MemoryData src(static_cast<void*>(output.data), output.dataSize, MemoryData::MEMORY_DVPP);
    ret = MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string result(static_cast<char*>(data.ptrData), data.size);
    ret = data.free(data.ptrData);
    EXPECT_EQ(ret, APP_ERR_OK);
    output.destory(output.data);
    std::string image = MxBase::FileUtils::ReadFileContent("./TestEncode/encode_dvpp.jpg");
    EXPECT_EQ(result, image);
}

TEST_F(DvppWrapperTest, TestVpcResizeNormal)
{
    std::string filepath = "./test5.jpg";
    DvppDataInfo input;
    DvppDataInfo output0;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    ResizeConfig config = {230, 240, 0., 0., 0};
    ret = g_dvppVpcProcessor->VpcResize(input, output0, config);
    EXPECT_EQ(ret, APP_ERR_OK);
    DvppDataInfo output;
    const uint32_t level = 100;
    ret = g_dvppJpegEncoder->DvppJpegEncode(output0, output, level);
    EXPECT_EQ(ret, APP_ERR_OK);
    input.destory(input.data);
    MemoryData data(output.dataSize, MemoryData::MEMORY_HOST);
    MemoryData src(static_cast<void*>(output.data), output.dataSize, MemoryData::MEMORY_DVPP);
    ret = MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string result(static_cast<char*>(data.ptrData), data.size);
    ret = data.free(data.ptrData);
    EXPECT_EQ(ret, APP_ERR_OK);
    output.destory(output.data);
    std::string content = MxBase::FileUtils::ReadFileContent("./TestVpcResize/result_resize_dvpp.jpg");
    EXPECT_EQ(content, result);
}

TEST_F(DvppWrapperTest, TestVpcPaddingNormal)
{
    std::string filepath = "./test5.jpg";
    DvppDataInfo input;
    DvppDataInfo output;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    MakeBorderConfig borderConfig;
    borderConfig.borderType = MxBase::MakeBorderConfig::BORDER_CONSTANT;
    borderConfig.left = PADDINGDIM;
    borderConfig.right = PADDINGDIM;
    borderConfig.top = PADDINGDIM;
    borderConfig.bottom = PADDINGDIM;
    borderConfig.channel_zero = PADDINGCOLOR;
    borderConfig.channel_one = PADDINGCOLOR;
    borderConfig.channel_two = PADDINGCOLOR;
    ret = g_dvppVpcProcessor->VpcPadding(input, output, borderConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    input.destory(input.data);
    output.destory(output.data);
}

TEST_F(DvppWrapperTest, TestVpcCropNormal)
{
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    DvppDataInfo output;
    CropRoiConfig config{22, 226, 230, 30};
    ret = g_dvppVpcProcessor->VpcCrop(input, output, config);
    EXPECT_EQ(ret, APP_ERR_OK);
    input.destory(input.data);
    std::string content = MxBase::FileUtils::ReadFileContent("./TestOneCropOne/result_oneCropOne_dvpp.jpg");
    std::string result = CopyDataToHost(output);
    EXPECT_EQ(content, result);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNNormal)
{
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<DvppDataInfo> vecDvpp(VEC_SIZE_5, DvppDataInfo());
    std::vector<CropRoiConfig> vecCfg;
    SetCropRoiConfig(vecCfg);
    ret = g_dvppVpcProcessor->VpcBatchCrop(input, vecDvpp, vecCfg);
    EXPECT_EQ(ret, APP_ERR_OK);
    for (size_t i = 0; i < vecDvpp.size(); i++) {
        std::string path = "./TestOneCropN/result_dvpp_" + std::to_string(i) + ".jpg";
        std::string content = MxBase::FileUtils::ReadFileContent(path);
        std::string result = CopyDataToHost(vecDvpp[i]);
        EXPECT_EQ(content, result);
    }
}

TEST_F(DvppWrapperTest, TestVpcBatchNCropNMNormal)
{
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<DvppDataInfo> vecInput(VECSIZEINPUT, input);
    std::vector<DvppDataInfo> vecDvpp(VECSIZE, DvppDataInfo());
    std::vector<CropRoiConfig> vecCfg;
    SetCropRoiConfig(vecCfg);
    ret = g_dvppVpcProcessor->VpcBatchCropMN(vecInput, vecDvpp, vecCfg);
    EXPECT_EQ(ret, APP_ERR_OK);
    for (size_t i = 0; i < vecDvpp.size(); i++) {
        std::string path = "./TestOneCropN/result_dvpp_" + std::to_string(i) + ".jpg";
        std::string content = MxBase::FileUtils::ReadFileContent(path);
        std::string result = CopyDataToHost(vecDvpp[i]);
        EXPECT_EQ(content, result);
    }
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResize1Notmal)
{
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<DvppDataInfo> vecDvpp(VEC_SIZE_5, DvppDataInfo());
    std::vector<CropRoiConfig> vecCfg;
    SetCropRoiConfig(vecCfg);
    ResizeConfig resizeConfig = {138, 156, 0., 0., 0};
    ret = g_dvppVpcProcessor->VpcBatchCropResize(input, vecDvpp, vecCfg, resizeConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    input.destory(input.data);
    for (size_t i = 0; i < vecDvpp.size(); i++) {
        std::string path = "./VpcBatchCropResize/cropResize_dvpp_" + std::to_string(i) + ".jpg";
        std::string content = MxBase::FileUtils::ReadFileContent(path);
        std::string result = CopyDataToHost(vecDvpp[i]);
        EXPECT_EQ(content, result);
    }
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResizeNNotmal)
{
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<DvppDataInfo> vecDvpp(VEC_SIZE_5, DvppDataInfo());
    std::vector<CropRoiConfig> vecCfg;
    SetCropRoiConfig(vecCfg);
    ResizeConfig resizeConfig;
    resizeConfig.width = RESIZEWIDTH;
    resizeConfig.height = RESIZEHEIGHT;
    std::vector<ResizeConfig> vecResize(VEC_SIZE_5, resizeConfig);
    ret = g_dvppVpcProcessor->VpcBatchCropResize(input, vecDvpp, vecCfg, vecResize);
    EXPECT_EQ(ret, APP_ERR_OK);
    input.destory(input.data);
    for (size_t i = 0; i < vecDvpp.size(); i++) {
        std::string path = "./VpcBatch1CropNResizeN/VpcBatch1CropNResizeN_dvpp_" + std::to_string(i) + ".jpg";
        std::string content = MxBase::FileUtils::ReadFileContent(path);
        std::string result = CopyDataToHost(vecDvpp[i]);
        EXPECT_EQ(content, result);
    }
}

TEST_F(DvppWrapperTest, TestVpcBatchNCropNResizeNNotmal)
{
    std::vector<DvppDataInfo> vecIn;
    APP_ERROR ret = SetCropResizeConfig(vecIn);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<DvppDataInfo> vecDvpp(VEC_SIZE_5, DvppDataInfo());
    std::vector<CropRoiConfig> vecCfg = {
            {150, 256, 256, 150},
            {120, 256, 256, 120},
            {110, 256, 256, 110},
            {140, 256, 256, 140},
            {130, 256, 256, 130}
    };
    ResizeConfig resizeConfig;
    resizeConfig.width = CROPWIDTH;
    resizeConfig.height = CROPHEIGHT;
    std::vector<ResizeConfig> vecResize(VEC_SIZE_5, resizeConfig);
    ret = g_dvppVpcProcessor->VpcBatchCropResize(vecIn, vecDvpp, vecCfg, vecResize);
    EXPECT_EQ(ret, APP_ERR_OK);
    for (size_t i = 0; i < vecDvpp.size(); i++) {
        std::string path = "./TestNCropNResizeN/TestNCropNResizeN_dvpp" + std::to_string(i) + ".jpg";
        std::string content = MxBase::FileUtils::ReadFileContent(path);
        std::string result = CopyDataToHost(vecDvpp[i]);
        EXPECT_EQ(content, result);
    }
}

TEST_F(DvppWrapperTest, TestVpcBatchNCropNNormal)
{
    std::vector<DvppDataInfo> vecIn;
    APP_ERROR ret = SetCropResizeConfig(vecIn);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::vector<DvppDataInfo> vecDvpp(VEC_SIZE_5, DvppDataInfo());
    std::vector<CropRoiConfig> vecCfg = {
            {50, 156, 156, 50},
            {20, 156, 156, 20},
            {10, 156, 156, 10},
            {40, 156, 156, 40},
            {30, 156, 156, 30}
    };
    ret = g_dvppVpcProcessor->VpcBatchCrop(vecIn, vecDvpp, vecCfg);
    EXPECT_EQ(ret, APP_ERR_OK);
    for (int i = 0; i < VEC_SIZE_5; i++) {
        std::string path = "./TestNCropN/TestNCropN_dvpp" + std::to_string(i) + ".jpg";
        std::string content = MxBase::FileUtils::ReadFileContent(path);
        std::string result = CopyDataToHost(vecDvpp[i]);
        EXPECT_EQ(content, result);
    }
}

TEST_F(DvppWrapperTest, TestYuvVariousFormat)
{
    for (const auto& value : YUV_PICTURES_INFO) {
        std::string fileName = "./yuv/" + value.first + YUV_FILE_SUFFIX;
        MxBase::MemoryData deviceMemoryData;
        auto ret = ReadYuvFileToDeviceMemory(fileName, deviceMemoryData);
        EXPECT_EQ(ret, true);
        int pixelBit = 1;
        auto iter = IMAGE_CONSTRAIN_VEC.find(value.second.format);
        if (iter != IMAGE_CONSTRAIN_VEC.end()) {
            pixelBit = iter->second.pixelBit;
        }
        DvppDataInfo inputDvppDataInfo;
        inputDvppDataInfo.data = (uint8_t*)deviceMemoryData.ptrData;
        inputDvppDataInfo.dataSize = deviceMemoryData.size;
        inputDvppDataInfo.width = value.second.width;
        inputDvppDataInfo.height = value.second.height;
        inputDvppDataInfo.format = value.second.format;
        inputDvppDataInfo.widthStride = DVPP_ALIGN_UP(inputDvppDataInfo.width, VPC_STRIDE_WIDTH) * pixelBit;
        inputDvppDataInfo.heightStride = DVPP_ALIGN_UP(inputDvppDataInfo.height, VPC_STRIDE_HEIGHT);
        ret = DvppWrapper::VpcPictureConstrainInfoCheck(inputDvppDataInfo);
        if (ret != APP_ERR_OK) {
            LogDebug << "VpcPictureConstrainInfoCheck failed. format(" << value.first << ").";
            continue;
        }
        std::vector<CropRoiConfig> vecCfg = {
            {0, 699, 699, 0}
        };
        ResizeConfig resizeConfig = {700, 700, 0., 0., 0};
        std::vector<DvppDataInfo> vecDvpp(vecCfg.size(), DvppDataInfo());
        ret = g_dvppVpcProcessor->VpcBatchCropResize(inputDvppDataInfo, vecDvpp, vecCfg, resizeConfig);
        if (ret == APP_ERR_OK) {
            LogInfo << value.first << " success.";
            for (size_t i = 0; i < vecDvpp.size(); i++) {
                std::string str = value.first + "_" + std::to_string(i);
                ret = g_dvppJpegEncoder->DvppJpegEncode(vecDvpp[i], "./1CropResizeN_YUV", str, LEVEL);
                EXPECT_EQ(ret, APP_ERR_OK);
            }
        } else {
            LogInfo << value.first << " failed.";
        }
        ret = WriteYuvFileFromDeviceMemory("./1CropResizeN_YUV", value.first + YUV_FILE_SUFFIX, deviceMemoryData);
        EXPECT_EQ(ret, true);
        ret = MemoryHelper::MxbsFree(deviceMemoryData);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(DvppWrapperTest, Test_VpcPictureConstrainInfoCheck_Should_Failed_When_WidthStride_Is_0)
{
    DvppDataInfo inputData;
    inputData.widthStride = 0;
    APP_ERROR ret = DvppWrapper::VpcPictureConstrainInfoCheck(inputData);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, Test_VpcPictureConstrainInfoCheck_Should_Failed_When_HeightStride_Is_0)
{
    DvppDataInfo inputData;
    inputData.heightStride = 0;
    APP_ERROR ret = DvppWrapper::VpcPictureConstrainInfoCheck(inputData);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, Test_VpcPictureConstrainInfoCheck_Should_Failed_When_Format_Is_Wrong)
{
    DvppDataInfo inputData;
    int format = 100;
    inputData.format = static_cast<MxbasePixelFormat>(format);
    APP_ERROR ret = DvppWrapper::VpcPictureConstrainInfoCheck(inputData);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, Test_VpcPictureConstrainInfoCheck_Should_Failed_When_WidthStride_Is_Not_Align)
{
    DvppDataInfo inputData;
    inputData.widthStride = DVPP_CONSTANT_17;
    inputData.heightStride = DVPP_CONSTANT_16;
    APP_ERROR ret = DvppWrapper::VpcPictureConstrainInfoCheck(inputData);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, Test_VpcPictureConstrainInfoCheck_Should_Failed_When_HeightStride_Is_Not_Align)
{
    DvppDataInfo inputData;
    inputData.widthStride = DVPP_CONSTANT_16;
    inputData.heightStride = DVPP_CONSTANT_17;
    APP_ERROR ret = DvppWrapper::VpcPictureConstrainInfoCheck(inputData);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, Test_VpcPictureConstrainInfoCheck_Should_Failed_When_Stride_Size_Is_Lower_Than_Origin_Size)
{
    DvppDataInfo inputData;
    inputData.width = DVPP_CONSTANT_32;
    inputData.height = DVPP_CONSTANT_32;
    inputData.widthStride = DVPP_CONSTANT_16;
    inputData.heightStride = DVPP_CONSTANT_16;
    APP_ERROR ret = DvppWrapper::VpcPictureConstrainInfoCheck(inputData);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, Test_VpcPictureConstrainInfoCheck_Should_Failed_When_DataSize_Is_Wrong)
{
    DvppDataInfo inputData;
    inputData.width = DVPP_CONSTANT_15;
    inputData.height = DVPP_CONSTANT_15;
    inputData.widthStride = DVPP_CONSTANT_16;
    inputData.heightStride = DVPP_CONSTANT_16;
    inputData.dataSize = DVPP_CONSTANT_12345;
    APP_ERROR ret = DvppWrapper::VpcPictureConstrainInfoCheck(inputData);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestJpgVariousFormat)
{
    for (const auto& value : JPG_PICTURES_INFO) {
        std::string fileName = "./jpg/" + value.first + JPG_FILE_SUFFIX;
        DvppDataInfo inputDvppDataInfo;
        inputDvppDataInfo.format = value.second;
        APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(fileName, inputDvppDataInfo);
        EXPECT_EQ(ret, APP_ERR_OK);
        ret = DvppWrapper::VpcPictureConstrainInfoCheck(inputDvppDataInfo);
        if (ret != APP_ERR_OK) {
            LogDebug << "VpcPictureConstrainInfoCheck failed. format(" << value.first << ").";
            continue;
        }
        std::vector<CropRoiConfig> vecCfg = {
            {0, 320, 320, 0}
        };
        ResizeConfig resizeConfig = {640, 640, 0., 0., 0};
        std::vector<DvppDataInfo> vecDvpp(vecCfg.size(), DvppDataInfo());
        ret = g_dvppVpcProcessor->VpcBatchCropResize(inputDvppDataInfo, vecDvpp, vecCfg, resizeConfig);
        if (ret == APP_ERR_OK) {
            LogInfo << value.first << " success.";
            for (size_t i = 0; i < vecDvpp.size(); i++) {
                std::string str = value.first + "_" + std::to_string(i);
                ret = g_dvppJpegEncoder->DvppJpegEncode(vecDvpp[i], "./1CropResizeN_JPG", str, LEVEL);
                EXPECT_EQ(ret, APP_ERR_OK);
            }
        } else {
            LogInfo << value.first << " failed.";
        }
    }
}

APP_ERROR VdecCallBack(std::shared_ptr<void> buffer, DvppDataInfo &dvppDataInfo, void *userData)
{
    MxBase::MemoryData deviceMemoryData;
    deviceMemoryData.ptrData = buffer.get();
    deviceMemoryData.size = dvppDataInfo.dataSize;
    deviceMemoryData.type = MxBase::MemoryData::MEMORY_DVPP;
    bool ret = WriteYuvFileFromDeviceMemory("./TestVideo/h264", "result.yuv", deviceMemoryData);
    if (!ret) {
        LogError << "VdecCallBack WriteYuvFileFromDeviceMemory failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    auto result = FileUtils::ReadFileContent("./TestVideo/h264/result.yuv");
    auto target = FileUtils::ReadFileContent("./TestVideo/h264/target.yuv");
    EXPECT_EQ(result, target);
    LogInfo << "vdec h264 success.";
    return APP_ERR_OK;
}

// vdec test
TEST_F(DvppWrapperTest, VdecH264Test)
{
    std::string fileName = "./TestVideo/h264/I_keyFrame.data";
    MxBase::MemoryData deviceMemoryData;
    auto ret = ReadYuvFileToDeviceMemory(fileName, deviceMemoryData);
    EXPECT_EQ(ret, true);

    DvppDataInfo inputDvppDataInfo;
    inputDvppDataInfo.dataSize = deviceMemoryData.size;
    inputDvppDataInfo.data = (uint8_t*)deviceMemoryData.ptrData;
    inputDvppDataInfo.height = HEIGHT_1080P;
    inputDvppDataInfo.width = WIDTH_1080P;

    VdecConfig vdecConfig;
    vdecConfig.channelId = 0;
    vdecConfig.width = inputDvppDataInfo.width;
    vdecConfig.height = inputDvppDataInfo.height;
    vdecConfig.inputVideoFormat = MXBASE_STREAM_FORMAT_H264_MAIN_LEVEL;
    vdecConfig.outputImageFormat = MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    vdecConfig.callbackFunc = VdecCallBack;

    ret = g_dvppCommon->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_dvppCommon->DvppVdec(inputDvppDataInfo, nullptr);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_dvppCommon->DeInitVdec();
    EXPECT_EQ(ret, APP_ERR_OK);
}

// vdec test invalid parameters
TEST_F(DvppWrapperTest, DvppVdecInvalidParam)
{
    const uint32_t tmpSize = 127;
    DvppDataInfo inputDvppDataInfo;
    inputDvppDataInfo.width = tmpSize;
    APP_ERROR ret = g_dvppCommon->DvppVdec(inputDvppDataInfo, nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_OUT_OF_RANGE);

    inputDvppDataInfo.height = tmpSize;
    inputDvppDataInfo.width = WIDTH_1080P;
    ret = g_dvppCommon->DvppVdec(inputDvppDataInfo, nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_OUT_OF_RANGE);

    inputDvppDataInfo.data = nullptr;
    inputDvppDataInfo.height = HEIGHT_1080P;
    inputDvppDataInfo.width = WIDTH_1080P;
    ret = g_dvppCommon->DvppVdec(inputDvppDataInfo, nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // construct non-nullptr
    inputDvppDataInfo.data = (uint8_t *)&inputDvppDataInfo;
    inputDvppDataInfo.dataSize = 0;
    inputDvppDataInfo.height = HEIGHT_1080P;
    inputDvppDataInfo.width = WIDTH_1080P;
    ret = g_dvppCommon->DvppVdec(inputDvppDataInfo, nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

APP_ERROR GetDvppJpegDecodeResult(IMAGEINFO style)
{
    std::string filepath = "./test5.jpg";
    DvppDataInfo output;
    DvppDataInfo input;
    APP_ERROR ret = GetDvppDataInfo(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    const int offset = 300;
    switch (style) {
        case IMAGEINFO::ERROR_HIGHT:
            input.height -= offset;
            break;
        case IMAGEINFO::ERROR_WIDTH:
            input.width -= offset;
            break;
        case IMAGEINFO::ERROR_MEMORY:
            input.destory(input.data);
            break;
        case IMAGEINFO::ERROR_SIZE:
            input.dataSize -= offset;
            break;
        case IMAGEINFO::ERROR_PATH:
            filepath.clear();
            ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, output);
            return ret;
        case IMAGEINFO::ERROR_TYPE:
            output.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_JPEG;
            break;
        default:
            return APP_ERR_OK;
    }
    ret = g_dvppJpegDecoder->DvppJpegDecode(input, output);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (input.data != nullptr) {
        input.destory(input.data);
    }
    return APP_ERR_COMM_FAILURE;
}

TEST_F(DvppWrapperTest, TestDvppJpegDecodeNullptr)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetDvppJpegDecodeResult(IMAGEINFO::ERROR_MEMORY);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestDvppJpegDecodeErrorSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetDvppJpegDecodeResult(IMAGEINFO::ERROR_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestDvppJpegDecodeErrorW)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetDvppJpegDecodeResult(IMAGEINFO::ERROR_WIDTH);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestDvppJpegDecodeErrorH)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetDvppJpegDecodeResult(IMAGEINFO::ERROR_HIGHT);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestDvppJpegDecodeErrorPath)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetDvppJpegDecodeResult(IMAGEINFO::ERROR_PATH);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestDvppJpegDecodeErrorType)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetDvppJpegDecodeResult(IMAGEINFO::ERROR_TYPE);
    EXPECT_NE(ret, APP_ERR_OK);
}

APP_ERROR GetVpcCropResult(IMAGEINFO style)
{
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    if (ret != APP_ERR_OK) {
        return APP_ERR_OK;
    }
    DvppDataInfo output;
    CropRoiConfig config {22, 226, 230, 30};
    const int offset = 1000;
    switch (style) {
        case IMAGEINFO::ERROR_CONFIG:
            config.x1 += offset;
            break;
        case IMAGEINFO::ERROR_HW:
            input.width += offset;
            break;
        case IMAGEINFO::ERROR_ALIGN_HW:
            input.heightStride += offset;
            break;
        case IMAGEINFO::ERROR_TYPE:
            input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_ABGR_8888;
            break;
        case IMAGEINFO::ERROR_SIZE:
            input.dataSize -= offset;
            break;
        case IMAGEINFO::ERROR_MEMORY:
            input.destory(input.data);
            break;
        default:
            return APP_ERR_OK;
    }
    ret = g_dvppVpcProcessor->VpcCrop(input, output, config);
    if (input.data != nullptr) {
        input.destory(input.data);
    }
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return APP_ERR_OK;
}

TEST_F(DvppWrapperTest, TestVpcCropErrorConfig)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropResult(IMAGEINFO::ERROR_CONFIG);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcCropErrorHW)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropResult(IMAGEINFO::ERROR_HW);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcCropErrorAlignHW)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropResult(IMAGEINFO::ERROR_ALIGN_HW);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcCropErrorStyle)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropResult(IMAGEINFO::ERROR_TYPE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, TestVpcCropErrorDataSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropResult(IMAGEINFO::ERROR_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcCropErrorDataInfo)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropResult(IMAGEINFO::ERROR_MEMORY);
    EXPECT_EQ(ret, APP_ERR_OK);
}


void TestResult(std::vector<DvppDataInfo>& vecDvpp)
{
    for (size_t i = 0; i < vecDvpp.size(); i++) {
        std::string path = "./VpcBatchCropResize/cropResize_dvpp_" + std::to_string(i) + ".jpg";
        std::string content = MxBase::FileUtils::ReadFileContent(path);
        std::string result = CopyDataToHost(vecDvpp[i]);

        EXPECT_EQ(content, result);
    }
}

APP_ERROR GetBatch1CropNResize1Result(IMAGEINFO style)
{
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<DvppDataInfo> vecDvpp(VECSIZE, DvppDataInfo());
    std::vector<CropRoiConfig> vecCfg;
    SetCropRoiConfig(vecCfg);
    ResizeConfig resizeConfig;
    resizeConfig.width = IMAGEWIDTH;
    resizeConfig.height = IMAGEHEIGTH;
    const int offset = 1000;
    switch (style) {
        case IMAGEINFO::ERROR_SMALLER:
            resizeConfig.width = 0;
            break;
        case IMAGEINFO::ERROR_OUTOFRANGE:
            resizeConfig.height = OFFSET;
            break;
        case IMAGEINFO::ERROR_OUTPUTSIZE_NE_CONFIGSIZE:
            vecDvpp.pop_back();
            break;
        case IMAGEINFO::ERROR_TYPE:
            input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_BOTTOM;
            break;
        case IMAGEINFO::ERROR_SIZE:
            input.dataSize += offset;
            break;
        case IMAGEINFO::ERROR_HW:
            input.width = 0;
            break;
        case IMAGEINFO::ERROR_ALIGN_HW:
            input.heightStride += offset;
            break;
        case IMAGEINFO::ERROR_MEMORY:
            input.data = nullptr;
            break;
        default:
            return APP_ERR_OK;
    }
    ret = g_dvppVpcProcessor->VpcBatchCropResize(input, vecDvpp, vecCfg, resizeConfig);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    TestResult(vecDvpp);
    return APP_ERR_COMM_FAILURE;
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResize1ConfigNoEqual)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResize1Result(IMAGEINFO::ERROR_OUTPUTSIZE_NE_CONFIGSIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResize1ErrorDataInfo)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResize1Result(IMAGEINFO::ERROR_MEMORY);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResize1OutOfRange)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResize1Result(IMAGEINFO::ERROR_OUTOFRANGE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResize1Smaller)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResize1Result(IMAGEINFO::ERROR_SMALLER);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResize1ErrorHW)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResize1Result(IMAGEINFO::ERROR_HW);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResize1ErrorAlignHW)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResize1Result(IMAGEINFO::ERROR_ALIGN_HW);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResize1ErrorStyle)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResize1Result(IMAGEINFO::ERROR_TYPE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResize1ErrorDataSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResize1Result(IMAGEINFO::ERROR_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

APP_ERROR GetBatch1CropNResizeNResult(IMAGEINFO style)
{
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<DvppDataInfo> vecDvpp(VECSIZE, DvppDataInfo());
    std::vector<CropRoiConfig> vecCfg;
    SetCropRoiConfig(vecCfg);
    ResizeConfig resizeConfig;
    resizeConfig.width = RESIZEWIDTH;
    resizeConfig.height = RESIZEHEIGHT;
    std::vector<ResizeConfig> vecResize(VECSIZE, resizeConfig);
    const int offset = 1000;
    switch (style) {
        case IMAGEINFO::ERROR_SMALLER:
            vecResize[0].width = 0;
            break;
        case IMAGEINFO::ERROR_OUTOFRANGE:
            vecResize[0].height = OFFSET;
            break;
        case IMAGEINFO::ERROR_OUTPUTSIZE_NE_CONFIGSIZE:
            vecDvpp.pop_back();
            break;
        case IMAGEINFO::ERROR_TYPE:
            input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_BOTTOM;
            break;
        case IMAGEINFO::ERROR_SIZE:
            input.dataSize += offset;
            break;
        case IMAGEINFO::ERROR_IMAGESIZE_NE_RESIZESIZE:
            vecResize.pop_back();
            break;
        case IMAGEINFO::ERROR_MEMORY:
            input.data = nullptr;
            break;
        default:
            return APP_ERR_OK;
    }
    ret = g_dvppVpcProcessor->VpcBatchCropResize(input, vecDvpp, vecCfg, vecResize);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    for (size_t i = 0; i < vecDvpp.size(); i++) {
        std::string path = "./VpcBatch1CropNResizeN/VpcBatch1CropNResizeN_dvpp_" + std::to_string(i) + ".jpg";
        std::string content = MxBase::FileUtils::ReadFileContent(path);
        std::string result = CopyDataToHost(vecDvpp[i]);
        EXPECT_EQ(content, result);
    }
    return APP_ERR_COMM_FAILURE;
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResizeNSmaller)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResizeNResult(IMAGEINFO::ERROR_SMALLER);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResizeNOutOfRange)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResizeNResult(IMAGEINFO::ERROR_OUTOFRANGE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResizeNOutputNECropConfig)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResizeNResult(IMAGEINFO::ERROR_OUTPUTSIZE_NE_CONFIGSIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResizeNErrorStyle)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResizeNResult(IMAGEINFO::ERROR_TYPE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResizeNErrorDataSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResizeNResult(IMAGEINFO::ERROR_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResizeNConfigNEResizeSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResizeNResult(IMAGEINFO::ERROR_IMAGESIZE_NE_RESIZESIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNResizeNErrorDataInfo)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatch1CropNResizeNResult(IMAGEINFO::ERROR_MEMORY);
    EXPECT_NE(ret, APP_ERR_OK);
}

APP_ERROR GetBatchNCropNResizeNResult(IMAGEINFO style)
{
    std::vector<DvppDataInfo> vecIn;
    APP_ERROR ret = SetCropResizeConfig(vecIn);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<DvppDataInfo> vecDvpp(VECSIZE, DvppDataInfo());
    std::vector<CropRoiConfig> vecCfg = {
            {150, 256, 256, 150},
            {120, 256, 256, 120},
            {110, 256, 256, 110},
            {140, 256, 256, 140},
            {130, 256, 256, 130}
    };
    ResizeConfig resizeConfig;
    resizeConfig.width = CROPWIDTH;
    resizeConfig.height = CROPHEIGHT;
    std::vector<ResizeConfig> vecResize(VECSIZE, resizeConfig);
    switch (style) {
        case IMAGEINFO::ERROR_OUTPUTSIZE_NE_CONFIGSIZE:
            vecDvpp.pop_back();
            break;
        case IMAGEINFO::ERROR_IMAGESIZE_NE_RESIZESIZE:
            vecResize.pop_back();
            break;
        case IMAGEINFO::ERROR_IMAGESIZE_NE_CONFIGSIZE:
            vecIn.pop_back();
            break;
        case IMAGEINFO::ERROR_CONFIG:
            vecCfg.pop_back();
            break;
        default:
            return APP_ERR_OK;
    }
    ret = g_dvppVpcProcessor->VpcBatchCropResize(vecIn, vecDvpp, vecCfg, vecResize);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    return APP_ERR_COMM_FAILURE;
}

TEST_F(DvppWrapperTest, VpcBatchCropResizeInvalidParam)
{
    DvppDataInfo inputInfo;
    std::vector<DvppDataInfo> outputVec;
    std::vector<CropRoiConfig> cropConfigVec;
    ResizeConfig resizeConfig;
    // batch 1 crop n and resize 1
    APP_ERROR ret = g_dvppVpcProcessor->VpcBatchCropResize(inputInfo, outputVec, cropConfigVec, resizeConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // batch 1 crop n and resize n
    std::vector<ResizeConfig> resizeConfigVec;
    ret = g_dvppVpcProcessor->VpcBatchCropResize(inputInfo, outputVec, cropConfigVec, resizeConfigVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // batch n crop n and resize n
    std::vector<DvppDataInfo> inputVec;
    ret = g_dvppVpcProcessor->VpcBatchCropResize(inputVec, outputVec, cropConfigVec, resizeConfigVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    std::vector<CropRoiConfig> cropConfigVec1 = {
            {150, 256, 256, 150},
            {120, 256, 256, 120},
    };

    // batch 1 crop n and resize 1
    ret = g_dvppVpcProcessor->VpcBatchCropResize(inputInfo, outputVec, cropConfigVec1, resizeConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // batch 1 crop n and resize n
    ret = g_dvppVpcProcessor->VpcBatchCropResize(inputInfo, outputVec, cropConfigVec1, resizeConfigVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // batch n crop n and resize n
    ret = g_dvppVpcProcessor->VpcBatchCropResize(inputVec, outputVec, cropConfigVec1, resizeConfigVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // batch 1 crop n and resize 1
    std::vector<DvppDataInfo> outputVec1(1, DvppDataInfo());
    ret = g_dvppVpcProcessor->VpcBatchCropResize(inputInfo, outputVec1, cropConfigVec1, resizeConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // batch 1 crop n and resize n
    ret = g_dvppVpcProcessor->VpcBatchCropResize(inputInfo, outputVec1, cropConfigVec1, resizeConfigVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    // batch n crop n and resize n
    ret = g_dvppVpcProcessor->VpcBatchCropResize(inputVec, outputVec1, cropConfigVec1, resizeConfigVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, TestVpcBatchNCropNResizeNErrorOutputSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatchNCropNResizeNResult(IMAGEINFO::ERROR_OUTPUTSIZE_NE_CONFIGSIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatchNCropNResizeNErrorResizeSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatchNCropNResizeNResult(IMAGEINFO::ERROR_IMAGESIZE_NE_RESIZESIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatchNCropNResizeNErrorImageSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatchNCropNResizeNResult(IMAGEINFO::ERROR_IMAGESIZE_NE_CONFIGSIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatchNCropNResizeNErrorConfigSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetBatchNCropNResizeNResult(IMAGEINFO::ERROR_CONFIG);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestGetPictureDescNormal)
{
    std::string content = MxBase::FileUtils::ReadFileContent("./test1.jpg");
    // Allocate memory on the device
    MemoryData data(content.size(), MemoryData::MEMORY_DVPP);
    MemoryData src((void *)const_cast<char *>(content.c_str()), content.size(), MemoryData::MEMORY_HOST);
    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_EQ(ret, APP_ERR_OK);
    DvppImageOutput imageOutput;
    DvppImageInfo imageInfo;
    imageInfo.data = src.ptrData;
    imageInfo.size = src.size;
    imageInfo.pictureType = imageInfo.PIXEL_FORMAT_JPEG;
    ret = g_dvppJpegDecoder->GetPictureDec(imageInfo, imageOutput);
    EXPECT_EQ(ret, APP_ERR_OK);
    const int imageWidth = 357;
    const int imageHeight = 500;
    EXPECT_EQ(imageOutput.width, imageWidth);
    EXPECT_EQ(imageOutput.height, imageHeight);
}

APP_ERROR GetTestGetPictureDescResult(IMAGEINFO type)
{
    std::string content = MxBase::FileUtils::ReadFileContent("./test1.jpg");
    // Allocate memory on the device
    MemoryData data(content.size(), MemoryData::MEMORY_DVPP);
    MemoryData src((void *)const_cast<char *>(content.c_str()), content.size(), MemoryData::MEMORY_HOST);
    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(data, src);
    EXPECT_EQ(ret, APP_ERR_OK);
    DvppImageOutput imageOutput;
    DvppImageInfo imageInfo;
    imageInfo.data = src.ptrData;
    imageInfo.size = src.size;
    imageInfo.pictureType = imageInfo.PIXEL_FORMAT_JPEG;
    switch (type) {
        case IMAGEINFO::ERROR_SIZE:
            imageInfo.size = 0;
            break;
        case IMAGEINFO::ERROR_MEMORY:
            imageInfo.data = nullptr;
            break;
        case IMAGEINFO::ERROR_TYPE:
            imageInfo.pictureType = imageInfo.PIXEL_FORMAT_PNG;
            break;
        default:
            return APP_ERR_OK;
    }
    ret = g_dvppJpegDecoder->GetPictureDec(imageInfo, imageOutput);
    EXPECT_NE(ret, APP_ERR_OK);
    return APP_ERR_COMM_FAILURE;
}

TEST_F(DvppWrapperTest, TestGetPictureDescErrorDataInfo)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetTestGetPictureDescResult(IMAGEINFO::ERROR_MEMORY);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestGetPictureDescErrorType)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetTestGetPictureDescResult(IMAGEINFO::ERROR_TYPE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestGetPictureDescErrorSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetTestGetPictureDescResult(IMAGEINFO::ERROR_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestDvppJpegEncodeSmallerLevel)
{
    std::string filepath = "./test5.jpg";
    DvppDataInfo output;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, output);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = g_dvppJpegEncoder->DvppJpegEncode(output, "./Encode", "encode", -1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, TestDvppJpegEncodeOutOfRangeLevel)
{
    std::string filepath = "./test5.jpg";
    DvppDataInfo output;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, output);
    EXPECT_EQ(ret, APP_ERR_OK);
    const uint32_t level = 1000;
    ret = g_dvppJpegEncoder->DvppJpegEncode(output, "./Encode", "encode", level);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

APP_ERROR GetDvppJpegEncodeResult(IMAGEINFO type)
{
    const int offset = 1000;
    const int currEncodeLevel = 100;
    std::string filepath = "./test4.jpg";
    DvppDataInfo output;
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);

    switch (type) {
        case IMAGEINFO::ERROR_WIDTH:
            input.width += offset;
            break;
        case IMAGEINFO::ERROR_ALIGN_HW:
            input.heightStride += offset;
            break;
        case IMAGEINFO::ERROR_TYPE:
            input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_ABGR_8888;
            break;
        case IMAGEINFO::ERROR_SIZE:
            input.dataSize -= offset;
            break;
        case IMAGEINFO::ERROR_MEMORY:
            input.data = nullptr;
            break;
        default:
            return APP_ERR_OK;
    }
    ret = g_dvppJpegEncoder->DvppJpegEncode(input, output, currEncodeLevel);
    if (input.data != nullptr) {
        input.destory(input.data);
    }
    return ret;
}

TEST_F(DvppWrapperTest, TestDvppJpegEncodeErrorWidth)
{
    APP_ERROR ret = GetDvppJpegEncodeResult(IMAGEINFO::ERROR_WIDTH);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, TestDvppJpegEncodeErrorFoarmat)
{
    APP_ERROR ret = GetDvppJpegEncodeResult(IMAGEINFO::ERROR_TYPE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, TestDvppJpegEncodeErrorAlignHeight)
{
    APP_ERROR ret = GetDvppJpegEncodeResult(IMAGEINFO::ERROR_ALIGN_HW);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, TestDvppJpegEncodeErrorSize)
{
    APP_ERROR ret = GetDvppJpegEncodeResult(IMAGEINFO::ERROR_SIZE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, TestDvppJpegEncodeErrorData)
{
    APP_ERROR ret = GetDvppJpegEncodeResult(IMAGEINFO::ERROR_MEMORY);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

APP_ERROR GetVpcResizeResult(IMAGEINFO type)
{
    const int offset = 1000;
    const uint32_t width = 240;
    const uint32_t height = 230;
    std::string filepath = "./test4.jpg";
    DvppDataInfo input;
    DvppDataInfo output;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    ResizeConfig config;
    config.width = width;
    config.height = height;

    switch (type) {
        case IMAGEINFO::ERROR_WIDTH:
            input.width += offset;
            break;
        case IMAGEINFO::ERROR_ALIGN_HW:
            input.heightStride += offset;
            break;
        case IMAGEINFO::ERROR_TYPE:
            input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_ABGR_8888;
            break;
        case IMAGEINFO::ERROR_SIZE:
            input.dataSize -= offset;
            break;
        case IMAGEINFO::ERROR_MEMORY:
            input.data = nullptr;
            break;
        default:
            return APP_ERR_OK;
    }
    ret = g_dvppVpcProcessor->VpcResize(input, output, config);
    if (input.data != nullptr) {
        input.destory(input.data);
    }
    return ret;
}

TEST_F(DvppWrapperTest, TestVpcResizeErrorWidth)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcResizeResult(IMAGEINFO::ERROR_WIDTH);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcResizeErrorFoarmat)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcResizeResult(IMAGEINFO::ERROR_TYPE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcResizeErrorAlignHeight)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcResizeResult(IMAGEINFO::ERROR_ALIGN_HW);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcResizeErrorSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcResizeResult(IMAGEINFO::ERROR_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcResizeErrorData)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcResizeResult(IMAGEINFO::ERROR_MEMORY);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcResizeOutOfRange)
{
    std::string filepath = "./test5.jpg";
    DvppDataInfo input;
    DvppDataInfo output;

    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    ResizeConfig config = {200000, 200000, 0., 0., 0};
    ret = g_dvppVpcProcessor->VpcResize(input, output, config);
    EXPECT_NE(ret, APP_ERR_OK);
    input.destory(input.data);
}

TEST_F(DvppWrapperTest, TestVpcResizeSmaller)
{
    std::string filepath = "./test5.jpg";
    DvppDataInfo input;
    DvppDataInfo output;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    ResizeConfig config;
    config.width = 1;
    config.height = 1;
    ret = g_dvppVpcProcessor->VpcResize(input, output, config);
    EXPECT_NE(ret, APP_ERR_OK);
    input.destory(input.data);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNConfigNoEqual)
{
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<DvppDataInfo> vecDvpp(VEC_SIZE_4, DvppDataInfo());
    std::vector<CropRoiConfig> vecCfg;
    SetCropRoiConfig(vecCfg);
    ret = g_dvppVpcProcessor->VpcBatchCrop(input, vecDvpp, vecCfg);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

APP_ERROR GetVpcBatch1CropNResult(IMAGEINFO type)
{
    const int offset = 1000;
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<DvppDataInfo> vecDvpp(VECSIZE, DvppDataInfo());
    std::vector<CropRoiConfig> vecCfg;
    SetCropRoiConfig(vecCfg);

    switch (type) {
        case IMAGEINFO::ERROR_WIDTH:
            input.width = 0;
            break;
        case IMAGEINFO::ERROR_ALIGN_HW:
            input.heightStride += offset;
            break;
        case IMAGEINFO::ERROR_TYPE:
            input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_ABGR_8888;
            break;
        case IMAGEINFO::ERROR_SIZE:
            input.dataSize -= offset;
            break;
        case IMAGEINFO::ERROR_MEMORY:
            input.data = nullptr;
            break;
        case IMAGEINFO::ERROR_OUTPUTSIZE_NE_CONFIGSIZE:
            vecDvpp.pop_back();
            break;
        default:
            return APP_ERR_OK;
    }
    ret = g_dvppVpcProcessor->VpcBatchCrop(input, vecDvpp, vecCfg);
    if (input.data != nullptr) {
        input.destory(input.data);
    }
    return ret;
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNErrorWidth)
{
    const int formatErrorCode = 1004;
    APP_ERROR ret = GetVpcBatch1CropNResult(IMAGEINFO::ERROR_WIDTH);
    EXPECT_EQ(ret, formatErrorCode);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNErrorFoarmat)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcBatch1CropNResult(IMAGEINFO::ERROR_TYPE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNErrorAlignHeight)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcBatch1CropNResult(IMAGEINFO::ERROR_ALIGN_HW);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNErrorSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcBatch1CropNResult(IMAGEINFO::ERROR_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNErrorData)
{
    const int DataNullptrErrCode = -1;
    APP_ERROR ret = GetVpcBatch1CropNResult(IMAGEINFO::ERROR_MEMORY);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, TestVpcBatch1CropNErrorOutputSize)
{
    const int DataNullptrErrCode = 1004;
    APP_ERROR ret = GetVpcBatch1CropNResult(IMAGEINFO::ERROR_OUTPUTSIZE_NE_CONFIGSIZE);
    EXPECT_EQ(ret, DataNullptrErrCode);
}

TEST_F(DvppWrapperTest, TestVpcBatchNCropNConfigNoEqual)
{
    std::vector<DvppDataInfo> vecIn;
    APP_ERROR ret = SetCropResizeConfig(vecIn);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::vector<DvppDataInfo> vecDvpp(VEC_SIZE_5, DvppDataInfo());
    std::vector<CropRoiConfig> vecCfg = {
            {50, 156, 156, 50},
            {20, 156, 156, 20},
            {10, 156, 156, 10},
            {40, 156, 156, 40},
            {40, 156, 156, 40},
            {30, 156, 156, 30}
    };
    ret = g_dvppVpcProcessor->VpcBatchCrop(vecIn, vecDvpp, vecCfg);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, TestVpcBatchNCropNImagesNoEqual)
{
    std::vector<DvppDataInfo> vecIn;
    APP_ERROR ret = SetCropResizeConfig(vecIn);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::vector<DvppDataInfo> vecDvpp(VEC_SIZE_4, DvppDataInfo());
    std::vector<CropRoiConfig> vecCfg = {
            {50, 156, 156, 50},
            {20, 156, 156, 20},
            {10, 156, 156, 10},
            {40, 156, 156, 40},
            {30, 156, 156, 30}
    };
    ret = g_dvppVpcProcessor->VpcBatchCrop(vecIn, vecDvpp, vecCfg);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, TestVpcBatchCropInvalidParam)
{
    DvppDataInfo inputInfo;
    std::vector<DvppDataInfo> outputVec;
    std::vector<CropRoiConfig> cropConfigVec;

    APP_ERROR ret = g_dvppVpcProcessor->VpcBatchCrop(inputInfo, outputVec, cropConfigVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    std::vector<CropRoiConfig> cropConfigVec1 = {{50, 156, 156, 50}};
    ret = g_dvppVpcProcessor->VpcBatchCrop(inputInfo, outputVec, cropConfigVec1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    std::vector<DvppDataInfo> inputVec;
    ret = g_dvppVpcProcessor->VpcBatchCropMN(inputVec, outputVec, cropConfigVec1);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, TestVpcCropAndPasteNormal)
{
    std::string filepath = "./test5.jpg";
    DvppDataInfo input;
    DvppDataInfo output;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    MxBase::MemoryData dataSrc{512 * 512 *3 / 2, MxBase::MemoryData::MemoryType::MEMORY_DVPP};
    ret = MxBase::MemoryHelper::MxbsMalloc(dataSrc);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MemoryHelper::MxbsMemset(dataSrc, 0, dataSrc.size);
    EXPECT_EQ(ret, APP_ERR_OK);
    output.dataSize = dataSrc.size;
    output.width = SIZE_512;
    output.height = SIZE_512;
    output.heightStride = SIZE_512;
    output.widthStride = SIZE_512;
    output.format = input.format;
    output.data = static_cast<uint8_t*>(dataSrc.ptrData);
    output.destory = (void(*)(void*))dataSrc.free;
    CropRoiConfig pasteAre{0, 300, 300, 0};
    CropRoiConfig cropAre{0, input.width, input.height, 0};
    ret = g_dvppVpcProcessor->VpcCropAndPaste(input, output, pasteAre, cropAre);
    EXPECT_EQ(ret, APP_ERR_OK);
    input.destory(input.data);
}

void SetOutputImageInfo(const DvppDataInfo &input, DvppDataInfo &output)
{
    const int width = 512;
    const float ratio = 1.5;
    MemoryData dataSrc {(size_t)(width * width * ratio), MemoryData::MEMORY_DVPP};
    APP_ERROR ret = MemoryHelper::MxbsMalloc(dataSrc);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MemoryHelper::MxbsMemset(dataSrc, 0, dataSrc.size);
    EXPECT_EQ(ret, APP_ERR_OK);
    output.dataSize = dataSrc.size;
    output.width = width;
    output.height = width;
    output.heightStride = width;
    output.widthStride = width;
    output.format = input.format;
    output.data = static_cast<uint8_t*>(dataSrc.ptrData);
    output.destory = (void(*)(void*))dataSrc.free;
}

APP_ERROR GetVpcCropAndPasteErrorInputResult(IMAGEINFO type)
{
    const int offset = 1000;
    const int pasteWidth = 300;
    const int pasteHeight = 300;
    std::string filepath = "./test5.jpg";
    DvppDataInfo input;
    DvppDataInfo output;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    SetOutputImageInfo(input, output);
    CropRoiConfig pasteAre {0, pasteWidth, pasteHeight, 0};
    CropRoiConfig cropAre {0, input.width, input.height, 0};

    switch (type) {
        case IMAGEINFO::ERROR_WIDTH:
            input.width = 0;
            break;
        case IMAGEINFO::ERROR_ALIGN_HW:
            input.heightStride += offset;
            break;
        case IMAGEINFO::ERROR_TYPE:
            input.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_ABGR_8888;
            break;
        case IMAGEINFO::ERROR_SIZE:
            input.dataSize -= offset;
            break;
        case IMAGEINFO::ERROR_MEMORY:
            input.data = nullptr;
            break;
        default:
            return APP_ERR_OK;
    }
    ret = g_dvppCommon->VpcCropAndPaste(input, output, pasteAre, cropAre);
    if (input.data != nullptr) {
        input.destory(input.data);
    }
    output.destory(output.data);
    return ret;
}

TEST_F(DvppWrapperTest, TestVpcCropAndPasteErrorInputSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropAndPasteErrorInputResult(IMAGEINFO::ERROR_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcCropAndPasteErrorInputHW)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropAndPasteErrorInputResult(IMAGEINFO::ERROR_WIDTH);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcCropAndPasteErrorInputAlignHW)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropAndPasteErrorInputResult(IMAGEINFO::ERROR_ALIGN_HW);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcCropAndPasteErrorInputType)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropAndPasteErrorInputResult(IMAGEINFO::ERROR_TYPE);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, TestVpcCropAndPasteErrorInputMemory)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropAndPasteErrorInputResult(IMAGEINFO::ERROR_MEMORY);
    EXPECT_NE(ret, APP_ERR_OK);
}

APP_ERROR GetVpcCropAndPasteErrorOutputResult(IMAGEINFO type)
{
    const int offset = 1000;
    const int pasteWidth = 300;
    const int pasteHeight = 300;
    std::string filepath = "./test5.jpg";
    DvppDataInfo input;
    DvppDataInfo output;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    SetOutputImageInfo(input, output);
    CropRoiConfig pasteAre {0, pasteWidth, pasteHeight, 0};
    CropRoiConfig cropAre {0, input.width, input.height, 0};

    switch (type) {
        case IMAGEINFO::ERROR_WIDTH:
            output.width = 0;
            break;
        case IMAGEINFO::ERROR_ALIGN_HW:
            output.heightStride += offset;
            break;
        case IMAGEINFO::ERROR_TYPE:
            output.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_ABGR_8888;
            break;
        case IMAGEINFO::ERROR_SIZE:
            output.dataSize -= offset;
            break;
        case IMAGEINFO::ERROR_MEMORY:
            output.data = nullptr;
            break;
        default:
            return APP_ERR_OK;
    }
    ret = g_dvppVpcProcessor->VpcCropAndPaste(input, output, pasteAre, cropAre);
    if (output.data != nullptr) {
        output.destory(output.data);
    }
    input.destory(input.data);
    return ret;
}

TEST_F(DvppWrapperTest, TestVpcCropAndPasteErrorOutputSize)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropAndPasteErrorOutputResult(IMAGEINFO::ERROR_SIZE);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcCropAndPasteErrorOutputHW)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropAndPasteErrorOutputResult(IMAGEINFO::ERROR_WIDTH);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcCropAndPasteErrorOutputAlignHW)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropAndPasteErrorOutputResult(IMAGEINFO::ERROR_ALIGN_HW);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcCropAndPasteErrorOutputType)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropAndPasteErrorOutputResult(IMAGEINFO::ERROR_TYPE);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, TestVpcCropAndPasteErrorOutputMemory)
{
    APP_ERROR ret = APP_ERR_OK;
    ret = GetVpcCropAndPasteErrorOutputResult(IMAGEINFO::ERROR_MEMORY);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, Test_DvppWrapperBase_Init_Should_Return_Fail)
{
    APP_ERROR ret = g_dvppCommon->Init();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, Test_DvppWrapperBase_Init_Should_Return_Success_With_Dvpp_Channel_Mode)
{
    MxBase::MxbaseDvppChannelMode dvppChannelMode = MXBASE_DVPP_CHNMODE_DEFAULT;
    APP_ERROR ret = g_dvppCommon->Init(dvppChannelMode);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, Test_DvppWrapperBase_InitJpegEncodeChannel_Should_Return_Success_With_Jpeg_Chn_Config)
{
    JpegEncodeChnConfig jpegEncodeChnConfig;
    APP_ERROR ret = g_dvppCommon->InitJpegEncodeChannel(jpegEncodeChnConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, Test_DvppWrapperBase_InitJpegDecodeChannel_Should_Return_Success_With_Jpeg_Chn_Config)
{
    JpegDecodeChnConfig jpegDecodeChnConfig;
    APP_ERROR ret = g_dvppCommon->InitJpegDecodeChannel(jpegDecodeChnConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, Test_DvppWrapperBase_InitVpcChannel_Should_Return_Success_With_Vpc_Chn_Config)
{
    VpcChnConfig vpcChnConfig;
    APP_ERROR ret = g_dvppCommon->InitVpcChannel(vpcChnConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, Test_DvppWrapperBase_InitPngDecodeChannel_Should_Return_Success_With_Png_Chn_Config)
{
    PngDecodeChnConfig pngDecodeChnConfig;
    APP_ERROR ret = g_dvppCommon->InitPngDecodeChannel(pngDecodeChnConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, Test_DvppWrapperBase_CheckVdecInput_Should_Return_Fail_With_Invalid_InputDataInfo)
{
    MxBase::DvppDataInfo inputDataInfo;
    APP_ERROR ret = g_dvppCommon->DvppVdec(inputDataInfo, nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_OUT_OF_RANGE);

    inputDataInfo.width = MxBase::MIN_VDEC_WIDTH;
    ret = g_dvppCommon->DvppVdec(inputDataInfo, nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_OUT_OF_RANGE);

    inputDataInfo.height = MxBase::MIN_VDEC_HEIGHT;
    ret = g_dvppCommon->DvppVdec(inputDataInfo, nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    std::string filepath = "./test5.jpg";
    ret = GetDvppDataInfo(filepath, inputDataInfo);
    EXPECT_EQ(ret, APP_ERR_OK);

    inputDataInfo.resizeWidth = 1;
    ret = g_dvppCommon->DvppVdec(inputDataInfo, nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    inputDataInfo.resizeWidth = MxBase::MIN_RESIZE_WIDTH;
    ret = g_dvppCommon->DvppVdec(inputDataInfo, nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperTest, Test_DvppWrapperBase_CheckMallocMode_Should_Return_Fail_With_Invalid_InputDataInfo)
{
    MxBase::DvppDataInfo inputDataInfo;
    std::string filepath = "./test5.jpg";
    APP_ERROR ret = GetDvppDataInfo(filepath, inputDataInfo);
    EXPECT_EQ(ret, APP_ERR_OK);

    inputDataInfo.resizeWidth = MxBase::MIN_RESIZE_WIDTH;
    inputDataInfo.resizeHeight = MxBase::MIN_RESIZE_HEIGHT;
    g_dvppCommon->dvppWrapperBase_->memoryMode_ = MemoryMode::USER_MALLOC;
    ret = g_dvppCommon->DvppVdec(inputDataInfo, nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    g_dvppCommon->dvppWrapperBase_->memoryMode_ = MemoryMode::SYS_MALLOC;
    inputDataInfo.outData = inputDataInfo.data;
    ret = g_dvppCommon->DvppVdec(inputDataInfo, nullptr);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

APP_ERROR StubGetOutputDataSizeWithEmptyData(DvppDataInfo& outputData, uint32_t &outputDataSize,
                                             const OutputConstrainInfo &outputConstrainInfo)
{
    return APP_ERR_OK;
}

TEST_F(DvppWrapperTest, Test_DvppWrapperBase_MallocOutput_Should_Return_Fail_With_Empty_OutputDataInfo)
{
    MOCKER(MxBase::ConfigUtils::GetOutputDataSize, APP_ERROR(DvppDataInfo&, uint32_t &, const OutputConstrainInfo &))
        .stubs()
        .will(invoke(StubGetOutputDataSizeWithEmptyData));
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    DvppDataInfo output;
    ret = g_dvppVpcProcessor->VpcCrop(input, output, CROP_ROI_CONFIG);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

APP_ERROR StubGetOutputDataSizeWithInvalidData(DvppDataInfo& outputData, uint32_t &outputDataSize,
                                               const OutputConstrainInfo &outputConstrainInfo)
{
    outputData.data = nullptr;
    outputData.dataSize = VECSIZE;
    return APP_ERR_OK;
}

TEST_F(DvppWrapperTest, Test_DvppWrapperBase_MallocOutput_Should_Return_Fail_With_Invalid_HasSizeOutputDataInfo)
{
    MOCKER(MxBase::ConfigUtils::GetOutputDataSize, APP_ERROR(DvppDataInfo&, uint32_t &, const OutputConstrainInfo &))
        .stubs()
        .will(invoke(StubGetOutputDataSizeWithInvalidData));
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    DvppDataInfo output;
    MOCKER_CPP(&acldvppMalloc).times(1).will(returnValue(-1));
    ret = g_dvppVpcProcessor->VpcCrop(input, output, CROP_ROI_CONFIG);
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
}

TEST_F(DvppWrapperTest, Test_DvppWrapperBase_MallocOutput_Should_Return_Success_With_Valid_HasSizeOutputDataInfo)
{
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_OK);
    DvppDataInfo output;
    ret = g_dvppVpcProcessor->VpcCrop(input, output, CROP_ROI_CONFIG);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperTest, Test_DvppWrapperBase_SetInputDataInfo_Should_Return_Fail_With_DvppMalloc_Fail)
{
    MOCKER_CPP(&acldvppMalloc).times(1).will(returnValue(-1));
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
}

TEST_F(DvppWrapperTest, Test_DvppWrapperBase_SetInputDataInfo_Should_Return_Fail_With_DvppCopy_Fail)
{
    MOCKER_CPP(&aclrtMemcpy).times(1).will(returnValue(-1));
    std::string filepath = "./test1.jpg";
    DvppDataInfo input;
    APP_ERROR ret = g_dvppJpegDecoder->DvppJpegDecode(filepath, input);
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_COPY);
}
}

int main(int argc, char* argv[])
{
    DeviceGuard deviceGuard;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
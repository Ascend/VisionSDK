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
 * Description: TestMxpiImageResize.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <map>
#include <vector>
#include <gtest/gtest.h>
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxpiCommon/PluginTestHelper.h"
#include "MxPlugins/MxpiImageCrop/MxpiImageCrop.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_imagecrop)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_imagecrop);
}

class TestMxpiImageCrop : public testing::Test {
public:
    virtual void SetUp()
    {
        PluginTestHelper::gstBufferVec_.clear();
        std::cout << "SetUp()" << std::endl;
        if (APP_ERR_OK != MxBase::Log::Init()) {
            LogWarn << "failed to init log.";
        }
    }

    virtual void TearDown()
    {
        std::cout << "TearDown()" << std::endl;
    }
};

TEST_F(TestMxpiImageCrop, PropertyResizeTypeUnknown)
{
    std::map<std::string, std::string> properties = {
        {"resizeType", "xxxx"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);
}

TEST_F(TestMxpiImageCrop, PropertyPaddingTypeUnknown)
{
    std::map<std::string, std::string> properties = {
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "xxxx"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);
}

TEST_F(TestMxpiImageCrop, PropertyHandleMethodUnknown)
{
    std::map<std::string, std::string> properties = {
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "xxxx"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);
}

TEST_F(TestMxpiImageCrop, PropertyCropTypeUnknown)
{
    std::map<std::string, std::string> properties = {
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "ascend"},
        {"cropType", "xxxx"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);
}

TEST_F(TestMxpiImageCrop, PropertyDataSourceEmpty)
{
    std::map<std::string, std::string> properties = {
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "ascend"},
        {"cropType", "cropCoordinate"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);
}

TEST_F(TestMxpiImageCrop, PropertyResizeTypeSetPaddingRightDown)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"resizeType", "Resizer_KeepAspectRatio_FastRCNN"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "ascend"},
        {"cropType", "cropCoordinate"},
        {"dataSourceImage", "auto"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);
}

TEST_F(TestMxpiImageCrop, PropertyMinDimensionGtMaxDimension)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "ascend"},
        {"cropType", "cropCoordinate"},
        {"minDimension", "225"},
        {"maxDimension", "224"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);
}

TEST_F(TestMxpiImageCrop, ProcessDataSourceError)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "xxxx"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "ascend"},
        {"cropType", "cropCoordinate"},
        {"dataSourceImage", "auto"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagecrop0-ascend.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, ProcessAscendOK)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "ascend"},
        {"cropType", "cropCoordinate"},
        {"dataSourceImage", "auto"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagecrop0-ascend.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, ProcessOpencvOK)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "opencv"},
        {"cropType", "cropCoordinate"},
        {"dataSourceImage", "auto"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagecrop0-opencv.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, ProcessAscendOKKeepAspectRatioFastRCNN)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"resizeType", "Resizer_KeepAspectRatio_FastRCNN"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "ascend"},
        {"cropType", "cropCoordinate"},
        {"dataSourceImage", "auto"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeWidth", "224"},
        {"resizeHeight", "224"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagecrop0-ascend.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, ProcessAscendOKKeepAspectRatioFit)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"resizeType", "Resizer_KeepAspectRatio_Fit"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "ascend"},
        {"cropType", "cropCoordinate"},
        {"dataSourceImage", "auto"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeWidth", "224"},
        {"resizeHeight", "224"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagecrop0-ascend.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, ProcessAscendOKOnlyPadding)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"resizeType", "Resizer_OnlyPadding"},
        {"paddingType", "Padding_Around"},
        {"handleMethod", "ascend"},
        {"cropType", "cropCoordinate"},
        {"dataSourceImage", "auto"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeWidth", "224"},
        {"resizeHeight", "224"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagecrop0-ascend.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_DVPP_CROP_FAIL);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, ProcessObjectVecEmpty)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "ascend"},
        {"cropType", "cropCoordinate"},
        {"dataSourceImage", "auto"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagecrop1-ascend.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_MXPLUGINS_METADATA_IS_NULL);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, ProcessAscendMemoryHost)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "ascend"},
        {"cropType", "cropCoordinate"},
        {"dataSourceImage", "auto"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagecrop0-opencv.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, ProcessOpencvMemoryDevice)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "opencv"},
        {"cropType", "cropCoordinate"},
        {"dataSourceImage", "auto"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagecrop0-ascend.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, ProcessAscendInvalidHeight)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "ascend"},
        {"cropType", "cropCoordinate"},
        {"dataSourceImage", "auto"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagecrop0-ascend-invalidheight.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, ProcessAscendMetadataFrameNotFound)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"handleMethod", "ascend"},
        {"cropType", "cropCoordinate"},
        {"dataSourceImage", "auto"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagecrop2-ascend.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, processAutoDetectFrame)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"autoDetectFrame", "0"},
        {"cropPointx0", "100"},
        {"cropPointy0", "100"},
        {"cropPointx1", "200"},
        {"cropPointy1", "200"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagedecoder0_output.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "./output/imagecrop0_output.json" }));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, processInvalidWidth)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"autoDetectFrame", "0"},
        {"cropPointx0", "0"},
        {"cropPointy0", "100"},
        {"cropPointx1", "5000"},
        {"cropPointy1", "200"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagedecoder0_output.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_MXPLUGINS_METADATA_IS_NULL);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, processInvalidCropPoint)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"autoDetectFrame", "0"},
        {"cropPointx0", "0"},
        {"cropPointy0", "0"},
        {"cropPointx1", "0"},
        {"cropPointy1", "0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagedecoder0_output.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_MXPLUGINS_METADATA_IS_NULL);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageCrop, processInvalidWidth2)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"autoDetectFrame", "0"},
        {"cropPointx0", "1"},
        {"cropPointy0", "100"},
        {"cropPointx1", "30"},
        {"cropPointy1", "200"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageCrop>("mxpi_imagecrop", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagecrop0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagedecoder0_output.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_DVPP_CROP_FAIL);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    Init();

    return RUN_ALL_TESTS();
}

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
#include <mockcpp/mockcpp.hpp>
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxpiCommon/PluginTestHelper.h"
#include "MxPlugins/MxpiImageResize/MxpiImageResize.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_imageresize)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_imageresize);
}

class TestMxpiImageResize : public testing::Test {
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
        // clear mock
        GlobalMockObject::verify();
        std::cout << "TearDown()" << std::endl;
    }
};

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Success_When_Param_Valid)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "416"},
        {"resizeWidth", "416"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", ""},
        {"handleMethod", "ascend"},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ASSERT_TRUE(PluginTestHelper::CheckResult({ "./output/imageresize0.json" }));
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Success_When_Resizer_Rescale)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "1"},
        {"resizeHeight", "416"},
        {"resizeWidth", "416"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_Rescale"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", ""},
        {"handleMethod", "opencv"},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Success_When_Resizer_Rescale_Double)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "416"},
        {"resizeWidth", "416"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_Rescale_Double"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", ""},
        {"handleMethod", "opencv"},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Success_When_Resizer_KeepAspectRatio_Short)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "416"},
        {"resizeWidth", "416"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_KeepAspectRatio_Short"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", ""},
        {"handleMethod", "opencv"},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Success_When_Resizer_KeepAspectRatio_Long)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "416"},
        {"resizeWidth", "416"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_KeepAspectRatio_Long"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", ""},
        {"handleMethod", "opencv"},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Failed_When_DataSource_Invalid)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "xxxxx"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "416"},
        {"resizeWidth", "416"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "ascend"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_MXPLUGINS_METADATA_IS_NULL);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Failed_When_Height_Exceed_ResizeHeight)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "416"},
        {"resizeWidth", "416"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_OnlyPadding"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "ascend"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Success_When_Resizer_KeepAspectRatio_Fit_Opencv)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "512"},
        {"resizeWidth", "512"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_KeepAspectRatio_Fit"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "ascend"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Success_When_Resizer_KeepAspectRatio_FastRCNN_Ascend)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "512"},
        {"resizeWidth", "512"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_KeepAspectRatio_FastRCNN"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "ascend"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Failed_When_Memtype_Dvpp_CvProcessor_Ppencv)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "512"},
        {"resizeWidth", "512"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_MS_Yolov4"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "opencv"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Success_When_Resizer_MS_Yolov4)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "512"},
        {"resizeWidth", "512"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_MS_Yolov4"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "opencv"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Success_When_Resizer_MS_Yolov4_Padding_Around)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "512"},
        {"resizeWidth", "512"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_MS_Yolov4"},
        {"paddingType", "Padding_Around"},
        {"paddingHeight", "640"},
        {"paddingWidth", "640"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "opencv"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Success_When_Resizer_PaddleOCR)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "512"},
        {"resizeWidth", "512"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_PaddleOCR"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "opencv"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Success_When_Resizer_KeepAspectRatio_FastRCNN_Opencv)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "512"},
        {"resizeWidth", "512"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_KeepAspectRatio_FastRCNN"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "opencv"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Success_When_Resizer_KeepAspectRatio_Fit_Ascend)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "512"},
        {"resizeWidth", "512"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_KeepAspectRatio_Fit"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "opencv"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Failed_When_Resizer_KeepAspectRatio_Fit_Not_310P)
{
    MOCKER_CPP(DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    MOCKER_CPP(DeviceManager::IsAscend310B).stubs().will(returnValue(true));
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "416"},
        {"resizeWidth", "416"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_KeepAspectRatio_Fit"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "ascend"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_NE(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, TestMxpiImageResize_Should_Return_Failed_When_Height_Exceed_PaddingHeight)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "512"},
        {"resizeWidth", "512"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_OnlyPadding"},
        {"paddingType", "Padding_Around"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "1"},
        {"cvProcessor", "opencv"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TestMxpiImageResize, ProcessInvalidcvProcessor)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "512"},
        {"resizeWidth", "512"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_OnlyPadding"},
        {"paddingType", "Padding_Around"},
        {"paddingHeight", "416"},
        {"paddingWidth", "416"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "xxxx"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, ProcessOpencvOnlyPadding)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "512"},
        {"resizeWidth", "512"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_OnlyPadding"},
        {"paddingType", "xxxx"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "opencv"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, ProcessResizeTypeStretch)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "416"},
        {"resizeWidth", "416"},
        {"minDimension", "225"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "ascend"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageResize, ProcessInvalidResizeWidth)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "416"},
        {"resizeWidth", "4097"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "ascend"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_DVPP_INVALID_IMAGE_WIDTH);
}

TEST_F(TestMxpiImageResize, ProcessInvalidResizeHeight)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "4097"},
        {"resizeWidth", "416"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_NO"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", "ascend"},
        {"handleMethod", ""},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_DVPP_INVALID_IMAGE_HEIGHT);
}

TEST_F(TestMxpiImageResize, ProcessPaddingTypeAround)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "416"},
        {"resizeWidth", "416"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "Resizer_Stretch"},
        {"paddingType", "Padding_Around"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", ""},
        {"handleMethod", "opencv"},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(TestMxpiImageResize, ProcessInvalidResizeType)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"parentName", ""},
        {"removeParentData", "0"},
        {"resizeHeight", "416"},
        {"resizeWidth", "416"},
        {"minDimension", "224"},
        {"maxDimension", "224"},
        {"resizeType", "xxxx"},
        {"paddingType", "Padding_Around"},
        {"paddingHeight", "32"},
        {"paddingWidth", "32"},
        {"RGBValue", ""},
        {"interpolation", "0"},
        {"cvProcessor", ""},
        {"handleMethod", "opencv"},
        {"scaleValue", "0"},
        {"paddingColorR", "0.0"},
        {"paddingColorG", "0.0"},
        {"paddingColorB", "0.0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageResize>("mxpi_imageresize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageresize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    Init();

    return RUN_ALL_TESTS();
}

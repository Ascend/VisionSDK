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
#include "MxPlugins/MxpiImageEncoder/MxpiImageEncoder.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_imageencoder)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_imageencoder);
}

class TestMxpiImageEncoder : public testing::Test {
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

TEST_F(TestMxpiImageEncoder, ProcessOK)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, TestMxpiImageEncoder_Should_Return_Success_When_Format_Rgb)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imageresize0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, TestMxpiImageEncoder_Should_Return_Fail_When_310)
{
    MOCKER_CPP(DeviceManager::IsAscend310).stubs().will(returnValue(true));
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imageresize0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_NE(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, TestMxpiImageEncoder_Should_Return_Fail_When_MxbsFree_Second_Fail)
{
    int mockTimes = 4;
    MOCKER_CPP(MemoryHelper::MxbsFree).times(mockTimes).will(returnValue(0)).then(returnValue(1)).then(returnValue(0))
        .then(returnValue(0));
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imageresize0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, TestMxpiImageEncoder_Should_Return_Fail_When_MxbsFree_Fail)
{
    MOCKER_CPP(MemoryHelper::MxbsFree).stubs().will(returnValue(1));
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imageresize0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, ProcessDataSourceNotFound)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "xxxx"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_MXPLUGINS_METADATA_IS_NULL);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, TestMxpiImageEncoder_Should_Return_Fail_When_DataSource_Empty)
{
    std::map<std::string, std::string> properties = {
        {"dataSource1", "mxpi_imagedecoder0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_MXPLUGINS_METADATA_IS_NULL);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, ProcessMetadataMxpiObjectList)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, ProcessInvalidVisonFormat)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0-invalidvisonformat.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_DVPP_INVALID_FORMAT);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, ProcessInvalidDataType)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0-invaliddatatype.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_DVPP_INVALID_FORMAT);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, ProcessDataSizeZero)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0-invaliddatasize1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_DVPP_INVALID_FORMAT);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, ProcessInvalidDataSize)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0-invaliddatasize2.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, ProcessWidthError)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0-invalidwidth.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_DVPP_INVALID_IMAGE_WIDTH);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, ProcessHeightError)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0-invalidheight.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_DVPP_INVALID_IMAGE_HEIGHT);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageEncoder, ProcessOddHeight)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageEncoder>("mxpi_imageencoder", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imageencoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageencoder0-oddheight.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
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

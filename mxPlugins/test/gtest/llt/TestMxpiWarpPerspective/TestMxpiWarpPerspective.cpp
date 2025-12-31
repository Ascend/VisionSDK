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
 * Description: TestMxpiWarpPerspective.
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
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxpiCommon/PluginTestHelper.h"
#include "MxPlugins/MxpiWarpPerspective/MxpiWarpPerspective.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_warpperspective)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_warpperspective);
}

class TestMxpiWarpPerspective : public testing::Test {
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

/**
 * test static pad and dynamic pad.
 * Get the coordinates from static pad(pad0)
 * Get the origin image from dynamic pad(pad1)
 */
TEST_F(TestMxpiWarpPerspective, TestDynamicPad)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"oriImageDataSource", "mxpi_imagedecoder0"},
        {"debugMode", "false"},
    };

    const int sinkPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiWarpPerspective>("mxpi_warpperspective", properties,
                                                                              sinkPadCount);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_warpperspective0";
    std::vector<std::string> fileNameVec = { "mxpi_modelinfer0.json", "mxpi_imagedecoder0.json" };
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles(fileNameVec, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::vector<std::string> keys = { "mxpi_warpperspective0" };
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "result_mxpi_warpperspective0.json" }, keys));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiWarpPerspective, TestMxpiWarpPerspective_Should_Return_Success_When_Properties_Empty)
{
    std::map<std::string, std::string> properties = {
    };

    const int sinkPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiWarpPerspective>("mxpi_warpperspective", properties,
                                                                              sinkPadCount);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_warpperspective0";
    std::vector<std::string> fileNameVec = { "mxpi_modelinfer0.json", "mxpi_imagedecoder0.json" };
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles(fileNameVec, bufferVec);

    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiWarpPerspective, TestMxpiWarpPerspective_Should_Return_Success_When_DebugMode_True)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"oriImageDataSource", "mxpi_imagedecoder0"},
        {"debugMode", "true"},
    };

    const int sinkPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiWarpPerspective>("mxpi_warpperspective", properties,
                                                                              sinkPadCount);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_warpperspective0";
    std::vector<std::string> fileNameVec = { "mxpi_modelinfer0.json", "mxpi_imagedecoder0.json" };
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles(fileNameVec, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::vector<std::string> keys = { "mxpi_warpperspective0" };
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "result_mxpi_warpperspective0.json" }, keys));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiWarpPerspective, TestMxpiWarpPerspective_Should_Return_Success_When_CreateDirectories_Fail)
{
    MOCKER_CPP(FileUtils::CreateDirectories).stubs().will(returnValue(false));
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"oriImageDataSource", "mxpi_imagedecoder0"},
        {"debugMode", "true"},
    };

    const int sinkPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiWarpPerspective>("mxpi_warpperspective", properties,
                                                                              sinkPadCount);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_warpperspective0";
    std::vector<std::string> fileNameVec = { "mxpi_modelinfer0.json", "mxpi_imagedecoder0.json" };
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles(fileNameVec, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::vector<std::string> keys = { "mxpi_warpperspective0" };
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "result_mxpi_warpperspective0.json" }, keys));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiWarpPerspective, TestMxpiWarpPerspective_Should_Return_Success_When_WriteFileContent_Fail)
{
    MOCKER_CPP(FileUtils::WriteFileContent).stubs().will(returnValue(false));
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"oriImageDataSource", "mxpi_imagedecoder0"},
        {"debugMode", "true"},
    };

    const int sinkPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiWarpPerspective>("mxpi_warpperspective", properties,
                                                                              sinkPadCount);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_warpperspective0";
    std::vector<std::string> fileNameVec = { "mxpi_modelinfer0.json", "mxpi_imagedecoder0.json" };
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles(fileNameVec, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::vector<std::string> keys = { "mxpi_warpperspective0" };
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "result_mxpi_warpperspective0.json" }, keys));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

/**
 * test static pad and dynamic pad.
 * Get the coordinates from static pad(pad0)
 * Get the origin image from dynamic pad(pad1)
 */
TEST_F(TestMxpiWarpPerspective, TestStaticPad)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"oriImageDataSource", "mxpi_imagedecoder0"},
        {"debugMode", "false"},
    };

    const int sinkPadCount = 1;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiWarpPerspective>("mxpi_warpperspective", properties,
                                                                              sinkPadCount);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_warpperspective0";
    std::vector<std::string> fileNameVec = { "mxpi_modelinfer0.json" };
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles(fileNameVec, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::vector<std::string> keys = { "mxpi_warpperspective0" };
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "result_mxpi_warpperspective0.json" }, keys));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiWarpPerspective, TestPad0HadErrorInfo)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"oriImageDataSource", "mxpi_imagedecoder0"},
        {"debugMode", "false"},
    };

    const int sinkPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiWarpPerspective>("mxpi_warpperspective", properties,
                                                                              sinkPadCount);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_warpperspective0";
    std::vector<std::string> fileNameVec = { "mxpi_modelinfer0.json", "mxpi_imagedecoder0.json" };
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles(fileNameVec, bufferVec);

    MxpiErrorInfo mxpiErrorInfo;
    mxpiErrorInfo.ret = APP_ERR_COMM_FAILURE;
    mxpiErrorInfo.errorInfo = "APP_ERR_COMM_FAILURE";

    MxpiMetadataManager mxpiMetadataManager(*bufferVec[0]);
    mxpiMetadataManager.AddErrorInfo("mxpi_modelinfer0", mxpiErrorInfo);

    pluginPtr->Process(bufferVec);

    MxpiBuffer resultMxpiBuffer {PluginTestHelper::gstBufferVec_[0]};
    MxpiMetadataManager resultMxpiMetadataManager(resultMxpiBuffer);
    EXPECT_TRUE(resultMxpiMetadataManager.GetErrorInfo() != nullptr);
    auto ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiWarpPerspective, TestPad1HadErrorInfo)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"oriImageDataSource", "mxpi_imagedecoder0"},
        {"debugMode", "false"},
    };

    const int sinkPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiWarpPerspective>("mxpi_warpperspective", properties,
                                                                              sinkPadCount);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_warpperspective0";
    std::vector<std::string> fileNameVec = { "mxpi_modelinfer0.json", "mxpi_imagedecoder0.json" };
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles(fileNameVec, bufferVec);

    MxpiErrorInfo mxpiErrorInfo;
    mxpiErrorInfo.ret = APP_ERR_COMM_FAILURE;
    mxpiErrorInfo.errorInfo = "APP_ERR_COMM_FAILURE";

    MxpiMetadataManager mxpiMetadataManager(*bufferVec[1]);
    mxpiMetadataManager.AddErrorInfo("mxpi_imagedecoder0", mxpiErrorInfo);

    pluginPtr->Process(bufferVec);

    MxpiBuffer resultMxpiBuffer {PluginTestHelper::gstBufferVec_[0]};
    MxpiMetadataManager resultMxpiMetadataManager(resultMxpiBuffer);
    EXPECT_TRUE(resultMxpiMetadataManager.GetErrorInfo() != nullptr);
    auto ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    Init();

    return RUN_ALL_TESTS();
}

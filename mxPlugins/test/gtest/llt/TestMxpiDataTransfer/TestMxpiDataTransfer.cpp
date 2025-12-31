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
#include "MxPlugins/MxpiDataTransfer/MxpiDataTransfer.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_datatransfer)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_datatransfer);
}

class TestMxpiDataTransfer : public testing::Test {
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

TEST_F(TestMxpiDataTransfer, Test_MxpiDataTransfer_Should_Return_Fail_When_Properties_Empty)
{
    std::map<std::string, std::string> properties = {
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiDataTransfer>("mxpi_datatransfer", properties);
    ASSERT_NE(pluginPtr, nullptr);
}

TEST_F(TestMxpiDataTransfer, Test_MxpiDataTransfer_Should_Return_Fail_When_TransferMode_Invalid)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer1"},
        {"outputDataKeys", "mxpi_modelinfer0,mxpi_modelinfer1"},
        {"transferMode", "xxxx"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiDataTransfer>("mxpi_datatransfer", properties);
    ASSERT_NE(pluginPtr, nullptr);
}

TEST_F(TestMxpiDataTransfer, Test_MxpiDataTransfer_Should_Return_Fail_When_RemoveSourceData_Invalid)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer1"},
        {"outputDataKeys", "mxpi_modelinfer0,mxpi_modelinfer1"},
        {"transferMode", "auto"},
        {"removeSourceData", "xxxx"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiDataTransfer>("mxpi_datatransfer", properties);
    ASSERT_NE(pluginPtr, nullptr);
}

TEST_F(TestMxpiDataTransfer, Test_MxpiDataTransfer_Should_Return_Fail_When_DataSource_Invalid)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer2"},
        {"outputDataKeys", "mxpi_modelinfer0,mxpi_modelinfer1"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiDataTransfer>("mxpi_datatransfer", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_datatransfer0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/dataserialize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_MXPLUGINS_METADATA_IS_NULL);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiDataTransfer, Test_MxpiDataTransfer_Should_Return_Fail_When_Input_Is_MxpiClassList)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer1"},
        {"outputDataKeys", "mxpi_modelinfer0,mxpi_modelinfer1"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiDataTransfer>("mxpi_datatransfer", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_datatransfer0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/dataserialize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiDataTransfer, Test_MxpiDataTransfer_Should_Return_Success_When_Input_Is_MxpiVisionList)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imageresize1"}
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiDataTransfer>("mxpi_datatransfer", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_datatransfer0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/dataserialize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiDataTransfer, Test_MxpiDataTransfer_Should_Return_Success_When_Input_Is_MxpiTensorPackageList)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer3"}
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiDataTransfer>("mxpi_datatransfer", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_datatransfer0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/dataserialize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiDataTransfer, Test_MxpiDataTransfer_Should_Return_Success_When_TransferMode_Is_D2h)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer3"},
        {"transferMode", "d2h"}
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiDataTransfer>("mxpi_datatransfer", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_datatransfer0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/dataserialize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiDataTransfer, Test_MxpiDataTransfer_Should_Return_Success_When_TransferMode_Is_H2d)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer3"},
        {"transferMode", "h2d"}
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiDataTransfer>("mxpi_datatransfer", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_datatransfer0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/dataserialize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiDataTransfer, Test_MxpiDataTransfer_Should_Return_Success_When_RemoveSourceData_Is_No)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer3"},
        {"removeSourceData", "no"}
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiDataTransfer>("mxpi_datatransfer", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_datatransfer0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/dataserialize0.json"}, bufferVec);
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

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
 * Description: TestMxpiOpencvOsd.
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
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "MxBase/SingleOp/OperatorDesc.h"
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxpiCommon/PluginTestHelper.h"
#include "MxPlugins/MxpiOpencvOsd/MxpiOpencvOsd.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_opencvosd)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_opencvosd);
}

class TestMxpiOpencvOsd : public testing::Test {
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

    APP_ERROR OsdProcess()
    {
        std::map<std::string, std::string> properties = {
            {"dataSourceOsd", "mxpi_channelosdcoordsconverter0"},
            {"dataSourceImage", "mxpi_channelimagesstitcher0_0"},
        };
        auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiOpencvOsd>("mxpi_opencvosd", properties);

        pluginPtr->elementName_ = "mxpi_opencvosd0";
        std::vector<MxpiBuffer*> bufferVec;
        PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_channelimagesstitcher0_0.json",
                                                  "./mxpi_channelosdcoordsconverter0_circle.json"}, bufferVec);
        APP_ERROR ret = pluginPtr->Process(bufferVec);
        pluginPtr->DeInit();
        return ret;
    }

TEST_F(TestMxpiOpencvOsd, Test_MxpiOpencvOsd_Should_Return_Success_When_Param_Valid)
{
    auto ret = OsdProcess();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiOpencvOsd, Test_MxpiOpencvOsd_Should_Return_Fail_When_CheckDirectoryExists_Fail)
{
    MOCKER_CPP(&FileUtils::CheckDirectoryExists).stubs().will(returnValue(false));
    auto ret = OsdProcess();
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TestMxpiOpencvOsd, Test_MxpiOpencvOsd_Should_Return_Fail_When_RegularFilePath_Fail)
{
    MOCKER_CPP(&FileUtils::RegularFilePath).stubs().will(returnValue(false));
    auto ret = OsdProcess();
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TestMxpiOpencvOsd, Test_MxpiOpencvOsd_Should_Return_Fail_When_First_AddInputTensorDesc_Fail)
{
    const int exceptionTime = 1;
    MOCKER_CPP(&OperatorDesc::AddInputTensorDesc).times(exceptionTime).will(returnValue(1));
    auto ret = OsdProcess();
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TestMxpiOpencvOsd, Test_MxpiOpencvOsd_Should_Return_Fail_When_Second_AddInputTensorDesc_Fail)
{
    const int exceptionTime = 2;
    MOCKER_CPP(&OperatorDesc::AddInputTensorDesc).times(exceptionTime).will(returnValue(0)).then(returnValue(1));
    auto ret = OsdProcess();
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TestMxpiOpencvOsd, Test_MxpiOpencvOsd_Should_Return_Fail_When_Third_AddInputTensorDesc_Fail)
{
    const int exceptionTime = 3;
    MOCKER_CPP(&OperatorDesc::AddInputTensorDesc).times(exceptionTime).will(returnValue(0)).then(returnValue(0))
               .then(returnValue(1));
    auto ret = OsdProcess();
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TestMxpiOpencvOsd, Test_MxpiOpencvOsd_Should_Return_Fail_When_Fourth_AddInputTensorDesc_Fail)
{
    const int exceptionTime = 4;
    MOCKER_CPP(&OperatorDesc::AddInputTensorDesc).times(exceptionTime).will(returnValue(0)).then(returnValue(0))
               .then(returnValue(0)).then(returnValue(1));
    auto ret = OsdProcess();
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TestMxpiOpencvOsd, Test_MxpiOpencvOsd_Should_Return_Fail_When_Fifth_AddInputTensorDesc_Fail)
{
    const int exceptionTime = 5;
    MOCKER_CPP(&OperatorDesc::AddInputTensorDesc).times(exceptionTime).will(returnValue(0)).then(returnValue(0))
               .then(returnValue(0)).then(returnValue(0)).then(returnValue(1));
    auto ret = OsdProcess();
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TestMxpiOpencvOsd, Test_MxpiOpencvOsd_Should_Return_Fail_When_Sixth_AddInputTensorDesc_Fail)
{
    const int exceptionTime = 6;
    MOCKER_CPP(&OperatorDesc::AddInputTensorDesc).times(exceptionTime).will(returnValue(0)).then(returnValue(0))
               .then(returnValue(0)).then(returnValue(0)).then(returnValue(0)).then(returnValue(1));
    auto ret = OsdProcess();
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TestMxpiOpencvOsd, Test_MxpiOpencvOsd_Should_Return_Fail_When_AddOutputTensorDesc_Fail)
{
    MOCKER_CPP(&OperatorDesc::AddOutputTensorDesc).stubs().will(returnValue(1));
    auto ret = OsdProcess();
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(TestMxpiOpencvOsd, Test_MxpiOpencvOsd_Should_Return_Fail_When_DataSourceImage_InValid)
{
    std::map<std::string, std::string> properties = {
    {"dataSourceOsd", "mxpi_channelosdcoordsconverter0"},
    {"dataSourceImage", "1111"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiOpencvOsd>("mxpi_opencvosd", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_opencvosd instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }

    pluginPtr->elementName_ = "mxpi_opencvosd0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_channelimagesstitcher0_0.json",
                                              "./mxpi_channelosdcoordsconverter0_circle.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_NE(ret, APP_ERR_OK);
}
}

int main(int argc, char* argv[])
{
    MxInit();

    testing::InitGoogleTest(&argc, argv);
    Init();

    MxDeInit();
    return RUN_ALL_TESTS();
}

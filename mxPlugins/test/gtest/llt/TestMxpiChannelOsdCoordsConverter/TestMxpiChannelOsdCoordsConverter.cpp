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
 * Description: TestMxpiChannelOsdCoordsConverter.
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
#include "MxPlugins/MxpiChannelOsdCoordsConverter/MxpiChannelOsdCoordsConverter.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_channelosdcoordsconverter)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_channelosdcoordsconverter);
}

class TestMxpiChannelOsdCoordsConverter : public testing::Test {
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

TEST_F(TestMxpiChannelOsdCoordsConverter, DynamicPad)
{
    std::map<std::string, std::string> properties = {
        {"dataSourceStitchInfo", "mxpi_channelimagesstitcher0_1"},
        {"dataSourceOsds", "mxpi_channeldistributor1_0, mxpi_channeldistributor1_1"},
        {"channelIds", "0,1"},
        {"enableFixedArea", "1"},
    };
    int sinkPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiChannelOsdCoordsConverter>(
        "mxpi_channelosdcoordsconverter", properties, sinkPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_channelosdcoordsconverter instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }

    pluginPtr->elementName_ = "mxpi_channelosdcoordsconverter0";
    std::vector<MxpiBuffer *> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles(
        {"./mxpi_channelimagesstitcher0_1.json", "./mxpi_channeldistributor1_0.json",
         "mxpi_channeldistributor1_1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({"./mxpi_channelosdcoordsconverter0.json"},
        {"mxpi_channelosdcoordsconverter0"}));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiChannelOsdCoordsConverter, DrawCircle)
{
    std::map<std::string, std::string> properties = {
        {"dataSourceStitchInfo", "mxpi_channelimagesstitcher0_1"},
        {"dataSourceOsds", "mxpi_channeldistributor1_0, mxpi_channeldistributor1_1"},
        {"channelIds", "0,1"},
        {"enableFixedArea", "1"},
    };
    int sinkPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiChannelOsdCoordsConverter>(
        "mxpi_channelosdcoordsconverter", properties, sinkPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_channelosdcoordsconverter instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }

    pluginPtr->elementName_ = "mxpi_channelosdcoordsconverter0";
    std::vector<MxpiBuffer *> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles(
        {"./mxpi_channelimagesstitcher0_1.json", "./mxpi_channeldistributor1_0_circle.json",
         "mxpi_channeldistributor1_1_circle.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiChannelOsdCoordsConverter, TestMxpiChannelOsdCoordsConverter_Should_Return_Fail_When_No_Properties)
{
    std::map<std::string, std::string> properties = {};
    int sinkPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiChannelOsdCoordsConverter>(
        "mxpi_channelosdcoordsconverter", properties, sinkPadCount);

    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_channelosdcoordsconverter0";
    std::vector<MxpiBuffer *> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles(
        {"./mxpi_channelimagesstitcher0_1.json", "./mxpi_channeldistributor1_0_circle.json",
        "mxpi_channeldistributor1_1_circle.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    Init();

    return RUN_ALL_TESTS();
}

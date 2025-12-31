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
 * Description: TestMxpiChannelSelector.
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
#include "MxPlugins/MxpiChannelSelector/MxpiChannelSelector.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_channelselector)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_channelselector);
}

class TestMxpiChannelSelector : public testing::Test {
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

TEST_F(TestMxpiChannelSelector, ValidChannel)
{
    std::map<std::string, std::string> properties = {
        {"channelIds", "0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiChannelSelector>("mxpi_channelselector", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_channelselector instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }

    pluginPtr->elementName_ = "mxpi_channelselector0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_parallel2serial1.json"},
        bufferVec);
    pluginPtr->dataSourceKeys_.push_back("mxpi_parallel2serial1");
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({"./mxpi_channelselector0.json"}));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiChannelSelector, InValidChannel)
{
    std::map<std::string, std::string> properties = {
        {"channelIds", "0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiChannelSelector>("mxpi_channelselector", properties);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_channelselector instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }

    pluginPtr->elementName_ = "mxpi_channelselector0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_parallel2serial1.json1"},
                                             bufferVec);
    pluginPtr->dataSourceKeys_.push_back("mxpi_parallel2serial1");
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({"./mxpi_channelselector0.json1"}));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiChannelSelector, TestMxpiChannelSelector_Should_Return_Fail_When_ChannelIds_Invalid)
{
    std::map<std::string, std::string> properties = {
        {"channelIds", "XXXX,0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiChannelSelector>("mxpi_channelselector", properties);

    ASSERT_NE(pluginPtr, nullptr);
}


TEST_F(TestMxpiChannelSelector, TestMxpiChannelSelector_Should_Return_Fail_When_ChannelIds_Negative)
{
    std::map<std::string, std::string> properties = {
        {"channelIds", "-1"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiChannelSelector>("mxpi_channelselector", properties);

    ASSERT_NE(pluginPtr, nullptr);
}

TEST_F(TestMxpiChannelSelector, TestMxpiChannelSelector_Should_Return_Fail_When_ChannelIds_Repeat)
{
    std::map<std::string, std::string> properties = {
        {"channelIds", "1,1"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiChannelSelector>("mxpi_channelselector", properties);

    ASSERT_NE(pluginPtr, nullptr);
}
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    Init();

    return RUN_ALL_TESTS();
}

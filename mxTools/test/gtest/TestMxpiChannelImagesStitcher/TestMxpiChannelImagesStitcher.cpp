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
 * Description: TestMxpiChannelImagesStitcher.
 * Author: MindX SDK
 * Create: 2025
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
#include "MxPlugins/MxpiChannelImagesStitcher/MxpiChannelImagesStitcher.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_channelimagesstitcher)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_channelimagesstitcher);
}

class TestMxpiChannelImagesStitcher : public testing::Test {
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

TEST_F(TestMxpiChannelImagesStitcher, DynamicPad)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_channeldistributor0_0, mxpi_channeldistributor0_1"},
        {"channelIds", "0,1"},
        {"outputWidth", "1920"},
        {"outputHeight", "1080"},
    };
    int dynamicPorts = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiChannelImagesStitcher>("mxpi_channelimagesstitcher",
        properties, dynamicPorts);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_channelimagesstitcher instance failed." << std::endl;
        ASSERT_NE(pluginPtr, nullptr);
    }

    pluginPtr->elementName_ = "mxpi_channelimagesstitcher0";
    pluginPtr->outputDataKeys_ = {"mxpi_channelimagesstitcher0_0", "mxpi_channelimagesstitcher0_1"};
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_channeldistributor0_0.json",
                                             "mxpi_channeldistributor0_1.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({"./mxpi_channelimagesstitcher0_0.json"},
        {"mxpi_channelimagesstitcher0_0"}));
    pluginPtr->DeInit();
}

TEST_F(TestMxpiChannelImagesStitcher, InvalidSink)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_channeldistributor0_0, mxpi_channeldistributor0_1"},
        {"channelIds", "0,1"},
        {"outputWidth", "1920"},
        {"outputHeight", "1080"},
    };
    int dynamicPorts = 0;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiChannelImagesStitcher>("mxpi_channelimagesstitcher",
        properties, dynamicPorts);
    EXPECT_NE(pluginPtr, nullptr);
}

TEST_F(TestMxpiChannelImagesStitcher, InvalidChannel)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_channeldistributor0_0, mxpi_channeldistributor0_1"},
        {"channelIds", "33"},
        {"outputWidth", "1920"},
        {"outputHeight", "1080"},
    };
    int dynamicPorts = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiChannelImagesStitcher>("mxpi_channelimagesstitcher",
        properties, dynamicPorts);
    EXPECT_NE(pluginPtr, nullptr);
}
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    Init();

    return RUN_ALL_TESTS();
}

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
#include "MxPlugins/MxpiSkipFrame/MxpiSkipFrame.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_skipframe)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_skipframe);
}

class TestMxpiSkipFrame : public testing::Test {
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

TEST_F(TestMxpiSkipFrame, ProcessFrameNumZero)
{
    std::map<std::string, std::string> properties = {
        {"frameNum", "0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiSkipFrame>("mxpi_skipframe", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_skipframe0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/skipframe0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiSkipFrame, ProcessFrameNumTwo)
{
    std::map<std::string, std::string> properties = {
        {"frameNum", "2"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiSkipFrame>("mxpi_skipframe", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_skipframe0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/skipframe0.json"}, bufferVec);
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

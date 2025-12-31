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
 * Description: TestMxpiOsdInstanceMerger.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <gtest/gtest.h>
#include "MxBase/Log/Log.h"
#include "MxPlugins/MxpiOsdInstanceMerger/MxpiOsdInstanceMerger.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxpiCommon/PluginTestHelper.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_osdinstancemerger)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_osdinstancemerger);
}

class TestMxpiOsdInstanceMerger : public testing::Test {
public:
    virtual void SetUp()
    {
        std::cout << "SetUp()" << std::endl;
    }

    virtual void TearDown()
    {
        std::cout << "TearDown()" << std::endl;
    }
};

TEST_F(TestMxpiOsdInstanceMerger, DynamicPad) {
    std::map<std::string, std::string> properties = {
            {"dataSourceList", "mxpi_object2osdinstances0,mxpi_class2osdinstances0"},
    };
    int sinkPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiOsdInstanceMerger>("mxpi_osdinstancemerger",
                                                                                properties, sinkPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_osdinstancemerger instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }

    pluginPtr->elementName_ = "mxpi_osdinstancemerger0";
    std::vector<MxpiBuffer *> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_object2osdinstances0.json",
                                              "./mxpi_class2osdinstances0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({"./mxpi_osdinstancemerger0.json"}, {"mxpi_osdinstancemerger0"}));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiOsdInstanceMerger, InvalidSink) {
    std::map<std::string, std::string> properties = {
            {"dataSourceList", "mxpi_object2osdinstances0,mxpi_class2osdinstances0"},
    };
    int sinkPadCount = 0;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiOsdInstanceMerger>("mxpi_osdinstancemerger",
                                                                                properties, sinkPadCount);
    EXPECT_NE(pluginPtr, nullptr);
    auto ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiOsdInstanceMerger, InvalidDataSource) {
    std::map<std::string, std::string> properties = {
            {"dataSourceList", "mxpi_object2osdinstances0"},
    };
    int sinkPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiOsdInstanceMerger>("mxpi_osdinstancemerger",
                                                                                properties, sinkPadCount);
    EXPECT_NE(pluginPtr, nullptr);
    auto ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiOsdInstanceMerger, InvalidMetaData) {
    std::map<std::string, std::string> properties = {
            {"dataSourceList", "mxpi_object2osdinstances0,mxpi_class2osdinstances0"},
    };
    int sinkPadCount = 2;
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiOsdInstanceMerger>("mxpi_osdinstancemerger",
                                                                                properties, sinkPadCount);

    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_osdinstancemerger instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }

    pluginPtr->elementName_ = "mxpi_osdinstancemerger0";
    std::vector<MxpiBuffer *> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./mxpi_object2osdinstances0_no_metadata.json",
                                              "./mxpi_class2osdinstances0_no_metadata.json"}, bufferVec);
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

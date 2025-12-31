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
 * Description: SetPluginPropertyTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */


#include <iostream>
#include <gtest/gtest.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsLog.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxStream;

namespace {
class SetPluginPropertyTest : public testing::Test {
public:
    virtual void SetUp()
    {
        LogDebug << "SetUp()";
    }

    virtual void TearDown()
    {
        LogDebug << "TearDown()" << std::endl;
    }
};

TEST_F(SetPluginPropertyTest, ChannelSelector) {
    LogInfo << "********ChannelSelector********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("ChannelSelector.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.SetElementProperty("ChannelSelector", "mxpi_channelselector0", "channelIds", "9");
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SetPluginPropertyTest, ChannelOsdCoordsConverter) {
    LogInfo << "********ChannelOsdCoordsConverter********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("ChannelOsdCoordsConverter.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.SetElementProperty("ChannelOsdCoordsConverter",
        "mxpi_channelosdcoordsconverter0", "channelIds", "10");
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(SetPluginPropertyTest, ChannelImagesStitcher) {
    LogInfo << "********ChannelImagesStitcher********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("ChannelImagesStitcher.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.SetElementProperty("ChannelImagesStitcher",
        "mxpi_channelimagesstitcher0", "outputWidth", "33");
    EXPECT_EQ(ret, APP_ERR_OK);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    gst_init(&argc, &argv);
    DeviceManager* m = DeviceManager::GetInstance();
    m->InitDevices();
    int ret = RUN_ALL_TESTS();
    m->DestroyDevices();
    return ret;
}
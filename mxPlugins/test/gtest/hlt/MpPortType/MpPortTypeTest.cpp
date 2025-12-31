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
 * Description: MpPortTypeTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */
 
#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;

namespace {
class MpPortTypeTest : public testing::Test {
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

TEST_F(MpPortTypeTest, TestShortNormalPortType)
{
    LogInfo << "********case TestShortNormalPortType********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("short_normal_port_type.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    mxStreamManager.StopStream("short_normal");
}

TEST_F(MpPortTypeTest, TestShortAbnormalPortType)
{
    LogInfo << "********case TestShortAbnormalPortType********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("short_abnormal_port_type.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
    mxStreamManager.StopStream("short_abnormal");
}

TEST_F(MpPortTypeTest, TestLongNormalPortType)
{
    LogInfo << "********case TestLongNormalPortType********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("long_normal_port_type.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    mxStreamManager.StopStream("long_normal");
}

TEST_F(MpPortTypeTest, TestResizeCropPortType)
{
    LogInfo << "********case TestResizeCropPortType********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("resize_crop_port_type.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    mxStreamManager.StopStream("long_abnormal");
}

TEST_F(MpPortTypeTest, TestSplitterNormalPortType)
{
    LogInfo << "********case TestSplitterNormalPortType********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("splitter_normal_port_type.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    mxStreamManager.StopStream("splitter_normal");
}

TEST_F(MpPortTypeTest, TestSplitterAbnormalPortType)
{
    LogInfo << "********case TestSplitterAbnormalPortType********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("splitter_abnormal_port_type.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
    mxStreamManager.StopStream("splitter_wrong");
}
}

int main(int argc, char *argv[])
{
    DeviceManager* deviceManager = DeviceManager::GetInstance();
    deviceManager->InitDevices();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    deviceManager->DestroyDevices();
    return ret;
}
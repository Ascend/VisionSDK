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
 * Description: MxpiChannelSelectorTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include <MxBase/Utils/FileUtils.h>
#include <MxTools/Proto/MxpiDumpData.pb.h>
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxpiCommon/DumpDataHelper.h"

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;
using namespace MxPlugins;

namespace {
class MxpiChannelSelectorTest : public testing::Test {
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

TEST_F(MxpiChannelSelectorTest, MxpiChannelSelectorCommon)
{
    LogInfo << "********case  TestMxpiChannelSelectorCommon********";
    std::string input = FileUtils::ReadFileContent("../test_pictures/beagle^1.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/MxpiChannelSelector.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "channelselector";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    sleep(2);
    std::string result1 = FileUtils::ReadFileContent("./output/channelselector.output");
    std::string test1 = FileUtils::ReadFileContent("./output/channelselectorActual.output");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result1, test1));
    mxStreamManager.DestroyAllStreams();
}

TEST_F(MxpiChannelSelectorTest, MxpiChannelSelectorIllegalProps)
{
    LogInfo << "********case  TestMxpiChannelSelectorIllegalProps********";
    std::string input = FileUtils::ReadFileContent("../test_pictures/beagle^1.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/MxpiChannelSelectorInvalid.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "channelselector";
    uint64_t uniqueId;
    mxStreamManager.SetElementProperty(streamName, "mxpi_channelselector0", "channelIds", "");
    ret = mxStreamManager.SendDataWithUniqueId(streamName, 0, mxstDataInput, uniqueId);
    mxStreamManager.DestroyAllStreams();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxpiChannelSelectorTest, MxpiChannelSelectorIllegalChannelIds)
{
    LogInfo << "********case  TestMxpiChannelSelectorIllegalChannelIds********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret =
        mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/MxpiChannelSelectorInvalidChannel1.pipeline");
    EXPECT_NE(ret, APP_ERR_OK);
    mxStreamManager.DestroyAllStreams();
}

TEST_F(MxpiChannelSelectorTest, MxpiChannelSelectorInvalidChannelIds)
{
    LogInfo << "********case  TestMxpiChannelSelectorInvalidChannelIds********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret =
        mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/MxpiChannelSelectorInvalidChannel2.pipeline");
    EXPECT_NE(ret, APP_ERR_OK);
    mxStreamManager.DestroyAllStreams();
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
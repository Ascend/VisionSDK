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
 * Description: MxpiDistributorTest.
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
const int TIME_SLEEP = 2;
class MxpiDistributorTest : public testing::Test {
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

TEST_F(MxpiDistributorTest, SRCNUM_ONE)
{
    LogInfo << "********case1  TestDistributor(srcnum=1)********";
    std::string input = FileUtils::ReadFileContent("./input/case_1.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_1.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "distributor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("./case_1_result.output");
    std::string test = FileUtils::ReadFileContent("./output/case_1.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiDistributorTest, SRCNUM_TWO)
{
    LogInfo << "********case2  TestDistributor(srcnum=2,first level split)********";
    std::string input = FileUtils::ReadFileContent("./input/case_2.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_2.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "distributor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
    std::string result1 = FileUtils::ReadFileContent("./case_2_result_1.output");
    std::string result2 = FileUtils::ReadFileContent("./case_2_result_2.output");
    std::string test1 = FileUtils::ReadFileContent("./output/case_2_1.output");
    std::string test2 = FileUtils::ReadFileContent("./output/case_2_2.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result1, test1));
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result2, test2));
}

TEST_F(MxpiDistributorTest, SRCNUM_TWO_LEVEL)
{
    LogInfo << "********case3  TestDistributor(srcnum=2,second level split)********";
    std::string input = FileUtils::ReadFileContent("./input/case_3.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_3.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "distributor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
    std::string result1 = FileUtils::ReadFileContent("./case_3_result_1.output");
    std::string result2 = FileUtils::ReadFileContent("./case_3_result_2.output");
    std::string test1 = FileUtils::ReadFileContent("./output/case_3_1.output");
    std::string test2 = FileUtils::ReadFileContent("./output/case_3_2.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result1, test1));
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result2, test2));
}

TEST_F(MxpiDistributorTest, SRCNUM_TWO_LEVEL_DISTRIBUTEALL)
{
    LogInfo << "********case3  TestDistributor(srcnum=2,second level split, distributeAll is yes)********";
    std::string input = FileUtils::ReadFileContent("./input/case_3.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_distribute_all.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "distributor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
    std::string result1 = FileUtils::ReadFileContent("./case_3_result_1.output");
    std::string result2 = FileUtils::ReadFileContent("./case_3_result_2.output");
    std::string test1 = FileUtils::ReadFileContent("./output/case_3_1.output");
    std::string test2 = FileUtils::ReadFileContent("./output/case_3_2.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result1, test1));
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result2, test2));
}

TEST_F(MxpiDistributorTest, CHANNEL_ID)
{
    LogInfo << "********case3  TestDistributor(channelIds=0,1)********";
    std::string input = FileUtils::ReadFileContent("./input/case_3.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_5.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "distributor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
}

TEST_F(MxpiDistributorTest, SPLIT_REMOVE_BLANK)
{
    LogInfo << "********case4  TestDistributor(srcnum=2,split remove blank)********";
    std::string input = FileUtils::ReadFileContent("./input/case_3.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_4.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "distributor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
    std::string result1 = FileUtils::ReadFileContent("./case_4_result_1.output");
    std::string result2 = FileUtils::ReadFileContent("./case_4_result_2.output");
    std::string test1 = FileUtils::ReadFileContent("./output/case_3_1.output");
    std::string test2 = FileUtils::ReadFileContent("./output/case_3_2.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result1, test1));
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result2, test2));
}

TEST_F(MxpiDistributorTest, INVALID_PIPELINE)
{
    LogInfo << "********case4  TestDistributor(invalid pipeline)********";
    std::string input = FileUtils::ReadFileContent("./input/case_4.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_empty_port.pipeline");
    EXPECT_NE(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_3.pipeline");
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "distributor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
}

TEST_F(MxpiDistributorTest, INVALID_PROTOBUF_TYPE)
{
    LogInfo << "********case4  TestDistributor(invalid protobuf type)********";
    std::string input = FileUtils::ReadFileContent("./input/case_5.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_invalid_name.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();
    int inPluginId = 0;
    std::string streamName = "distributor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    MxStream::MxstDataOutput* output = mxStreamManager.GetResult(streamName, inPluginId);
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

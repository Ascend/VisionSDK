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
 * Description: MxpiSynchronizeTest.
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
class MxpiSynchronizeTest : public testing::Test {
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

TEST_F(MxpiSynchronizeTest, SINKNUM_ONE)
{
    LogInfo << "********case1  TestSynchronize(sinknum=1)********";
    std::string input1 = FileUtils::ReadFileContent("./input/case_x.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_1.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput1;
    mxstDataInput1.dataPtr = (uint32_t*) input1.c_str();
    mxstDataInput1.dataSize = input1.size();

    std::string streamName = "synchronize";
    mxStreamManager.SendData(streamName, 0, mxstDataInput1);

    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("./output/case_x.output");
    std::string test = FileUtils::ReadFileContent("./case_1_result_1.output");

    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiSynchronizeTest, SINKNUM_TWO)
{
    LogInfo << "********case1  TestSynchronize(sinknum=2)********";
    std::string input1 = FileUtils::ReadFileContent("./input/case_x.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_2.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput1;
    mxstDataInput1.dataPtr = (uint32_t*) input1.c_str();
    mxstDataInput1.dataSize = input1.size();

    std::string streamName = "synchronize";
    mxStreamManager.SendData(streamName, 0, mxstDataInput1);

    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("./output/case_x.output");
    std::string test = FileUtils::ReadFileContent("./case_2_result_1.output");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiSynchronizeTest, SINKNUM_THREE)
{
    LogInfo << "********case3  TestSynchronize(sinknum=3)********";
    std::string input1 = FileUtils::ReadFileContent("./input/case_x.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_3.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput1;
    mxstDataInput1.dataPtr = (uint32_t*) input1.c_str();
    mxstDataInput1.dataSize = input1.size();

    std::string streamName = "synchronize";
    mxStreamManager.SendData(streamName, 0, mxstDataInput1);

    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("./output/case_x.output");
    std::string test = FileUtils::ReadFileContent("./case_3_result_1.output");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result, test));
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

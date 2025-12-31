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
class MxpiRoiGeneratorTest : public testing::Test {
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

TEST_F(MxpiRoiGeneratorTest, SizeBlock)
{
    LogInfo << "********case1  TestMxpiRoiGenerator (SizeBlock)********";
    std::string input = FileUtils::ReadFileContent("./input/case.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/size_block.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "sizeblock";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string result1 = FileUtils::ReadFileContent("./output/case_1_1.output");
    std::string test1 = FileUtils::ReadFileContent("./output/case_1_result_1.output");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result1, test1));
    std::string result2 = FileUtils::ReadFileContent("./output/case_1_2.output");
    std::string test2 = FileUtils::ReadFileContent("./output/case_1_result_2.output");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result2, test2));
}

TEST_F(MxpiRoiGeneratorTest, NumBlock)
{
    LogInfo << "********case2  TestMxpiRoiGenerator (NumBlock)********";
    std::string input = FileUtils::ReadFileContent("./input/case.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/num_block.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "numblock";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string result1 = FileUtils::ReadFileContent("./output/case_2_1.output");
    std::string test1 = FileUtils::ReadFileContent("./output/case_2_result_1.output");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result1, test1));
    std::string result2 = FileUtils::ReadFileContent("./output/case_2_2.output");
    std::string test2 = FileUtils::ReadFileContent("./output/case_2_result_2.output");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result2, test2));
}

TEST_F(MxpiRoiGeneratorTest, Custom)
{
    LogInfo << "********case3  TestMxpiRoiGenerator (Custom)********";
    std::string input = FileUtils::ReadFileContent("./input/case.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/custom.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "custom";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string result1 = FileUtils::ReadFileContent("./output/case_3_1.output");
    std::string test1 = FileUtils::ReadFileContent("./output/case_3_result_1.output");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result1, test1));
    std::string result2 = FileUtils::ReadFileContent("./output/case_3_2.output");
    std::string test2 = FileUtils::ReadFileContent("./output/case_3_result_2.output");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result2, test2));
}

TEST_F(MxpiRoiGeneratorTest, ErrorSrcPadNum)
{
    LogInfo << "********case4  TestMxpiRoiGenerator (Error_src_num)********";
    std::string input = FileUtils::ReadFileContent("./input/case.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/error_src_pad_num.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
    sleep(2);
    mxStreamManager.DestroyAllStreams();
}

TEST_F(MxpiRoiGeneratorTest, Error0)
{
    LogInfo << "********case5  TestMxpiRoiGenerator (Error0)********";
    std::string input = FileUtils::ReadFileContent("./input/case.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/parameters_error_0.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "error0";
    ret = mxStreamManager.SendData(streamName, 0, mxstDataInput);
    EXPECT_EQ(ret, APP_ERR_OK);
    sleep(2);
    mxStreamManager.DestroyAllStreams();
}

TEST_F(MxpiRoiGeneratorTest, Error1)
{
    LogInfo << "********case6  TestMxpiRoiGenerator (Error1)********";
    std::string input = FileUtils::ReadFileContent("./input/case.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/parameters_error_1.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);

    sleep(2);
    mxStreamManager.DestroyAllStreams();
}

TEST_F(MxpiRoiGeneratorTest, Error2)
{
    LogInfo << "********case7  TestMxpiRoiGenerator (Error2)********";
    std::string input = FileUtils::ReadFileContent("./input/case.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/parameters_error_2.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);

    sleep(2);
    mxStreamManager.DestroyAllStreams();
}

TEST_F(MxpiRoiGeneratorTest, Error3)
{
    LogInfo << "********case8  TestMxpiRoiGenerator (Error3)*****123451864512****";
    std::string input = FileUtils::ReadFileContent("./input/case.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/parameters_error_3.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "error3";
    ret = mxStreamManager.SendData(streamName, 0, mxstDataInput);
    EXPECT_EQ(ret, APP_ERR_OK);
    sleep(2);
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
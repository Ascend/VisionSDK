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
class MxpiObjectSelectorTest : public testing::Test {
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

TEST_F(MxpiObjectSelectorTest, Test0)
{
    LogInfo << "********case0  TestMxpiObjectSelector ********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/InvalidChar.pipeline");
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxpiObjectSelectorTest, Test1)
{
    LogInfo << "********case1  TestMxpiObjectSelector ********";
    std::string input = FileUtils::ReadFileContent("./input/ObjectSelector_1.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/ObjectSelector_1.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "ObjectSelector_1";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("./output/ObjectSelectorActual_1.output");
    std::string test = FileUtils::ReadFileContent("./output/ObjectSelector_1.output");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiObjectSelectorTest, Test2)
{
LogInfo << "********case2  TestMxpiObjectSelector ********";
std::string input = FileUtils::ReadFileContent("./input/ObjectSelector_1.input");
MxStreamManager mxStreamManager;
mxStreamManager.InitManager();
APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/ObjectSelector_2.pipeline");
EXPECT_EQ(ret, APP_ERR_OK);

MxstDataInput mxstDataInput;
mxstDataInput.dataPtr = (uint32_t *) input.c_str();
mxstDataInput.dataSize = input.size();

std::string streamName = "ObjectSelector_2";
mxStreamManager.SendData(streamName, 0, mxstDataInput);

sleep(2);
mxStreamManager.DestroyAllStreams();
std::string result = FileUtils::ReadFileContent("./output/ObjectSelectorActual_2.output");
std::string test = FileUtils::ReadFileContent("./output/ObjectSelector_1.output");
EXPECT_TRUE(DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiObjectSelectorTest, ERRORTest3)
{
LogInfo << "********case3  TestMxpiObjectSelector ********";
std::string input = FileUtils::ReadFileContent("./input/ObjectSelector_1.input");
MxStreamManager mxStreamManager;
mxStreamManager.InitManager();
APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/ObjectSelector_3.pipeline");
EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxpiObjectSelectorTest, ERRORTest4)
{
LogInfo << "********case4  TestMxpiObjectSelector ********";
std::string input = FileUtils::ReadFileContent("./input/ObjectSelector_1.input");
MxStreamManager mxStreamManager;
mxStreamManager.InitManager();
APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/ObjectSelector_4.pipeline");
EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxpiObjectSelectorTest, ERRORTest5)
{
LogInfo << "********case5  TestMxpiObjectSelector ********";
std::string input = FileUtils::ReadFileContent("./input/ObjectSelector_1.input");
MxStreamManager mxStreamManager;
mxStreamManager.InitManager();
APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/ObjectSelector_5.pipeline");
EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxpiObjectSelectorTest, ERRORTest6)
{
LogInfo << "********case6  TestMxpiObjectSelector ********";
std::string input = FileUtils::ReadFileContent("./input/ObjectSelector_1.input");
MxStreamManager mxStreamManager;
mxStreamManager.InitManager();
APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/ObjectSelector_6.pipeline");
EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxpiObjectSelectorTest, ERRORTest7)
{
LogInfo << "********case7  TestMxpiObjectSelector ********";
std::string input = FileUtils::ReadFileContent("./input/ObjectSelector_1.input");
MxStreamManager mxStreamManager;
mxStreamManager.InitManager();
APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/ObjectSelector_7.pipeline");
EXPECT_NE(ret, APP_ERR_OK);
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
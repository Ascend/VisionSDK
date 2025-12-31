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
 * Description: MxpiNmsOverlapedRoiTest.
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
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;

namespace {
class MxpiNmsOverlapedRoiV2Test : public testing::Test {
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

TEST_F(MxpiNmsOverlapedRoiV2Test, TEST_NMSOVERLAPEDROIV2_ERROR0)
{
    LogInfo << "********case0 test plugins: mxpi_nmsoverlapedroiV2_error0 *******";
    std::string input0 = FileUtils::ReadFileContent("./input/case0.input");
    std::string input1 = FileUtils::ReadFileContent("./input/case0.input");

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_error0.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput0;
    mxstDataInput0.dataPtr = (uint32_t*) input0.c_str();
    mxstDataInput0.dataSize = input0.size();

    MxstDataInput mxstDataInput1;
    mxstDataInput1.dataPtr = (uint32_t*) input1.c_str();
    mxstDataInput1.dataSize = input1.size();

    std::string streamName = "error0";
    mxStreamManager.SendData(streamName, 0, mxstDataInput0);
    ret = mxStreamManager.SendData(streamName, 1, mxstDataInput1);
    EXPECT_EQ(ret, APP_ERR_OK);

    sleep(2);
    mxStreamManager.DestroyAllStreams();
}

TEST_F(MxpiNmsOverlapedRoiV2Test, TEST_NMSOVERLAPEDROIV2_ERROR1)
{
    LogInfo << "********case1 test plugins: mxpi_nmsoverlapedroiV2_error1*******";
    std::string input0 = FileUtils::ReadFileContent("./input/case1.input");
    std::string input1 = FileUtils::ReadFileContent("./input/case1.input");

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_error1.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput0;
    mxstDataInput0.dataPtr = (uint32_t*) input0.c_str();
    mxstDataInput0.dataSize = input0.size();
    MxstDataInput mxstDataInput1;
    mxstDataInput1.dataPtr = (uint32_t*) input1.c_str();
    mxstDataInput1.dataSize = input1.size();

    std::string streamName = "error1";
    mxStreamManager.SendData(streamName, 0, mxstDataInput0);
    ret = mxStreamManager.SendData(streamName, 1, mxstDataInput1);
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
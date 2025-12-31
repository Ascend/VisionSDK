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
 * Description: MxpiFaceAlignmentTest.
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
#include "MxTools/Proto/MxpiDumpData.pb.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "../MxpiCommon/DumpDataHelper.h"

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;
using namespace MxPlugins;

namespace {
    class MxpiFaceAlignmentTest : public testing::Test {
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


TEST_F(MxpiFaceAlignmentTest, TestFaceAlignment)
{
    LogInfo << "********case  TestFaceAlignment********";
    std::string input1 = FileUtils::ReadFileContent("input1.output");
    std::string input2 = FileUtils::ReadFileContent("input2.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("FaceAlignment.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput1;
    mxstDataInput1.dataPtr = (uint32_t*) input1.c_str();
    mxstDataInput1.dataSize = input1.size();
    MxstDataInput mxstDataInput2;
    mxstDataInput2.dataPtr = (uint32_t*) input2.c_str();
    mxstDataInput2.dataSize = input2.size();

    std::string streamName = "facealignment";
    mxStreamManager.SendData(streamName, 0, mxstDataInput1);
    mxStreamManager.SendData(streamName, 1, mxstDataInput2);

    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("facealignment.output");
    std::string test = FileUtils::ReadFileContent("result.output");

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
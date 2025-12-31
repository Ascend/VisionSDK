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
 * Description: SsdvggModelTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <cmath>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDumpData.pb.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxStream/StreamManager/MxStreamManager.h"

using namespace MxStream;
using namespace MxPlugins;
using namespace MxTools;
using namespace MxBase;

namespace {
const int NUM_FOUR = 4;

class SsdvggModelTest : public testing::Test {
public:
    virtual void SetUp()
    {
        FileUtils::RemoveDirectories("./dump_datas");
        std::cout << "SetUp()" << std::endl;
    }

    virtual void TearDown()
    {
        std::cout << "TearDown()" << std::endl;
    }
};


TEST_F(SsdvggModelTest, TestSsdvggModel)
{
    LogInfo << "********case SsdvggModelTest********";
    std::string testImage = FileUtils::ReadFileContent("ssdvgg_test.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("Ssdvgg.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) testImage.c_str();
    mxstDataInput.dataSize = testImage.size();

    std::string streamName = "im_ssdvgg";

    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    int endurance = 500000;
    int checkFrequency = 50000;
    std::string result;
    for (int j = 0; j < ceil(endurance / checkFrequency); j++) {
        if (FileUtils::CheckFileExists("./ssd_vgg_test_result.json")) {
            result = FileUtils::ReadFileContent("./ssd_vgg_test_result.json");
            break;
        }
        usleep(checkFrequency);
    }
    LogInfo << "********case TestSsdvggModel end********";
    mxStreamManager.DestroyAllStreams();

    std::string testResult = FileUtils::ReadFileContent("./ssd_vgg_test_result.json");
    std::string expectResult = FileUtils::ReadFileContent("./ssd_vgg_expect_result.json");
    EXPECT_TRUE(DumpDataHelper::CompareDumpData(testResult, expectResult));
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
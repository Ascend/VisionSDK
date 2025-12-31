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
 * Description: CrnnModelTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <cmath>
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

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;

namespace {
    class CrnnModelTest : public testing::Test {
    public:
        virtual void SetUp()
        {
            FileUtils::RemoveDirectories("./dump_datas");
            std::cout << "SetUp()" << std::endl;
        }

        virtual void TearDown()
        {
            std::cout << "TearDown()" << std::endl;
            FileUtils::RemoveDirectories("./dump_datas");
        }
    };

TEST_F(CrnnModelTest, TestCrnnModel)
{
    LogInfo << "********case TestCrnnModel********";
    std::string testImage = FileUtils::ReadFileContent("crnn_test.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("Crnn.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) testImage.c_str();
    mxstDataInput.dataSize = testImage.size();

    std::string streamName = "im_crnn";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    int endurance = 1000000;
    int checkFrequency = 50000;
    std::string result;
    for (int j = 0; j < ceil(endurance / checkFrequency); j++) {
        if (FileUtils::CheckFileExists("./dump_datas/crnn_result.json")) {
            result = FileUtils::ReadFileContent("./dump_datas/crnn_result.json");
            break;
        }
        usleep(checkFrequency);
    }
    EXPECT_TRUE(result.size() > 0);
    MxTools::MxpiDumpData dumpData;
    auto status = google::protobuf::util::JsonStringToMessage(result, &dumpData);
    EXPECT_TRUE(status.ok());

    for (auto& metaData : dumpData.metadata()) {
        if (metaData.key() == "mxpi_modelinfer0") {
            MxpiAttributeList mxpiAttributeList;
            google::protobuf::util::JsonStringToMessage(metaData.content(), &mxpiAttributeList);
            std::string attribute = mxpiAttributeList.attributevec(0).attrvalue();
            EXPECT_STREQ("2Y7543NXC8894A7", attribute.c_str());
        }
    }
    LogInfo << "********case TestCrnnModel end********";
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
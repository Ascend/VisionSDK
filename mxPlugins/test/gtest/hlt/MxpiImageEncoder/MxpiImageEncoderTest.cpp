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
 * Description: MxpiImageEncoderTest.
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
#include "MxpiCommon/DumpDataHelper.h"

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;
using namespace MxPlugins;

namespace {
const int SLEEP_TIME = 2;
}

namespace {
    class MxpiImageEncoderTest : public testing::Test {
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

TEST_F(MxpiImageEncoderTest, TestYUVFormat)
{
    LogInfo << "********case1 TestYUVFormat(input image format is YUV)********";
    std::string input = FileUtils::ReadFileContent("./input/yuv_format.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/yuv_format_test.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "imageencoder";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string target = FileUtils::ReadFileContent("./output/yuv_format_output.jpg");
    FileUtils::ModifyFilePermission("./yuv_format_result.jpg", MxBase::FILE_MODE);
    std::string result = FileUtils::ReadFileContent("./yuv_format_result.jpg");
    EXPECT_EQ(result, target);
}

TEST_F(MxpiImageEncoderTest, TestRGBFormat)
{
    LogInfo << "********case2 TestRGBFormat(input image format is RGB)********";
    std::string input = FileUtils::ReadFileContent("./input/rgb_format.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/rgb_format_test.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "imageencoder";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string target = FileUtils::ReadFileContent("./output/rgb_format_output.jpg");
    FileUtils::ModifyFilePermission("./rgb_format_result.jpg", MxBase::FILE_MODE);
    std::string result = FileUtils::ReadFileContent("./rgb_format_result.jpg");
    EXPECT_EQ(result, target);
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
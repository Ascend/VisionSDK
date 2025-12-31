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
 * Description: MxpiImageDecoderTest.
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
    class MxpiImageDecoderTest : public testing::Test {
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

TEST_F(MxpiImageDecoderTest, TestImageDecoder)
{
    LogInfo << "********case  TestImageDecoder********";
    std::string input = FileUtils::ReadFileContent("input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("imagedecoder.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "imagedecoder";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("imagedecoder.output");
    std::string test = FileUtils::ReadFileContent("result.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiImageDecoderTest, TestBmpImageDecoder)
{
    LogInfo << "********case  TestBmpImageDecoder********";
    std::string input = FileUtils::ReadFileContent("bmp_input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("bmp_image_decoder.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "imagedecoder";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("bmp_image_decoder.output");
    std::string test = FileUtils::ReadFileContent("bmp_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiImageDecoderTest, TestOpencvImageDecoder)
{
    LogInfo << "********case  TestOpencvImageDecoder********";
    std::string input = FileUtils::ReadFileContent("opencv_input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("opencv_image_decoder.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "imagedecoder";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("opencv_image_decoder.output");
    std::string test = FileUtils::ReadFileContent("opencv_result.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
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
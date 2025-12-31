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
 * Description: TestMxpiVideoDecoder.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <gtest/gtest.h>
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxpiCommon/PluginTestHelper.h"
#include "MxPlugins/MxpiVideoDecoder/MxpiVideoDecoder.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_videodecoder)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_videodecoder);
}

class TestMxpiVideoDecoder : public testing::Test {
public:
    virtual void SetUp()
    {
        PluginTestHelper::gstBufferVec_.clear();
        std::cout << "SetUp()" << std::endl;
    }

    virtual void TearDown()
    {
        std::cout << "TearDown()" << std::endl;
    }
};

TEST_F(TestMxpiVideoDecoder, InputH264)
{
    std::map<std::string, std::string> properties = {
        {"inputVideoFormat", "H264"},
        {"vdecChannelId", "0"},
        {"outMode", "1"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiVideoDecoder>(
            "mxpi_videodecoder", properties);
    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_videodecoder instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }

    pluginPtr->elementName_ = "mxpi_videodecoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./h264_vdec.input"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
    sleep(2);
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "./h264_vdec.output" }));
}

TEST_F(TestMxpiVideoDecoder, InvalidInputFormat)
{
    std::map<std::string, std::string> properties = {
    {"inputVideoFormat", "H2644"},
    {"vdecChannelId", "0"},
    {"outMode", "1"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiVideoDecoder>(
    "mxpi_videodecoder", properties);
    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_videodecoder instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }

    pluginPtr->elementName_ = "mxpi_videodecoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./h264_vdec.input"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_DVPP_H26X_DECODE_FAIL);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiVideoDecoder, InvalidImageFormat)
{
    std::map<std::string, std::string> properties = {
    {"inputVideoFormat", "H264"},
    {"outputImageFormat", "YUV420SP_NV121"},
    {"vdecChannelId", "0"},
    {"outMode", "1"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiVideoDecoder>(
    "mxpi_videodecoder", properties);
    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_videodecoder instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }

    pluginPtr->elementName_ = "mxpi_videodecoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./h264_vdec.input"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiVideoDecoder, NV21)
{
    std::map<std::string, std::string> properties = {
    {"inputVideoFormat", "H264"},
    {"outputImageFormat", "YUV420SP_NV21"},
    {"vdecChannelId", "0"},
    {"outMode", "1"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiVideoDecoder>(
    "mxpi_videodecoder", properties);
    if (pluginPtr == nullptr) {
        std::cout << "get mxpi_videodecoder instance failed." << std::endl;
        EXPECT_NE(pluginPtr, nullptr);
        return;
    }

    pluginPtr->elementName_ = "mxpi_videodecoder0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./h264_vdec.input"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    Init();

    return RUN_ALL_TESTS();
}

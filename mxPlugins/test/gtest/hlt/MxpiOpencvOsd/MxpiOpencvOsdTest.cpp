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
 * Description: MxpiOpencvOsdTest.
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
#include "MxTools/Proto/MxpiOSDType.pb.h"
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
class MxpiOpencvOsdTest : public testing::Test {
public:
    static MxStreamManager mxStreamManager;

    static void SetUpTestSuite()
    {
        mxStreamManager.InitManager();
    }

    static void TearDownTestSuite()
    {
        mxStreamManager.DestroyAllStreams();
    }
};

MxStreamManager MxpiOpencvOsdTest::mxStreamManager;

TEST_F(MxpiOpencvOsdTest, Test_Opencvosd_Success_When_Normal_Osd_Input)
{
    std::string streamName = "opencvosd1";
    std::string pipelineFile = "./pipeline/OpencvOsdNormal.pipeline";
    std::string imageFile = "./input/test.jpg";
    std::string osdFile = "./input/ExternalOsdInstancesNormal.json";
    std::string targetFile = "./output/osd_normal_golden.jpg";
    std::string resultFile = "./output/osd_normal.jpg";
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile(pipelineFile);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string inputImageData = FileUtils::ReadFileContent(imageFile);
    EXPECT_FALSE(inputImageData.empty());
    MxstDataInput inputImage;
    inputImage.dataPtr = (uint32_t*)inputImageData.c_str();
    inputImage.dataSize = inputImageData.size();
    ret = mxStreamManager.SendData(streamName, 0, inputImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string iuputOsd = FileUtils::ReadFileContent(osdFile);
    EXPECT_FALSE(iuputOsd.empty());
    auto osdInstancesList = std::make_shared<MxTools::MxpiOsdInstancesList>();
    google::protobuf::util::JsonStringToMessage(iuputOsd, osdInstancesList.get());
    MxStream::MxstProtobufIn protobuf;
    std::string keyName = "appsrc1";
    protobuf.key = keyName;
    protobuf.messagePtr = std::static_pointer_cast<google::protobuf::Message>(osdInstancesList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(protobuf);
    ret = mxStreamManager.SendProtobuf(streamName, 1, dataBufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    sleep(SLEEP_TIME);

    std::string target = FileUtils::ReadFileContent(targetFile);
    FileUtils::ModifyFilePermission(resultFile, MxBase::FILE_MODE);
    std::string result = FileUtils::ReadFileContent(resultFile);
    EXPECT_EQ(result, target);
}

TEST_F(MxpiOpencvOsdTest, Test_Opencvosd_Success_When_Empty_Osd_Input)
{
    std::string streamName = "opencvosd0";
    std::string pipelineFile = "./pipeline/OpencvOsdEmpty.pipeline";
    std::string imageFile = "./input/test.jpg";
    std::string osdFile = "./input/ExternalOsdInstancesEmpty.json";
    std::string targetFile = "./output/osd_empty_golden.jpg";
    std::string resultFile = "./output/osd_empty.jpg";
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile(pipelineFile);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string inputImageData = FileUtils::ReadFileContent(imageFile);
    EXPECT_FALSE(inputImageData.empty());
    MxstDataInput inputImage;
    inputImage.dataPtr = (uint32_t*)inputImageData.c_str();
    inputImage.dataSize = inputImageData.size();
    ret = mxStreamManager.SendData(streamName, 0, inputImage);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string iuputOsd = FileUtils::ReadFileContent(osdFile);
    EXPECT_FALSE(iuputOsd.empty());
    auto osdInstancesList = std::make_shared<MxTools::MxpiOsdInstancesList>();
    google::protobuf::util::JsonStringToMessage(iuputOsd, osdInstancesList.get());
    MxStream::MxstProtobufIn protobuf;
    std::string keyName = "appsrc1";
    protobuf.key = keyName;
    protobuf.messagePtr = std::static_pointer_cast<google::protobuf::Message>(osdInstancesList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(protobuf);
    ret = mxStreamManager.SendProtobuf(streamName, 1, dataBufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    sleep(SLEEP_TIME);

    std::string target = FileUtils::ReadFileContent(targetFile);
    FileUtils::ModifyFilePermission(resultFile, MxBase::FILE_MODE);
    std::string result = FileUtils::ReadFileContent(resultFile);
    EXPECT_EQ(result, target);
}
} // namespace
int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}
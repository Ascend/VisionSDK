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
 * Description: MpLoadDataTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <map>
#include <gtest/gtest.h>
#include <google/protobuf/util/json_util.h>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxTools/Proto/MxpiDumpData.pb.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxpiCommon/DumpDataHelper.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxStream;
using namespace MxPlugins;

namespace {
constexpr int COMMON_SLEEP_TIME = 50000;
class MpLoadDataTest : public testing::Test {
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

TEST_F(MpLoadDataTest, Test_MxpiImageDecoder_Plugin_Should_Return_Success_When_Pipeline_Is_Valid)
{
    LogInfo << "********case mxpi_imagedecoder********";

    std::string expectResult = MxBase::FileUtils::ReadFileContent("./output/imagedecoder0.json");
    std::string streamsConfig = MxBase::FileUtils::ReadFileContent("./pipeline/imagedecoder0.pipeline");

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreams(streamsConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    usleep(COMMON_SLEEP_TIME);
    mxStreamManager.StopStream("TestPlugin");

    std::string result = MxBase::FileUtils::ReadFileContent("./imagedecoder0-output.json");

    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result, expectResult));
}

TEST_F(MpLoadDataTest, Test_MxpiImageResize0_Plugin_Should_Return_Success_When_Pipeline_Is_Valid)
{
    LogInfo << "********case mxpi_imageresize0********";

    std::string expectResult = MxBase::FileUtils::ReadFileContent("./output/imageresize0.json");
    std::string streamsConfig = MxBase::FileUtils::ReadFileContent("./pipeline/imageresize0.pipeline");

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreams(streamsConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    usleep(COMMON_SLEEP_TIME);
    mxStreamManager.StopStream("TestPlugin");

    std::string result = MxBase::FileUtils::ReadFileContent("./imageresize0-output.json");

    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result, expectResult));
}

TEST_F(MpLoadDataTest, Test_MxpiModelInfer_Plugin_Should_Return_Success_When_Pipeline_Is_Valid)
{
    LogInfo << "********case mxpi_modelinfer********";

    std::string expectResult = MxBase::FileUtils::ReadFileContent("./output/modelinfer0.json");
    std::string streamsConfig = MxBase::FileUtils::ReadFileContent("./pipeline/modelinfer0.pipeline");

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreams(streamsConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    usleep(COMMON_SLEEP_TIME);
    mxStreamManager.StopStream("TestPlugin");

    std::string result = MxBase::FileUtils::ReadFileContent("./modelinfer0-output.json");

    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result, expectResult));
}

TEST_F(MpLoadDataTest, Test_MxpiImageCorp_Plugin_Should_Return_Success_When_Pipeline_Is_Valid)
{
    LogInfo << "********case mxpi_imagecrop********";

    std::string expectResult = MxBase::FileUtils::ReadFileContent("./output/imagecrop0.json");
    std::string streamsConfig = MxBase::FileUtils::ReadFileContent("./pipeline/imagecrop0.pipeline");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreams(streamsConfig);
    EXPECT_EQ(ret, APP_ERR_OK);

    usleep(COMMON_SLEEP_TIME);
    mxStreamManager.StopStream("TestPlugin");

    std::string result = MxBase::FileUtils::ReadFileContent("./imagecrop0-output.json");

    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result, expectResult));
}

TEST_F(MpLoadDataTest, Test_MxpiImageResize1_Plugin_Should_Return_Success_When_Pipeline_Is_Valid)
{
    LogInfo << "********case mxpi_imageresize1********";

    std::string expectResult = MxBase::FileUtils::ReadFileContent("./output/imageresize1.json");
    std::string streamsConfig = MxBase::FileUtils::ReadFileContent("./pipeline/imageresize1.pipeline");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreams(streamsConfig);
    EXPECT_EQ(ret, APP_ERR_OK);

    usleep(COMMON_SLEEP_TIME);
    mxStreamManager.StopStream("TestPlugin");

    std::string result = MxBase::FileUtils::ReadFileContent("./imageresize1-output.json");

    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result, expectResult));
}

TEST_F(MpLoadDataTest, Test_MxpiDataSerialize_Plugin_Should_Return_Success_When_Pipeline_Is_Valid)
{
    LogInfo << "********case mxpi_dataserialize********";

    std::string expectResult = MxBase::FileUtils::ReadFileContent("./output/dataserialize0.json");
    std::string streamsConfig = MxBase::FileUtils::ReadFileContent("./pipeline/dataserialize.pipeline");

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreams(streamsConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
    usleep(COMMON_SLEEP_TIME);
    mxStreamManager.StopStream("TestPlugin");

    std::string result = MxBase::FileUtils::ReadFileContent("./dataserialize0-output.json");

    MxTools::MxpiDumpData expectDumpData;
    google::protobuf::util::JsonStringToMessage(expectResult, &expectDumpData);

    MxTools::MxpiDumpData dumpDataResult;
    auto status = google::protobuf::util::JsonStringToMessage(result, &dumpDataResult);
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(dumpDataResult.buffer().bufferdata().size(), expectDumpData.buffer().bufferdata().size());
    EXPECT_EQ(dumpDataResult.buffer().bufferdata(), expectDumpData.buffer().bufferdata());

    EXPECT_TRUE(DumpDataHelper::CompareDumpData(result, expectResult));
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
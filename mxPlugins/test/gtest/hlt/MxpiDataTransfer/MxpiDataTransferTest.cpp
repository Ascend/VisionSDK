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
 * Description: MxpiDataTransferTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "gtest/gtest.h"
#include "google/protobuf/util/json_util.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDumpData.pb.h"
#include "MxpiCommon/DumpDataHelper.h"

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;
using namespace MxPlugins;

namespace {
unsigned int sleepTime = 100000; // unit: microsecond

APP_ERROR DataGeneration(std::string pipelineFileName)
{
    std::string catImage = FileUtils::ReadFileContent("./input/test.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile(pipelineFileName);
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) catImage.c_str();
    mxstDataInput.dataSize = catImage.size();

    std::string streamName = "GetDataPipeline";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    usleep(sleepTime);
    mxStreamManager.StopStream("GetDataPipeline");

    return ret;
}

class MxpiDataTransferTest : public testing::Test {
public:
    virtual void SetUp()
    {
        std::cout << "SetUp()" << std::endl;
        APP_ERROR ret = DataGeneration("./pipeline/GenerateVisionData.pipeline");
        EXPECT_EQ(ret, APP_ERR_OK);
        ret = DataGeneration("./pipeline/GenerateTensorData.pipeline");
        EXPECT_EQ(ret, APP_ERR_OK);
    }

    virtual void TearDown()
    {
        std::cout << "TearDown()" << std::endl;
    }
};

TEST_F(MxpiDataTransferTest, TestVision)
{
    LogInfo << "********case  TestVision********";

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/DataTransferVision.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    usleep(sleepTime);
    ret = mxStreamManager.StopStream("GetDataPipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string groundtruthResult = FileUtils::ReadFileContent("./output/VisionResultGroundtruth.json");
    std::string targetResult = FileUtils::ReadFileContent("./output/VisionResultTarget.json");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(targetResult, groundtruthResult));
}

TEST_F(MxpiDataTransferTest, TestTensor)
{
    LogInfo << "********case  TestTensor********";

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/DataTransferTensor.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    usleep(sleepTime);
    ret = mxStreamManager.StopStream("GetDataPipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string groundtruthResult = FileUtils::ReadFileContent("./output/TensorResultGroundtruth.json");
    std::string targetResult = FileUtils::ReadFileContent("./output/TensorResultTarget.json");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(targetResult, groundtruthResult));
}

TEST_F(MxpiDataTransferTest, TestUnknownType)
{
    LogInfo << "********case  TestTensor********";

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/DataTransferUnknown.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    usleep(sleepTime);
    ret = mxStreamManager.StopStream("GetDataPipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string groundtruthResult = FileUtils::ReadFileContent("./output/UnkownResultGroundtruth.json");
    FileUtils::ModifyFilePermission("./output/UnknownResultTarget.json", MxBase::FILE_MODE);
    std::string targetResult = FileUtils::ReadFileContent("./output/UnknownResultTarget.json");
    EXPECT_EQ(targetResult, groundtruthResult);
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
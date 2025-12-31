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
 * Description: AutoDataSourceTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include <MxBase/Utils/FileUtils.h>
#include <MxTools/Proto/MxpiDumpData.pb.h>
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxpiCommon/DumpDataHelper.h"

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;
using namespace MxPlugins;

namespace {
class AutoDataSourceTest : public testing::Test {
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
/*
 * plugins that has property dataSource:
 *      mxpi_motsimplesortV2, mxpi_nmsoverlapedroiV2, mxpi_imagecrop, mxpi_datatransfer, mxpi_facealignment
 *      mxpi_videoencoder, mxpi_imageencoder, mxpi_parallel2serial, mxpi_imageresize, mxpi_modelinfer, mxpi_distributor
 *
 *  test_case_1: mxpi_imageresize, mxpi_modelinfer, mxpi_distributor
 *
 *  test_case_2: mxpi_imageencoder
 *
 *  test_case_3: mxpi_parallel2serial, mxpi_videoencoder
 *
 *  test_case_4: mxpi_facealignment
 *
 *  test_case_5: mxpi_imagecrop
 *
 *  test_case_6: mxpi_nmsoverlapedroiV2
 *
 *  test_case_7: mxpi_motsimplesortV2
 *
 *  test_case_8: mxpi_datatransfer
 */

TEST_F(AutoDataSourceTest, TEST_RESIZE_MODELINFER_DISTRIBUTOR)
{
    LogInfo << "********case1 test plugins: mxpi_imageresize, mxpi_modelinfer, mxpi_distributor********";
    std::string input = FileUtils::ReadFileContent("./input/case1.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_1.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "autoDataSource";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string expect = FileUtils::ReadFileContent("./output/case1.output");
    std::string result = FileUtils::ReadFileContent("./case1_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, expect));
}

TEST_F(AutoDataSourceTest, TEST_IMAGEENCODER)
{
    LogInfo << "********case2 test plugins: mxpi_imageencoder********";
    std::string input = FileUtils::ReadFileContent("./input/case2.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_2.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "autoDataSource";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string expect = FileUtils::ReadFileContent("./output/case2.jpg");
    FileUtils::ModifyFilePermission("./encode_result.jpg", MxBase::FILE_MODE);
    std::string result = FileUtils::ReadFileContent("./encode_result.jpg");
    EXPECT_EQ(result, expect);
}

TEST_F(AutoDataSourceTest, TEST_PARALLEL2SERIAL_VIDEOENCODER)
{
    LogInfo << "********case3 test plugins: mxpi_parallel2serial, mxpi_videoencoder********";
    std::string input0 = FileUtils::ReadFileContent("./input/case3.input0");
    std::string input1 = FileUtils::ReadFileContent("./input/case3.input1");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_3.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput0;
    mxstDataInput0.dataPtr = (uint32_t*) input0.c_str();
    mxstDataInput0.dataSize = input0.size();

    MxstDataInput mxstDataInput1;
    mxstDataInput1.dataPtr = (uint32_t*) input1.c_str();
    mxstDataInput1.dataSize = input1.size();

    std::string streamName = "autoDataSource";
    mxStreamManager.SendData(streamName, 0, mxstDataInput0);
    mxStreamManager.SendData(streamName, 1, mxstDataInput1);
    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string expect0 = FileUtils::ReadFileContent("./output/case3.output");
    std::string result0 = FileUtils::ReadFileContent("./case3_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result0, expect0));
    std::string expect1 = FileUtils::ReadFileContent("./output/case3.output1");
    std::string result1 = FileUtils::ReadFileContent("./case3_result.output1");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result1, expect1));
}

TEST_F(AutoDataSourceTest, TEST_FACEALIGNMENT)
{
    LogInfo << "********case4 test plugins: mxpi_facealignment********";
    std::string input0 = FileUtils::ReadFileContent("./input/case4.input0");
    std::string input1 = FileUtils::ReadFileContent("./input/case4.input1");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_4.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput0;
    mxstDataInput0.dataPtr = (uint32_t*) input0.c_str();
    mxstDataInput0.dataSize = input0.size();

    MxstDataInput mxstDataInput1;
    mxstDataInput1.dataPtr = (uint32_t*) input1.c_str();
    mxstDataInput1.dataSize = input1.size();

    std::string streamName = "autoDataSource";
    mxStreamManager.SendData(streamName, 0, mxstDataInput0);
    mxStreamManager.SendData(streamName, 1, mxstDataInput1);
    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string expect = FileUtils::ReadFileContent("./output/case4.output");
    std::string result = FileUtils::ReadFileContent("./case4_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, expect));
}

TEST_F(AutoDataSourceTest, TEST_IMAGECROP)
{
    LogInfo << "********case5 test plugins: mxpi_imagecrop********";
    std::string input = FileUtils::ReadFileContent("./input/case5.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_5.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "autoDataSource";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string expect = FileUtils::ReadFileContent("./output/case5.output");
    std::string result = FileUtils::ReadFileContent("./case5_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, expect));
}

TEST_F(AutoDataSourceTest, TEST_NMSOVERLAPEDROIV2)
{
    LogInfo << "********case6 test plugins: mxpi_nmsoverlapedroiV2********";
    std::string input0 = FileUtils::ReadFileContent("./input/case6.input");
    std::string input1 = FileUtils::ReadFileContent("./input/case6.input");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_6.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput0;
    mxstDataInput0.dataPtr = (uint32_t*) input0.c_str();
    mxstDataInput0.dataSize = input0.size();

    MxstDataInput mxstDataInput1;
    mxstDataInput1.dataPtr = (uint32_t*) input1.c_str();
    mxstDataInput1.dataSize = input1.size();

    std::string streamName = "autoDataSource";
    mxStreamManager.SendData(streamName, 0, mxstDataInput0);
    mxStreamManager.SendData(streamName, 1, mxstDataInput1);
    sleep(2);
    mxStreamManager.DestroyAllStreams();
    std::string expect = FileUtils::ReadFileContent("./output/case6.output");
    std::string result = FileUtils::ReadFileContent("./case6_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, expect));
}

TEST_F(AutoDataSourceTest, TEST_RESIZE_INVALID)
{
    LogInfo << "********case1 test plugins: mxpi_imageresize invalid********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_1_invalid_resize.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
}

TEST_F(AutoDataSourceTest, TEST_MODELINFER_INVALID)
{
    LogInfo << "********case1 test plugins: mxpi_modelinfer invalid********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_1_invalid_modelinfer.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
}

TEST_F(AutoDataSourceTest, TEST_DISTRIBUTOR_INVALID)
{
    LogInfo << "********case1 test plugins: mxpi_distributor invalid********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile(
        "./pipeline/test_case_1_invalid_distributor.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
}

TEST_F(AutoDataSourceTest, TEST_IMAGEENCODER_INVALID)
{
    LogInfo << "********case2 test plugins: mxpi_imageencoder invalid********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_2_invalid.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
}

TEST_F(AutoDataSourceTest, TEST_VIDEOENCODER_INVALID)
{
    LogInfo << "********case3 test plugins: mxpi_videoencoder invalid********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_3_invalid.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
}

TEST_F(AutoDataSourceTest, TEST_FACEALIGNMENT_INVALID_PORT0)
{
    LogInfo << "********case4 test plugins: mxpi_facealignment invalid port 0********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_4_invalid_port0.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
}

TEST_F(AutoDataSourceTest, TEST_FACEALIGNMENT_INVALID_PORT1)
{
    LogInfo << "********case4 test plugins: mxpi_facealignment invalid port 1********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_4_invalid_port1.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
}

TEST_F(AutoDataSourceTest, TEST_IMAGECROP_INVALID)
{
    LogInfo << "********case5 test plugins: mxpi_imagecrop invalid********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_5_invalid.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
}

TEST_F(AutoDataSourceTest, TEST_NMSOVERLAPEDROIV2_INVALID_PORT0)
{
    LogInfo << "********case6 test plugins: mxpi_nmsoverlapedroiV2 invalid port 0********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_6_invalid_port0.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
}

TEST_F(AutoDataSourceTest, TEST_NMSOVERLAPEDROIV2_INVALID_PORT1)
{
    LogInfo << "********case6 test plugins: mxpi_nmsoverlapedroiV2 invalid port 1********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_6_invalid_port1.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
}

TEST_F(AutoDataSourceTest, TEST_MOTSIMPLESORTV2_INVALID_PORT0)
{
    LogInfo << "********case7 test plugins: mxpi_motsimplesortV2 invalid port 0********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_7_invalid_port0.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
}

TEST_F(AutoDataSourceTest, TEST_MOTSIMPLESORTV2_INVALID_PORT1)
{
    LogInfo << "********case7 test plugins: mxpi_motsimplesortV2 invalid port 1********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/test_case_7_invalid_port1.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_CHANGE_STATE_FAILED);
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

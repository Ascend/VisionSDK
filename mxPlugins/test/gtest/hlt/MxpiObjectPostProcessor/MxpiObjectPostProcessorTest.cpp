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
 * Description: MultiBatchsizeTest.
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
class MxpiObjectPostProcessorTest : public testing::Test {
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

TEST_F(MxpiObjectPostProcessorTest, TestBasic)
{
    LogInfo << "********case  MxpiObjectPostProcessorTest********";
    std::string input = FileUtils::ReadFileContent("dog.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("ObjectPostProcessor.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "objectpostprocessor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("objectpostprocessor.output");
    std::string test = FileUtils::ReadFileContent("result.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiObjectPostProcessorTest, TestMaskRcnn)
{
    LogInfo << "********case  MxpiObjectPostProcessorTestMaskRcnn********";
    std::string input = FileUtils::ReadFileContent("dog.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/MaskRcnn.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "maskrcnn";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("./output/maskRcnn.output");
    std::string test = FileUtils::ReadFileContent("./output/maskrcnnActual.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiObjectPostProcessorTest, TestYolov5)
{
    LogInfo << "********case  MxpiObjectPostProcessorTestYolov5********";
    std::string input = FileUtils::ReadFileContent("dog.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/yolov5.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "yolov5";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("./output/yolov5.output");
    std::string test = FileUtils::ReadFileContent("./output/yolov5Actual.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiObjectPostProcessorTest, TestSsdMindspore)
{
    LogInfo << "********case  MxpiObjectPostProcessorTestSsdMindspore********";
    std::string input = FileUtils::ReadFileContent("dog.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/SsdMindspore.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "ssdmindspore";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("./output/ssd_mindspore.output");
    std::string test = FileUtils::ReadFileContent("./output/ssd_mindsporeActual.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiObjectPostProcessorTest, TestRetinanet)
{
    LogInfo << "********case  MxpiObjectPostProcessorTestRetinanet********";
    std::string input = FileUtils::ReadFileContent("dog.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/Retinanet.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "retinanet";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("./output/retinanet.output");
    std::string test = FileUtils::ReadFileContent("./output/retinanetActual.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiObjectPostProcessorTest, TestYolov3PostProcess)
{
    LogInfo << "********case  TestYolov3PostProcess********";
    std::string input = FileUtils::ReadFileContent("dog.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("Yolov3PostProcess.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "objectpostprocessor";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("Yolov3PostProcess.output");
    std::string test = FileUtils::ReadFileContent("Yolov3PostProcessResult.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiObjectPostProcessorTest, TestYolov4MSPostProcess)
{
    LogInfo << "********case  TestYolov4MSPostProcess********";
    std::string input = FileUtils::ReadFileContent("yolov4MS.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/yolov4ms.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "detection";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("./output/yolov4msActual.output");
    std::string test = FileUtils::ReadFileContent("./output/yolov4ms.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiObjectPostProcessorTest, TestFasterRcnnFPN)
{
    LogInfo << "********case  MxpiObjectPostProcessorTestFasterRcnnFPN********";
    std::string input = FileUtils::ReadFileContent("dog.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/FasterRcnnFPN.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "FasterRcnnFPN";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(2);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("./output/FasterRcnnFPN.output");
    std::string test = FileUtils::ReadFileContent("./output/FasterRcnnFPNActual.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiObjectPostProcessorTest, TestFasterRcnnMS)
{
    LogInfo << "********case  MxpiObjectPostProcessorTestFasterRcnnMS********";
    std::string input = FileUtils::ReadFileContent("fasterrcnnMs.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/FasterRcnnMs.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    float tensor[4] = {3648, 5472, 0.374269006, 0.374269006};
    auto tensorPackageList = std::make_shared<MxTools::MxpiTensorPackageList>();
    auto tensorPackage = tensorPackageList->add_tensorpackagevec();
    auto tensorVec = tensorPackage->add_tensorvec();
    tensorVec->set_tensordataptr((uint64_t)tensor);
    tensorVec->set_tensordatasize(16);
    tensorVec->add_tensorshape(1);
    tensorVec->add_tensorshape(4);
    tensorVec->set_tensordatatype(0);
    MxStream::MxstProtobufIn protobuf;
    protobuf.key = "appsrc1";
    protobuf.messagePtr = std::static_pointer_cast<google::protobuf::Message>(tensorPackageList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(protobuf);

    std::string streamName = "fasterrcnnMs";
    mxStreamManager.SendData(streamName, "appsrc0", mxstDataInput);
    mxStreamManager.SendProtobuf(streamName, "appsrc1", dataBufferVec);
    sleep(2);
    mxStreamManager.StopStream(streamName);
    std::string result = FileUtils::ReadFileContent("./output/FasterRcnnMs.output");
    std::string test = FileUtils::ReadFileContent("./output/FasterRcnnMsActual.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiObjectPostProcessorTest, TestMaskRcnnMs)
{
    LogInfo << "********case  MxpiObjectPostProcessorTestMaskRcnnMs********";
    std::string input0 = FileUtils::ReadFileContent("../models/maskrcnnMs/maskrcnnMs.input0");
    std::string input1 = FileUtils::ReadFileContent("../models/maskrcnnMs/maskrcnnMs.input1");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("./pipeline/MaskRcnnMs.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput0, mxstDataInput1;
    mxstDataInput0.dataPtr = (uint32_t*) input0.c_str();
    mxstDataInput0.dataSize = input0.size();
    mxstDataInput1.dataPtr = (uint32_t*) input1.c_str();
    mxstDataInput1.dataSize = input1.size();

    std::string streamName = "maskrcnnMs";
    mxStreamManager.SendData(streamName, "appsrc0", mxstDataInput0);
    mxStreamManager.SendData(streamName, "appsrc1", mxstDataInput1);
    MxStream::MxstDataOutput* output = mxStreamManager.GetResult(streamName, 0, 5*1000);
    sleep(2);
    mxStreamManager.StopStream(streamName);

    std::string result = FileUtils::ReadFileContent("./output/maskrcnnMs.output");
    std::string test = FileUtils::ReadFileContent("./output/maskrcnnMsActual.output");

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

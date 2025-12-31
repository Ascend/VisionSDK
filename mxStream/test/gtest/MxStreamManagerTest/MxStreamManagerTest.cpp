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
 * Description: stream manager test.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <cstring>
#include <iostream>
#include <thread>
#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxStream/StreamManager/MxsmStream.h"
#include "MxStream/Packet/Packet.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxBase/Utils/FileUtils.h"

using namespace MxStream;

namespace {
const unsigned int TIME_OUT = 500;
const int THREAD_SIZE = 5;
const int DATA_SIZE = 20;
// The input data may be large.
const int NEW_DATA_SIZE = 2000000;
constexpr uint32_t VPC_CHN_NUM = 8;
constexpr uint32_t JPEGE_CHN_NUM = 8;
constexpr uint32_t JPEGD_CHN_NUM = 8;
constexpr uint32_t PNGD_CHN_NUM = 8;
constexpr uint64_t UNIQUE_ID = 5;
constexpr int REDUCE_SIZE = 2;
const std::string PIPELINE_CONFIG_PATH = "./Sample.pipeline";
const int FAKE_DATA_TYPE = 2;
const float FAKE_COORDINATE_VAL = 52.1;
const float FAKE_CONFIDENCE_VAL = 22.2;
const int FAKE_TENSOR_DATE_SIZE = 22222;
const uint32_t FAKE_VIDEO_HEIGHT = 224;
const uint32_t FAKE_VIDEO_HEIGHT_ALIGNED = 448;
const uint64_t FAKE_FREE_FUNC = 22;
const uint64_t FAKE_MXVISIONDATA_PTR = 10000;
const uint64_t FAKE_MXVISIONDATA_DATASIZE = 10000;
std::shared_ptr<MxStream::MxStreamManager> g_mxStreamManagerPtr;

class MxStreamManagerTest : public testing::Test {
public:
    void SetUp() override
    {
        LogDebug << "SetUp()";
    }

    void TearDown() override
    {
        // clear mock
        GlobalMockObject::verify();
        LogDebug << "TearDown()";
    }

    static void SetUpTestSuite()
    {
        g_mxStreamManagerPtr = std::make_shared<MxStream::MxStreamManager>();
        if (g_mxStreamManagerPtr == nullptr) {
            throw std::runtime_error("Fail to malloc g_mxStreamManager.");
        }
        // init stream manager
        APP_ERROR ret = g_mxStreamManagerPtr->InitManager();
        if (ret != APP_ERR_OK) {
            throw std::runtime_error("Fail to init g_mxStreamManager.");
        }
        // create stream by pipeline config file
        ret = g_mxStreamManagerPtr->CreateMultipleStreamsFromFile(PIPELINE_CONFIG_PATH);
        if (ret != APP_ERR_OK) {
            throw std::runtime_error("Fail to create streams for g_mxStreamManager.");
        }
    }

    static void TearDownTestSuite()
    {
        g_mxStreamManagerPtr->DestroyAllStreams();
        g_mxStreamManagerPtr == nullptr;
    }
};


APP_ERROR InitAndSendData(MxStreamManager& mxStreamManager, std::string& streamName, int inPluginId, uint64_t uniqueId)
{
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataSize = NEW_DATA_SIZE;
    auto dataPtr = new (std::nothrow) uint32_t[NEW_DATA_SIZE];
    std::unique_ptr<uint32_t[]> data(dataPtr);
    mxstDataInput.dataPtr = data.get();
    return mxStreamManager.SendDataWithUniqueId(streamName, inPluginId, mxstDataInput, uniqueId);
}

TEST_F(MxStreamManagerTest, Test_SendData_Should_Return_Success_When_InitManager_From_AppGlobalCfgExtra)
{
    LogInfo << "********TestSendDataNormal********";
    std::string input = "hello world!!!";
    MxBase::AppGlobalCfgExtra appGlobalCfgExtra;
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager(appGlobalCfgExtra);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string pipeLine = "EasyStreamPipeline";
    ret = mxStreamManager.SendData(pipeLine, 0, mxstDataInput);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestSendDataNormal)
{
    LogInfo << "********TestSendDataNormal********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string pipeLine = "EasyStreamPipeline";
    ret = mxStreamManager.SendData(pipeLine, 0, mxstDataInput);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestSendDataErrorStreamName)
{
    LogInfo << "********TestSendDataErrorStreamName********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string pipeLine = "EasyStreamPipelineError";
    ret = mxStreamManager.SendData(pipeLine, 0, mxstDataInput);
    EXPECT_EQ(ret, APP_ERR_STREAM_NOT_EXIST);
}

TEST_F(MxStreamManagerTest, TestSendDataErrorInPluginId)
{
    LogInfo << "********TestSendDataErrorInPluginId********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string pipeLine = "EasyStreamPipeline";
    int idx = 0;
    ret = mxStreamManager.SendData(pipeLine, idx++, mxstDataInput);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.SendData(pipeLine, idx++, mxstDataInput);
    EXPECT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
    ret = mxStreamManager.SendData(pipeLine, idx++, mxstDataInput);
    EXPECT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
}

TEST_F(MxStreamManagerTest, TestSendDataEmptyDataBuffer)
{
    LogInfo << "********TestSendDataEmptyDataBuffer********";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
    std::string pipeLine = "EasyStreamPipeline";
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = nullptr;
    ret = mxStreamManager.SendData(pipeLine, 0, mxstDataInput);
    EXPECT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
}

TEST_F(MxStreamManagerTest, TestSendDataAbnormalDataBuffer)
{
    LogInfo << "********TestSendDataAbnormalDataBuffer********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size()-REDUCE_SIZE;

    std::string pipeLine = "EasyStreamPipeline";
    ret = mxStreamManager.SendData(pipeLine, 0, mxstDataInput);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestSendDataEmptyRoiBoxs)
{
    LogInfo << "********TestSendDataEmptyRoiBoxs********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string pipeLine = "EasyStreamPipeline";
    ret = mxStreamManager.SendData(pipeLine, 0, mxstDataInput);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestSendDataErrorRoiBoxs)
{
    LogInfo << "********TestSendDataErrorRoiBoxs********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();
    MxstServiceInfo serviceInfo;
    std::vector<CropRoiBox> roiBoxs;
    roiBoxs.push_back({0, 0, 100, 100});
    roiBoxs.push_back({110, 110, 100, 100});
    serviceInfo.roiBoxs = roiBoxs;
    mxstDataInput.serviceInfo = serviceInfo;
    std::string pipeLine = "EasyStreamPipeline";
    ret = mxStreamManager.SendData(pipeLine, 0, mxstDataInput);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestSendDataNormalMultiThreading)
{
    LogInfo << "********TestSendDataNormalMultiThreading********";
    std::string input = "hello world!!!";
    int threadCount = 2;
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    auto threadCallback = [&mxStreamManager, &mxstDataInput]() {
        std::string pipeLine = "EasyStreamPipeline";
        int sendCount = 5;
        for (int i = 0; i < sendCount; ++i) {
            auto ret = mxStreamManager.SendData(pipeLine, 0, mxstDataInput);
            EXPECT_EQ(ret, APP_ERR_OK);
        }
    };
    std::thread threads[threadCount];
    for (int i = 0; i < threadCount; ++i) {
        threads[i] = std::thread(threadCallback);
    }

    for (int i = 0; i < threadCount; ++i) {
        threads[i].join();
    }
}

TEST_F(MxStreamManagerTest, TestSendProtobufStreamName)
{
    LogInfo << "********case TestSendProtobufStreamName********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    auto objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiVision* mxpiVision = objectList->add_visionvec();
    MxTools::MxpiVisionData *visionData =  mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST);
    visionData->set_datastr(input);

    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = "appsrc0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    ret = mxStreamManager.SendProtobuf(streamName, inPluginId, dataBufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> strvec;
    strvec.push_back("appsrc0");
    std::vector<MxstProtobufOut> bufvec = mxStreamManager.GetProtobuf(streamName, inPluginId, strvec);
    ASSERT_GT(bufvec.size(), 0);
    EXPECT_EQ(bufvec[0].errorCode, 0);
    auto resultList = std::static_pointer_cast<MxTools::MxpiVisionList>(bufvec[0].messagePtr);
    EXPECT_EQ(resultList->visionvec(0).visiondata().datastr(), input);
}

TEST_F(MxStreamManagerTest, TestSendProtobufErrorStreamName)
{
    LogInfo << "********case TestSendProtobufErrorStreamName********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    auto objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiVision* mxpiVision = objectList->add_visionvec();
    MxTools::MxpiVisionData *visionData =  mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST);
    visionData->set_datastr(input);

    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = "appsrc0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    std::string streamName = "";
    int inPluginId = 0;
    ret = mxStreamManager.SendProtobuf(streamName, inPluginId, dataBufferVec);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestSendProtobufInPluginIdError)
{
    LogInfo << "********case TestSendProtobufInPluginIdError********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    auto objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiVision* mxpiVision = objectList->add_visionvec();
    MxTools::MxpiVisionData* visionData = mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST);
    visionData->set_datastr(input);

    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = "appsrc0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 5;
    ret = mxStreamManager.SendProtobuf(streamName, inPluginId, dataBufferVec);
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestSendProtobufInProtoVecError)
{
    LogInfo << "********case TestSendProtobufInProtoVecError********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    auto objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiVision* mxpiVision = objectList->add_visionvec();
    MxTools::MxpiVisionData *visionData =  mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST);
    visionData->set_datastr(input);

    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = "appsrc0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec = {};
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    ret = mxStreamManager.SendProtobuf(streamName, inPluginId, dataBufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, SendAndGetDataWithUniqueIdInPluginIdNotExists)
{
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string input = "SendAndGetDataWithUniqueIdInPluginIdNotExists";
    MxstDataInput dataBuffer;
    dataBuffer.dataPtr = (uint32_t*) input.c_str();
    dataBuffer.dataSize = input.size();

    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    uint64_t uniqueId;
    ret = mxStreamManager.SendDataWithUniqueId(streamName, inPluginId, dataBuffer, uniqueId);
    EXPECT_EQ(ret, APP_ERR_OK);

    int wrongInPluginId = 1;
    ret = mxStreamManager.SendDataWithUniqueId(streamName, wrongInPluginId, dataBuffer, uniqueId);
    EXPECT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);

    uint64_t uniqueIdWrong = 1;
    MxstDataOutput* uniqueIdWrongResult = mxStreamManager.GetResultWithUniqueId(streamName, uniqueIdWrong, TIME_OUT);
    EXPECT_EQ(uniqueIdWrongResult->errorCode, APP_ERR_STREAM_TIMEOUT);
}

TEST_F(MxStreamManagerTest, DestroyAllStreamsWhileSendDataWithUniqueId)
{
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string input = "DestroyAllStreamsWhileSendDataWithUniqueId";
    MxstDataInput dataBuffer;
    dataBuffer.dataPtr = (uint32_t*) input.c_str();
    dataBuffer.dataSize = input.size();

    ret = mxStreamManager.StopStream("NotFoundStream");
    EXPECT_EQ(ret, APP_ERR_STREAM_NOT_EXIST);

    ret = mxStreamManager.StopStream("EasyStreamPipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    uint64_t uniqueId;
    ret = mxStreamManager.SendDataWithUniqueId(streamName, inPluginId, dataBuffer, uniqueId);
    EXPECT_EQ(ret, APP_ERR_STREAM_NOT_EXIST);
}

TEST_F(MxStreamManagerTest, SendDataWithInvalidDataBuffer)
{
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput dataBuffer;
    dataBuffer.dataPtr = nullptr;
    dataBuffer.dataSize = 0;

    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    uint64_t uniqueId;
    ret = mxStreamManager.SendDataWithUniqueId(streamName, inPluginId, dataBuffer, uniqueId);
    EXPECT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
}

TEST_F(MxStreamManagerTest, SendDataWithInvalidStreamName)
{
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string input = "SendDataWithInvalidStreamName";
    MxstDataInput dataBuffer;
    dataBuffer.dataPtr = (uint32_t*) input.c_str();
    dataBuffer.dataSize = input.size();

    std::string invalidStreamName = "InvalidStreamName";
    int inPluginId = 0;
    uint64_t uniqueId;
    ret = mxStreamManager.SendDataWithUniqueId(invalidStreamName, inPluginId, dataBuffer, uniqueId);
    EXPECT_EQ(ret, APP_ERR_STREAM_NOT_EXIST);
}

TEST_F(MxStreamManagerTest, TestGetResultNormal) {
    LogInfo << "********case TestGetResultNormal********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "EasyStreamPipeline";
    ret = mxStreamManager.SendData(streamName, 0, mxstDataInput);
    MxstDataOutput *ptr = mxStreamManager.GetResult(streamName, 0);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->dataSize, input.size());
}

TEST_F(MxStreamManagerTest, TestGetResultErrorStreamName) {
    LogInfo << "********case TestGetResultErrorStreamName********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "EasyStreamPipeline";
    ret = mxStreamManager.SendData(streamName, 0, mxstDataInput);
    streamName = "";
    MxstDataOutput *ptr = mxStreamManager.GetResult(streamName, 0);
    EXPECT_NE(ptr, nullptr);
    EXPECT_NE(ptr->dataSize, input.size());
}

TEST_F(MxStreamManagerTest, TestGetResultErrorOutPluginId) {
    LogInfo << "********case TestGetResultErrorOutPluginId********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "EasyStreamPipeline";
    ret = mxStreamManager.SendData(streamName, 0, mxstDataInput);
    int outPluginId = 5;
    MxstDataOutput *ptr = mxStreamManager.GetResult(streamName, outPluginId);
    EXPECT_NE(ptr, nullptr);
    EXPECT_NE(ptr->dataSize, input.size());
}

TEST_F(MxStreamManagerTest, TestGetResultNormalMultiThreading) {
    LogInfo << "********case TestGetResultNormalMultiThreading********";
    std::string input = "hello world!!!";
    int threadCount = 2;
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    auto threadCallback = [&mxStreamManager, &mxstDataInput, &input]() {
        std::string streamName = "EasyStreamPipeline";
        int count = 5;
        for (int i = 0; i < count; i++) {
            auto ret = mxStreamManager.SendData(streamName, 0, mxstDataInput);
            EXPECT_EQ(ret, APP_ERR_OK);
            MxstDataOutput *ptr = mxStreamManager.GetResult(streamName, 0);
            LogInfo << i << " ptr->dataPtr: " << (char *)ptr->dataPtr;
            if ((size_t)ptr->dataSize != input.size()) {
                EXPECT_NE(strstr((const char *)ptr->dataPtr, "Internal error"), nullptr);
            }
        }
    };
    std::thread threads[threadCount];
    for (int i = 0; i < threadCount; i++) {
        threads[i] = std::thread(threadCallback);
    }
    for (int i = 0; i < threadCount; i++) {
        threads[i].join();
    }
}

TEST_F(MxStreamManagerTest, StreamTransModeTest)
{
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = "test";
    auto visionList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(visionList, nullptr);

    MxTools::MxpiVision* mxpiVision = visionList->add_visionvec();
    MxTools::MxpiVisionData* visionData = mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(visionList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);

    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    ret = mxStreamManager.SendProtobuf(streamName, inPluginId, dataBufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string input = "StreamTransModeTest";
    MxstDataInput inputBuffer;
    inputBuffer.dataPtr = (uint32_t*) input.c_str();
    inputBuffer.dataSize = input.size();
    uint64_t uniqueId;
    ret = mxStreamManager.SendDataWithUniqueId(streamName, inPluginId, inputBuffer, uniqueId);
    EXPECT_EQ(ret, APP_ERR_STREAM_TRANS_MODE_NOT_MATCHED);
}

TEST_F(MxStreamManagerTest, TestGetProtobufStreamName)
{
    LogInfo << "********case TestGetProtobufStreamName********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    auto objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiVision* mxpiVision = objectList->add_visionvec();
    MxTools::MxpiVisionData *visionData =  mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST);
    visionData->set_datastr(input);

    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = "appsrc0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    ret = mxStreamManager.SendProtobuf(streamName, inPluginId, dataBufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> strvec;
    strvec.push_back("appsrc0");
    std::vector<MxstProtobufOut> bufvec = mxStreamManager.GetProtobuf(streamName, inPluginId, strvec);
    EXPECT_EQ(bufvec[0].errorCode, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestGetProtobufWrongStreamName)
{
    LogInfo << "********case TestGetProtobufWrongStreamName********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    auto objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiVision* mxpiVision = objectList->add_visionvec();
    MxTools::MxpiVisionData *visionData =  mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST);
    visionData->set_datastr(input);

    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = "appsrc0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    ret = mxStreamManager.SendProtobuf(streamName, inPluginId, dataBufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> strvec;
    strvec.push_back("appsrc0");
    std::string errorStreamName = "ErrorEasyStreamPipeline";
    std::vector<MxstProtobufOut> bufvec = mxStreamManager.GetProtobuf(errorStreamName, inPluginId, strvec);
    EXPECT_EQ(bufvec[0].errorCode, APP_ERR_STREAM_NOT_EXIST);
}

TEST_F(MxStreamManagerTest, TestGetProtobufInPluginId)
{
    LogInfo << "********case TestGetProtobufInPluginId********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    auto objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiVision* mxpiVision = objectList->add_visionvec();
    MxTools::MxpiVisionData *visionData =  mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST);
    visionData->set_datastr(input);

    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = "appsrc0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    ret = mxStreamManager.SendProtobuf(streamName, inPluginId, dataBufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> strvec;
    strvec.push_back("appsrc0");
    std::vector<MxstProtobufOut> bufvec = mxStreamManager.GetProtobuf(streamName, inPluginId, strvec);
    EXPECT_EQ(bufvec[0].errorCode, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestSendProtobufProtoVec)
{
    LogInfo << "********case TestSendProtobufProtoVec********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    auto objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiVision* mxpiVision = objectList->add_visionvec();
    MxTools::MxpiVisionData *visionData =  mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST);
    visionData->set_datastr(input);

    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = "appsrc0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    ret = mxStreamManager.SendProtobuf(streamName, inPluginId, dataBufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> strvec;
    strvec.push_back("appsrc0");
    std::vector<MxstProtobufOut> bufvec = mxStreamManager.GetProtobuf(streamName, inPluginId, strvec);
    EXPECT_EQ(bufvec[0].errorCode, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestSendProtobufProtoVecEmpty)
{
    LogInfo << "********case TestSendProtobufProtoVecEmpty********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::shared_ptr<MxTools::MxpiVisionList> objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiVision* mxpiVision = objectList->add_visionvec();
    MxTools::MxpiVisionData *visionData =  mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST);
    visionData->set_datastr(input);

    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = "appsrc0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    ret = mxStreamManager.SendProtobuf(streamName, inPluginId, dataBufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> strvec;
    std::vector<MxstProtobufOut> bufvec = mxStreamManager.GetProtobuf(streamName, inPluginId, strvec);
    EXPECT_EQ(bufvec[0].errorCode, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxStreamManagerTest, TestGetProtobufKey)
{
    LogInfo << "********case TestGetProtobufKey********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::shared_ptr<MxTools::MxpiVisionList> objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiVision* mxpiVision = objectList->add_visionvec();
    MxTools::MxpiVisionData *visionData =  mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST);
    visionData->set_datastr(input);

    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = "appsrc0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    ret = mxStreamManager.SendProtobuf(streamName, inPluginId, dataBufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> strvec;
    strvec.push_back("appsrc0");
    std::vector<MxstProtobufOut> bufvec = mxStreamManager.GetProtobuf(streamName, inPluginId, strvec);
    EXPECT_EQ(bufvec[0].errorCode, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestGetProtobufWrongKey)
{
    LogInfo << "********case TestGetProtobufWrongKey********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::shared_ptr<MxTools::MxpiVisionList> objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiVision* mxpiVision = objectList->add_visionvec();
    MxTools::MxpiVisionData *visionData =  mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST);
    visionData->set_datastr(input);

    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = "appsrc0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    ret = mxStreamManager.SendProtobuf(streamName, inPluginId, dataBufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> strvec;
    strvec.push_back("appsrc1");
    std::vector<MxstProtobufOut> bufvec = mxStreamManager.GetProtobuf(streamName, inPluginId, strvec);
    EXPECT_EQ(bufvec.size(), 0);
}

TEST_F(MxStreamManagerTest, GetResultWithUniqueIdStreamNameNotExist)
{
    LogInfo << "********case GetResultWithUniqueIdStreamNameNotExist********";
    MxStreamManager mxStreamManager;
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    uint64_t uniqueId = 0;
    APP_ERROR ret = InitAndSendData(mxStreamManager, streamName, inPluginId, uniqueId);
    EXPECT_EQ(ret, APP_ERR_OK);

    streamName = "";
    MxstDataOutput* retOut = mxStreamManager.GetResultWithUniqueId(streamName, uniqueId, TIME_OUT);
    EXPECT_NE(retOut->errorCode, APP_ERR_OK);
    delete retOut;
    retOut = nullptr;
}

TEST_F(MxStreamManagerTest, GetResultWithUniqueIdUniqueIdError)
{
    LogInfo << "********case GetResultWithUniqueIdUniqueIdError********";
    MxStreamManager mxStreamManager;
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    uint64_t uniqueId = 0;
    APP_ERROR ret = InitAndSendData(mxStreamManager, streamName, inPluginId, uniqueId);
    EXPECT_EQ(ret, APP_ERR_OK);

    uniqueId = UNIQUE_ID;
    MxstDataOutput* retOut = mxStreamManager.GetResultWithUniqueId(streamName, uniqueId, TIME_OUT);
    EXPECT_NE(retOut->errorCode, APP_ERR_OK);
    delete retOut;
    retOut = nullptr;
}

TEST_F(MxStreamManagerTest, GetResultWithUniqueIdWhileDestroyAllStreams)
{
    LogInfo << "********case GetResultWithUniqueIdWhileDestroyAllStreams********";
    MxStreamManager mxStreamManager;
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    uint64_t uniqueId = 0;
    APP_ERROR ret = InitAndSendData(mxStreamManager, streamName, inPluginId, uniqueId);
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = mxStreamManager.StopStream(streamName);
    EXPECT_EQ(ret, APP_ERR_OK);
    MxstDataOutput* retOut = mxStreamManager.GetResultWithUniqueId(streamName, uniqueId, TIME_OUT);
    EXPECT_NE(retOut->errorCode, APP_ERR_OK);
    delete retOut;
    retOut = nullptr;
}

TEST_F(MxStreamManagerTest, GetResultWithUniqueIdMultiThread)
{
    LogInfo << "********case GetResultWithUniqueIdMultiThread********";
    MxStreamManager mxStreamManager;
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    uint64_t uniqueId = 0;
    APP_ERROR ret = InitAndSendData(mxStreamManager, streamName, inPluginId, uniqueId);
    EXPECT_EQ(ret, APP_ERR_OK);

    auto getResultWithUniqueId = [&mxStreamManager, &streamName, &uniqueId](int* result, unsigned int timeOut)
    {
        MxstDataOutput *retOut = mxStreamManager.GetResultWithUniqueId(streamName, uniqueId, timeOut);
        *result = retOut->errorCode;
        delete retOut;
        retOut = nullptr;
    };
    std::thread thread[THREAD_SIZE];
    int result[THREAD_SIZE] = {0};
    for (int i = 0; i < THREAD_SIZE; i++) {
        thread[i] = std::thread(getResultWithUniqueId, &result[i], TIME_OUT);
    }
    int resultCount = 0;
    for (int i = 0; i < THREAD_SIZE; i++) {
        thread[i].join();
        resultCount += result[i];
    }
    EXPECT_NE(resultCount, 0);
}

TEST_F(MxStreamManagerTest, GetResultWithUniqueIdNoWait)
{
    LogInfo << "********case GetResultWithUniqueIdNoWait********";
    MxStreamManager mxStreamManager;
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    uint64_t uniqueId = 0;
    APP_ERROR ret = InitAndSendData(mxStreamManager, streamName, inPluginId, uniqueId);
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataOutput* retOut = mxStreamManager.GetResultWithUniqueId(streamName, uniqueId, 0);
    EXPECT_NE(retOut->errorCode, APP_ERR_OK);
    delete retOut;
    retOut = nullptr;
}

TEST_F(MxStreamManagerTest, TestCreateMultipleStreamsStreamsConfig)
{
    LogInfo << "*******CreateMultipleStreams API condition: valid streamConfig*********";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    std::string streamsConfig = MxBase::FileUtils::ReadFileContent("EasyStream.pipeline");
    ret = mxStreamManager.CreateMultipleStreams(streamsConfig);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestCreateMultipleStreamsInvalidStreamsConfig)
{
    LogInfo << "*******CreateMultipleStreams API condition: invalid streamsConfig*********";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    std::string streamsConfig = MxBase::FileUtils::ReadFileContent("Invalid.pipeline");
    ret = mxStreamManager.CreateMultipleStreams(streamsConfig);
    EXPECT_EQ(ret, APP_ERR_STREAM_INVALID_CONFIG);
}

TEST_F(MxStreamManagerTest, TestCreateMultipleStreamsFromFileStreamsFilePath)
{
    LogInfo << "*******CreateMultipleStreamsFromFile API condition: valid streamsFilePath, valid content*********";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestCreateMultipleStreamsFromFileStreamsFilePathInvalidContent)
{
    LogInfo << "*******CreateMultipleStreamsFromFile API condition: valid streamsFilePath, invalid content*********";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("Invalid.pipeline");
    EXPECT_EQ(ret, APP_ERR_STREAM_INVALID_CONFIG);
}

TEST_F(MxStreamManagerTest, TestCreateMultipleStreamsFromFileStreamsFilePathInvalidStreamsFilePath)
{
    LogInfo << "*******CreateMultipleStreamsFromFile API condition: invalid streamsFilePath*********";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("NotExisted.pipeline");
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PATH);
}

TEST_F(MxStreamManagerTest, TestSendDataAndGetResult)
{
    LogInfo << "********TestSendDataAndGetResult case********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string pipeLine = "EasyStreamPipeline";
    mxStreamManager.SendData(pipeLine, 0, mxstDataInput);
    MxstDataOutput *ptr = mxStreamManager.GetResult(pipeLine, 0);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->dataSize, input.size());
}

void ThreadFunc()
{
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, InitManagerMultiThread)
{
    LogInfo << "********InitManagerPrintCore case********";
    std::thread threadObj1(ThreadFunc);
    std::thread threadObj2(ThreadFunc);
    std::thread threadObj3(ThreadFunc);
    threadObj1.join();
    threadObj2.join();
    threadObj3.join();
}

void RequireWrongMetadata(MxStreamManager &mxStreamManager, std::vector<MxStream::MxstMetadataInput> &metadataVec,
    const std::string &streamName, MxstBufferInput &bufferInput)
{
    APP_ERROR ret = mxStreamManager.SendData(streamName, "appsrc0", metadataVec, bufferInput);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> dataSourceVec;
    dataSourceVec.emplace_back("appsrc1");
    MxstBufferAndMetadataOutput bufferAndMetaOut = mxStreamManager.GetResult(streamName, "appsink0", dataSourceVec);
    EXPECT_EQ(bufferAndMetaOut.errorCode, APP_ERR_OK);
    EXPECT_EQ(bufferAndMetaOut.metadataVec[0].errorCode, APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST);

    ret = mxStreamManager.SendData(streamName, "appsrc0", metadataVec, bufferInput);
    EXPECT_EQ(ret, APP_ERR_OK);
    dataSourceVec.clear();
    dataSourceVec.emplace_back("appsrc0");
    bufferAndMetaOut = mxStreamManager.GetResult(streamName, "appsink1", dataSourceVec);
    EXPECT_EQ(bufferAndMetaOut.errorCode, APP_ERR_STREAM_ELEMENT_INVALID);

    ret = mxStreamManager.SendData(streamName, "appsrc0", metadataVec, bufferInput);
    EXPECT_EQ(ret, APP_ERR_OK);
    dataSourceVec.clear();
    dataSourceVec.emplace_back("appsrc0");
    dataSourceVec.emplace_back("appsrc1");
    bufferAndMetaOut = mxStreamManager.GetResult(streamName, "appsink0", dataSourceVec);
    EXPECT_EQ(bufferAndMetaOut.errorCode, APP_ERR_OK);
    EXPECT_EQ(bufferAndMetaOut.metadataVec[0].errorCode, APP_ERR_OK);
    EXPECT_EQ(bufferAndMetaOut.metadataVec[1].errorCode, APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST);

    LogInfo << "**********run RequireWrongMetadata OK************";
}

TEST_F(MxStreamManagerTest, TestSendDataAndGetResultWithBufferAndMetadata)
{
    LogInfo << "********case TestSendDataAndGetResult********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::shared_ptr<MxTools::MxpiVisionList> visionList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(visionList, nullptr);

    MxTools::MxpiVision* mxpiVision = visionList->add_visionvec();
    MxTools::MxpiVisionData *visionData =  mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST);
    visionData->set_datastr(input);

    MxStream::MxstMetadataInput metadataInput;
    metadataInput.dataSource = "appsrc0";
    metadataInput.messagePtr = std::static_pointer_cast<google::protobuf::Message>(visionList);
    std::vector<MxStream::MxstMetadataInput> metadataVec;
    metadataVec.push_back(metadataInput);
    std::string streamName = "EasyStreamPipeline";
    MxstBufferInput bufferInput;
    bufferInput.dataSize = DATA_SIZE;
    int data[DATA_SIZE] = {0};
    bufferInput.dataPtr = (uint32_t *)data;
    ret = mxStreamManager.SendData(streamName, "appsrc0", metadataVec, bufferInput);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> dataSourceVec;
    dataSourceVec.push_back("appsrc0");
    MxstBufferAndMetadataOutput bufferAndMetaOut = mxStreamManager.GetResult(streamName, "appsink0", dataSourceVec);
    EXPECT_EQ(bufferAndMetaOut.errorCode, APP_ERR_OK);
    auto visionListOut = std::static_pointer_cast<MxTools::MxpiVisionList>(bufferAndMetaOut.metadataVec[0].dataPtr);
    EXPECT_TRUE(visionList->visionvec(0).visiondata().datastr() == input);
    EXPECT_EQ(bufferAndMetaOut.errorCode, APP_ERR_OK);
    dataSourceVec.clear();
    auto output = mxStreamManager.GetResult(streamName, "appsink0", dataSourceVec);
    EXPECT_EQ(output.errorCode, APP_ERR_COMM_FAILURE);

    RequireWrongMetadata(mxStreamManager, metadataVec, streamName, bufferInput);
}

TEST_F(MxStreamManagerTest, SendDataWithPluginName)
{
    LogInfo << "********SendDataWithPluginName case********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();
    std::vector<MxstMetadataInput> metadataVec;
    MxstBufferInput bufferInput;

    std::string pipeLine = "EasyStreamPipeline";
    ret = mxStreamManager.SendData(pipeLine, "appsrcxxx", metadataVec, bufferInput);
    EXPECT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
    ret = mxStreamManager.SendData(pipeLine, "appsrcxxx", mxstDataInput);
    EXPECT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
    mxStreamManager.SendData(pipeLine, "appsrc0", mxstDataInput);
    MxstDataOutput *ptr = mxStreamManager.GetResult(pipeLine, 0);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->dataSize, input.size());
}

TEST_F(MxStreamManagerTest, TestSendProtobufWithPluginName)
{
    LogInfo << "********case TestSendProtobufWithPluginName********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::shared_ptr<MxTools::MxpiVisionList> objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiVisionList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiVision* mxpiVision = objectList->add_visionvec();
    MxTools::MxpiVisionData *visionData =  mxpiVision->mutable_visiondata();
    visionData->set_deviceid(0);
    visionData->set_memtype(MxTools::MXPI_MEMORY_HOST);
    visionData->set_datastr(input);

    MxStream::MxstProtobufIn dataBuffer;
    dataBuffer.key = "appsrc0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    std::vector<MxStream::MxstProtobufIn> dataBufferVec;
    dataBufferVec.push_back(dataBuffer);
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    ret = mxStreamManager.SendProtobuf(streamName, "appsrc0", dataBufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::vector<std::string> strvec;
    strvec.push_back("appsrc0");
    std::vector<MxstProtobufOut> bufvec = mxStreamManager.GetProtobuf(streamName, inPluginId, strvec);
    EXPECT_EQ(bufvec[0].errorCode, 0);
    EXPECT_NE(bufvec.size(), 0);
    ret = mxStreamManager.StopStream(streamName);
    EXPECT_EQ(ret, APP_ERR_OK);
    auto resultList = std::static_pointer_cast<MxTools::MxpiVisionList>(bufvec[0].messagePtr);
    EXPECT_EQ(resultList->visionvec(0).visiondata().datastr(), input);
}

TEST_F(MxStreamManagerTest, SendDataWithUniqueIdWithPluginName)
{
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string input = "SendAndGetDataWithUniqueIdInPluginIdNotExists";
    MxstDataInput dataBuffer;
    dataBuffer.dataPtr = (uint32_t*) input.c_str();
    dataBuffer.dataSize = input.size();

    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    uint64_t uniqueId;
    ret = mxStreamManager.SendDataWithUniqueId(streamName, "appsrcxxx", dataBuffer, uniqueId);
    EXPECT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
    ret = mxStreamManager.SendDataWithUniqueId(streamName, "appsrc0", dataBuffer, uniqueId);
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataOutput* uniqueIdResult = mxStreamManager.GetResultWithUniqueId(streamName, inPluginId, TIME_OUT);
    EXPECT_EQ(uniqueIdResult->errorCode, APP_ERR_OK);
    EXPECT_EQ(uniqueIdResult->dataSize, input.size());
}

TEST_F(MxStreamManagerTest, Test_StreamManager_Should_Return_Fail_When_Pipeline_Is_Empty_Or_Repeated)
{
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EmptyFile.pipeline");
    ASSERT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EmptyJson.pipeline");
    ASSERT_EQ(ret, APP_ERR_STREAM_INVALID_CONFIG);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EmptyStream.pipeline");
    ASSERT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("RepeatedStream.pipeline");
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("RepeatedStream.pipeline");
    ASSERT_EQ(ret, APP_ERR_STREAM_EXIST);
    std::vector<MxstProtobufIn> protoVec;
    std::vector<MxstMetadataInput> metadataVec;
    MxstDataInput dataBuffer;
    MxstBufferInput bufferInput;
    std::vector<std::string> dataSourceVec;
    uint64_t uniqueId = 0;
    ret = mxStreamManager.SendData("NotFound", -1, dataBuffer);
    ASSERT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = mxStreamManager.SendData("NotFound", "appsrc0", dataBuffer);
    ASSERT_EQ(ret, APP_ERR_STREAM_NOT_EXIST);
    ret = mxStreamManager.SendData("NotFound", "appsrc0", metadataVec, bufferInput);
    ASSERT_EQ(ret, APP_ERR_STREAM_NOT_EXIST);
    ret = mxStreamManager.SendDataWithUniqueId("NotFound", -1, dataBuffer, uniqueId);
    ASSERT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = mxStreamManager.SendDataWithUniqueId("NotFound", "appsrc0", dataBuffer, uniqueId);
    ASSERT_EQ(ret, APP_ERR_STREAM_NOT_EXIST);
    ret = mxStreamManager.SendProtobuf("Stream", -1, protoVec);
    ASSERT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = mxStreamManager.SendProtobuf("NotFound", "appsrc0", protoVec);
    ASSERT_EQ(ret, APP_ERR_STREAM_NOT_EXIST);
    auto output = mxStreamManager.GetResult("NotFound", "appsrc0", dataSourceVec);
    ASSERT_EQ(output.errorCode, APP_ERR_STREAM_NOT_EXIST);
    ret = mxStreamManager.SetElementProperty("NotFound", "appsrc0", "propName", "propValue");
    ASSERT_EQ(ret, APP_ERR_STREAM_NOT_EXIST);
}

TEST_F(MxStreamManagerTest, InvaildProperty) {
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("AppsrcPropertyDuration1.pipeline");
    ASSERT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("AppsrcPropertyDuration2.pipeline");
    ASSERT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("AppsrcPropertyDuration3.pipeline");
    ASSERT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("AppsrcPropertyFormat.pipeline");
    ASSERT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("AppsrcPropertyMinLatency.pipeline");
    ASSERT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
}

TEST_F(MxStreamManagerTest, SendDataWithUniqueIdMultiInput) {
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataSize = NEW_DATA_SIZE;
    auto dataPtr = new (std::nothrow) uint32_t[NEW_DATA_SIZE];
    std::unique_ptr<uint32_t[]> data(dataPtr);
    mxstDataInput.dataPtr = data.get();

    std::vector<MxstDataInput> dataBufferVec{mxstDataInput};
    std::vector<int> inPluginIdVec{0};
    uint64_t uniqueId = 0;

    std::string streamName = "EasyStreamPipeline";
    ret = mxStreamManager.SendMultiDataWithUniqueId(streamName, inPluginIdVec, dataBufferVec, uniqueId);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, TestGetResultSPNormal) {
    LogInfo << "********case TestGetResultSPNormal********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "EasyStreamPipeline";
    ret = mxStreamManager.SendData(streamName, 0, mxstDataInput);
    std::shared_ptr<MxstDataOutput> ptr = mxStreamManager.GetResultSP(streamName, 0);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->dataSize, input.size());
}

TEST_F(MxStreamManagerTest, TestGetResultWithUniqueIdSPNormal) {
    LogInfo << "********case TestGetResultWithUniqueIdSPNormal********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "EasyStreamPipeline";
    uint64_t uniqueId = 0;
    ret = mxStreamManager.SendDataWithUniqueId(streamName, 0, mxstDataInput, uniqueId);
    std::shared_ptr<MxstDataOutput> ptr = mxStreamManager.GetResultWithUniqueIdSP(streamName, uniqueId, TIME_OUT);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->dataSize, input.size());
}

TEST_F(MxStreamManagerTest, TestGetMultiResultWithUniqueIdSPNormal) {
    LogInfo << "********case GetMultiResultWithUniqueIdSPNormal********";
    std::string input = "hello world!!!";
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.InitManager();
    ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t *)input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "EasyStreamPipeline";
    uint64_t uniqueId = 0;
    int inPluginId = 0;
    std::vector<int> inPluginIdVec = {inPluginId};
    std::vector<MxStream::MxstDataInput> dataBufferVec = {mxstDataInput};
    ret = mxStreamManager.SendMultiDataWithUniqueId(streamName, inPluginIdVec, dataBufferVec, uniqueId);
    std::vector<std::shared_ptr<MxStream::MxstDataOutput>> outputVec =
        mxStreamManager.GetMultiResultWithUniqueIdSP(streamName, uniqueId, TIME_OUT);
    std::shared_ptr<MxStream::MxstDataOutput> ptr = outputVec[0];
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(ptr->dataSize, input.size());
}

TEST_F(MxStreamManagerTest, DestroyAllStreamsNormal)
{
    LogInfo << "********case DestroyAllStreamsNormal********";
    MxStreamManager mxStreamManager;
    std::string streamName = "EasyStreamPipeline";
    int inPluginId = 0;
    uint64_t uniqueId = 0;
    APP_ERROR ret = InitAndSendData(mxStreamManager, streamName, inPluginId, uniqueId);
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataOutput* retOut = mxStreamManager.GetResultWithUniqueId(streamName, 1, TIME_OUT);
    EXPECT_NE(retOut->errorCode, APP_ERR_OK);
    delete retOut;
    retOut = nullptr;
}

TEST_F(MxStreamManagerTest, Test_Create_Streams_Should_Return_Fail_When_Mock_File_invalid)
{
    MOCKER_CPP(MxBase::FileUtils::IsFileValid).stubs().will(returnValue(false));
    MxStreamManager mxStreamManager;
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxStreamManagerTest, Test_Get_MxImageMaskList_Packet_Should_Return_Failure_When_Send_Fake_Data)
{
    // read image file and build stream input
    MxStream::MxImageMaskList imageMaskList;
    MxStream::MxImageMask test;
    test.className.push_back("ddd");
    test.shape.push_back(1);
    test.dataType = FAKE_DATA_TYPE;
    test.dataStr = "testimageClassList";
    imageMaskList.imageMaskList.push_back(test);
    std::string streamName = "classification+detection";

    // send data into stream
    Packet<MxStream::MxImageMaskList> data(imageMaskList);
    APP_ERROR ret = g_mxStreamManagerPtr->SendPacket<MxStream::MxImageMaskList>(data, "classification+detection",
                                                                                "appsrc0");
    EXPECT_EQ(ret, APP_ERR_OK);

    // get stream output
    Packet<std::shared_ptr<MxImageMaskList>> output;
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "mxpi_dataserialize0");
    EXPECT_NE(ret, APP_ERR_OK);
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "appsink0");
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, Test_Get_MxClassList_Packet_Should_Return_Failure_When_Send_Fake_Data)
{
    // read image file and build stream input
    MxStream::MxClassList classList;
    MxStream::MxClass test;
    test.classId = 0;
    test.className = "ssss";
    test.confidence = 0;
    classList.classList.push_back(test);

    // send data into stream
    Packet<MxStream::MxClassList> data(classList);
    APP_ERROR ret = g_mxStreamManagerPtr->SendPacket<MxStream::MxClassList>(data, "classification+detection",
                                                                            "appsrc0");
    EXPECT_EQ(ret, APP_ERR_OK);
    // get stream output
    Packet<std::shared_ptr<MxClassList>> output;
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "mxpi_dataserialize0");
    EXPECT_NE(ret, APP_ERR_OK);
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "appsink0");
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, Test_Get_MxObjectList_Packet_Should_Return_Failure_When_Send_Fake_Data)
{
    // read image file and build stream input

    MxStream::MxClassList classList;
    MxStream::MxClass testClass;
    testClass.classId = 0;
    testClass.className = "ssss";
    testClass.confidence = 0;

    MxStream::MxObjectList objectList;
    MxStream::MxObject test;
    test.x0 = FAKE_COORDINATE_VAL;
    test.y0 = FAKE_COORDINATE_VAL;
    test.x1 = FAKE_COORDINATE_VAL;
    test.y1 = FAKE_COORDINATE_VAL;
    test.classList.push_back(testClass);
    objectList.objectList.push_back(test);

    // send data into stream
    Packet<MxStream::MxObjectList> data(objectList);
    APP_ERROR ret = g_mxStreamManagerPtr->SendPacket<MxStream::MxObjectList>(data, "classification+detection",
                                                                             "appsrc0");
    EXPECT_EQ(ret, APP_ERR_OK);
    // get stream output
    Packet<std::shared_ptr<MxObjectList>> output;
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "mxpi_dataserialize0");
    EXPECT_NE(ret, APP_ERR_OK);
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "appsink0");
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, Test_Get_MxPoseList_Packet_Should_Return_Failure_When_Send_Fake_Data)
{
    MxStream::MxKeyPoint testPoint;
    testPoint.x = FAKE_COORDINATE_VAL;
    testPoint.y = FAKE_COORDINATE_VAL;
    testPoint.name = 0;
    testPoint.score = FAKE_CONFIDENCE_VAL;

    MxStream::MxPoseList poseList;
    MxStream::MxPose test;
    test.keyPoints.push_back(testPoint);
    test.score = FAKE_CONFIDENCE_VAL;
    poseList.poseList.push_back(test);

    // send data into stream
    Packet<MxStream::MxPoseList> data(poseList);
    APP_ERROR ret = g_mxStreamManagerPtr->SendPacket<MxStream::MxPoseList>(data, "classification+detection", "appsrc0");
    EXPECT_EQ(ret, APP_ERR_OK);

    // get stream output
    Packet<std::shared_ptr<MxPoseList>> output;
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "mxpi_dataserialize0");
    EXPECT_NE(ret, APP_ERR_OK);
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "appsink0");
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, Test_Get_MxTensorPackageList_Packet_Should_Return_Failure_When_Send_Fake_Data)
{
    MxStream::MxTensor testTensor;
    testTensor.tensorDataPtr = 1;
    testTensor.tensorDataSize = FAKE_TENSOR_DATE_SIZE;
    testTensor.deviceId = 0;
    testTensor.freeFunc = FAKE_FREE_FUNC;
    testTensor.tensorShape.push_back(1);
    testTensor.dataStr = "test";
    testTensor.tensorDataType = 1;

    MxStream::MxTensorPackageList tensorPackageList;
    MxStream::MxTensorPackage test;
    test.tensors.push_back(testTensor);
    tensorPackageList.tensorPackageList.push_back(test);

    // send data into stream
    Packet<MxStream::MxTensorPackageList> data(tensorPackageList);
    APP_ERROR ret = g_mxStreamManagerPtr->SendPacket<MxStream::MxTensorPackageList>(data, "classification+detection",
                                                                                    "appsrc0");
    EXPECT_EQ(ret, APP_ERR_OK);

    // get stream output
    Packet<std::shared_ptr<MxTensorPackageList>> output;
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "mxpi_dataserialize0");
    EXPECT_NE(ret, APP_ERR_OK);
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "appsink0");
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, Test_Get_MxTextsInfoList_Packet_Should_Return_Failure_When_Send_Fake_Data)
{
    MxStream::MxTextsInfo textsInfo;
    textsInfo.text.push_back("test");
    MxStream::MxTextsInfoList testTextsInfoList;
    testTextsInfoList.textsInfoList.push_back(textsInfo);

    // send data into stream
    Packet<MxStream::MxTextsInfoList> data(testTextsInfoList);
    APP_ERROR ret = g_mxStreamManagerPtr->SendPacket<MxStream::MxTextsInfoList>(data, "classification+detection",
                                                                                "appsrc0");
    EXPECT_EQ(ret, APP_ERR_OK);
    // get stream output
    Packet<std::shared_ptr<MxTextsInfoList>> output;
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "mxpi_dataserialize0");
    EXPECT_NE(ret, APP_ERR_OK);
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "appsink0");
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, Test_Get_MxVisionList_Packet_Should_Return_Failure_When_Send_Fake_Data)
{
    MxStream::MxVisionInfo testVisionInfo;
    testVisionInfo.format = 1;
    testVisionInfo.width = FAKE_VIDEO_HEIGHT;
    testVisionInfo.height = FAKE_VIDEO_HEIGHT;
    testVisionInfo.widthAligned = FAKE_VIDEO_HEIGHT_ALIGNED;
    testVisionInfo.heightAligned = FAKE_VIDEO_HEIGHT_ALIGNED;
    testVisionInfo.resizeType = 1;
    testVisionInfo.keepAspectRatioScaling = 1;

    MxStream::MxVisionData testVisionData;
    testVisionData.dataPtr = FAKE_MXVISIONDATA_PTR;
    testVisionData.dataSize = FAKE_MXVISIONDATA_DATASIZE;
    testVisionData.deviceId = 1;

    MxStream::MxVision textVision;
    textVision.visionInfo = testVisionInfo;
    textVision.visionData = testVisionData;

    MxStream::MxVisionList testVisionList;
    testVisionList.visionList.push_back(textVision);

    // send data into stream
    Packet<MxStream::MxVisionList> data(testVisionList);
    APP_ERROR ret = g_mxStreamManagerPtr->SendPacket<MxStream::MxVisionList>(data, "classification+detection",
                                                                             "appsrc0");
    EXPECT_EQ(ret, APP_ERR_OK);
    // get stream output
    Packet<std::shared_ptr<MxVisionList>> output;
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "mxpi_dataserialize0");
    EXPECT_NE(ret, APP_ERR_OK);
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "appsink0");
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, Test_Get_MxTextObjectList_Packet_Should_Return_Failure_When_Send_Fake_Data)
{
    MxStream::MxTextObject textObject;
    textObject.x0 = FAKE_COORDINATE_VAL;
    textObject.y0 = FAKE_COORDINATE_VAL;
    textObject.x1 = FAKE_COORDINATE_VAL;
    textObject.y1 = FAKE_COORDINATE_VAL;
    textObject.x2 = FAKE_COORDINATE_VAL;
    textObject.y2 = FAKE_COORDINATE_VAL;
    textObject.x3 = FAKE_COORDINATE_VAL;
    textObject.y3 = FAKE_COORDINATE_VAL;
    textObject.confidence = FAKE_CONFIDENCE_VAL;
    textObject.text = "textObject";

    MxStream::MxTextObjectList testtTextObjectList;
    testtTextObjectList.textObjectList.push_back(textObject);
    // send data into stream
    Packet<MxStream::MxTextObjectList> data(testtTextObjectList);
    APP_ERROR ret = g_mxStreamManagerPtr->SendPacket<MxStream::MxTextObjectList>(data, "classification+detection",
                                                                                 "appsrc0");
    EXPECT_EQ(ret, APP_ERR_OK);
    // get stream output
    Packet<std::shared_ptr<MxTextObjectList>> output;
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "mxpi_dataserialize0");
    EXPECT_NE(ret, APP_ERR_OK);
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "appsink0");
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxStreamManagerTest, Test_Get_MxstBufferOutput_Packet_Return_Failure_When_Send_Fake_Data)
{
    // read image file and build stream input
    MxStream::MxstDataInput dataBuffer;
    dataBuffer.dataSize = FAKE_MXVISIONDATA_DATASIZE;
    dataBuffer.dataPtr = new (std::nothrow) uint32_t[dataBuffer.dataSize];

    // send data into stream
    Packet<MxStream::MxstDataInput> data;
    data.SetItem(dataBuffer);
    APP_ERROR ret = g_mxStreamManagerPtr->SendPacket<MxStream::MxstDataInput>(data, "classification+detection",
                                                                              "appsrc0");
    EXPECT_EQ(ret, APP_ERR_OK);
    // get stream output
    Packet<std::shared_ptr<MxstBufferOutput>> output;
    ret = g_mxStreamManagerPtr->GetPacket(output, "classification+detection", "mxpi_dataserialize0");
    EXPECT_NE(ret, APP_ERR_OK);
    delete[] dataBuffer.dataPtr;
}

TEST_F(MxStreamManagerTest, InvalidChar)
{
    LogInfo << "********case InvalidChar********";
    MxStreamManager mxStreamManager;
    std::string streamName = "EasyStreamPipeline";
    APP_ERROR ret = mxStreamManager.InitManager();
    mxStreamManager.CreateMultipleStreamsFromFile("EasyStream.pipeline");
    std::string invalidStreamName = "EasyStream\nPipeline";
    std::vector<MxstProtobufIn> protoVec;
    std::vector<MxstMetadataInput> metadataVec;
    MxstBufferInput bufferInput;
    MxstDataInput dataBuffer;
    std::vector<int> inPluginIdVec;
    std::vector<std::string> dataSourceVec;
    std::vector<MxstDataInput> dataBufferVec;
    int inPluginId = 0;
    uint64_t uniqueId = 0;
    ret = mxStreamManager.SendDataWithUniqueId(invalidStreamName, inPluginId, dataBuffer, uniqueId);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = mxStreamManager.SendDataWithUniqueId(invalidStreamName, invalidStreamName, dataBuffer, uniqueId);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = mxStreamManager.SendProtobuf(invalidStreamName, inPluginId, protoVec);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = mxStreamManager.SendProtobuf(invalidStreamName, invalidStreamName, protoVec);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = mxStreamManager.SendData(invalidStreamName, invalidStreamName, metadataVec, bufferInput);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = mxStreamManager.SendData(invalidStreamName, inPluginId, dataBuffer);
    EXPECT_NE(ret, APP_ERR_OK);
    ret = mxStreamManager.SendData(invalidStreamName, invalidStreamName, dataBuffer);
    EXPECT_NE(ret, APP_ERR_OK);
    mxStreamManager.GetResult(invalidStreamName, invalidStreamName, dataSourceVec, TIME_OUT);
    mxStreamManager.GetProtobuf(invalidStreamName, inPluginId, dataSourceVec);
    mxStreamManager.GetProtobuf(streamName, -1, dataSourceVec);
    mxStreamManager.GetResult(invalidStreamName, -1, TIME_OUT);
    mxStreamManager.GetResult(streamName, -1, TIME_OUT);
    ret = mxStreamManager.SendMultiDataWithUniqueId(invalidStreamName, inPluginIdVec, dataBufferVec, uniqueId);
    EXPECT_NE(ret, APP_ERR_OK);
    mxStreamManager.GetResultWithUniqueId(invalidStreamName, 1, TIME_OUT);
    mxStreamManager.GetMultiResultWithUniqueId(streamName, uniqueId, TIME_OUT);
    mxStreamManager.GetMultiResultWithUniqueId(invalidStreamName, uniqueId, TIME_OUT);
    ret = mxStreamManager.StopStream(invalidStreamName);
    EXPECT_NE(ret, APP_ERR_OK);
}
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


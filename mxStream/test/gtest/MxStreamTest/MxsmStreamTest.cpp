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
 * Description: mxStream test.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */
#include <iostream>
#include <gtest/gtest.h>
#include "MxBase/ErrorCode/ErrorCode.h"
#define private public
#define protected public
#include "MxStream/StreamManager/MxsmElement.h"
#include "MxStream/StreamManager/MxsmStream.h"
#undef private
#undef protected
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"

using namespace MxStream;

namespace {
constexpr float CROPROIBOX_X0 = 2.1;
constexpr float CROPROIBOX_Y0 = 2.2;
constexpr float CROPROIBOX_X1 = 2.3;
constexpr float CROPROIBOX_Y1 = 2.4;
constexpr float CONFIDENCE_VALUE = 0.1;

class MxsmStreamTest : public testing::Test {
public:
    void SetUp() override
    {
        std::cout << "SetUp()" << std::endl;
    }

    void TearDown() override
    {
        std::cout << "TearDown()" << std::endl;
    }
};

nlohmann::json GetJsonObject(const std::string &StreamsConfig)
{
    nlohmann::json streamsJson;
    try {
        streamsJson = nlohmann::json::parse(StreamsConfig);
        if (!streamsJson.is_object() || streamsJson.empty()) {
            std::cout << "CreateMultipleStreams: invalid param." << std::endl;
        }
    } catch (std::exception &ex) {
        std::cout << "The input is not JSON format." << std::endl;
    }

    return streamsJson;
}

TEST_F(MxsmStreamTest, Test_MxsmStream_Should_Return_Fail_When_Pipeline_Is_Invalid)
{
    MxsmStream mxsmStream;
    std::cout << "******case 1********" << std::endl;
    std::string streamsConfig;
    std::string streamName = "StreamName1";
    APP_ERROR ret = mxsmStream.CreateStream(streamName, GetJsonObject(streamsConfig));
    EXPECT_EQ(ret, APP_ERR_STREAM_EXIST);

    std::cout << "******case 2********" << std::endl;
    streamsConfig = R"({"filesink0":"..."})";
    streamName = "";
    ret = mxsmStream.CreateStream(streamName, GetJsonObject(streamsConfig));
    EXPECT_NE(ret, APP_ERR_OK);

    std::cout << "******case 3********" << std::endl;
    streamName = "StreamName5";
    streamsConfig = R"({"appsrc0":{"factory":"appsrc"},"appsink0":{"factory":"appsink"}})";
    ret = mxsmStream.CreateStream(streamName, GetJsonObject(streamsConfig));
    EXPECT_NE(ret, APP_ERR_OK);
    ret = mxsmStream.DestroyStream();
    EXPECT_EQ(ret, APP_ERR_OK);

    std::cout << "end to CreateStreamFake" << std::endl;
}

TEST_F(MxsmStreamTest, Test_MxsmStream_Should_Return_Success_When_Pipeline_Is_Valid)
{
    std::cout << "******CreateStreamReal case 1********" << std::endl;
    MxsmStream mxsmStream;
    const std::string filePath = "./test.pipecont";
    auto streamsConfig = MxBase::FileUtils::ReadFileContent(filePath);
    std::string streamName = "StreamName1";
    APP_ERROR ret = mxsmStream.CreateStream(streamName, GetJsonObject(streamsConfig));
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = mxsmStream.DestroyStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxsmStreamTest, Test_MxsmStream_Should_Return_Fail_When_Pipeline_Without_Factory)
{
    std::cout << "******CreateStreamWithNoFactoryPipeline case 1********" << std::endl;
    const std::string filePath = "./NoFactory.pipeline";
    auto streamsConfig = MxBase::FileUtils::ReadFileContent(filePath);
    std::string streamsName("appsrc");
    MxsmStream mxsmStream;
    APP_ERROR ret = mxsmStream.CreateStream(streamsName, GetJsonObject(streamsConfig));
    EXPECT_NE(ret, APP_ERR_OK);
}

TEST_F(MxsmStreamTest, Test_MxsmStream_Should_Return_Success_When_Pipeline_Is_Right)
{
    std::cout << "******CreateStreamWithRightPipeline case 1********" << std::endl;
    APP_ERROR ret = APP_ERR_OK;
    MxsmStream mxsmStream;
    const std::string filePath = "./test2.pipecont";
    auto streamsConfig = MxBase::FileUtils::ReadFileContent(filePath);
    std::string streamName = "StreamName1";
    nlohmann::json streamJsonValue = GetJsonObject(streamsConfig);
    if (!streamJsonValue.is_object() || streamJsonValue.empty()) {
        std::cout << "[InitStreams] Stream json is not in object format or empty." << std::endl;
        EXPECT_NE(ret, APP_ERR_COMM_FAILURE);
    } else {
        ret = mxsmStream.CreateStream(streamName, streamJsonValue);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
    ret = mxsmStream.DestroyStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxsmStreamTest, Test_MxsmStream_Should_Return_Fail_When_Pipeline_json_Format_Is_Invalid)
{
    std::cout << "******CreateStreamWithInvalidJsonFormatPipeline case 1********" << std::endl;
    APP_ERROR ret = APP_ERR_OK;
    MxsmStream mxsmStream;
    const std::string filePath = "./NoJsonFormat.pipeline";
    auto streamsConfig = MxBase::FileUtils::ReadFileContent(filePath);
    std::string streamName = "StreamName1";
    nlohmann::json streamJsonValue = GetJsonObject(streamsConfig);
    if (!streamJsonValue.is_object() || streamJsonValue.empty()) {
        std::cout << "[InitStreams] Stream json is not in object format or empty." << std::endl;
        EXPECT_NE(ret, APP_ERR_COMM_FAILURE);
    } else {
        ret = mxsmStream.CreateStream(streamName, streamJsonValue);
        EXPECT_NE(ret, APP_ERR_OK);
    }
}

TEST_F(MxsmStreamTest, Test_MxsmStream_Should_Return_Success_When_Pipeline_Contain_Tee)
{
    std::cout << "******CreateStreamlWithTee case AddTee********" << std::endl;
    APP_ERROR ret = APP_ERR_OK;
    MxsmStream mxsmStream;
    const std::string pipelinePath = "./AddTee.pipeline";
    auto streamsConfig = MxBase::FileUtils::ReadFileContent(pipelinePath);
    std::string streamName = "StreamName1";
    ret = mxsmStream.CreateStream(streamName, GetJsonObject(streamsConfig));
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = mxsmStream.DestroyStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxsmStreamTest, Test_MxsmStream_Should_Return_Success_When_Pipeline_Contain_Queue)
{
    std::cout << "******CreateStreamWithQueue case AddQueue********" << std::endl;
    APP_ERROR ret = APP_ERR_OK;
    MxsmStream mxsmStream;
    const std::string pipelinePath = "./AddQueue.pipeline";
    auto streamsConfig = MxBase::FileUtils::ReadFileContent(pipelinePath);
    std::string streamName = "StreamName1";
    nlohmann::json streamJsonValue = GetJsonObject(streamsConfig);
    if (!streamJsonValue.is_object() || streamJsonValue.empty()) {
        std::cout << "[InitStreams] Stream json is not in object format or empty." << std::endl;
        EXPECT_NE(ret, APP_ERR_COMM_FAILURE);
    } else {
        ret = mxsmStream.CreateStream(streamName, streamJsonValue);
        EXPECT_EQ(ret, APP_ERR_OK);
    }
    ret = mxsmStream.DestroyStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxsmStreamTest, Test_Send_Protobuf_And_Get_Result_Should_Return_Success_When_Pipeline_Is_Valid)
{
    std::cout << "******SendProtobufAndGetResult case 7*******" << std::endl;
    const std::string pipelinePath = "./AppsrcAndAppsink.pipeline";
    auto streamsConfig = MxBase::FileUtils::ReadFileContent(pipelinePath);
    std::string streamsName("appsrc");
    MxsmStream mxsmStream;
    APP_ERROR ret = mxsmStream.CreateStream(streamsName, GetJsonObject(streamsConfig));
    EXPECT_EQ(ret, APP_ERR_OK);

    std::shared_ptr<MxTools::MxpiClassList> objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiClassList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiClass* mxpiClass = objectList->add_classvec();
    mxpiClass->set_classid(0);
    mxpiClass->set_classname("people");
    mxpiClass->set_confidence(CONFIDENCE_VALUE);
    MxTools::MxpiMetaHeader* mxpiMetaHeader = mxpiClass->add_headervec();
    mxpiMetaHeader->set_memberid(0);
    mxpiMetaHeader->set_datasource("mxpi_rtspsrc0");
    std::vector<MxstProtobufIn> protoVec;
    MxstProtobufIn dataBuffer;
    dataBuffer.key = "mxpi_modelinfer0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    protoVec.push_back(dataBuffer);
    int inPluginId = 0;

    ret = mxsmStream.SendProtobuf(inPluginId, protoVec);
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataOutput* mxstDataOutput = mxsmStream.GetResult(inPluginId);
    EXPECT_NE(mxstDataOutput, nullptr);
    std::cout << "mxstDataOutput->dataSize= " << mxstDataOutput->dataSize << std::endl;
    ret = mxsmStream.DestroyStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxsmStreamTest, Test_Send_Protobuf_And_Get_Protobuf_Should_Return_Success_When_Pipeline_Is_Valid)
{
    std::cout << "******SendProtobufAndGetProtobuf case 8********" << std::endl;
    const std::string pipelinePath = "./AppsrcAndAppsink.pipeline";
    auto streamsConfig = MxBase::FileUtils::ReadFileContent(pipelinePath);
    std::string streamsName("appsrc");
    MxsmStream mxsmStream;
    APP_ERROR ret = mxsmStream.CreateStream(streamsName, GetJsonObject(streamsConfig));
    EXPECT_EQ(ret, APP_ERR_OK);

    std::shared_ptr<MxTools::MxpiClassList> objectList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiClassList>();
    EXPECT_NE(objectList, nullptr);

    MxTools::MxpiClass* mxpiClass = objectList->add_classvec();
    mxpiClass->set_classid(0);
    mxpiClass->set_classname("people");
    mxpiClass->set_confidence(CONFIDENCE_VALUE);
    MxTools::MxpiMetaHeader* mxpiMetaHeader = mxpiClass->add_headervec();
    mxpiMetaHeader->set_memberid(0);
    mxpiMetaHeader->set_datasource("mxpi_rtspsrc0");
    std::vector<MxstProtobufIn> protoVec;
    MxstProtobufIn dataBuffer;
    dataBuffer.key = "mxpi_modelinfer0";
    dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(objectList);
    protoVec.push_back(dataBuffer);
    int inPluginId = 0;
    ret = mxsmStream.SendProtobuf(inPluginId, protoVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::string key = "mxpi_modelinfer0";
    std::vector<std::string> keyVec;
    keyVec.clear();
    keyVec.push_back(key);
    std::vector<MxstProtobufOut> mxstOutputVec = mxsmStream.GetProtobuf(inPluginId, keyVec);
    for (size_t i = 0; i < mxstOutputVec.size(); i++) {
        MxstProtobufOut mxstOutput = mxstOutputVec[i];
        EXPECT_NE(mxstOutput.messagePtr, nullptr);
        std::cout << "mxstOutput messageName:" << mxstOutput.messageName << std::endl;
        std::shared_ptr<MxTools::MxpiClassList> result =
            std::static_pointer_cast<MxTools::MxpiClassList>(mxstOutput.messagePtr);
        std::cout << "mxstDataOutput messagePtr classId:" << result->classvec(0).classid() << std::endl;
        std::cout << "mxstDataOutput messagePtr className:" << result->classvec(0).classname() << std::endl;
        std::cout << "mxstDataOutput messagePtr confidence:" << result->classvec(0).confidence() << std::endl;
    }
    ret = mxsmStream.DestroyStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxsmStreamTest, Test_Send_Int_Data_And_Get_Result_Should_Return_Success_When_Pipeline_Is_Valid)
{
    std::cout << "******case SendIntDataAndGetResult********" << std::endl;
    const std::string pipelinePath = "./AppsrcAndAppsink.pipeline";
    auto streamsConfig = MxBase::FileUtils::ReadFileContent(pipelinePath);
    std::string streamsName("appsrc");
    MxsmStream mxsmStream;
    APP_ERROR ret = mxsmStream.CreateStream(streamsName, GetJsonObject(streamsConfig));
    EXPECT_EQ(ret, APP_ERR_OK);

    CropRoiBox cropRoiBox {};
    cropRoiBox.x0 = CROPROIBOX_X0;
    cropRoiBox.y0 = CROPROIBOX_Y0;
    cropRoiBox.x1 = CROPROIBOX_X1;
    cropRoiBox.y1 = CROPROIBOX_Y1;
    MxstServiceInfo mxstServiceInfo = MxstServiceInfo();
    mxstServiceInfo.roiBoxs.push_back(cropRoiBox);
    MxstDataInput dataBuffer;
    dataBuffer.serviceInfo = mxstServiceInfo;
    dataBuffer.dataSize = 1;
    uint32_t dataPtr[1] = {0};
    dataBuffer.dataPtr = dataPtr;
    int inPluginId = 0;
    ret = mxsmStream.SendData(inPluginId, dataBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataOutput* mxstDataOutput = mxsmStream.GetResult(inPluginId);
    EXPECT_NE(mxstDataOutput, nullptr);
    std::cout << "mxstDataOutput->dataSize= " << mxstDataOutput->dataSize << std::endl;
    ret = mxsmStream.DestroyStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxsmStreamTest, Test_Send_String_Data_And_Get_Result_Should_Return_Success_When_Pipeline_Is_Valid)
{
    std::cout << "******case SendDataAndReceive3********" << std::endl;
    const std::string pipelinePath = "./AppsrcAndAppsink.pipeline";
    auto streamsConfig = MxBase::FileUtils::ReadFileContent(pipelinePath);
    std::string streamsName("appsrc");
    MxsmStream mxsmStream;
    APP_ERROR ret = mxsmStream.CreateStream(streamsName, GetJsonObject(streamsConfig));
    EXPECT_EQ(ret, APP_ERR_OK);

    CropRoiBox cropRoiBox {};
    cropRoiBox.x0 = CROPROIBOX_X0;
    cropRoiBox.y0 = CROPROIBOX_Y0;
    cropRoiBox.x0 = CROPROIBOX_X1;
    cropRoiBox.y1 = CROPROIBOX_Y1;
    MxstServiceInfo mxstServiceInfo = MxstServiceInfo();
    mxstServiceInfo.roiBoxs.push_back(cropRoiBox);
    MxstDataInput dataBuffer;
    dataBuffer.serviceInfo = mxstServiceInfo;
    std::string input = "Hello World!!!";
    dataBuffer.dataPtr = (uint32_t*) input.c_str();
    dataBuffer.dataSize = input.size();
    int inPluginId = 0;
    ret = mxsmStream.SendData(inPluginId, dataBuffer);
    EXPECT_EQ(ret, APP_ERR_OK);
    std::cout << "start to GetResult" << std::endl;
    MxstDataOutput* mxstDataOutput = mxsmStream.GetResult(inPluginId);
    EXPECT_NE(mxstDataOutput, nullptr);
    std::cout << "mxstDataOutput->dataSize= " << mxstDataOutput->dataSize << std::endl;
    ret = mxsmStream.DestroyStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxsmStreamTest, Test_GetElementAndOrder_Should_Return_Fail_When_Use_Fake_Data)
{
    const std::string pipelinePath = "./AppsrcAndAppsink.pipeline";
    std::string fakeStreamName = "fakeStreamName";
    std::unique_ptr<MxsmElement> mxsmElementPtr = std::make_unique<MxsmElement>(fakeStreamName);
    mxsmElementPtr->metaDataNextLink_ = {"FakeMessage1", "FakeMessage2"};
    auto streamsConfig = MxBase::FileUtils::ReadFileContent(pipelinePath);
    std::string streamsName("appsrc");
    MxsmStream mxsmStream;
    APP_ERROR ret = mxsmStream.CreateStream(streamsName, GetJsonObject(streamsConfig));
    EXPECT_EQ(ret, APP_ERR_OK);
    mxsmStream.GetElementAndOrder(mxsmElementPtr);
    mxsmElementPtr->metaDataNextLink_ = {"FakeMessage1:FakeMessage2", "FakeMessage3"};
    mxsmStream.GetElementAndOrder(mxsmElementPtr);
    ret = mxsmStream.DestroyStream();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxsmStreamTest, Test_IsInOutElementNameCorrect_Should_Return_Fail_When_Use_Fake_Data)
{
    MxsmStream mxsmStream;
    std::string fakeElementName = "fakeElementName";
    auto ret = mxsmStream.IsInOutElementNameCorrect(fakeElementName, INPUT_OUTPUT_ELEMENT::INPUT_ELEMENT);
    EXPECT_EQ(ret, false);
}

TEST_F(MxsmStreamTest, Test_Element_API_Should_Return_Fail_When_Property_Is_Invalid)
{
    std::string streamName = "streamName";
    nlohmann::json elementObject = nlohmann::json::object();
    MxsmElement mxsmElement(streamName);
    bool needUnref = true;
    GValue value = { 0, };
    GParamSpec gParamSpec;

    auto gobjectClassPtr = mxsmElement.GetElementClass("InvalidFactory", needUnref);
    EXPECT_EQ(gobjectClassPtr, nullptr);

    APP_ERROR ret = mxsmElement.ValidateUlongProperty(nullptr, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
    ret = mxsmElement.ValidateUlongProperty(&gParamSpec, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);

    ret = mxsmElement.ValidateLongProperty(nullptr, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
    ret = mxsmElement.ValidateLongProperty(&gParamSpec, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);

    ret = mxsmElement.ValidateUintProperty(nullptr, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
    ret = mxsmElement.ValidateUintProperty(&gParamSpec, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);

    ret = mxsmElement.ValidateIntProperty(nullptr, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
    ret = mxsmElement.ValidateIntProperty(&gParamSpec, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);

    ret = mxsmElement.ValidateUint64Property(nullptr, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);

    ret = mxsmElement.ValidateInt64Property(nullptr, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
    ret = mxsmElement.ValidateInt64Property(&gParamSpec, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);

    ret = mxsmElement.ValidateFloatProperty(nullptr, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
    ret = mxsmElement.ValidateFloatProperty(&gParamSpec, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);

    ret = mxsmElement.ValidateDoubleProperty(nullptr, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
    ret = mxsmElement.ValidateDoubleProperty(&gParamSpec, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);

    ret = mxsmElement.ValidateDefaultProperty(nullptr, "", value);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);

    ret = mxsmElement.ValidateElementSingleProperty(nullptr, "", "");
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);

    ret = mxsmElement.ValidateElementObject(elementObject);
    EXPECT_EQ(ret, APP_ERR_ELEMENT_INVALID_FACTORY);

    ret = MxsmElement::PerformLink(nullptr, nullptr, 1, 1);
    EXPECT_EQ(ret, APP_ERR_STREAM_ELEMENT_INVALID);
}
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    gst_init(&argc, &argv);

    return RUN_ALL_TESTS();
}

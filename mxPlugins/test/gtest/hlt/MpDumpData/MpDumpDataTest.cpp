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
 * Description: MpDumpDataTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <gtest/gtest.h>
#include <google/protobuf/util/json_util.h>
#include <unistd.h>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxTools/Proto/MxpiDumpData.pb.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"

using namespace MxTools;
using namespace MxStream;
using namespace MxBase;

namespace {
unsigned int sleepTime = 300000; // unit: microsecond

class MpDumpDataTest : public testing::Test {
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

/**
 * 测试经过appsrc发送数据之后，dump出来的数据正确, 使用mxStreamManager SendData GetResult发送和获取结果
 * 1、发送文字："Hello World!!!"
 * 2、dump buffer内容
 * 3、GetResult获取结果
 * appsrc -> mxpi_dumpdata -> appsink
 */
TEST_F(MpDumpDataTest, TestHelloWorldBufferDump)
{
    LogInfo << "********case  TestHelloWorldBufferDump********";

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("appsink.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string input = "Hello World!!!";
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "DumpDataPipeline";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    MxstDataOutput* pDataOutput = mxStreamManager.GetResult(streamName, 0);
    std::string result((char*) pDataOutput->dataPtr, pDataOutput->dataSize);

    // test use protobuf to deSerialize. bytes类型定义的参数使用json序列化和反序列化时，会自动把二进制base64编码和解码
    MxTools::MxpiDumpData dumpData;
    auto status = google::protobuf::util::JsonStringToMessage(result, &dumpData);
    EXPECT_TRUE(status.ok());

    const std::string& bufferData = dumpData.buffer().bufferdata();
    EXPECT_EQ(input, bufferData);

    mxStreamManager.StopStream("DumpDataPipeline");
}

/**
 * 使用filesink.pipeline创建stream
 * 1、发送文字："Hello World!!!"
 * 2、dump buffer内容
 * 3、输出到文件filesink.out中
 * 4、读取filesink.out文件内容，校验结果
 * appsrc -> mxpi_dumpdata -> filesink
 */
TEST_F(MpDumpDataTest, TestHelloWorldBufferFileDump)
{
    LogInfo << "********case  testHelloWorldBufferFileDump********";

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("filesink.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string input = "Hello World!!!";
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "DumpDataPipeline";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(1);
    mxStreamManager.StopStream("DumpDataPipeline");

    FileUtils::ModifyFilePermission("filesink.output", MxBase::FILE_MODE);
    std::string result = FileUtils::ReadFileContent("filesink.output");
    LogInfo << result;
    MxTools::MxpiDumpData dumpData;
    auto status = google::protobuf::util::JsonStringToMessage(result, &dumpData);
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(input, dumpData.buffer().bufferdata());
}

/**
 * 验证当存在location属性时，透传上游插件到下游插件
 * 使用locationProp.pipeline创建stream
 * 1、发送文字："Hello World!!!"
 * 2、dump buffer内容
 * 3、输出到文件filesink.out中
 * 4、读取filesink_locationProp.output文件内容，校验结果
 * appsrc -> mxpi_dumpdata -> filesink
 */
TEST_F(MpDumpDataTest, TestLocationProp)
{
    LogInfo << "********case testLocationProp********";

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("locationProp.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string input = "Hello World!!!";
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "DumpDataPipeline";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    usleep(sleepTime);
    mxStreamManager.StopStream("DumpDataPipeline");
    FileUtils::ModifyFilePermission("filesink_locationProp.output", MxBase::FILE_MODE);
    std::string result = FileUtils::ReadFileContent("filesink_locationProp.output");
    EXPECT_EQ(input, result);
}

/**
 * 验证当存在location属性时，透传上游插件到下游插件
 * 使用locationProp.pipeline创建stream
 * 1、发送文字："Hello World!!!"
 * 2、dump buffer内容
 * 3、输出到文件filesink.out中
 * 4、读取filesink_locationProp.output文件内容，校验结果
 * appsrc -> mxpi_dumpdata -> filesink
 */
TEST_F(MpDumpDataTest, TestMultipleOutputFiles)
{
    LogInfo << "********case testMultipleOutputFiles********";

    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("multiOutputFiles.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string catImage = FileUtils::ReadFileContent("cat.jpg");
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) catImage.c_str();
    mxstDataInput.dataSize = catImage.size();

    std::string streamName = "decodePipeline";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    mxStreamManager.GetResult(streamName, 0);

    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    mxStreamManager.GetResult(streamName, 0);

    mxStreamManager.StopStream("DumpDataPipeline");

    std::string result0 = FileUtils::ReadFileContent("output/multiOutputFiles.output");
    std::string result1 = FileUtils::ReadFileContent("output/multiOutputFiles.output1");
    EXPECT_EQ(result0, result1);
}

/**
* 验证filterMetaDataKeys能正确生效
* filterMetaDataKeysProp.pipeline创建stream
* 1、发送图片cat.jpg
* 2、经过解码插件后dump
* 3、输出到文件filterMetaDataKeys.output中
* 4、读取filterMetaDataKeys.output文件内容，校验结果
* appsrc -> mxpi_imagedecoder -> mxpi_dumpdata -> filesink
*/
TEST_F(MpDumpDataTest, TestFilterKeysProp)
{
    LogInfo << "********case testFilterKeysProp********";

    std::string streamsConfig = FileUtils::ReadFileContent("filterMetaDataKeysProp.pipeline");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreams(streamsConfig);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string catImage = FileUtils::ReadFileContent("cat.jpg");
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) catImage.c_str();
    mxstDataInput.dataSize = catImage.size();

    std::string streamName = "decodePipeline";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    usleep(sleepTime);
    mxStreamManager.StopStream("DumpDataPipeline");
    FileUtils::ModifyFilePermission("filterMetaDataKeys.output", MxBase::FILE_MODE);
    std::string result = FileUtils::ReadFileContent("filterMetaDataKeys.output");
    MxTools::MxpiDumpData mxpiDumpData;
    auto status = google::protobuf::util::JsonStringToMessage(result, &mxpiDumpData);
    EXPECT_TRUE(status.ok());

    EXPECT_EQ(2, mxpiDumpData.metadata_size());

    std::vector<std::string> keys;
    for (auto& metaData : mxpiDumpData.metadata()) {
        LogInfo << "key = " << metaData.key();
        keys.push_back(metaData.key());

        if (metaData.key() == "mxpi_imagedecoder0") {
            MxpiVisionList mxpiVisionList;
            google::protobuf::util::JsonStringToMessage(metaData.content(), &mxpiVisionList);

            const auto& visionVec = mxpiVisionList.visionvec(0);
            EXPECT_EQ(visionVec.visiondata().memtype(), visionVec.visiondata().memtype());
        }
    }

    auto iter = std::find(keys.begin(), keys.end(), "ExternalObjects");
    EXPECT_TRUE(iter == keys.end());

    iter = std::find(keys.begin(), keys.end(), "ReservedFrameInfo");
    EXPECT_TRUE(iter == keys.end());

    iter = std::find(keys.begin(), keys.end(), "ReservedVisionList");
    EXPECT_TRUE(iter != keys.end());
}

/**
* 验证requiredMetaDataKeys能正确生效, location属性的输出文件可以带目录创建
* requiredMetaDataKeysProp.pipeline创建stream
* 1、发送图片cat.jpg
* 2、经过解码插件后dump
* 3、输出到文件requiredMetaDataKeys.output中
* 4、读取requiredMetaDataKeys.output文件内容，校验结果
* appsrc -> mxpi_imagedecoder -> mxpi_dumpdata -> filesink
*/
TEST_F(MpDumpDataTest, TestRequiredKeysProp)
{
    LogInfo << "********case testFilterKeysProp********";

    std::string streamsConfig = FileUtils::ReadFileContent("requiredMetaDataKeysProp.pipeline");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreams(streamsConfig);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string catImage = FileUtils::ReadFileContent("cat.jpg");
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) catImage.c_str();
    mxstDataInput.dataSize = catImage.size();

    std::string streamName = "decodePipeline";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    usleep(sleepTime);
    mxStreamManager.StopStream("DumpDataPipeline");

    std::string result = FileUtils::ReadFileContent("output/requiredMetaDataKeys.output");
    MxTools::MxpiDumpData mxpiDumpData;
    auto status = google::protobuf::util::JsonStringToMessage(result, &mxpiDumpData);
    EXPECT_TRUE(status.ok());
    EXPECT_EQ(3, mxpiDumpData.metadata_size());

    std::vector<std::string> keys;
    for (auto& metaData : mxpiDumpData.metadata()) {
        LogInfo << "key = " << metaData.key();
        keys.push_back(metaData.key());
    }

    auto iter = std::find(keys.begin(), keys.end(), "ExternalObjects");
    EXPECT_TRUE(iter != keys.end());

    iter = std::find(keys.begin(), keys.end(), "ReservedFrameInfo");
    EXPECT_TRUE(iter != keys.end());

    iter = std::find(keys.begin(), keys.end(), "ReservedVisionList");
    EXPECT_TRUE(iter != keys.end());

    iter = std::find(keys.begin(), keys.end(), "mxpi_imagedecoder0");
    EXPECT_TRUE(iter == keys.end());
}

/**
 * 测试导出TensorPackageList数据
 *
 * 使用SampleTensor.pipeline创建stream
 */
TEST_F(MpDumpDataTest, TestDumpTensorPackageList)
{
    LogInfo << "********case  testImageDecodeDump********";
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("SampleTensor.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string catImage = FileUtils::ReadFileContent("cat.jpg");
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) catImage.c_str();
    mxstDataInput.dataSize = catImage.size();

    std::string streamName = "classification+detection";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    usleep(sleepTime);

    // test use protobuf to deSerialize. bytes类型定义的参数使用json序列化和反序列化时，会自动把二进制base64编码和解码
    std::string result = FileUtils::ReadFileContent("tensorPackageList.output");
    MxTools::MxpiDumpData dumpData;
    auto status = google::protobuf::util::JsonStringToMessage(result, &dumpData);
    EXPECT_TRUE(status.ok());

    EXPECT_EQ(dumpData.metadata().size(), 1);
    EXPECT_EQ(dumpData.metadata(0).protodatatype(), "MxpiTensorPackageList");
    EXPECT_EQ(dumpData.metadata(0).key(), "mxpi_modelinfer0");

    MxTools::MxpiTensorPackageList tensorPackageList;
    auto tensorStatus = google::protobuf::util::JsonStringToMessage(dumpData.metadata(0).content(), &tensorPackageList);
    EXPECT_TRUE(tensorStatus.ok());

    int dataSize = tensorPackageList.tensorpackagevec(0).tensorvec(0).tensordatasize();
    EXPECT_EQ(dataSize, 172380);
    EXPECT_EQ(dataSize, tensorPackageList.tensorpackagevec(0).tensorvec(0).datastr().size());

    mxStreamManager.StopStream("DumpDataPipeline");
}

/**
 * 测试导出长流程所有插件的数据
 *
 * 使用SampleDump.pipeline创建stream
 */
TEST_F(MpDumpDataTest, TestDumpLongProcessPlugins)
{
    LogInfo << "********case  testImageDecodeDump********";
    std::string streamsConfig = FileUtils::ReadFileContent("SampleDump.pipeline");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreams(streamsConfig);
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string catImage = FileUtils::ReadFileContent("cat.jpg");
    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) catImage.c_str();
    mxstDataInput.dataSize = catImage.size();

    std::string streamName = "classification+detection";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);

    MxstDataOutput* pDataOutput = mxStreamManager.GetResult(streamName, 0);
    std::string result((char*) pDataOutput->dataPtr, pDataOutput->dataSize);
    LogInfo << result;
    auto index = result.find_first_of("Egyptian cat");
    EXPECT_NE(index, result.npos);

    mxStreamManager.StopStream("DumpDataPipeline");
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
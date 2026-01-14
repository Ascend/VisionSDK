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
 * Description: gtest unit cases.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <gtest/gtest.h>
#include <gst/gst.h>
#include "MxBase/Log/Log.h"
#include "MxStream/DataType/DataHelper.h"
#include "MxStream/Stream/SequentialStream.h"
#include "MxStream/Stream/FunctionalStream.h"

using namespace MxStream;

namespace {
class StreamTest : public testing::Test {
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

TEST_F(StreamTest, SequentialStream)
{
    std::map<std::string, std::string> props0 = {
        {"modelPath", "/home/simon/models/yolov3/yolov3_tf_bs1_fp16.om"},
        {"postProcessConfigPath", "/home/simon/models/yolov3/yolov3_tf_bs1_fp16.cfg"},
        {"labelPath", "/home/simon/models/yolov3/yolov3.names"},
    };
    std::map<std::string, std::string> props1 = {
        {"modelPath", "/home/simon/models/resnet50/resnet50_aipp_tf.om"},
        {"postProcessConfigPath", "/home/simon/models/resnet50/resnet50_aipp_tf.cfg"},
        {"labelPath", "/home/simon/models/resnet50/resnet50_clsidx_to_labels.names"},
    };
    std::map<std::string, std::string> props2 = {
        {"outputDataKeys", "mxpi_modelinfer0,mxpi_modelinfer1"}
    };

    SequentialStream stream("stream");
    stream.SetDeviceId("0");

    stream.Add(PluginNode("appsrc"));
    stream.Add(PluginNode("mxpi_imagedecoder"));
    stream.Add(PluginNode("mxpi_imageresize"));
    stream.Add(PluginNode("mxpi_modelinfer", props0));
    stream.Add(PluginNode("mxpi_imagecrop"));
    stream.Add(PluginNode("mxpi_imageresize"));
    stream.Add(PluginNode("mxpi_modelinfer", props1));
    stream.Add(PluginNode("mxpi_dataserialize", props2));
    stream.Add(PluginNode("appsink"));

    auto ret = stream.Build();
    ASSERT_EQ(ret, APP_ERR_OK);
    ret = stream.Stop();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(StreamTest, DISABLED_FunctionalStream)
{
    std::map<std::string, std::string> props0 = {
        {"resizeHeight", "512"},
        {"resizeWidth", "512"}
    };
    std::map<std::string, std::string> props1 = {
        {"dataSource", "appsrc1"}
    };
    auto appsrc0 = PluginNode("appsrc");
    auto appsrc1 = PluginNode("appsrc");
    auto parallel2serial0 = PluginNode("mxpi_parallel2serial", props1)(appsrc1);
    auto imagedecoder0 = PluginNode("mxpi_imagedecoder")(appsrc0);
    auto imageresize0 = PluginNode("mxpi_imageresize", props0)(imagedecoder0);
    auto opencvosd0 = PluginNode("mxpi_opencvosd")(imageresize0, parallel2serial0);
    auto imageencoder0 = PluginNode("mxpi_imageencoder")(opencvosd0);
    auto appsink0 = PluginNode("appsink")(imageencoder0);

    std::vector<PluginNode> inputs = {appsrc0, appsrc1};
    std::vector<PluginNode> outputs = {appsink0};
    FunctionalStream stream("stream", inputs, outputs);
    stream.SetDeviceId("0");
    auto ret = stream.Build();
    ASSERT_EQ(ret, APP_ERR_OK);
    MxstBufferInput bufferInput0 = MxStream::DataHelper::ReadImage("./test.jpg");
    std::vector<MxstMetadataInput> mxstMetadataInputVec0;
    ret = stream.SendData("appsrc2", mxstMetadataInputVec0, bufferInput0);
    ASSERT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
    auto output = stream.GetResult("appsink1", std::vector<std::string>(), 1);
    ASSERT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
}

TEST_F(StreamTest, SequentialStreamReadPipeline)
{
    SequentialStream stream1("Sample.pipeline");
    stream1.SetDeviceId("-1");
    SequentialStream stream2("IsInvalid.pipeline");
    SequentialStream stream3("Empty.pipeline");
    SequentialStream stream4("TwoStream.pipeline");
    auto streamJson = stream1.ToJson();
    ASSERT_GT(streamJson.size(), 1);
    auto ret = stream1.Build();
    ASSERT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = stream1.SetElementProperty("mxpi_tensorinfer0", "modelPath",
                                     "/home/simon/models/yolov3/yolov3_tf_bs1_fp16.om");
    ASSERT_EQ(ret, APP_ERR_STREAM_NOT_EXIST);
    ret = stream3.Build();
    ASSERT_EQ(ret, APP_ERR_STREAM_EXIST);
}

TEST_F(StreamTest, FunctionalStreamReadPipeline)
{
    FunctionalStream stream1("Sample.pipeline");
    auto appsrc0 = PluginNode("appsrc");
    auto appsrc1 = PluginNode("appsrc");
    auto parallel2serial0 = PluginNode("mxpi_parallel2serial")(appsrc1);
    auto imagedecoder0 = PluginNode("mxpi_imagedecoder")(appsrc0);
    auto imageresize0 = PluginNode("mxpi_imageresize")(imagedecoder0);
    auto opencvosd0 = PluginNode("mxpi_opencvosd")(imageresize0, parallel2serial0);
    auto imageencoder0 = PluginNode("mxpi_imageencoder")(opencvosd0);
    auto appsink0 = PluginNode("appsink")(imageencoder0);
    imageresize0(imageencoder0);
    imageresize0.Properties();
    std::vector<PluginNode> pnVec;
    pnVec.emplace_back(PluginNode("appsrc"));

    std::vector<PluginNode> inputs = {appsrc0, appsrc1};
    std::vector<PluginNode> outputs = {appsink0};
    FunctionalStream stream2("stream", inputs, outputs);
    auto data = DataHelper::ReadFile("Sample.pipeline");
    ASSERT_GT(data.size(), 0);
    auto ret = stream2.Build();
    ASSERT_EQ(ret, APP_ERR_STREAM_ELEMENT_EXIST);
}

TEST_F(StreamTest, SetNextNode)
{
    std::map<std::string, std::string> props0 = {
        {"resizeHeight", "512"},
        {"resizeWidth", "512"}
    };
    std::map<std::string, std::string> props1 = {
        {"dataSource", "appsrc1"}
    };
    auto appsrc0 = PluginNode("appsrc");
    auto appsrc1 = PluginNode("appsrc");
    auto parallel2serial0 = PluginNode("mxpi_parallel2serial", props1)(appsrc1);
    auto imagedecoder0 = PluginNode("mxpi_imagedecoder")(appsrc0);
    auto tee0 = PluginNode("tee")(imagedecoder0);
    auto imageresize0 = PluginNode("mxpi_imageresize", props0)(tee0);
    auto opencvosd0 = PluginNode("mxpi_opencvosd")(imageresize0, parallel2serial0, tee0);
    auto imageencoder0 = PluginNode("mxpi_imageencoder")(opencvosd0, tee0);
    auto appsink0 = PluginNode("appsink")(imageencoder0);

    std::vector<PluginNode> inputs = {appsrc0, appsrc1};
    std::vector<PluginNode> outputs = {appsink0};
    FunctionalStream stream("stream", inputs, outputs);
    auto ret = stream.Build();
    ASSERT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
}

TEST_F(StreamTest, Test_Stream_Should_Return_Fail_When_Appsink_Name_Is_Invalid)
{
    std::map<std::string, std::string> props = {};
    SequentialStream stream("stream");
    stream.SetDeviceId("0");
    stream.Add(PluginNode("appsrc"));
    stream.Add(PluginNode("mxpi_imagedecoder"));
    stream.Add(PluginNode("mxpi_imageresize"));
    stream.Add(PluginNode("mxpi_modelinfer"));
    stream.Add(PluginNode("mxpi_imagecrop"));
    stream.Add(PluginNode("mxpi_imageresize"));
    stream.Add(PluginNode("mxpi_modelinfer"));
    stream.Add(PluginNode("mxpi_dataserialize"));
    stream.Add(PluginNode("appsink", props, "xxxappsink"));

    auto ret = stream.Build();
    ASSERT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(StreamTest, FactoryNameError)
{
    std::map<std::string, std::string> props = {};
    SequentialStream stream("stream");
    stream.SetDeviceId("xxx");
    stream.Add(PluginNode("notfound"));
    stream.Add(PluginNode("mxpi_imagedecoder"));
    stream.Add(PluginNode("mxpi_imageresize"));
    stream.Add(PluginNode("mxpi_modelinfer"));
    stream.Add(PluginNode("mxpi_imagecrop"));
    stream.Add(PluginNode("mxpi_imageresize"));
    stream.Add(PluginNode("mxpi_modelinfer"));
    stream.Add(PluginNode("mxpi_dataserialize"));
    stream.Add(PluginNode("appsink"));

    auto ret = stream.Build();
    ASSERT_EQ(ret, APP_ERR_ELEMENT_INVALID_FACTORY);
}

TEST_F(StreamTest, FactoryNotExist)
{
    std::map<std::string, std::string> props = {};
    SequentialStream stream("FactoryNotExist.pipeline");
    auto ret = stream.Build();
    ASSERT_EQ(ret, APP_ERR_ELEMENT_INVALID_FACTORY);
}

TEST_F(StreamTest, PropertyNotExist)
{
    std::map<std::string, std::string> props = {};
    SequentialStream stream("PropertyNotExist.pipeline");
    auto ret = stream.Build();
    ASSERT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
}

TEST_F(StreamTest, PropertyNotObject)
{
    std::map<std::string, std::string> props = {};
    SequentialStream stream("PropertyNotObject.pipeline");
    auto ret = stream.Build();
    ASSERT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
}

TEST_F(StreamTest, PropertyNotString)
{
    std::map<std::string, std::string> props = {};
    SequentialStream stream("PropertyNotString.pipeline");
    auto ret = stream.Build();
    ASSERT_EQ(ret, APP_ERR_ELEMENT_INVALID_PROPERTIES);
}

TEST_F(StreamTest, RtspsrcPluginNode)
{
    std::map<std::string, std::string> props1 = {
        {"rtspUrl", "rtsp://127.0.0.1:10000/1_1080_25.264"},
        {"channelId", "0"},
    };
    std::map<std::string, std::string> props2 = {
        {"inputVideoFormat", "H264"},
        {"outputImageFormat", "YUV420SP_NV12"},
        {"vdecChannelId", "0"},
    };
    std::map<std::string, std::string> props3 = {
        {"dataSource", "mxpi_videodecoder0"},
        {"resizeHeight", "416"},
        {"resizeWidth", "416"},
    };
    std::map<std::string, std::string> props4 = {
        {"dataSource", "mxpi_imageresize0"},
        {"modelPath", "/home/simon/models/yolov3/yolov3_tf_bs1_fp16.om"},
    };
    std::map<std::string, std::string> props5 = {
        {"dataSource", "mxpi_tensorinfer0"},
        {"postProcessConfigPath", "/home/simon/models/yolov3/yolov3_tf_bs1_fp16.cfg"},
        {"labelPath", "/home/simon/models/yolov3/coco.names"},
        {"postProcessLibPath", "libyolov3postprocess.so"},
    };
    std::map<std::string, std::string> props6 = {
        {"outputDataKeys", "mxpi_objectpostprocessor0"},
    };
    SequentialStream stream("stream");
    stream.SetDeviceId("0");

    stream.Add(PluginNode("mxpi_rtspsrc", props1));
    stream.Add(PluginNode("mxpi_videodecoder", props2));
    stream.Add(PluginNode("mxpi_imageresize", props3));
    stream.Add(PluginNode("mxpi_tensorinfer", props4));
    stream.Add(PluginNode("mxpi_objectpostprocessor", props5));
    stream.Add(PluginNode("mxpi_dataserialize", props6));
    stream.Add(PluginNode("fakesink"));

    auto ret = stream.Build();
    ASSERT_EQ(ret, APP_ERR_OK);
}

TEST_F(StreamTest, DISABLED_PluginVector)
{
    std::map<std::string, std::string> props0 = {
            {"resizeHeight", "512"},
            {"resizeWidth", "512"}
    };
    std::map<std::string, std::string> props1 = {
            {"dataSource", "appsrc1"}
    };
    auto appsrc0 = PluginNode("appsrc");
    auto appsrc1 = PluginNode("appsrc");
    auto parallel2serial0 = PluginNode("mxpi_parallel2serial", props1)(appsrc1);
    auto imagedecoder0 = PluginNode("mxpi_imagedecoder")(appsrc0);
    auto imageresize0 = PluginNode("mxpi_imageresize", props0)(imagedecoder0);

    std::vector<PluginNode> nodeList = {imageresize0, parallel2serial0};

    auto opencvosd0 = PluginNode("mxpi_opencvosd")(nodeList);
    auto imageencoder0 = PluginNode("mxpi_imageencoder")(opencvosd0);
    auto appsink0 = PluginNode("appsink")(imageencoder0);

    std::vector<PluginNode> inputs = {appsrc0, appsrc1};
    std::vector<PluginNode> outputs = {appsink0};
    FunctionalStream stream("stream", inputs, outputs);
    stream.SetDeviceId("0");
    auto ret = stream.Build();
    ASSERT_EQ(ret, APP_ERR_OK);
    MxstBufferInput bufferInput0 = MxStream::DataHelper::ReadImage("./test.jpg");
    std::vector<MxstMetadataInput> mxstMetadataInputVec0;
    ret = stream.SendData("appsrc2", mxstMetadataInputVec0, bufferInput0);
    ASSERT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
    auto output = stream.GetResult("appsink1", std::vector<std::string>(), 1);
    ASSERT_EQ(ret, APP_ERR_STREAM_INVALID_LINK);
}

TEST_F(StreamTest, StopStreamTestNotBuild)
{
    SequentialStream stream("Sample.pipeline");
    auto ret = stream.Stop();
    ASSERT_EQ(ret, APP_ERR_STREAM_NOT_EXIST);
}

TEST_F(StreamTest, Test_Stream_Should_Init_And_Work_Unsuccessfully_When_Using_Fake_Data)
{
    std::string streamName = "FakeStreamName";
    Stream stream("Sample.pipeline", streamName);
    stream.Start();
    stream.Build();
    std::vector<int> inPluginIdVec;
    std::vector<MxstDataInput> dataInputVec;
    uint64_t uniqueId = 0;
    APP_ERROR ret = stream.SendMultiDataWithUniqueId(inPluginIdVec, dataInputVec, uniqueId);
    ASSERT_NE(ret, APP_ERR_OK);
    stream.GetMultiResultWithUniqueId(0, 0);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    gst_init(&argc, &argv);
    return RUN_ALL_TESTS();
}
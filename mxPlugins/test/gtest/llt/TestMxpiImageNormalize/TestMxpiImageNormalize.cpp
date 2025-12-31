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
 * Description: TestMxpiImageNormalize.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <map>
#include <vector>
#include <gtest/gtest.h>
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxpiCommon/DumpDataHelper.h"
#include "MxpiCommon/PluginTestHelper.h"
#include "MxPlugins/MxpiImageNormalize/MxpiImageNormalize.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
ExportPluginRegister(mxpi_imagenormalize)
void Init()
{
    gst_init(nullptr, nullptr);
    PluginRegister(mxpi_imagenormalize);
}

class TestMxpiImageNormalize : public testing::Test {
public:
    virtual void SetUp()
    {
        PluginTestHelper::gstBufferVec_.clear();
        std::cout << "SetUp()" << std::endl;
        if (APP_ERR_OK != MxBase::Log::Init()) {
            LogWarn << "failed to init log.";
        }
    }

    virtual void TearDown()
    {
        std::cout << "TearDown()" << std::endl;
    }
};

TEST_F(TestMxpiImageNormalize, ProcessDataSourceError)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "xxxxx"},
        {"alpha", "0,0,0"},
        {"beta", "1,1,1"},
        {"format", "auto"},
        {"dataType", "UINT8"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageNormalize>(
            "mxpi_imagenormalize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagenormalize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagenormalize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_MXPLUGINS_METADATA_IS_NULL);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageNormalize, ProcessMetadataProtoDataTypeError)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_modelinfer0"},
        {"alpha", "0,0,0"},
        {"beta", "1,1,1"},
        {"format", "auto"},
        {"dataType", "UINT8"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageNormalize>(
        "mxpi_imagenormalize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagenormalize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagecrop0-ascend.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageNormalize, ProcessMetadataMemTypeError)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"alpha", "0,0,0"},
        {"beta", "1,1,1"},
        {"format", "auto"},
        {"dataType", "UINT8"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageNormalize>(
        "mxpi_imagenormalize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagenormalize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagenormalize0-invalidmemtype.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageNormalize, ProcessMetadataFormatError)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"alpha", "0,0,0"},
        {"beta", "1,1,1"},
        {"format", "auto"},
        {"dataType", "UINT8"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageNormalize>(
        "mxpi_imagenormalize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagenormalize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagenormalize0-invalidformat.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageNormalize, ProcessMetadataDataSizeError)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"alpha", "0,0,0"},
        {"beta", "1,1,1"},
        {"format", "auto"},
        {"dataType", "UINT8"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageNormalize>(
        "mxpi_imagenormalize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "mxpi_imagenormalize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imagenormalize0-invaliddatasize.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageNormalize, TestMxpiImageNormalize_Should_Return_Success_When_Alpha1_Beta0)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imageresize0"},
        {"alpha", "1,1,1"},
        {"beta", "0,0,0"},
        {"dataType", "FLOAT32"},
        {"processType", "0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageNormalize>("mxpi_imagenormalize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "opencv_normalize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageNormalize, TestMxpiImageNormalize_Should_Return_Success_When_Alpha1_Beta1)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imageresize0"},
        {"alpha", "1,1,1"},
        {"beta", "1,1,1"},
        {"dataType", "FLOAT32"},
        {"processType", "0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageNormalize>("mxpi_imagenormalize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "opencv_normalize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageNormalize, InputBGR)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imagedecoder0"},
        {"dataType", "FLOAT32"},
        {"format", "BGR888"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageNormalize>("mxpi_imagenormalize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "opencv_normalize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/convert_fp32.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "./output/convert_fp32.json" }));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageNormalize, testNormalizationUint8)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imageresize0"},
        {"dataType", "UINT8"},
        {"processType", "0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageNormalize>("mxpi_imagenormalize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "opencv_normalize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "./output/normalizationUint8_output.json" }));
    ret = pluginPtr->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(TestMxpiImageNormalize, testNormalizationFloat32)
{
    std::map<std::string, std::string> properties = {
        {"dataSource", "mxpi_imageresize0"},
        {"dataType", "FLOAT32"},
        {"processType", "0"},
    };
    auto pluginPtr = PluginTestHelper::GetPluginInstance<MxpiImageNormalize>("mxpi_imagenormalize", properties);
    ASSERT_NE(pluginPtr, nullptr);

    pluginPtr->elementName_ = "opencv_normalize0";
    std::vector<MxpiBuffer*> bufferVec;
    PluginTestHelper::GetMxpiBufferFromFiles({"./input/imageresize0.json"}, bufferVec);
    auto ret = pluginPtr->Process(bufferVec);
    EXPECT_EQ(ret, APP_ERR_OK);
    EXPECT_TRUE(PluginTestHelper::CheckResult({ "./output/normalizationFloat32_output.json" }));
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

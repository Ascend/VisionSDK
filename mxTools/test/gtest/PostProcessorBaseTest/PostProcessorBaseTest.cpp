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
 * Description: PostProcessorBaseTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <gtest/gtest.h>
#include <gst/gst.h>
#include <string>
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/PostProcessPluginBases/MxImagePostProcessorBase.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

using namespace MxTools;
using namespace MxBase;

namespace {
class PostProcessorBaseTest : public testing::Test {
public:
    virtual void SetUp()
    {
        LogDebug << "SetUp()" << std::endl;
    }

    virtual void TearDown()
    {
        LogDebug << "TearDown()" << std::endl;
    }
};

TEST_F(PostProcessorBaseTest, SeperateUsage)
{
    MxImagePostProcessorBase plugin;
    std::map<std::string, std::shared_ptr<void>> configParamMap = {};
    APP_ERROR ret = plugin.Init(configParamMap);
    EXPECT_NE(ret, APP_ERR_OK);

    std::vector<MxTools::MxpiBuffer *> mxpiBuffer = {};
    ret = plugin.Process(mxpiBuffer);
    EXPECT_NE(ret, APP_ERR_OK);

    auto buffer = MxpiBufferManager::CreateHostBuffer(InputParam{});
    mxpiBuffer.push_back(buffer);
    ret = plugin.Process(mxpiBuffer);
    EXPECT_NE(ret, APP_ERR_OK);

    plugin.DefineInputPorts();
    plugin.DefineOutputPorts();
    plugin.DefineProperties();
}

TEST_F(PostProcessorBaseTest, Test_Init_Should_Return_Success_When_Legal_Config)
{
    MxImagePostProcessorBase plugin;
    std::map<std::string, std::shared_ptr<void>> configParamMap = {};
    configParamMap["dataSourceRoiBoxes"] = std::make_shared<std::string>("1,2,3,4");
    configParamMap["dataSourceResize"] = std::make_shared<std::string>("240,240");
    configParamMap["dataSourceImage"] = std::make_shared<std::string>("test.jpg");
    configParamMap["dataSource"] = std::make_shared<std::string>("mxpi_tensorinfer0");
    configParamMap["funcLanguage"] = std::make_shared<std::string>("python");
    configParamMap["postProcessConfigPath"] = std::make_shared<std::string>("yolov3_tf_bs1_fp16.cfg");
    configParamMap["labelPath"] = std::make_shared<std::string>("yolov3.names");
    configParamMap["postProcessLibPath"] = std::make_shared<std::string>("../../../python");
    configParamMap["className"] = std::make_shared<std::string>("Yolov3PostProcess");
    configParamMap["pythonModule"] = std::make_shared<std::string>("postprocess.post");
    configParamMap["postProcessConfigContent"] = std::make_shared<std::string>("postProcessConfigContent");
    plugin.dataSourceKeys_ = {"mxpi_tensorinfer0"};
    APP_ERROR ret = plugin.Init(configParamMap);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(PostProcessorBaseTest, Test_Process_Should_Return_Failed_When_DataSource_Has_No_Data)
{
    MxImagePostProcessorBase plugin;
    std::map<std::string, std::shared_ptr<void>> configParamMap = {};
    configParamMap["dataSourceRoiBoxes"] = std::make_shared<std::string>("1,2,3,4");
    configParamMap["dataSourceResize"] = std::make_shared<std::string>("240,240");
    configParamMap["dataSourceImage"] = std::make_shared<std::string>("test.jpg");
    configParamMap["dataSource"] = std::make_shared<std::string>("mxpi_tensorinfer0");
    configParamMap["funcLanguage"] = std::make_shared<std::string>("python");
    configParamMap["postProcessConfigPath"] = std::make_shared<std::string>("yolov3_tf_bs1_fp16.cfg");
    configParamMap["labelPath"] = std::make_shared<std::string>("yolov3.names");
    configParamMap["postProcessLibPath"] = std::make_shared<std::string>("../../../python");
    configParamMap["className"] = std::make_shared<std::string>("Yolov3PostProcess");
    configParamMap["pythonModule"] = std::make_shared<std::string>("postprocess.post");
    configParamMap["postProcessConfigContent"] = std::make_shared<std::string>("postProcessConfigContent");
    plugin.dataSourceKeys_ = {"mxpi_tensorinfer0"};
    APP_ERROR ret = plugin.Init(configParamMap);

    InputParam inputParam;
    inputParam.dataSize = 0;
    auto mxpiBuffer = MxpiBufferManager::CreateHostBuffer(inputParam);

    std::vector<MxTools::MxpiBuffer *> mxpiBuffers = {mxpiBuffer};
    ret = plugin.Process(mxpiBuffers);
    EXPECT_EQ(ret, APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

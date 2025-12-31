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
 * Description: DvppImageDecoderTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <gtest/gtest.h>

#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"

#include "MxPlugins/MxpiImageDecoder/MxpiImageDecoder.h"
#include "DvppImageDecoder_test.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

/**
 * MxpiImageDecoder's DT TestCase
 */

// save the buffer pointer, for next stage (to check)
static GstBuffer *g_GstBuffer;
GstFlowReturn MyChain(GstPad *pad, GstObject *parent, GstBuffer *buffer)
{
    g_GstBuffer = buffer;
    return GST_FLOW_OK;
}

// GIVEN("^there has \"(.*)\" plugin$")
void PrepareEnv()
{
    gst_init(NULL, NULL);

    // manual register, this will call:
    //  plugin::DefineProperties() & DefineInputPorts() & DefineOutputPorts()
    gst_plugin_mxpi_imagedecoder_register();
}

// WHEN("^I put \"(.*)\" to \"(.*)\" sink$")
void FillInput(MxpiBuffer* &buffer)
{
    std::string jsonString = FileUtils::ReadFileContent("./input.output");
    char *path = NULL;
    path = get_current_dir_name();
    std::cout<<"path:"<<path<<std::endl;
    InputParam inputParam;
    inputParam.dataSize = jsonString.size();
    inputParam.ptrData = (void*) jsonString.c_str();
    MxpiBuffer *resultBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);

    buffer = MxpiBufferDump::DoLoad(*resultBuffer);
}

// THEN("^I got \"(.*)\" at \"(.*)\" src$")
void CheckResult()
{
    MxpiBuffer pResult;
    pResult.buffer = g_GstBuffer;

    // demo: save output (for extern check)
    auto filterMetaDataKeys = MxBase::StringUtils::Split("", ',');
    auto requiredMetaDataKeys = MxBase::StringUtils::Split("dvppimagedecoder0", ',');
    std::string jsonString = MxpiBufferDump::DoDump(pResult, filterMetaDataKeys, requiredMetaDataKeys);
    FileUtils::WriteFileContent("./output.json", jsonString);

    // demo: ErrorInfo check
    MxpiMetadataManager mxpiMetadataManager(pResult);
    auto errorInfoPtr = mxpiMetadataManager.GetErrorInfo();
    if (errorInfoPtr != nullptr) {
        std::string result;
        auto errorInfo = *(std::static_pointer_cast<std::map<std::string, MxpiErrorInfo>>(errorInfoPtr));
        for (auto it = errorInfo.begin(); it != errorInfo.end(); it++) {
            result += (it->second).errorInfo + "\n";
        }
    }

    // do more check, and assign the test be passed or failed

    ASSERT_TRUE(TRUE);
}


TEST(DvppImageDecoder, testMxpiImageCorp)
{
    PrepareEnv();

    GstElement *decoder = gst_element_factory_make("mxpi_imagedecoder", NULL);
    gst_element_set_state(decoder, GST_STATE_PLAYING); // this will call plugin::Init()
    MxpiImageDecoder *pD = (MxpiImageDecoder *)(GST_MXBASE(decoder)->pluginInstance);
    ASSERT_TRUE(pD != NULL);

    GstElement *fakesink = gst_element_factory_make("fakesink", NULL);
    gst_element_set_state(fakesink, GST_STATE_PLAYING);
    GstPad * pCatch = gst_element_get_static_pad(fakesink, "sink");
    gst_pad_link(((MxGstBase*)decoder)->srcPadVec[0], pCatch);
    gst_pad_set_chain_function_full(pCatch, MyChain, NULL, NULL);

    MxpiBuffer *resultBuffer = NULL;
    FillInput(resultBuffer);
    ASSERT_TRUE(resultBuffer != NULL);

    std::vector<MxpiBuffer *> vMxpiBuffer;
    vMxpiBuffer.push_back(resultBuffer);
    pD->Process(vMxpiBuffer);

    CheckResult();

    // this will call plugin::DeInit()
    gst_element_set_state(decoder, GST_STATE_NULL);
}

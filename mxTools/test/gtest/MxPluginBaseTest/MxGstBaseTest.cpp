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
 * Description: MxGstBaseTest.
 * Author: Mind SDK
 * Create: 2025
 * History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <gst/gst.h>
#include <string>

#include "MxTools/PluginToolkit/base/MxGstBase.h"
#include "MxTools/PluginToolkit/base/MxPluginBase.h"
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/base/MxGstBase.h"
#include "MxPluginBaseDptr.hpp"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

using namespace MxTools;
using namespace MxBase;
namespace MxTools {
extern void InitProperty(const MxGstBase *filter, const GParamSpec *param);
extern void MxGstBaseInit(MxGstBase *filter, MxGstBaseClass *klass);
extern GstPad *MxGstBaseRequestNewPad(
    GstElement *element, GstPadTemplate *templ, const gchar *nameTempl, const GstCaps *caps);
extern void MxGstBaseReleasePad(GstElement *element, GstPad *pad);
extern void MxGstBaseFinalize(GObject *object);
extern void ClearFilterWhenReadyToNull(MxGstBase *filter);
extern gboolean MxGstBaseDefaultEvent(GstPad *pad, GstObject *parent, GstEvent *event);
extern int GetPadIndex(GstPad *pad, std::vector<GstPad *> padVector);
extern gboolean MxGstBaseDefaultQuery(GstPad *pad, GstObject *parent, GstQuery *query);
extern gboolean HandleSrcPadCustomQuery(GstPad *pad, MxGstBase *filter, const std::string &elementName,
    GstStructure *queryStructure, const gchar *structName);
extern bool HandleSinkPadCustomQuery(GstPad *pad, MxGstBase *filter, const std::string &elementName,
    GstStructure *queryStructure, const gchar *structName);
extern gboolean HandleCustomQuery(GstPad *pad, GstObject *parent, GstQuery *query);
extern gboolean HandleCustomQuery(GstPad *pad, GstObject *parent, GstQuery *query);
extern gboolean MxGstBasePadQuery(GstPad *pad, GstObject *parent, GstQuery *query);
extern GstFlowReturn SyncBufferProcess(MxGstBase &filter, MxpiBuffer &mxpiBuffer, int index);
extern bool JudgeBufSize(const std::vector<MxpiBuffer *> &input, int index);
extern GstFlowReturn BufferProcess(MxGstBase *filter, GstBuffer *gstBuffer, int index);
extern GstFlowReturn MxGstBaseChain(GstPad *pad, GstObject *parent, GstBuffer *gstBuffer);
extern APP_ERROR SetDevice(const int &deviceId);
extern gboolean SetDynamicImageSize(MxGstBase *filter);
}  // namespace MxTools

namespace {
constexpr int TWICE = 2;
constexpr int THREE_TIMES = 3;
constexpr int INIT_DEVICEID = -1;
constexpr int INIT_DATASIZE = 1;
MxTools::InputParam g_bufferParam = {"", INIT_DEVICEID, INIT_DATASIZE};
class MxGstBaseTest : public testing::Test {
public:
    virtual void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

class MxPluginBaseDerived : public MxPluginBase {
public:
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
    {
        return APP_ERR_OK;
    }
    APP_ERROR DeInit()
    {
        return APP_ERR_OK;
    }
    APP_ERROR Process(std::vector<MxpiBuffer *> &mxpiBuffer)
    {
        return APP_ERR_OK;
    }
    APP_ERROR RunProcess(std::vector<MxpiBuffer *> &mxpiBuffer)
    {
        return APP_ERR_OK;
    }
};

MxPluginBase *StubCreatePluginInstance()
{
    return nullptr;
}

void StubLock(GMutex *mutex)
{}
void StubUnlock(GMutex *mutex)
{}
void StubClearFilterWhenReadyToNull(MxGstBase *filter)
{}

TEST_F(MxGstBaseTest, Test_MxGstBase_MxGstBaseInit_Should_Fail_When_Input_Nullptr)
{
    APP_ERROR ret = APP_ERR_OK;
    MxGstBase base;
    MxGstBaseClass param;
    try {
        MxGstBaseInit(nullptr, &param);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);

    ret = APP_ERR_OK;
    try {
        MxGstBaseInit(&base, nullptr);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_MxGstBaseInit_Should_Fail_When_CreatePluginInstance_Fail)
{
    APP_ERROR ret = APP_ERR_OK;
    MxGstBase base;
    MxGstBaseClass klass;
    klass.CreatePluginInstance = StubCreatePluginInstance;
    try {
        MxGstBaseInit(&base, &klass);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_InitProperty_Should_Sucess_When_Different_Type)
{
    MxGstBase base;
    base.configParam = std::make_unique<std::map<std::string, std::shared_ptr<void>>>();
    GParamSpec *pspec = g_param_spec_long("long", "long", "long", 0, 1, 0, G_PARAM_READWRITE);
    InitProperty(&base, pspec);
    std::shared_ptr<void> ret = (*base.configParam)[pspec->name];
    EXPECT_NE(ret, nullptr);
    g_param_spec_unref(pspec);

    pspec = g_param_spec_ulong("ulong", "ulong", "ulong", 0, 1, 0, G_PARAM_READWRITE);
    InitProperty(&base, pspec);
    ret = (*base.configParam)[pspec->name];
    EXPECT_NE(ret, nullptr);
    g_param_spec_unref(pspec);

    pspec = g_param_spec_float("float", "float", "float", 0.0, 1.0, 0, G_PARAM_READWRITE);
    InitProperty(&base, pspec);
    ret = (*base.configParam)[pspec->name];
    EXPECT_NE(ret, nullptr);
    g_param_spec_unref(pspec);

    pspec = g_param_spec_double("double", "double", "double", 0.0, 1.0, 0, G_PARAM_READWRITE);
    InitProperty(&base, pspec);
    ret = (*base.configParam)[pspec->name];
    EXPECT_NE(ret, nullptr);
    g_param_spec_unref(pspec);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_MxGstBaseRequestNewPad_Should_Sucess_When_Pad_Is_Src)
{
    MOCKER_CPP(&g_mutex_lock).stubs().will(invoke(StubLock));
    MOCKER_CPP(&g_mutex_unlock).stubs().will(invoke(StubUnlock));
    MxGstBase base;
    GstPadTemplate *templ = gst_pad_template_new("src", GST_PAD_SRC, GST_PAD_ALWAYS, gst_caps_new_any());
    GstCaps *caps = gst_caps_new_any();
    gchar *nameTempl = "test";
    GstPad *ret = MxGstBaseRequestNewPad(GST_ELEMENT(&base), templ, nameTempl, caps);
    EXPECT_NE(ret, nullptr);
    gst_caps_unref(caps);
    gst_object_unref(ret);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_MxGstBaseReleasePad_Should_Sucess_When_Pad_Is_Src)
{
    MOCKER_CPP(&g_mutex_lock).stubs().will(invoke(StubLock));
    MOCKER_CPP(&g_mutex_unlock).stubs().will(invoke(StubUnlock));
    MOCKER_CPP(&ClearFilterWhenReadyToNull).stubs().will(invoke(StubClearFilterWhenReadyToNull));
    constexpr int padNum = 2;
    MxGstBase base;
    base.padIdx = padNum;
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    MxGstBaseReleasePad(GST_ELEMENT(&base), pad);
    EXPECT_EQ(base.padIdx, 1);
    gst_object_unref(pad);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_MxGstBaseDefaultEvent_Should_Sucess_With_Different_Event_CAPS)
{
    MxGstBase base;
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    GstCaps *caps = gst_caps_new_simple("video/x-raw", "format", G_TYPE_STRING, "I420", "width", G_TYPE_INT, 1280,
                                        "height", G_TYPE_INT, 720, "framerate", GST_TYPE_FRACTION, 30, 1, NULL);
    GstEvent *event = gst_event_new_caps(caps);
    gboolean retValue = true;
    MOCKER_CPP(&gst_pad_event_default).stubs().will(returnValue(retValue));
    gboolean ret = MxGstBaseDefaultEvent(pad, GST_OBJECT(&base), event);
    EXPECT_EQ(ret, true);
    gst_caps_unref(caps);
    gst_event_unref(event);
    gst_object_unref(pad);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_MxGstBaseDefaultEvent_Should_Sucess_With_Different_Event_FLUSH_START)
{
    MxGstBase base;
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    gboolean retValue = true;
    MOCKER_CPP(&gst_pad_event_default).stubs().will(returnValue(retValue));
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    plugin.pluginName_ = "osd";
    base.flushStartNum = 1;
    base.sinkPadVec = {};
    GstEvent *event = gst_event_new_flush_start();
    gboolean ret = MxGstBaseDefaultEvent(pad, GST_OBJECT(&base), event);
    EXPECT_EQ(ret, true);
    gst_event_unref(event);
    gst_object_unref(pad);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_MxGstBaseDefaultEvent_Should_Sucess_With_Different_Event_FLUSH_STOP)
{
    MxGstBase base;
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    gboolean retValue = true;
    MOCKER_CPP(&gst_pad_event_default).stubs().will(returnValue(retValue));
    base.flushStopNum = 1;
    base.sinkPadVec = {};
    GstEvent *event = gst_event_new_flush_stop(true);
    gboolean ret = MxGstBaseDefaultEvent(pad, GST_OBJECT(&base), event);
    EXPECT_EQ(ret, true);
    gst_event_unref(event);
    gst_object_unref(pad);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_GetPadIndex_Should_Sucess_When_Valid_Input)
{
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    std::vector<GstPad *> padVector = {nullptr, nullptr, pad};
    int ret = GetPadIndex(pad, padVector);
    constexpr int expectedIdx = 2;
    EXPECT_EQ(ret, expectedIdx);
    gst_object_unref(pad);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_MxGstBaseDefaultQuery_Should_Sucess_When_Valid_Input)
{
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    MxGstBase base;

    GstQuery *query = gst_query_new_position(GST_FORMAT_TIME);
    gboolean retValue = true;
    MOCKER_CPP(&gst_pad_query_default).stubs().will(returnValue(retValue));
    gboolean ret = MxGstBaseDefaultQuery(pad, GST_OBJECT(&base), query);
    EXPECT_EQ(ret, true);
    gst_object_unref(pad);
    gst_query_unref(query);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_HandleSrcPadCustomQuery_Should_Fail_With_Error_Size)
{
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    plugin.outputDataKeys_ = {};
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    GstStructure *structure = gst_structure_new_empty("test");
    MOCKER_CPP(&GetPadIndex).stubs().will(returnValue(1));
    gboolean ret = HandleSrcPadCustomQuery(pad, &base, "test", structure, "test");
    EXPECT_EQ(ret, false);
    gst_object_unref(pad);
    gst_structure_free(structure);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_HandleSrcPadCustomQuery_Should_Success_With_Valid_Input)
{
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    plugin.outputDataKeys_ = {"test", "test"};
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    GstStructure *structure = gst_structure_new_empty("test");
    MOCKER_CPP(&GetPadIndex).stubs().will(returnValue(0));
    MOCKER_CPP(&strcasecmp).stubs().will(returnValue(0));
    gboolean ret = HandleSrcPadCustomQuery(pad, &base, "test", structure, "test");
    EXPECT_EQ(ret, true);
    gst_object_unref(pad);
    gst_structure_free(structure);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_HandleSinkPadCustomQuery_Should_Fail_With_Error_Name)
{
    MxGstBase base;
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    GstStructure *structure = gst_structure_new_empty("test");
    MOCKER_CPP(&strcasecmp).stubs().will(returnValue(1));
    bool ret = HandleSinkPadCustomQuery(pad, &base, "test", structure, "test");
    EXPECT_EQ(ret, false);
    gst_object_unref(pad);
    gst_structure_free(structure);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_HandleSinkPadCustomQuery_Should_Fail_With_Error_Addr)
{
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    plugin.elementDynamicImageSize_ = {};
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    GstStructure *structure = gst_structure_new_empty("test");
    MOCKER_CPP(&strcasecmp).stubs().will(returnValue(0));
    const std::string retStr = "test";
    MOCKER_CPP(&MxPluginBase::GetElementNameWithObjectAddr).stubs().will(returnValue(retStr));
    bool ret = HandleSinkPadCustomQuery(pad, &base, "test", structure, "test");
    EXPECT_EQ(ret, false);

    plugin.elementDynamicImageSize_ = {{"test", {}}};
    constexpr int retInt = -1;
    MOCKER_CPP(&GetPadIndex).stubs().will(returnValue(retInt));
    ret = HandleSinkPadCustomQuery(pad, &base, "test", structure, "test");
    EXPECT_EQ(ret, false);
    gst_object_unref(pad);
    gst_structure_free(structure);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_HandleSinkPadCustomQuery_Should_Success_With_Vaild_Input)
{
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    ImageSize size(1, 1);
    plugin.elementDynamicImageSize_ = {{"test", {size}}};
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    GstStructure *structure = gst_structure_new_empty("test");
    MOCKER_CPP(&strcasecmp).stubs().will(returnValue(0));
    MOCKER_CPP(&GetPadIndex).stubs().will(returnValue(0));
    const std::string retStr = "test";
    MOCKER_CPP(&MxPluginBase::GetElementNameWithObjectAddr).stubs().will(returnValue(retStr));
    bool ret = HandleSinkPadCustomQuery(pad, &base, "test", structure, "test");
    EXPECT_EQ(ret, true);
    gst_object_unref(pad);
    gst_structure_free(structure);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_HandleCustomQuery_Should_Fail_When_Get_Name_Fail)
{
    MxGstBase base;
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    GstQuery *query = gst_query_new_position(GST_FORMAT_TIME);
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    const gchar *rerPtr = nullptr;
    MOCKER_CPP(&gst_structure_get_name).stubs().will(returnValue(rerPtr));
    gboolean ret = HandleCustomQuery(pad, GST_OBJECT(&base), query);
    EXPECT_EQ(ret, false);
    gst_object_unref(pad);
    gst_query_unref(query);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_HandleCustomQuery_Should_Success_With_Src_Pad)
{
    MxGstBase base;
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    GstQuery *query = gst_query_new_position(GST_FORMAT_TIME);
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    const gchar *rerPtr = "test";
    MOCKER_CPP(&gst_structure_get_name).stubs().will(returnValue(rerPtr));
    MOCKER_CPP(&strcasecmp).times(TWICE).will(returnValue(0)).then(returnValue(1));
    gboolean retValue = true;
    MOCKER_CPP(&gst_pad_query_default).stubs().will(returnValue(retValue));
    gboolean ret = HandleCustomQuery(pad, GST_OBJECT(&base), query);
    EXPECT_EQ(ret, true);

    MOCKER_CPP(&HandleSrcPadCustomQuery).stubs().will(returnValue(retValue));
    ret = HandleCustomQuery(pad, GST_OBJECT(&base), query);
    EXPECT_EQ(ret, true);
    gst_object_unref(pad);
    gst_query_unref(query);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_HandleCustomQuery_Should_Success_With_Sink_Pad)
{
    MxGstBase base;
    GstPad *pad = gst_pad_new("src", GST_PAD_SINK);
    GstQuery *query = gst_query_new_position(GST_FORMAT_TIME);
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    const gchar *rerPtr = "test";
    MOCKER_CPP(&gst_structure_get_name).stubs().will(returnValue(rerPtr));
    MOCKER_CPP(&HandleSinkPadCustomQuery).stubs().will(returnValue(true));
    gboolean ret = HandleCustomQuery(pad, GST_OBJECT(&base), query);
    EXPECT_EQ(ret, true);
    gst_object_unref(pad);
    gst_query_unref(query);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_MxGstBasePadQuery_Should_Success_With_CUSTOM)
{
    MxGstBase base;
    GstStructure *structure = gst_structure_new_empty("test");
    GstPad *pad = gst_pad_new("src", GST_PAD_SRC);
    GstQuery *query = gst_query_new_custom(GST_QUERY_CUSTOM, structure);
    gboolean retValue = true;
    MOCKER_CPP(&HandleCustomQuery).stubs().will(returnValue(retValue));
    gboolean ret = MxGstBasePadQuery(pad, GST_OBJECT(&base), query);
    EXPECT_EQ(ret, true);
    gst_structure_free(structure);
    gst_object_unref(pad);
    gst_query_unref(query);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_SyncBufferProcess_Should_Success_When_JudgeBufSize_True)
{
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    base.input = {buffer, buffer};
    base.inputQueue = {nullptr, nullptr};
    MOCKER_CPP(&IsStreamElementNameExist).stubs().will(returnValue(true));
    GstFlowReturn ret = SyncBufferProcess(base, *buffer, 0);
    EXPECT_EQ(ret, GST_FLOW_OK);
    MxpiBufferManager::DestroyBuffer(buffer);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_SyncBufferProcess_Should_Success_When_JudgeBufSize_False)
{
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    base.input = {nullptr, buffer};
    MxpiBuffer *buffer1 = new MxpiBuffer();
    base.inputQueue = {buffer1, nullptr};
    MOCKER_CPP(&IsStreamElementNameExist).stubs().will(returnValue(true));
    MOCKER_CPP(&JudgeBufSize).stubs().will(returnValue(false));
    GstFlowReturn ret = SyncBufferProcess(base, *buffer, 0);
    EXPECT_EQ(ret, GST_FLOW_OK);
    MxpiBufferManager::DestroyBuffer(buffer);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_BufferProcess_Should_Success_When_Status_SYNC)
{
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    plugin.status_ = SYNC;
    GstBuffer gstBuffer;
    GstFlowReturn retErr = GST_FLOW_OK;
    MOCKER_CPP(&SyncBufferProcess).stubs().will(returnValue(retErr));
    GstFlowReturn ret = BufferProcess(&base, &gstBuffer, 0);
    EXPECT_EQ(ret, GST_FLOW_OK);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_BufferProcess_Should_Success_When_Status_ASYNC_And_Data_InValid)
{
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    base.input = {nullptr, nullptr};
    plugin.status_ = ASYNC;
    plugin.srcPadNum_ = 1;
    GstBuffer gstBuffer;
    MOCKER_CPP(&strcasecmp).times(TWICE).will(returnValue(1));
    std::shared_ptr<std::map<std::string, MxpiErrorInfo>> errorRet =
        std::make_shared<std::map<std::string, MxpiErrorInfo>>();
    MxpiErrorInfo info = {APP_ERR_COMM_FAILURE, "error"};
    errorRet->insert({"error", info});
    MOCKER_CPP(&MxpiMetadataManager::GetErrorInfo).times(1).will(returnValue(errorRet));
    GstFlowReturn ret = BufferProcess(&base, &gstBuffer, 0);
    EXPECT_EQ(ret, GST_FLOW_OK);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_BufferProcess_Should_Success_When_Status_ASYNC_And_Data_Valid)
{
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    base.input = {nullptr, nullptr};
    plugin.status_ = ASYNC;
    plugin.srcPadNum_ = 0;
    GstBuffer gstBuffer;
    MOCKER_CPP(&IsStreamElementNameExist).stubs().will(returnValue(true));
    std::shared_ptr<std::map<std::string, MxpiErrorInfo>> errorRet =
        std::make_shared<std::map<std::string, MxpiErrorInfo>>();
    MxpiErrorInfo info = {APP_ERR_COMM_FAILURE, "error"};
    errorRet->insert({"error", info});
    GstFlowReturn ret = BufferProcess(&base, &gstBuffer, 0);
    EXPECT_EQ(ret, GST_FLOW_OK);
}
TEST_F(MxGstBaseTest, Test_MxGstBase_MxGstBaseChain_Should_Fail_With_Error_Idx)
{
    GstPad *pad = gst_pad_new("src", GST_PAD_SINK);
    MxGstBase base;
    GstBuffer buffer;
    constexpr int retVal = -1;
    MOCKER_CPP(&GetPadIndex).stubs().will(returnValue(retVal));
    GstFlowReturn ret = MxGstBaseChain(pad, GST_OBJECT(&base), &buffer);
    EXPECT_EQ(ret, GST_FLOW_ERROR);
    gst_object_unref(pad);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_MxGstBaseChain_Should_Fail_When_SetDevice_Fail)
{
    GstPad *pad = gst_pad_new("src", GST_PAD_SINK);
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    plugin.useDevice_ = true;
    GstBuffer buffer;
    APP_ERROR retVal = APP_ERR_COMM_FAILURE;
    MOCKER_CPP(&SetDevice).stubs().will(returnValue(retVal));
    GstFlowReturn ret = MxGstBaseChain(pad, GST_OBJECT(&base), &buffer);
    EXPECT_EQ(ret, GST_FLOW_ERROR);
    gst_object_unref(pad);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_MxGstBaseChain_Should_Success_With_Valid_Input)
{
    GstPad *pad = gst_pad_new("src", GST_PAD_SINK);
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    GstBuffer buffer;
    MOCKER_CPP(&GetPadIndex).stubs().will(returnValue(0));
    MOCKER_CPP(&SetDevice).stubs().will(returnValue(0));
    MOCKER_CPP(&BufferProcess).stubs().will(returnValue(0));
    GstFlowReturn ret = MxGstBaseChain(pad, GST_OBJECT(&base), &buffer);
    EXPECT_EQ(ret, GST_FLOW_OK);
    gst_object_unref(pad);
}
TEST_F(MxGstBaseTest, Test_MxGstBase_SetDynamicImageSize_Should_Success_When_Query_Fail)
{
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    base.configParam = std::make_unique<std::map<std::string, std::shared_ptr<void>>>();
    (*base.configParam)["resizeHeight"] = nullptr;
    (*base.configParam)["resizeWidth"] = nullptr;
    base.srcPadVec = {nullptr};
    gboolean retVal = false;
    MOCKER_CPP(&gst_pad_peer_query).stubs().will(returnValue(retVal));
    gboolean ret = SetDynamicImageSize(&base);
    EXPECT_EQ(ret, true);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_SetDynamicImageSize_Should_Success_When_Get_Value_Fail)
{
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    base.configParam = std::make_unique<std::map<std::string, std::shared_ptr<void>>>();
    (*base.configParam)["resizeHeight"] = nullptr;
    (*base.configParam)["resizeWidth"] = nullptr;
    base.srcPadVec = {nullptr};
    gboolean retVal = true;
    MOCKER_CPP(&gst_pad_peer_query).stubs().will(returnValue(retVal));
    const GValue *retPtr = nullptr;
    MOCKER_CPP(&gst_structure_get_value).stubs().will(returnValue(retPtr));
    gboolean ret = SetDynamicImageSize(&base);
    EXPECT_EQ(ret, true);
}

TEST_F(MxGstBaseTest, Test_MxGstBase_SetDynamicImageSize_Should_Success_When_Valid_Input)
{
    MxGstBase base;
    MxPluginBaseDerived plugin;
    base.pluginInstance = &plugin;
    base.configParam = std::make_unique<std::map<std::string, std::shared_ptr<void>>>();
    (*base.configParam)["resizeHeight"] = nullptr;
    (*base.configParam)["resizeWidth"] = nullptr;
    base.srcPadVec = {nullptr};
    gboolean retVal = true;
    MOCKER_CPP(&gst_pad_peer_query).stubs().will(returnValue(retVal));
    const GValue *retPtr = new GValue();
    MOCKER_CPP(&gst_structure_get_value).stubs().will(returnValue(retPtr));
    const gchar *retStr = "test";
    MOCKER_CPP(&g_value_get_string).stubs().will(returnValue(retStr));
    std::vector<std::string> retVec = {"1", "1"};
    MOCKER_CPP(&StringUtils::Split).stubs().will(returnValue(retVec));
    gboolean ret = SetDynamicImageSize(&base);
    EXPECT_EQ(ret, true);
}
}  // namespace

int main(int argc, char *argv[])
{
    gst_init(&argc, &argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

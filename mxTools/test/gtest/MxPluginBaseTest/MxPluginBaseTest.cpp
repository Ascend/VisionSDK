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
 * Description: MxPluginBaseTest.
 * Author: Mind SDK
 * Create: 2025
 * History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include <gst/gst.h>
#include <string>

#define private public
#define protected public
#include "MxTools/PluginToolkit/base/MxPluginBase.h"
#undef private
#undef protected

#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/base/MxGstBase.h"
#include "MxPluginBaseDptr.hpp"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

using namespace MxTools;
using namespace MxBase;

namespace {
constexpr int THREE_TIMES = 3;
constexpr int INIT_DEVICEID = -1;
constexpr int INIT_DATASIZE = 1;
MxTools::InputParam g_bufferParam = {"", INIT_DEVICEID, INIT_DATASIZE};
class MxPluginBaseTest : public testing::Test {
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
};

APP_ERROR ProcessStub(MxPluginBaseDerived *p, std::vector<MxpiBuffer *> &mxpiBuffer)
{
    throw std::runtime_error("In ProcessStub.");
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_RunProcess_Should_Fail_With_Empty_Input)
{
    std::vector<MxpiBuffer *> inputs = {};
    MxPluginBaseDerived plugin;
    APP_ERROR ret = plugin.RunProcess(inputs);
    EXPECT_EQ(ret, APP_ERR_COMM_OUT_OF_RANGE);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_RunProcess_Should_Fail_With_Async_Check_Fail)
{
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    std::vector<MxpiBuffer *> inputs = {buffer};
    MOCKER_CPP(&MxPluginBase::AsyncPreProcessCheck).times(1).will(returnValue(1));
    MxPluginBaseDerived plugin;
    plugin.status_ = ASYNC;
    APP_ERROR ret = plugin.RunProcess(inputs);
    EXPECT_EQ(ret, 1);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_RunProcess_Should_Fail_With_Sync_Check_Fail)
{
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    std::vector<MxpiBuffer *> inputs = {buffer};
    MOCKER_CPP(&MxPluginBase::SyncPreProcessCheck).times(1).will(returnValue(1));
    MxPluginBaseDerived plugin;
    plugin.status_ = SYNC;
    APP_ERROR ret = plugin.RunProcess(inputs);
    EXPECT_EQ(ret, 1);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_RunProcess_Should_Success_When_Process_Success)
{
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    std::vector<MxpiBuffer *> inputs = {buffer};
    MOCKER_CPP(&MxPluginBase::SyncPreProcessCheck).times(1).will(returnValue(0));
    MxPluginBaseDerived plugin;
    plugin.status_ = SYNC;
    APP_ERROR ret = plugin.RunProcess(inputs);
    EXPECT_EQ(ret, APP_ERR_OK);
    MxpiBufferManager::DestroyBuffer(buffer);
}

TEST_F(MxPluginBaseTest, DISABLED_Test_MxPluginBase_RunProcess_Should_Success_When_Process_Throw_Exception)
{
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    std::vector<MxpiBuffer *> inputs = {buffer};
    MxPluginBaseDerived plugin;
    MOCKER_CPP_VIRTUAL(plugin, &MxPluginBaseDerived::Process).times(1).will(invoke(ProcessStub));
    plugin.status_ = SYNC;
    APP_ERROR ret = plugin.RunProcess(inputs);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_AsyncPreProcessCheck_Should_Success_With_No_Check)
{
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    std::vector<MxpiBuffer *> inputs = {buffer};
    MxPluginBaseDerived plugin;
    plugin.doPreErrorCheck_ = false;
    plugin.doPreMetaDataCheck_ = false;
    APP_ERROR ret = plugin.AsyncPreProcessCheck(inputs);
    EXPECT_EQ(ret, APP_ERR_OK);
    MxpiBufferManager::DestroyBuffer(buffer);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_AsyncPreProcessCheck_Should_Fail_With_Wrong_Error_Check)
{
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    std::vector<MxpiBuffer *> inputs;
    MxPluginBaseDerived plugin;
    plugin.srcPadNum_ = 0;
    APP_ERROR ret = plugin.AsyncPreProcessCheck(inputs);
    EXPECT_EQ(ret, APP_ERR_COMM_OUT_OF_RANGE);

    inputs = {buffer};
    plugin.doPreErrorCheck_ = true;
    std::shared_ptr<std::map<std::string, MxpiErrorInfo>> errorRet =
        std::make_shared<std::map<std::string, MxpiErrorInfo>>();
    MOCKER_CPP(&MxpiMetadataManager::GetErrorInfo).times(1).will(returnValue(errorRet));
    ret = plugin.AsyncPreProcessCheck(inputs);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_AsyncPreProcessCheck_Should_Fail_With_Wrong_MetaData_Check)
{
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    std::vector<MxpiBuffer *> inputs = {buffer};
    MxPluginBaseDerived plugin;
    plugin.srcPadNum_ = 0;
    plugin.doPreErrorCheck_ = false;
    plugin.doPreMetaDataCheck_ = true;
    std::shared_ptr<void> dataRet = nullptr;
    MOCKER_CPP(&MxpiMetadataManager::GetMetadata).times(1).will(returnValue(dataRet));
    APP_ERROR ret = plugin.AsyncPreProcessCheck(inputs);
    EXPECT_EQ(ret, APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_SyncPreProcessCheck_Should_Fail_With_Empty_Buffer)
{
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    std::vector<MxpiBuffer *> inputs = {buffer, nullptr};
    MxPluginBaseDerived plugin;
    APP_ERROR ret = plugin.SyncPreProcessCheck(inputs);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_SyncPreProcessCheck_Should_Fail_With_Wrong_Error_Check)
{
    MxpiBuffer *buffer1 = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    MxpiBuffer *buffer2 = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    std::vector<MxpiBuffer *> inputs = {buffer1, buffer2};
    MxPluginBaseDerived plugin;
    plugin.srcPadNum_ = 0;
    plugin.doPreErrorCheck_ = true;
    std::shared_ptr<std::map<std::string, MxpiErrorInfo>> errorRet =
        std::make_shared<std::map<std::string, MxpiErrorInfo>>();
    MxpiErrorInfo info = {APP_ERR_COMM_FAILURE, "error"};
    errorRet->insert({"error", info});
    MOCKER_CPP(&MxpiMetadataManager::GetErrorInfo).times(THREE_TIMES).will(returnValue(errorRet));
    APP_ERROR ret = plugin.SyncPreProcessCheck(inputs);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_SyncPreProcessCheck_Should_Fail_With_Wrong_MetaData_Size_Check)
{
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    std::vector<MxpiBuffer *> inputs = {buffer};
    MxPluginBaseDerived plugin;
    plugin.sinkPadNum_ = 1;
    plugin.dataSourceKeys_ = {};
    plugin.doPreErrorCheck_ = false;
    plugin.doPreMetaDataCheck_ = true;
    APP_ERROR ret = plugin.SyncPreProcessCheck(inputs);
    EXPECT_EQ(ret, APP_ERR_COMM_OUT_OF_RANGE);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_SyncPreProcessCheck_Should_Fail_With_Wrong_MetaData_Data_Check)
{
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    std::vector<MxpiBuffer *> inputs = {buffer};
    MxPluginBaseDerived plugin;
    plugin.srcPadNum_ = 1;
    plugin.dataSourceKeys_ = {};
    plugin.doPreErrorCheck_ = false;
    plugin.doPreMetaDataCheck_ = true;
    plugin.sinkPadNum_ = 1;
    plugin.dataSourceKeys_ = {"test"};
    std::shared_ptr<void> dataRet = nullptr;
    MOCKER_CPP(&MxpiMetadataManager::GetMetadata).times(1).will(returnValue(dataRet));
    MOCKER_CPP(&MxpiMetadataManager::CopyMetadata).times(1).will(returnValue(0));
    APP_ERROR ret = plugin.SyncPreProcessCheck(inputs);
    EXPECT_EQ(ret, APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_SyncPreProcessCheck_Should_Success_With_Correct_Input)
{
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    std::vector<MxpiBuffer *> inputs = {buffer};
    MxPluginBaseDerived plugin;
    plugin.doPreErrorCheck_ = false;
    plugin.doPreMetaDataCheck_ = false;
    APP_ERROR ret = plugin.SyncPreProcessCheck(inputs);
    EXPECT_EQ(ret, APP_ERR_OK);
    MxpiBufferManager::DestroyBuffer(buffer);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_SendData_Should_Fail_With_Nullptr_Input)
{
    MxpiBuffer *buffer = new MxpiBuffer();
    buffer->buffer = nullptr;
    MxPluginBaseDerived plugin;
    APP_ERROR ret = plugin.SendData(0, *buffer);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_SendData_Should_Fail_With_Invalid_Input)
{
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    MxPluginBaseDerived plugin;
    MxGstBase gstBase;
    plugin.pMxPluginBaseDptr_->elementInstance_ = (void *)(&gstBase);
    const int padNum = 257;
    std::vector<GstPad *> srcPadVec(padNum, nullptr);
    gstBase.srcPadVec = srcPadVec;
    APP_ERROR ret = plugin.SendData(0, *buffer);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);

    buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    gstBase.srcPadVec.clear();
    ret = plugin.SendData(0, *buffer);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_SendData_Should_Success_With_Valid_Input)
{
    MxpiBuffer *buffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    MxPluginBaseDerived plugin;
    MxGstBase gstBase;
    plugin.pMxPluginBaseDptr_->elementInstance_ = (void *)(&gstBase);
    std::vector<GstPad *> srcPadVec(1, nullptr);
    gstBase.srcPadVec = srcPadVec;
    MOCKER_CPP(&IsStreamElementNameExist).times(1).will(returnValue(true));
    MOCKER_CPP(&ConvertReturnCodeToLocal).times(1).will(returnValue(0));
    APP_ERROR ret = plugin.SendData(0, *buffer);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_SendDataToAllPorts_Should_Fail_With_Nullptr_Input)
{
    MxpiBuffer *buffer = new MxpiBuffer();
    buffer->buffer = nullptr;
    MxPluginBaseDerived plugin;
    APP_ERROR ret = plugin.SendDataToAllPorts(*buffer);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    delete buffer;
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_GenerateStaticPortsInfo_Should_Fail_With_Wrong_portsDesc)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        PortDirection portDir = INPUT_PORT;
        std::vector<std::vector<std::string>> portsDesc = {};
        MxpiPortInfo portInfo;
        MxPluginBaseDerived plugin;
        plugin.GenerateStaticPortsInfo(portDir, portsDesc, portInfo);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_GenerateStaticPortsInfo_Should_Fail_With_Wrong_portsNum)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        PortDirection portDir = INPUT_PORT;
        std::vector<std::vector<std::string>> portsDesc = {{"test"}};
        MxpiPortInfo portInfo;
        const int num = 1200;
        portInfo.portNum = num;
        MxPluginBaseDerived plugin;
        plugin.GenerateStaticPortsInfo(portDir, portsDesc, portInfo);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_GenerateDynamicPortsInfo_Should_Fail_With_Wrong_portsDesc)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        PortDirection portDir = INPUT_PORT;
        std::vector<std::vector<std::string>> portsDesc = {};
        MxpiPortInfo portInfo;
        MxPluginBaseDerived plugin;
        plugin.GenerateDynamicPortsInfo(portDir, portsDesc, portInfo);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_GenerateDynamicPortsInfo_Should_Fail_With_Wrong_portsNum)
{
    APP_ERROR ret = APP_ERR_OK;
    try {
        PortDirection portDir = OUTPUT_PORT;
        std::vector<std::vector<std::string>> portsDesc = {{"test"}};
        MxpiPortInfo portInfo;
        const int num = 1500;
        portInfo.portNum = num;
        MxPluginBaseDerived plugin;
        plugin.GenerateDynamicPortsInfo(portDir, portsDesc, portInfo);
    } catch (std::exception &e) {
        ret = APP_ERR_COMM_FAILURE;
    }
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_DefinePorts_Should_Success)
{
    MxPluginBaseDerived plugin;
    MxpiPortInfo inputInfo = plugin.DefineInputPorts();
    EXPECT_EQ(inputInfo.direction, INPUT_PORT);
    MxpiPortInfo outputInfo = plugin.DefineOutputPorts();
    EXPECT_EQ(outputInfo.direction, OUTPUT_PORT);
    auto properties = plugin.DefineProperties();
    EXPECT_EQ(properties.size(), 1);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_Dump_Load_Should_Success)
{
    std::string retValue = "test";
    MOCKER_CPP(&MxpiBufferDump::DoDump).times(1).will(returnValue(retValue));
    MxPluginBaseDerived plugin;
    MxpiBuffer mxpiBuffer = {};
    std::vector<std::string> filterKeys = {};
    std::vector<std::string> requiredKeys = {};
    std::string info = plugin.DoDump(mxpiBuffer, filterKeys, requiredKeys);
    EXPECT_EQ(info, retValue);

    MxTools::MxpiBuffer *retBuffer = new MxpiBuffer();
    MOCKER_CPP(MxTools::MxpiBufferDump::DoLoad, MxTools::MxpiBuffer * (MxTools::MxpiBuffer &, int))
        .times(1).will(returnValue(retBuffer));
    MxpiBuffer *buffer = plugin.DoLoad(mxpiBuffer);
    EXPECT_NE(buffer, nullptr);
    delete retBuffer;
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_SendMxpiErrorInfo_Should_Fail_With_Nullptr_Input)
{
    MxPluginBaseDerived plugin;
    MxpiBuffer *mxpiBuffer = new MxpiBuffer();
    std::string pluginName = "";
    std::string errorText = "error";
    APP_ERROR ret = plugin.SendMxpiErrorInfo(*mxpiBuffer, pluginName, APP_ERR_COMM_FAILURE, errorText);
    EXPECT_EQ(ret, APP_ERR_PLUGIN_TOOLKIT_METADATA_PLUGINNAME_EMPTY);

    MOCKER_CPP(&MxpiMetadataManager::AddErrorInfo).times(1).will(returnValue(0));
    mxpiBuffer = new MxpiBuffer();
    mxpiBuffer->buffer = nullptr;
    ret = plugin.SendMxpiErrorInfo(*mxpiBuffer, pluginName, APP_ERR_COMM_FAILURE, errorText);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_SendMxpiErrorInfo_Should_Success_With_Valid_Input)
{
    MOCKER_CPP(&MxpiMetadataManager::AddErrorInfo).times(1).will(returnValue(0));
    MOCKER_CPP(&MxPluginBase::SendData).times(1).will(returnValue(0));
    MxPluginBaseDerived plugin;
    plugin.srcPadNum_ = 1;
    MxpiBuffer *mxpiBuffer = MxpiBufferManager::CreateHostBuffer(g_bufferParam);
    std::string pluginName = "";
    std::string errorText = "error";
    APP_ERROR ret = plugin.SendMxpiErrorInfo(*mxpiBuffer, pluginName, APP_ERR_COMM_FAILURE, errorText);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_SetOutputDataKeys_Should_Success_With_Valid_Input)
{
    MxPluginBaseDerived plugin;
    plugin.srcPadNum_ = 0;
    APP_ERROR ret = plugin.SetOutputDataKeys();
    EXPECT_EQ(ret, APP_ERR_OK);

    plugin.srcPadNum_ = 1;
    ret = plugin.SetOutputDataKeys();
    EXPECT_EQ(ret, APP_ERR_OK);

    plugin.srcPadNum_ = THREE_TIMES;
    ret = plugin.SetOutputDataKeys();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxPluginBaseTest, Test_MxPluginBase_GetElementNameWithObjectAddr_Should_Success)
{
    MxPluginBaseDerived plugin;
    const std::string retValue = "test";
    plugin.elementName_ = retValue;
    std::string ret = plugin.GetElementNameWithObjectAddr();
    auto pos = ret.find(retValue);
    EXPECT_NE(pos, std::string::npos);
}
}  // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

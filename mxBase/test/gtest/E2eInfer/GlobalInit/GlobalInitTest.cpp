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
 * Description: Gtest unit cases.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "MxBase/DeviceManager/DeviceManager.h"
#include "ResourceManager/HAL/AclApi.h"
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "ResourceManager/GlobalInit/LogManager.hpp"
#include "ResourceManager/StreamTensorManager/StreamTensorManager.h"

namespace {
using namespace MxBase;
constexpr uint32_t INVALID_MIN_CHN_NUM = 0;
constexpr uint32_t INVALID_MAX_CHN_NUM = 129;
constexpr uint32_t DUMMY_SIZE2 = 2;
class GlobalInitTest : public testing::Test {
protected:
    void SetUp() {
    }
    void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

TEST_F(GlobalInitTest, Test_MxInit_Should_Return_Success_When_Device_Is_Ok)
{
    APP_ERROR ret = MxBase::MxInit();
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MxDeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(GlobalInitTest, Test_MxInit_Should_Return_Success_When_AppGlobalCfg_Is_default)
{
    AppGlobalCfg globalCfg;
    APP_ERROR ret = MxBase::MxInit(globalCfg);
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = MxBase::MxDeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(GlobalInitTest, Test_MxInit_Should_Return_Fail_When_vpcChnNum_Is_Invalid)
{
    AppGlobalCfg globalCfg;
    globalCfg.vpcChnNum = INVALID_MIN_CHN_NUM;
    APP_ERROR ret = MxBase::MxInit(globalCfg);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = MxBase::MxDeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(GlobalInitTest, Test_MxInit_Should_Return_Fail_When_jpegdChnNum_Is_Invalid)
{
    AppGlobalCfgExtra globalCfgExtra;
    globalCfgExtra.jpegdChnNum = INVALID_MAX_CHN_NUM;
    APP_ERROR ret = MxBase::MxInit(globalCfgExtra);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    ret = MxBase::MxDeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(GlobalInitTest, Test_MxInitFromConfig_Should_Return_Success_When_Input_Parameters_OK)
{
    MOCKER_CPP(&AclApi::aclSetCompileopt).stubs().will(returnValue(0));
    MOCKER_CPP(&AclApi::aclopCompile).stubs().will(returnValue(0));
    if (DeviceManager::IsAscend310P()) {
        APP_ERROR ret = MxBase::MxInitFromConfig("./TestInit/OpConfig.json");
        EXPECT_EQ(ret, APP_ERR_OK);
        ret = MxBase::MxDeInit();
        EXPECT_EQ(ret, APP_ERR_OK);
    }
}

TEST_F(GlobalInitTest, Test_MxInitFromConfig_Should_Return_Fail_When_Path_Not_Exist)
{
    APP_ERROR ret = MxBase::MxInitFromConfig("./TestInit/test.json");
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PATH);
    MxBase::MxDeInit();
}

TEST_F(GlobalInitTest, Test_MxInitFromConfig_Should_Return_Fail_When_File_Not_Json)
{
    APP_ERROR ret = MxBase::MxInitFromConfig("./TestInit/NotJson.txt");
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    MxBase::MxDeInit();
}

TEST_F(GlobalInitTest, Test_MxInitFromConfig_Should_Return_Fail_When_Invalid_Json)
{
    APP_ERROR ret = MxBase::MxInitFromConfig("./TestInit/Invalid.json");
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
    MxBase::MxDeInit();
}

TEST_F(GlobalInitTest, Test_MxInitFromConfig_Should_Return_Fail_When_Json_Not_Contains_Operations)
{
    APP_ERROR ret = MxBase::MxInitFromConfig("./TestInit/NotContainOperations.json");
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
    MxBase::MxDeInit();
}

TEST_F(GlobalInitTest, Test_MxInitFromConfig_Should_Return_Fail_When_Json_Not_Contains_Name)
{
    APP_ERROR ret = MxBase::MxInitFromConfig("./TestInit/NotContainName.json");
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
    MxBase::MxDeInit();
}

TEST_F(GlobalInitTest, Test_MxInitFromConfig_Should_Return_Fail_When_Json_Not_Contains_Preload_List)
{
    APP_ERROR ret = MxBase::MxInitFromConfig("./TestInit/NotContainPreloadList.json");
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
    MxBase::MxDeInit();
}

TEST_F(GlobalInitTest, Test_MxInitFromConfig_Should_Return_Fail_When_globalCfg_vpcChnNum_Exceed_Min)
{
    MOCKER_CPP(&AclApi::aclopCompile).stubs().will(returnValue(0));
    AppGlobalCfg globalCfg;
    globalCfg.vpcChnNum = 0;
    APP_ERROR ret = MxBase::MxInitFromConfig("./TestInit/OpConfig.json", globalCfg);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(GlobalInitTest, Test_MxInitFromConfig_Should_Return_Fail_When_globalCfg_vpcChnNum_Exceed_Max)
{
    MOCKER_CPP(&AclApi::aclopCompile).stubs().will(returnValue(0));
    AppGlobalCfg globalCfg;
    const int maxPoolSize = 129;
    globalCfg.vpcChnNum = maxPoolSize;
    APP_ERROR ret = MxBase::MxInitFromConfig("./TestInit/OpConfig.json", globalCfg);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(GlobalInitTest, Test_MxDeInit_Success)
{
    MOCKER_CPP(&AscendStream::DestroyAscendStream).times(1).will(returnValue(0));
    MOCKER_CPP(&DeviceManager::DestroyDevices).times(1).will(returnValue(0));
    MOCKER(&StreamTensorManager::DeInit).stubs().will(returnValue(0));
    MOCKER_CPP(&Log::Deinit).times(1).will(returnValue(0));
    APP_ERROR ret = MxDeInit();
    EXPECT_EQ(ret, 0);
}

TEST_F(GlobalInitTest, Test_MxDeInit_DestroyAscendStream_Failure)
{
    MOCKER_CPP(&AscendStream::DestroyAscendStream).times(1).will(returnValue(1));
    MOCKER_CPP(&DeviceManager::DestroyDevices).times(1).will(returnValue(0));
    MOCKER(&StreamTensorManager::DeInit).stubs().will(returnValue(0));
    MOCKER_CPP(&Log::Deinit).times(1).will(returnValue(0));
    APP_ERROR ret = MxDeInit();
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(GlobalInitTest, Test_MxDeInit_DestroyDevices_Failure)
{
    MOCKER_CPP(&AscendStream::DestroyAscendStream).times(1).will(returnValue(0));
    MOCKER_CPP(&DeviceManager::DestroyDevices).times(1).will(returnValue(1));
    MOCKER(&StreamTensorManager::DeInit).stubs().will(returnValue(0));
    MOCKER_CPP(&Log::Deinit).times(1).will(returnValue(0));
    APP_ERROR ret = MxDeInit();
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(GlobalInitTest, Test_MxDeInit_StreamTensorManager_DeInit_Failure)
{
    MOCKER_CPP(&AscendStream::DestroyAscendStream).times(1).will(returnValue(0));
    MOCKER_CPP(&DeviceManager::DestroyDevices).times(1).will(returnValue(0));
    MOCKER(&StreamTensorManager::DeInit).stubs().will(returnValue(1));
    MOCKER_CPP(&Log::Deinit).times(1).will(returnValue(0));
    APP_ERROR ret = MxDeInit();
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(GlobalInitTest, Test_MxDeInit_Log_Deinit_Failure)
{
    MOCKER_CPP(&AscendStream::DestroyAscendStream).times(1).will(returnValue(0));
    MOCKER_CPP(&DeviceManager::DestroyDevices).times(1).will(returnValue(0));
    MOCKER(&StreamTensorManager::DeInit).stubs().will(returnValue(0));
    MOCKER_CPP(&Log::Deinit).times(1).will(returnValue(1));

    APP_ERROR ret = MxDeInit();
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}


TEST_F(GlobalInitTest, Test_MxInit_Success)
{
    MOCKER_CPP(&Log::Init).times(1).will(returnValue(0));
    MOCKER_CPP(&LogManager::LogRotateStart).times(1).will(returnValue(0));
    MOCKER_CPP(&DeviceManager::InitDevices).times(1).will(returnValue(0));
    APP_ERROR ret = MxInit();
    EXPECT_EQ(ret, 0);
}

TEST_F(GlobalInitTest, Test_MxInit_LogInitFailed)
{
    MOCKER_CPP(&Log::Init).times(1).will(returnValue(1));
    APP_ERROR ret = MxInit();
    EXPECT_EQ(ret, 1);
}

TEST_F(GlobalInitTest, Test_MxInit_LogRotateStartFailed)
{
    MOCKER_CPP(&Log::Init).times(1).will(returnValue(0));
    MOCKER_CPP(&LogManager::LogRotateStart).times(1).will(returnValue(1));
    APP_ERROR ret = MxInit();
    EXPECT_EQ(ret, 1);
}

TEST_F(GlobalInitTest, Test_MxInit_InitDevicesFailed)
{
    MOCKER_CPP(&Log::Init).times(1).will(returnValue(0));
    MOCKER_CPP(&LogManager::LogRotateStart).times(1).will(returnValue(0));
    MOCKER_CPP(&DeviceManager::InitDevices).times(1).will(returnValue(1));
    APP_ERROR ret = MxInit();
    EXPECT_EQ(ret, 1);
}

TEST_F(GlobalInitTest, Test_MxInit_DevicesNotSupportFailed)
{
    MOCKER_CPP(&Log::Init).times(1).will(returnValue(0));
    MOCKER_CPP(&LogManager::LogRotateStart).times(1).will(returnValue(0));
    MOCKER_CPP(&DeviceManager::InitDevices).times(1).will(returnValue(0));
    MOCKER_CPP(DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    MOCKER_CPP(DeviceManager::IsAtlas800IA2).stubs().will(returnValue(false));
    APP_ERROR ret = MxInit();
    EXPECT_EQ(ret, 0);
}

TEST_F(GlobalInitTest, Test_MxInit_WithInput_DevicesNotSupportFailed)
{
    AppGlobalCfgExtra globalCfgExtra;
    MOCKER_CPP(DeviceManager::IsAscend310P).stubs().will(returnValue(false));
    MOCKER_CPP(DeviceManager::IsAtlas800IA2).stubs().will(returnValue(false));
    APP_ERROR ret = MxInit(globalCfgExtra);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}


TEST_F(GlobalInitTest, Test_MxInit_WithInput_PngdChnNumFailed)
{
    AppGlobalCfgExtra globalCfgExtra;
    globalCfgExtra.vpcChnNum = DUMMY_SIZE2;
    globalCfgExtra.jpegdChnNum = DUMMY_SIZE2;
    globalCfgExtra.pngdChnNum = 0;
    APP_ERROR ret = MxInit(globalCfgExtra);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(GlobalInitTest, Test_MxInit_WithInput_JpegeChnNumFailed)
{
    AppGlobalCfgExtra globalCfgExtra;
    globalCfgExtra.vpcChnNum = DUMMY_SIZE2;
    globalCfgExtra.jpegdChnNum = DUMMY_SIZE2;
    globalCfgExtra.jpegeChnNum = 0;
    APP_ERROR ret = MxInit(globalCfgExtra);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(GlobalInitTest, Test_MxInit_WithInput_InitFailed)
{
    AppGlobalCfgExtra globalCfgExtra;
    globalCfgExtra.vpcChnNum = DUMMY_SIZE2;
    globalCfgExtra.jpegdChnNum = DUMMY_SIZE2;
    globalCfgExtra.jpegeChnNum = DUMMY_SIZE2;
    MOCKER_CPP(&MxBase::Log::Init).times(1).will(returnValue(1));
    APP_ERROR ret = MxInit(globalCfgExtra);
    EXPECT_EQ(ret, 1);
}

TEST_F(GlobalInitTest, Test_MxInit_WithInput_LogRotateFailed)
{
    AppGlobalCfgExtra globalCfgExtra;
    globalCfgExtra.vpcChnNum = DUMMY_SIZE2;
    globalCfgExtra.jpegdChnNum = DUMMY_SIZE2;
    globalCfgExtra.jpegeChnNum = DUMMY_SIZE2;
    MOCKER_CPP(&Log::Init).times(1).will(returnValue(0));
    MOCKER_CPP(&LogManager::LogRotateStart).times(1).will(returnValue(1));
    APP_ERROR ret = MxInit(globalCfgExtra);
    EXPECT_EQ(ret, 1);
}

TEST_F(GlobalInitTest, Test_MxInit_WithInput_InitDevicesFailed)
{
    AppGlobalCfgExtra globalCfgExtra;
    globalCfgExtra.vpcChnNum = DUMMY_SIZE2;
    globalCfgExtra.jpegdChnNum = DUMMY_SIZE2;
    globalCfgExtra.jpegeChnNum = DUMMY_SIZE2;
    MOCKER_CPP(&Log::Init).times(1).will(returnValue(0));
    MOCKER_CPP(&LogManager::LogRotateStart).times(1).will(returnValue(0));
    MOCKER_CPP(&DeviceManager::InitDevices).times(1).will(returnValue(1));
    APP_ERROR ret = MxInit(globalCfgExtra);
    EXPECT_EQ(ret, 1);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

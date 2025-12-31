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
* Description: DT test for the DvppWrapperWithAcl.cpp file.
* Author: Mind SDK
* Create: 2025
* History: NA
 */

#include <gtest/gtest.h>
#include <mockcpp/mockcpp.hpp>
#include "acl/acl.h"
#include "MxBase/MxBase.h"
#define private public
#include "include/MxBase/DvppWrapper/DvppWrapper.h"
#undef private
#include "module/ResourceManager/DvppWrapper/DvppWrapperWithAcl/DvppWrapperWithAcl.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "module/ResourceManager/GlobalInit/LogManager.hpp"
#include "include/MxBase/MemoryHelper/MemoryHelper.h"

namespace {
using namespace MxBase;
const int TWO = 2;
const int THREE = 3;
const int FOUR = 4;
const uint32_t OVERRANGE = 10;
const uint32_t MAX_VDEC_CHANNEL_NUM_310 = 31;
const uint32_t MAX_VDEC_CHANNEL_NUM_310B = 127;
uint8_t g_data1[3] = {1, 2, 3};
uint8_t g_data2 = 2;
aclvdecChannelDesc* g_channelDesc = (aclvdecChannelDesc*)&g_data1;
acldvppChannelDesc* g_channel = (acldvppChannelDesc*)&g_data1;

class DvppWrapperWithAclTest : public testing::Test {
protected:
    void TearDown() override
    {
        GlobalMockObject::verify();
    }
};

TEST_F(DvppWrapperWithAclTest, Test_Init_Should_Return_Fail_When_acldvppCreateChannelDesc_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->Init();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_Init_Should_Return_Fail_When_acldvppCreateChannel_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppCreateChannelDesc).times(1).will(returnValue(g_channel));
    MOCKER_CPP(&acldvppCreateChannel).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->Init();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_Init_Should_Return_Fail_When_aclrtCreateStream_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppCreateChannelDesc).times(1).will(returnValue(g_channel));
    MOCKER_CPP(&acldvppCreateChannel).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtCreateStream).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->Init();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_Init_Should_Return_True)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppCreateChannelDesc).times(1).will(returnValue(g_channel));
    MOCKER_CPP(&acldvppCreateChannel).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtCreateStream).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->Init();
    EXPECT_EQ(ret, APP_ERR_OK);
    APP_ERROR ret2 = dvppWrapper.dvppWrapperBase_->Init();
    EXPECT_EQ(ret2, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_Init2_Should_Return_Fail_When_acldvppCreateChannelDesc_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MxbaseDvppChannelMode dvppChannelMode = MXBASE_DVPP_CHNMODE_DEFAULT;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->Init(dvppChannelMode);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_InitJpegEncodeChannel_Should_Return_Fail_When_acldvppCreateChannelDesc_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    JpegEncodeChnConfig jpegEncodeChnConfig;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitJpegEncodeChannel(jpegEncodeChnConfig);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_InitJpegDecodeChannel_Should_Return_Fail_When_acldvppCreateChannelDesc_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    JpegDecodeChnConfig jpegDecodeChnConfig;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitJpegDecodeChannel(jpegDecodeChnConfig);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_InitVpcChannel_Should_Return_Fail_When_acldvppCreateChannelDesc_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    VpcChnConfig vpcChnConfig;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVpcChannel(vpcChnConfig);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_InitPngDecodeChannel_Should_Return_Fail_When_acldvppCreateChannelDesc_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    PngDecodeChnConfig config;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitPngDecodeChannel(config);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_CheckVdecConfig_Should_Return_Fail_When_IsAscend310_outputImageFormat_Wrong)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true));
    VdecConfig vdecConfig;
    vdecConfig.outputImageFormat = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_400;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_DVPP_INVALID_FORMAT);
}

TEST_F(DvppWrapperWithAclTest, Test_CheckVdecConfig_Should_Return_Fail_When_IsAscend310_YUV_channelId_Wrong)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true));
    VdecConfig vdecConfig;
    vdecConfig.outputImageFormat = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    vdecConfig.channelId = OVERRANGE + MAX_VDEC_CHANNEL_NUM_310;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE);
}

TEST_F(DvppWrapperWithAclTest, Test_CheckVdecConfig_Should_Return_Fail_When_IsAscend310_YVU_channelId_Wrong)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true));
    VdecConfig vdecConfig;
    vdecConfig.outputImageFormat = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vdecConfig.channelId = OVERRANGE + MAX_VDEC_CHANNEL_NUM_310;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE);
}

TEST_F(DvppWrapperWithAclTest, Test_CheckVdecConfig_Should_Return_Success_When_IsAscend310)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true));
    VdecConfig vdecConfig;
    vdecConfig.outputImageFormat = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vdecConfig.channelId = TWO;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_CheckVdecConfig_Should_Return_Fail_When_IsAscend310B_outputImageFormat_Wrong)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(true));
    VdecConfig vdecConfig;
    vdecConfig.outputImageFormat = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_400;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_DVPP_INVALID_FORMAT);
}

TEST_F(DvppWrapperWithAclTest, Test_CheckVdecConfig_Should_Return_Fail_When_IsAscend310B_YUV_channelId_Wrong)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(true));
    VdecConfig vdecConfig;
    vdecConfig.outputImageFormat = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    vdecConfig.channelId = OVERRANGE + MAX_VDEC_CHANNEL_NUM_310B;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE);
}

TEST_F(DvppWrapperWithAclTest, Test_CheckVdecConfig_Should_Return_Fail_When_IsAscend310B_YVU_channelId_Wrong)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(true));
    VdecConfig vdecConfig;
    vdecConfig.outputImageFormat = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vdecConfig.channelId = OVERRANGE + MAX_VDEC_CHANNEL_NUM_310B;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE);
}

TEST_F(DvppWrapperWithAclTest, Test_CheckVdecConfig_Should_Return_Fail_When_IsAscend310B_RGB_channelId_Wrong)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(true));
    VdecConfig vdecConfig;
    vdecConfig.outputImageFormat = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_RGB_888;
    vdecConfig.channelId = OVERRANGE + MAX_VDEC_CHANNEL_NUM_310B;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE);
}

TEST_F(DvppWrapperWithAclTest, Test_CheckVdecConfig_Should_Return_Fail_When_IsAscend310B_BGR_channelId_Wrong)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(true));
    VdecConfig vdecConfig;
    vdecConfig.outputImageFormat = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_BGR_888;
    vdecConfig.channelId = OVERRANGE + MAX_VDEC_CHANNEL_NUM_310B;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_VDEC_CHANNEL_NUM_OUT_OF_RANGE);
}

TEST_F(DvppWrapperWithAclTest, Test_CheckVdecConfig_Should_Return_Success_When_IsAscend310B_YVU_channelId_Pass)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(1));
    VdecConfig vdecConfig;
    vdecConfig.outputImageFormat = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vdecConfig.channelId = TWO;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_CheckVdecConfig_Should_Return_Success_When_Neither_310_310B)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(1));
    VdecConfig vdecConfig;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_InitVdec_Should_Return_Fail_When_SetVdecChannelDesc_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&aclvdecCreateChannelDesc).times(1).will(returnValue(g_channelDesc));
    VdecConfig vdecConfig;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DeInitVdec_Should_Return_Success)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_join).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DeInitVdec();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_DeInitVdec_Should_Return_Fail_When_aclvdecDestroyChannel_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_join).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvdecDestroyChannel).times(1).will(returnValue(1));
    MOCKER_CPP(&aclvdecDestroyChannelDesc).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    dvppWrapper.dvppWrapperBase_->vdecChannelDesc_ = g_channelDesc;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DeInitVdec();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DeInitVdec_Should_Return_Fail_When_aclvdecDestroyChannelDesc_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_join).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvdecDestroyChannel).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvdecDestroyChannelDesc).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    dvppWrapper.dvppWrapperBase_->vdecChannelDesc_ = g_channelDesc;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DeInitVdec();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DeInitVdec_Should_Return_Success_When_Join_Thread_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_join).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DeInitVdec();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_DeInit_Should_Return_Success_When_All_Nullptr)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_DeInit_Should_Return_Fail_When_All_AclFunc_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(1));
    MOCKER_CPP(&aclrtDestroyStream).times(1).will(returnValue(1));
    MOCKER_CPP(&acldvppDestroyChannel).times(1).will(returnValue(1));
    MOCKER_CPP(&acldvppDestroyChannelDesc).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    dvppWrapper.dvppWrapperBase_->dvppStream_ = g_channelDesc;
    dvppWrapper.dvppWrapperBase_->dvppChannelDesc_ = g_channelDesc;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DeInit();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DeInit_Should_Return_Success_When_All_AclFunc_Pass)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtDestroyStream).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppDestroyChannel).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppDestroyChannelDesc).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    dvppWrapper.dvppWrapperBase_->dvppStream_ = g_channelDesc;
    dvppWrapper.dvppWrapperBase_->dvppChannelDesc_ = g_channelDesc;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DeInit();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppVdec_Should_Return_Fail_When_CheckVdecInput_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppDataInfo inputDataInfo;
    inputDataInfo.outDataSize = 1;
    uint8_t* userData = &g_data2;
    inputDataInfo.outData = userData;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVdec(inputDataInfo, userData);
    EXPECT_EQ(ret, APP_ERR_COMM_OUT_OF_RANGE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppVdec_Should_Return_Fail_When_SetVdecStreamInputDesc_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppDataInfo inputDataInfo;
    inputDataInfo.outDataSize = 1;
    inputDataInfo.width = MIN_VDEC_WIDTH + TWO;
    inputDataInfo.height = MIN_VDEC_HEIGHT + TWO;
    uint8_t* userData = &g_data2;
    inputDataInfo.outData = userData;
    inputDataInfo.data = userData;
    inputDataInfo.dataSize = 1;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVdec(inputDataInfo, userData);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppVdec_Should_Return_Fail_When_GetDvppOutputDataStrideSize_Fail)
{
    acldvppStreamDesc* mockStream = (acldvppStreamDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true));
    MOCKER_CPP(&acldvppCreateStreamDesc).times(1).will(returnValue(mockStream));
    MOCKER_CPP(&acldvppSetStreamDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetStreamDescSize).times(1).will(returnValue(0));
    DvppDataInfo inputDataInfo;
    inputDataInfo.outDataSize = 1;
    inputDataInfo.width = MIN_VDEC_WIDTH + TWO;
    inputDataInfo.height = MIN_VDEC_HEIGHT + TWO;
    uint8_t* userData = &g_data2;
    inputDataInfo.outData = userData;
    inputDataInfo.data = userData;
    inputDataInfo.dataSize = 1;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVdec(inputDataInfo, userData);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppVdecFlush_Should_Return_Success)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetStreamDescEos).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvdecSendFrame).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVdecFlush();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppVdecFlush_Should_Return_Fail_When_Already_Sent_EOS)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetStreamDescEos).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvdecSendFrame).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    APP_ERROR tmp = dvppWrapper.dvppWrapperBase_->DvppVdecFlush();
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVdecFlush();
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppVdecFlush_Should_Return_Fail_When_acldvppSetStreamDescEos_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetStreamDescEos).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVdecFlush();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppVdecFlush_Should_Return_Fail_When_aclvdecSendFrame_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetStreamDescEos).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvdecSendFrame).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVdecFlush();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegDecodeWithAdaptation_Should_Return_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegDecodeWithAdaptation(inputDataInfo, outputDataInfo);
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegConvertColor_Should_Return_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegConvertColor(inputDataInfo, outputDataInfo);
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegDecode_Should_Return_Fail_When_CheckDataSize_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegDecode(inputDataInfo, outputDataInfo);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegDecode_Should_Return_Fail_When_acldvppCreatePicDesc_Fail)
{
    acldvppPicDesc* emptyPtr = nullptr;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(emptyPtr));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegDecode(inputDataInfo, outputDataInfo);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegDecode_Should_Return_Fail_When_acldvppMalloc_Fail)
{
    acldvppPicDesc* mockPtr = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(mockPtr));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegDecode(inputDataInfo, outputDataInfo);
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegDecode_Should_Return_Fail_When_acldvppMalloc_Pass)
{
    acldvppPicDesc* mockPtr = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(mockPtr));
    MOCKER_CPP(&acldvppMalloc).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegDecode(inputDataInfo, outputDataInfo);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegDecode_Should_Return_Fail_When_outputDataInfo_data_Not_Nullptr)
{
    acldvppPicDesc* mockPtr = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(mockPtr));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint8_t* userData = &g_data2;
    outputDataInfo.data = userData;
    outputDataInfo.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_RGBA_8888;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegDecode(inputDataInfo, outputDataInfo);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegDecode_Should_Return_Fail_When_AclDvppSetPicDesc_Format_Not_8888)
{
    acldvppPicDesc* mockPtr = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(mockPtr));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint8_t* userData = &g_data2;
    outputDataInfo.data = userData;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegDecode(inputDataInfo, outputDataInfo);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegDecode_Should_Return_Fail_When_acldvppJpegDecodeAsync_Fail)
{
    acldvppPicDesc* mockPtr = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(mockPtr));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppJpegDecodeAsync).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint8_t* userData = &g_data2;
    outputDataInfo.data = userData;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegDecode(inputDataInfo, outputDataInfo);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegDecode_Should_Return_Fail_When_aclrtSynchronizeStream_Fail)
{
    acldvppPicDesc* mockPtr = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(mockPtr));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppJpegDecodeAsync).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint8_t* userData = &g_data2;
    outputDataInfo.data = userData;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegDecode(inputDataInfo, outputDataInfo);
    EXPECT_EQ(ret, APP_ERR_DVPP_JPEG_DECODE_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegDecode_Should_Return_Success)
{
    acldvppPicDesc* mockPtr = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(mockPtr));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppJpegDecodeAsync).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint8_t* userData = &g_data2;
    outputDataInfo.data = userData;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegDecode(inputDataInfo, outputDataInfo);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppPngDecode_Should_Return_Fail_When_CheckDataSize_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    hi_pixel_format format;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppPngDecode(inputDataInfo, outputDataInfo, format);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppPngDecode_Should_Return_Fail_When_acldvppCreatePicDesc_Is_Nullptr)
{
    acldvppPicDesc* outputDesc = nullptr;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(outputDesc));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    hi_pixel_format format;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppPngDecode(inputDataInfo, outputDataInfo, format);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppPngDecode_Should_Return_Fail_When_acldvppMalloc_Fail)
{
    acldvppPicDesc* outputDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(outputDesc));
    MOCKER_CPP(&acldvppMalloc).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    hi_pixel_format format;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppPngDecode(inputDataInfo, outputDataInfo, format);
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppPngDecode_Should_Return_Fail_When_AclDvppSetPicDesc_Fail)
{
    acldvppPicDesc* outputDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(outputDesc));
    MOCKER_CPP(&acldvppMalloc).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    hi_pixel_format format;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppPngDecode(inputDataInfo, outputDataInfo, format);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppPngDecode_Should_Return_Fail_When_acldvppPngDecodeAsync_Fail)
{
    acldvppPicDesc* outputDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(outputDesc));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppPngDecodeAsync).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint8_t* userData = &g_data2;
    outputDataInfo.data = userData;
    hi_pixel_format format;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppPngDecode(inputDataInfo, outputDataInfo, format);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppPngDecode_Should_Return_Fail_When_aclrtSynchronizeStream_Fail)
{
    acldvppPicDesc* outputDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(outputDesc));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppPngDecodeAsync).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint8_t* userData = &g_data2;
    outputDataInfo.data = userData;
    hi_pixel_format format;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppPngDecode(inputDataInfo, outputDataInfo, format);
    EXPECT_EQ(ret, APP_ERR_DVPP_PNG_DECODE_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppPngDecode_Should_Return_Success)
{
    acldvppPicDesc* outputDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&MemoryHelper::CheckDataSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(outputDesc));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppPngDecodeAsync).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint8_t* userData = &g_data2;
    outputDataInfo.data = userData;
    hi_pixel_format format;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppPngDecode(inputDataInfo, outputDataInfo, format);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegEncode_Should_Return_Fail_When_SetDvppPicDescData_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint32_t encodeLevel = 1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegEncode(inputDataInfo, outputDataInfo, encodeLevel);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegEncode_Should_Return_Fail_When_jpegeConfig_Is_Nullptr)
{
    acldvppJpegeConfig* outputDesc = nullptr;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppCreateJpegeConfig).times(1).will(returnValue(outputDesc));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint32_t encodeLevel = 1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegEncode(inputDataInfo, outputDataInfo, encodeLevel);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegEncode_Should_Return_Fail_When_acldvppSetJpegeConfigLevel_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetJpegeConfigLevel).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint32_t encodeLevel = 1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegEncode(inputDataInfo, outputDataInfo, encodeLevel);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegEncode_Should_Return_Fail_When_acldvppJpegPredictEncSize_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetJpegeConfigLevel).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppJpegPredictEncSize).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint32_t encodeLevel = 1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegEncode(inputDataInfo, outputDataInfo, encodeLevel);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegEncode_Should_Return_Fail_When_acldvppMalloc_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetJpegeConfigLevel).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppJpegPredictEncSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppMalloc).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint32_t encodeLevel = 1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegEncode(inputDataInfo, outputDataInfo, encodeLevel);
    EXPECT_EQ(ret, APP_ERR_ACL_BAD_ALLOC);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegEncode_Should_Return_Fail_When_acldvppJpegEncodeAsync_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetJpegeConfigLevel).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppJpegPredictEncSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppMalloc).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppJpegEncodeAsync).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint32_t encodeLevel = 1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegEncode(inputDataInfo, outputDataInfo, encodeLevel);
    EXPECT_EQ(ret, APP_ERR_DVPP_JPEG_ENCODE_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegEncode_Should_Return_Fail_When_aclrtSynchronizeStream_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetJpegeConfigLevel).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppJpegPredictEncSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppMalloc).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppJpegEncodeAsync).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint32_t encodeLevel = 1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegEncode(inputDataInfo, outputDataInfo, encodeLevel);
    EXPECT_EQ(ret, APP_ERR_DVPP_JPEG_ENCODE_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppJpegEncode_Should_Return_Success)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetJpegeConfigLevel).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppJpegPredictEncSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppMalloc).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppJpegEncodeAsync).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    uint32_t encodeLevel = 1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppJpegEncode(inputDataInfo, outputDataInfo, encodeLevel);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_VpcPrepareCrop_Should_Return_Fail_When_SetDvppPicDescData_Input_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    CropRoiConfig cropConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcPrepareCrop(inputDataInfo, outputDataInfo, cropConfig, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_VpcPrepareCrop_Should_Return_Fail_When_SetDvppPicDescData_Output_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetPicDescData).times(TWO).will(returnValue(0)).then(returnValue(1));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    CropRoiConfig cropConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcPrepareCrop(inputDataInfo, outputDataInfo, cropConfig, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_VpcPrepareCrop_Should_Return_Fail_When_cropRioCfg_Is_Nullptr)
{
    acldvppRoiConfig* outputDesc = nullptr;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetPicDescData).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppCreateRoiConfig).times(1).will(returnValue(outputDesc));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    CropRoiConfig cropConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcPrepareCrop(inputDataInfo, outputDataInfo, cropConfig, stream);
    EXPECT_EQ(ret, APP_ERR_DVPP_RESIZE_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_PrepareCropAndPastePicDescData_Should_Return_Fail_When_Set_Input_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppWrapper dvppWrapper;
    const DvppDataInfo input;
    DvppDataInfo outputDataInfo;
    CropRoiConfig cropConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcCropAndPaste(input,
                                                                  outputDataInfo, cropConfig, cropConfig, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_PrepareCropAndPastePicDescData_Should_Return_Fail_When_Set_Output_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetPicDescData).times(TWO).will(returnValue(0)).then(returnValue(1));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    const DvppDataInfo input;
    DvppDataInfo outputDataInfo;
    CropRoiConfig cropConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcCropAndPaste(input,
                                                                  outputDataInfo, cropConfig, cropConfig, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_VpcCropAndPaste_Should_Return_Fail_When_CheckCropAndPasteParameter_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppSetPicDescData).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(TWO).will(returnValue(0));
    DvppWrapper dvppWrapper;
    const DvppDataInfo input;
    DvppDataInfo outputDataInfo;
    CropRoiConfig cropConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcCropAndPaste(input,
                                                                  outputDataInfo, cropConfig, cropConfig, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperWithAclTest, Test_SetBatchPicDesc_Should_Return_Fail_When_Create_Input_BatchPicDesc_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppWrapper dvppWrapper;
    std::vector<DvppDataInfo> inputDataInfoVec;
    std::vector<DvppDataInfo> outputDataInfoVec;
    std::vector<CropRoiConfig> cropConfigVec;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->BatchCrop(inputDataInfoVec,
                                                            outputDataInfoVec, cropConfigVec, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_INVALID_PARAM);
}

TEST_F(DvppWrapperWithAclTest, Test_PrepareResizePicDescData_Should_Return_Fail_When_SetDvppPicDescData_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppWrapper dvppWrapper;
    MxBase::DvppDataInfo inputDataInfo;
    MxBase::DvppDataInfo outputDataInfo;
    MxBase::ResizeConfig resizeConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcResize(inputDataInfo,
                                                            outputDataInfo, resizeConfig, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_PrepareResizePicDescData_Should_Return_Fail_When_GetDvppOutput_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    MxBase::DvppDataInfo inputDataInfo;
    MxBase::DvppDataInfo outputDataInfo;
    outputDataInfo.format = MxbasePixelFormat::MXBASE_PIXEL_FORMAT_RGB_888;
    MxBase::ResizeConfig resizeConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcResize(inputDataInfo,
                                                            outputDataInfo, resizeConfig, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperWithAclTest, Test_PrepareResizePicDescData_Should_Return_Fail_When_MxbsMalloc_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310P).times(1).will(returnValue(false));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    MxBase::DvppDataInfo inputDataInfo;
    MxBase::DvppDataInfo outputDataInfo;
    MxBase::ResizeConfig resizeConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcResize(inputDataInfo,
                                                            outputDataInfo, resizeConfig, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperWithAclTest, Test_PrepareResizePicDescData_Should_Return_Fail_When_acldvppFree_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310P).times(1).will(returnValue(false));
    MOCKER_CPP(&acldvppSetPicDescData).times(TWO).will(returnValue(0)).then(returnValue(1));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(1).will(returnValue(0));
    MOCKER_CPP(&MemoryHelper::MxbsMalloc).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppFree).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    MxBase::DvppDataInfo inputDataInfo;
    MxBase::DvppDataInfo outputDataInfo;
    MxBase::ResizeConfig resizeConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcResize(inputDataInfo,
                                                            outputDataInfo, resizeConfig, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_PrepareResizePicDescData_Should_Return_Fail_When_SetDvppPicDescData2_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310P).times(1).will(returnValue(false));
    MOCKER_CPP(&acldvppSetPicDescData).times(TWO).will(returnValue(0)).then(returnValue(1));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(1).will(returnValue(0));
    MOCKER_CPP(&MemoryHelper::MxbsMalloc).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppFree).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    MxBase::DvppDataInfo inputDataInfo;
    MxBase::DvppDataInfo outputDataInfo;
    MxBase::ResizeConfig resizeConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcResize(inputDataInfo,
                                                            outputDataInfo, resizeConfig, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_PrepareResizePicDescData_Should_Return_Success)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310P).times(1).will(returnValue(false));
    MOCKER_CPP(&acldvppSetPicDescData).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&MemoryHelper::MxbsMalloc).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppFree).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    MxBase::DvppDataInfo inputDataInfo;
    MxBase::DvppDataInfo outputDataInfo;
    MxBase::ResizeConfig resizeConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcResize(inputDataInfo,
                                                            outputDataInfo, resizeConfig, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_PrepareResizePicDescData_Should_Return_Success2)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310P).times(1).will(returnValue(false));
    MOCKER_CPP(&acldvppSetPicDescData).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppFree).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    MxBase::DvppDataInfo inputDataInfo;
    MxBase::DvppDataInfo outputDataInfo;
    uint8_t* outputDesc = (uint8_t*)&g_data1;
    outputDataInfo.data = outputDesc;
    MxBase::ResizeConfig resizeConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcResize(inputDataInfo,
                                                            outputDataInfo, resizeConfig, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_VpcResize_Should_Return_Fail_When_acldvpp_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310P).times(1).will(returnValue(false));
    MOCKER_CPP(&acldvppSetPicDescData).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppFree).times(1).will(returnValue(0));
    acldvppResizeConfig *tmpResizeConfig = nullptr;
    MOCKER_CPP(&acldvppCreateResizeConfig).times(1).will(returnValue(tmpResizeConfig));
    DvppWrapper dvppWrapper;
    MxBase::DvppDataInfo inputDataInfo;
    MxBase::DvppDataInfo outputDataInfo;
    uint8_t* outputDesc = (uint8_t*)&g_data1;
    outputDataInfo.data = outputDesc;
    MxBase::ResizeConfig resizeConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcResize(inputDataInfo,
                                                            outputDataInfo, resizeConfig, stream);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_POINTER);
}

TEST_F(DvppWrapperWithAclTest, Test_VpcResize_Should_Return_Fail_When_acldvppSetResize_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310P).times(1).will(returnValue(false));
    MOCKER_CPP(&acldvppSetPicDescData).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppFree).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetResizeConfigInterpolation).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    MxBase::DvppDataInfo inputDataInfo;
    MxBase::DvppDataInfo outputDataInfo;
    uint8_t* outputDesc = (uint8_t*)&g_data1;
    outputDataInfo.data = outputDesc;
    MxBase::ResizeConfig resizeConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcResize(inputDataInfo,
                                                            outputDataInfo, resizeConfig, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_VpcResize_Should_Return_Fail_When_aclrtSynchronizeStream_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310P).times(1).will(returnValue(false));
    MOCKER_CPP(&acldvppSetPicDescData).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppFree).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppVpcResizeAsync).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    MxBase::DvppDataInfo inputDataInfo;
    MxBase::DvppDataInfo outputDataInfo;
    uint8_t* outputDesc = (uint8_t*)&g_data1;
    outputDataInfo.data = outputDesc;
    MxBase::ResizeConfig resizeConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcResize(inputDataInfo,
                                                            outputDataInfo, resizeConfig, stream);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_VpcResize_Should_Return_Success)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310P).times(1).will(returnValue(false));
    MOCKER_CPP(&acldvppSetPicDescData).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(TWO).will(returnValue(0));
    MOCKER_CPP(&acldvppVpcResizeAsync).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    MxBase::DvppDataInfo inputDataInfo;
    MxBase::DvppDataInfo outputDataInfo;
    uint8_t* outputDesc = (uint8_t*)&g_data1;
    outputDataInfo.data = outputDesc;
    MxBase::ResizeConfig resizeConfig;
    AscendStream stream;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcResize(inputDataInfo,
                                                            outputDataInfo, resizeConfig, stream);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_VpcPadding_Should_Return_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    DvppDataInfo outputDataInfo;
    MakeBorderConfig makeBorderConfig;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->VpcPadding(inputDataInfo, outputDataInfo, makeBorderConfig);
    EXPECT_EQ(ret, APP_ERR_DVPP_VPC_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_GetPictureDec_Should_Return_Fail_When_Data_Is_Nullptr)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppWrapper dvppWrapper;
    DvppImageInfo imageInfo;
    const aclmdlDataset* myPtr;
    imageInfo.data = myPtr;
    DvppImageOutput imageOutput;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->GetPictureDec(imageInfo, imageOutput);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_GetPictureDec_Should_Return_Fail_When_acldvppJpegGetImageInfo_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppJpegGetImageInfo).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppImageInfo imageInfo;
    imageInfo.pictureType = DvppImageInfo::PictureType::PIXEL_FORMAT_JPEG;
    DvppImageOutput imageOutput;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->GetPictureDec(imageInfo, imageOutput);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_GetPictureDec_Should_Return_Success_When_pictureType_Is_JPEG)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppJpegGetImageInfo).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    DvppImageInfo imageInfo;
    imageInfo.pictureType = DvppImageInfo::PictureType::PIXEL_FORMAT_JPEG;
    DvppImageOutput imageOutput;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->GetPictureDec(imageInfo, imageOutput);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_GetPictureDec_Should_Return_Fail_When_acldvppPngGetImageInfo_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppPngGetImageInfo).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppImageInfo imageInfo;
    imageInfo.pictureType = DvppImageInfo::PictureType::PIXEL_FORMAT_PNG;
    DvppImageOutput imageOutput;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->GetPictureDec(imageInfo, imageOutput);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_GetPictureDec_Should_Return_Success_When_pictureType_Is_PNG)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppPngGetImageInfo).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    DvppImageInfo imageInfo;
    imageInfo.pictureType = DvppImageInfo::PictureType::PIXEL_FORMAT_PNG;
    DvppImageOutput imageOutput;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->GetPictureDec(imageInfo, imageOutput);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_GetPictureDec_Should_Return_Fail_When_pictureType_Is_Wrong)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppWrapper dvppWrapper;
    DvppImageInfo imageInfo;
    imageInfo.pictureType = DvppImageInfo::PictureType::PIXEL_FORMAT_ANY;
    DvppImageOutput imageOutput;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->GetPictureDec(imageInfo, imageOutput);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DeInitVenc_Should_Return_Fail_When_aclvencDestroyChannel_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&aclvencDestroyChannel).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    dvppWrapper.dvppWrapperBase_->vencChannelDesc_ = std::make_shared<uint8_t>(1);
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DeInitVenc();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DeInitVenc_Should_Return_Fail_When_aclrtSynchronizeStream_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&aclvencDestroyChannel).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(1));
    MOCKER_CPP(&aclrtDestroyStream).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    dvppWrapper.dvppWrapperBase_->vencChannelDesc_ = std::make_shared<uint8_t>(1);
    dvppWrapper.dvppWrapperBase_->vencFrameConfig_ = std::make_shared<uint8_t>(1);
    uint8_t* outputDesc = (uint8_t*)&g_data1;
    dvppWrapper.dvppWrapperBase_->vencStream_ = outputDesc;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DeInitVenc();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DeInitVenc_Should_Return_Fail_When_aclrtDestroyStream_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&aclvencDestroyChannel).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtDestroyStream).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    dvppWrapper.dvppWrapperBase_->vencChannelDesc_ = std::make_shared<uint8_t>(1);
    dvppWrapper.dvppWrapperBase_->vencFrameConfig_ = std::make_shared<uint8_t>(1);
    uint8_t* outputDesc = (uint8_t*)&g_data1;
    dvppWrapper.dvppWrapperBase_->vencStream_ = outputDesc;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DeInitVenc();
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_DeInitVenc_Should_Return_Success_When_joinThreadErr_Is_Zero)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&aclvencDestroyChannel).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSynchronizeStream).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtDestroyStream).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_join).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    dvppWrapper.dvppWrapperBase_->vencChannelDesc_ = std::make_shared<uint8_t>(1);
    dvppWrapper.dvppWrapperBase_->vencFrameConfig_ = std::make_shared<uint8_t>(1);
    uint8_t* outputDesc = (uint8_t*)&g_data1;
    dvppWrapper.dvppWrapperBase_->vencStream_ = outputDesc;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DeInitVenc();
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppVenc_Should_Return_Fail_When_handleFunc_Is_Nullptr)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* handleFunc = nullptr;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVenc(inputDataInfo, handleFunc);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppVenc_Should_Return_Fail_When_handleFunc_Not_Nullptr)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t)>* handleFunc =
        (std::function<void(std::shared_ptr<uint8_t>, uint32_t)>*)&g_data1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVenc(inputDataInfo, handleFunc);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppVenc2_Should_Return_Fail_When_dvppPicDesc_Is_Nullptr)
{
    acldvppPicDesc *dvppPicDesc = nullptr;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(dvppPicDesc));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc =
        (std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>*)&g_data1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVenc(inputDataInfo, handleFunc);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DvppWrapperWithAclTest, Test_SetVencPicDesc_Should_Return_Fail_When_acldvppSetPicDescData_Fail)
{
    acldvppPicDesc *dvppPicDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(dvppPicDesc));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(1));
    MOCKER_CPP(&acldvppDestroyPicDesc).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc =
        (std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>*)&g_data1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVenc(inputDataInfo, handleFunc);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_SetVencPicDesc_Should_Return_Fail_When_acldvppSetPicDescSize_Fail)
{
    acldvppPicDesc *dvppPicDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(dvppPicDesc));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(1));
    MOCKER_CPP(&acldvppDestroyPicDesc).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc =
        (std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>*)&g_data1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVenc(inputDataInfo, handleFunc);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_SetVencPicDesc_Should_Return_Fail_When_acldvppSetPicDescFormat_Fail)
{
    acldvppPicDesc *dvppPicDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(dvppPicDesc));
    MOCKER_CPP(&acldvppDestroyPicDesc).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc =
        (std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>*)&g_data1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVenc(inputDataInfo, handleFunc);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_SetVencPicDesc_Should_Return_Fail_When_acldvppSetPicDescWidth_Fail)
{
    acldvppPicDesc *dvppPicDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(dvppPicDesc));
    MOCKER_CPP(&acldvppDestroyPicDesc).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc =
        (std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>*)&g_data1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVenc(inputDataInfo, handleFunc);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_SetVencPicDesc_Should_Return_Fail_When_acldvppSetPicDescHeight_Fail)
{
    acldvppPicDesc *dvppPicDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(dvppPicDesc));
    MOCKER_CPP(&acldvppDestroyPicDesc).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc =
        (std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>*)&g_data1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVenc(inputDataInfo, handleFunc);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_SetVencPicDesc_Should_Return_Fail_When_acldvppSetPicDescWidthStride_Fail)
{
    acldvppPicDesc *dvppPicDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(dvppPicDesc));
    MOCKER_CPP(&acldvppDestroyPicDesc).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc =
        (std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>*)&g_data1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVenc(inputDataInfo, handleFunc);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_SetVencPicDesc_Should_Return_Fail_When_acldvppSetPicDescHeightStride_Fail)
{
    acldvppPicDesc *dvppPicDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(dvppPicDesc));
    MOCKER_CPP(&acldvppDestroyPicDesc).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc =
        (std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>*)&g_data1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVenc(inputDataInfo, handleFunc);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_SetVencPicDesc_Should_Return_Success)
{
    acldvppPicDesc *dvppPicDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(dvppPicDesc));
    MOCKER_CPP(&acldvppDestroyPicDesc).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSendFrame).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc =
        (std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>*)&g_data1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVenc(inputDataInfo, handleFunc);
    EXPECT_EQ(ret, APP_ERR_DVPP_H26X_ENCODE_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_DvppVenc2_Should_Return_Success)
{
    acldvppPicDesc *dvppPicDesc = (acldvppPicDesc*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&acldvppCreatePicDesc).times(1).will(returnValue(dvppPicDesc));
    MOCKER_CPP(&acldvppSetPicDescData).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescSize).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescFormat).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidth).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeight).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescWidthStride).times(1).will(returnValue(0));
    MOCKER_CPP(&acldvppSetPicDescHeightStride).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSendFrame).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    DvppDataInfo inputDataInfo;
    std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>* handleFunc =
        (std::function<void(std::shared_ptr<uint8_t>, uint32_t, void**)>*)&g_data1;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->DvppVenc(inputDataInfo, handleFunc);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(DvppWrapperWithAclTest, Test_ThreadFunc_Should_Return_Nullptr_When_SetDevice_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&DeviceManager::SetDevice).times(1).will(returnValue(1));
    MOCKER_CPP(&aclvdecCreateChannel).times(1).will(returnValue(1));
    VdecConfig vdecConfig;
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVdec(vdecConfig);
    EXPECT_EQ(ret, APP_ERR_ACL_FAILURE);
}

TEST_F(DvppWrapperWithAclTest, Test_EncodeThreadFunc_Should_Return_Fail_When_SetDevice_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true));
    MOCKER_CPP(&DeviceManager::SetDevice).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_InitVenc_Should_Return_Fail_When_pthread_create_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_InitVenc_Should_Return_Fail_When_pthread_setname_np_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_InitVenc_Should_Return_Fail_When_aclvencCreateChannelDesc_Fail)
{
    aclvencChannelDesc* desc = nullptr;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencCreateChannelDesc).times(1).will(returnValue(desc));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_InitVenc_Should_Return_Fail_When_CreateVencChannel_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_CreateVencChannel_Should_Return_Fail_When_aclvencSetChannel_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetChannelDescThreadId).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_CreateVencChannel_Should_Return_Fail_When_aclvencSetChannelDescCallback_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetChannelDescCallback).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_CreateVencChannel_Should_Return_Fail_When_aclvencSetChannelDescEnType_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetChannelDescEnType).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_SetChannelDescPicParam_Return_Fail_When_aclvencSetChannelDescPicFormat_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetChannelDescPicFormat).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_SetChannelDescPicParam_Return_Fail_When_aclvencSetChannelDescPicWidth_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetChannelDescPicWidth).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_SetChannelDescPicParam_Return_Fail_When_aclvencSetChannelDescPicHeight_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetChannelDescPicHeight).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_SetChannelDescParam_Return_Fail_When_aclvencSetChannelDescKeyFrame_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetChannelDescKeyFrameInterval).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_SetChannelDescParam_Should_Return_Fail_When_aclvencSetChannelDescParam1_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetChannelDescParam).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_SetChannelDescParam_Should_Return_Fail_When_aclvencSetChannelDescParam2_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetChannelDescParam).times(TWO).will(returnValue(0)).then(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_SetChannelDescParam_Should_Return_Fail_When_aclvencSetChannelDescParam3_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetChannelDescParam).times(THREE).will(returnValue(0)).then(returnValue(0)).then(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_SetChannelDescParam_Should_Return_Fail_When_IsAscend310_And_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetChannelDescParam).times(FOUR).will(returnValue(0))
        .then(returnValue(0)).then(returnValue(0)).then(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_SetChannelDescParam_Should_Return_Fail_When_IsAscend310B_And_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(true));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetChannelDescParam).times(FOUR).will(returnValue(0))
        .then(returnValue(0)).then(returnValue(0)).then(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_SetChannelDescParam_Should_Return_Success_When_Neither_IsAscend310B)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencCreateChannel).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_CreateVencChannel_Should_Return_Success)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencCreateChannel).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtCreateStream).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_CreateVencStream_Should_Return_Fail_When_aclrtSubscribeReport_Fail)
{
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencCreateChannel).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtCreateStream).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSubscribeReport).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_CreateVencStream_Should_Return_Fail_When_aclvencCreateFrameConfig_Fail)
{
    aclvencFrameConfig* conf = nullptr;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencCreateChannel).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtCreateStream).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSubscribeReport).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencCreateFrameConfig).times(1).will(returnValue(conf));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_CreateVencStream_Should_Return_Fail_When_aclvencSetFrameConfigForceIFrame_Fail)
{
    aclvencFrameConfig* conf = (aclvencFrameConfig*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencCreateChannel).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtCreateStream).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSubscribeReport).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencCreateFrameConfig).times(1).will(returnValue(conf));
    MOCKER_CPP(&aclvencDestroyFrameConfig).times(1).will(returnValue(1));
    MOCKER_CPP(&aclvencSetFrameConfigForceIFrame).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_CreateVencStream_Should_Return_Fail_When_aclvencSetFrameConfigEos_Fail)
{
    aclvencFrameConfig* conf = (aclvencFrameConfig*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencCreateChannel).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtCreateStream).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSubscribeReport).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencCreateFrameConfig).times(1).will(returnValue(conf));
    MOCKER_CPP(&aclvencDestroyFrameConfig).times(1).will(returnValue(1));
    MOCKER_CPP(&aclvencSetFrameConfigForceIFrame).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetFrameConfigEos).times(1).will(returnValue(1));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_COMM_INIT_FAIL);
}

TEST_F(DvppWrapperWithAclTest, Test_InitVenc_Should_Return_Success)
{
    aclvencFrameConfig* conf = (aclvencFrameConfig*)&g_data1;
    MOCKER_CPP(&DeviceManager::IsAscend310).times(TWO).will(returnValue(true)).then(returnValue(false));
    MOCKER_CPP(&DeviceManager::IsAscend310B).times(1).will(returnValue(false));
    MOCKER_CPP(&pthread_create).times(1).will(returnValue(0));
    MOCKER_CPP(&pthread_setname_np).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencCreateChannel).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtCreateStream).times(1).will(returnValue(0));
    MOCKER_CPP(&aclrtSubscribeReport).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencCreateFrameConfig).times(1).will(returnValue(conf));
    MOCKER_CPP(&aclvencDestroyFrameConfig).times(1).will(returnValue(1));
    MOCKER_CPP(&aclvencSetFrameConfigForceIFrame).times(1).will(returnValue(0));
    MOCKER_CPP(&aclvencSetFrameConfigEos).times(1).will(returnValue(0));
    DvppWrapper dvppWrapper;
    APP_ERROR ret = dvppWrapper.dvppWrapperBase_->InitVenc();
    EXPECT_EQ(ret, APP_ERR_OK);
}

}

int main(int argc, char *argv[])
{
    MxInit();
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    MxDeInit();
    return ret;
}
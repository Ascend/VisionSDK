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
 * Description: MxpiBufferDumpTest.
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
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"
#undef private
#undef protected

#include <google/protobuf/util/json_util.h>
#include <absl/strings/string_view.h>
#include <absl/status/status.h>
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/base/MxGstBase.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/Utils/FileUtils.h"

using namespace MxTools;
using namespace MxBase;

namespace {
constexpr int THREE_TIMES = 3;
constexpr int INIT_DEVICEID = -1;
constexpr int INIT_DATASIZE = 1;
const std::string TMP_PATH = "./";
MxTools::InputParam g_bufferParam = {"", INIT_DEVICEID, INIT_DATASIZE};
class MxpiBufferDumpTest : public testing::Test {
public:
    virtual void TearDown()
    {
        // clear mock
        GlobalMockObject::verify();
    }
};

TEST_F(MxpiBufferDumpTest, Test_MxpiBufferDump_DoLoad_Should_Fail_When_Path_Invalid)
{
    MOCKER_CPP(&MxBase::FileUtils::RegularFilePath).times(1).will(returnValue(false));
    MxpiBufferDump bufferDump;
    MxTools::MxpiBuffer *ret = bufferDump.DoLoad(TMP_PATH, 0);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(MxpiBufferDumpTest, Test_MxpiBufferDump_DoLoad_Should_Fail_When_File_Invalid)
{
    MOCKER_CPP(&MxBase::FileUtils::RegularFilePath).times(1).will(returnValue(true));
    MOCKER_CPP(&MxBase::FileUtils::IsFileValid).times(1).will(returnValue(false));
    MxpiBufferDump bufferDump;
    MxTools::MxpiBuffer *ret = bufferDump.DoLoad(TMP_PATH, 0);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(MxpiBufferDumpTest, Test_MxpiBufferDump_DoLoad_Should_Fail_When_Protobuf_To_Json_Fail)
{
    MOCKER_CPP(&MxBase::FileUtils::RegularFilePath).times(1).will(returnValue(true));
    MOCKER_CPP(&MxBase::FileUtils::IsFileValid).times(1).will(returnValue(true));
    const std::string retStr = "";
    MOCKER_CPP(&MxBase::FileUtils::ReadFileContent).times(1).will(returnValue(retStr));
    MxpiBufferDump bufferDump;
    MxTools::MxpiBuffer *ret = bufferDump.DoLoad(TMP_PATH, 0);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(MxpiBufferDumpTest, Test_MxpiBufferDump_DoLoad_Should_Fail_When_Create_Buffer_Fail)
{
    MOCKER_CPP(&MxBase::FileUtils::RegularFilePath).times(1).will(returnValue(true));
    MOCKER_CPP(&MxBase::FileUtils::IsFileValid).times(1).will(returnValue(true));
    const std::string retStr = "";
    MOCKER_CPP(&MxBase::FileUtils::ReadFileContent).times(1).will(returnValue(retStr));
    MxpiBufferDump bufferDump;
    absl::Status retVal = absl::OkStatus();
    MOCKER_CPP(&google::protobuf::json::JsonStringToMessage,
        absl::Status(absl::string_view, google::protobuf::Message *, const google::protobuf::json::ParseOptions &))
        .times(1)
        .will(returnValue(retVal));
    MxpiBuffer *retPtr = nullptr;
    MOCKER_CPP(&MxpiBufferManager::CreateHostBufferAndCopyData).times(1).will(returnValue(retPtr));
    MxTools::MxpiBuffer *ret = bufferDump.DoLoad(TMP_PATH, 0);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(MxpiBufferDumpTest, Test_MxpiBufferDump_DoLoad_Should_Fail_When_Build_Data_Fail)
{
    MOCKER_CPP(&MxBase::FileUtils::RegularFilePath).times(1).will(returnValue(true));
    MOCKER_CPP(&MxBase::FileUtils::IsFileValid).times(1).will(returnValue(true));
    const std::string retStr = "";
    MOCKER_CPP(&MxBase::FileUtils::ReadFileContent).times(1).will(returnValue(retStr));
    MxpiBufferDump bufferDump;
    absl::Status retVal = absl::OkStatus();
    MOCKER_CPP(&google::protobuf::json::JsonStringToMessage,
        absl::Status(absl::string_view, google::protobuf::Message *, const google::protobuf::json::ParseOptions &))
        .times(1)
        .will(returnValue(retVal));
    MxpiBuffer *retPtr = new MxpiBuffer();
    MOCKER_CPP(&MxpiBufferManager::CreateHostBufferAndCopyData).times(1).will(returnValue(retPtr));
    MOCKER_CPP(&MxpiBufferDump::BuildMetaData).times(1).will(returnValue(1));
    MOCKER_CPP(&MxpiMetadataManager::RemoveMetadata).times(1).will(returnValue(0));
    MxTools::MxpiBuffer *ret = bufferDump.DoLoad(TMP_PATH, 0);
    EXPECT_EQ(ret, nullptr);
}

TEST_F(MxpiBufferDumpTest, Test_MxpiBufferDump_BuildStringData_Should_Success_With_Valid_Input)
{
    MxpiBuffer buffer;
    MxTools::MxpiMetadataManager manger(buffer);
    MxTools::MetaData data;
    MOCKER_CPP(&MxpiMetadataManager::AddProtoMetadata).times(1).will(returnValue(0));
    MxpiBufferDump bufferDump;
    APP_ERROR ret = bufferDump.BuildStringData(&manger, data);
    std::shared_ptr<MxpiTensorPackageList> list = std::make_shared<MxpiTensorPackageList>();
    std::shared_ptr<void> voidPtr = std::static_pointer_cast<void>(list);
    std::pair<const std::string, std::shared_ptr<void>> item = {"test", voidPtr};
    bufferDump.BuildTensorPackageList(item);
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxpiBufferDumpTest, Test_MxpiBufferDump_BuildTensorPackage_Should_Fail_When_Protobuf_To_Json_Fail)
{
    MxpiBuffer buffer;
    MxTools::MxpiMetadataManager manger(buffer);
    MxTools::MetaData data;
    MxpiBufferDump bufferDump;
    absl::Status retVal = absl::CancelledError();
    MOCKER_CPP(&google::protobuf::json::JsonStringToMessage,
        absl::Status(absl::string_view, google::protobuf::Message *, const google::protobuf::json::ParseOptions &))
        .times(1)
        .will(returnValue(retVal));
    APP_ERROR ret = bufferDump.BuildTensorPackage(&manger, data, 0);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxpiBufferDumpTest, Test_MxpiBufferDump_BuildVisionData_Should_Fail_When_Protobuf_To_Json_Fail)
{
    MxpiBuffer buffer;
    MxTools::MxpiMetadataManager manger(buffer);
    MxTools::MetaData data;
    MxpiBufferDump bufferDump;
    absl::Status retVal = absl::CancelledError();
    MOCKER_CPP(&google::protobuf::json::JsonStringToMessage,
        absl::Status(absl::string_view, google::protobuf::Message *, const google::protobuf::json::ParseOptions &))
        .times(1)
        .will(returnValue(retVal));
    APP_ERROR ret = bufferDump.BuildVisionData(&manger, data, 0);
    EXPECT_EQ(ret, APP_ERR_COMM_FAILURE);
}

TEST_F(MxpiBufferDumpTest, Test_MxpiBufferDump_HandleProtoDataType_Should_Fail_When_HasInvalidChar)
{
    MOCKER_CPP(&StringUtils::HasInvalidChar, bool(const std::string &)).times(1).will(returnValue(true));
    std::shared_ptr<MxpiTensorPackageList> list = std::make_shared<MxpiTensorPackageList>();
    std::shared_ptr<void> voidPtr = std::static_pointer_cast<void>(list);
    std::pair<const std::string, std::shared_ptr<void>> item = {"test1", voidPtr};
    const std::string metaKey = "test2";
    MetaData dumpMetaData;
    MxpiBufferDump bufferDump;
    APP_ERROR ret = bufferDump.HandleProtoDataType(item, metaKey, &dumpMetaData, true);
    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(MxpiBufferDumpTest, Test_MxpiBufferDump_HandleProtoDataType_Should_Fail_When_Nullptr)
{
    MOCKER_CPP(&StringUtils::HasInvalidChar, bool(const std::string &)).times(1).will(returnValue(false));
    std::pair<const std::string, std::shared_ptr<void>> item = {"test1", nullptr};
    const std::string metaKey = "test2";
    MetaData dumpMetaData;
    MxpiBufferDump bufferDump;
    APP_ERROR ret = bufferDump.HandleProtoDataType(item, metaKey, &dumpMetaData, true);
    EXPECT_EQ(ret, APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL);
}

TEST_F(MxpiBufferDumpTest, Test_MxpiBufferDump_HandleProtoDataType_Should_Fail_When_Protobuf_To_Json_Fail)
{
    MOCKER_CPP(&StringUtils::HasInvalidChar, bool(const std::string &)).times(2).will(returnValue(false));
    std::shared_ptr<MxpiTensorPackageList> list = std::make_shared<MxpiTensorPackageList>();
    std::pair<const std::string, std::shared_ptr<void>> item = {"test3", list};
    const std::string metaKey = "test3";
    MetaData dumpMetaData;
    MxpiBufferDump bufferDump;
    absl::Status retVal = absl::CancelledError();
    MOCKER_CPP(&google::protobuf::json::MessageToJsonString,
        absl::Status(const google::protobuf::Message &message,
            std::string *output,
            const google::protobuf::json::PrintOptions &options))
        .times(1)
        .will(returnValue(retVal));
    APP_ERROR ret = bufferDump.HandleProtoDataType(item, metaKey, &dumpMetaData, false);
    EXPECT_EQ(ret, APP_ERR_PLUGIN_TOOLKIT_MESSAGE_TO_STRING_FAILED);
}

TEST_F(MxpiBufferDumpTest, Test_MxpiBufferDump_IsMetaDataKeyFilter_Should_Fail_With_FilterKeys_Not_Empty)
{
    const std::vector<std::string> filterKeys = {"test1", "test2"};
    const std::vector<std::string> requiredKeys = {};
    const std::string metaKey = "test";
    MxpiBufferDump bufferDump;
    bool ret = bufferDump.IsMetaDataKeyFilter(filterKeys, requiredKeys, metaKey);
    EXPECT_EQ(ret, false);
}

TEST_F(MxpiBufferDumpTest, Test_MxpiBufferDump_IsMetaDataKeyFilter_Should_Fail_With_RequiredKeys_Not_Empty)
{
    const std::string retStr = "test";
    MOCKER_CPP(&StringUtils::Trim).times(1).will(returnValue(retStr));
    const std::vector<std::string> filterKeys = {};
    const std::vector<std::string> requiredKeys = {"test", "test2"};
    const std::string metaKey = "test";
    MxpiBufferDump bufferDump;
    bool ret = bufferDump.IsMetaDataKeyFilter(filterKeys, requiredKeys, metaKey);
    EXPECT_EQ(ret, false);
}
}  // namespace

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    gst_init(&argc, &argv);

    return RUN_ALL_TESTS();
}
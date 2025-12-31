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
 * Description: MxpiPictureResizeTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */
 
#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDumpData.pb.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxpiCommon/DumpDataHelper.h"

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;

namespace {
const int TIME_SLEEP = 2;
    class MxpiPictureResizeTest : public testing::Test {
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

TEST_F(MxpiPictureResizeTest, TestGetData)
{
    LogInfo << "********case PictureResize TestGetData********";
    std::string catImage = FileUtils::ReadFileContent("test.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("getdata.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) catImage.c_str();
    mxstDataInput.dataSize = catImage.size();

    std::string streamName = "GetDataPipeline";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();

    std::string input = FileUtils::ReadFileContent("input.output");
    EXPECT_TRUE(input.size() > 0);
    MxTools::MxpiDumpData dumpData;
    auto status = google::protobuf::util::JsonStringToMessage(input, &dumpData);
    EXPECT_TRUE(status.ok());
    bool flag = false;
    for (auto& metaData : dumpData.metadata()) {
        LogInfo << "key = " << metaData.key();
        if (metaData.key() == "mxpi_imagedecoder0") {
            flag = true;
            break;
        }
    }
    EXPECT_TRUE(flag);
    std::string  result = FileUtils::ReadFileContent("result.output");
    EXPECT_TRUE(result.size() > 0);
    status = google::protobuf::util::JsonStringToMessage(result, &dumpData);
    EXPECT_TRUE(status.ok());
    LogInfo << "metadata size = " << dumpData.metadata_size();
    flag = false;
    for (auto& metaData : dumpData.metadata()) {
        LogInfo << "key = " << metaData.key();
        if (metaData.key() == "mxpi_imageresize0") {
            flag = true;
            break;
        }
    }
    EXPECT_TRUE(flag);
}

TEST_F(MxpiPictureResizeTest, DISABLED_TestPictureResize)
{
    LogInfo << "********case  TestPictureResize********";
    std::string input = FileUtils::ReadFileContent("input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("pictureresize.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "pictureresize";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("pictureresize.output");
    std::string test = FileUtils::ReadFileContent("result.output");

    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiPictureResizeTest, TestOpencvPictureResize)
{
    LogInfo << "********case  TestOpencvPictureResize********";
    std::string input = FileUtils::ReadFileContent("opencv_input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("opencv_pictureresize.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "pictureresize";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("opencv_pictureresize.output");
    std::string test = FileUtils::ReadFileContent("opencv_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiPictureResizeTest, TestOpencvPictureLongfitResize)
{
    LogInfo << "********case  TestOpencvPictureLongfitResize********";
    std::string input = FileUtils::ReadFileContent("opencv_input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("opencv_longfit_pictureresize.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "pictureresize";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("opencv_longfit_pictureresize.output");
    std::string test = FileUtils::ReadFileContent("opencv_longfit_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiPictureResizeTest, TestOpencvPictureShortfitResize)
{
    LogInfo << "********case  TestOpencvPictureShortfitResize********";
    std::string input = FileUtils::ReadFileContent("opencv_input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("opencv_shortfit_pictureresize.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "pictureresize";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("opencv_shortfit_pictureresize.output");
    std::string test = FileUtils::ReadFileContent("opencv_shortfit_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiPictureResizeTest, TestOpencvPictureRescale)
{
    LogInfo << "********case  TestOpencvPictureRescale********";
    std::string input = FileUtils::ReadFileContent("opencv_input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("opencv_rescale_pictureresize.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "pictureresize";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("opencv_rescale_pictureresize.output");
    std::string test = FileUtils::ReadFileContent("opencv_rescale_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiPictureResizeTest, TestOpencvPictureRescaleDouble)
{
    LogInfo << "********case  TestOpencvPictureRescale********";
    std::string input = FileUtils::ReadFileContent("opencv_input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret =
            mxStreamManager.CreateMultipleStreamsFromFile("opencv_rescale_double_pictureresize.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "pictureresize";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("opencv_rescale_double_pictureresize.output");
    std::string test = FileUtils::ReadFileContent("opencv_rescale_double_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiPictureResizeTest, TestOpencvPicturePadding)
{
    LogInfo << "********case  TestOpencvPicturePadding********";
    std::string input = FileUtils::ReadFileContent("opencv_input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("opencv_padding_pictureresize.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "pictureresize";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
    std::string result = FileUtils::ReadFileContent("opencv_padding_pictureresize.output");
    std::string test = FileUtils::ReadFileContent("opencv_padding_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
}

TEST_F(MxpiPictureResizeTest, TestOpencvPicturePaddingAround)
{
    LogInfo << "********case  TestOpencvPicturePadding********";
    std::string input = FileUtils::ReadFileContent("opencv_input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("opencv_padding_pictureresize_around.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "pictureresize";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    std::string result = FileUtils::ReadFileContent("opencv_padding_pictureresize_around.output");
    std::string test = FileUtils::ReadFileContent("opencv_padding_around_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
    mxStreamManager.DestroyAllStreams();
}

TEST_F(MxpiPictureResizeTest, TestDvppResizeAndCropWithPaddingAround)
{
    LogInfo << "********case  TestDvppResizeAndCropWithPaddingAround********";
    std::string image = FileUtils::ReadFileContent("val_2017_173057.jpg");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("dvpp_padding_pictureresize_around.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) image.c_str();
    mxstDataInput.dataSize = image.size();

    std::string streamName = "detection";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    mxStreamManager.DestroyAllStreams();
}

TEST_F(MxpiPictureResizeTest, TestOpencvPicturePaddingRemove)
{
    LogInfo << "********case  TestOpencvPicturePadding********";
    std::string input = FileUtils::ReadFileContent("opencv_input.output");
    MxStreamManager mxStreamManager;
    mxStreamManager.InitManager();
    APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("opencv_padding_pictureresize_remove.pipeline");
    EXPECT_EQ(ret, APP_ERR_OK);

    MxstDataInput mxstDataInput;
    mxstDataInput.dataPtr = (uint32_t*) input.c_str();
    mxstDataInput.dataSize = input.size();

    std::string streamName = "pictureresize";
    mxStreamManager.SendData(streamName, 0, mxstDataInput);
    sleep(TIME_SLEEP);
    std::string result = FileUtils::ReadFileContent("opencv_padding_pictureresize_remove.output");
    std::string test = FileUtils::ReadFileContent("opencv_padding_result.output");
    EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
    mxStreamManager.DestroyAllStreams();
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
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
 * Description: MultiBatchsizeTest.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <iostream>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>
#include <MxBase/Utils/FileUtils.h>
#include <MxTools/Proto/MxpiDataType.pb.h>
#include "MxBase/Log/Log.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxStream/StreamManager/MxStreamManager.h"
#include "MxpiCommon/DumpDataHelper.h"

using namespace MxStream;
using namespace MxTools;
using namespace MxBase;
using namespace MxPlugins;

namespace {
    class MxpiTextGenerationPostProcessorTest : public testing::Test {
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

    void GetProtobufData(const MxStream::MxstDataInput& dataInput, std::vector<MxStream::MxstProtobufIn>& dataBufferVec)
    { 
        auto tensorPackageList = std::make_shared<MxTools::MxpiTensorPackageList>();
        auto tensorPackage = tensorPackageList->add_tensorpackagevec();
        auto tensorVec = tensorPackage->add_tensorvec();
        tensorVec->set_tensordataptr((uint64_t)dataInput.dataPtr);
        tensorVec->set_tensordatasize(dataInput.dataSize);
        tensorVec->set_tensordatatype(MxBase::TENSOR_DTYPE_INT32);
        tensorVec->set_memtype(MxTools::MXPI_MEMORY_HOST_NEW);
        tensorVec->set_deviceid(0);
        tensorVec->add_tensorshape(1);
        const uint32_t inputLength = 128;
        tensorVec->add_tensorshape(inputLength);

        MxStream::MxstProtobufIn dataBuffer;
        dataBuffer.key = "appsrc0";
        dataBuffer.messagePtr = std::static_pointer_cast<google::protobuf::Message>(tensorPackageList);
        dataBufferVec.push_back(dataBuffer);
    }

    TEST_F(MxpiTextGenerationPostProcessorTest, TestCrnn)
    {
        LogInfo << "********case MxpiTextGenerationPostProcessorTest TestCrnn********";
        std::string input = FileUtils::ReadFileContent("word.jpg");
        MxStreamManager mxStreamManager;
        mxStreamManager.InitManager();
        APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("CrnnPostProcess.pipeline");
        EXPECT_EQ(ret, APP_ERR_OK);

        MxstDataInput mxstDataInput;
        mxstDataInput.dataPtr = (uint32_t*) input.c_str();
        mxstDataInput.dataSize = input.size();

        std::string streamName = "crnnpostprocess";
        mxStreamManager.SendData(streamName, 0, mxstDataInput);
        sleep(2);
        mxStreamManager.StopStream(streamName);
        std::string result = FileUtils::ReadFileContent("crnnpostprocess.output");
        std::string test = FileUtils::ReadFileContent("crnnresult.output");

        EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
    }

    TEST_F(MxpiTextGenerationPostProcessorTest, DISABLED_TestTransformer)
    {
        LogInfo << "********case MxpiTextGenerationPostProcessorTest TestTransformer********";
        std::string input = FileUtils::ReadFileContent("../models/transformer/length_128_input_0.bin");
        MxStreamManager mxStreamManager;
        mxStreamManager.InitManager();
        APP_ERROR ret = mxStreamManager.CreateMultipleStreamsFromFile("TransformerPostProcess.pipeline");
        EXPECT_EQ(ret, APP_ERR_OK);

        MxstDataInput mxstDataInput;
        mxstDataInput.dataPtr = (uint32_t*) input.c_str();
        mxstDataInput.dataSize = input.size();

        std::vector<MxStream::MxstProtobufIn> dataBufferVec;
        GetProtobufData(mxstDataInput, dataBufferVec);

        std::string streamName = "transformerpostprocess";
        mxStreamManager.SendProtobuf(streamName, 0, dataBufferVec);
        sleep(2);
        mxStreamManager.DestroyAllStreams();
        std::string result = FileUtils::ReadFileContent("transformerpostprocess.output");
        std::string test = FileUtils::ReadFileContent("transformerresult.output");

        EXPECT_TRUE(MxPlugins::DumpDataHelper::CompareDumpData(result, test));
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

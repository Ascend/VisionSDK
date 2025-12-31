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
 * Description: stream manager test.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <cstring>
#include <iostream>
#include <gtest/gtest.h>
#include <memory>
#include <google/protobuf/message.h>

#include "MxBase/Log/Log.h"
#include "MxStream/Packet/DataTransform.h"
#include "MxStream/Packet/PacketDataType.h"
#include "MxTools/Proto/MxpiDataType.pb.h"


using namespace MxStream;
using namespace MxTools;

namespace {
class DataTransformTest : public testing::Test {
public:
    void SetUp() override
    {
        LogDebug << "SetUp()";
    }

    void TearDown() override
    {
        LogDebug << "TearDown()" << std::endl;
    }
};

TEST_F(DataTransformTest, MxVisionListTest)
{
    MxVisionList packet;
    std::shared_ptr<google::protobuf::Message> messagePtr = std::make_shared<MxpiVisionList>();
    VisionListTransform(packet, messagePtr);
}

TEST_F(DataTransformTest, ClassListTest)
{
    MxClassList packet;
    std::shared_ptr<google::protobuf::Message> messagePtr = std::make_shared<MxpiClassList>();
    ClassListTransform(packet, messagePtr);
}

TEST_F(DataTransformTest, ImageMaskListTest)
{
    MxImageMaskList packet;
    std::shared_ptr<google::protobuf::Message> messagePtr = std::make_shared<MxpiImageMaskList>();
    ImageMaskListTransform(packet, messagePtr);
}

TEST_F(DataTransformTest, ObjectListTest)
{
    MxObjectList packet;
    std::shared_ptr<google::protobuf::Message> messagePtr = std::make_shared<MxpiObjectList>();
    ObjectListTransform(packet, messagePtr);
}

TEST_F(DataTransformTest, TensorPackageListTest)
{
    MxTensorPackageList packet;
    std::shared_ptr<google::protobuf::Message> messagePtr = std::make_shared<MxpiTensorPackageList>();
    TensorPackageListTransform(packet, messagePtr);
}

TEST_F(DataTransformTest, PoseListTest)
{
    MxPoseList packet;
    std::shared_ptr<google::protobuf::Message> messagePtr = std::make_shared<MxpiPoseList>();
    PoseListTransform(packet, messagePtr);
}

TEST_F(DataTransformTest, TextObjectListTest)
{
    MxTextObjectList packet;
    std::shared_ptr<google::protobuf::Message> messagePtr = std::make_shared<MxpiTextObjectList>();
    TextObjectListTransform(packet, messagePtr);
}

TEST_F(DataTransformTest, TextsInfoListTransformTest)
{
    MxTextsInfoList packet;
    std::shared_ptr<google::protobuf::Message> messagePtr = std::make_shared<MxpiTextsInfoList>();
    TextsInfoListTransform(packet, messagePtr);
}
}
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


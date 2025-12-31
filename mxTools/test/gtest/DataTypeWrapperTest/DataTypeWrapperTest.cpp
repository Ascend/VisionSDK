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
 * Description: DataTypeWrapperTest.
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
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeConverter.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

using namespace MxTools;
using namespace MxBase;

namespace {
constexpr int HUNDRED = 100;
constexpr int ZERO = 0;
constexpr float ZERO_F = 0;
class DataTypeWrapperTest : public testing::Test {
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

TEST_F(DataTypeWrapperTest, Test_ConstructProtobuf_Object_Should_Success)
{
    MxpiObjectList objectList;
    std::vector<std::vector<uint8_t>> mask = {{1, 2, 3, 4, 5, 6, 7}};
    auto newObject = objectList.add_objectvec();
    newObject->set_x0(ZERO);
    newObject->set_x1(HUNDRED);
    newObject->set_y0(ZERO);
    newObject->set_y1(HUNDRED);
    auto newClass = newObject->add_classvec();
    newClass->set_classid(ZERO);
    newClass->set_classname("background");
    newClass->set_confidence(1.0);
    auto newHeader = newObject->add_headervec();
    newHeader->set_datasource("test");
    auto object = ObjectInfo{};
    object.x1 = HUNDRED;
    object.y1 = HUNDRED;
    object.confidence = 1.0;
    object.className = "background";
    object.mask = mask;
    std::vector<std::vector<MxBase::ObjectInfo>> objectInfos = {{object}};
    auto protobuf = ConstructProtobuf(objectInfos, "test");
    EXPECT_NE(objectList.DebugString(), protobuf->DebugString());
}

TEST_F(DataTypeWrapperTest, Test_ConstructProtobuf_Class_Should_Success)
{
    MxpiClassList classList;
    auto classInfo = ClassInfo{0, 1.0, "background"};
    std::vector<std::vector<MxBase::ClassInfo>> classInfos = {{classInfo}};
    auto protobuf = ConstructProtobuf(classInfos, "test");
    EXPECT_NE(classList.DebugString(), protobuf->DebugString());
}

TEST_F(DataTypeWrapperTest, Test_ConstructProtobuf_Seg_Should_Success)
{
    MxpiImageMaskList maskList;
    std::vector<std::vector<int>> pixels = {{1, 2, 3}, {4, 5, 6}};
    std::vector<std::string> labelMap = {"object1", "object2"};
    auto semanticInfo = SemanticSegInfo{pixels, labelMap};
    std::vector<MxBase::SemanticSegInfo> semanticInfos = {semanticInfo};
    auto protobuf = ConstructProtobuf(semanticInfos, "test");
    EXPECT_NE(maskList.DebugString(), protobuf->DebugString());
}

TEST_F(DataTypeWrapperTest, Test_ConstructProtobuf_Text_Should_Success)
{
    MxpiTextsInfoList textInfoList;
    TextsInfo textsInfo;
    std::vector<MxBase::TextsInfo> textInfos = {textsInfo};
    auto protobuf = ConstructProtobuf(textInfos, "test");
    EXPECT_NE(textInfoList.DebugString(), protobuf->DebugString());
}

TEST_F(DataTypeWrapperTest, Test_ConstructProtobuf_Pose_Should_Success)
{
    MxpiPoseList poseList;
    std::map<int, std::vector<float>> keyPointMap = {
        {0, {1.1, 1.2}},
        {1, {1.1, 1.2}},
    };

    std::map<int, float> scoreMap = {
        {0, 0.8},
        {1, 0.2},
    };
    float score = 0.8;
    KeyPointDetectionInfo keyPointInfo = {keyPointMap, scoreMap, score};
    std::vector<std::vector<MxBase::KeyPointDetectionInfo>> keyPointInfos = {{keyPointInfo}};
    auto protobuf = ConstructProtobuf(keyPointInfos, "test");
    EXPECT_NE(poseList.DebugString(), protobuf->DebugString());
}

TEST_F(DataTypeWrapperTest, Test_ConstructProtobuf_TextObject_Should_Success)
{
    MxpiPoseList poseList;
    TextObjectInfo textObjectInfo;
    std::vector<std::vector<MxBase::TextObjectInfo>> textObjectInfos = {{textObjectInfo}};
    auto protobuf = ConstructProtobuf(textObjectInfos, "test");
    EXPECT_NE(poseList.DebugString(), protobuf->DebugString());
}

TEST_F(DataTypeWrapperTest, Test_StackMxpiVisionPreProcess_Should_Success)
{
    std::string elementName = "test";

    MxTools::MxpiVisionInfo srcMxpiVisionInfo;
    MxTools::MxpiVisionInfo dstMxpiVisionInfo;
    MxBase::CropResizePasteConfig cropResizePasteConfig = {ZERO, ZERO, ZERO, ZERO,
                                                           ZERO, ZERO, ZERO, ZERO,
                                                           ZERO};
    MxTools::MxpiVisionPreProcess* preprocessInfo = srcMxpiVisionInfo.add_preprocessinfo();
    preprocessInfo->set_widthsrc(ZERO);
    preprocessInfo->set_heightsrc(ZERO);
    preprocessInfo->set_cropleft(ZERO);
    preprocessInfo->set_cropright(ZERO);
    preprocessInfo->set_croptop(ZERO);
    preprocessInfo->set_cropbottom(ZERO);
    preprocessInfo->set_pasteleft(ZERO);
    preprocessInfo->set_pasteright(ZERO);
    preprocessInfo->set_pastetop(ZERO);
    preprocessInfo->set_pastebottom(ZERO);
    preprocessInfo->set_interpolation(ZERO);
    preprocessInfo->set_interpolation(ZERO);
    preprocessInfo->set_elementname(elementName);

    srcMxpiVisionInfo.set_format(ZERO);
    srcMxpiVisionInfo.set_width(HUNDRED);
    srcMxpiVisionInfo.set_height(ZERO);
    srcMxpiVisionInfo.set_widthaligned(ZERO);
    srcMxpiVisionInfo.set_heightaligned(ZERO);
    srcMxpiVisionInfo.set_resizetype(ZERO);
    srcMxpiVisionInfo.set_keepaspectratioscaling(ZERO_F);

    StackMxpiVisionPreProcess(dstMxpiVisionInfo, srcMxpiVisionInfo, cropResizePasteConfig, elementName);

    EXPECT_EQ(preprocessInfo->widthsrc(), dstMxpiVisionInfo.preprocessinfo(0).widthsrc());
}

TEST_F(DataTypeWrapperTest, Test_ConstrutImagePreProcessInfo_Should_Failed_When_CropRight_Lower_Than_CropLeft)
{
    std::string elementName = "test";

    MxTools::MxpiVisionInfo visionInfo;
    MxBase::ImagePreProcessInfo positionInfo(ZERO, ZERO);
    MxTools::MxpiVisionPreProcess* preprocessInfo = visionInfo.add_preprocessinfo();
    preprocessInfo->set_widthsrc(ZERO);
    preprocessInfo->set_heightsrc(ZERO);
    preprocessInfo->set_cropleft(ZERO);
    preprocessInfo->set_cropright(ZERO);
    preprocessInfo->set_croptop(ZERO);
    preprocessInfo->set_cropbottom(ZERO);
    preprocessInfo->set_pasteleft(ZERO);
    preprocessInfo->set_pasteright(ZERO);
    preprocessInfo->set_pastetop(ZERO);
    preprocessInfo->set_pastebottom(ZERO);
    preprocessInfo->set_interpolation(ZERO);
    preprocessInfo->set_interpolation(ZERO);
    preprocessInfo->set_elementname(elementName);

    visionInfo.set_format(ZERO);
    visionInfo.set_width(ZERO);
    visionInfo.set_height(ZERO);
    visionInfo.set_widthaligned(ZERO);
    visionInfo.set_heightaligned(ZERO);
    visionInfo.set_resizetype(ZERO);
    visionInfo.set_keepaspectratioscaling(ZERO_F);

    auto ret = ConstrutImagePreProcessInfo(visionInfo, positionInfo);

    EXPECT_EQ(ret, APP_ERR_COMM_INVALID_PARAM);
}

TEST_F(DataTypeWrapperTest, Test_ConstrutImagePreProcessInfo_Should_Success)
{
    std::string elementName = "test";

    MxTools::MxpiVisionInfo visionInfo;
    MxBase::ImagePreProcessInfo positionInfo(ZERO, ZERO);
    MxTools::MxpiVisionPreProcess* preprocessInfo = visionInfo.add_preprocessinfo();
    preprocessInfo->set_widthsrc(ZERO);
    preprocessInfo->set_heightsrc(ZERO);
    preprocessInfo->set_cropleft(ZERO);
    preprocessInfo->set_cropright(HUNDRED);
    preprocessInfo->set_croptop(ZERO);
    preprocessInfo->set_cropbottom(HUNDRED);
    preprocessInfo->set_pasteleft(ZERO);
    preprocessInfo->set_pasteright(HUNDRED);
    preprocessInfo->set_pastetop(ZERO);
    preprocessInfo->set_pastebottom(HUNDRED);
    preprocessInfo->set_interpolation(ZERO);
    preprocessInfo->set_interpolation(ZERO);
    preprocessInfo->set_elementname(elementName);

    visionInfo.set_format(ZERO);
    visionInfo.set_width(ZERO);
    visionInfo.set_height(ZERO);
    visionInfo.set_widthaligned(ZERO);
    visionInfo.set_heightaligned(ZERO);
    visionInfo.set_resizetype(ZERO);
    visionInfo.set_keepaspectratioscaling(ZERO_F);

    auto ret = ConstrutImagePreProcessInfo(visionInfo, positionInfo);

    EXPECT_EQ(ret, APP_ERR_OK);
}
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

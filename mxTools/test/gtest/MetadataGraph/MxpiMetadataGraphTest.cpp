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
 * Description: MxPiMetadataGraph Test.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/MetadataGraph/MxpiMetadataGraph.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

using namespace MxTools;
using namespace MxBase;

namespace {
const int HEIGHT_TEST_VALUE = 10;
const int WIDTH_TEST_VALUE = 10;
const int MEMBER_ID_TEST_VALUE = 0;
const float POSITIONX0 = 100;
const float POSITIONX1 = 100;
const float POSITIONY0 = 100;
const float POSITIONY1 = 100;

std::shared_ptr<MxpiVisionList> CreateMetadata(const std::string &parentName, int memberId, int width, int height)
{
    std::shared_ptr<MxpiVisionList> nodeListMessage = MxBase::MemoryHelper::MakeShared<MxpiVisionList>();
    if (nodeListMessage == nullptr) {
        std::cout << "Create QueueSizeStatisticsDptr object failed. Failed to allocate memory." << std::endl;
        return nullptr;
    }
    MxpiVision *vision = nodeListMessage.get()->add_visionvec();
    MxpiMetaHeader *header = vision->add_headervec();
    header->set_datasource(parentName);
    header->set_memberid(memberId);
    MxpiVisionInfo *visionInfo = vision->mutable_visioninfo();
    visionInfo->set_height(height);
    visionInfo->set_width(width);
    return nodeListMessage;
}

std::shared_ptr<MxpiObjectList> CreateObjectMetadata(const std::string &parentName)
{
    std::shared_ptr<MxpiObjectList> nodeListMessage = MxBase::MemoryHelper::MakeShared<MxpiObjectList>();
    if (nodeListMessage == nullptr) {
        std::cout << "Create MxpiObjectList object failed. Failed to allocate memory." << std::endl;
        return nullptr;
    }
    MxTools::MxpiObject *objectData = nodeListMessage->add_objectvec();
    objectData->set_x0(POSITIONX0);
    objectData->set_y0(POSITIONY0);
    objectData->set_x1(POSITIONX1);
    objectData->set_y1(POSITIONY1);
    MxTools::MxpiClass *classInfo = objectData->add_classvec();
    const int classId = 2;
    classInfo->set_classid(classId);
    classInfo->set_classname("classname");
    MxTools::MxpiMetaHeader *header = objectData->add_headervec();
    header->set_datasource(parentName);
    header->set_memberid(0);
    return nodeListMessage;
}

std::shared_ptr<MxpiVisionList> CreateNoHeaderMetadata(int width, int height)
{
    std::shared_ptr<MxpiVisionList> nodeListMessage = MxBase::MemoryHelper::MakeShared<MxpiVisionList>();
    if (nodeListMessage == nullptr) {
        std::cout << "Create MxpiVisionList object failed. Failed to allocate memory." << std::endl;
        return nullptr;
    }
    MxpiVision *vision = nodeListMessage.get()->add_visionvec();
    MxpiVisionInfo *visionInfo = vision->mutable_visioninfo();
    visionInfo->set_height(height);
    visionInfo->set_width(width);
    return nodeListMessage;
}

APP_ERROR MatchJsonString(const std::string &src, const std::string &dest)
{
    nlohmann::json jsonValueSrc = nlohmann::json::parse(src);
    nlohmann::json jsonValueDest = nlohmann::json::parse(dest);
    std::string jsonStringSrc = jsonValueSrc.dump();
    std::string jsonStringDest = jsonValueDest.dump();
    std::cout << jsonStringSrc << std::endl;
    std::cout << jsonStringDest << std::endl;
    if (jsonValueSrc.size() != jsonValueDest.size()) {
        return APP_ERR_COMM_FAILURE;
    }
    if (jsonStringSrc.size() != jsonStringDest.size()) {
        return APP_ERR_COMM_FAILURE;
    }

    return APP_ERR_OK;
}

void CreateSubGraph(MxpiMetadataGraph &metadataGraph, std::map<std::string,
    std::shared_ptr<google::protobuf::Message>> &nodeMap)
{
    std::string nodeListName = "NodeList0";
    std::shared_ptr<MxpiVisionList> nodeListMessage =
    CreateMetadata("NodeListRoot", 0, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    APP_ERROR ret =
    metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    nodeListName = "NodeList1";
    nodeListMessage = CreateMetadata("NodeListRoot", MEMBER_ID_TEST_VALUE, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    ret = metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    nodeListName = "NodeList2";
    nodeListMessage = CreateMetadata("NodeList1", 0, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    ret = metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    nodeListName = "NodeList3";
    nodeListMessage = CreateMetadata("NodeList1", 0, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    ret = metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);
}

class MxpiMetadataGraphTest : public testing::Test {
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

TEST_F(MxpiMetadataGraphTest, AddNodeList0)
{
    MxpiMetadataGraph metadataGraph;
    std::string nodeListName = "NodeList0";
    std::shared_ptr<MxpiVisionList> nodeListMessage =
        CreateMetadata("NodeListRoot", 0, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    APP_ERROR ret =
        metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxpiMetadataGraphTest, AddNodeList1)
{
    MxpiMetadataGraph metadataGraph;
    std::map<std::string, std::shared_ptr<google::protobuf::Message>> nodeMap;
    std::string nodeListName = "NodeList0";
    std::shared_ptr<MxpiVisionList> nodeListMessage =
        CreateMetadata("NodeListRoot", 0, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    APP_ERROR ret =
        metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    nodeListName = "NodeList1";
    nodeListMessage = CreateMetadata("NodeListRoot", 0, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    ret = metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    nodeListName = "NodeList2";
    nodeListMessage = CreateMetadata("NodeList1", 0, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    ret = metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);
}

TEST_F(MxpiMetadataGraphTest, AddNoHeaderNodeList)
{
    MxpiMetadataGraph metadataGraph;
    std::string nodeListName = "NodeList0";
    std::shared_ptr<MxpiVisionList> nodeListMessage =
        CreateNoHeaderMetadata(WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    APP_ERROR ret =
        metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);
    std::string jsonString = metadataGraph.GetJsonString();
    EXPECT_EQ(jsonString.length() > 0 ? APP_ERR_OK : APP_ERR_COMM_FAILURE, APP_ERR_OK);
}

TEST_F(MxpiMetadataGraphTest, GetJsonString0)
{
    MxpiMetadataGraph metadataGraph;
    std::map<std::string, std::shared_ptr<google::protobuf::Message>> nodeMap;
    std::string nodeListName = "NodeList0";
    std::shared_ptr<MxpiVisionList> nodeListMessage =
        CreateMetadata("NodeListRoot", 0, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    APP_ERROR ret =
        metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    nodeListName = "NodeList1";
    nodeListMessage = CreateMetadata("NodeListRoot", MEMBER_ID_TEST_VALUE, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    ret = metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    nodeListName = "NodeList2";
    nodeListMessage = CreateMetadata("NodeList1", 0, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    ret = metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string jsonString = metadataGraph.GetJsonString();
    std::string jsonStringCompare = "{\"MxpiVision\":[{\"MxpiVision\":[{\"visionInfo\":{\"format\":0,\"height\":10,"
        "\"heightAligned\":0,\"keepAspectRatioScaling\":0,\"preprocessInfo\":[],\"resizeType\":0,\"width\":10,"
        "\"widthAligned\":0}}]"
        ",\"visionInfo\":{\"format\":0,\"height\":10,\"heightAligned\":0,\"keepAspectRatioScaling\":0,"
        "\"preprocessInfo\":[],\"resizeType\":"
        "0,\"width\":10,\"widthAligned\":0}},{\"visionInfo\":{\"format\":0,\"height\":10,\"heightAligned\":0"
        ",\"keepAspectRatioScaling\":0,\"preprocessInfo\":[],\"resizeType\":0,\"width\":10,\"widthAligned\":0}}]}";
    EXPECT_EQ(MatchJsonString(jsonString, jsonStringCompare), APP_ERR_OK);
}

TEST_F(MxpiMetadataGraphTest, GetJsonString1)
{
    MxpiMetadataGraph metadataGraph;
    std::map<std::string, std::shared_ptr<google::protobuf::Message>> nodeMap;
    CreateSubGraph(metadataGraph, nodeMap);

    APP_ERROR ret = metadataGraph.MarkNodeListAsInvalid("NodeList3");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string nodeListName = "NodeList4";
    std::shared_ptr<MxpiVisionList> nodeListMessage =
        CreateMetadata("NodeListRoot", MEMBER_ID_TEST_VALUE, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    ret = metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string jsonString = metadataGraph.GetJsonString();
    std::string jsonStringCompare = "{\"MxpiVision\":[{\"MxpiVision\":[{\"visionInfo\":{\"format\":0,\"height\":10,"
        "\"heightAligned\":0,\"keepAspectRatioScaling\":0,\"preprocessInfo\":[],\"resizeType\":0,\"width\":10,"
        "\"widthAligned\":0}}]"
        ",\"visionInfo\":{\"format\":0,\"height\":10,\"heightAligned\":0,\"keepAspectRatioScaling\":0,"
        "\"preprocessInfo\":[],\"resizeType\":0"
        ",\"width\":10,\"widthAligned\":0}},{\"visionInfo\":{\"format\":0,\"height\":10,\"heightAligned\":0"
        ",\"keepAspectRatioScaling\":0,\"preprocessInfo\":[],\"resizeType\":0,\"width\":10,\"widthAligned\":0}},{"
        "\"visionInfo\":{\"format\":"
        "0,\"height\":10,\"heightAligned\":0,\"keepAspectRatioScaling\":0,\"preprocessInfo\":[],\"resizeType\":0,"
        "\"width\":10"
        ",\"widthAligned\":0}}]}";
    EXPECT_EQ(MatchJsonString(jsonString, jsonStringCompare), APP_ERR_OK);

    nodeListName = "NodeList4";
    jsonString = metadataGraph.GetJsonStringFromNodelist(nodeListName);
    jsonStringCompare = "{\"MxpiVision\":[{\"visionInfo\":{\"format\":0,\"height\":10,"
        "\"heightAligned\":0,\"keepAspectRatioScaling\":0,\"preprocessInfo\":[],\"resizeType\":0,\"width\":10"
        ",\"widthAligned\":0}}]}";
    EXPECT_EQ(MatchJsonString(jsonString, jsonStringCompare), APP_ERR_OK);
}

TEST_F(MxpiMetadataGraphTest, GetJsonString2)
{
    MxpiMetadataGraph metadataGraph;
    std::map<std::string, std::shared_ptr<google::protobuf::Message>> nodeMap;
    CreateSubGraph(metadataGraph, nodeMap);
    APP_ERROR ret = metadataGraph.MarkNodeListAsInvalid("NodeList2");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string nodeListName = "NodeList4";
    std::shared_ptr<MxpiVisionList> nodeListMessage =
        CreateMetadata("NodeList2", MEMBER_ID_TEST_VALUE, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    ret = metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    nodeListName = "NodeList5";
    nodeListMessage = CreateMetadata("NodeListRoot", MEMBER_ID_TEST_VALUE, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    ret = metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    ret = metadataGraph.MarkAllNodesAsInvalid();
    std::string jsonString = metadataGraph.GetJsonString();
    std::string jsonStringCompare = "null";
    EXPECT_EQ(MatchJsonString(jsonString, jsonStringCompare), APP_ERR_OK);
}

TEST_F(MxpiMetadataGraphTest, GetJsonString3)
{
    MxpiMetadataGraph metadataGraph;
    std::map<std::string, std::shared_ptr<google::protobuf::Message>> nodeMap;
    CreateSubGraph(metadataGraph, nodeMap);
    APP_ERROR ret = metadataGraph.MarkNodeListAsInvalid("NodeList3");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string nodeListName = "NodeList4";
    std::shared_ptr<MxpiVisionList> nodeListMessage =
        CreateMetadata("NodeList1", MEMBER_ID_TEST_VALUE, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    ret = metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    nodeListName = "NodeList5";
    nodeListMessage = CreateMetadata("NodeListRoot", MEMBER_ID_TEST_VALUE, WIDTH_TEST_VALUE, HEIGHT_TEST_VALUE);
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    ret = metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string jsonString = metadataGraph.GetJsonString();
    std::string jsonStringCompare = "{\"MxpiVision\":[{\"visionInfo\":{\"format\":0,\"height\":10,\"heightAligned\":0,"
                                    "\"keepAspectRatioScaling\":0,\"preprocessInfo\":[]"
        ",\"resizeType\":0,\"width\":10,\"widthAligned\":0}},{\"visionInfo\":{\"format\":0,\"height\":10,"
        "\"heightAligned\":0,\"keepAspectRatioScaling\":0,\"preprocessInfo\":[],\"resizeType\":0,\"width\":10,"
        "\"widthAligned\":0}},"
        "{\"MxpiVision\":[{\"visionInfo\":{\"format\":0,\"height\":10,\"heightAligned\":0,\"keepAspectRatioScaling\":0,"
        "\"preprocessInfo\":[]"
        ",\"resizeType\":0,\"width\":10,\"widthAligned\":0}},{\"visionInfo\":{\"format\":0,\"height\":10"
        ",\"heightAligned\":0,\"keepAspectRatioScaling\":0,\"preprocessInfo\":[],\"resizeType\":0,\"width\":10,"
        "\"widthAligned\":0}}]"
        ",\"visionInfo\":{\"format\":0,\"height\":10,\"heightAligned\":0,\"keepAspectRatioScaling\":0,"
        "\"preprocessInfo\":[],\"resizeType\":0"
        ",\"width\":10,\"widthAligned\":0}}]}";
    EXPECT_EQ(MatchJsonString(jsonString, jsonStringCompare), APP_ERR_OK);

    nodeListName = "NodeList4";
    jsonString = metadataGraph.GetJsonStringFromNodelist(nodeListName);
    jsonStringCompare = "{\"MxpiVision\":[{\"visionInfo\":{\"format\":0,\"height\":10,"
        "\"heightAligned\":0,\"keepAspectRatioScaling\":0,\"preprocessInfo\":[],\"resizeType\":0,"
        "\"width\":10,\"widthAligned\":0}}]}";
    EXPECT_EQ(MatchJsonString(jsonString, jsonStringCompare), APP_ERR_OK);
}

TEST_F(MxpiMetadataGraphTest, GetJsonString4)
{
    MxpiMetadataGraph metadataGraph;
    std::map<std::string, std::shared_ptr<google::protobuf::Message>> nodeMap;
    CreateSubGraph(metadataGraph, nodeMap);
    APP_ERROR ret = metadataGraph.MarkNodeListAsInvalid("NodeList0");
    EXPECT_EQ(ret, APP_ERR_OK);
    ret = metadataGraph.MarkNodeListAsInvalid("NodeList1");
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string nodeListName = "NodeList4";
    std::shared_ptr<MxpiObjectList> nodeListMessage = CreateObjectMetadata("NodeList0");
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage);
    ret = metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage));
    EXPECT_EQ(ret, APP_ERR_OK);

    nodeListName = "NodeList5";
    std::shared_ptr<MxpiObjectList> nodeListMessage2 = CreateObjectMetadata("NodeList0");
    nodeMap[nodeListName] = std::static_pointer_cast<google::protobuf::Message>(nodeListMessage2);
    ret =
        metadataGraph.AddNodeList(nodeListName, std::static_pointer_cast<google::protobuf::Message>(nodeListMessage2));
    EXPECT_EQ(ret, APP_ERR_OK);

    std::string jsonString = metadataGraph.GetJsonString();
    std::string jsonStringCompare =
        "{\"MxpiObject\":[{\"classVec\":[{\"classId\":2,\"className\":\"classname\",\"confidence\""
        ":0,\"headerVec\":[]}],\"x0\":100,\"x1\":100,\"y0\":100,\"y1\":100},{\"classVec\":[{\""
        "classId\":2,\"className\":\"classname\",\"confidence\":0,\"headerVec\":[]}],\"x0\":100,"
        "\"x1\":100,\"y0\":100,\"y1\":100}],\"MxpiVision\":[{\"visionInfo\":{\"format\":0,\"height\":10,"
        "\"heightAligned\":0,\"keepAspectRatioScaling\":0,\"preprocessInfo\":[],\"resizeType\":0,\"width\":10,"
        "\"widthAligned\":0}},"
        "{\"visionInfo\":{\"format\":0,\"height\":10,"
        "\"heightAligned\":0,\"keepAspectRatioScaling\":0,\"preprocessInfo\":[],\"resizeType\":0,\"width\":10,"
        "\"widthAligned\":0}}]}";
    EXPECT_EQ(MatchJsonString(jsonString, jsonStringCompare), APP_ERR_OK);

    nodeListName = "NodeList4";
    jsonString = metadataGraph.GetJsonStringFromNodelist(nodeListName);
    jsonStringCompare = "{\"MxpiObject\":[{\"classVec\":[{\"classId\":2,\"className\":\"classname\",\"confidence\":"
        "0,\"headerVec\":[]}],\"x0\":100,\"x1\":100,\"y0\":100,\"y1\":100}]}";
    EXPECT_EQ(MatchJsonString(jsonString, jsonStringCompare), APP_ERR_OK);
}
}  // namespace
int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

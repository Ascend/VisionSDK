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
 * Description: DataStructure transforming of Packet and Message.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef DATA_TRANSFORM_H
#define DATA_TRANSFORM_H

#include <string>
#include "MxStream/Packet/PacketDataType.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include <google/protobuf/message.h>
#include "MxStream/Packet/Packet.h"

namespace MxStream {
APP_ERROR VisionListTransform(MxVisionList &visionList, std::shared_ptr<google::protobuf::Message> messagePtr);
APP_ERROR ClassListTransform(MxClassList &classList, std::shared_ptr<google::protobuf::Message> messagePtr);
APP_ERROR ImageMaskListTransform(MxImageMaskList &imageMaskList, std::shared_ptr<google::protobuf::Message> messagePtr);
APP_ERROR ObjectListTransform(MxObjectList &objectList, std::shared_ptr<google::protobuf::Message> messagePtr);
APP_ERROR TensorPackageListTransform(MxTensorPackageList &tensorList,
                                     std::shared_ptr<google::protobuf::Message> messagePtr);
APP_ERROR PoseListTransform(MxPoseList &poseList, std::shared_ptr<google::protobuf::Message> messagePtr);
APP_ERROR TextObjectListTransform(MxTextObjectList &textObjectList,
                                  std::shared_ptr<google::protobuf::Message> messagePtr);
APP_ERROR TextsInfoListTransform(MxTextsInfoList &textsInfoList, std::shared_ptr<google::protobuf::Message> messagePtr);

template <class T>
APP_ERROR Message2Packet(std::shared_ptr<google::protobuf::Message> messagePtr, Packet<std::shared_ptr<T>>& packet);
}
#endif
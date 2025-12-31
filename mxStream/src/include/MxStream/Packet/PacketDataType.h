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
 * Description: Packet DataType Sending to Stream.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef PACKET_DATATYPE_H
#define PACKET_DATATYPE_H

#include <stdint.h>
#include <string>
#include <vector>
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxStream {
enum MxDataType {
    UINT8 = 0,
    FLOAT32 = 1,
};


struct MxMetaHeader {
    std::string parentName;
    int32_t memberId;
    std::string dataSource;
};

struct MxVisionInfo {
    uint32_t format;
    uint32_t width;
    uint32_t height;
    uint32_t widthAligned;
    uint32_t heightAligned;
    uint32_t resizeType;
    float keepAspectRatioScaling;
};

struct MxVisionData {
    uint64_t dataPtr;
    int32_t dataSize;
    uint32_t deviceId;
    MxBase::MemoryData::MemoryType memType;
    uint32_t freeFunc;
    std::string dataStr;
    MxDataType dataType;
};

struct MxVision {
    std::vector<MxMetaHeader> headers;
    MxVisionInfo visionInfo;
    MxVisionData visionData;
};

struct MxVisionList {
    std::vector<MxVision> visionList;
};

struct MxClass {
    std::vector<MxMetaHeader> headers;
    int32_t classId;
    std::string className;
    float confidence;
};

struct MxClassList {
    std::vector<MxClass> classList;
};

struct MxImageMask {
    std::vector<MxMetaHeader> headers;
    std::vector<std::string> className;
    std::vector<int32_t> shape;
    int32_t dataType;
    std::string dataStr;
};

struct MxImageMaskList {
    std::vector<MxImageMask> imageMaskList;
};

struct MxObject {
    std::vector<MxMetaHeader> headers;
    float x0;
    float y0;
    float x1;
    float y1;
    std::vector<MxClass> classList;
    MxImageMask imageMask;
};

struct MxObjectList {
    std::vector<MxObject> objectList;
};

struct MxTensor {
    uint64_t tensorDataPtr;
    int32_t tensorDataSize;
    uint32_t deviceId;
    MxBase::MemoryData::MemoryType memType;
    uint64_t freeFunc;
    std::vector<int32_t> tensorShape;
    std::string dataStr;
    int32_t tensorDataType;
};

struct MxTensorPackage {
    std::vector<MxMetaHeader> headers;
    std::vector<MxTensor> tensors;
};

struct MxTensorPackageList {
    std::vector<MxTensorPackage> tensorPackageList;
};

struct MxKeyPoint {
    float x;
    float y;
    int32_t name;
    float score;
};

struct MxPose {
    std::vector<MxMetaHeader> headers;
    std::vector<MxKeyPoint> keyPoints;
    float score;
};

struct MxPoseList {
    std::vector<MxPose> poseList;
};

struct MxTextObject {
    std::vector<MxMetaHeader> headers;
    float x0;
    float y0;
    float x1;
    float y1;
    float x2;
    float y2;
    float x3;
    float y3;
    float confidence;
    std::string text;
};

struct MxTextObjectList {
    std::vector<MxTextObject> textObjectList;
};

struct MxTextsInfo {
    std::vector<MxMetaHeader> headers;
    std::vector<std::string> text;
};

struct MxTextsInfoList {
    std::vector<MxTextsInfo> textsInfoList;
};
}

#endif
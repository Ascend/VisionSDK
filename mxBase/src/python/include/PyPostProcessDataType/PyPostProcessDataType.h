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
 * Description: Python Post Process DataType.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef PY_POST_PROCESS_DATA_TYPE_H
#define PY_POST_PROCESS_DATA_TYPE_H

#include <map>
#include <string>
#include <vector>
#include "PyImage/PyImage.h"
#include "MxBase/PostProcessBases/PostProcessDataType.h"
namespace {
    const uint32_t DEFAULT_WIDTH_HEIGHT = 224;
}
namespace PyBase {

enum class ResizeType { RESIZER_STRETCHING = 0, RESIZER_TF_KEEP_ASPECT_RATIO = 1, RESIZER_MS_KEEP_ASPECT_RATIO = 2 };

class KeyPointDetectionInfo {
public:
    std::map<int, std::vector<float>> keyPointMap;
    std::map<int, float> scoreMap;
    float score;
    std::string Print();
    void FromBase(const MxBase::KeyPointDetectionInfo& other);
};
class TextObjectInfo {
public:
    float x0;
    float y0;
    float x1;
    float y1;
    float x2;
    float y2;
    float x3;
    float y3;
    float confidence;
    std::string result;
    std::string Print();
    void FromBase(const MxBase::TextObjectInfo& other);
};

class TextsInfo {
public:
    std::vector<std::string> text;
    std::string Print();
    void FromBase(const MxBase::TextsInfo& other);
};

class AttributeInfo {
public:
    int attrId;
    std::string attrName;
    std::string attrValue;
    float confidence;
    std::string Print();
    void FromBase(const MxBase::AttributeInfo& other);
};

class SemanticSegInfo {
public:
    std::vector<std::vector<int>> pixels;
    std::vector<std::string> labelMap;
    std::string Print();
    void FromBase(const MxBase::SemanticSegInfo& other);
};

class ClassInfo {
public:
    int classId;
    float confidence;
    std::string className;
    std::string Print();
    void FromBase(const MxBase::ClassInfo& other);
};

class ResizedImageInfo {
public:
    ResizedImageInfo() = default;
    uint32_t widthResize = 0;                               // memoryWidth
    uint32_t heightResize = 0;                              // memoryHeight
    uint32_t widthOriginal = 0;                             // imageWidth
    uint32_t heightOriginal = 0;                            // imageHeight
    ResizeType resizeType = ResizeType::RESIZER_STRETCHING; // resizeType
    float keepAspectRatioScaling = 0;                       // widthScaleRatio
    std::string Print();
    MxBase::ResizedImageInfo ToBase() const;
};

class ObjectInfo {
public:
    ObjectInfo() = default;
    float x0 = 0;
    float y0 = 0;
    float x1 = 0;
    float y1 = 0;
    float confidence = 0;
    float classId = 0;
    std::string className;
    std::vector<std::vector<uint8_t>> mask;
    std::string Print();
    void FromBase(const MxBase::ObjectInfo& other);
};
ResizedImageInfo resize_info(const Image& image, uint32_t resize_height = DEFAULT_WIDTH_HEIGHT,
                             uint32_t resize_width = DEFAULT_WIDTH_HEIGHT,
                             const ResizeType& resize_type = ResizeType::RESIZER_STRETCHING);
} // namespace PyBase

#endif
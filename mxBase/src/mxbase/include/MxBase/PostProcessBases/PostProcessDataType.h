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
 * Description: Post-processing Data Structure Definition.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef POST_PROCESS_DATA_TYPE
#define POST_PROCESS_DATA_TYPE

#include <string>
#include <vector>
#include <map>

#include "MxBase/Common/HiddenAttr.h"

namespace MxBase {
enum SDK_AVAILABLE_FOR_OUT TensorArrangementType {
    TYPE_NHWC = 0,
    TYPE_NCHW = 1,
    TYPE_NHW = 2,
    TYPE_NWH = 3
};

const float COORDINATE_PARAM = 2.0;
const float MEAN_PARAM = 0.5;
const int BOX_DIM = 4;
const uint32_t VECTOR_FIRST_INDEX = 0;
const uint32_t VECTOR_SECOND_INDEX = 1;
const uint32_t VECTOR_THIRD_INDEX = 2;
const uint32_t VECTOR_FOURTH_INDEX = 3;
const uint32_t VECTOR_FIFTH_INDEX = 4;
const uint32_t SHAPE_SIZE_4 = 4;
const uint32_t SHAPE_SIZE_3 = 3;

// 1.Class for Object detection
class SDK_AVAILABLE_FOR_OUT ObjectInfo {
public:
    ObjectInfo() = default;
    ObjectInfo(float x0_, float y0_, float x1_, float y1_, float confidence_, float classId_, std::string className_,
               std::vector<std::vector<uint8_t>> mask_) {
        x0 = x0_;
        y0 = y0_;
        x1 = x1_;
        y1 = y1_;
        confidence = confidence_;
        classId = classId_;
        className = className_;
        mask = mask_;
    }
public:
    float x0 = 0;
    float y0 = 0;
    float x1 = 0;
    float y1 = 0;
    float confidence = 0;
    float classId = 0;
    std::string className;
    std::vector<std::vector<uint8_t>> mask;

};

enum SDK_AVAILABLE_FOR_OUT ResizeType {
    RESIZER_STRETCHING = 0,
    RESIZER_TF_KEEP_ASPECT_RATIO,
    RESIZER_MS_KEEP_ASPECT_RATIO,
    RESIZER_ONLY_PADDING,
    RESIZER_KEEP_ASPECT_RATIO_LONG,
    RESIZER_KEEP_ASPECT_RATIO_SHORT,
    RESIZER_RESCALE,
    RESIZER_RESCALE_DOUBLE,
    RESIZER_MS_YOLOV4,
};

class SDK_AVAILABLE_FOR_OUT ImagePreProcessInfo {
public:
    ImagePreProcessInfo()
    {
        imageWidth = 0;
        imageHeight = 0;
        originalWidth = 0;
        originalHeight = 0;
        xRatio = 1.0;
        xBias = 0.0;
        yRatio = 1.0;
        yBias = 0.0;
        x0Valid = 0.0;
        y0Valid = 0.0;
        x1Valid = 0.0;
        y1Valid = 0.0;
    }
    ImagePreProcessInfo(uint32_t width, uint32_t height)
    {
        imageWidth = width;
        imageHeight = height;
        originalWidth = width;
        originalHeight = height;
        xRatio = 1.0;
        xBias = 0.0;
        yRatio = 1.0;
        yBias = 0.0;
        x0Valid = 0.0;
        y0Valid = 0.0;
        x1Valid = width;
        y1Valid = height;
    }
    ImagePreProcessInfo(uint32_t widthResize, uint32_t heightResize, uint32_t widthOriginal, uint32_t heightOriginal)
    {
        if (!((widthOriginal == 0) || (heightOriginal == 0))) {
            xRatio = widthResize / (float)widthOriginal;
            yRatio = heightResize / (float)heightOriginal;
        } else {
            xRatio = 1.0;
            yRatio = 1.0;
        }
        imageWidth = widthResize;
        imageHeight = heightResize;
        originalWidth = widthOriginal;
        originalHeight = heightOriginal;
        xBias = 0.0;
        yBias = 0.0;
        x0Valid = 0.0;
        y0Valid = 0.0;
        x1Valid = widthResize;
        y1Valid = heightResize;
    }
    ~ImagePreProcessInfo() {}
public:
    // image
    uint32_t imageWidth = 0;           // memoryWidth
    uint32_t imageHeight = 0;          // memoryHeight
    uint32_t originalWidth = 0;           // originalyWidth
    uint32_t originalHeight = 0;          // originalHeight

    // mapping parameters
    float xRatio = 1.0;
    float xBias = 0.0;
    float yRatio = 1.0;
    float yBias = 0.0;

    // valid region
    float x0Valid = 0.0;
    float y0Valid = 0.0;
    float x1Valid = 0.0;
    float y1Valid = 0.0;
};

class SDK_AVAILABLE_FOR_OUT ResizedImageInfo {
public:
    ResizedImageInfo() {}
    ResizedImageInfo(uint32_t wResize, uint32_t hResize, uint32_t wOriginal, uint32_t hOriginal, ResizeType rType,
        float kARScaling) : widthResize(wResize), heightResize(hResize), widthOriginal(wOriginal),
        heightOriginal(hOriginal), resizeType(rType), keepAspectRatioScaling(kARScaling) {}
    uint32_t widthResize = 0;           // memoryWidth
    uint32_t heightResize = 0;          // memoryHeight
    uint32_t widthOriginal = 0;         // imageWidth
    uint32_t heightOriginal = 0;        // imageHeight
    ResizeType resizeType = RESIZER_STRETCHING; // resizeType
    float keepAspectRatioScaling = 0;   // widthScaleRatio
};


class SDK_AVAILABLE_FOR_OUT CropRoiBox {
public:
    float x0;
    float y0;
    float x1;
    float y1;
};

// 2.Class for classification
class SDK_AVAILABLE_FOR_OUT ClassInfo {
public:
    int classId;
    float confidence;
    std::string className;
};

// 3.Class for semantic segmentation
class SDK_AVAILABLE_FOR_OUT SemanticSegInfo {
public:
    std::vector<std::vector<int>> pixels;
    std::vector<std::string> labelMap;
};

// 4.Class for attribute tasks
class SDK_AVAILABLE_FOR_OUT AttributeInfo {
public:
    int attrId;
    std::string attrName;
    std::string attrValue;
    float confidence;
};

// 5.Class for text generation (i.e. translation, OCR)
class SDK_AVAILABLE_FOR_OUT TextsInfo {
public:
    std::vector<std::string> text;
};

// 6.Class for text object detection
class SDK_AVAILABLE_FOR_OUT TextObjectInfo {
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
};

// 7.Class for key point detection
class SDK_AVAILABLE_FOR_OUT KeyPointDetectionInfo {
public:
    std::map<int, std::vector<float>> keyPointMap;
    std::map<int, float> scoreMap;
    float score;
};
}
#endif
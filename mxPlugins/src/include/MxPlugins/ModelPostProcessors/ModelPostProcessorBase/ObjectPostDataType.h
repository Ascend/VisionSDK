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
 * Description: Data structure definition for object detection post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef YOLOV3POST_H
#define YOLOV3POST_H

#include <algorithm>
#include <vector>
#include <memory>

const float COORDINATE_PARAM = 2.0;
const int BOX_DIM = 4;
const int VECTOR_FIRST_INDEX = 0;
const int VECTOR_SECOND_INDEX = 1;
const int VECTOR_THIRD_INDEX = 2;
const int VECTOR_FOURTH_INDEX = 3;
const int VECTOR_FIFTH_INDEX = 4;

struct OutputLayer {
    int layerIdx;
    int width;
    int height;
    float anchors[6];
};

struct NetInfo {
    int anchorDim;
    int classNum;
    int bboxDim;
    int netWidth;
    int netHeight;
    std::vector<OutputLayer> outputLayers;
};

struct ImageInfo {
    int modelWidth;
    int modelHeight;
    int imgWidth;
    int imgHeight;
};

// Detect Info which could be transformed by DetectBox
struct ObjDetectInfo {
    float x0;
    float y0;
    float x1;
    float y1;
    float confidence;
    float classId;
};

#endif

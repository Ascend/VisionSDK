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
 * Description: Filter duplicate targets based on area size.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/CV/ObjectDetection/Nms/Nms.h"
#include <cmath>
#include "MxBase/PostProcessBases/PostProcessDataType.h"

namespace {
    const float EPSILON = 1e-6;
    const float FLOAT_AVERAGE = 2.f;
    const int MAX_BOX_PARAMETER = 8192;
}
namespace MxBase {
static bool GetArea(const DetectBox& boxA, const DetectBox& boxB, float& area)
{
    if ((int)boxA.x < 0 || (int)boxA.x > MAX_BOX_PARAMETER ||
        (int)boxB.y < 0 || (int)boxB.y > MAX_BOX_PARAMETER ||
        (int)boxA.width < 0 || (int)boxA.width > MAX_BOX_PARAMETER ||
        (int)boxA.height < 0 || (int)boxA.height > MAX_BOX_PARAMETER) {
        return false;
    }

    float left = std::max(boxA.x - boxA.width / FLOAT_AVERAGE, boxB.x - boxB.width / FLOAT_AVERAGE);
    float right = std::min(boxA.x + boxA.width / FLOAT_AVERAGE, boxB.x + boxB.width / FLOAT_AVERAGE);
    float top = std::max(boxA.y - boxA.height / FLOAT_AVERAGE, boxB.y - boxB.height / FLOAT_AVERAGE);
    float bottom = std::min(boxA.y + boxA.height / FLOAT_AVERAGE, boxB.y + boxB.height / FLOAT_AVERAGE);
    if (top > bottom || left > right) { // If no intersection
        return false;
    }
    area = (right - left) * (bottom - top);
    return true;
}

float CalcIou(DetectBox boxA, DetectBox boxB, IOUMethod method)
{
    // intersection / union
    float area;
    if (!GetArea(boxA, boxB, area)) {
        return 0.0f;
    }
    float areaA = boxA.width * boxA.height;
    float areaB = boxB.width * boxB.height;
    if (method == IOUMethod::MAX) {
        if (fabs(std::max(areaA, areaB)) < EPSILON) {
            return 0;
        }
        return area / std::max(areaA, areaB);
    }
    if (method == IOUMethod::MIN) {
        if (fabs(std::min(areaA, areaB)) < EPSILON) {
            return 0;
        }
        return area / std::min(areaA, areaB);
    }
    if (method == IOUMethod::DIOU) {
        if (fabs(areaA + areaB - area) < EPSILON) {
            return 0;
        }
        float iou = area / (areaA + areaB - area);
        float boxACenterX = boxA.x;
        float boxACenterY = boxA.y;
        float boxBCenterX = boxB.x;
        float boxBCenterY = boxB.y;
        float interDiag = (boxACenterX - boxBCenterX) * (boxACenterX - boxBCenterX) +
            (boxACenterY - boxBCenterY) * (boxACenterY - boxBCenterY);
        float outLeft = std::min(boxA.x - boxA.width / 2.f, boxB.x - boxB.width / 2.f);
        float outRight = std::max(boxA.x + boxA.width / 2.f, boxB.x + boxB.width / 2.f);
        float outTop = std::min(boxA.y - boxA.height / 2.f, boxB.y - boxB.height / 2.f);
        float outBottom = std::max(boxA.y + boxA.height / 2.f, boxB.y + boxB.height / 2.f);
        float outerDiag = (outRight - outLeft) * (outRight - outLeft) + (outBottom - outTop) * (outBottom - outTop);
        if (fabs(outerDiag) < EPSILON) {
            return 0;
        }
        return iou - interDiag / outerDiag;
    }
    if (fabs(areaA + areaB - area) < EPSILON) {
        return 0;
    }
    return area / (areaA + areaB - area);
}

void FilterByIou(std::vector<DetectBox> dets,
                 std::vector<DetectBox>& sortBoxes, float iouThresh, IOUMethod method)
{
    for (size_t m = 0; m < dets.size(); ++m) {
        auto& item = dets[m];
        sortBoxes.push_back(item);
        for (size_t n = m + 1; n < dets.size(); ++n) {
            if (CalcIou(item, dets[n], method) > iouThresh) {
                dets.erase(dets.begin() + n);
                --n;
            }
        }
    }
}

void NmsSort(std::vector<DetectBox>& detBoxes, float iouThresh, IOUMethod method)
{
    if (detBoxes.size() == 0) {
        return;
    }
    std::vector<DetectBox> sortBoxes;
    std::map<int, std::vector<DetectBox>> resClassMap;
    for (const auto& item : detBoxes) {
        resClassMap[item.classID].push_back(item);
    }

    std::map<int, std::vector<DetectBox>>::iterator iter;
    for (iter = resClassMap.begin(); iter != resClassMap.end(); ++iter) {
        std::sort(iter->second.begin(), iter->second.end(), [=](const DetectBox& a, const DetectBox& b) {
            return a.prob > b.prob;
        });
        FilterByIou(iter->second, sortBoxes, iouThresh, method);
    }
    detBoxes = std::move(sortBoxes);
}

void NmsSortByArea(std::vector<DetectBox>& detBoxes, const float iouThresh, const IOUMethod method)
{
    if (detBoxes.size() == 0) {
        return;
    }
    std::vector<DetectBox> sortBoxes;
    std::map<int, std::vector<DetectBox>> resClassMap;
    for (const auto& item : detBoxes) {
        resClassMap[item.classID].push_back(item);
    }

    std::map<int, std::vector<DetectBox>>::iterator iter;
    for (iter = resClassMap.begin(); iter != resClassMap.end(); ++iter) {
        std::sort(iter->second.begin(), iter->second.end(), [=](const DetectBox& a, const DetectBox& b) {
            return a.width * a.height > b.width * b.height;
        });
        FilterByIou(iter->second, sortBoxes, iouThresh, method);
    }
    detBoxes = std::move(sortBoxes);
}

static bool CheckObjectInfoBox(const ObjectInfo& box)
{
    if ((int)box.x0 < 0 || (int)box.x0 > MAX_BOX_PARAMETER ||
        (int)box.x1 < 0 || (int)box.x1 > MAX_BOX_PARAMETER ||
        (int)box.y0 < 0 || (int)box.y0 > MAX_BOX_PARAMETER ||
        (int)box.y1 < 0 || (int)box.y1 > MAX_BOX_PARAMETER) {
        return false;
    }
    return true;
}

float CalcIou(ObjectInfo boxA, ObjectInfo boxB, IOUMethod method)
{
    if (!CheckObjectInfoBox(boxA) && !CheckObjectInfoBox(boxB)) {
        return 0.0f;
    }
    float boxAWidth = boxA.x1 - boxA.x0;
    float boxBWidth = boxB.x1 - boxB.x0;
    float boxAHeight = boxA.y1 - boxA.y0;
    float boxBHeight = boxB.y1 - boxB.y0;
    float left = std::max(boxA.x0, boxB.x0);
    float right = std::min(boxA.x1, boxB.x1);
    float top = std::max(boxA.y0, boxB.y0);
    float bottom = std::min(boxA.y1, boxB.y1);
    if (top > bottom || left > right) { // If no intersection
        return 0.0f;
    }
    // intersection / union
    float area = (right - left) * (bottom - top);
    if (method == IOUMethod::MAX) {
        if (fabs(std::max(boxAWidth * boxAHeight, boxBWidth * boxBHeight)) < EPSILON) {
            return 0;
        }
        return area / std::max(boxAWidth * boxAHeight, boxBWidth * boxBHeight);
    }
    if (method == IOUMethod::MIN) {
        if (fabs(std::min(boxAWidth * boxAHeight, boxBWidth * boxBHeight)) < EPSILON) {
            return 0;
        }
        return area / std::min(boxAWidth * boxAHeight, boxBWidth * boxBHeight);
    }
    if (fabs(boxAWidth * boxAHeight + boxBWidth * boxBHeight - area) < EPSILON) {
        return 0;
    }
    return area / (boxAWidth * boxAHeight + boxBWidth * boxBHeight - area);
}

void FilterByIou(std::vector<ObjectInfo> dets,
                 std::vector<ObjectInfo>& sortBoxes, float iouThresh, IOUMethod method)
{
    for (size_t m = 0; m < dets.size(); ++m) {
        auto& item = dets[m];
        sortBoxes.push_back(item);
        for (size_t n = m + 1; n < dets.size(); ++n) {
            if (CalcIou(item, dets[n], method) > iouThresh) {
                dets.erase(dets.begin() + n);
                --n;
            }
        }
    }
}

void NmsSort(std::vector<ObjectInfo>& detBoxes, float iouThresh, IOUMethod method)
{
    if (detBoxes.size() == 0) {
        return;
    }
    std::vector<ObjectInfo> sortBoxes;
    std::map<int, std::vector<ObjectInfo>> resClassMap;
    for (const auto& item : detBoxes) {
        resClassMap[static_cast<int>(item.classId)].push_back(item);
    }

    std::map<int, std::vector<ObjectInfo>>::iterator iter;
    for (iter = resClassMap.begin(); iter != resClassMap.end(); ++iter) {
        std::sort(iter->second.begin(), iter->second.end(), [=](const ObjectInfo& a, const ObjectInfo& b) {
            return a.confidence > b.confidence;
        });
        FilterByIou(iter->second, sortBoxes, iouThresh, method);
    }
    detBoxes = std::move(sortBoxes);
}

void NmsSortByArea(std::vector<ObjectInfo>& detBoxes, const float iouThresh, const IOUMethod method)
{
    if (detBoxes.size() == 0) {
        return;
    }
    std::vector<ObjectInfo> sortBoxes;
    std::map<int, std::vector<ObjectInfo>> resClassMap;
    for (const auto& item : detBoxes) {
        resClassMap[static_cast<int>(item.classId)].push_back(item);
    }

    std::map<int, std::vector<ObjectInfo>>::iterator iter;
    for (iter = resClassMap.begin(); iter != resClassMap.end(); ++iter) {
        std::sort(iter->second.begin(), iter->second.end(), [=](const ObjectInfo& a, const ObjectInfo& b) {
            return (a.x1 - a.x0) * (a.y1 - a.y0) > (b.x1 - b.x0) * (b.y1 - b.y0);
        });
        FilterByIou(iter->second, sortBoxes, iouThresh, method);
    }
    detBoxes = std::move(sortBoxes);
}
}
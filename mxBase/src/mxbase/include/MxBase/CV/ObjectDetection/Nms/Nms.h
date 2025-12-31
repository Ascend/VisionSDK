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

#ifndef NMS_H
#define NMS_H

#include <algorithm>
#include <vector>
#include <map>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/CV/Core/DataType.h"

namespace MxBase {
float CalcIou(DetectBox a, DetectBox b, IOUMethod method = UNION);

void FilterByIou(std::vector<DetectBox> dets,
                 std::vector<DetectBox>& sortBoxes, float iouThresh, IOUMethod method = UNION);

void NmsSort(std::vector<DetectBox>& detBoxes, float iouThresh, IOUMethod method = UNION);
void NmsSortByArea(std::vector<DetectBox>& detBoxes, const float iouThresh, const IOUMethod method = UNION);

class ObjectInfo;
float CalcIou(ObjectInfo a, ObjectInfo b, IOUMethod method = UNION);

void FilterByIou(std::vector<ObjectInfo> dets,
                 std::vector<ObjectInfo>& sortBoxes, float iouThresh, IOUMethod method = UNION);

void NmsSort(std::vector<ObjectInfo>& detBoxes, float iouThresh, IOUMethod method = UNION);
void NmsSortByArea(std::vector<ObjectInfo>& detBoxes, const float iouThresh, const IOUMethod method = UNION);
}  // namespace MxBase
#endif
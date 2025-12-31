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
 * Description: Blocking Queue Function Implementation.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef KALMANTRACKER_H
#define KALMANTRACKER_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/tracking.hpp"
#include "MxBase/CV/Core/DataType.h"
#include "MxBase/Log/Log.h"
#include <cmath>

namespace MxBase {
const int OFFSET = 2;
const int MULTIPLE = 2;

class KalmanTracker {
public:
    KalmanTracker() {}

    ~KalmanTracker() {}

    void CvKalmanInit(const MxBase::DetectBox &initRect);

    MxBase::DetectBox Predict();

    void Update(const MxBase::DetectBox &stateMat);

private:
    cv::KalmanFilter cvkalmanfilter_ = {};
    cv::Mat measurement_ = {};
    bool isInitialized_ = false;
};
}
#endif

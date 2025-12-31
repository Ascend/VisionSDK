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
 * Description: Compute affine transformation matrices.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef SIMILARITY_TRANSFORM_H
#define SIMILARITY_TRANSFORM_H

#include <vector>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxBase {
class SimilarityTransform {
public:
    // Constructor
    SimilarityTransform();
    // Destructor
    ~SimilarityTransform();
    // Calculate the affine transformation matrix
    cv::Mat Transform(const std::vector<cv::Point2f> &srcPoint, const std::vector<cv::Point2f> &dstPoint) const;

private:
    // Calculate the mean value
    cv::Point2f GetMean(const std::vector<cv::Point2f> &srcPoint) const;
    // Calculate the variance
    double GetSumVars(const cv::Mat &array) const;
};
}  // namespace MxBase
#endif
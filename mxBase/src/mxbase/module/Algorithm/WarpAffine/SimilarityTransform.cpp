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

#include "MxBase/CV/WarpAffine/SimilarityTransform.h"
#include <cmath>
#include "MxBase/Log/Log.h"

namespace {
const int TRANS_HEIGHT = 2;
const int TRANS_WIDTH = 3;
const int MAX_POINT_PARAMETER = 8192;
const size_t MAX_POINT_COUNT = 10000;
const double EPSILON_DBL = 1e-15;
}

namespace MxBase {
SimilarityTransform::SimilarityTransform() {}
SimilarityTransform::~SimilarityTransform() {}

/**
 * @description: Calculate the mean value
 * @param: srcPoint: vector of storing two-dimensional point information
 * @return: Point2f
 */
cv::Point2f SimilarityTransform::GetMean(const std::vector<cv::Point2f> &srcPoint) const
{
    float sumX = 0;
    float sumY = 0;
    for (auto point : srcPoint) {
        sumX += point.x;
        sumY += point.y;
    }
    return cv::Point2f(sumX / srcPoint.size(), sumY / srcPoint.size());
}

/**
 * @description: Calculate the variance
 * @param: array: array of storing two-dimensional point information
 * @return: variance
 */
double SimilarityTransform::GetSumVars(const cv::Mat &array) const
{
    int rows = array.rows;
    if (rows == 0) {
        return 0;
    }
    double meanX = 0;
    double meanY = 0;
    for (int i = 0; i < rows; i++) {
        meanX += array.at<double>(i, 0);
        meanY += array.at<double>(i, 1);
    }
    meanX /= rows;
    meanY /= rows;
    double sumX = 0;
    double sumY = 0;
    for (int i = 0; i < rows; i++) {
        sumX += (array.at<double>(i, 0) - meanX) * (array.at<double>(i, 0) - meanX);
        sumY += (array.at<double>(i, 1) - meanY) * (array.at<double>(i, 1) - meanY);
    }
    return sumX / rows + sumY / rows;
}

static bool CheckPointValidity(const std::vector<cv::Point2f> &point)
{
    for (auto p : point) {
        if ((int)p.x > MAX_POINT_PARAMETER || (int)p.x < 0 ||
            (int)p.y > MAX_POINT_PARAMETER || (int)p.y < 0) {
            return false;
        }
    }
    return true;
}

/**
 * @description: Calculate the affine transformation matrix
 * @param: srcPoint: Point information before transformation
 *         dstPoint: Point information after transformation
 * @return: Mat
 */
cv::Mat SimilarityTransform::Transform(const std::vector<cv::Point2f> &srcPoint,
                                       const std::vector<cv::Point2f> &dstPoint) const
{
    if (srcPoint.size() != dstPoint.size() || srcPoint.size() == 0 || srcPoint.size() > MAX_POINT_COUNT ||
        !CheckPointValidity(srcPoint) || !CheckPointValidity(dstPoint)) {
        return {};
    }
    int dim = 2;
    int numPoint = static_cast<int>(srcPoint.size());
    cv::Point2f srcPointMean = GetMean(srcPoint);
    cv::Point2f dstPointMean = GetMean(dstPoint);
    cv::Mat srcDemean(numPoint, dim, CV_64F);
    cv::Mat dstDemean(numPoint, dim, CV_64F);
    for (int i = 0; i < numPoint; i++) {
        srcDemean.at<double>(i, 0) = srcPoint[i].x - srcPointMean.x;
        srcDemean.at<double>(i, 1) = srcPoint[i].y - srcPointMean.y;
        dstDemean.at<double>(i, 0) = dstPoint[i].x - dstPointMean.x;
        dstDemean.at<double>(i, 1) = dstPoint[i].y - dstPointMean.y;
    }
    cv::Mat a = (dstDemean.t() * srcDemean) / numPoint;
    cv::Mat d = cv::Mat::ones(dim, 1, CV_64F);
    if (cv::determinant(a) < 0) {
        d.at<double>(1, 0) = -1;
    }
    cv::Mat s;
    cv::Mat u;
    cv::Mat v;
    cv::SVDecomp(a, s, u, v);
    cv::Mat t = cv::Mat::eye(dim + 1, dim + 1, CV_64F);
    cv::Mat tmpT = u * (cv::Mat::diag(d) * v);
    cv::Mat subT = t(cv::Rect(0, 0, dim, dim));
    tmpT.copyTo(subT);
    double var = GetSumVars(srcDemean);
    double varInv = 0.;
    if (fabs(var) > EPSILON_DBL) {
        varInv = 1.0 / var;
    }
    cv::Mat tmptd = s.t() * d;
    cv::Mat scale = varInv * tmptd;
    cv::Mat srcMeanMat(dim, 1, CV_64F);
    srcMeanMat.at<double>(0, 0) = srcPointMean.x;
    srcMeanMat.at<double>(1, 0) = srcPointMean.y;
    cv::Mat dstMeanMat(dim, 1, CV_64F);
    dstMeanMat.at<double>(0, 0) = dstPointMean.x;
    dstMeanMat.at<double>(1, 0) = dstPointMean.y;
    cv::Mat offset = dstMeanMat - scale.at<double>(0, 0) * (subT * srcMeanMat);
    cv::Mat subOffset = t(cv::Rect(dim, 0, 1, dim));
    offset.copyTo(subOffset);
    cv::Mat subOffsetT = t(cv::Rect(0, 0, dim, dim));
    subOffsetT = subOffsetT * scale.at<double>(0, 0);
    return t(cv::Rect(0, 0, TRANS_WIDTH, TRANS_HEIGHT));
}
}


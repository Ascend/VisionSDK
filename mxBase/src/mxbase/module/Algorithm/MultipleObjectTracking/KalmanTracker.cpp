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
 * Description: Prediction of Moving Target Coordinate Frame by Kalman Filter.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/CV/MultipleObjectTracking/KalmanTracker.h"

/*
 * The SORT algorithm uses a linear constant velocity model,which assumes 7
 * states, including
 * x coordinate of bounding box center
 * y coordinate of bounding box center
 * area of bounding box
 * aspect ratio of w to h
 * velocity of x
 * velocity of y
 * variation rate of area
 *
 * The aspect ratio is considered to be unchanged, so there is no additive item
 * for aspect ratio in the transitionMatrix
 *
 *
 * Kalman filter equation step by step
 * (1)  X(k|k-1)=AX(k-1|k-1)+BU(k)
 * X(k|k-1) is the predicted state(statePre),X(k-1|k-1) is the k-1 statePost,A
 * is transitionMatrix, B is controlMatrix, U(k) is control state, in SORT U(k) is 0.
 *
 * (2)  P(k|k-1)=AP(k-1|k-1)A'+Q
 * P(k|k-1) is the predicted errorCovPre, P(k-1|k-1) is the k-1 errorCovPost,
 * Q is processNoiseCov
 *
 * (3)  Kg(k)=P(k|k-1)H'/(HP(k|k-1))H'+R
 * Kg(k) is the kalman gain, the ratio of estimate variance in total variance,
 * H is the measurementMatrix,R is the measurementNoiseCov
 *
 * (4)  X(k|k)=X(k|k-1)+Kg(k)(Z(k)-HX(k|k-1))
 * X(k|k) is the k statePost, Z(k) is the measurement of K, in SORT Z(k) is
 * the detection result of k
 *
 * (5)  P(k|k)=(1-Kg(k)H)P(k|k-1)
 * P(k|k) is the errorCovPost
 */
namespace MxBase {
void KalmanTracker::CvKalmanInit(const MxBase::DetectBox &initRect)
{
    const int stateDim = 7;
    const int measureDim = 4;
    cvkalmanfilter_ = cv::KalmanFilter(stateDim, measureDim, 0); // zero control
    measurement_ = cv::Mat::zeros(measureDim, 1, CV_32F);  // 4 measurements, Z(k), according to detection results

    // A, will not be updated
    cvkalmanfilter_.transitionMatrix = (cv::Mat_<float>(stateDim, stateDim) <<
        1, 0, 0, 0, 1, 0, 0,
        0, 1, 0, 0, 0, 1, 0,
        0, 0, 1, 0, 0, 0, 1,
        0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 1);

    cv::setIdentity(cvkalmanfilter_.measurementMatrix);                                 // H, will not be updated
    cv::setIdentity(cvkalmanfilter_.processNoiseCov, cv::Scalar::all(1e-2));     // Q, will not be updated
    cv::setIdentity(cvkalmanfilter_.measurementNoiseCov, cv::Scalar::all(1e-1)); // R, will bot be updated
    cv::setIdentity(cvkalmanfilter_.errorCovPost, cv::Scalar::all(1));           // P(k-1|k-1), will be updated

    // initialize state vector with bounding box in
    // [center_x,center_y,area,ratio]
    // style, the velocity is 0
    // X(k-1|k-1)
    double checkData = (double)initRect.x + (double)initRect.width / MULTIPLE;
    if (checkData > __FLT_MAX__) {
        LogError << "Input is out of range. x: " << initRect.x
                 << ". width: " << initRect.width << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    cvkalmanfilter_.statePost.at<float>(0, 0) = (float)checkData;
    checkData = (double)initRect.y + (double)initRect.height / MULTIPLE;
    if (checkData > __FLT_MAX__) {
        LogError << "Input is out of range. y: " << initRect.y
                 << ". height: " << initRect.height << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    cvkalmanfilter_.statePost.at<float>(1, 0) = (float)checkData;
    checkData = (double)initRect.width * (double)initRect.height;
    if (checkData > __FLT_MAX__) {
        LogError << "Input is out of range. width: " << initRect.width
                 << ". height: " << initRect.height << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    cvkalmanfilter_.statePost.at<float>(OFFSET, 0) = (float)checkData;
    if (fabsf(initRect.height) > DBL_EPSILON) {
        cvkalmanfilter_.statePost.at<float>(OFFSET + 1, 0) = initRect.width / initRect.height;
    } else {
        cvkalmanfilter_.statePost.at<float>(OFFSET + 1, 0) = 0;
    }
    isInitialized_ = true;
}

// Predict the bounding box.
MxBase::DetectBox KalmanTracker::Predict()
{
    // predict
    // return X(k|k-1)=AX(k-1|k-1), and update
    // P(k|k-1) <- AP(k-1|k-1)A'+Q
    MxBase::DetectBox detectBox {};
    if (!isInitialized_) {
        LogError << "KalmanTracker is not initialized." << GetErrorInfo(APP_ERR_COMM_NOT_INIT);
        return detectBox;
    }
    cv::Mat predictState;
    try {
        predictState = cvkalmanfilter_.predict();
    } catch (const std::exception& ex) {
        return detectBox;
    }
    auto* pData = (float*) (predictState.data);
    if (pData == nullptr) {
        return detectBox;
    }
    float w = std::sqrt((*(pData + OFFSET)) * (*(pData + OFFSET + 1)));
    if (w < DBL_EPSILON) {
        detectBox.x = 0;
        detectBox.y = 0;
        detectBox.height = 0;
        detectBox.width = 0;
        return detectBox;
    }

    float h = (*(pData + OFFSET)) / w;
    float x = (*pData) - w / MULTIPLE;
    float y = (*(pData + 1)) - h / MULTIPLE;
    if (x < 0 && (*pData) > std::numeric_limits<float>::epsilon()) {
        x = 0;
    }
    if (y < 0 && (*(pData + 1)) > std::numeric_limits<float>::epsilon()) {
        y = 0;
    }

    detectBox.x = x;
    detectBox.y = y;
    detectBox.height = h;
    detectBox.width = w;

    return detectBox;
}

// Update the state using observed bounding box
void KalmanTracker::Update(const MxBase::DetectBox &stateMat)
{
    // measurement_, update Z(k)
    if (!isInitialized_) {
        LogError << "KalmanTracker is not initialized." << GetErrorInfo(APP_ERR_COMM_NOT_INIT);
        return;
    }
    auto* pData = (float*) (measurement_.data);
    if (pData == nullptr) {
        return;
    }
    double checkData = (double)stateMat.x + (double)stateMat.width / MULTIPLE;
    if (checkData > __FLT_MAX__) {
        LogError << "Input is out of range. x: " << stateMat.x
                 << ". width: " << stateMat.width << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    *pData = (float)checkData;
    checkData = (double)stateMat.y + (double)stateMat.height / MULTIPLE;
    if (checkData > __FLT_MAX__) {
        LogError << "Input is out of range. y: " << stateMat.y
                 << ". height: " << stateMat.height << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    *(pData + 1) = (float)checkData;
    checkData = (double)stateMat.width * (double)stateMat.height;
    if (checkData > __FLT_MAX__) {
        LogError << "Input is out of range. width: " << stateMat.width
                 << ". height: " << stateMat.height << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return;
    }
    *(pData + OFFSET) = (float)checkData;
    if (fabsf(stateMat.height) > DBL_EPSILON) {
        *(pData + OFFSET + 1) = stateMat.width / stateMat.height;
    } else {
        *(pData + OFFSET + 1) = 0;
    }

    // update, do the following steps:
    // Kg(k): P(k|k-1)H'/(HP(k|k-1))H'+R
    // X(k|k): X(k|k-1)+Kg(k)(Z(k)-HX(k|k-1))
    // P(k|k): (1-Kg(k)H)P(k|k-1)
    cvkalmanfilter_.correct(measurement_);
}
}
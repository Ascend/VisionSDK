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
 * Description: Perform an affine transformation.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef WARP_AFFINE_H
#define WARP_AFFINE_H

#include "opencv4/opencv2/highgui.hpp"
#include "opencv4/opencv2/imgcodecs.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"

namespace MxBase {
const uint32_t LANDMARK_LEN = 10;

struct KeyPointInfo {
    float kPBefore[LANDMARK_LEN];
};

class WarpAffine {
public:
    // Constructor
    WarpAffine();
    // Destructor
    ~WarpAffine();

    // Processing function of warp affine operation
    APP_ERROR Process(std::vector<MxBase::DvppDataInfo> &warpAffineDataInfoInputVec,
                      std::vector<DvppDataInfo> &warpAffineDataInfoOutputVec,
                      std::vector<KeyPointInfo> &keyPointInfoVec, int picHeight, int picWidth);

private:
    // Perform warp affine operations
    APP_ERROR ApplyWarpAffine(DvppDataInfo &warpAffineDataInfoInput,
                              DvppDataInfo &warpAffineDataInfoOutput,
                              KeyPointInfo &keyPointInfo);
    // Get standard key points information
    void GetSrcLandmark(std::vector<cv::Point2f> &points);
    // Relative transformation of key points coordinates, from 96*96 to 112*112
    void GetDstLandmark(std::vector<cv::Point2f> &points, float kPBefore[]);
    // Crop the input picture
    APP_ERROR GetCropImage(DvppDataInfo &warpAffineDataInfoInput, cv::Mat &image);
    // Calculate the transformation matrix and perform affine transformation
    void CalWarpImage(const std::vector<cv::Point2f> &srcPoints,
                      const std::vector<cv::Point2f> &dstPoints,
                      cv::Mat &warpImage, cv::Mat &srcImageNv21);
    // Save the aligned picture
    APP_ERROR SetWarpImage(DvppDataInfo &warpAffineDataInfoOutput,
                           const cv::Mat &imageWarp);
private:
    int picWidth_ = 0;
    int picHeight_ = 0;
};
}
#endif

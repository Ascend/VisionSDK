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
 * Description: Tensor Features(ModelProcess and Sift) include file.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#ifndef MX_TENSOR_FEATURES_H
#define MX_TENSOR_FEATURES_H

#include <string>
#include <opencv4/opencv2/opencv.hpp>
#include "MxBase/MxBase.h"
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxBase {
    static const int INPUT_TENSOR_DIM = 3;
    // support channel count
    static const int TENSOR_CHANNEL_COUNT = 1;
    static const int TENSOR_HEIGHT_INDEX = 0;
    static const int TENSOR_WIDTH_INDEX = 1;
    static const int TENSOR_CHANNEL_INDEX = 2;
    static const int SUPPORT_IMAGE_WIDTH = 1280;
    static const int SUPPORT_IMAGE_HEIGHT = 720;
    // assumed gaussian blur for input image
    static const float SIFT_INIT_SIGMA = 0.5f;
    // determines the size of a single descriptor orientation histogram
    static const float SIFT_DESCR_SCL_FCTR = 3.f;
    static const float KEYPOINT_SIZE_SCALE = 2;
    // threshold on magnitude of elements of descriptor vector
    static const float SIFT_DESCR_MAG_THR = 0.2f;
    // factor used to convert floating-point descriptor to unsigned char
    static const float SIFT_INT_DESCR_FCTR = 512.f;
    static const int SIFT_IMG_BORDER = 5;
    static const float POINT_MAX_DIFF = (float) (INT_MAX / 3);
    static const int SIFT_FIXPT_SCALE = 1;
    // DOG Normalization Coefficient
    static const float DOG_IMG_SCALE = 1.f / (255 * SIFT_FIXPT_SCALE);
    // DOG First order partial derivative coefficient
    static const float DOG_DERIV_SCALE = DOG_IMG_SCALE * 0.5f;
    // DOG Second order partial derivative coefficient
    static const float DOG_SECOND_DERIV_SCALE = DOG_IMG_SCALE;
    // DOG Second order mixed partial derivative coefficient
    static const float DOG_CROSS_DERIV_SCALE = DOG_IMG_SCALE * 0.25f;
    // maximum steps of keypoint interpolation before failure
    static const int SIFT_MAX_INTERP_STEPS = 5;
    // default width of descriptor histogram array
    static const int SIFT_DESCR_WIDTH = 4;
    // default number of bins per histogram in descriptor array
    static const int SIFT_DESCR_HIST_BINS = 8;
    // determines gaussian sigma_ for orientation assignment
    static const float SIFT_ORI_SIG_FCTR = 1.5f;
    // default number of bins in histogram for orientation assignment
    static const int SIFT_ORI_HIST_BINS = 36;
    // determines the radius of the region used in orientation assignment
    static const float SIFT_ORI_RADIUS = 4.5f;
    // orientation magnitude relative to max that results in new feature
    static const float SIFT_ORI_PEAK_RATIO = 0.8f;
    static const int OCTAVE_XI_OFFSET = 16;
    static const int OCTAVE_LAYER_OFFSET = 8;
    static const int SIFT_SUPPORT_OCTAVE_LAYERS = 3;
    static const double SIFT_SUPPORT_SIGMA = 1.6;
    static const double SIFT_EDGE_THRESHOLD_MAX = 1000;
    static const double SIFT_CONTRAST_THRESHOLD_MAX = 20;

    class Sift {
    public:
        explicit Sift(int nFeatures = 0, int nOctaveLayers = 3,
                      double contrastThreshold = 0.04, double edgeThreshold = 10, double sigma = 1.6,
                      int descriptorType = CV_32F);

        APP_ERROR Init(int32_t deviceId = 0);

        APP_ERROR DetectAndCompute(Tensor _image, Rect _mask,
                                   std::vector<cv::KeyPoint> &keyPoints,
                                   cv::OutputArray descriptors,
                                   bool useProvidedKeyPoints);
    protected:
        std::string modelPath_;
        int nFeatures_;
        int nOctaveLayers_;
        double contrastThreshold_;
        double edgeThreshold_;
        double sigma_;
        int descriptorType_;
        class ModelProcess;
        std::shared_ptr<ModelProcess> modelProcess_ = nullptr;

    private:

        void FindScaleSpaceExtrema(const std::vector<cv::Mat> &gaussPyramid, const std::vector<cv::Mat> &dogPyramid,
                                   std::vector<cv::KeyPoint> &keyPoints) const;

        APP_ERROR BuildPyramid(cv::Mat &cvImg, std::vector<cv::Mat> &gaussPyramid, std::vector<cv::Mat> &dogPyramid,
                               int nOctaves = 9);

        int DescriptorFeatureSize() const;
    };

}
#endif

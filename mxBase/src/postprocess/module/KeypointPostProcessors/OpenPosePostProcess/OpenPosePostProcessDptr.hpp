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
 * Description: OpenPose model post-processing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef OPENPOSE_POST_PROCESS_DPTR_H
#define OPENPOSE_POST_PROCESS_DPTR_H

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "KeypointPostProcessors/OpenPosePostProcess.h"
#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"

namespace MxBase {
    const float EPSILON = 1e-6;
    const int IMAGE_HEIGHTINDEX = 0;
    const int IMAGE_WIDTHINDEX = 1;
    const int IMAGE_CHANNELINDEX = 2;
    const float ROUND_NUM = 0.5;
    const int SHOWED_ONCE = 1;
    const int SHOWED_TWICE = 2;
    const int NUM_TO_PAIR = 2;
    const int PAIR_OFFSET = 1;
    const int NEVERSHOWED_SIGN = -1;
    const float HALF_SCALE = 0.5;
    const int SPLIT_TWO_PART = 2;
    struct PeakPoint {
        int x;
        int y;
        float score;
        int id;
    };
    struct UnitVector {
        float x;
        float y;
    };
    struct CandidateConnection {
        int idx1;
        int idx2;
        float score;
        float etc;
    };
    struct PartConnection {
        int cid1;
        int cid2;
        float score;
        int peakId1;
        int peakId2;
    };
}

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER OpenPosePostProcessDptr {
public:
    explicit OpenPosePostProcessDptr(OpenPosePostProcess *pOpenPosePostProcess);

    ~OpenPosePostProcessDptr() = default;

    OpenPosePostProcessDptr(const OpenPosePostProcessDptr &other);

    OpenPosePostProcessDptr &operator=(const OpenPosePostProcessDptr &other);

    APP_ERROR Upsample(std::vector<TensorBase> &tensors, const std::vector<ResizedImageInfo> &resizedImageInfos,
        std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos);

    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);

    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;

    APP_ERROR GetOpenPoseConfig();

public:
    int keyPointNum_ = 19;
    int filterSize_ = 25;
    float sigma_ = 3;
    int modelType_ = 0;
    const int CURRENT_VERSION = 2000002;
    OpenPosePostProcess* qPtr_ = nullptr;

private:
    const float HEAT_THRESH = 0.05;
    const float VECTOR_SCORE_THRESH = 0.05;
    const int VECTOR_CNT1_THRESH = 8;
    const int PART_CNT_THRESH = 4;
    const float HUMAN_SCORE_THRESH = 0.4;
    const int PAF_STEP = 10;
    static const int PART_NUM = 18;
    static const int PAIR_NUM = 2;
    const int MAPCHANNEL = 3;
    const int PAFCHANNEL = 2;
    const int HEATCHANNEL = 1;
    const int DEFAULT_KERNEL_SIZE = 3;
    const int HUMANSCORE_INDEX = 18;
    const int KEYPOINTNUM_INDEX = 19;
    const int HUMANINFO_NUM = 20;
    const int DEFAULT_POOLING_STEP = 1;
    const int FILTERSIZE_MAX = 100;
    const int SIGMA_MAX = 10;
    const int NHWC_C_DIM = 3;
    const uint32_t MAX_IMAGE_EDGE = 8192;
    
    // OpenPose output layer permutation (reference: https://github.com/ildoonet/tf-pose-estimation/blob/master/tf_pose/pafprocess/pafprocess.h)
    static const int PAIRS_SIZE = 19;
    const int PAIRS_NET[PAIRS_SIZE * PAIR_NUM] = {
        12, 13, 20, 21, 14, 15, 16, 17, 22, 23, 24, 25, 0, 1, 2, 3, 4, 5,
        6, 7, 8, 9, 10, 11, 28, 29, 30, 31, 34, 35, 32, 33, 36, 37, 18, 19, 26, 27
    };
    const int HEAT_PAIRS[PAIRS_SIZE * PAIR_NUM] = {
        1, 2, 1, 5, 2, 3, 3, 4, 5, 6, 6, 7, 1, 8, 8, 9, 9, 10, 1, 11,
        11, 12, 12, 13, 1, 0, 0, 14, 14, 16, 0, 15, 15, 17, 2, 16, 5, 17
    };

    void GetImageInfoShape(uint32_t batchNum, const std::vector<TensorBase> &tensors,
        const std::vector<ResizedImageInfo> &resizedImageInfos);
    void ProcessMat(cv::Mat peakMat, cv::Mat upHeatMat, cv::Mat upPafMat);
    int GetNumHumans();
    int GetPartCid(int humanid, int partid);
    float GetScore(int humanid);
    int GetPartX(int cid);
    int GetPartY(int cid);
    float GetPartScore(int cid);
    int RoundPaf(float v);
    static bool CompCandidate(CandidateConnection a, CandidateConnection b);
    std::vector<UnitVector> GetPafVectors(cv::Mat upPafMat, const int& chid1, const int& chid2, PeakPoint& peak1,
        PeakPoint& peak2);

    cv::Mat MaxPooling(cv::Mat img);
    float GetPoolMax(cv::Mat img, int j, int i, int c, int size);
    APP_ERROR MakeGaussianKernel(cv::Mat kernel, int size, float sigma);
    void KeyPointOutput(std::vector<KeyPointDetectionInfo>& keyPointInfos, cv::Mat peakMat, cv::Mat upHeatMat,
        cv::Mat upPafMat);
    void PickCandidates(std::vector<PeakPoint>& peakAList, std::vector<PeakPoint>& peakBList, int pairid,
        cv::Mat upPafMat, std::vector<CandidateConnection>& candidates);
    void ConnectCandidates(std::vector<PeakPoint>& peakAList, std::vector<PeakPoint>& peakBList,
        std::vector<PartConnection>& onePairConnections, std::vector<CandidateConnection>& candidates);
    void GenerateSubset(std::vector<PartConnection> connectionAll[], int size);
    void AddConnections(PartConnection& conn, int pairid, int subsetidx1, int subsetidx2, int found);
    float CheckEqual(float a, float b);
    APP_ERROR SeparateMatValue(cv::Mat& heatMatOut, cv::Mat& pafMatOut, float& outputInfo, uint32_t &outputLen);
    void GeneratePeaks(float& peaks, float& heatmap, std::vector<int> shape, std::vector<int> index, int& peakCnt);
    void ConnectOnePair(int pairId, int partId1, int partId2, std::vector<PartConnection>& onePairConnections);
    void CombineSubset(PartConnection& conn, int partId1, int partId2, int subsetIdx1, int subsetIdx2);
    void ProcessPafMat(cv::Mat upPafMat);
    void ConnectSubset(int found, int subsetId, int& subsetIdx1, int& subsetIdx2);
    double CalcGaussianValue(double x);
    std::vector<std::vector<float>> subset_ = {};
    std::vector<PeakPoint> peakInfosLine_ = {};
    uint32_t outputModelHeight_ = 0;
    uint32_t outputModelWidth_ = 0;
    std::vector<PeakPoint> peakInfos_[PART_NUM] = {{}};
};

template<typename T>
T IndexMat(T& mat, std::vector<int> shape, std::vector<int> index)
{
    int idx = 0;
    float *matData = &mat;
    for (size_t i = 0; i < shape.size(); i++) {
        idx = idx * shape[i] + index[i];
    }
    return static_cast<T>(matData[idx]);
}

double OpenPosePostProcessDptr::CalcGaussianValue(double x)
{
    // constants
    const double point1 =  0.254829592;
    const double point2 = -0.284496736;
    const double point3 =  1.421413741;
    const double point4 = -1.453152027;
    const double point5 =  1.061405429;
    const double num  =  0.3275911;

    // check the sign of x
    int sign = 1;
    if (x < 0) {
        sign = -1;
    }
    x = fabs(x) / sqrt(2.);

    // A&S formula 7.1.26
    double timeStep = 1.0 / (1.0 + num * x);
    double y = 1.0 - (((((point5 * timeStep + point4) * timeStep) + point3) * timeStep + point2) * timeStep + point1)
        * timeStep * exp(-x * x);
    return HALF_SCALE * (1.0 + sign * y);
}

OpenPosePostProcessDptr::OpenPosePostProcessDptr(OpenPosePostProcess *pOpenPosePostProcess)
    : qPtr_(pOpenPosePostProcess)
{}

OpenPosePostProcessDptr& OpenPosePostProcessDptr::operator=(const OpenPosePostProcessDptr &other)
{
    if (this == &other) {
        return *this;
    }
    keyPointNum_ =  other.keyPointNum_;
    modelType_ = other.modelType_;
    filterSize_ = other.filterSize_;
    sigma_ = other.sigma_;
    return *this;
}

APP_ERROR OpenPosePostProcessDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (!IsValidTensors(tensors)) {
        LogError << "Input tensors are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return qPtr_->CheckAndMoveTensors(tensors);
}

bool OpenPosePostProcessDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    if (tensors.size() == 0) {
        LogError << "The tensors vector is empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (tensors[0].GetDataTypeSize() != qPtr_->FOUR_BYTE) {
        LogError << "The tensor type(" << TensorDataTypeStr[tensors[0].GetTensorType()]
                 << ") mismatched. requires(" << qPtr_->FOUR_BYTE << ") bytes tensortype."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto shape = tensors[0].GetShape();
    if (shape.size() != 0x4) {
        LogError << "The number of tensor dimensions (" << shape.size() << ") " << "is not equal to ("
                 << 0x4 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (!IsDenominatorZero(shape[NHWC_C_DIM] - keyPointNum_ * sigma_)) {
        LogError << "The number of tensor[0][3] (" << shape[NHWC_C_DIM] << ") " << "is not equal to ("
                 << keyPointNum_ * sigma_ << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

APP_ERROR OpenPosePostProcessDptr::GetOpenPoseConfig()
{
    APP_ERROR ret = qPtr_->configData_.GetFileValue<int>("MODEL_TYPE", modelType_, 0x0, 0x3e8);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read MODEL_TYPE from config, default value(0) will be used as modelType_.";
    }
    ret = qPtr_->configData_.GetFileValue<int>("KEYPOINT_NUM", keyPointNum_, 0x0, 0x64);
    if (keyPointNum_ > INT_MAX / std::max(PAFCHANNEL, MAPCHANNEL)) {
        LogError << "Error : keyPointNum is too large: " << keyPointNum_ << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read KEYPOINT_NUM from config, default value(19) will be used as keyPointNum_.";
    }
    ret = qPtr_->configData_.GetFileValue<int>("FILTER_SIZE", filterSize_, 0x0, FILTERSIZE_MAX);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read FILTER_SIZE from config, default value(25) will be used as filterSize_.";
    }
    ret = qPtr_->configData_.GetFileValue<float>("SIGMA", sigma_, 0x0, SIGMA_MAX);
    if (ret != APP_ERR_OK) {
        LogWarn << GetErrorInfo(ret)
                << "Fail to read SIGMA from config, default value(3) will be used as sigma_.";
    }
    return APP_ERR_OK;
}

void OpenPosePostProcessDptr::GetImageInfoShape(uint32_t batchNum, const std::vector<TensorBase> &tensors,
    const std::vector<ResizedImageInfo> &)
{
    LogDebug << "Start to process GetImageInfoShape.";
    outputModelHeight_ = tensors[batchNum].GetShape()[modelType_ ? 0x2 : 0x1];
    outputModelWidth_ = tensors[batchNum].GetShape()[modelType_ ? 0x3 : 0x2];
    LogDebug << "End to process GetImageInfoShape.";
}


APP_ERROR OpenPosePostProcessDptr::Upsample(std::vector<TensorBase> &tensors,
    const std::vector<ResizedImageInfo> &resizedImageInfos,
    std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos)
{
    LogDebug << "Start to upsample.";
    auto tensor = tensors[0];
    auto shape = tensor.GetShape();
    uint32_t batchSize = shape[0];
    if (resizedImageInfos.size() < batchSize) {
        LogError << "Size of resizedImageInfos is invalid, size: "
                 << resizedImageInfos.size() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (tensors.size() < batchSize) {
        LogError << "Size of tensors is invalid, size: "
                 << tensors.size() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    const size_t modelTypeShapeSize = 4;
    const size_t nonModelTypeShapeSize = 3;
    for (uint32_t i = 0; i < batchSize; i++) {
        uint32_t resizedHeight = resizedImageInfos[i].heightResize;
        uint32_t resizedWidth = resizedImageInfos[i].widthResize;
        shape = tensors[i].GetShape();
        if (modelType_) {
            if (shape.size() < modelTypeShapeSize) {
                LogError << "Shape of tensors[" << i << "] is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }else {
            if (shape.size() < nonModelTypeShapeSize) {
                LogError << "Shape of tensors[" << i << "] is invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
        }

        if (resizedHeight > MAX_IMAGE_EDGE || resizedWidth > MAX_IMAGE_EDGE) {
            LogError << "The resizedHeight or resizedWidth is out of range. resizedHeight: " << resizedHeight
                     << ", resizedWidth: " << resizedWidth << ", maxImageEdge: " << MAX_IMAGE_EDGE << "."
                     << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
            return APP_ERR_COMM_OUT_OF_RANGE;
        }
        GetImageInfoShape(i, tensors, resizedImageInfos);
        cv::Mat gaussianMat = cv::Mat::zeros(resizedHeight, resizedWidth, CV_32FC(static_cast<size_t>(keyPointNum_)));
        cv::Mat peakMat = cv::Mat::zeros(resizedHeight, resizedWidth, CV_32FC(static_cast<size_t>(keyPointNum_)));
        cv::Mat upPafMat = cv::Mat::zeros(resizedHeight, resizedWidth,
            CV_32FC(static_cast<size_t>(keyPointNum_ * PAFCHANNEL)));
        cv::Mat upHeatMat = cv::Mat::zeros(resizedHeight, resizedWidth, CV_32FC(static_cast<size_t>(keyPointNum_)));
        cv::Mat heatMat = cv::Mat::zeros(outputModelHeight_, outputModelWidth_,
                                         CV_32FC(static_cast<size_t>(keyPointNum_)));
        cv::Mat pafMat = cv::Mat::zeros(outputModelHeight_, outputModelWidth_,
                                        CV_32FC(static_cast<size_t>(keyPointNum_ * PAFCHANNEL)));
        cv::Mat &heatMatOut = heatMat;
        cv::Mat &pafMatOut = pafMat;
        if (tensor.GetBuffer() == nullptr) {
            LogError << "The buffer is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        float& outputInfo = *((float *) (tensor.GetBuffer()) + i * tensor.GetByteSize() / batchSize);
        uint32_t outputLen = tensor.GetSize() / batchSize;
        APP_ERROR ret = SeparateMatValue(heatMatOut, pafMatOut, outputInfo, outputLen);
        if (ret != APP_ERR_OK) {
            LogError << "Separate mat value failed." << GetErrorInfo(ret);
            return ret;
        }
        cv::resize(heatMat, upHeatMat, cv::Size(static_cast<int>(resizedWidth), static_cast<int>(resizedHeight)),
            cv::INTER_AREA);
        cv::resize(pafMat, upPafMat, cv::Size(static_cast<int>(resizedWidth), static_cast<int>(resizedHeight)),
            cv::INTER_AREA);
        cv::Mat gaussianKernel = cv::Mat::zeros(filterSize_, filterSize_, CV_32FC1);
        ret = MakeGaussianKernel(gaussianKernel, filterSize_, sigma_);
        if (ret != APP_ERR_OK) {
            LogError << "MakeGaussianKernel failed." << GetErrorInfo(ret);
            return ret;
        }
        cv::filter2D(upHeatMat, gaussianMat, -1, gaussianKernel, cv::Point(-1, -1), 0, cv::BORDER_CONSTANT);
        peakMat = MaxPooling(gaussianMat);
        std::vector<KeyPointDetectionInfo> keyPointInfo;
        KeyPointOutput(keyPointInfo, peakMat, upHeatMat, upPafMat);
        keyPointInfos.push_back(keyPointInfo);
    }
    LogDebug << "Upsample successed.";
    return APP_ERR_OK;
}

APP_ERROR OpenPosePostProcessDptr::SeparateMatValue(cv::Mat& heatMatOut, cv::Mat& pafMatOut,
                                                    float& outputInfo, uint32_t& outputLen)
{
    float *heatMatData = (float *) heatMatOut.data;
    float *pafMatData = (float *) pafMatOut.data;
    float *outputData = &outputInfo;
    uint32_t WidthStride = static_cast<uint32_t>(keyPointNum_ * MAPCHANNEL);
    uint32_t HeightStride = static_cast<uint32_t>(keyPointNum_ * MAPCHANNEL) * outputModelWidth_;
    uint32_t headStride = outputModelWidth_ * static_cast<uint32_t>(keyPointNum_);
    uint32_t pafWidthStride = static_cast<uint32_t>(keyPointNum_ * PAFCHANNEL);
    uint32_t pafHeightStride = static_cast<uint32_t>(keyPointNum_ * PAFCHANNEL) * outputModelWidth_;
    uint32_t beginLen = (outputModelHeight_ - 1) * HeightStride + (outputModelWidth_ - 1) * WidthStride
                        + pafWidthStride + static_cast<uint32_t>(keyPointNum_) - 1;
    uint32_t heatMatLen = (outputModelHeight_ - 1) * headStride + (outputModelWidth_ - 1)
                          * static_cast<uint32_t>(keyPointNum_) + static_cast<uint32_t>(keyPointNum_) - 1;
    uint32_t pafMatLen = (outputModelHeight_ - 1) * pafHeightStride + (outputModelWidth_ - 1) * pafWidthStride
                        + 2 * static_cast<uint32_t>(keyPointNum_) - 1;
    uint32_t heatMaxLen = outputModelHeight_ * outputModelWidth_ * static_cast<uint32_t>(keyPointNum_);
    uint32_t pafMaxLen = outputModelHeight_ * outputModelWidth_ * static_cast<uint32_t>(keyPointNum_ * PAFCHANNEL);
    if (beginLen >= outputLen || pafMatLen >= pafMaxLen || heatMatLen >= heatMaxLen) {
        LogError << "Calculate separate mat value failed, please check!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (uint32_t y = 0; y < outputModelHeight_; ++y) {
        for (uint32_t x = 0; x < outputModelWidth_; ++x) {
            for (int c = 0; c < keyPointNum_; ++c) {
                float *begin = outputData + y * HeightStride + x * WidthStride;
                heatMatData[y * headStride + x * static_cast<uint32_t>(keyPointNum_) + static_cast<uint32_t>(c)] =
                    *(begin + c);
                pafMatData[y * pafHeightStride + x * pafWidthStride + static_cast<uint32_t>(c)] =
                    *(begin + keyPointNum_ + c);
                pafMatData[y * pafHeightStride + x * pafWidthStride +
                    static_cast<uint32_t>(keyPointNum_) + static_cast<uint32_t>(c)] =
                    *(begin + static_cast<int>(pafWidthStride) + c);
            }
        }
    }
    return APP_ERR_OK;
}

APP_ERROR OpenPosePostProcessDptr::MakeGaussianKernel(cv::Mat kernel, int size, float sigma)
{
    if (size == 0) {
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    int gaussianKernelHeight = size;
    int gaussianKernelWidth = size;
    float interval = (SPLIT_TWO_PART * sigma + 1) / (float)size;
    float step = (SPLIT_TWO_PART * sigma + interval) / (float)size;
    float start = (-sigma - (interval / SPLIT_TWO_PART));
    float index[size + 1];
    for (int i = 0; i < size + 1; i++) {
        index[i] = start + (i * step);
    }
    float cdf[size + 1];
    for (int i = 0; i < size + 1; i++) {
        cdf[i] = CalcGaussianValue(index[i]);
    }
    float kern1D[size];
    for (int i = 0; i < size; i++) {
        kern1D[i] = cdf[i + 1] - cdf[i];
    }
    float* outData = (float *)kernel.data;
    float kernelSum = 0;
    for (int i = 0; i < gaussianKernelHeight; i++) {
        for (int j = 0; j < gaussianKernelWidth; j++) {
            outData[i * kernel.cols + j] = sqrt(kern1D[i] * kern1D[j]);
            kernelSum += outData[i * kernel.cols + j];
        }
    }
    if (fabs(kernelSum) < EPSILON) {
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    for (int i = 0; i < gaussianKernelHeight; i++) {
        for (int j = 0; j < gaussianKernelWidth; j++) {
            outData[i * kernel.cols + j] = outData[i * kernel.cols + j] / kernelSum;
        }
    }
    return APP_ERR_OK;
}

cv::Mat OpenPosePostProcessDptr::MaxPooling(cv::Mat img)
{
    int height = img.rows;
    int width = img.cols;
    int channels = img.channels();
    cv::Mat peakMat = cv::Mat::zeros(height, width, CV_32FC(static_cast<size_t>(channels)));

    int r = DEFAULT_POOLING_STEP;
    int size = DEFAULT_KERNEL_SIZE;
    float *imgData = (float *)img.data;
    float *peakMatData = (float *)peakMat.data;

    for (int64_t j = 0; j < height; j += r) {
        for (int64_t i = 0; i < width; i += r) {
            for (int64_t c = 0; c < channels; c++) {
                float v = GetPoolMax(img, j, i, c, size);
                float current = *(imgData + j * channels * width + i * channels + c);
                *(peakMatData + j * channels * width + i * channels + c) = CheckEqual(current, v);
            }
        }
    }
    return peakMat;
}

float OpenPosePostProcessDptr::CheckEqual(float a, float b)
{
    if (fabs(a - b) < EPSILON) {
        return b;
    } else {
        return 0;
    }
}

float OpenPosePostProcessDptr::GetPoolMax(cv::Mat img, int row, int col, int c, int size)
{
    float v = 0;
    int height = img.rows;
    int width = img.cols;
    int channels = img.channels();
    float* imgData = (float *)img.data;
    int begin = -((size - 1) / SPLIT_TWO_PART);
    int end = ((size - 1) / SPLIT_TWO_PART) + 1;
    for (int j = begin; j < end; j++) {
        if ((row + j >= height) || (row + j < 0)) break;
        for (int i = begin; i < end; i++) {
            if ((col + i >= width) || (col + i < 0)) break;
            v = fmax(*(imgData + (int64_t)(row + j) * channels * width + (int64_t)(col + i) * channels + c), v);
        }
    }
    return v;
}

void OpenPosePostProcessDptr::KeyPointOutput(std::vector<KeyPointDetectionInfo>& keyPointInfos, cv::Mat peakMat,
    cv::Mat upHeatMat, cv::Mat upPafMat)
{
    ProcessMat(peakMat, upHeatMat, upPafMat);
    for (int humanId = 0; humanId < GetNumHumans(); humanId++) {
        KeyPointDetectionInfo keyPointInfo;
        bool is_added = false;
        for (int part_idx = 0; part_idx < PART_NUM; part_idx++) {
            int cIdx = static_cast<int>(GetPartCid(humanId, part_idx));
            if (cIdx < 0) {
                continue;
            }
            is_added = true;
            std::vector<float> keyPoint = {};
            if (IsDenominatorZero(peakMat.cols) || IsDenominatorZero(peakMat.rows)) {
                LogError << "peakMat.cols is " << peakMat.cols << ", peakMat.rows is " << peakMat.rows << "."
                         << GetErrorInfo(APP_ERR_COMM_FAILURE);
                return;
            }
            keyPointInfo.keyPointMap[part_idx].push_back((float)GetPartX(cIdx) / (float)peakMat.cols);
            keyPointInfo.keyPointMap[part_idx].push_back((float)GetPartY(cIdx) / (float)peakMat.rows);
            keyPointInfo.scoreMap[part_idx] = GetPartScore(cIdx);
        }
        if (is_added) {
            keyPointInfo.score = GetScore(humanId);
        }
        keyPointInfos.push_back(keyPointInfo);
    }
}

void OpenPosePostProcessDptr::ProcessMat(cv::Mat peakMat, cv::Mat upHeatMat, cv::Mat upPafMat)
{
    int p1 = peakMat.rows;
    int p2 = peakMat.cols;
    int p3 = peakMat.channels();
    float& peaks = *((float *) peakMat.data);
    float& heatmap = *((float *) upHeatMat.data);
    for (auto peakInfo : peakInfos_) {
        peakInfo.clear();
    }
    int peakCnt = 0;
    std::vector<int> shape {p1, p2, p3};
    for (int partId = 0; partId < PART_NUM; partId++) {
        for (int y = 0; y < p1; y++) {
            for (int x = 0; x < p2; x++) {
                std::vector<int> index {y, x, partId};
                GeneratePeaks(peaks, heatmap, shape, index, peakCnt);
            }
        }
    }
    ProcessPafMat(upPafMat);
}

void OpenPosePostProcessDptr::ProcessPafMat(cv::Mat upPafMat)
{
    peakInfosLine_.clear();
    for (int partId = 0; partId < PART_NUM; partId++) {
        for (int i = 0; i < (int) peakInfos_[partId].size(); i++) {
            peakInfosLine_.push_back(peakInfos_[partId][i]);
        }
    }
    // Start to Connect
    std::vector<PartConnection> connectionAll[PAIRS_SIZE];
    for (int pairId = 0; pairId < PAIRS_SIZE; pairId++) {
        std::vector<CandidateConnection> candidates;
        std::vector<PeakPoint>& peakAList = peakInfos_[HEAT_PAIRS[pairId * NUM_TO_PAIR]];
        std::vector<PeakPoint>& peakBList = peakInfos_[HEAT_PAIRS[pairId * NUM_TO_PAIR + PAIR_OFFSET]];
        if (peakAList.size() == 0 || peakBList.size() == 0) {
            continue;
        }
        PickCandidates(peakAList, peakBList, pairId, upPafMat, candidates);
        std::vector<PartConnection>& onePairConnections = connectionAll[pairId];
        sort(candidates.begin(), candidates.end(), CompCandidate);
        ConnectCandidates(peakAList, peakBList, onePairConnections, candidates);
    }
    subset_.clear();
    GenerateSubset(connectionAll, end(connectionAll) - begin(connectionAll));
    for (int i = static_cast<int>(subset_.size()) - 1; i >= 0; i--) {
        if (subset_[i][KEYPOINTNUM_INDEX] < PART_CNT_THRESH ||
            subset_[i][HUMANSCORE_INDEX] / subset_[i][KEYPOINTNUM_INDEX] < HUMAN_SCORE_THRESH) {
            subset_.erase(subset_.begin() + i);
        }
    }
}

void OpenPosePostProcessDptr::GeneratePeaks(float& peaks, float& heatmap, std::vector<int> shape,
                                            std::vector<int> index, int& peakCnt)
{
    if (IndexMat<float>(peaks, shape, index) > HEAT_THRESH) {
        PeakPoint info;
        info.id = peakCnt++;
        info.x = index[IMAGE_WIDTHINDEX];
        info.y = index[IMAGE_HEIGHTINDEX];
        info.score = IndexMat<float>(heatmap, shape, index);
        peakInfos_[index[IMAGE_CHANNELINDEX]].push_back(info);
    }
}

void OpenPosePostProcessDptr::PickCandidates(std::vector<PeakPoint>& peakAList, std::vector<PeakPoint>& peakBList,
    int pairId, cv::Mat upPafMat, std::vector<CandidateConnection>& candidates)
{
    int f1 = upPafMat.rows;
    int h1 = f1;
    for (int peadAId = 0; peadAId < (int) peakAList.size(); peadAId++) {
        PeakPoint& peakA = peakAList[peadAId];
        for (int peakBId = 0; peakBId < (int) peakBList.size(); peakBId++) {
            PeakPoint& peakB = peakBList[peakBId];
            // calculate vector(direction)
            UnitVector vec;
            vec.x = peakB.x - peakA.x;
            vec.y = peakB.y - peakA.y;
            float norm = (float) sqrt(vec.x * vec.x + vec.y * vec.y);
            if (norm < EPSILON) continue;
            vec.x = vec.x / norm;
            vec.y = vec.y / norm;

            std::vector<UnitVector> pafVecs = GetPafVectors(upPafMat, PAIRS_NET[pairId * NUM_TO_PAIR],
                PAIRS_NET[pairId * NUM_TO_PAIR + PAIR_OFFSET], peakA, peakB);
            float scores = 0.0f;
            // criterion 1 : score threshold count
            int pointNumCriterion = 0;
            for (int i = 0; i < PAF_STEP; i++) {
                float score = vec.x * pafVecs[i].x + vec.y * pafVecs[i].y;
                scores += score;
                pointNumCriterion += (int)(score > VECTOR_SCORE_THRESH);
            }
            float scoreCriterion = scores / PAF_STEP + std::min(0.0, HALF_SCALE * h1 / norm - 1.0);
            if (pointNumCriterion > VECTOR_CNT1_THRESH && scoreCriterion > std::numeric_limits<float>::epsilon()) {
                CandidateConnection candidate;
                candidate.idx1 = peadAId;
                candidate.idx2 = peakBId;
                candidate.score = scoreCriterion;
                candidate.etc = scoreCriterion + peakA.score + peakB.score;
                candidates.push_back(candidate);
            }
        }
    }
}

void OpenPosePostProcessDptr::ConnectCandidates(std::vector<PeakPoint>& peakAList, std::vector<PeakPoint>& peakBList,
    std::vector<PartConnection>& onePairConnections, std::vector<CandidateConnection>& candidates)
{
    for (int cId = 0; cId < (int)candidates.size(); cId++) {
        CandidateConnection& candidate = candidates[cId];
        bool assigned = false;
        for (int connId = 0; connId < (int)onePairConnections.size(); connId++) {
            if (onePairConnections[connId].peakId1 == candidate.idx1) {
                // already assigned
                assigned = true;
                break;
            }
            if (assigned) break;
            if (onePairConnections[connId].peakId2 == candidate.idx2) {
                // already assigned
                assigned = true;
                break;
            }
            if (assigned) break;
        }
        if (assigned) continue;

        PartConnection conn;
        conn.peakId1 = candidate.idx1;
        conn.peakId2 = candidate.idx2;
        conn.score = candidate.score;
        conn.cid1 = peakAList[candidate.idx1].id;
        conn.cid2 = peakBList[candidate.idx2].id;
        onePairConnections.push_back(conn);
    }
}

void OpenPosePostProcessDptr::GenerateSubset(std::vector<PartConnection> connectionAll[], int)
{
    for (int pairId = 0; pairId < PAIRS_SIZE; pairId++) {
        std::vector<PartConnection>& onePairConnections = connectionAll[pairId];
        int partId1 = HEAT_PAIRS[pairId * NUM_TO_PAIR];
        int partId2 = HEAT_PAIRS[pairId * NUM_TO_PAIR + PAIR_OFFSET];
        ConnectOnePair(pairId, partId1, partId2, onePairConnections);
    }
}

void OpenPosePostProcessDptr::ConnectOnePair(int pairId, int partId1, int partId2,
    std::vector<PartConnection>& onePairConnections)
{
    for (int connId = 0; connId < (int) onePairConnections.size(); connId++) {
        int found = 0;
        int subsetIdx1 = 0;
        int subsetIdx2 = 0;
        for (int subsetId = 0; subsetId < (int) subset_.size(); subsetId++) {
            if (IsDenominatorZero(subset_[subsetId][partId1] - onePairConnections[connId].cid1) ||
                IsDenominatorZero(subset_[subsetId][partId2] - onePairConnections[connId].cid2)) {
                ConnectSubset(found, subsetId, subsetIdx1, subsetIdx2);
                found += 1;
            }
        }
        AddConnections(onePairConnections[connId], pairId, subsetIdx1, subsetIdx2, found);
    }
}

void OpenPosePostProcessDptr::ConnectSubset(int found, int subsetId, int& subsetIdx1, int& subsetIdx2)
{
    if (found == 0) subsetIdx1 = subsetId;
    if (found == 1) subsetIdx2 = subsetId;
}

void OpenPosePostProcessDptr::AddConnections(PartConnection& conn, int pairId, int subsetIdx1, int subsetIdx2,
    int found)
{
    int partId1 = HEAT_PAIRS[pairId * NUM_TO_PAIR];
    int partId2 = HEAT_PAIRS[pairId * NUM_TO_PAIR + PAIR_OFFSET];
    if (found == 1) {
        if (!IsDenominatorZero(subset_[subsetIdx1][partId2] - conn.cid2)) {
            subset_[subsetIdx1][partId2] = conn.cid2;
            subset_[subsetIdx1][KEYPOINTNUM_INDEX] += 1;
            subset_[subsetIdx1][HUMANSCORE_INDEX] += peakInfosLine_[conn.cid2].score + conn.score;
        }
    } else if (found == SHOWED_TWICE) {
        CombineSubset(conn, partId1, partId2, subsetIdx1, subsetIdx2);
    } else if (found == 0 && pairId < (PART_NUM - 1)) {
        std::vector<float> row(HUMANINFO_NUM);
        for (int i = 0; i < HUMANINFO_NUM; i++) {
            row[i] = NEVERSHOWED_SIGN;
        }
        row[partId1] = conn.cid1;
        row[partId2] = conn.cid2;
        row[KEYPOINTNUM_INDEX] = SHOWED_TWICE;
        row[HUMANSCORE_INDEX] = peakInfosLine_[conn.cid1].score + peakInfosLine_[conn.cid2].score + conn.score;
        subset_.push_back(row);
    }
}

void OpenPosePostProcessDptr::CombineSubset(PartConnection& conn, int, int partId2, int subsetIdx1,
    int subsetIdx2)
{
    bool showedUp = false;
    for (int subsetId = 0; subsetId < PART_NUM; subsetId++) {
        if (subset_[subsetIdx1][subsetId] > std::numeric_limits<float>::epsilon()
            && subset_[subsetIdx2][subsetId] > std::numeric_limits<float>::epsilon()) {
            showedUp = true;
        }
    }
    if (!showedUp) {
        for (int subsetId = 0; subsetId < PART_NUM; subsetId++) {
            subset_[subsetIdx1][subsetId] += (subset_[subsetIdx2][subsetId] + 1);
        }
        subset_[subsetIdx1][KEYPOINTNUM_INDEX] += subset_[subsetIdx2][KEYPOINTNUM_INDEX];
        subset_[subsetIdx1][HUMANSCORE_INDEX] += subset_[subsetIdx2][HUMANSCORE_INDEX];
        subset_[subsetIdx1][HUMANSCORE_INDEX] += conn.score;
        subset_.erase(subset_.begin() + subsetIdx2);
    } else {
        subset_[subsetIdx1][partId2] = conn.cid2;
        subset_[subsetIdx1][KEYPOINTNUM_INDEX] += 1;
        subset_[subsetIdx1][HUMANSCORE_INDEX] += peakInfosLine_[conn.cid2].score + conn.score;
    }
}

int OpenPosePostProcessDptr::GetNumHumans()
{
    return subset_.size();
}

int OpenPosePostProcessDptr::GetPartCid(int humanId, int partId)
{
    return subset_[humanId][partId];
}

float OpenPosePostProcessDptr::GetScore(int humanId)
{
    if (IsDenominatorZero(subset_[humanId][KEYPOINTNUM_INDEX])) {
        LogError << "The value is invalid to be divided, Plase check it." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return 0.0f;
    }
    return subset_[humanId][HUMANSCORE_INDEX] / subset_[humanId][KEYPOINTNUM_INDEX];
}

int OpenPosePostProcessDptr::GetPartX(int cid)
{
    return peakInfosLine_[cid].x;
}

int OpenPosePostProcessDptr::GetPartY(int cid)
{
    return peakInfosLine_[cid].y;
}

float OpenPosePostProcessDptr::GetPartScore(int cid)
{
    return peakInfosLine_[cid].score;
}

int OpenPosePostProcessDptr::RoundPaf(float v)
{
    return (int) (v + ROUND_NUM);
}

bool OpenPosePostProcessDptr::CompCandidate(CandidateConnection a, CandidateConnection b)
{
    return a.score > b.score;
}

std::vector<UnitVector> OpenPosePostProcessDptr::GetPafVectors(cv::Mat upPafMat, const int &channelId1,
    const int &channelId2, PeakPoint &peak1, PeakPoint &peak2)
{
    float *pafmap = (float *) upPafMat.data;
    int f1 = upPafMat.rows;
    int f2 = upPafMat.cols;
    int f3 = upPafMat.channels();
    std::vector<UnitVector> pafVectors;

    std::vector<int> shape {f1, f2, f3};

    const float STEP_X = (peak2.x - peak1.x) / float(PAF_STEP);
    const float STEP_Y = (peak2.y - peak1.y) / float(PAF_STEP);

    for (int i = 0; i < PAF_STEP; i++) {
        int locationX = RoundPaf(peak1.x + i * STEP_X);
        int locationY = RoundPaf(peak1.y + i * STEP_Y);

        UnitVector v;
        std::vector<int> index {locationY, locationX, channelId1};
        v.x = IndexMat<float>(*pafmap, shape, index);
        index[IMAGE_CHANNELINDEX] = channelId2;
        v.y = IndexMat<float>(*pafmap, shape, index);

        pafVectors.push_back(v);
    }
    return pafVectors;
}
}
#endif

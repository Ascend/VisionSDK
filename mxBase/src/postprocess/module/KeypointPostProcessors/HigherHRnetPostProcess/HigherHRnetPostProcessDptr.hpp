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
 * Description: HigherHRnet model post-processing.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef HIGHER_HRNET_POST_PROCESS_DPTR_H
#define HIGHER_HRNET_POST_PROCESS_DPTR_H

#include <algorithm>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include "KeypointPostProcessors/HigherHRnetPostProcess.h"
#include "MxBase/CV/MultipleObjectTracking/Huangarian.h"
#include "MxBase/GlobalManager/GlobalManager.h"

namespace MxBase {
const float EPSILON = 1e-6;
const int TENSOR_NUM = 2;
const int DEFAULT_KEY_POINT_NUM = 17;
const int DEFAULT_CHANNEL_NUM = 34;
const double DEFAULT_SCALE_X = 2.0;
const double DEFAULT_SCALE_Y = 2.0;
const int DEFAULT_WIDTH = 128;
const int DEFAULT_HEIGHT = 160;
const int DEFAULT_HEIGHT_IDX = 2;
const int DEFAULT_WIDTH_IDX = 3;
const int PADDING = 2;
const int KERNEL_SIZE = 5;
const int MAX_PEOPLE_NUM = 30;
const float DEFAULT_THRESH = 0.1;
const float PADDING_NUM = 1e10;
const float TAG_THRESH = 1.;
const std::vector<int> KEYPOINT_ORDER_VEC = {0, 1, 2, 3, 4, 5, 6, 11, 12, 7, 8, 9, 10, 13, 14, 15, 16};
}

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER HigherHRnetPostProcessDptr {
public:
    explicit HigherHRnetPostProcessDptr(HigherHRnetPostProcess *pHigherHRnetPostProcess);

    ~HigherHRnetPostProcessDptr() = default;

    HigherHRnetPostProcessDptr(const HigherHRnetPostProcessDptr &other);

    HigherHRnetPostProcessDptr &operator=(const HigherHRnetPostProcessDptr &other);

    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);

    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;
    APP_ERROR BiInterLinear(cv::Mat& src, cv::Mat& dst, double sx, double sy);
    APP_ERROR KeyPointsDetect(const std::vector<TensorBase> &tensors,
        const std::vector<ResizedImageInfo> &resizedImageInfos,
        std::vector<std::vector<KeyPointDetectionInfo>>& keyPointInfos);
    float GetPixelVal(cv::Mat& src, int row, int col, int chl);
    bool isValidPixel(cv::Mat& src, int row, int col);
    std::vector<cv::Mat> GetHeatMapAndTags(float* interPolateTensor1st, float* interPolateTensor2nd,
                                           int imageFeatHight2nd, int imageFeatWidth2nd);
    float GetPoolMax(cv::Mat &img, int row, int col, int c, int size);
    cv::Mat MaxPooling(cv::Mat& img);
    void SparseHeatMap(cv::Mat& heatMap);
    void InsertTargetVal(std::vector<std::pair<float, int>> &tmpTopk, std::pair<float, int> targetVal);
    void Normalize(std::vector<std::vector<float>> &diff, std::vector<std::vector<float>> &joints);
    float GetMean(std::vector<float> &keys);
    APP_ERROR GetHuangarian(std::vector<std::vector<float>> diff, std::vector<int> &matchPairs,
        int numAdd, int numGroup);
    APP_ERROR Match(std::vector<std::vector<float>> &topkValTags, std::vector<std::vector<int>> &topkInd,
        std::vector<std::vector<float>> &topkHeatVal, std::vector<std::vector<std::vector<float>>> &matchedRes);
    APP_ERROR Adjust(std::vector<std::vector<std::vector<float>>> &matchedRes, cv::Mat& heatMap);
    void ComputeScores(std::vector<std::vector<std::vector<float>>> &matchedRes, std::vector<float> &scores);
    void AdjustCore(std::vector<std::vector<std::vector<float>>> &matchedRes, cv::Mat& heatMap, int i, int j);
    APP_ERROR Refine(std::vector<std::vector<std::vector<float>>> &matchedRes, cv::Mat& heatMap, cv::Mat& tags);
    APP_ERROR RefineCore(cv::Mat& heatMap, cv::Mat& tags, std::vector<std::vector<float>>& keypoints);
    APP_ERROR GetTopkPose(std::vector<cv::Mat> &heatAndTags, std::vector<std::vector<std::vector<float>>> &matchedRes,
        std::vector<float> &scores);
    std::vector<float> GetScale(const ResizedImageInfo& resizedImageInfo);
    std::vector<float> Get3rdPoint(std::vector<float> src0, std::vector<float> src1);
    cv::Mat GetAffineTransform(std::vector<int> &center, std::vector<float> &scale, std::vector<int> &heatMapSize);
    APP_ERROR GetFinalPreds(std::vector<std::vector<std::vector<float>>> &matchedRes,
        const ResizedImageInfo& resizedImageInfo, cv::Mat& heatMap,
        std::vector<std::vector<std::vector<float>>> &finalRes);
    void AffineTrans(std::vector<std::vector<float>> &targetCoords, cv::Mat &trans, int p);
    void Evaluate(std::vector<std::vector<std::vector<float>>> &finalRes, std::vector<float> &scores,
        std::vector<KeyPointDetectionInfo> &keyPointInfos);
    int GetJointIdx(std::vector<std::pair<float, std::vector<std::vector<float>>>> &jointDict,
        std::vector<float> &tag, int k);
    APP_ERROR JointsProcess(std::vector<std::pair<float, std::vector<std::vector<float>>>> &jointDict,
        std::vector<std::pair<float, std::vector<float>>> &tagDict, std::vector<std::vector<float>> &joints,
        std::vector<float> &tag, std::map<float, int> &tagToIdx);
    void TopKProcess(std::vector<std::vector<std::pair<float, int>>> &topkValHeatAndInd,
        std::vector<std::vector<float>> &topkValTags, std::vector<std::vector<int>> &topkInd,
        std::vector<std::vector<float>> &topkHeatVal, std::vector<cv::Mat> &heatAndTags);
    void FormKeyPoints(std::vector<std::vector<float>>& keypoints, std::vector<std::vector<float>> &ans);
    void GetRefineAns(std::pair<int, int>& argmax, cv::Mat& heatMap, float* tmpHeatPtr,
        std::vector<std::vector<float>>& ans);

public:
    HigherHRnetPostProcess* qPtr_ = nullptr;
    int keyPointNum_ = DEFAULT_KEY_POINT_NUM;
    int channelNum_ = DEFAULT_CHANNEL_NUM;
    int imageFeatHight_ = DEFAULT_HEIGHT;
    int imageFeatWidth_ = DEFAULT_WIDTH;
    int heightIndex_ = DEFAULT_HEIGHT_IDX;
    int widthIndex_ = DEFAULT_WIDTH_IDX;
    float thresh_ = DEFAULT_THRESH;
    int orderIdx_ = 0;
    std::vector<std::vector<float>> defaultDict_;
};

HigherHRnetPostProcessDptr::HigherHRnetPostProcessDptr(HigherHRnetPostProcess *pHigherHRnetPostProcess)
    : qPtr_(pHigherHRnetPostProcess)
{}

HigherHRnetPostProcessDptr& HigherHRnetPostProcessDptr::operator=(const HigherHRnetPostProcessDptr &other)
{
    if (this == &other) {
        return *this;
    }
    keyPointNum_ = other.keyPointNum_;
    channelNum_ = other.channelNum_;
    imageFeatHight_ = other.imageFeatHight_;
    imageFeatWidth_ = other.imageFeatWidth_;
    heightIndex_ = other.heightIndex_;
    widthIndex_ = other.widthIndex_;
    thresh_ = other.thresh_;
    orderIdx_ = other.orderIdx_;
    defaultDict_ = other.defaultDict_;
    return *this;
}

bool HigherHRnetPostProcessDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    if (tensors.size() != TENSOR_NUM) {
        LogError << "Model' output tensors must be equal to 2, while got " << tensors.size() << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    for (size_t i = 0; i < tensors.size(); i++) {
        if (tensors[i].GetDataTypeSize() != qPtr_->FOUR_BYTE) {
            LogError << "The tensor type(" << TensorDataTypeStr[tensors[i].GetTensorType()]
                     << ") mismatched. requires(" << qPtr_->FOUR_BYTE << ") bytes tensortype."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
    const size_t tensorLen = 4;
    auto shape1 = tensors[0].GetShape();
    if (shape1.size() != tensorLen) {
        LogError << "The output tensor's shape is not equal to 4." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (channelNum_ != static_cast<int>(shape1[1])) {
        LogError << "The output tensor's channel num " << shape1[1] << " is not equal to setting("
                 << channelNum_ << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto shape2 = tensors[1].GetShape();
    if (shape2.size() != tensorLen) {
        LogError << "The output tensor's shape is not equal to 4." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (shape2[0] != shape1[0]) {
        LogError << "The output tensor's batchsize is not equal." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (keyPointNum_ != static_cast<int>(shape2[1])) {
        LogError << "The output tensor's keyPoint num " << shape2[1] << " is not equal to setting("
                 << keyPointNum_ << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (shape1[heightIndex_] == 0 || shape1[widthIndex_] == 0) {
        LogError << "The number of tensor'shape[" << heightIndex_ << "] and ["<< widthIndex_
                 << "]must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    double scaleX = static_cast<double>(shape2[heightIndex_]) / static_cast<double>(shape1[heightIndex_]);
    double scaleY = static_cast<double>(shape2[widthIndex_]) / static_cast<double>(shape1[widthIndex_]);
    if (!IsDenominatorZero(scaleX - scaleY)) {
        LogError << "Scale ration is not equal for width and height!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

APP_ERROR HigherHRnetPostProcessDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (!IsValidTensors(tensors)) {
        LogError << "Input tensors are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return qPtr_->CheckAndMoveTensors(tensors);
}

float HigherHRnetPostProcessDptr::GetPixelVal(cv::Mat& src, int row, int col, int chl)
{
    int pixelIdx = row * src.cols + col + chl * src.rows * src.cols;
    return *((float*)src.data + pixelIdx);
}

bool HigherHRnetPostProcessDptr::isValidPixel(cv::Mat& src, int row, int col)
{
    if (row >= src.rows || col >= src.cols || row < 0 || col < 0) {
        return false;
    }
    return true;
}

APP_ERROR HigherHRnetPostProcessDptr::BiInterLinear(cv::Mat& src, cv::Mat& dst, double sx, double sy)
{
    int dstRows = static_cast<int>(round(sx * src.rows));
    int dstCols = static_cast<int>(round(sy * src.cols));
    dst = cv::Mat(dstRows, dstCols, src.type());
    int chl = dst.channels();
    int beginIdx = 0;
    for (int i = 0; i < dst.rows; i++) {
        double index_i = (i + 0.5) / sx - 0.5;
        if (index_i < 0) index_i = 0;
        if (index_i > src.rows - 1) index_i = src.rows - 1;
        int i1 = static_cast<int>(floor(index_i));
        int i2 = static_cast<int>(ceil(index_i));
        double u = index_i - i1;
        for (int j = 0; j < dst.cols; j++) {
            double index_j = (j + 0.5) / sy - 0.5;
            if (index_j < 0) index_j = 0;
            if (index_j > src.cols - 1) index_j = src.cols - 1;
            int j1 = static_cast<int>(floor(index_j));
            int j2 = static_cast<int>(ceil(index_j));
            double v = index_j - j1;
            if (!isValidPixel(src, i1, j1) || !isValidPixel(src, i1, j2)
                || !isValidPixel(src, i2, j1) || !isValidPixel(src, i2, j2)) {
                    LogError << "Calc PixelVal failed. Please check models output."
                             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
            }
            for (int z = 0; z < chl; z++) {
                beginIdx = dst.cols * dst.rows * z + i * dst.cols + j;
                *((float*)dst.data + beginIdx) = (1 - u) * (1 - v) * GetPixelVal(src, i1, j1, z)
                                               + (1 - u) * v * GetPixelVal(src, i1, j2, z)
                                               + u * (1 - v) * GetPixelVal(src, i2, j1, z)
                                               + u * v * GetPixelVal(src, i2, j2, z);
            }
        }
    }
    return APP_ERR_OK;
}

std::vector<cv::Mat> HigherHRnetPostProcessDptr::GetHeatMapAndTags(float* interPolateTensor1st,
    float* interPolateTensor2nd, int imageFeatHight2nd, int imageFeatWidth2nd)
{
    std::vector<cv::Mat> heatAndTags;
    int stride = keyPointNum_ * imageFeatHight2nd * imageFeatWidth2nd;
    for (int i = 0; i < stride; i++) {
        *(interPolateTensor1st + i) = (*(interPolateTensor2nd + i) + *(interPolateTensor1st + i)) / 2.0;
    }

    cv::Mat srcHeatMap(imageFeatHight2nd, imageFeatWidth2nd, CV_32FC(static_cast<size_t>(keyPointNum_)),
        interPolateTensor1st);
    cv::Mat dstHeatMap;
    float multiInter = 2.0;
    BiInterLinear(srcHeatMap, dstHeatMap, multiInter, multiInter);
    heatAndTags.push_back(dstHeatMap);

    cv::Mat srcTags(imageFeatHight2nd, imageFeatWidth2nd, CV_32FC(static_cast<size_t>(keyPointNum_)),
        interPolateTensor1st + stride);
    cv::Mat dstTags;
    BiInterLinear(srcTags, dstTags, multiInter, multiInter);
    heatAndTags.push_back(dstTags);
    return heatAndTags;
}

float HigherHRnetPostProcessDptr::GetPoolMax(cv::Mat &img, int row, int col, int c, int size)
{
    float v = 0.;
    int height = img.rows;
    int width = img.cols;
    float* imgData = (float *)img.data;
    int begin = -((size - 1) / 2);
    int end = ((size - 1) / 2) + 1;
    for (int j = begin; j < end; j++) {
        if ((row + j >= height) || (row + j < 0)) continue;
        for (int i = begin; i < end; i++) {
            if ((col + i >= width) || (col + i < 0)) continue;
            v = fmax(*(imgData + img.rows * img.cols * c + (row + j) * width + col + i), v);
        }
    }
    return v;
}

cv::Mat HigherHRnetPostProcessDptr::MaxPooling(cv::Mat& img)
{
    int height = img.rows;
    int width = img.cols;
    int channels = img.channels();
    cv::Mat peakMat(height, width, CV_32FC(static_cast<size_t>(channels)));

    int size = KERNEL_SIZE;
    float *peakMatData = (float *)peakMat.data;

    for (int64_t j = 0 ; j < height; j += 1) {
        for (int64_t i = 0; i < width; i += 1) {
            for (int64_t c = 0; c < channels; c++) {
                float v = GetPoolMax(img, j, i, c, size);
                *(peakMatData + width * height * c + j * width + i) = v;
            }
        }
    }
    return peakMat;
}

void HigherHRnetPostProcessDptr::SparseHeatMap(cv::Mat& heatMap)
{
    cv::Mat dstHeatMap = MaxPooling(heatMap);
    uint64_t stride = static_cast<uint64_t>(heatMap.cols * heatMap.rows * heatMap.channels());
    float* heatMapPtr = (float*) heatMap.data;
    float* dstHeatMapPtr = (float*) dstHeatMap.data;
    for (uint64_t i = 0; i < stride; i++) {
        if (fabs(*(heatMapPtr + i) - *(dstHeatMapPtr + i)) < EPSILON) {
            *(dstHeatMapPtr + i) = 1.0;
        } else {
            *(dstHeatMapPtr + i) = 0.0;
        }
        *(heatMapPtr + i) *= *(dstHeatMapPtr + i);
    }
}

static bool PairComp(std::pair<float, int> a, std::pair<float, int> b)
{
    if (!IsDenominatorZero(a.first - b.first)) return a.first > b.first;
    else return a.second < b.second;
}

void HigherHRnetPostProcessDptr::InsertTargetVal(std::vector<std::pair<float, int>> &tmpTopk,
    std::pair<float, int> targetVal)
{
    int idx = 0;
    for (; idx < (int)tmpTopk.size(); idx++) {
        if (tmpTopk[idx].first < targetVal.first) {
            break;
        }
    }
    tmpTopk.insert(tmpTopk.begin() + idx, targetVal);
    tmpTopk.pop_back();
}

float HigherHRnetPostProcessDptr::GetMean(std::vector<float> &keys)
{
    float sum = 0.;
    for (int i = 0; i < (int)keys.size(); i++) {
        sum += keys[i];
    }
    return sum / keys.size();
}

void HigherHRnetPostProcessDptr::Normalize(std::vector<std::vector<float>> &diff,
    std::vector<std::vector<float>> &joints)
{
    size_t tagIdx = 2;
    float multiple = 100.;
    for (int i = 0; i < (int)diff.size(); ++i) {
        for (int j = 0; j < (int)diff[0].size(); ++j) {
            diff[i][j] = ((float)round(diff[i][j])) * multiple;
            diff[i][j] -= joints[i][tagIdx];
        }
    }
}

APP_ERROR HigherHRnetPostProcessDptr::GetHuangarian(std::vector<std::vector<float>> diff, std::vector<int> &matchPairs,
    int numAdd, int numGroup)
{
    std::vector<std::vector<int>> diffNormed;
    for (int i = 0; i < (int)diff.size(); i++) {
        std::vector<int> diffTmp;
        for (int j = 0; j < (int)diff[0].size(); j++) {
            diffTmp.push_back(static_cast<int>(round(diff[i][j] * 1e5))  * -1);
        }
        diffNormed.push_back(diffTmp);
    }
    if (numAdd > numGroup) {
        for (int k = 0; k < numAdd; k++) {
            for (int z = 0; z < numAdd - numGroup; z++) {
                diffNormed[k].push_back(int(-1)*int(PADDING_NUM));
            }
        }
    }

    HungarianHandle hungarianHandleObj;
    APP_ERROR ret = HungarianHandleInit(hungarianHandleObj, diffNormed.size(), diffNormed[0].size());
    if (ret != APP_ERR_OK) {
        return ret;
    }
    HungarianSolve(hungarianHandleObj, diffNormed, diffNormed.size(), diffNormed[0].size());
    for (int i = 0; i < (int)diffNormed.size(); i++) {
        matchPairs.push_back(hungarianHandleObj.resX[i]);
    }
    return APP_ERR_OK;
}

int HigherHRnetPostProcessDptr::GetJointIdx(std::vector<std::pair<float, std::vector<std::vector<float>>>> &jointDict,
    std::vector<float> &tag, int k)
{
    int existIdx = -1;
    for (int y = 0; y < (int)jointDict.size(); y++) {
        if (fabs(jointDict[y].first - tag[k]) < EPSILON) {
            existIdx = y;
            break;
        }
    }
    return existIdx;
}

APP_ERROR HigherHRnetPostProcessDptr::JointsProcess(
    std::vector<std::pair<float, std::vector<std::vector<float>>>> &jointDict,
    std::vector<std::pair<float, std::vector<float>>> &tagDict, std::vector<std::vector<float>> &joints,
    std::vector<float> &tag, std::map<float, int> &tagToIdx)
{
    std::vector<float> groupKeys;
    std::vector<float> groupTags;
    for (int k = 0; k < MAX_PEOPLE_NUM && k < (int)jointDict.size(); k++) {
        groupKeys.push_back(jointDict[k].first);
    }
    for (int k = 0; k < (int)groupKeys.size(); k++) {
        groupTags.push_back(GetMean(tagDict[k].second));
    }
    std::vector<std::vector<float>> diff;
    size_t confIdx = 3;
    for (int k = 0; k < (int)joints.size(); k++) {
        std::vector<float> tmpDiff;
        for (int z = 0; z < (int)groupTags.size(); z++) {
            tmpDiff.push_back(fabs(joints[k][confIdx] - groupTags[z]));
        }
        diff.push_back(tmpDiff);
    }
    std::vector<std::vector<float>> diffSaved = diff;
    Normalize(diff, joints);
    int numAdd = static_cast<int>(diff.size());
    int numGroup = static_cast<int>(diff[0].size());

    std::vector<int> matchPairs;
    APP_ERROR ret = GetHuangarian(diff, matchPairs, numAdd, numGroup);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    for (int k = 0; k < (int)matchPairs.size(); k++) {
        if (k < numAdd && matchPairs[k] < numGroup && diffSaved[k][matchPairs[k]] < TAG_THRESH) {
            int key = tagToIdx[groupKeys[matchPairs[k]]];
            (jointDict[key].second)[orderIdx_] = joints[k];
            (tagDict[key].second).push_back(tag[k]);
        } else if (GetJointIdx(jointDict, tag, k) == -1) {
            jointDict.push_back(std::pair<float, std::vector<std::vector<float>>>(tag[k], defaultDict_));
            tagToIdx[tag[k]] = static_cast<int>(jointDict.size()) - 1;
            int key = tagToIdx[tag[k]];
            (jointDict[key].second)[orderIdx_] = joints[k];
            tagDict.push_back(std::pair<float, std::vector<float>>(tag[k], {tag[k]}));
        } else {
            jointDict[GetJointIdx(jointDict, tag, k)].second[orderIdx_] = joints[k];
            tagDict[GetJointIdx(jointDict, tag, k)].second = {tag[k]};
        }
    }
    return APP_ERR_OK;
}

APP_ERROR HigherHRnetPostProcessDptr::Match(std::vector<std::vector<float>> &topkValTags,
    std::vector<std::vector<int>> &topkInd, std::vector<std::vector<float>> &topkHeatVal,
    std::vector<std::vector<std::vector<float>>> &matchedRes)
{
    std::vector<std::pair<float, std::vector<std::vector<float>>>> jointDict;
    std::vector<std::pair<float, std::vector<float>>> tagDict;
    size_t keyPointDim = 4;
    defaultDict_ = std::vector<std::vector<float>>(keyPointNum_, std::vector<float>(keyPointDim, 0.));
    std::map<float, int> tagToIdx;
    for (int i = 0; i < keyPointNum_; i++) {
        orderIdx_ = KEYPOINT_ORDER_VEC[i];
        std::vector<std::vector<float>> joints;
        std::vector<float> tag;
        for (int j = 0; j < MAX_PEOPLE_NUM; j++) {
            if (topkHeatVal[orderIdx_][j] <= thresh_) continue;
            joints.push_back({(float)topkInd[orderIdx_][j*2], (float)topkInd[orderIdx_][j*2+1],
                topkHeatVal[orderIdx_][j], topkValTags[orderIdx_][j]});
            tag.push_back(topkValTags[orderIdx_][j]);
        }
        if (joints.empty()) continue;

        if (i == 0 || jointDict.size() == 0) {
            for (int k = 0; k < (int)joints.size(); k++) {
                jointDict.push_back(std::pair<float, std::vector<std::vector<float>>>(tag[k], defaultDict_));
                tagToIdx[tag[k]] = static_cast<int>(jointDict.size()) - 1;
                (jointDict[tagToIdx[tag[k]]].second)[orderIdx_] = joints[k];
                tagDict.push_back(std::pair<float, std::vector<float>>(tag[k], {tag[k]}));
            }
        } else {
            APP_ERROR ret = JointsProcess(jointDict, tagDict, joints, tag, tagToIdx);
            if (ret != APP_ERR_OK) {
                return ret;
            }
        }
    }
    for (int i = 0; i < (int)jointDict.size(); i++) {
        matchedRes.push_back(jointDict[i].second);
    }
    return APP_ERR_OK;
}

void HigherHRnetPostProcessDptr::AdjustCore(std::vector<std::vector<std::vector<float>>> &matchedRes,
    cv::Mat& heatMap, int i, int j)
{
    float* heatPtr = (float*) heatMap.data;
    float y = matchedRes[i][j][0];
    float x = matchedRes[i][j][1];
    int yy = static_cast<int> (matchedRes[i][j][0]);
    int xx = static_cast<int> (matchedRes[i][j][1]);
    float* tmpIdx = heatPtr + j * heatMap.cols * heatMap.rows;
    float offVal = 0.25;
    float doubleOff = 0.5;
    if (*(tmpIdx + xx * heatMap.cols + std::min(yy+1, heatMap.cols-1)) >
        *(tmpIdx + xx * heatMap.cols + std::max(yy-1, 0))) {
        y += offVal;
    } else {
        y -= offVal;
    }
    if (*(tmpIdx + std::min(xx+1, heatMap.rows-1)*heatMap.cols + yy) >
        *(tmpIdx + std::max(0, xx-1) * heatMap.cols + yy)) {
        x += offVal;
    } else {
        x -= offVal;
    }
    matchedRes[i][j][0] = y + doubleOff;
    matchedRes[i][j][1] = x + doubleOff;
}

APP_ERROR HigherHRnetPostProcessDptr::Adjust(std::vector<std::vector<std::vector<float>>> &matchedRes, cv::Mat& heatMap)
{
    size_t confIdx = 2;
    for (int i = 0; i < (int)matchedRes.size(); i++) {
        for (int j = 0; j < (int)matchedRes[0].size(); j++) {
            if (matchedRes[i][j][confIdx] <= 0)  continue;
            AdjustCore(matchedRes, heatMap, i, j);
        }
    }
    return APP_ERR_OK;
}

void HigherHRnetPostProcessDptr::ComputeScores(std::vector<std::vector<std::vector<float>>> &matchedRes,
    std::vector<float> &scores)
{
    size_t tagIdx = 2;
    for (int i = 0; i < (int)matchedRes.size(); i++) {
        float score = 0.;
        for (int j = 0; j < (int)matchedRes[0].size(); j++) {
            score += matchedRes[i][j][tagIdx];
        }
        score /= (float) matchedRes[0].size();
        scores.push_back(score);
    }
}

APP_ERROR HigherHRnetPostProcessDptr::Refine(std::vector<std::vector<std::vector<float>>> &matchedRes, cv::Mat& heatMap,
    cv::Mat& tags)
{
    APP_ERROR ret = APP_ERR_OK;
    for (int i = 0; i < (int)matchedRes.size(); i++) {
        ret = RefineCore(heatMap, tags, matchedRes[i]);
        if (ret != APP_ERR_OK) {
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR HigherHRnetPostProcessDptr::RefineCore(cv::Mat& heatMap, cv::Mat& tags,
    std::vector<std::vector<float>>& keypoints)
{
    float* heatMapPtr = (float*) heatMap.data;
    float* tagsPtr = (float*) tags.data;
    int stride = tags.cols * tags.rows;
    float preTag = 0.;
    int cnt = 0;
    size_t confIdx = 2;
    for (int i = 0; i < (int)keypoints.size(); i++) {
        if (keypoints[i][confIdx] > std::numeric_limits<float>::epsilon()) {
            int x = static_cast<int> (keypoints[i][0]);
            int y = static_cast<int> (keypoints[i][1]);
            preTag += (*(tagsPtr + i * stride + y * tags.cols + x));
            cnt += 1;
        }
    }
    if (!cnt) {
        LogError << "Divide 0" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    preTag /= float(cnt);
    std::vector<std::vector<float>> ans;
    for (int i = 0; i < (int)keypoints.size(); i++) {
        float* tmpHeatPtr = heatMapPtr + i * heatMap.cols * heatMap.rows;
        float* tmpTagPtr = tagsPtr + i * tags.cols * tags.rows;
        std::pair<int, int> argmax;
        float minNum = -1e5;
        for (int s = 0; s < (int)tags.cols * tags.rows; s++) {
            float vaa = *(tmpHeatPtr + s) - round(fabs((*(tmpTagPtr + s) - preTag)));
            if (vaa > minNum) {
                minNum = vaa;
                argmax.first = s / tags.cols;
                argmax.second = s % tags.cols;
            }
        }
        GetRefineAns(argmax, heatMap, tmpHeatPtr, ans);
    }
    if (!ans.empty()) {
        FormKeyPoints(keypoints, ans);
    }
    return APP_ERR_OK;
}

void HigherHRnetPostProcessDptr::GetRefineAns(std::pair<int, int>& argmax, cv::Mat& heatMap, float* tmpHeatPtr,
    std::vector<std::vector<float>>& ans)
{
    float x = (float)argmax.second;
    float y = (float)argmax.first;
    int xx = argmax.second;
    int yy = argmax.first;
    float val = *(tmpHeatPtr + yy * heatMap.cols + xx);
    float offVal = 0.25;
    float doubleOff = 0.5;
    x += doubleOff;
    y += doubleOff;
    if (*(tmpHeatPtr + yy * heatMap.cols + std::min(xx+1, heatMap.cols-1)) <
        *(tmpHeatPtr + yy * heatMap.cols + std::max(xx-1, 0))) {
        x -= offVal;
    } else {
        x += offVal;
    }
    if (*(tmpHeatPtr + std::min(yy+1, heatMap.rows-1) * heatMap.cols + xx) <
        *(tmpHeatPtr + std::max(0, yy-1) * heatMap.cols + xx)) {
        y -= offVal;
    } else {
        y += offVal;
    }
    ans.push_back({x, y, val});
}

void HigherHRnetPostProcessDptr::FormKeyPoints(std::vector<std::vector<float>>& keypoints,
    std::vector<std::vector<float>> &ans)
{
    size_t tagIdx = 2;
    float boundVal = 0.;
    for (int i = 0; i < keyPointNum_; i++) {
        if (ans[i][tagIdx] > boundVal && keypoints[i][tagIdx] - 0x0 < EPSILON) {
            for (int j = 0; j < (int)ans[i].size(); j++) {
                keypoints[i][j] = ans[i][j];
            }
        }
    }
}

void HigherHRnetPostProcessDptr::TopKProcess(std::vector<std::vector<std::pair<float, int>>> &topkValHeatAndInd,
    std::vector<std::vector<float>> &topkValTags, std::vector<std::vector<int>> &topkInd,
    std::vector<std::vector<float>> &topkHeatVal, std::vector<cv::Mat> &heatAndTags)
{
    cv::Mat& heatMap = heatAndTags[0];
    cv::Mat& tags = heatAndTags[1];
    float* tagPtr = (float*) tags.data;
    int stride = heatMap.rows * heatMap.cols;
    for (int i = 0; i < keyPointNum_; i++) {
        std::vector<float> tmpTopkTags;
        std::vector<int> tmpTopkInd;
        std::vector<float> tmpTopkVal;
        float* beginTagPtr = tagPtr + i * stride;
        for (int k = 0; k < MAX_PEOPLE_NUM; k++) {
            tmpTopkTags.push_back(*(beginTagPtr + topkValHeatAndInd[i][k].second));
            tmpTopkInd.push_back(topkValHeatAndInd[i][k].second % (int)heatMap.cols);
            tmpTopkInd.push_back(topkValHeatAndInd[i][k].second / (int)heatMap.cols);
            tmpTopkVal.push_back(topkValHeatAndInd[i][k].first);
        }
        topkValTags.push_back(tmpTopkTags);
        topkInd.push_back(tmpTopkInd);
        topkHeatVal.push_back(tmpTopkVal);
    }
}

APP_ERROR HigherHRnetPostProcessDptr::GetTopkPose(std::vector<cv::Mat> &heatAndTags,
    std::vector<std::vector<std::vector<float>>> &matchedRes, std::vector<float> &scores)
{
    cv::Mat& heatMap = heatAndTags[0];
    cv::Mat& tags = heatAndTags[1];
    SparseHeatMap(heatMap);
    std::vector<std::vector<std::pair<float, int>>> topkValHeatAndInd;
    float* sparseHeatPtr = (float*) heatMap.data;
    int stride = heatMap.rows * heatMap.cols;
    float heatMapDataSize = heatMap.total() * heatMap.elemSize() / qPtr_->FOUR_BYTE;
    float maxIndex = (keyPointNum_-1) * stride;
    if ((maxIndex + stride - 1) >= heatMapDataSize || (maxIndex + MAX_PEOPLE_NUM - 1) >= heatMapDataSize) {
        LogError << "The data exceeds the maximum size of the heat map." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (int i = 0; i < keyPointNum_; i++) {
        std::vector<std::pair<float, int>> tmpTopk;
        for (int k = 0; k < MAX_PEOPLE_NUM; k++) {
            tmpTopk.push_back(std::pair<float, int> (*(sparseHeatPtr + i * stride + k), k));
        }
        std::sort(tmpTopk.begin(), tmpTopk.end(), PairComp);
        for (int k = MAX_PEOPLE_NUM; k < stride; k++) {
            std::pair<float, int> targetVal(*(sparseHeatPtr + i * stride + k), k);
            InsertTargetVal(tmpTopk, targetVal);
        }
        topkValHeatAndInd.push_back(tmpTopk);
    }
    std::vector<std::vector<float>> topkValTags;
    std::vector<std::vector<int>> topkInd;
    std::vector<std::vector<float>> topkHeatVal;
    TopKProcess(topkValHeatAndInd, topkValTags, topkInd, topkHeatVal, heatAndTags);
    APP_ERROR ret = Match(topkValTags, topkInd, topkHeatVal, matchedRes);
    if (ret != APP_ERR_OK) {
        LogError << "Match top k pose failed." << GetErrorInfo(ret);
        return ret;
    }
    Adjust(matchedRes, heatMap);
    ComputeScores(matchedRes, scores);
    ret = Refine(matchedRes, heatMap, tags);
    if (ret != APP_ERR_OK) {
        LogError << "Refine pose failed." << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

std::vector<float> HigherHRnetPostProcessDptr::GetScale(const ResizedImageInfo& resizedImageInfo)
{
    std::vector<float> scale;
    int w = resizedImageInfo.widthOriginal;
    int h = resizedImageInfo.heightOriginal;
    int minInputSize = (512 + 63) / 64 * 64;
    if (w < h) {
        int resizeW = minInputSize;
        int resizeH = ((int)(float(minInputSize) / float(w) * float(h) + float(63.)) / 64 * 64);
        scale = {float(w) / float(200.), float(resizeH) / float(resizeW) * float(w) / float(200.)};
    } else {
        int resizeH = minInputSize;
        int resizeW = ((int)(float(minInputSize) / float(h) * float(w) + float(63.)) / 64 * 64);
        scale = {float(resizeW) / float(resizeH) * float(h) / float(200.), (float)h / float(200.)};
    }
    return scale;
}

std::vector<float> HigherHRnetPostProcessDptr::Get3rdPoint(std::vector<float> src0, std::vector<float> src1)
{
    std::vector<float> direct;
    for (int i = 0; i < (int)src0.size(); i++) {
        direct.push_back(src0[i] - src1[i]);
    }
    std::vector<float> res{src1[0] - direct[1], src1[1] + direct[0]};
    return res;
}

cv::Mat HigherHRnetPostProcessDptr::GetAffineTransform(std::vector<int> &center, std::vector<float> &scale,
    std::vector<int> &heatMapSize)
{
    float srcW = scale[0] * 200.;
    int dstW = heatMapSize[0];
    int dstH = heatMapSize[1];
    std::vector<float> srcDir {0, srcW * float(-0.5)};
    std::vector<float> dstDir {0, dstW * float(-0.5)};
    size_t matH = 3;
    size_t matW = 2;
    std::vector<std::vector<float>> src(matH, std::vector<float>(matW, 0.));
    std::vector<std::vector<float>> dst(matH, std::vector<float>(matW, 0.));
    for (int i = 0; i < (int)src[0].size(); i++) {
        src[0][i] = (float) center[i];
    }
    for (int i = 0; i < (int)src[1].size(); i++) {
        src[1][i] = (float) center[i] + srcDir[i];
    }
    size_t affine1st = 0;
    size_t affine2nd = 1;
    size_t affine3rd = 2;
    dst[affine1st] = {(float)dstW * float(0.5), (float)dstH * float(0.5)};
    dst[affine2nd] = {(float)dstW * float(0.5), (float)dstH * float(0.5) + dstDir[1]};
    src[affine3rd] = Get3rdPoint(src[0], src[1]);
    dst[affine3rd] = Get3rdPoint(dst[0], dst[1]);
    cv::Mat trans(0x2, 0x3, CV_64FC1);
    cv::Point2f srcMat[3] = {cv::Point2f(src[0][0], src[0][1]), cv::Point2f(src[1][0], src[1][1]),
        cv::Point2f(src[2][0], src[2][1])};
    cv::Point2f dstMat[3] = {cv::Point2f(dst[0][0], dst[0][1]), cv::Point2f(dst[1][0], dst[1][1]),
        cv::Point2f(dst[2][0], dst[2][1])};
    trans = cv::getAffineTransform(dstMat, srcMat);
    return trans;
}

APP_ERROR HigherHRnetPostProcessDptr::GetFinalPreds(std::vector<std::vector<std::vector<float>>> &matchedRes,
    const ResizedImageInfo& resizedImageInfo, cv::Mat& heatMap, std::vector<std::vector<std::vector<float>>> &finalRes)
{
    std::vector<int> center {static_cast<int> ((float)(resizedImageInfo.widthOriginal) / 2. + 0.5),
        static_cast<int> ((float)(resizedImageInfo.heightOriginal) / 2. + 0.5)};
    std::vector<float> scale = GetScale(resizedImageInfo);

    std::vector<int> heatMapSize {heatMap.cols, heatMap.rows};
    for (int i = 0; i < (int)matchedRes.size(); i++) {
        std::vector<std::vector<float>> targetCoords = matchedRes[i];
        cv::Mat trans = GetAffineTransform(center, scale, heatMapSize);
        if (trans.empty()) {
            LogError << "Affine Transform failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        for (int p = 0; p < (int)matchedRes[i].size(); p++) {
            AffineTrans(targetCoords, trans, p);
        }
        finalRes.push_back(targetCoords);
    }
    return APP_ERR_OK;
}

void HigherHRnetPostProcessDptr::AffineTrans(std::vector<std::vector<float>> &targetCoords, cv::Mat &trans, int p)
{
    double* ptr = (double*) trans.data;
    std::vector<float> pt {targetCoords[p][0], targetCoords[p][1], 1.};
    targetCoords[p][0] = targetCoords[p][1] = 0.;

    for (size_t i = 0; i < pt.size(); i++) {
        targetCoords[p][0] += *(ptr+i) * pt[i];
        targetCoords[p][1] += *(ptr+i+0x3) * pt[i];
    }
}

void HigherHRnetPostProcessDptr::Evaluate(std::vector<std::vector<std::vector<float>>> &finalRes,
    std::vector<float> &scores, std::vector<KeyPointDetectionInfo> &keyPointInfos)
{
    size_t confIdx = 2;
    for (int i = 0; i < (int)finalRes.size(); i++) {
        KeyPointDetectionInfo keyPointInfo;
        for (int j = 0; j < (int)finalRes[i].size(); j++) {
            keyPointInfo.keyPointMap[j].push_back(finalRes[i][j][0]);
            keyPointInfo.keyPointMap[j].push_back(finalRes[i][j][1]);
            keyPointInfo.scoreMap[j] = (finalRes[i][j][confIdx]);
        }
        keyPointInfo.score = scores[i];
        keyPointInfos.push_back(keyPointInfo);
    }
}

APP_ERROR HigherHRnetPostProcessDptr::KeyPointsDetect(const std::vector<TensorBase> &tensors,
    const std::vector<ResizedImageInfo> &resizedImageInfos,
    std::vector<std::vector<KeyPointDetectionInfo>> &keyPointInfos)
{
    size_t heightIdx = 2;
    size_t widthIdx = 3;
    imageFeatHight_ = static_cast<int>((tensors[0].GetShape())[heightIdx]);
    imageFeatWidth_ = static_cast<int>((tensors[0].GetShape())[widthIdx]);
    auto tensor = tensors[0];
    auto shape = tensor.GetShape();
    uint32_t batchSize = shape[0];
    APP_ERROR ret = APP_ERR_OK;
    if (tensors[0].GetShape()[heightIndex_] == 0 || tensors[0].GetShape()[widthIndex_] == 0) {
        LogError << "The number of tensor'shape[" << heightIndex_ << "] and ["<< widthIndex_
                 << "]must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (uint32_t i = 0; i < batchSize; i++) {
        float* dataPtr1st = (float*)(tensor.GetBuffer()) + i * (tensor.GetStrides())[0] / qPtr_->FOUR_BYTE;
        if (dataPtr1st == nullptr) {
            return APP_ERR_COMM_INVALID_POINTER;
        }
        cv::Mat srcImg(imageFeatHight_, imageFeatWidth_, CV_32FC(static_cast<size_t>(channelNum_)), dataPtr1st);
        cv::Mat dstImg;
        double scaleX = (double)(tensors[1].GetShape()[heightIndex_]) / (double)(tensors[0].GetShape()[heightIndex_]);
        double scaleY = (double)(tensors[1].GetShape()[widthIndex_]) / (double)(tensors[0].GetShape()[widthIndex_]);
        BiInterLinear(srcImg, dstImg, scaleX, scaleY);
        float* interPolateTensor1st = (float*)dstImg.data;
        float* interPolateTensor2nd = (float*)(tensors[1].GetBuffer()) +
            i * (tensors[1].GetStrides())[0] / (qPtr_->FOUR_BYTE);
        if (interPolateTensor1st == nullptr || interPolateTensor2nd == nullptr) {
            return APP_ERR_COMM_INVALID_POINTER;
        }
        std::vector<cv::Mat> heatAndTags = GetHeatMapAndTags(interPolateTensor1st, interPolateTensor2nd,
            tensors[1].GetShape()[heightIndex_], tensors[1].GetShape()[widthIndex_]);
        std::vector<std::vector<std::vector<float>>> matchedRes;
        std::vector<float> scores;
        ret = GetTopkPose(heatAndTags, matchedRes, scores);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        std::vector<std::vector<std::vector<float>>> finalRes;
        ret = GetFinalPreds(matchedRes, resizedImageInfos[i], heatAndTags[0], finalRes);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        std::vector<KeyPointDetectionInfo> keyPointVec;
        Evaluate(finalRes, scores, keyPointVec);
        keyPointInfos.push_back(keyPointVec);
    }
    return ret;
}
}

#endif
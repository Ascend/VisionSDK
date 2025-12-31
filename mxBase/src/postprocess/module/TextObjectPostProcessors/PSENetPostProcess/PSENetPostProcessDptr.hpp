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
 * Description: PSENetPostProcess private interface for internal use only.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef PSENETPOSTPROCESS_DPTR_H
#define PSENETPOSTPROCESS_DPTR_H

#include "TextObjectPostProcessors/PSENetPostProcess.h"
#include "MxBase/GlobalManager/GlobalManager.h"

namespace MxBase {
const int MODEL_OUTPUT_HEIGHT = 704;
const int MODEL_OUTPUT_WIDTH = 1216;
const int DIRECT_NUM = 4;
const int KERNEL_DIM = 1;
const int HEIGHT_DIM = 2;
const int WEIGHT_DIM = 3;
}

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER PSENetPostProcessDptr {
public:
    explicit PSENetPostProcessDptr(PSENetPostProcess* qPtr);

    ~PSENetPostProcessDptr() = default;

    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;

    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);

    APP_ERROR ObjectDetectionOutput(const std::vector<TensorBase> &tensors,
        std::vector<std::vector<TextObjectInfo>> &textObjInfos, const std::vector<ResizedImageInfo> &resizedImageInfos);

    static void Pse(const std::vector<std::vector<std::vector<unsigned int>>> &kernels, const float &minAreaPse,
        std::vector<std::vector<int>> &textLine);

    static void GetKernelsMat(const std::vector<std::vector<std::vector<unsigned int>>> &kernels,
        std::vector<cv::Mat> &kernelsMat);

    static void GrowingTextLine(const std::vector<cv::Mat> &kernelsMat, const float &minAreaPse,
        std::vector<std::vector<int>> &textLine);

    static void BfsForTextLine(const std::vector<cv::Mat> &kernelsMat, std::queue<cv::Point> curQueue,
        std::vector<std::vector<int>> &textLine);

    void GenerateBoxes(const std::vector<std::vector<int>> &textLine, const std::vector<std::vector<float>> &score,
        const ResizedImageInfo &resizedImageInfos, std::vector<TextObjectInfo> &textObjInfos) const;

    APP_ERROR GetSrcData(const std::vector<TensorBase> &tensors, const int &i,
        std::vector<std::vector<std::vector<unsigned int>>> &kernels, std::vector<std::vector<float>> &score) const;

    float min_kernel_area_ = .0f;
    float pse_scale_ = .0f;
    float min_score_ = .0f;
    float min_area_ = .0f;
    int kernel_num_ = 0;

public:
    PSENetPostProcess *qPtr_ = nullptr;
    int objectNumTensor_ = DEFAULT_OBJECT_NUM_TENSOR;
};

PSENetPostProcessDptr::PSENetPostProcessDptr(PSENetPostProcess* qPtr)
    :qPtr_(qPtr)
{}

bool PSENetPostProcessDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    LogDebug << "Start to check the output tensor of model";
    if (tensors.size() < 0x1) {
        LogError << "The number of tensors (" << tensors.size() << ") "
                 << "is less than required (" << 0x1 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    auto outputShape = tensors[objectNumTensor_].GetShape();
    std::vector<int>shape;
    for (size_t i = 0; i < outputShape.size(); i++) {
        shape.push_back(static_cast<int>(outputShape[i]));
    }
    if (outputShape.size() != DIRECT_NUM) {
        LogError << "The number of tensor[0] dimensions (" << outputShape.size() << ") "
                 << "is not equal to (" << DIRECT_NUM << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (shape[KERNEL_DIM] != kernel_num_ ||  shape[HEIGHT_DIM] != MODEL_OUTPUT_HEIGHT
        || shape[WEIGHT_DIM] != MODEL_OUTPUT_WIDTH) {
            LogError << "Please Check output tensors[0]'s shape"
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
    }
    LogDebug << "End to check the output tensor of model";
    return true;
}

APP_ERROR PSENetPostProcessDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "The input tensors are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    return qPtr_->CheckAndMoveTensors(tensors);
}

void SetCurQueue(const std::vector<cv::Mat> &kernelsMat, const int &kernelId, std::vector<std::vector<int>> &textLine,
    std::queue<cv::Point> &curQueue, bool &isEdge)
{
    std::vector<int> dx = { -1, 1, 0, 0 };
    std::vector<int> dy = { 0, 0, -1, 1 };
    cv::Point point = curQueue.front();
    curQueue.pop();
    int x = point.x;
    int y = point.y;
    int label = textLine[x][y];
    for (int d = 0; d < DIRECT_NUM; ++d) {
        int tmpX = x + dx[d];
        int tmpY = y + dy[d];
        if (tmpX < 0 || tmpX >= (int)textLine.size()) {
            continue;
        }
        if (tmpY < 0 || tmpY >= (int)textLine[1].size()) {
            continue;
        }
        if (kernelsMat[kernelId].at<char>(tmpX, tmpY) == 0) {
            continue;
        }
        if (textLine[tmpX][tmpY] > 0) {
            continue;
        }
        cv::Point pointTmp(tmpX, tmpY);
        curQueue.push(pointTmp);
        textLine[tmpX][tmpY] = label;
        isEdge = false;
    }
}

void PSENetPostProcessDptr::BfsForTextLine(const std::vector<cv::Mat> &kernelsMat, std::queue<cv::Point> curQueue,
    std::vector<std::vector<int>> &textLine)
{
    std::queue<cv::Point> nextQueue;
    for (int kernelId = (int)kernelsMat.size() - 0x2; kernelId >= 0; --kernelId) {
        while (!curQueue.empty()) {
            bool isEdge = true;
            cv::Point point = curQueue.front();
            SetCurQueue(kernelsMat, kernelId, textLine, curQueue, isEdge);
            if (isEdge) {
                nextQueue.push(point);
            }
        }
        swap(curQueue, nextQueue);
    }
}

void PSENetPostProcessDptr::GrowingTextLine(const std::vector<cv::Mat> &kernelsMat, const float &minAreaPse,
    std::vector<std::vector<int>> &textLine)
{
    cv::Mat labelMat;
    int labelNum = cv::connectedComponents(kernelsMat[kernelsMat.size() - 1], labelMat, DIRECT_NUM);
    LogDebug << "label num: " << labelNum;
    std::vector<int> area(labelNum + 1, 0);
    for (int x = 0; x < labelMat.rows; ++x) {
        for (int y = 0; y < labelMat.cols; ++y) {
            int label = labelMat.at<int>(x, y);
            if (label == 0) {
                continue;
            }
            area[label] += 1;
        }
    }
    std::queue<cv::Point> curQueue;
    for (int x = 0; x < labelMat.rows; ++x) {
        std::vector<int> row(labelMat.cols);
        for (int y = 0; y < labelMat.cols; ++y) {
            int label = labelMat.at<int>(x, y);
            if (label == 0) {
                continue;
            }
            if (area[label] < minAreaPse) {
                continue;
            }
            cv::Point point(x, y);
            curQueue.push(point);
            row[y] = label;
        }
        textLine.emplace_back(row);
    }
    BfsForTextLine(kernelsMat, curQueue, textLine);
}

void PSENetPostProcessDptr::GetKernelsMat(const std::vector<std::vector<std::vector<unsigned int>>> &kernels,
    std::vector<cv::Mat> &kernelsMat)
{
    for (const auto &kernel : kernels) {
        cv::Mat kernelMat = cv::Mat::zeros(MODEL_OUTPUT_HEIGHT, MODEL_OUTPUT_WIDTH, CV_8UC1);
        for (size_t x = 0; x < MODEL_OUTPUT_HEIGHT; x++) {
            for (size_t y = 0; y < MODEL_OUTPUT_WIDTH; y++) {
                kernelMat.at<char>(x, y) = kernel[x][y];
            }
        }
        kernelsMat.emplace_back(kernelMat);
    }
}

void PSENetPostProcessDptr::Pse(const std::vector<std::vector<std::vector<unsigned int>>> &kernels,
    const float &minAreaPse, std::vector<std::vector<int>> &textLine)
{
    // get kernals for opencv's Mat
    std::vector<cv::Mat> kernelsMat;
    GetKernelsMat(kernels, kernelsMat);
    // get text line
    GrowingTextLine(kernelsMat, minAreaPse, textLine);
}

void PSENetPostProcessDptr::GenerateBoxes(const std::vector<std::vector<int>> &textLine,
    const std::vector<std::vector<float>> &score, const ResizedImageInfo &resizedImageInfos,
    std::vector<TextObjectInfo> &textObjInfos) const
{
    std::vector<float> scale = {(float)resizedImageInfos.widthOriginal / textLine[0].size(),
        (float)resizedImageInfos.heightOriginal / textLine.size()
    };
    std::unordered_map<int, float> scoreLabelSum;
    std::map<int, std::vector<std::vector<size_t>>> pointMapDest;
    for (size_t row = 0; row < textLine.size(); row++) {
        for (size_t col = 0; col < textLine[0].size(); col++) {
            scoreLabelSum[textLine[row][col]] += score[row][col];
            if (pointMapDest[textLine[row][col]].empty()) {
                std::vector<size_t> temp = { row, col };
                pointMapDest[textLine[row][col]] = {temp};
            } else {
                std::vector<size_t> temp = { row, col };
                pointMapDest[textLine[row][col]].push_back(temp);
            }
        }
    }
    for (auto pointMap = pointMapDest.begin(); pointMap != pointMapDest.end();) {
        if (pointMap->first == 0) {
            ++pointMap;
            continue;
        }
        std::vector<cv::Point> points;
        if ((pointMap->second.size() < min_area_) || pointMap->second.empty() ||
            (scoreLabelSum[pointMap->first] / pointMap->second.size()) < min_score_) {
            ++pointMap;
            continue;
        }
        for (auto pointValue : pointMap->second) {
            cv::Point point(pointValue[1], pointValue[0]);
            points.push_back(point);
        }
        cv::RotatedRect rect = cv::minAreaRect(points);
        cv::Mat bbox;
        TextObjectInfo textObjDetectInfo {};
        cv::boxPoints(rect, bbox);
        textObjDetectInfo.x3 = bbox.at<float>(0x0, 0x0) * scale[0x0];
        textObjDetectInfo.y3 = bbox.at<float>(0x0, 0x1) * scale[0x1];
        textObjDetectInfo.x0 = bbox.at<float>(0x1, 0x0) * scale[0x0];
        textObjDetectInfo.y0 = bbox.at<float>(0x1, 0x1) * scale[0x1];
        textObjDetectInfo.x1 = bbox.at<float>(0x2, 0x0) * scale[0x0];
        textObjDetectInfo.y1 = bbox.at<float>(0x2, 0x1) * scale[0x1];
        textObjDetectInfo.x2 = bbox.at<float>(0x3, 0x0) * scale[0x0];
        textObjDetectInfo.y2 = bbox.at<float>(0x3, 0x1) * scale[0x1];
        ++pointMap;
        textObjInfos.push_back(textObjDetectInfo);
    }
}

APP_ERROR PSENetPostProcessDptr::GetSrcData(const std::vector<TensorBase> &tensors, const int &i,
    std::vector<std::vector<std::vector<unsigned int>>> &kernels, std::vector<std::vector<float>> &score) const
{
    float *kernelsPtr = (float *)qPtr_->GetBuffer(tensors[objectNumTensor_], i);
    if (kernelsPtr == nullptr) {
        LogError << "The kernelsPtr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    for (auto t = 0; t < kernel_num_; t++) {
        for (auto j = 0; j < MODEL_OUTPUT_HEIGHT; j++) {
            for (auto k = 0; k < MODEL_OUTPUT_WIDTH; k++) {
                float value = kernelsPtr[k + j * MODEL_OUTPUT_WIDTH + t * MODEL_OUTPUT_HEIGHT * MODEL_OUTPUT_WIDTH];
                float kernelsZeroLayer = kernelsPtr[k + j * MODEL_OUTPUT_WIDTH];
                score[j][k] = fastmath::sigmoid(kernelsZeroLayer);
                float outPut = (fastmath::sign(value - 1.0f) + 1) / 0x2;
                float text = (fastmath::sign(kernelsZeroLayer - 1.0f) + 1) / 0x2;
                kernels[t][j][k] = outPut * text;
            }
        }
    }
    return APP_ERR_OK;
}

APP_ERROR PSENetPostProcessDptr::ObjectDetectionOutput(const std::vector<TensorBase> &tensors,
    std::vector<std::vector<TextObjectInfo>> &textObjInfos, const std::vector<ResizedImageInfo> &resizedImageInfos)
{
    LogDebug << "PSENetPostProcess start to write results.";
    auto shape = tensors[objectNumTensor_].GetShape();
    uint32_t batchSize = shape[0];
    std::vector<std::vector<float>> textProposals;
    std::vector<float> scores;
    clock_t startTime, endTime;
    startTime = clock();
    if (resizedImageInfos.size() < batchSize) {
        LogError << "Size of resizedImageInfos is invalid, size: "
                 << resizedImageInfos.size() << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (uint32_t i = 0; i < batchSize; i++) {
        std::vector<TextObjectInfo> textObjInfo;
        ResizedImageInfo resizedImageInfo = resizedImageInfos[i];
        LogDebug << "imgInfo.imgHeight: " << resizedImageInfo.heightOriginal;
        LogDebug << "imgInfo.imgWidth: " << resizedImageInfo.widthOriginal;
        LogDebug << "imgInfo.modelHeight: " << resizedImageInfo.heightResize;
        LogDebug << "imgInfo.modelWidth: " << resizedImageInfo.widthResize;
        std::vector<std::vector<std::vector<unsigned int>>> kernels(kernel_num_,
            std::vector<std::vector<unsigned int>>(MODEL_OUTPUT_HEIGHT, std::vector<unsigned int>(MODEL_OUTPUT_WIDTH)));
        std::vector<std::vector<float>> score(MODEL_OUTPUT_HEIGHT, std::vector<float>(MODEL_OUTPUT_WIDTH, 0));
        APP_ERROR ret = GetSrcData(tensors, i, kernels, score);
        if (ret != APP_ERR_OK) {
            LogError << "GetSrcData failed." << GetErrorInfo(ret);
            return ret;
        }
        std::vector<std::vector<int>> textLine;
        // pse algorithm
        if (IsDenominatorZero(pse_scale_)) {
            LogError << "The value of PSE_SCALE must not equal to zero!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        Pse(kernels, min_kernel_area_ / (pse_scale_ * pse_scale_), textLine);
        if (textLine.empty() || textLine[0].empty()) {
            LogWarn << "Can't detect text lines";
            continue;
        }
        // get boxes
        GenerateBoxes(textLine, score, resizedImageInfo, textObjInfo);
        textObjInfos.push_back(textObjInfo);
    }
    endTime = clock();
    auto dur = (double)(endTime - startTime);
    LogInfo << "PSENet sdk run time: " << dur / CLOCKS_PER_SEC << "s";
    LogDebug << "PSENetPostProcess write results successed.";
    return APP_ERR_OK;
}
}

#endif
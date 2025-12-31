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
 * Description: Private interface of the UNetMindSporePostProcess for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef UNETMINDSPORE_POST_PROCESS_DPTR_H
#define UNETMINDSPORE_POST_PROCESS_DPTR_H

#include "MxBase/GlobalManager/GlobalManager.h"

namespace MxBase {
class SDK_UNAVAILABLE_FOR_OTHER UNetMindSporePostProcessDptr {
public:
    explicit UNetMindSporePostProcessDptr(UNetMindSporePostProcess* pUNetMindSporePostProcess);

    UNetMindSporePostProcessDptr(const UNetMindSporePostProcessDptr &other);

    UNetMindSporePostProcessDptr& operator=(const UNetMindSporePostProcessDptr &other);

    ~UNetMindSporePostProcessDptr() = default;

    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);

    bool TestModelType(const std::vector<TensorBase> &tensors) const;

    bool IsValidTensors(const std::vector<TensorBase> &tensors) const;

    APP_ERROR ArgmaxSemanticSegOutput(const std::vector<TensorBase> &tensors,
        const std::vector<ResizedImageInfo> &resizedImageInfos, std::vector<SemanticSegInfo> &semanticSegInfos);

    int GenArgmaxPixelIndex(const uint32_t &classNum, const uint32_t &classIndexOffset,
    	const float *tensorPtr, const uint32_t modelSizeProduct, const size_t tensorSize);

    APP_ERROR ModelArgmaxDirectOutput(const std::vector<TensorBase>& tensors,
    	const std::vector<ResizedImageInfo>& resizedImageInfos, std::vector<SemanticSegInfo> &semanticSegInfos);

    APP_ERROR SetSemanticSegPixels(const size_t tensorSize, const uint32_t outputModelHeight,
        const uint32_t outputModelWidth, const float *tensorPtr, SemanticSegInfo &semanticSegInfo);

    enum PostType {
        ARGMAX_RESIZE_NEAR = 0,
        OM_ARGMAX_DICT_OUT = 1
    };
public:
    uint32_t postType_ = 0;
    uint32_t resizeType_ = 1;
    uint32_t heightIndex_ = 1;
    uint32_t widthIndex_ = 2;
    UNetMindSporePostProcess* qPtr_ = nullptr;
    const uint32_t MAX_IMAGE_EDGE = 8192;
};

UNetMindSporePostProcessDptr::UNetMindSporePostProcessDptr(UNetMindSporePostProcess *pUNetMindSporePostProcess)
    : qPtr_(pUNetMindSporePostProcess)
{}

UNetMindSporePostProcessDptr::UNetMindSporePostProcessDptr(const UNetMindSporePostProcessDptr &other)
{
    *this = other;
}

UNetMindSporePostProcessDptr &UNetMindSporePostProcessDptr::operator=(const UNetMindSporePostProcessDptr &other)
{
    if (this == &other) {
        return *this;
    }
    heightIndex_ = other.heightIndex_;
    widthIndex_ = other.widthIndex_;
    return *this;
}

APP_ERROR UNetMindSporePostProcessDptr::CheckAndMoveTensors(std::vector<TensorBase> &tensors)
{
    if (qPtr_->checkModelFlag_ && !IsValidTensors(tensors)) {
        LogError << "Input tensors are invalid." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return qPtr_->CheckAndMoveTensors(tensors);
}

bool UNetMindSporePostProcessDptr::TestModelType(const std::vector<TensorBase> &tensors) const
{
    auto shape = tensors[0].GetShape();
    if (postType_ == ARGMAX_RESIZE_NEAR) {
        if (shape.size() != SHAPE_SIZE_4) {
            LogError << "The number of tensor dimensions (" << shape.size() << ") " << "is not equal to ("
                     << SHAPE_SIZE_4 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    } else {
        if (shape.size() != SHAPE_SIZE_3) {
            LogError << "The number of tensor dimensions (" << shape.size() << ") " << "is not equal to ("
                     << SHAPE_SIZE_3 << ")" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
    }
    if (qPtr_->modelType_ == TYPE_NHWC) {
        if (shape.size() != SHAPE_SIZE_4 || shape[VECTOR_FOURTH_INDEX] != qPtr_->classNum_) {
            return false;
        }
    } else if (qPtr_->modelType_ == TYPE_NCHW) {
        if (shape[0x1] != qPtr_->classNum_) {
            return false;
        }
    } else if (qPtr_->modelType_ == TYPE_NHW) {
        LogInfo << "NOTE: model output type is NHW";
    } else if (qPtr_->modelType_ == TYPE_NWH) {
        LogInfo << "NOTE: model output type is NWH";
    }
    return true;
}

bool UNetMindSporePostProcessDptr::IsValidTensors(const std::vector<TensorBase> &tensors) const
{
    if (tensors.empty()) {
        LogError << "The tensors size is empty, at least one." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    if (tensors[0].GetDataTypeSize() != qPtr_->FOUR_BYTE) {
        LogError << "The tensor type(" << TensorDataTypeStr[tensors[0].GetTensorType()]
                 << ") mismatched. requires(" << qPtr_->FOUR_BYTE << ") bytes tensortype."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return TestModelType(tensors);
}

int UNetMindSporePostProcessDptr::GenArgmaxPixelIndex(const uint32_t &classNum, const uint32_t &classIndexOffset,
    const float *tensorPtr, const uint32_t modelSizeProduct, const size_t tensorSize)
{
    int maxIndex = 0;
    uint32_t curPtr = classIndexOffset;
    float maxValue = tensorPtr[curPtr];
    for (uint32_t cl = 0; cl < classNum; cl++) {
        curPtr = cl * modelSizeProduct + classIndexOffset;
        if (tensorSize <= curPtr) {
            LogError << "The tensorSize must be larger than curPtr." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return -1;
        }
        if (tensorPtr[curPtr] > maxValue) {
            maxValue = tensorPtr[curPtr];
            maxIndex = static_cast<int>(cl);
        }
    }
    return maxIndex;
}

APP_ERROR UNetMindSporePostProcessDptr::ArgmaxSemanticSegOutput(const std::vector<TensorBase>& tensors,
    const std::vector<ResizedImageInfo>& resizedImageInfos, std::vector<SemanticSegInfo> &semanticSegInfos)
{
    LogDebug << "UNetMindSporePostProcess start to write results.";
    auto tensor = tensors[0];
    auto shape = tensor.GetShape();
    uint32_t batchSize = shape[0];

    for (uint32_t i = 0; i < batchSize; i++) {
        uint32_t outputModelWidth = resizedImageInfos[i].widthResize;
        uint32_t outputModelHeight = resizedImageInfos[i].heightResize;
        if (outputModelHeight > MAX_IMAGE_EDGE || outputModelWidth > MAX_IMAGE_EDGE) {
            LogError << "The resizedHeight or resizedWidth is out of range. resizedHeight: " << outputModelWidth
                     << "resizedHeight: " << outputModelHeight << ", maxImageEdge: " << MAX_IMAGE_EDGE << "."
                     << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
            return APP_ERR_COMM_OUT_OF_RANGE;
        }
        size_t tensorSize = tensor.GetSize();
        SemanticSegInfo semanticSegInfo;
        // Argmax between classes.
        float *tensorPtr = (float*)qPtr_->GetBuffer(tensor, i);
        if (tensorPtr == nullptr) {
            LogError << "The tensorPtr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        APP_ERROR ret = SetSemanticSegPixels(tensorSize, outputModelHeight, outputModelWidth,
                                             tensorPtr, semanticSegInfo);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to set SemanticSegInfo of pixels." << GetErrorInfo(ret);
            return ret;
        }
        semanticSegInfo.labelMap = qPtr_->labelMap_;
        semanticSegInfos.push_back(semanticSegInfo);
    }
    LogDebug << "semanticSegInfos. " << semanticSegInfos[0].pixels[0].size();

    LogDebug << "UNetMindSporePostProcess write results successed.";
    return APP_ERR_OK;
}

APP_ERROR UNetMindSporePostProcessDptr::SetSemanticSegPixels(const size_t tensorSize, const uint32_t outputModelHeight,
    const uint32_t outputModelWidth, const float *tensorPtr, SemanticSegInfo &semanticSegInfo)
{
    uint32_t modelSizeProduct = outputModelWidth * outputModelHeight;
    for (uint32_t y = 0; y < outputModelHeight; y++) {
        std::vector<int> result;
        for (uint32_t x = 0; x < outputModelWidth; x++) {
            uint32_t classIndexOffset = y * outputModelWidth + x;
            int indexNum = GenArgmaxPixelIndex(qPtr_->classNum_, classIndexOffset,
                                               tensorPtr, modelSizeProduct, tensorSize);
            if (indexNum == -1) {
                LogError << "Failed to get indexNum." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
                return APP_ERR_COMM_OUT_OF_RANGE;
            }
            result.push_back(indexNum);
        }
        semanticSegInfo.pixels.push_back(result);
    }
    return APP_ERR_OK;
}

APP_ERROR UNetMindSporePostProcessDptr::ModelArgmaxDirectOutput(const std::vector<TensorBase>& tensors,
    const std::vector<ResizedImageInfo>&, std::vector<SemanticSegInfo> &semanticSegInfos)
{
    LogInfo << "UNetMindSpore ModelArgmaxDirectOutput start to write results.";
    auto tensor = tensors[0];
    auto shape = tensor.GetShape();
    uint32_t batchSize = shape[0];
    for (uint32_t i = 0; i < batchSize; i++) {
        uint32_t outputModelWidth = tensor.GetShape()[widthIndex_];
        uint32_t outputModelHeight = tensor.GetShape()[heightIndex_];
        std::cout << outputModelWidth << ", " << outputModelHeight << std::endl;
        SemanticSegInfo semanticSegInfo;
        std::vector<int> subPixel(outputModelWidth);
        semanticSegInfo.pixels.resize(outputModelHeight, subPixel);
        std::vector<std::vector<int>> results(outputModelHeight, std::vector<int>(outputModelWidth));
        auto tensorPtr = (int32_t *)qPtr_->GetBuffer(tensor, i);
        if (tensorPtr == nullptr) {
            LogError << "The tensorPtr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        for (uint32_t y = 0; y < outputModelHeight; y++) {
            for (uint32_t x = 0; x < outputModelWidth; x++) {
                auto curPtr = y * outputModelWidth + x;
                semanticSegInfo.pixels[y][x] = tensorPtr[curPtr];
            }
        }
        semanticSegInfo.labelMap = qPtr_->labelMap_;
        semanticSegInfos.push_back(semanticSegInfo);
    }
    LogDebug << "UNetMindSpore ModelArgmaxDirectOutput write results successed.";
    return APP_ERR_OK;
}
}
#endif

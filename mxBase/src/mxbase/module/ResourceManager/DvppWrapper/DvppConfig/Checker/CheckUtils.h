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
 * Description: Check Utils Class.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#ifndef MXBASE_CHECKUTILS_H
#define MXBASE_CHECKUTILS_H
#include "MxBase/E2eInfer/DataType.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"

namespace {
constexpr uint32_t ZERO_NUM = 0;
}

namespace MxBase {

struct HeightAndWidthConstrainInfo {
    uint32_t wMin;
    uint32_t wMax;
    uint32_t hMin;
    uint32_t hMax;
};

struct CommonConstrainInfo {
    HeightAndWidthConstrainInfo strideShapeConstrain;
    uint32_t widthStrideAlign;
    uint32_t heightStrideAlign;
    float memorySizeRatio;
};

struct OutputConstrainInfo {
    uint32_t outputWidthAlign;
    uint32_t outputHeightAlign;
    float widthStrideRatio;
    float heightStrideRatio;
    float outputMemoryRatio;
};

struct ResizeConstrainConfig {
    HeightAndWidthConstrainInfo resizeScaleConstrain;
    float ratioMin;
    float ratioMax;
    uint32_t inputHeightAlign;
    uint32_t inputWidthAlign;
    uint32_t resizeAreaHeightAlign;
    uint32_t resizeAreaWidthAlign;
    std::vector <Interpolation> interpolationRange;
    HeightAndWidthConstrainInfo inputShapeConstrainInfo;
    CommonConstrainInfo commonConstrainInfo;
    OutputConstrainInfo outputConstrainInfo;
};

struct ResizeRatio {
    float ratioMin;
    float ratioMax;
};

struct CropPasteConstrainConfig {
    HeightAndWidthConstrainInfo pasteImageShapeConstrain;
    uint32_t cropWMin;
    uint32_t cropHMin;
    HeightAndWidthConstrainInfo pasteAreaShapeConstrain;
    ResizeRatio* resizeRatio;
    HeightAndWidthConstrainInfo inputShapeConstrainInfo;
    CommonConstrainInfo commonConstrainInfo;
};

struct CropConstrainConfig {
    HeightAndWidthConstrainInfo cropAreaShapeConstrain;
    uint32_t inputSizeMin;
    uint32_t inputSizeMax;
    uint32_t cropAreaSizeMin;
    uint32_t cropAreaSizeMax;
    uint32_t outputSizeMin;
    uint32_t outputSizeMax;
    uint32_t widthAlign;
    uint32_t heightAlign;
    HeightAndWidthConstrainInfo inputShapeConstrainInfo;
    CommonConstrainInfo commonConstrainInfo;
    OutputConstrainInfo outputConstrainInfo;
};

struct PaddingConstrainConfig {
    HeightAndWidthConstrainInfo inputShapeConstrainInfo;
    HeightAndWidthConstrainInfo outputShapeConstrain;
    uint32_t colorMin;
    uint32_t colorMax;
    std::vector<BorderType> borderTypeRange;
    CommonConstrainInfo commonConstrainInfo;
    OutputConstrainInfo outputConstrainInfo;
};

struct EncodeConstrainConfig {
    uint32_t encodeLevelMin;
    uint32_t encodeLevelMax;
    uint32_t rgbAndBgrChannel;
    uint32_t yuvAndYvuChannel;
    HeightAndWidthConstrainInfo inputShapeConstrainInfo;
    CommonConstrainInfo commonConstrainInfo;
    OutputConstrainInfo outputConstrainInfo;
};

struct CropResizeConstrainConfig {
    uint32_t cropWMin;
    uint32_t cropHMin;
    HeightAndWidthConstrainInfo resizeScaleConstrain;
    float ratioMin;
    float ratioMax;
    uint32_t widthAlign;
    uint32_t heightAlign;
    HeightAndWidthConstrainInfo inputShapeConstrainInfo;
    CommonConstrainInfo commonConstrainInfo;
    OutputConstrainInfo outputConstrainInfo;
};

struct ConvertFormatConstrainConfig {
    HeightAndWidthConstrainInfo inputShapeConstrainInfo;
    CommonConstrainInfo commonConstrainInfo;
    OutputConstrainInfo outputConstrainInfo;
};

class CheckUtils {
public:
    static APP_ERROR VpcPictureConstrainInfoCheck(const DvppDataInfo& inputDataInfo,
                                                  const CommonConstrainInfo &constrainInfo)
    {
        if (!IsWidthAndHeightStrideLegal(inputDataInfo, constrainInfo)) {
            LogError << "The image widthStride or heightStride is not supported. format(" << inputDataInfo.format
                     << "), widthStride(" << inputDataInfo.widthStride << "), heightStride("
                     << inputDataInfo.heightStride << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }

        uint32_t dataSize = inputDataInfo.widthStride * inputDataInfo.heightStride * constrainInfo.memorySizeRatio;
        if (inputDataInfo.dataSize != dataSize) {
            LogError << "Aligned input image data size not match the dvpp image data size. dataSize(" << dataSize <<
                     "), imageSize(" << inputDataInfo.dataSize << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        return APP_ERR_OK;
    }

    template<class T>
    static void NotInRangeErrorInfo(const std::string& moduleName, const std::string& paramName, const T& currentValue,
                                    const T& minValue, const T& maxValue)
    {
        LogError << moduleName << ": Current " << paramName << " is invalid. " << "The " << paramName
                 << " should be in the range of [" << minValue << ", " << maxValue << "], but current value is "
                 << currentValue << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
    }

    template<class T>
    static void NotEqualErrorInfo(const std::string& moduleName, const std::string& paramName, const T& currentValue,
                                  const T& targetValue)
    {
        LogError << moduleName << ": The " << paramName << " should be equal to " << targetValue
                 << ", but current value is " << currentValue << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
    }

    template<class T>
    static void NotEqualErrorInfo(const std::string& moduleName, const std::string& paramName,
                                  const std::string& targetName, const T& currentValue, const T& targetValue)
    {
        LogError << moduleName << ": The " << paramName << " should be equal to " << targetName << "."
                 << "The value of " << paramName << " is " << currentValue << ", the value of " << targetName << " is "
                 << targetValue << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
    }

private:
    static bool IsWidthAndHeightStrideLegal(const DvppDataInfo &inputDataInfo,
                                            const CommonConstrainInfo &constrainInfo)
    {
        uint32_t width = inputDataInfo.width;
        uint32_t height = inputDataInfo.height;
        uint32_t widthStride = inputDataInfo.widthStride;
        uint32_t heightStride = inputDataInfo.heightStride;
        uint32_t minWidthStride = constrainInfo.strideShapeConstrain.wMin;
        uint32_t maxWidthStride = constrainInfo.strideShapeConstrain.wMax;
        uint32_t minHeightStride = constrainInfo.strideShapeConstrain.hMin;
        uint32_t maxHeightStride = constrainInfo.strideShapeConstrain.hMax;
        if (widthStride < minWidthStride || widthStride > maxWidthStride) {
            LogError << "Aligned width(" << widthStride << ") out of range[" << minWidthStride
                     << "," << maxWidthStride << "]." << GetErrorInfo(APP_ERR_DVPP_INVALID_IMAGE_WIDTH);
            return false;
        }
        if (heightStride < minHeightStride || heightStride > maxHeightStride) {
            LogError << "Aligned height(" << heightStride << ") out of range[" << minHeightStride << ","
                     << maxHeightStride << "]." << GetErrorInfo(APP_ERR_DVPP_INVALID_IMAGE_HEIGHT);
            return false;
        }

        if (widthStride % constrainInfo.widthStrideAlign || heightStride % constrainInfo.heightStrideAlign) {
            LogError << "Width stride:" << widthStride << " is not " << constrainInfo.widthStrideAlign
                     << "-aligned" << " or height stride:" << heightStride << " is not "
                     << constrainInfo.heightStrideAlign << "-aligned." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        if ((heightStride < height) || (widthStride < width)) {
            LogError << "Width stride:" << widthStride << " or height stride:"
                     << heightStride << " should be larger than or equal to the original size:"
                     << "(" << width << ", " << height << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return false;
        }
        return true;
    }
};

}

#endif

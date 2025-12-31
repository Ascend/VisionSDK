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
 * Description: Python-based ImageProcessor.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef PY_IMAGE_PROCESSOR_H
#define PY_IMAGE_PROCESSOR_H

#include <vector>
#include <string>
#include "MxBase/E2eInfer/ImageProcessor/ImageProcessor.h"
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "MxBase/E2eInfer/Rect/Rect.h"
#include "MxBase/E2eInfer/Dim/Dim.h"
#include "MxBase/E2eInfer/Color/Color.h"
#include "MxBase/E2eInfer/DataType.h"
#include "PyImage/PyImage.h"

namespace PyBase {
const int DEFAULT_ENCODE_LEVEL = 100;
class ImageProcessor {
public:
    explicit ImageProcessor(int deviceId = 0);
    PyBase::Image decode_bytes(PyObject *data, const uint32_t dataSize,
                               const MxBase::ImageFormat &decodeFormat = MxBase::ImageFormat::YUV_SP_420);
    PyObject *encode_bytes(const Image &inputImage, int encodeLevel = DEFAULT_ENCODE_LEVEL);
    PyBase::Image decode(const std::string& inputPath,
                         const MxBase::ImageFormat& decodeFormat = MxBase::ImageFormat::YUV_SP_420);
    void encode(const Image& inputImage, const std::string& savePath, int encodeLevel = DEFAULT_ENCODE_LEVEL);
    PyBase::Image resize(const Image& inputImage, const MxBase::Size& resize,
                         const MxBase::Interpolation& interpolation = MxBase::Interpolation::HUAWEI_HIGH_ORDER_FILTER);
    PyBase::Image padding(const Image& inputImage, MxBase::Dim padDim, const MxBase::Color& color,
                          const MxBase::BorderType& borderType);
    std::vector<PyBase::Image> crop(const PyBase::Image& inputImage, const std::vector<MxBase::Rect>& cropRectVec);
    std::vector<PyBase::Image> crop(const std::vector<PyBase::Image>& inputImageVec,
                                    const std::vector<MxBase::Rect>& cropRectVec);
    std::vector<PyBase::Image> crop_resize(const PyBase::Image& inputImage,
                                           const std::vector<std::pair<MxBase::Rect, MxBase::Size>>& cropResizeVec);
    void crop_paste(const PyBase::Image& inputImage, const std::pair<MxBase::Rect, MxBase::Rect>& cropPasteRect,
                    PyBase::Image& pastedImage);
    PyBase::Image convert_format(const PyBase::Image& inputImage, const MxBase::ImageFormat& outputFormat);

private:
    std::shared_ptr<MxBase::ImageProcessor> imageProcessor_ = nullptr;
};

PyBase::Image image(const std::string& inputPath, int deviceId, const MxBase::ImageFormat& decodeFormat);
} // namespace PyBase
#endif
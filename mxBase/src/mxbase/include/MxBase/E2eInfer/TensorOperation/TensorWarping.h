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
* Description: Manage Tensor Warp Operations.
* Author: MindX SDK
* Create: 2023
* History: NA
*/

#ifndef MXBASE_TENSORWARPING_H_
#define MXBASE_TENSORWARPING_H_

#include <vector>
#include <memory>
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/E2eInfer/Size/Size.h"
#include "MxBase/Asynchron/AscendStream.h"

namespace MxBase {

enum class WarpPerspectiveMode {
    INTER_LINEAR = 0
};

enum class WarpAffineMode {
    INTER_LINEAR = 0
};

enum class PaddingMode {
    PADDING_CONST = 0
};

enum class RotateAngle {
    ROTATE_90 = 90,
    ROTATE_180 = 180,
    ROTATE_270 = 270
};

/**
 * @description: Tensor Resize, support UINT8.
 * @param src: Input tensor.
 * @param dst: Output tensor.
 * @param resize: Resize param, Output tensor size.
 * @param interpolation: Interpolation mode, default is BILINEAR_SIMILAR_OPENCV.
 * @param keepMargin: Tensor with margin or not, default is false.
 * @param stream: stream to operate Resize.
 */
APP_ERROR Resize(const Tensor &src, Tensor &dst, const Size &resize,
    const Interpolation interpolation = Interpolation::BILINEAR_SIMILAR_OPENCV,
    bool keepMargin = false, AscendStream& stream = AscendStream::DefaultStream());

/**
 * @description: Tensor ResizePaste for multiple inputImages, support UINT8.
 * @param background: Background Tensor.
 * @param inputPics: Vector of pasted image tensor.
 * @param pasteRects: Vector of paste rects.
 * @param dst: Result Tensor, can be same as background.
 * @param keepMargin: Tensor with margin or not, default is false.
 * @param stream: stream to operate ResizePaste.
 */
APP_ERROR ResizePaste(const MxBase::Tensor &background, std::vector<MxBase::Tensor> &inputPics,
    std::vector<MxBase::Rect> &pasteRects, MxBase::Tensor &dst, bool keepMargin = false,
    MxBase::AscendStream &stream = MxBase::AscendStream::DefaultStream());

/**
 * @description: Tensor WarpPerspective, support UINT8/FP32.
 * @param src: Input Tensor.
 * @param dst: Output tensor.
 * @param transMatrix: Transformation matrix, must be 3x3.
 * @param paddingMode: Padding Mode, current only support PADDING_CONST.
 * @param warpPerspectiveMode: WarpPerspective Mode, current only support INTER_LINEAR.
 * @param stream: stream to operate WarpPerspective.
 */
APP_ERROR WarpPerspective(const Tensor &src, Tensor &dst, const std::vector<std::vector<float>> transMatrix,
                          const PaddingMode paddingMode, const float borderValue,
                          const WarpPerspectiveMode warpPerspectiveMode,
                          AscendStream &stream = AscendStream::DefaultStream());

/**
 * @description: Tensor WarpAffineHiper, support UINT8/FP32.
 * @param src: Input Tensor.
 * @param dst: Output tensor.
 * @param transMatrix: Transformation matrix, must be 2x3.
 * @param paddingMode: Padding Mode, current only support PADDING_CONST.
 * @param borderValue: padding color value.
 * @param WarpAffineMode: WarpAffine Mode, current only support INTER_LINEAR.
 * @param stream: stream to operate WarpPerspective.
 */
APP_ERROR WarpAffineHiper(const Tensor &src, Tensor &dst, const std::vector<std::vector<float>> transMatrix,
                          const PaddingMode paddingMode, const float borderValue, const WarpAffineMode warpAffineMode,
                          AscendStream& stream = AscendStream::DefaultStream());


/**
 * @description: Rotate tensor, support UINT8.
 * @param src: input Tensor vector for op compute.
 * @param dst: output Tensor vector after rotate certain angle.
 * @param angle: angle to rotate input Tensor, can be 90, 180, 270 clockwise.
 * @param stream: stream to operate op.
*/
APP_ERROR Rotate(const Tensor &src, Tensor &dst, const RotateAngle angle,
                 AscendStream& stream = AscendStream::DefaultStream());

/**
 * @description: Tensor CropResize for multiple Rects and Sizes, support UINT8.
 * @param inputTensor: Input tensor.
 * @param cropRectVec: Areas to crop, list of Rects.
 * @param sizeVec: sizes will tensor resize to, list of Sizes.
 * @param outputTensorVec: Result tensor list.
 * @param interpolation: Interpolation mode, default is BILINEAR_SIMILAR_OPENCV.
 * @param keepMargin: Tensor with margin or not, default is false.
 * @param stream: stream to operate CropResize.
 */
APP_ERROR CropResize(const Tensor &inputTensor, const std::vector<Rect> &cropRectVec,
                     const std::vector<Size> &sizeVec, std::vector<Tensor> &outputTensorVec,
                     const Interpolation interpolation = Interpolation::BILINEAR_SIMILAR_OPENCV,
                     bool keepMargin = false, AscendStream& stream = AscendStream::DefaultStream());

/**
 * @description: Tensor Crop, support UINT8.
 * @param inputTensor: Input tensor.
 * @param cropRect: Area to crop.
 * @param outputTensor: Result tensor.
 * @param keepMargin: Tensor with margin or not, default is false.
 * @param stream: stream to operate Crop.
 */
APP_ERROR Crop(const Tensor &inputTensor, const Rect &cropRect, Tensor &outputTensor,
               bool keepMargin = false, AscendStream& stream = AscendStream::DefaultStream());

/**
 * @description: Tensor Crop for multiple Rects, support UINT8.
 * @param inputTensor: Input tensor.
 * @param cropRectVec: Areas to crop, list of Rects.
 * @param outputTensorVec: Result tensor list.
 * @param keepMargin: Tensor with margin or not, default is false.
 * @param stream: stream to operate Crop.
 */
APP_ERROR Crop(const Tensor &inputTensor, const std::vector<Rect> &cropRectVec, std::vector<Tensor> &outputTensorVec,
               bool keepMargin = false, AscendStream& stream = AscendStream::DefaultStream());
}

#endif

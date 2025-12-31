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
* Description: Core Operations On Tensors.
* Author: MindX SDK
* Create: 2023
* History: NA
*/
#ifndef MXBASE_COREOPERATIONSONTENSORS_H
#define MXBASE_COREOPERATIONSONTENSORS_H

#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/Asynchron/AscendStream.h"

namespace MxBase {
/**
* @description: Batch dimension split tensor, support UINT8, FLOAT16, FLOAT32.
* @param tv: input to perform split and operator
* @param dst: result tensors
* @param isReplace: reuse the src tensor or not.
* @param stream: stream to operate op
*/
APP_ERROR BatchSplit(const Tensor &src, std::vector<Tensor> &dst, bool isReplace,
                     AscendStream &stream = AscendStream::DefaultStream());

/**
* @description: Horizontal stack tensor, support UINT8, FLOAT16, FLOAT32.
* @param tv: input to perform stack and operator
* @param dst: result tensor
* @param stream: stream to operate op
*/
APP_ERROR Hstack(const std::vector<Tensor> &tv, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());

/**
* @description: Vertical stack tensors, support UINT8, FLOAT16, FLOAT32.
* @param tv: input to perform stack and operator
* @param dst: result tensor
* @param stream: stream to operate op
*/
APP_ERROR Vstack(const std::vector<Tensor> &tv, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());

/**
* @description: transpose tensor, support UINT8, FLOAT16, FLOAT32.
* @param src: input to perform transpose and operator
* @param axes: the axes
* @param dst: result tensor
* @param stream: stream to operate op
*/
APP_ERROR Transpose(const Tensor &src, Tensor &dst, std::vector<int> axes,
                    AscendStream &stream = AscendStream::DefaultStream());
/**
 * @description: Split tensors, support UINT8, FLOAT16, FLOAT32.
 * @param src: Input to perform split and operator.
 * @param tv: Result Tensor.
 * @param stream: stream to operate op.
 */
APP_ERROR Split(const Tensor &src, std::vector<Tensor> &tv, AscendStream &stream = AscendStream::DefaultStream());

/**
* @description: Merge tensor, support UINT8, FLOAT16, FLOAT32.
* @param tv: input to perform merge and operator
* @param dst: result tensor
* @param stream: stream to operate op
*/
APP_ERROR Merge(const std::vector <Tensor> &tv, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
/**
 * @description: Tile tensor, support UINT8, FLOAT16, FLOAT32.
 * @param src: input Tensor vector for op compute.
 * @param dst: output Tensor vector for op compute.
 * @param multiples: extended dimension information.
 * @param stream: stream to operate op.
*/
APP_ERROR Tile(const Tensor &src, Tensor &dst, const std::vector<uint32_t> &multiples,
               AscendStream& stream = AscendStream::DefaultStream());

/**
 * @description: Convert tensor format, support UINT8.
 * @param inputTensor: input Tensor.
 * @param outputTensor: output Tensor.
 * @param mode: original and target types of color format conversion.
 * @param keepMargin: tensor with margin or not, default is false.
 * @param stream: stream to operate CropResize.
*/
APP_ERROR CvtColor(const Tensor &inputTensor, Tensor &outputTensor, const CvtColorMode &mode, bool keepMargin = false,
                   AscendStream &stream = AscendStream::DefaultStream());

/**
 * @description: Erode tensor, support UINT8, FLOAT16, FLOAT32.
 * @param src: Input Tensor vector for op compute.
 * @param dst: Output Tensor vector for op compute.
 * @param blurconfig: Erode information.
 * @param stream: stream to operate op.
*/
APP_ERROR Erode(const Tensor &src, Tensor &dst, const BlurConfig &blurconfig,
                AscendStream& stream = AscendStream::DefaultStream());
}
#endif
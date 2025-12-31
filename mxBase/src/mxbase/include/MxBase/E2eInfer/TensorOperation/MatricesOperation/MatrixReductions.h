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
 * Description: Manage Tensor Matrix Reductions.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */
#ifndef MXBASE_MATRIXREDUCTIONS_H
#define MXBASE_MATRIXREDUCTIONS_H

#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/Asynchron/AscendStream.h"
namespace MxBase {
/**
 * @description: Sum tensor, support UINT8, FLOAT16, FLOAT32.
 * @param src: input Tensor vector for op compute.
 * @param dst: output Tensor vector for op compute.
 * @param stream: stream to operate op.
 */
APP_ERROR Sum(const Tensor &src, Tensor &dst, AscendStream &stream = AscendStream::DefaultStream());

/**
 * @description: Reduce tensor, support UINT8, FLOAT16, FLOAT32.
 * @param src: input Tensor vector for op compute.
 * @param dst: output Tensor vector for op compute.
 * @param rDim: Dimension index along which the tensor is reduced.
 * @param rType: reduction operation.
 * @param stream: stream to operate op.
 */
APP_ERROR Reduce(const Tensor &src, Tensor &dst,
                 const MxBase::ReduceDim &rDim, const MxBase::ReduceType &rType,
                 AscendStream &stream = AscendStream::DefaultStream());

/**
 * @description: Get the minimal value and maximum value of tensor, support UINT8, FLOAT16, FLOAT32.
 * @param src: input Tensor vector for op compute.
 * @param minVal: minimal value of Tensor.
 * @param maxVal: maximal value of Tensor.
 * @param stream: stream to operate op.
 */
APP_ERROR MinMax(const Tensor &src, Tensor &minVal, Tensor &maxVal,
                 AscendStream &stream = AscendStream::DefaultStream());
/**
* @description: Compute Sum of Abs of each element in src tensor, support UINT8, FLOAT16, FLOAT32.
* @param src: Input to perform AbsSum operator
* @param dst: Result tensor.
* @param stream: Synchronous or asynchronous execution the op.
*/
APP_ERROR AbsSum(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());

/**
* @description: Compute Sum of square of each element in src tensor, support UINT8, FLOAT32.
* @param src: Input to perform SquareSum operator
* @param dst: Result tensor.
* @param stream: Synchronous or asynchronous execution the op.
*/
APP_ERROR SqrSum(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());
/**
 * @description: Finds global minimum and maximum tensor elements and returns their values with locations,
 *               support UINT8, FLOAT16, FLOAT32.
 * @param src: Input single-channel tensor for op compute.
 * @param minVal: Output tensor, minimum value in tensor.
 * @param maxVal: Output tensor, maximum value in tensor.
 * @param minLoc: Output tensor, minimum value location in tensor.
 * @param maxLoc: Output tensor, maximum value location in tensor.
 * @param stream: Stream to operate op.
 */
APP_ERROR MinMaxLoc(const Tensor &src, Tensor &minVal, Tensor &maxVal, Tensor &minLoc, Tensor &maxLoc,
                    AscendStream &stream = AscendStream::DefaultStream());
}

#endif
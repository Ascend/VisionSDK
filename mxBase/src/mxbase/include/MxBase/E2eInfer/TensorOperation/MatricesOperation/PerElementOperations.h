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
 * Description: Manage Tensor Per Element Operations.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */
#ifndef MXBASE_PERELEMENTOPERATIONS_H
#define MXBASE_PERELEMENTOPERATIONS_H

#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/Asynchron/AscendStream.h"

namespace MxBase {
/**
* @description: Add tensors, support UINT8, FLOAT16, FLOAT32.
* @param src1: Addend.
* @param src2: Summand.
* @param dst: Result tensor.
* @param stream: stream to operate op.
*/
APP_ERROR Add(const Tensor &src1, const Tensor &src2, Tensor &dst,
              AscendStream& stream = AscendStream::DefaultStream());

/**
* @description: Subtract tensors, support UINT8, FLOAT16, FLOAT32.
* @param src1: Subtrahend.
* @param src2: Subtractor.
* @param dst: Result Tensor.
* @param stream: stream to operate op.
*/
APP_ERROR Subtract(const Tensor &src1, const Tensor &src2, Tensor &dst,
                   AscendStream& stream = AscendStream::DefaultStream());

/**
 * @description: Multiply tensors, support UINT8, FLOAT16, FLOAT32.
 * @param src1: Multiplier.
 * @param src2: Multiplicator.
 * @param dst: Result Tensor.
 * @param stream: stream to operate op.
 */
APP_ERROR Multiply(const Tensor &src1, const Tensor &src2,
                   Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());

/**
 * @description: Multiply tensors, support UINT8, FLOAT16, FLOAT32.
 * @param src1: Multiplier.
 * @param src2: Multiplicator.
 * @param dst: Result Tensor.
 * @param scale: Input float value for op compute.
 * @param stream: stream to operate op.
 */
APP_ERROR Multiply(const Tensor &src1, const Tensor &src2, Tensor &dst, double scale,
                   AscendStream& stream = AscendStream::DefaultStream());

/**
 * @description: Divide tensors, support mixed use of uint8 and fp32/fp16.
 * @param src1: Dividend.
 * @param src2: Divisor.
 * @param dst: Result Tensor.
 * @param stream: stream to operate op.
 */
APP_ERROR Divide(const Tensor &src1, const Tensor &src2,
                 Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());

/**
 * @description: Divide tensors, support mixed use of uint8 and fp32/fp16.
 * @param src1: Dividend.
 * @param src2: Divisor.
 * @param dst: Result Tensor.
 * @param scale: Input float value for op compute.
 * @param stream: stream to operate op.
 */
APP_ERROR Divide(const Tensor &src1, const Tensor &src2, Tensor &dst, float scale,
                 AscendStream& stream = AscendStream::DefaultStream());

/**
 * @description: BitwiseAnd tensors, support UINT8.
 * @param src1: Input to perform bitwise and operator.
 * @param src2: Input to perform bitwise and operator.
 * @param dst: Result Tensor.
 * @param stream: stream to operate op.
 */
APP_ERROR BitwiseAnd(const Tensor &src1, const Tensor &src2, Tensor &dst,
                     AscendStream &stream = AscendStream::DefaultStream());

/**
 * @description: BitwiseOr tensors, support UINT8.
 * @param src1: Input to perform bitwise or operator.
 * @param src2: Input to perform bitwise or operator.
 * @param dst: Result Tensor.
 * @param stream: stream to operate op.
 */
APP_ERROR BitwiseOr(const Tensor &src1, const Tensor &src2, Tensor &dst,
                    AscendStream& stream = AscendStream::DefaultStream());

/**
 * @description: BitwiseXor tensors, support UINT8.
 * @param src1: Input to perform bitwise xor operator.
 * @param src2: Input to perform bitwise xor operator.
 * @param dst: Result Tensor.
 * @param stream: stream to operate op.
 */
APP_ERROR BitwiseXor(const Tensor &src1, const Tensor &src2, Tensor &dst,
                     AscendStream& stream = AscendStream::DefaultStream());

/**
 * @description: BitwiseNot tensor, support UINT8.
 * @param src: Input to perform bitwise not operator.
 * @param dst: Result Tensor.
 * @param stream: stream to operate op.
 */
APP_ERROR BitwiseNot(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());

/**
* @description: Exponentiation of each element in two tensors, support UINT8, FLOAT16, FLOAT32.
* @param src1: Base number tensor
* @param src2: Exponent tensor
* @param dst: Result tensor.
* @param stream: Synchronous or asynchronous execution the op.
*/
APP_ERROR Pow(const Tensor &src1, const Tensor &src2, Tensor &dst,
              AscendStream& stream = AscendStream::DefaultStream());

/**
* @description: Compute Square of each element in src tensor, support UINT8, FLOAT16, FLOAT32.
* @param src: Input to perform Square operator
* @param dst: Result tensor.
* @param stream: Synchronous or asynchronous execution the op.
*/
APP_ERROR Sqr(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());

/**
* @description: Sqrt tensors, support FLOAT16, FLOAT32.
* @param src: Source tensor for sqrt operation.
* @param dst: Dst tensor for sqrt operation.
* @param stream: Stream to operate op.
*/
APP_ERROR Sqrt(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());

/**
* @description: Exp tensors, support FLOAT16, FLOAT32.
* @param src: Source tensor for exp operation.
* @param dst: Dst tensor for exp operation.
* @param stream: Stream to operate op.
*/
APP_ERROR Exp(const Tensor &src, Tensor &dst,
              AscendStream& stream = AscendStream::DefaultStream());

/**
* @description: Natural logarithm of each element in a tensor, support FLOAT16, FLOAT32.
* @param src: Input to perform natural logarithm.
* @param dst: Result tensor.
* @param stream: Synchronous or asynchronous execution the op.
*/
APP_ERROR Log(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());

/**
* @description: Threshold binary tensor, support UINT8, FLOAT16, FLOAT32.
* @param src: Input tensor for threshold binary operation.
* @param dst: Output tensor for threshold binary operation.
* @param thresh: Threshold for op compute.
* @param maxVal: Max value for op compute.
* @param stream: Stream to operate op.
*/
APP_ERROR ThresholdBinary(const Tensor &src, Tensor &dst, float thresh, float maxVal,
                          AscendStream &stream = AscendStream::DefaultStream());

/**
* @description: Threshold tensor, support UINT8, FLOAT16, FLOAT32.
* @param src: Input tensor for threshold operation.
* @param dst: Output tensor for threshold operation.
* @param thresh: Threshold for op compute.
* @param maxVal: Max value for op compute.
* @param thresholdType: ThresholdType value for threshold binary operation.
* @param stream: Stream to operate op.
*/
APP_ERROR Threshold(const Tensor &src, Tensor &dst, float thresh, float maxVal,
                    const ThresholdType &thresholdType = ThresholdType::THRESHOLD_BINARY,
                    AscendStream &stream = AscendStream::DefaultStream());

/**
* @description: AddWeighted tensors, support UINT8, FLOAT16, FLOAT32.
* @param src1: Input tensor1 for addweighted operation.
* @param alpha: Input float value for op compute.
* @param src2: Input tensor2 for addweighted operation.
* @param beta: Input float value for op compute.
* @param gamma: Input float value for op compute.
* @param dst: Output tensor for addweighted operation.
* @param stream: Stream to operate op.
*/
APP_ERROR AddWeighted(const Tensor &src1, float alpha, const Tensor &src2, float beta, float gamma, Tensor &dst,
                      AscendStream &stream = AscendStream::DefaultStream());

/**
* @description: Abs diff of each element in tensors, support UINT8, FLOAT16, FLOAT32.
* @param src1: Input to perform abs diff.
* @param src2: Input to perform abs diff.
* @param dst: Result tensor.
* @param stream: Synchronous or asynchronous execution the op.
*/
APP_ERROR AbsDiff(const Tensor &src1, const Tensor &src2, Tensor &dst,
                  AscendStream& stream = AscendStream::DefaultStream());

/**
* @description: Calculating absolute values of each element in tensor, support UINT8, FLOAT16, FLOAT32.
* @param src: Input tensor
* @param dst: Result tensor.
* @param stream: Synchronous or asynchronous execution the op.
*/
APP_ERROR Abs(const Tensor &src, Tensor &dst, AscendStream& stream = AscendStream::DefaultStream());

/**
* @description: Calculating the value of src1 * scale + src2, support UINT8, FLOAT16, FLOAT32.
* @param src1: Base number tensor.
* @param scale: Scaling factor.
* @param src2: Add tensor.
* @param dst: Result tensor.
* @param stream: Synchronous or asynchronous execution the op.
*/
APP_ERROR ScaleAdd(const Tensor &src1, float scale, const Tensor &src2, Tensor &dst,
                   AscendStream& stream = AscendStream::DefaultStream());

/**
* @description: Convert tensor datatype
* @param src: Input tensor
* @param dst:  Output tensor
* @param dataType: The datatype to be converted
* @param stream: Stream to operate op
*/
APP_ERROR ConvertTo(const Tensor &src, Tensor &dst, const MxBase::TensorDType &dataType,
                    AscendStream &stream = AscendStream::DefaultStream());

/**
 * @description: Clip tensor, support UINT8, FLOAT16, FLOAT32.
 * @param src: input Tensor vector for op compute.
 * @param dst: output Tensor vector for op compute.
 * @param minVal: minimal value for op compute.
 * @param maxVal: max value for op compute.
 * @param stream: stream to operate op.
 */
APP_ERROR Clip(const Tensor &src, Tensor &dst, float minVal, float maxVal,
               AscendStream &stream = AscendStream::DefaultStream());

/**
* @description: calculating the min value of src1 and src2, support UINT8, FLOAT16, FLOAT32.
* @param src1: input tensor.
* @param src2: input tensor.
* @param dst: Result tensor.
* @param stream: Synchronous or asynchronous execution the op.
*/
APP_ERROR Min(const Tensor &src1, const Tensor &src2, Tensor &dst,
              AscendStream& stream = AscendStream::DefaultStream());

/**
* @description: calculating the max value of src1 and src2, support UINT8, FLOAT16, FLOAT32.
* @param src1: input tensor.
* @param src2: input tensor.
* @param dst: Result tensor.
* @param stream: Synchronous or asynchronous execution the op.
*/
APP_ERROR Max(const Tensor &src1, const Tensor &src2, Tensor &dst,
              AscendStream& stream = AscendStream::DefaultStream());

/**
 * @description: SortIdX tensors, support UINT8, FLOAT16, FLOAT32.
 * @param src: Source tensor of sort operation.
 * @param dstIdx: SortIdx result tensor.
 * @param axis An optional attribute indicates the sorting axis.
 * @param descending: An optional attribute indicates descending sort or not.
 * @param stream: stream to operate op.
 */
APP_ERROR SortIdx(const Tensor &src, Tensor &dstIdx, int axis, bool descending,
                  AscendStream &stream = AscendStream::DefaultStream());

/**
* @description: Compare tensors, support UINT8, FLOAT16, FLOAT32.
* @param src1: Input to perform compare and operator.
* @param src2: Input to perform compare and operator.
* @param dst: dst tensor for compare operation.
* @param cmpOp: op type, support CmpOp::CMP_EQ, CmpOp::CMP_NE, CmpOp::CMP_LT, CmpOp::CMP_GT, CmpOp::CMP_LE,
 *              CmpOp::CMP_GE, means ==, !=, <, >, <=, >=.
* @param stream: stream to operate op.
*/
APP_ERROR Compare(const Tensor &src1, const Tensor &src2, Tensor &dst, const CmpOp cmpOp = CmpOp::CMP_EQ,
                  AscendStream &stream = AscendStream::DefaultStream());

/**
 * @description: Sort tensors, support UINT8, FLOAT16, FLOAT32.
 * @param src: Source tensor of sort operation.
 * @param dst: Sort result tensor.
 * @param axis An optional attribute indicates the sorting axis.
 * @param descending: An optional attribute indicates desending sort or not.
 * @param stream: stream to operate op.
 */
APP_ERROR Sort(const Tensor &src, Tensor &dst, int axis, bool descending,
               AscendStream &stream = AscendStream::DefaultStream());

/**
* @description: calculating the value of src * scale + bias, support UINT8, FLOAT16, FLOAT32.
* @param src: base number tensor.
* @param dst: Result tensor.
* @param scale: scaling factor.
* @param bias: bias factor.
* @param stream: Synchronous or asynchronous execution the op.
*/
APP_ERROR Rescale(const Tensor &src, Tensor &dst, float scale, float bias,
                  AscendStream& stream = AscendStream::DefaultStream());

}

#endif

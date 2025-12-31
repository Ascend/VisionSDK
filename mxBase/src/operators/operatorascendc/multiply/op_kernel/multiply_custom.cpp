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
 * Description: multiply operator kernel file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#include "kernel_operator.h"
#include "vector_scheduler.h"
using namespace AscendC;

constexpr int32_t BUFFER_NUM = 1;

template <typename T0, typename T1, typename T2>
class KernelMultiply {
public:
    __aicore__ inline KernelMultiply(float scale)
        : scale(scale)
    {}

    __aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z, size_t bufferNum, size_t bufferBytes,
                                size_t gmIdx, size_t gmDataLen)
    {
        if (bufferBytes <= 0) {
            return;
        }

        size_t sizeT = 1;
        if constexpr(sizeof(T0) > sizeof(T1)) {
            sizeT = sizeof(T0) / sizeof(T1);
            pipe.InitBuffer(inQueueX, bufferNum, bufferBytes * sizeT);
            pipe.InitBuffer(inQueueY, bufferNum, bufferBytes);
        } else if constexpr(sizeof(T1) > sizeof(T0)) {
            sizeT = sizeof(T1) / sizeof(T0);
            pipe.InitBuffer(inQueueX, bufferNum, bufferBytes);
            pipe.InitBuffer(inQueueY, bufferNum, bufferBytes * sizeT);
        } else {
            pipe.InitBuffer(inQueueX, bufferNum, bufferBytes * sizeT);
            pipe.InitBuffer(inQueueY, bufferNum, bufferBytes * sizeT);
        }
        pipe.InitBuffer(outQueueZ, bufferNum, bufferBytes * sizeT);

        xGm.SetGlobalBuffer((__gm__ T0*)x + gmIdx, gmDataLen);
        yGm.SetGlobalBuffer((__gm__ T1*)y + gmIdx, gmDataLen);
        zGm.SetGlobalBuffer((__gm__ T2*)z + gmIdx, gmDataLen);

        if constexpr(sizeof(T0) == 1 && sizeof(T0) == sizeof(T1)) { // u8 u8
            pipe.InitBuffer(bufXf32, bufferBytes * sizeof(float));
            pipe.InitBuffer(bufYf32, bufferBytes * sizeof(float));
            pipe.InitBuffer(bufZf32, bufferBytes * sizeof(float));
            pipe.InitBuffer(bufXf16, bufferBytes * sizeof(half));
        }

        if constexpr((sizeof(T0) > sizeof(T1) && sizeof(T0) - sizeof(T1) > sizeof(half)) ||
                     (sizeof(T1) > sizeof(T0) && sizeof(T1) - sizeof(T0) > sizeof(half))) {
            pipe.InitBuffer(bufXf32, bufferBytes * sizeof(float));
            pipe.InitBuffer(bufXf16, bufferBytes * sizeof(half));
        }
    }

    __aicore__ inline void MultiplyEQ(uint32_t idx, size_t len)
    {
        // compute
        auto xLocal = inQueueX.DeQue<T0>();
        auto yLocal = inQueueY.DeQue<T1>();
        auto zLocal = outQueueZ.AllocTensor<T2>();

        if constexpr(sizeof(T0) == sizeof(uint8_t)) {
            auto txLocal = bufXf32.Get<float>();
            auto tyLocal = bufYf32.Get<float>();
            auto tzLocal = bufZf32.Get<float>();
            auto txLocalFp16 = bufXf16.Get<half>();

            Cast(txLocalFp16, xLocal, RoundMode::CAST_NONE, len);
            Cast(txLocal, txLocalFp16, RoundMode::CAST_NONE, len);

            Duplicate<float>(tyLocal, this->scale, len);
            Mul(txLocal, txLocal, tyLocal, len);

            Cast(txLocalFp16, yLocal, RoundMode::CAST_NONE, len);
            Cast(tyLocal, txLocalFp16, RoundMode::CAST_NONE, len);
            Mul(tzLocal, txLocal, tyLocal, len);

            Cast(txLocalFp16, tzLocal, RoundMode::CAST_ODD, len);
            Cast(zLocal, txLocalFp16, RoundMode::CAST_TRUNC, len);
        } else {
            Mul(xLocal, xLocal, yLocal, len);
            Duplicate<T2>(zLocal, static_cast<T2>(this->scale), len);
            Mul(zLocal, xLocal, zLocal, len);
        }

        outQueueZ.EnQue<T2>(zLocal);
        inQueueX.FreeTensor(xLocal);
        inQueueY.FreeTensor(yLocal);
    }

    __aicore__ inline void MultiplyGT(uint32_t idx, size_t len)
    {
        // compute
        auto xLocal = inQueueX.DeQue<T0>();
        auto yLocal = inQueueY.DeQue<T1>();
        auto zLocal = outQueueZ.AllocTensor<T2>();

        if constexpr(sizeof(T0) - sizeof(T1) > sizeof(half)) {
            // fp32 u8 fp32 scale(fp32)
            auto txLocal = bufXf32.Get<float>();
            auto txLocalFp16 = bufXf16.Get<half>();

            // y: u8 -> fp16 -> fp32
            Cast(txLocalFp16, yLocal, RoundMode::CAST_NONE, len);
            Cast(txLocal, txLocalFp16, RoundMode::CAST_NONE, len);

            Mul(xLocal, xLocal, txLocal, len);
            Duplicate<float>(txLocal, this->scale, len);
            Mul(zLocal, xLocal, txLocal, len);
        } else {
            // fp32 fp16 fp32 | fp16 u8 fp16
            // cal x * y = z
            Cast(zLocal, yLocal, RoundMode::CAST_NONE, len);
            Mul(zLocal, xLocal, zLocal, len);
            // cal z * scale => z
            Duplicate<T2>(xLocal, static_cast<T2>(this->scale), len);
            Mul(zLocal, xLocal, zLocal, len);
        }

        outQueueZ.EnQue<T2>(zLocal);
        inQueueX.FreeTensor(xLocal);
        inQueueY.FreeTensor(yLocal);
    }

    __aicore__ inline void MultiplyLT(uint32_t idx, size_t len)
    {
        // compute
        auto xLocal = inQueueX.DeQue<T0>();
        auto yLocal = inQueueY.DeQue<T1>();
        auto zLocal = outQueueZ.AllocTensor<T2>();

        if constexpr(sizeof(T1) - sizeof(T0) > sizeof(half)) {
            // u8 fp32 fp32 scale(fp32)
            auto txLocal = bufXf32.Get<float>();
            auto txLocalFp16 = bufXf16.Get<half>();

            // x: u8 -> fp16 -> fp32
            Cast(txLocalFp16, xLocal, RoundMode::CAST_NONE, len);
            Cast(txLocal, txLocalFp16, RoundMode::CAST_NONE, len);

            Mul(yLocal, yLocal, txLocal, len);
            Duplicate<float>(txLocal, this->scale, len);
            Mul(zLocal, yLocal, txLocal, len);
        } else {
            // fp16 fp32 | u8 fp16
            // cal y * x = z
            Cast(zLocal, xLocal, RoundMode::CAST_NONE, len);
            Mul(zLocal, yLocal, zLocal, len);
            // cal z * scale => z
            Duplicate<T2>(yLocal, static_cast<T2>(this->scale), len);
            Mul(zLocal, yLocal, zLocal, len);
        }

        outQueueZ.EnQue<T2>(zLocal);
        inQueueX.FreeTensor(xLocal);
        inQueueY.FreeTensor(yLocal);
    }

    __aicore__ inline void CalcForAlign32(uint32_t idx, size_t len)
    {
        if (len <= 0) {
            return;
        }
        // copyIn
        auto xLocal = inQueueX.AllocTensor<T0>();
        auto yLocal = inQueueY.AllocTensor<T1>();

        DataCopy(xLocal, xGm[idx], len);
        DataCopy(yLocal, yGm[idx], len);
        inQueueX.EnQue(xLocal);
        inQueueY.EnQue(yLocal);

        if constexpr(sizeof(T0) == sizeof(T1)) {
            MultiplyEQ(idx, len);
        } else if constexpr(sizeof(T0) > sizeof(T1)) {
            MultiplyGT(idx, len);
        } else {
            MultiplyLT(idx, len);
        }

        // CopyOut
        auto zLocal = outQueueZ.DeQue<T2>();
        DataCopy(zGm[idx], zLocal, len);
        outQueueZ.FreeTensor(zLocal);
    }

protected:
    TPipe pipe;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueX;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueY;
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueZ;
    TBuf<TPosition::VECCALC> bufXf32;
    TBuf<TPosition::VECCALC> bufYf32;
    TBuf<TPosition::VECCALC> bufZf32;
    TBuf<TPosition::VECCALC> bufXf16;

    GlobalTensor<T0> xGm;
    GlobalTensor<T1> yGm;
    GlobalTensor<T2> zGm;

    float scale;
};

template <typename T0, typename T1, typename T2>
__aicore__ void run_op(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR tiling, float ubVarNum)
{
    GET_TILING_DATA(tilingData, tiling);
    float scale = tilingData.scale;
    size_t sizeT = sizeof(T0);
    if (sizeof(T0) > sizeof(T1)) {
        sizeT = sizeof(T1);
    }
    VectorScheduler sch(tilingData.size, GetBlockNum(), BUFFER_NUM, ubVarNum, sizeT);
    KernelMultiply<T0, T1, T2> op(scale);
    size_t orgVecIdx = GetBlockIdx() * sch.dataLenPerCore;
    op.Init(x, y, z, sch.bufferNum, sch.dataBytesPerLoop, orgVecIdx, sch.dataLen);
    sch.run(&op, sch.dataLen);
}

extern "C" __global__ __aicore__ void multiply_custom(GM_ADDR x1, GM_ADDR x2, GM_ADDR y, GM_ADDR workspace,
                                                      GM_ADDR tiling) {
    if (TILING_KEY_IS(1)) { // fp16 fp16 fp16
        // inQueueX，inQueueY，outQueueZ
        constexpr float ubVarNum = 3;
        run_op<half, half, half>(x1, x2, y, tiling, ubVarNum);
    } else if (TILING_KEY_IS(2)) { // fp32 fp32 fp32
        // inQueueX，inQueueY，outQueueZ
        constexpr float ubVarNum = 3;
        run_op<float, float, float>(x1, x2, y, tiling, ubVarNum);
    } else if (TILING_KEY_IS(3)) { // u8 u8 u8
        // inQueueX，inQueueY，outQueueZ, bufXf32, bufYf32, bufZf32, bufXf16
        constexpr float ubVarNum = 3 + 3 * 4 + 1 * 2;
        run_op<uint8_t, uint8_t, uint8_t>(x1, x2, y, tiling, ubVarNum);
    } else if (TILING_KEY_IS(4)) { // fp16 u8 fp16
        // inQueueX，inQueueY，outQueueZ
        constexpr float ubVarNum = 1 + 2 * 2;
        run_op<half, uint8_t, half>(x1, x2, y, tiling, ubVarNum);
    } else if (TILING_KEY_IS(5)) { // fp32 u8 fp32
        // inQueueX，inQueueY，outQueueZ, bufXf32, bufXf16
        constexpr float ubVarNum = 1 + 3 * 4 + 1 * 2;
        run_op<float, uint8_t, float>(x1, x2, y, tiling, ubVarNum);
    } else if (TILING_KEY_IS(6)) { // fp32 fp16 fp32
        // inQueueX，inQueueY，outQueueZ
        constexpr float ubVarNum = 1 + 2 * 2;
        run_op<float, half, float>(x1, x2, y, tiling, ubVarNum);
    } else if (TILING_KEY_IS(7)) { // u8 fp16 fp16
        // inQueueX，inQueueY，outQueueZ
        constexpr float ubVarNum = 1 + 2 * 2;
        run_op<uint8_t, half, half>(x1, x2, y, tiling, ubVarNum);
    } else if (TILING_KEY_IS(8)) { // u8 fp32 fp32
        // inQueueX，inQueueY，outQueueZ, bufXf32, bufXf16
        constexpr float ubVarNum = 1 + 3 * 4 + 1 * 2;
        run_op<uint8_t, float, float>(x1, x2, y, tiling, ubVarNum);
    } else if (TILING_KEY_IS(9)) { // fp16 fp32 fp32
        // inQueueX，inQueueY，outQueueZ
        constexpr float ubVarNum = 1 + 2 * 2;
        run_op<half, float, float>(x1, x2, y, tiling, ubVarNum);
    }
}

#ifndef __CCE_KT_TEST__

// call of kernel function
void multiply_custom_do(uint32_t blockDim, void *l2ctrl, void *stream, uint8_t *x, uint8_t *y, uint8_t *z,
                        uint8_t *workspace, uint8_t *tiling)
{
    multiply_custom<<<blockDim, l2ctrl, stream>>>(x, y, z, workspace, tiling);
}
#endif
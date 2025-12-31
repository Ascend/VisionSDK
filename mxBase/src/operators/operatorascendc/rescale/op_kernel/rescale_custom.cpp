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
 * Description: rescale operator kernel file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#include "kernel_operator.h"
#include "vector_scheduler.h"
using namespace AscendC;

constexpr int32_t BUFFER_NUM = 1;

template <typename T>
class KernelRescale {
public:
    __aicore__ inline KernelRescale(float scale, float bias)
        : scale(scale), bias(bias)
    {}

    __aicore__ inline void Init(GM_ADDR x, GM_ADDR z, size_t bufferNum, size_t bufferBytes,
                                size_t gmIdx, size_t gmDataLen)
    {
        if (bufferBytes <= 0) {
            return;
        }
        pipe.InitBuffer(inQueueX, bufferNum, bufferBytes);
        pipe.InitBuffer(outQueueZ, bufferNum, bufferBytes);
        if (sizeof(T) == 1) {
            pipe.InitBuffer(bufXf32, bufferBytes * sizeof(float));
            pipe.InitBuffer(bufZf32, bufferBytes * sizeof(float));
            pipe.InitBuffer(bufXf16, bufferBytes * sizeof(half));
        }

        xGm.SetGlobalBuffer((__gm__ T*)x + gmIdx, gmDataLen);
        zGm.SetGlobalBuffer((__gm__ T*)z + gmIdx, gmDataLen);
    }

    __aicore__ inline void CalcForAlign32(uint32_t idx, size_t len)
    {
        if (len <= 0) {
            return;
        }
        // copyIn
        auto xLocal = inQueueX.AllocTensor<T>();

        DataCopy(xLocal, xGm[idx], len);
        inQueueX.EnQue(xLocal);

        // compute
        xLocal = inQueueX.DeQue<T>();
        auto zLocal = outQueueZ.AllocTensor<T>();

        if constexpr(sizeof(T) == 1) {
            auto txLocal = bufXf32.Get<float>();
            auto tzLocal = bufZf32.Get<float>();
            auto tLocalF16 = bufXf16.Get<half>();

            Cast(tLocalF16, xLocal, RoundMode::CAST_NONE, len);
            Cast(txLocal, tLocalF16, RoundMode::CAST_NONE, len);

            Duplicate<float>(tzLocal, this->scale, len);
            Mul(txLocal, txLocal, tzLocal, len);

            Duplicate<float>(tzLocal, this->bias, len);
            Add(tzLocal, txLocal, tzLocal, len);

            Cast(tLocalF16, tzLocal, RoundMode::CAST_ODD, len);
            Cast(zLocal, tLocalF16, RoundMode::CAST_TRUNC, len);
        } else {
            Duplicate<T>(zLocal, this->scale, len);
            Mul(xLocal, xLocal, zLocal, len);
            Duplicate<T>(zLocal, this->bias, len);
            Add(zLocal, xLocal, zLocal, len);
        }

        outQueueZ.EnQue<T>(zLocal);
        inQueueX.FreeTensor(xLocal);

        // CopyOut
        zLocal = outQueueZ.DeQue<T>();
        DataCopy(zGm[idx], zLocal, len);
        outQueueZ.FreeTensor(zLocal);
    }

protected:
    TPipe pipe;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueX;
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueZ;
    TBuf<TPosition::VECCALC> bufXf32;
    TBuf<TPosition::VECCALC> bufZf32;
    TBuf<TPosition::VECCALC> bufXf16;

    GlobalTensor<T> xGm;
    GlobalTensor<T> zGm;

    float scale;
    float bias;
};

template <typename T>
__aicore__ void run_op(GM_ADDR x, GM_ADDR z, GM_ADDR tiling, float ubVarNum)
{
    GET_TILING_DATA(tilingData, tiling);
    float scale = tilingData.scale;
    float bias = tilingData.bias;
    VectorScheduler sch(tilingData.size, GetBlockNum(), BUFFER_NUM, ubVarNum, sizeof(T));
    KernelRescale<T> op(scale, bias);
    size_t orgVecIdx = GetBlockIdx() * sch.dataLenPerCore;
    op.Init(x, z, sch.bufferNum, sch.dataBytesPerLoop, orgVecIdx, sch.dataLen);
    sch.run(&op, sch.dataLen);
}

extern "C" __global__ __aicore__ void rescale_custom(GM_ADDR x, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)
{
    if (TILING_KEY_IS(1)) {
        constexpr float ubVarNum = 2;
        run_op<half>(x, z, tiling, ubVarNum);
    }
    else if (TILING_KEY_IS(2)) {
        constexpr float ubVarNum = 2;
        run_op<float>(x, z, tiling, ubVarNum);
    }
    else if (TILING_KEY_IS(3)) {
        constexpr float ubVarNum = 2 + 2 * 4 + 1 * 2;
        run_op<uint8_t>(x, z, tiling, ubVarNum);
    }
}

#ifndef __CCE_KT_TEST__

// call of kernel function
void rescale_custom_do(uint32_t blockDim, void *l2ctrl, void *stream, uint8_t *x, uint8_t *z, uint8_t *workspace,
                       uint8_t *tiling)
{
    rescale_custom<<<blockDim, l2ctrl, stream>>>(x, z, workspace, tiling);
}
#endif

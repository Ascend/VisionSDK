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
 * Description: add weighted operator kernel file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#include "kernel_operator.h"
#include "vector_scheduler.h"
using namespace AscendC;

constexpr int32_t BUFFER_NUM = 1;

template <typename T>
class KernelAddWeighted {
public:
    __aicore__ inline KernelAddWeighted(float alpha, float beta, float gamma)
        : alpha(alpha), beta(beta), gamma(gamma)
    {}

    __aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z, size_t bufferNum, size_t bufferBytes,
                                size_t gmIdx, size_t gmDataLen)
    {
        if (bufferBytes <= 0) {
            return;
        }
        pipe.InitBuffer(inQueueX, bufferNum, bufferBytes);
        pipe.InitBuffer(inQueueY, bufferNum, bufferBytes);
        pipe.InitBuffer(outQueueZ, bufferNum, bufferBytes);
        if (sizeof(T) == 1) {
            pipe.InitBuffer(bufXf32, bufferBytes * sizeof(float));
            pipe.InitBuffer(bufYf32, bufferBytes * sizeof(float));
            pipe.InitBuffer(bufZf32, bufferBytes * sizeof(float));
            pipe.InitBuffer(bufXf16, bufferBytes * sizeof(half));
        }

        xGm.SetGlobalBuffer((__gm__ T*)x + gmIdx, gmDataLen);
        yGm.SetGlobalBuffer((__gm__ T*)y + gmIdx, gmDataLen);
        zGm.SetGlobalBuffer((__gm__ T*)z + gmIdx, gmDataLen);
    }

    __aicore__ inline void CalcForAlign32(uint32_t idx, size_t len)
    {
        if (len <= 0) {
            return;
        }
        // copyIn
        auto xLocal = inQueueX.AllocTensor<T>();
        auto yLocal = inQueueY.AllocTensor<T>();

        DataCopy(xLocal, xGm[idx], len);
        DataCopy(yLocal, yGm[idx], len);
        inQueueX.EnQue(xLocal);
        inQueueY.EnQue(yLocal);

        // compute
        xLocal = inQueueX.DeQue<T>();
        yLocal = inQueueY.DeQue<T>();
        auto zLocal = outQueueZ.AllocTensor<T>();

        if constexpr(sizeof(T) == 1) {
            auto txLocal = bufXf32.Get<float>();
            auto tyLocal = bufYf32.Get<float>();
            auto tzLocal = bufZf32.Get<float>();
            auto txLocalF16 = bufXf16.Get<half>();

            Cast(txLocalF16, xLocal, RoundMode::CAST_NONE, len);
            Cast(txLocal, txLocalF16, RoundMode::CAST_NONE, len);

            Duplicate<float>(tyLocal, this->alpha, len);
            Mul(txLocal, txLocal, tyLocal, len);

            Cast(txLocalF16, yLocal, RoundMode::CAST_NONE, len);
            Cast(tyLocal, txLocalF16, RoundMode::CAST_NONE, len);
            Duplicate<float>(tzLocal, this->beta, len);
            Mul(tyLocal, tyLocal, tzLocal, len);

            Duplicate<float>(tzLocal, this->gamma, len);
            Add(txLocal, txLocal, tyLocal, len);
            Add(tzLocal, txLocal, tzLocal, len);

            Cast(txLocalF16, tzLocal, RoundMode::CAST_ODD, len);
            Cast(zLocal, txLocalF16, RoundMode::CAST_TRUNC, len);
        } else {
            Duplicate<T>(zLocal, this->alpha, len);
            Mul(xLocal, xLocal, zLocal, len);
            Duplicate<T>(zLocal, this->beta, len);
            Mul(yLocal, yLocal, zLocal, len);
            Duplicate<T>(zLocal, this->gamma, len);
            Add(xLocal, xLocal, yLocal, len);
            Add(zLocal, xLocal, zLocal, len);
        }

        outQueueZ.EnQue<T>(zLocal);
        inQueueX.FreeTensor(xLocal);
        inQueueY.FreeTensor(yLocal);

        // CopyOut
        zLocal = outQueueZ.DeQue<T>();
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

    GlobalTensor<T> xGm;
    GlobalTensor<T> yGm;
    GlobalTensor<T> zGm;

    float alpha;
    float beta;
    float gamma;
};

template <typename T>
__aicore__ void run_op(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR tiling, float ubVarNum)
{
    GET_TILING_DATA(tilingData, tiling);
    float alpha = tilingData.alpha;
    float beta = tilingData.beta;
    float gamma = tilingData.gamma;
    VectorScheduler sch(tilingData.size, GetBlockNum(), BUFFER_NUM, ubVarNum, sizeof(T));
    KernelAddWeighted<T> op(alpha, beta, gamma);
    size_t orgVecIdx = GetBlockIdx() * sch.dataLenPerCore;
    op.Init(x, y, z, sch.bufferNum, sch.dataBytesPerLoop, orgVecIdx, sch.dataLen);
    sch.run(&op, sch.dataLen);
}

extern "C" __global__ __aicore__ void add_weighted_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace,
                                                          GM_ADDR tiling)
{
    if (TILING_KEY_IS(1)) {
        constexpr float ubVarNum = 3;
        run_op<half>(x, y, z, tiling, ubVarNum);
    }
    else if (TILING_KEY_IS(2)) {
        constexpr float ubVarNum = 3;
        run_op<float>(x, y, z, tiling, ubVarNum);
    }
    else if (TILING_KEY_IS(3)) {
        constexpr float ubVarNum = 3 + 3 * 4 + 1 * 2;
        run_op<uint8_t>(x, y, z, tiling, ubVarNum);
    }
}

#ifndef __CCE_KT_TEST__

// call of kernel function
void add_weighted_custom_do(uint32_t blockDim, void *l2ctrl, void *stream, uint8_t *x, uint8_t *y, uint8_t *z,
                            uint8_t *workspace, uint8_t *tiling)
{
    add_weighted_custom<<<blockDim, l2ctrl, stream>>>(x, y, z, workspace, tiling);
}
#endif

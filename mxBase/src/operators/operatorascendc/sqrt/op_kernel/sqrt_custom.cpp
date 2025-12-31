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
 * Description: Sqrt operator kernel file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#include "kernel_operator.h"
#include "vector_scheduler.h"
using namespace AscendC;

constexpr int32_t BUFFER_NUM = 2; // double buffer

template <typename T>
class KernelSqrt {
public:
    __aicore__ inline KernelSqrt()
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
            pipe.InitBuffer(calcBufX, bufferBytes * sizeof(half));
        }

        xGm.SetGlobalBuffer((__gm__ T*)x + gmIdx, gmDataLen);
        zGm.SetGlobalBuffer((__gm__ T*)z + gmIdx, gmDataLen);
    }

    __aicore__ void CalcForAlign32(uint32_t idx, size_t len)
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
            LocalTensor<half> txLocal = calcBufX.Get<half>();
            Cast(txLocal, xLocal, RoundMode::CAST_NONE, len);
            Sqrt(txLocal, txLocal, len);
            Cast(zLocal, txLocal, RoundMode::CAST_NONE, len);
        } else {
            Sqrt(zLocal, xLocal, len);
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
    TBuf<TPosition::VECCALC> calcBufX;
    GlobalTensor<T> xGm;
    GlobalTensor<T> zGm;
};

template <typename T>
__aicore__ void run_op(GM_ADDR x, GM_ADDR z, GM_ADDR tiling, float ubVarNum)
{
    GET_TILING_DATA(tilingData, tiling);
    VectorScheduler sch(tilingData.size, GetBlockNum(), BUFFER_NUM, ubVarNum, sizeof(T));
    KernelSqrt<T> op;
    size_t orgVecIdx = GetBlockIdx() * sch.dataLenPerCore;
    op.Init(x, z, sch.bufferNum, sch.dataBytesPerLoop, orgVecIdx, sch.dataLen);
    sch.run(&op, sch.dataLen);
}

extern "C" __global__ __aicore__ void sqrt_custom(GM_ADDR x, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling) {
    if (TILING_KEY_IS(1)) {
        constexpr float ubVarNum = 2;
        run_op<half>(x, z, tiling, ubVarNum);
    }
    else if (TILING_KEY_IS(2)) {
        constexpr float ubVarNum = 2;
        run_op<float>(x, z, tiling, ubVarNum);
    }
    else if (TILING_KEY_IS(3)) {
        constexpr float ubVarNum = 4;
        run_op<uint8_t>(x, z, tiling, ubVarNum);
    }
}

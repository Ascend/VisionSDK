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
 * Description: abs operator kernel file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#include "kernel_operator.h"
#include "vector_scheduler.h"
using namespace AscendC;

constexpr int32_t BUFFER_NUM = 1;
constexpr float UB_VAR_NUM = 2;

template <typename T>
class KernelAbs {
public:
    __aicore__ inline KernelAbs()
    {}

    __aicore__ inline void Init(GM_ADDR x, GM_ADDR z, size_t bufferNum, size_t bufferBytes, size_t gmIdx,
                                size_t gmDataLen)
    {
        if (bufferBytes <= 0) {
            return;
        }
        pipe.InitBuffer(inQueueX, bufferNum, bufferBytes);
        pipe.InitBuffer(outQueueZ, bufferNum, bufferBytes);

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
            DataCopy(zLocal, xLocal, len);
        } else {
            Abs(zLocal, xLocal, len);
        }
        outQueueZ.EnQue<T>(zLocal);

        // CopyOut
        zLocal = outQueueZ.DeQue<T>();
        DataCopy(zGm[idx], zLocal, len);
        inQueueX.FreeTensor(xLocal);
        outQueueZ.FreeTensor(zLocal);
    }

protected:
    TPipe pipe;
    TQue<QuePosition::VECIN, BUFFER_NUM> inQueueX;
    TQue<QuePosition::VECOUT, BUFFER_NUM> outQueueZ;
    GlobalTensor<T> xGm;
    GlobalTensor<T> zGm;
};

template <typename T>
__aicore__ void run_op(GM_ADDR x, GM_ADDR z, GM_ADDR tiling, float ubVarNum)
{
    GET_TILING_DATA(tilingData, tiling);
    VectorScheduler sch(tilingData.size, GetBlockNum(), BUFFER_NUM, ubVarNum, sizeof(T));
    KernelAbs<T> op;
    size_t orgVecIdx = GetBlockIdx() * sch.dataLenPerCore;
    op.Init(x, z, sch.bufferNum, sch.dataBytesPerLoop, orgVecIdx, sch.dataLen);
    sch.run(&op, sch.dataLen);
}

extern "C" __global__ __aicore__ void abs_custom(GM_ADDR x, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling) {
    if (TILING_KEY_IS(1)) {
        run_op<half>(x, z, tiling, UB_VAR_NUM);
    } else if (TILING_KEY_IS(2)) {
        run_op<float>(x, z, tiling, UB_VAR_NUM);
    } else if (TILING_KEY_IS(3)) {
        run_op<uint8_t>(x, z, tiling, UB_VAR_NUM);
    }
}

#ifndef __CCE_KT_TEST__

// call of kernel function
void abs_custom_do(uint32_t blockDim, void *l2ctrl, void *stream, uint8_t *x, uint8_t *z, uint8_t *workspace,
                   uint8_t* tiling)
{
    abs_custom<<<blockDim, l2ctrl, stream>>>(x, z, workspace, tiling);
}
#endif

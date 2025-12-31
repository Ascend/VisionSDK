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
 * Description: vector scheduler framework file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef ASCENDC_OPERATOR_VECTOR_SCHEDULER_H
#define ASCENDC_OPERATOR_VECTOR_SCHEDULER_H

#include "kernel_operator.h"
using namespace AscendC;

constexpr size_t UB_SIZE_BYTE = 248 * 1024;

__aicore__ inline size_t UpAlignN(size_t n, size_t N)
{
return (n + N - 1) / N * N;
}

__aicore__ inline size_t DownAlignN(size_t n, size_t N)
{
return n / N * N;
}

class VectorComputer {
public:
    __aicore__ inline VectorComputer() {};
    __aicore__ inline void CalcForAlign32(uint32_t idx, size_t len) {};
};

/*
 * Schedule policy: When data address is consecutive during calculation,
 * fill the vector calculation of each tick as much as possible.
    c0  c1  c2  /c3 \
   |___|___|___|___|_|   L1 Inter-core(based on the number of cores)
   /    \________
  /   core n     \
 |___|___|___|___|_|     L2 Intra-core(based on ub size, 32 bytes aligned)
 ^   ^   ^   ^   |  \
/      \         |   \
|_32B*n_| * loop_|    \    L3 (based on ub size, 32 bytes aligned)
<calc i,n> *loop |_32B_|_32B_|__|   L4 (32 bytes aligned)
                          |_32B_|   L5 (32 bytes aligned, address rollback schedule)
                 < calc i, n >
                          <calc i,n>
*/
class VectorScheduler {
public:
    __aicore__ inline VectorScheduler(size_t contentLen, size_t blockDim, size_t bufferNum, float ubVarCount,
                                      size_t sizeofT, size_t alignBlockBytes = 32)
        : blockDim(blockDim), bufferNum(bufferNum), ubVarCount(ubVarCount), sizeofT(sizeofT),
          alignBlockBytes(alignBlockBytes)
    {
        auto blockIdx = GetBlockIdx();
        this->dataLenPerAlignBlock = this->alignBlockBytes / this->sizeofT;
        // L1
        this->dataLenPerCore = contentLen / this->blockDim;
        if (this->dataLenPerCore < this->dataLenPerAlignBlock) {
            this->dataLenPerCore = blockIdx == 0 ? contentLen : 0;
            this->dataLenTailL1 = 0;
        } else {
            this->dataLenTailL1 = contentLen % this->blockDim;
        }

        // L2
        int maxUbSizePerVar = UB_SIZE_BYTE / ubVarCount / this->bufferNum;
        this->dataBytesPerLoop = DownAlignN(maxUbSizePerVar, alignBlockBytes);
        this->dataLenPerLoop = this->dataBytesPerLoop / this->sizeofT;

        this->dataLen = this->dataLenPerCore;
        if (blockIdx == this->blockDim - 1) {
            this->dataLen += this->dataLenTailL1;
        }
        this->bufferBytesPerVar = this->dataLen > this->dataLenPerLoop ? this->dataBytesPerLoop : UpAlignN(
            this->dataLen * this->sizeofT, this->alignBlockBytes);
    }

    template<class Computer>
    __aicore__ inline void run(Computer* computer, size_t len) {
        //  c0  c1  c2   c3
        // |___|___|___|___|_|
        // <   >       <     >
        //             idx,len

        //  /   core n data  \
        // |___|___|___|___|_|
        // <      loops     >
        if (len <= 0) {
            return;
        }
        size_t loops = len / this->dataLenPerLoop;
        size_t tailLen = len % this->dataLenPerLoop;
        size_t tailLenAlignSize = DownAlignN(tailLen, this->dataLenPerAlignBlock);
        size_t tailLenBackoff = tailLen - tailLenAlignSize;

#ifdef __CCE_KT_TEST__
        std::cout << "sch= run on (core " << GetBlockIdx() << ")============" << "\n\t" <<
            "dataLen " << len << "\n\t" <<
            "- loops " << loops << "\n\t" <<
            "- dataLen/Loop " << this->dataLenPerLoop << "\n\t" <<
            "- dataBytes/Loop " << this->dataBytesPerLoop << "\n\t" <<
            "tailLen " << tailLen << "\n\t" <<
            "- tailLenAlignSize " << tailLenAlignSize << "\n\t" <<
            "- tailLenBackoff " << tailLenBackoff << "\n\t" << std::endl;
#endif
        uint32_t idx = 0;
        for (size_t i = 0; i < loops; i++) {
#ifdef __CCE_KT_TEST__
            std::cout << "sch= calc loop(" << i + 1 <<"/" << loops << ") ==========" << "\n\t" <<
                "dataLen/Loop " << this->dataLenPerLoop << "\n\t" <<
                "- loop idx " << idx << "\n\t" <<
                "- len " << this->dataLenPerLoop << "\n\t" << std::endl;
#endif
            computer->CalcForAlign32(idx, this->dataLenPerLoop);
            idx = idx + this->dataLenPerLoop;
        }

        //        data
        // |___|___|___|___|_|
        //                 < >
        // |_32B_|_32B_|__|
        // <           >
        if (tailLenAlignSize) {
            idx = loops * this->dataLenPerLoop; // 0
            computer->CalcForAlign32(idx, tailLenAlignSize);
#ifdef __CCE_KT_TEST__
            std::cout << "sch= calc tail(align32) =======" << "\n\t" <<
                "tailLenAlignSize " << tailLenAlignSize << "\n\t" <<
                "- tailIdx  " << idx << "\n\t" <<
                "- len " << tailLenAlignSize << "\n\t" << std::endl;
#endif
        }

        // |_32B_|_32B_|__|
        //          |_32B_|
        //  backoff <     >
        if (tailLenBackoff > 0) {
            idx = len >= this->dataLenPerAlignBlock ? len - this->dataLenPerAlignBlock : 0;
#ifdef __CCE_KT_TEST__
            std::cout << "sch= calc tail(backoff) ========" << "\n\t" <<
                "tailLenBackoff " << tailLenBackoff << "\n\t" <<
                "- backoff idx " << idx << "\n\t" <<
                "- calcDataLen " << this->dataLenPerAlignBlock << "\n\t" << std::endl;
#endif
            computer->CalcForAlign32(idx, this->dataLenPerAlignBlock);
        }
    }

public:
    float ubVarCount;
    size_t blockDim;
    size_t bufferNum;

    size_t sizeofT;
    size_t alignBlockBytes;

    size_t dataLenPerAlignBlock;
    // L1
    size_t dataLen;
    size_t dataLenPerCore;
    size_t dataLenTailL1;
    size_t bufferBytesPerVar;
    // L2
    size_t dataLenPerLoop;
    size_t dataBytesPerLoop;
    size_t loopL2;
    size_t dataLenTailL2;
};

#endif // ASCENDC_OPERATOR_VECTOR_SCHEDULER_H

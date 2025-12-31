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
 * Description: Tensor Check Framework CommonUtils include file.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#ifndef MX_TENSOROPERATION_TENSOR_CHECK_UTILS_H
#define MX_TENSOROPERATION_TENSOR_CHECK_UTILS_H

#include <vector>
#include <algorithm>
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/E2eInfer/DataType.h"

namespace MxBase {
    constexpr uint32_t DIM_HW = 2;
    constexpr uint32_t DIM_HWC = 3;
    constexpr int IDX_HWC_H = 0;
    constexpr int IDX_HWC_W = 1;
    constexpr int IDX_HWC_C = 2;
    constexpr int IDX_MIN_W = 0;
    constexpr int IDX_MIN_H = 1;
    constexpr int IDX_MAX_W = 2;
    constexpr int IDX_MAX_H = 3;

    struct TensorImgParam {
        std::string tensorName;
        const Tensor &tensor_;
        bool allowEmpty = false;

        int32_t deviceId = -1;
        uint32_t height = 0;
        uint32_t width = 0;
        uint32_t channels = 0;
        uint32_t dim = 0;
        uint32_t roiX0 = 0;
        uint32_t roiY0 = 0;
        uint32_t roiX1 = 0;
        uint32_t roiY1 = 0;
        uint32_t roiH = 0;
        uint32_t roiW = 0;
        uint32_t roiStart = 0;
        MemoryData::MemoryType memType = MemoryData::MemoryType::MEMORY_HOST;
        TensorDType dataType = TensorDType::UNDEFINED;

        TensorImgParam(std::string name, const Tensor &tensor, bool isAllowEmpty = false);
    };

    class TensorImgParamChecker {
    public:
        TensorImgParamChecker(std::string tensorName,
                              std::vector <uint32_t> &hwRange,
                              std::vector <MemoryData::MemoryType> &memTypeChoices,
                              std::vector <TensorDType> &dataTypeChoices,
                              std::vector <uint32_t> &channelChoices,
                              std::vector <uint32_t> &dimChoices,
                              bool allowEmpty = false);

        bool CheckNotEmpty(const Tensor &tensor);
        bool CheckDim(uint32_t dim);
        bool CheckMemType(MemoryData::MemoryType memType);
        bool CheckDataType(TensorDType dataType);
        bool CheckChannels(uint32_t channels);
        bool CheckImgSize(uint32_t width, uint32_t height);
        bool Check(const TensorImgParam &tensorImgParam);

        static std::string ToString(TensorDType dataType);
        static std::string ToString(MemoryData::MemoryType memoryType);

    private:
        std::string tensorName_;
        std::vector <uint32_t> &hwRange_;  // minW, minH, maxW, maxH
        std::vector <MemoryData::MemoryType> &memTypeChoices_;
        std::vector <TensorDType> &dataTypeChoices_;
        std::vector <uint32_t> &channelChoices_;
        std::vector <uint32_t> &dimChoices_;
        bool allowEmpty_;
    };
}

#endif
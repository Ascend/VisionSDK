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
 * Description: PostProcess common utils.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include <random>
#include <vector>
#include "MxBase/Tensor/TensorBase/TensorBase.h"
class PostProcessUtils {
public:
    template <typename T>
    static void SetRandomTensorValue(std::vector<MxBase::TensorBase> &tensors, T min, T max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distrib(static_cast<float>(min), static_cast<float>(max));
        for (size_t i = 0; i < tensors.size(); i++) {
            T *data = reinterpret_cast<T *>(tensors[i].GetBuffer());
            for (size_t j = 0; j < tensors[i].GetSize(); j++) {
                data[j] = static_cast<T>(distrib(gen));
            }
        }
    }
};
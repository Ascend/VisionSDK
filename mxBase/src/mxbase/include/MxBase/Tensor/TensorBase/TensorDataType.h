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
 * Description: TensorDataType Interface.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef TENSOR_DATATYPE_H
#define TENSOR_DATATYPE_H

#include <vector>
#include <string>
#include <memory>
#include <map>

namespace MxBase {
enum TensorDataType {
    TENSOR_DTYPE_UNDEFINED = -1,
    TENSOR_DTYPE_FLOAT32 = 0,
    TENSOR_DTYPE_FLOAT16 = 1,
    TENSOR_DTYPE_INT8 = 2,
    TENSOR_DTYPE_INT32 = 3,
    TENSOR_DTYPE_UINT8 = 4,
    TENSOR_DTYPE_INT16 = 6,
    TENSOR_DTYPE_UINT16 = 7,
    TENSOR_DTYPE_UINT32 = 8,
    TENSOR_DTYPE_INT64 = 9,
    TENSOR_DTYPE_UINT64 = 10,
    TENSOR_DTYPE_DOUBLE64 = 11,
    TENSOR_DTYPE_BOOL = 12
};
struct BaseTensor {
    void* buf;
    std::vector<int> shape;
    size_t size;
};
static std::map<int, std::string> TensorDataTypeStr = {
    {-1, "TENSOR_DTYPE_UNDEFINED"},
    {0, "TENSOR_DTYPE_FLOAT32"},
    {1, "TENSOR_DTYPE_FLOAT16"},
    {2, "TENSOR_DTYPE_INT8"},
    {3, "TENSOR_DTYPE_INT32"},
    {4, "TENSOR_DTYPE_UINT8"},
    {6, "TENSOR_DTYPE_INT16"},
    {7, "TENSOR_DTYPE_UINT16"},
    {8, "TENSOR_DTYPE_UINT32"},
    {9, "TENSOR_DTYPE_INT64"},
    {10, "TENSOR_DTYPE_UINT64"},
    {11, "TENSOR_DTYPE_DOUBLE64"},
    {12, "TENSOR_DTYPE_BOOL"},
};
}

#endif
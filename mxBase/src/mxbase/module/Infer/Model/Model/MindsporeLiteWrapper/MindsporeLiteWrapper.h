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
 * Description: Mindspore lite wrapper interface.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#ifndef MINDSPORELITEWRAPPER_H
#define MINDSPORELITEWRAPPER_H

#include <string>
#include <vector>
#include "MxBase/E2eInfer/DataType.h"
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxBase {
enum class MiddleDataType {
    UNDEFINED = -1,
    FLOAT32 = 0,
    FLOAT16 = 1,
    INT8 = 2,
    INT32 = 3,
    UINT8 = 4,
    INT16 = 6,
    UINT16 = 7,
    UINT32 = 8,
    INT64 = 9,
    UINT64 = 10,
    DOUBLE64 = 11,
    BOOL = 12
};

enum class MiddleDataFormat {
    NCHW = 0,
    NHWC = 1
};

struct MiddleTensor {
    void *dataPtr = nullptr;
    std::vector<int64_t> dataShape = {};
    size_t dataSize = 0;
    MiddleDataType dataType = MiddleDataType::UNDEFINED;
    MiddleDataFormat dataFormat = MiddleDataFormat::NCHW;
};

class MindsporeLiteWrapper {
public:
    MindsporeLiteWrapper() = default;

    virtual ~MindsporeLiteWrapper() = default;

    virtual APP_ERROR InitFromFile(const char *modelPath) = 0;

    virtual APP_ERROR InitFromMemory(const ModelLoadOptV2 &mdlLoadOpt) = 0;

    virtual APP_ERROR SetDevice(const size_t deviceId) = 0;

    virtual APP_ERROR Infer(std::vector<MiddleTensor> &inputs, std::vector<MiddleTensor> &outputs) = 0;

    virtual APP_ERROR GetModelInputs(std::vector<MiddleTensor> &inputTensors) = 0;

    virtual APP_ERROR GetModelOutputs(std::vector<MiddleTensor> &outputTensors) = 0;
};

using CreateInstance = MindsporeLiteWrapper*(*)();

using DestroyInstance = void(*)(MindsporeLiteWrapper*);
} // MxBase namespace end
#endif

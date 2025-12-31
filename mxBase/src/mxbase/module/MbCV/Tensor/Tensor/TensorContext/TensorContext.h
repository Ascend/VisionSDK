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
 * Description: Set the context to the corresponding device ID.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef TENSOR_CONTEXT_H
#define TENSOR_CONTEXT_H

#include <map>
#include <memory>
#include <mutex>
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxBase {
enum class ContextMode {
    CONTEXT_IDEL = 0,
    CONTEXT_USING
};

class TensorContext {
public:
    TensorContext();
    ~TensorContext();
    static std::shared_ptr<TensorContext> GetInstance();
    APP_ERROR SetContext(const uint32_t &deviceId);
};
}
#endif
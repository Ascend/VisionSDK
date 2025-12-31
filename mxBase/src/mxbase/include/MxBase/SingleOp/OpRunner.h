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
 * Description: Runtime library of a single operator.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef OP_RUNNER_H
#define OP_RUNNER_H

#include "MxBase/SingleOp/OperatorDesc.h"

namespace MxBase {
/**
 * Op Runner
 */
class OpRunner {
public:
    /**
     * @brief Init op runner
     * @param [in] path: op path
     */
    APP_ERROR Init(std::string path, std::string type);

    /**
     * @brief DeInit op runner
     */
    APP_ERROR DeInit();

    /**
     * @brief Run op
     * @return run result
     */
    APP_ERROR RunOp(OperatorDesc opDesc);

private:
    void* opStream_ = nullptr;
    std::string opType_ = "";
    void* opAttr_ = nullptr;
};
}
#endif // OP_RUNNER_H

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
 * Description: Tensor BackgroundReplace Operation
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */
#include "MxBase/E2eInfer/DataType.h"
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/TikFramework.h"
#include "MxBase/E2eInfer/TensorOperation/TensorReplace.h"

namespace MxBase {
APP_ERROR BackgroundReplace(Tensor &background, const Tensor &replace, const Tensor &mask, Tensor &dst,
    AscendStream &stream)
{
    APP_ERROR ret = TensorReplace(background, replace, mask, dst, false, stream);
    if (ret != APP_ERR_OK) {
        LogError << "BackgroundReplace: TensorReplace failed." << GetErrorInfo(ret);
    }
    return ret;
}
}
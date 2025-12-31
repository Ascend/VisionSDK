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
 * Description: Tensor operation framework implement file.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/OpCall.h"
#include "MxBase/DeviceManager/DeviceManager.h"

namespace MxBase {
    APP_ERROR RunOp(RunOpParam &para, AscendStream &stream)
    {
        if (para.fixedType == "dsl") {
            return DslRunOp(para.opType, para.srcVec, para.dstVec, stream, para.dslParam);
        } else if (para.fixedType == "aclnn") {
            return AclnnRunOp(para.opType, para.srcVec, para.dstVec, static_cast<void *>(&para.ascendcParam), stream);
        } else if (DeviceManager::IsAscend310P()) {
            return AclnnRunOp(para.opType, para.srcVec, para.dstVec, static_cast<void *>(&para.ascendcParam), stream);
        } else if (DeviceManager::IsAscend310B()) {
            return DslRunOp(para.opType, para.srcVec, para.dstVec, stream, para.dslParam);
        } else {
            LogError << "RunOp: current op is only supported on device 310P/310B now, current device is "
                     << DeviceManager::GetSocName() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
}
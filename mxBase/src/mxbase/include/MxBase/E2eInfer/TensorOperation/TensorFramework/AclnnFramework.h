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
* Description: Tensor Operation AscendC Framework header file.
* Author: MindX SDK
* Create: 2024
* History: NA
*/

#ifndef MXBASE_ACLNNFRAMEWORK_H
#define MXBASE_ACLNNFRAMEWORK_H

#include <iostream>
#include "MxBase/E2eInfer/TensorOperation/TensorFramework/CommonUtils.h"

namespace MxBase {
    struct CommonAclnnPara {
        std::vector<int32_t> intAttr;
        std::vector<float> floatAttr;
        OpDataType dataType = OP_DT_UNDEFINED;
    };

    APP_ERROR AclnnRunOp(const std::string &opType, const std::vector<Tensor> &srcVec,
                         std::vector<Tensor> &dstVec, void* tiling, AscendStream &stream);
}
#endif
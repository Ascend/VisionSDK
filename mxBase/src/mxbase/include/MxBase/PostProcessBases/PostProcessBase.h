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
 * Description: Used to initialize the model post-processing.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef POST_PROCESS_BASE_H
#define POST_PROCESS_BASE_H

#include <vector>
#include <map>
#include <string>

#include "MxBase/PostProcessBases/PostProcessDataType.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"
#include "MxBase/ConfigUtil/ConfigUtil.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Common/Version.h"

namespace MxBase {
class PostProcessBase {
public:
    PostProcessBase() = default;

    PostProcessBase(const PostProcessBase &other) = default;

    virtual ~PostProcessBase() = default;

    PostProcessBase& operator=(const PostProcessBase &other);

    virtual APP_ERROR Init(const std::map<std::string, std::string> &postConfig);

    virtual APP_ERROR DeInit();

    virtual uint64_t GetCurrentVersion();

protected:
    APP_ERROR LoadConfigData(const std::map<std::string, std::string> &postConfig);

    APP_ERROR CheckAndMoveTensors(std::vector<TensorBase> &tensors);

    void* GetBuffer(const TensorBase& tensor, uint32_t index) const;

    bool JudgeResizeType(const ResizedImageInfo& resizedImageInfo);

protected:
    MxBase::ConfigData configData_;
    bool checkModelFlag_ = true;
    bool isInitConfig_ = false;

    const uint32_t ZERO_BYTE = 0;
    const uint32_t ONE_BYTE = 1;
    const uint32_t TWO_BYTE = 2;
    const uint32_t FOUR_BYTE = 4;
    const uint32_t EIGHT_BYTE = 8;
    const uint32_t MAX_TENSOR_VEC_SIZE = 128;
};
}


#endif
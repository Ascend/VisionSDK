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
 * Description: Config load for preload jason config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include "OperationLoaders/ConfigLoader.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxBase {
    ConfigLoader::ConfigLoader(std::string configFile)
    {
        jsonPtr_ = MemoryHelper::MakeShared<MxBase::JsonPtr>(configFile);
        if (jsonPtr_ == nullptr) {
            LogError << "Create JsonPtr jsonPtr_ failed, failed to allocated memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
        }
    }

    ConfigLoader::~ConfigLoader() {}

    APP_ERROR ConfigLoader::GetOpInfoByName(const std::string opName, JsonPtr &jsonPtr)
    {
        return jsonPtr_->GetOpInfoByName(opName, jsonPtr);
    }

    APP_ERROR ConfigLoader::GetAllOpNames(std::vector<std::string> &out)
    {
        return jsonPtr_->GetAllOpNames(out);
    }
}
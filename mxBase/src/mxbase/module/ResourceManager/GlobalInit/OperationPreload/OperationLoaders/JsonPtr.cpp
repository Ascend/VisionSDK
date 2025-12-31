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

#include "MxBase/Utils/StringUtils.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "JsonPtr.h"

namespace MxBase {
    JsonPtr::JsonPtr()
    {
        jsonObjPtr_ = MemoryHelper::MakeShared<MxBase::JsonObject>();
        if (jsonObjPtr_ == nullptr) {
            LogError << "Create JsonObject jsonObjPtr_ failed, failed to allocated memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
        }
    }

    JsonPtr::JsonPtr(std::string configFile)
    {
        jsonObjPtr_ = MemoryHelper::MakeShared<MxBase::JsonObject>(configFile);
        if (jsonObjPtr_ == nullptr) {
            LogError << "Create JsonObject jsonObjPtr_ failed, failed to allocated memory."
                     << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_ALLOC_MEM));
        }
    }

    APP_ERROR JsonPtr::GetOpInfoByName(const std::string opName, JsonPtr &jsonPtr)
    {
        auto iter = (jsonObjPtr_->allOpJson_).find(opName);
        if ((iter == (jsonObjPtr_->allOpJson_).end())) {
            LogError << "Op [" << opName << "] not found in map." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        jsonPtr.jsonObjPtr_->cfgJson_ = jsonObjPtr_->allOpJson_[opName];
        return APP_ERR_OK;
    }

    APP_ERROR JsonPtr::GetAllOpNames(std::vector<std::string> &out)
    {
        out = jsonObjPtr_->allOps_;
        return APP_ERR_OK;
    }

    APP_ERROR JsonPtr::GetOpName(std::string &opName) const
    {
        return jsonObjPtr_->GetOpName(opName);
    }

    APP_ERROR JsonPtr::GetPreloadList(std::string &preloadList) const
    {
        return jsonObjPtr_->GetPreloadList(preloadList);
    }

    APP_ERROR JsonPtr::GetAttrNameByIndex(size_t index, std::string &attrName) const
    {
        return jsonObjPtr_->GetValByIndexAndKey(index, "attr_name", attrName);
    }

    APP_ERROR JsonPtr::GetAttrTypeByIndex(size_t index, std::string &attrIndex) const
    {
        return jsonObjPtr_->GetValByIndexAndKey(index, "attr_type", attrIndex);
    }

    APP_ERROR JsonPtr::GetAttrValByIndex(size_t index, std::string &attrVal) const
    {
        return jsonObjPtr_->GetValByIndexAndKey(index, "attr_val", attrVal);
    }

    APP_ERROR JsonPtr::GetInputShapeByIndex(size_t index, std::string &inputShape) const
    {
        return jsonObjPtr_->GetValByIndexAndKey(index, "input_shape", inputShape);
    }

    APP_ERROR JsonPtr::GetInputTypeByIndex(size_t index, std::string &inputType) const
    {
        return jsonObjPtr_->GetValByIndexAndKey(index, "input_type", inputType);
    }

    APP_ERROR JsonPtr::GetOutputShapeByIndex(size_t index, std::string &outputShape) const
    {
        return jsonObjPtr_->GetValByIndexAndKey(index, "output_shape", outputShape);
    }

    APP_ERROR JsonPtr::GetOutputTypeByIndex(size_t index, std::string &outputType) const
    {
        return jsonObjPtr_->GetValByIndexAndKey(index, "output_type", outputType);
    }

    APP_ERROR JsonPtr::CheckAllkeyByIndex(size_t index) const
    {
        return jsonObjPtr_->CheckAllkeyByIndex(index);
    }
}
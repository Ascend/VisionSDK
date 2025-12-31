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

#ifndef MXBASE_JSONPTR_H
#define MXBASE_JSONPTR_H

#include "MxBase/Log/Log.h"
#include "JsonObject.h"


namespace MxBase {

    class JsonPtr {
    public:
        JsonPtr();

        explicit JsonPtr(std::string configFile);

        APP_ERROR GetOpInfoByName(const std::string opName, JsonPtr &jsonPtr);

        APP_ERROR GetAllOpNames(std::vector<std::string> &out);

        APP_ERROR GetOpName(std::string &opName) const;

        APP_ERROR GetPreloadList(std::string &preloadList) const;

        APP_ERROR GetAttrNameByIndex(size_t index, std::string &attrName) const;

        APP_ERROR GetAttrTypeByIndex(size_t index, std::string &attrType) const;

        APP_ERROR GetAttrValByIndex(size_t index, std::string &attrVal) const;

        APP_ERROR GetInputShapeByIndex(size_t index, std::string &inputShape) const;

        APP_ERROR GetInputTypeByIndex(size_t index, std::string &inputType) const;

        APP_ERROR GetOutputShapeByIndex(size_t index, std::string &outputShape) const;

        APP_ERROR GetOutputTypeByIndex(size_t index, std::string &outputType) const;

        APP_ERROR CheckAllkeyByIndex(size_t index) const;

    private:
        std::shared_ptr<MxBase::JsonObject> jsonObjPtr_;
    };
}
#endif

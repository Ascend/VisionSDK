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

#ifndef MXBASE_CONFIGLOADER_H
#define MXBASE_CONFIGLOADER_H

#include "MxBase/Log/Log.h"
#include "JsonPtr.h"

namespace MxBase {
    class ConfigLoader {
    public:
        explicit ConfigLoader(std::string configFile);

        ~ConfigLoader();

        APP_ERROR GetOpInfoByName(const std::string opName, JsonPtr &jsonPtr);

        APP_ERROR GetAllOpNames(std::vector<std::string> &out);

    private:
        std::shared_ptr<MxBase::JsonPtr> jsonPtr_;
    };
}
#endif

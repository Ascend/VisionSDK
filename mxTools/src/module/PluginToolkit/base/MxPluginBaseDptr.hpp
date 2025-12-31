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
 * Description: Private MxPluginBase interfaces for internal use only.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MX_PLUGIN_BASE_DPTR_H
#define MX_PLUGIN_BASE_DPTR_H

namespace MxTools {
class SDK_UNAVAILABLE_FOR_OTHER MxPluginBaseDptr {
public:

    void* elementInstance_;
    std::mutex configParamMutex_;
};
}
#endif // MX_PLUGIN_BASE_DPTR_H_
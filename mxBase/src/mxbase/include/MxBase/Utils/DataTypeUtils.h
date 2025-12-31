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
 * Description: Data type related function processing.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#ifndef MX_DATATYPEUTILS_H
#define MX_DATATYPEUTILS_H
#include <sys/stat.h>

namespace MxBase {
    class DataTypeUtils {
    public:
        /** Convert float32 value to float16 value
         * @param in float32 value to be converted
         * @param out converted float16 value stored as uint16_t format
         * @return
         */
        static void Float32ToFloat16(uint16_t *__restrict out, float &in);
    };
}
#endif // MX_MX_DATATYPEUTILS_H

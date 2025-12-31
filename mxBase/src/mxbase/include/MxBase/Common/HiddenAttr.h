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
 * Description: Define macros for other files.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MINDX_SDK_HIDDENATTR_H
#define MINDX_SDK_HIDDENATTR_H

#define SDK_AVAILABLE_FOR_OUT __attribute__((visibility("default")))

#define SDK_AVAILABLE_FOR_IN __attribute__((visibility("default")))

#define SDK_UNAVAILABLE_FOR_OTHER __attribute__((visibility("hidden")))

#define SDK_DEPRECATED_FOR(f) __attribute__((__deprecated__("Use '" #f "' instead")))

#endif
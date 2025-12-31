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
 * Description: Obtaining the SDK Version.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MINDX_SDK_VERSION_H
#define MINDX_SDK_VERSION_H

#define MINDX_SDK_VERSION_MAJOR 5
#define MINDX_SDK_VERSION_MINOR 0
#define MINDX_SDK_VERSION_MICRO 0
#define VERSION_INTERVAL 1000

#include <string>
#include "MxBase/Common/HiddenAttr.h"
namespace MxBase {
/**
 * Get MindX SDK mxVision version
 *
 * @return Version number: string.
 *
 */
std::string GetSDKVersion();
}

/**
 * Get MindX SDK version
 *
 * @return Version number calculated from major, minor, and micro version, [0, UINT64_MAX].
 *
 */
#define MINDX_SDK_VERSION ((uint64_t)(MINDX_SDK_VERSION_MAJOR) * (VERSION_INTERVAL) * (VERSION_INTERVAL) + \
    (uint64_t)(MINDX_SDK_VERSION_MINOR) * (VERSION_INTERVAL) + (uint64_t)(MINDX_SDK_VERSION_MICRO))

/**
 * Get MindX SDK major version.
 *
 * @return Major version, [0, UINT32_MAX].
 */
#define MINDX_SDK_MAJOR_VERSION MINDX_SDK_VERSION_MAJOR

/**
 * Get MindX SDK major version.
 *
 * @return Minor version, [0, UINT32_MAX].
 */
#define MINDX_SDK_MINOR_VERSION MINDX_SDK_VERSION_MINOR

/**
 * Get MindX SDK major version.
 *
 * @return Micro version, [0, UINT32_MAX].
 */
#define MINDX_SDK_MICRO_VERSION MINDX_SDK_VERSION_MICRO

#endif

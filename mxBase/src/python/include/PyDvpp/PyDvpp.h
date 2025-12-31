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
 * Description: Python-based DVPP interface.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#ifndef PY_DVPP_H
#define PY_DVPP_H

#include <string>
#include "PyImage/PyImage.h"
#include "MxBase/E2eInfer/DataType.h"

namespace PyBase {
PyBase::Image read_image(const std::string& inputPath, int deviceId, const MxBase::ImageFormat& decodeFormat);
PyBase::Image resize(const PyBase::Image& inputImage, const MxBase::Size& size,
                     const MxBase::Interpolation& interpolation);
} // namespace PyBase
#endif
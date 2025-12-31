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
 * Description: Smart Pointer Remover.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <opencv2/opencv.hpp>
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"

namespace MxTools {
bool MatPtrDeleter(uint64_t dataptr, uint64_t matptr)
{
    cv::Mat* mat = (cv::Mat*)matptr;
    if (mat != nullptr) {
        if ((void *)dataptr == mat->data) {
            delete mat;
            mat = nullptr;
            return true;
        } else {
            delete mat;
            mat = nullptr;
            return false;
        }
    }
    return false;
}
}
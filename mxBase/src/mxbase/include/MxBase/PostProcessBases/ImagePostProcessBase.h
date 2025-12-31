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
 * Description: Post-processing of image tasks.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef IMAGE_POST_PROCESS_H
#define IMAGE_POST_PROCESS_H
#include "MxBase/PostProcessBases/PostProcessBase.h"

namespace MxBase {
class ImagePostProcessBase : public PostProcessBase {
public:
    void SetCropRoiBoxes(std::vector<MxBase::CropRoiBox> cropRoiBoxes)
    {
        cropRoiBoxes_ = cropRoiBoxes;
    }

protected:
    std::vector<MxBase::CropRoiBox> cropRoiBoxes_ = {};
};
}

#endif
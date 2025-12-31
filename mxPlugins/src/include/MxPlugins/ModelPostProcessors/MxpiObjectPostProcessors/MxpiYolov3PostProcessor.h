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
 * Description: Yolov3 model post-processing.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPLUGINS_MPYOLOV3POSTPROCESSOR_H
#define MXPLUGINS_MPYOLOV3POSTPROCESSOR_H

#include "MxPlugins/ModelPostProcessors/ModelPostProcessorBase/MxpiObjectPostProcessorBase.h"
#include "MxBase/ModelPostProcessors/ObjectPostProcessors/Yolov3PostProcessor.h"

class MxpiYolov3PostProcessor : public MxPlugins::MxpiObjectPostProcessorBase {
public:
    APP_ERROR Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) override;

    APP_ERROR DeInit() override;

    APP_ERROR Process(std::shared_ptr<void>& metaDataPtr, MxBase::PostProcessorImageInfo postProcessorImageInfo,
        std::vector<MxTools::MxpiMetaHeader>& headerVec, std::vector<std::vector<MxBase::BaseTensor>>& tensors)
        override;

private:
    MxBase::Yolov3PostProcessor postProcessorInstance_;
};

extern "C" {
    std::shared_ptr<MxPlugins::MxpiModelPostProcessorBase> GetInstance();
}

#endif
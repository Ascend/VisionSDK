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
 * Description: Post-processing of the resnet binary attribute model.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPLUGINS_RESNETATTRIBUTEPOSTPROCESSOR_H
#define MXPLUGINS_RESNETATTRIBUTEPOSTPROCESSOR_H

#include <MxBase/ErrorCode/ErrorCode.h>
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#include "MxPlugins/ModelPostProcessors/ModelPostProcessorBase/MxpiModelPostProcessorBase.h"
#include "MxBase/Maths/FastMath.h"

class ResNetAttributePostProcessor : public MxPlugins::MxpiModelPostProcessorBase {
public:
    APP_ERROR Init(const std::string& configPath, const std::string& labelPath, MxBase::ModelDesc modelDesc) override;

    APP_ERROR DeInit() override;

    APP_ERROR Process(std::shared_ptr<void>& metaDataPtr, MxBase::PostProcessorImageInfo postProcessorImageInfo,
        std::vector<MxTools::MxpiMetaHeader>& headerVec, std::vector<std::vector<MxBase::BaseTensor>>& tensors)
        override;
    
private:
    APP_ERROR GetAttributeIndex(std::string& strAttributeIndex);

    void ResNetAttributeOutput(std::vector<std::shared_ptr<void>>& featLayerData);
    void MakeAttributeMap(std::ifstream& in, std::string& stringRead);
    void MakeNameMap(std::ifstream& in, std::string& stringRead);
    void MakeValueMap(std::ifstream& in, std::string& stringRead);

private:
    // threshold for object filtering.
    size_t attributeNum_;
    std::vector<std::vector<int>> attributeIndex_;
    std::vector<float> confidence_;
    std::vector<std::string> result_;
    // labels info
    std::vector<std::string> attributeNameVec_;
    std::vector<std::string> attributeValueVec_;
    MxBase::ConfigData configData_;
    std::string activationFunction_;
};

extern "C" {
    std::shared_ptr<MxPlugins::MxpiModelPostProcessorBase> GetInstance();
}

#endif // RESNETAttrPOSTPROCESSOR_H
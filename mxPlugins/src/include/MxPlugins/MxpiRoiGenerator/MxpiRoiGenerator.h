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
 * Description: Plugin for generate roi for image crop.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MX_MXPIROIGENERATOR_H
#define MX_MXPIROIGENERATOR_H

#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxBase/PostProcessBases/PostProcessDataType.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/Utils/SplitterUtils.h"

namespace MxPlugins {
class MxpiRoiGenerator : public MxTools::MxPluginBase {
public:
    /**
    * @description: Init configs.
    * @param configParamMap: config.
    * @return: Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap) override;

    /**
    * @description: DeInit.
    * @return: Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @description: MxpiRoiGenerator plugin define properties.
    * @return: properties.
    */
    static std::vector<std::shared_ptr<void>> DefineProperties();

    /**
    * @description: MxpiRoiGenerator plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

    /**
    * @api
    * @brief Define the number and data type of input ports.
    * @return MxTools::MxpiPortInfo.
    */
    static MxTools::MxpiPortInfo DefineInputPorts();

    /**
    * @api
    * @brief Define the number and data type of output ports.
    * @return MxTools::MxpiPortInfo.
    */
    static MxTools::MxpiPortInfo DefineOutputPorts();

private:
    APP_ERROR GetImageSize(MxTools::MxpiBuffer& buffer);

    APP_ERROR AddMetadataAndSendBuff(MxTools::MxpiBuffer &buffer, MxTools::MxpiObjectList &cropObjectList,
        MxTools::MxpiObjectList &mergeObjectList);

    APP_ERROR JudgeUsage();

    APP_ERROR JudgeSplitType(std::map<std::string, std::shared_ptr<void>> &configParamMap);

    APP_ERROR CustomProcess(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer);

    APP_ERROR CheckRoiWithImageSize();

private:
    int blockHeight_ = 1;
    int blockWidth_ = 1;
    int overlapHeight_ = 0;
    int overlapWidth_ = 0;
    int chessboardHeight_ = 1;
    int chessboardWidth_ = 1;
    int imageWidth_ = 0;
    int imageHeight_ = 0;
    Type splitType_;
    std::vector<MxBase::CropRoiBox> cropRoiVec_ {};
    std::vector<MxBase::CropRoiBox> mergeRoiVec_ {};
    bool needMergeRoi_ = true;
};
}

#endif // MX_MXPIROIGENERATOR_H

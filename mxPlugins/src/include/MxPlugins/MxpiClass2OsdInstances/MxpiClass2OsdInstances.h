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
 * Description: Classification result transfer to drawing unit plug-in.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MXPLUGINS_MXPICLASS2OSDINSTANCES_H
#define MXPLUGINS_MXPICLASS2OSDINSTANCES_H

#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/PostProcessBases/PostProcessDataType.h"
#include "MxBase/Utils/OSDUtils.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/Proto/MxpiOSDType.pb.h"

/**
 * This plugin is used for stream distribute. Temporarily only class id and channel id is supported.
 */
namespace MxPlugins {
class MxpiClass2OsdInstances : public MxTools::MxPluginBase {
public:
    /**
    * @description: Init configs.
    * @param configParamMap: config.
    * @return: Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap) override;

    /**
    * @description: DeInit device.
    * @return: Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @description: MxpiClass2OsdInstances plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer) override;

    /**
    * @description: MxpiClass2OsdInstances plugin define properties.
    * @return: properties.
    */
    static std::vector<std::shared_ptr<void>> DefineProperties();

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
    APP_ERROR ErrorProcessing(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, APP_ERROR ret);

    APP_ERROR InitAndRefreshProps(bool refreshStage = false);

    APP_ERROR InitRectConfig();

    APP_ERROR CheckMetaData(MxTools::MxpiMetadataManager &manager, MxTools::MxpiClassList &classList);

    APP_ERROR GetVisionInfo(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
                            std::vector<MxBase::ImagePreProcessInfo> &imagePreProcessInfos);

    APP_ERROR CoreProcess(MxTools::MxpiClassList &classList, std::vector<MxBase::ImagePreProcessInfo> &infos,
                          MxTools::MxpiOsdInstancesList &osdInstancesList);

    APP_ERROR CreateClassRect(MxTools::MxpiOsdInstances &mxpiOsdInstances, MxBase::RoiBox &textSpace, int classId);

    APP_ERROR CreateText(MxTools::MxpiOsdInstances &mxpiOsdInstances,
                         MxBase::RoiBox &textSpace, const std::string &str);

private:
    std::string dataSourceClass_ = "";
    std::string dataSourceImage_ = "";

    // for class
    MxBase::TextPositionType position_ = MxBase::LEFT_TOP_IN;
    uint32_t topK_ = 1;

    // for text
    int fontFace_ = 0;
    double fontScale_ = 1.0f;
    int fontThickness_ = 1;
    int fontLineType_ = 0;

    // for rectangle
    bool createRect_ = false;
    std::map<int, std::vector<int>> colorMap_ = {};
    int rectThickness_ = 1;
    int rectLineType_ = 0;

    std::map<int, int> countEachRoi_ = {};

    // saved configParamMap
    std::map<std::string, std::shared_ptr<void>>* configParamMap_ = nullptr;
};
}
#endif
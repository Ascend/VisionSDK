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
 * Description: Target Box Conversion Drawing Unit Plug-in.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MXPLUGINS_MXPIOBJECT2OSDINSTANCES_H
#define MXPLUGINS_MXPIOBJECT2OSDINSTANCES_H

#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/Proto/MxpiOSDType.pb.h"

/**
 * This plugin is used for stream distribute. Temporarily only class id and channel id is supported.
 */
namespace MxPlugins {
class MxpiObject2OsdInstances : public MxTools::MxPluginBase {
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
    * @description: MxpiObject2OsdInstances plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

    /**
    * @description: MxpiObject2OsdInstances plugin define properties.
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
    APP_ERROR InitAndRefreshProps(bool refreshStage = false);

    APP_ERROR InitTextConfig();

    APP_ERROR CheckMetaData(MxTools::MxpiMetadataManager &manager, MxTools::MxpiObjectList &objectList);

    APP_ERROR CoreProcess(MxTools::MxpiObjectList &objectList, MxTools::MxpiOsdInstancesList &osdInstancesList);

    APP_ERROR CreateObjectRect(MxTools::MxpiOsdInstances &mxpiOsdInstances, MxTools::MxpiObject &object, uint8_t colorB,
                          uint8_t colorG, uint8_t colorR);
    APP_ERROR CreateText(MxTools::MxpiOsdInstances &mxpiOsdInstances,
                         MxTools::MxpiObject &object, const std::string& str);

private:
    std::map<int, std::vector<int>> colorMap_ = {};
    // for text
    int fontFace_ = 0;
    double fontScale_ = 1.0f;
    int fontThickness_ = 1;
    int fontLineType_ = 0;
    bool createText_ = true;

    // for rectangle
    int rectThickness_ = 1;
    int rectLineType_ = 0;

    // saved configParamMap
    std::map<std::string, std::shared_ptr<void>>* configParamMap_ = nullptr;
};
}

#endif
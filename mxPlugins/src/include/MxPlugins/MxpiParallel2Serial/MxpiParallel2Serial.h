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
 * Description: Input data from multiple ports is output sequentially through one port.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPLUGINS_MXPIPARALLEL2SERIAL_H
#define MXPLUGINS_MXPIPARALLEL2SERIAL_H
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"

/**
 * This plugin is used for changing parallel to serial.
 */
namespace MxPlugins {
class MxpiParallel2Serial : public MxTools::MxPluginBase {
public:
    MxpiParallel2Serial();

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
    * @description: MxpiParallel2Serial plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

    /**
    * @description: MxpiParallel2Serial plugin define properties.
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

    std::string Vec2Str(const std::vector<std::string>& strVec);

    APP_ERROR CheckProperty();

    APP_ERROR CommonUse(MxTools::MxpiBuffer& mxpiBuffer);

    APP_ERROR SpecialUse(MxTools::MxpiBuffer& mxpiBuffer, const int idx);

    std::vector<std::string> dataKeyVec_;

    std::string parentName_;

    std::ostringstream errorInfo_;

    int removeParentData_;
};
}

#endif
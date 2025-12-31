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
 * Description: The common function for dtframe test.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MXPLUGINGENERATOR_DEMOB_H
#define MXPLUGINGENERATOR_DEMOB_H

#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxBase/ErrorCode/ErrorCode.h"

/**
 * This demo test the custom property。
 */
namespace MxTools {
class DemoB : public MxTools::MxPluginBase {
public:
    int Init(std::map<std::string, std::shared_ptr<void>>& configParam);

    int DeInit();

    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer) override;

    static std::vector<std::shared_ptr<void>> DefineProperties();

    static MxTools::MxpiPortInfo DefineInputPorts();

    static MxTools::MxpiPortInfo DefineOutputPorts();
};
}  // namespace MxTools
#endif

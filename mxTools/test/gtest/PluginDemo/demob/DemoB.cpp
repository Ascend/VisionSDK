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

#include "DemoB.h"

namespace MxTools {

int DemoB::Init(std::map<std::string, std::shared_ptr<void>>& configParam)
{
    std::cout << "I am in DemoB Init" << std::endl;
    std::shared_ptr<std::string> stringValue = std::static_pointer_cast<std::string>(configParam["string"]);
    std::cout << "param(string) value=" << *stringValue << std::endl;

    return 0;
}

int DemoB::DeInit()
{
    std::cout << "I am in DemoB DeInit" << std::endl;
    return 0;
}

APP_ERROR DemoB::Process(std::vector<MxpiBuffer*>& mxpiBuffer)
{
    std::cout << "I am in DemoB Process" << std::endl;
    MxpiBuffer* buffer = mxpiBuffer[0];
    SendData(0, *buffer);
    return 0;
}

std::vector<std::shared_ptr<void>> DemoB::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    std::shared_ptr<ElementProperty<std::string>> stringProPtr(
        new ElementProperty<std::string> {STRING, "string", "string nick name", "", "haha"});

    properties.push_back(stringProPtr);
    return properties;
}

MxpiPortInfo DemoB::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateDynamicInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxpiPortInfo DemoB::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateDynamicOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

namespace {
MX_PLUGIN_GENERATE(DemoB)
}
}  // namespace MxTools
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

#include "DemoA.h"

#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"

namespace MxTools {

int DemoA::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    std::cout << "I am in DemoA Init" << std::endl;
    std::shared_ptr<std::string> stringValue = std::static_pointer_cast<std::string>(configParamMap["string"]);
    std::cout << "param(string) value=" << *stringValue << std::endl;

    std::shared_ptr<int> intValue = std::static_pointer_cast<int>(configParamMap["int"]);
    std::cout << "param(int) value=" << *intValue << std::endl;

    std::shared_ptr<uint> uintValue = std::static_pointer_cast<uint>(configParamMap["uint"]);
    std::cout << "param(uint) value=" << *uintValue << std::endl;

    std::shared_ptr<long> longValue = std::static_pointer_cast<long>(configParamMap["long"]);
    std::cout << "param(long) value=" << *longValue << std::endl;

    std::shared_ptr<ulong> ulongValue = std::static_pointer_cast<ulong>(configParamMap["ulong"]);
    std::cout << "param(ulong) value=" << *ulongValue << std::endl;

    std::shared_ptr<float> floatValue = std::static_pointer_cast<float>(configParamMap["float"]);
    std::cout << "param(float) value=" << *floatValue << std::endl;

    std::shared_ptr<double> doubleValue = std::static_pointer_cast<double>(configParamMap["double"]);
    std::cout << "param(double) value=" << *doubleValue << std::endl;

    return 0;
}

int DemoA::DeInit()
{
    std::cout << "I am in DemoA DeInit" << std::endl;
    return 0;
}

APP_ERROR DemoA::Process(std::vector<MxpiBuffer*>& mxpiBuffer)
{
    std::cout << "I am in DemoA Process" << std::endl;

    std::string ss = "Hello World";
    InputParam inputParam;
    inputParam.dataSize = ss.size();
    inputParam.ptrData = (void *)ss.c_str();
    inputParam.deviceId = 0;
    MxpiBuffer* tempVuff = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);

    for (unsigned int i = 0; i < mxpiBuffer.size(); ++i) {
        MxpiBuffer* buffer = mxpiBuffer[i];
        if (buffer != nullptr) {
            SendData(0, *tempVuff);
        }
    }

    return 0;
}

MxpiPortInfo DemoA::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxpiPortInfo DemoA::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

std::vector<std::shared_ptr<void>> DemoA::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    std::shared_ptr<ElementProperty<std::string>> stringProPtr(
        new ElementProperty<std::string> {STRING, "string", "string nick name", "this is describe", "defaultValue"});
    std::shared_ptr<ElementProperty<int>> intProPtr(
        new ElementProperty<int> {INT, "int", "int nick name", "int desc", -100, -1000, 1024});
    std::shared_ptr<ElementProperty<uint>> uintProPtr(
        new ElementProperty<uint> {UINT, "uint", "uint nick name", "uint desc", 100, 0, 1024});
    std::shared_ptr<ElementProperty<long>> longProPtr(
        new ElementProperty<long> {LONG, "long", "long nick name", "long desc", -1000, -1000, 10240});
    std::shared_ptr<ElementProperty<ulong>> ulongProPtr(
        new ElementProperty<ulong> {ULONG, "ulong", "ulong nick name", "ulong desc", 1000, 0, 10240});
    std::shared_ptr<ElementProperty<float>> floatProPtr(
        new ElementProperty<float> {FLOAT, "float", "float nick name", "float desc", 11.1, 0.1, 1024.2});
    std::shared_ptr<ElementProperty<double>> doubleProPtr(
        new ElementProperty<double> {DOUBLE, "double", "double nick name", "double desc", 22.1, 0.2, 1024.2});

    properties.push_back(stringProPtr);
    properties.push_back(intProPtr);
    properties.push_back(uintProPtr);
    properties.push_back(longProPtr);
    properties.push_back(ulongProPtr);
    properties.push_back(floatProPtr);
    properties.push_back(doubleProPtr);

    return properties;
}

namespace {
MX_PLUGIN_GENERATE(DemoA)
}
}  // namespace MxTools
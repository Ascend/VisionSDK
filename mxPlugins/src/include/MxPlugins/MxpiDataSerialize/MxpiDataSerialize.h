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
 * Description: Serialize the result into a JSON string.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXBASE_MPDATASERIALIZE_H
#define MXBASE_MPDATASERIALIZE_H

#include <vector>
#include <string>

#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxPlugins {
class MxpiDataSerialize : public MxTools::MxPluginBase {
public:
    /**
    * @api
    * @param configParamMap.
    * @return Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap) override;

    /**
    * @api
    * @return Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @api
    * @param mxpiBuffer.
    * @return Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer) override;

    /**
    * @api
    * @brief Define the parameter of configure properties.
    * @return std::vector<std::shared_ptr<void>>
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
    MxTools::MxpiBuffer *DoSerialize(MxTools::MxpiBuffer &buffer);

    /*
    * @description: add error info to metadata,get by appsink callback
    * @param: MxpiBuffer mxpiBuffer to add metadata,
    * @param: string errorInfo error info
    * @param: APP_ERROR errorCode error code
    */
    APP_ERROR
    AddErrorInfoMetadata(MxTools::MxpiBuffer &mxpiBuffer, std::string errorInfo, const APP_ERROR &errorCode);

    void AddExternalInfoMetadata(MxTools::MxpiBuffer &mxpiBufferOld, MxTools::MxpiBuffer &mxpiBufferNew);

private:
    std::vector<std::string> inputKeys_;
    bool isEraseHeaderVecInfo_ = true;
};
}
#endif

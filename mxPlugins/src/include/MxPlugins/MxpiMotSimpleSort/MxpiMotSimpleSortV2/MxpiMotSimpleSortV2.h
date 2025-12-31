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
 * Description: Implements multi-objective path logging and adjusts the input port of the plug-in.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPLUGINS__MOTSIMPLESORTV2_H
#define MXPLUGINS__MOTSIMPLESORTV2_H

#include "MxPlugins/MxpiMotSimpleSort/MxpiMotSimpleSortBase.h"

class MxpiMotSimpleSortV2 : public MxPlugins::MxpiMotSimpleSortBase {
public:
    /**
     * @api
     * @param configParamMap
     * @return
     */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap);

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

    /**
     * @api
     * @param mxpiBuffer
     * @return
     */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer);

    /**
     * @api
     * @brief Definition the parameter of configure properties.
     * @return std::vector<std::shared_ptr<void>>
     */
    static std::vector<std::shared_ptr<void>> DefineProperties();

private:
    /**
    * @api
    * @brief determine different usage, track with feature or not
    * @return
    */
    APP_ERROR JudgeUsage();

    /**
    * @api
    * @brief Obtains and processes the mxpibuffer data.
    * @return
    */
    APP_ERROR GetDataFromBuffer(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer);

    /**
    * @api
    * @brief process mxpibuffer with error information that received from previous plugin
    * @return
    */
    APP_ERROR ErrorInfoProcess(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer);

    /**
    * @api
    * @brief check data source
    * @return
    */
    APP_ERROR CheckDataSource(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer);

    /**
    * @api
    * @brief get object list
    * @return
    */
    APP_ERROR GetModelInferResult(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
            std::vector<MxPlugins::DetectObject> &detectObjectList);
    /**
    * @api
    * @brief send mxpibuffer with error info
    * @return
    */
    APP_ERROR SendMxpiErrorInfo(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, APP_ERROR errorCode,
            const std::string& errorText);

    /**
    * @api
    * @brief check protobuf type
    * @return
    */
    APP_ERROR CheckProtobufType(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer);
};

#endif

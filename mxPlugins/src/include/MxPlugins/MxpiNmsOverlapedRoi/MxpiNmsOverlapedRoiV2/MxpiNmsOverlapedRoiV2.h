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
 * Description: Used to filter duplicate targets in the overlapping area after partitioning and adjust ports.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef ATLASINDUSTRYSDK_MPNMSOVERLAPEDROIV2_H
#define ATLASINDUSTRYSDK_MPNMSOVERLAPEDROIV2_H

#include "MxPlugins/MxpiNmsOverlapedRoi/MxpiNmsOverlapedRoiBase.h"

namespace MxPlugins {
class MxpiNmsOverlapedRoiV2 : public MxPlugins::MxpiNmsOverlapedRoiBase {
public:

    /**
    * @api
    * @param configParamMap.
    * @return Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>>& configParamMap);

    /**
    * @api
    * @brief Definition the parameter of configure properties.
    * @return Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer);

    /**
    * @api
    * @brief Definition the parameter of configure properties.
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

    /**
    * @api
    * @brief Get block information.
    * @return Error code.
    */
    APP_ERROR GetBlockDataInfo(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer);

    /**
    * @api
    * @brief error information process.
    * @return Error code.
    */
    APP_ERROR ErrorInfoProcess(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer);

    /**
    * @api
    * @brief check data source.
    * @return Error code.
    */
    APP_ERROR CheckDataSource(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer);

    /**
    * @api
    * @brief send mxpibuffer with error information.
    * @return Error code.
    */
    APP_ERROR SendMxpiErrorInfo(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, APP_ERROR errorCode);
};
}

#endif

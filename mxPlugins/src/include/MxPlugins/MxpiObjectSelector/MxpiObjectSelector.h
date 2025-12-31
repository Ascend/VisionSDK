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
 * Description: Used to filter the MxpiObjectList.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MXPLUGINS_MXPIOBJECTSELECTOR_H
#define MXPLUGINS_MXPIOBJECTSELECTOR_H

#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/base/MxPluginBase.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/ErrorCode/ErrorCode.h"
/**
* @api
* @brief Definition of MxpiObjectSelector class.
*/
namespace MxPlugins {
class MxpiObjectSelector : public MxTools::MxPluginBase {
public:
    /**
     * @api
     * @brief Initialize configure parameter.
     * @param configParamMap
     * @return APP_ERROR
     */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>>& configParamMap) override;
    /**
     * @api
     * @brief DeInitialize configure parameter.
     * @return APP_ERROR
     */
    APP_ERROR DeInit() override;
    /**
     * @api
     * @brief Process the data of MxpiBuffer.
     * @param mxpiBuffer
     * @return APP_ERROR
     */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer) override;
    /**
     * @api
     * @brief Definition the parameter of configure properties.
     * @return std::vector<std::shared_ptr<void>>
     */
    static std::vector<std::shared_ptr<void>> DefineProperties();

private:
    APP_ERROR GetData(const std::shared_ptr<void> &metadata);
    APP_ERROR GenerateOutput(const MxTools::MxpiObjectList &srcMxpiObjectList,
        MxTools::MxpiObjectList& dstMxpiObjectList);
    APP_ERROR SelectObject(int topN, int bttomN,
        MxTools::MxpiObjectList &filterObjectList, std::vector<float> &filterObjectVec,
        MxTools::MxpiObjectList &dstMxpiObjectList);
    APP_ERROR CheckParamsInput(int topN, int bottomN, int minArea, int maxArea, std::string type);
    std::ostringstream errorInfo_;
    std::string selectorConfig_;
    int topN_;
    int bottomN_;
    std::string type_;
    int minArea_;
    int maxArea_;
    double confThresh;
};
}

#endif // MXPLUGINS_MXPIOBJECTSELECTOR_H

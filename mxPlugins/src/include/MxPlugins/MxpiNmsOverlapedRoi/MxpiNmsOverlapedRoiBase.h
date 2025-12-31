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

#ifndef ATLASINDUSTRYSDK_MPNMSOVERLAPEDROI_H
#define ATLASINDUSTRYSDK_MPNMSOVERLAPEDROI_H

#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/CV/ObjectDetection/Nms/Nms.h"

namespace MxPlugins {
class MxpiNmsOverlapedRoiBase : public MxTools::MxPluginBase {
public:
    /**
    * @api
    * @param configParamMap.
    * @return Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>>& configParamMap) override;

    /**
    * @api
    * @return Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @api
    * @brief Definition the parameter of configure properties.
    * @return Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer) override;

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

protected:
    void CalcEffectiveArea();
    APP_ERROR GetBlockDataInfo(MxTools::MxpiBuffer &buffer);
    APP_ERROR FilterRepeatObject(std::shared_ptr<MxTools::MxpiObjectList> mxpiObjectList,
                            std::shared_ptr<MxTools::MxpiObjectList> metaDataPtr);
    void ConvertObjectToDetectBox(std::vector<MxBase::DetectBox>& detectBoxes,
                                  std::vector<MxTools::MxpiObject> filterObjectVec);
    APP_ERROR ConvertDetectBoxToObject(const std::vector<MxBase::DetectBox>& detectBoxes,
                                  std::shared_ptr<MxTools::MxpiObjectList> metaDataPtr);
    void CompareBlockObject(std::shared_ptr<MxTools::MxpiObjectList> mxpiObjectList);
    void CalcAreaOffsetX(int i, int j);
    void CalcAreaOffsetY(int i, int j);
    bool IsExceedingEffectiveArea(const MxTools::MxpiObject& object);
    bool IsIntersecting(std::map<int, MxTools::MxpiObject>& mxpiObject, int index);

protected:
    std::ostringstream errorInfo_;         // Error info logger
    std::string previousPluginName_ = "";  // Previous plugin name
    std::string blockPluginName_ = "";     // Block plugin name
    float nmsValue_ = 0.f;     // Block plugin name
    std::map<int, MxTools::MxpiObject> mxpiObjectMap_ = {}; // Block object information
};
}

#endif

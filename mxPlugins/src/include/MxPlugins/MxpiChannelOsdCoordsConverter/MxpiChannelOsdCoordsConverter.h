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
 * Description: Multichannel Coordinate Conversion Plug-in.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MXPLUGINS_MXPICHANNELOSDCOORDSCONVERTER_H
#define MXPLUGINS_MXPICHANNELOSDCOORDSCONVERTER_H

#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/PostProcessBases/PostProcessDataType.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/Proto/MxpiOSDType.pb.h"

/**
 * This plugin is used for stream distribute. Temporarily only class id and channel id is supported.
 */
namespace MxPlugins {
class MxpiChannelOsdCoordsConverter : public MxTools::MxPluginBase {
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
    * @description: MxpiChannelOsdCoordsConverter plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

    /**
    * @description: MxpiChannelOsdCoordsConverter plugin define properties.
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

    bool IsValidChannelIds(const std::vector<int>& channelIds);

    APP_ERROR ErrorProcessing(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer, APP_ERROR ret);

    APP_ERROR CheckMetaData(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer, std::vector<int>& validBufferPorts);

    APP_ERROR GetImagePreProcessInfo(std::shared_ptr<MxTools::MxpiVisionList> visionList,
        std::map<int, MxBase::ImagePreProcessInfo> &imagePreProcessInfos);

    APP_ERROR ConstructInputData(std::vector<MxTools::MxpiBuffer*> &mxpiBuffers,
        std::vector<int>& validBufferPorts, std::map<int, MxBase::ImagePreProcessInfo> &imagePreProcessInfos,
        std::map<int, MxTools::MxpiOsdInstancesList> &inputOsdInstancesLists);

    APP_ERROR ConstrutImagePreProcessInfos(MxTools::MxpiBuffer &mxpiBuffer,
        std::map<int, MxBase::ImagePreProcessInfo> &imagePreProcessInfos);

    APP_ERROR CoreProcess(std::map<int, MxBase::ImagePreProcessInfo> &imagePreProcessInfos,
        std::map<int, MxTools::MxpiOsdInstancesList> &osdInstancesList,
        MxTools::MxpiOsdInstancesList &outputOsdInstancesList);

    APP_ERROR ConvertRectCoords(MxTools::MxpiOsdInstances& output,
        MxTools::MxpiOsdInstances &oldInstances, MxBase::ImagePreProcessInfo &preInfo);

    APP_ERROR ConvertTextCoords(MxTools::MxpiOsdInstances& output,
        MxTools::MxpiOsdInstances &oldInstances, MxBase::ImagePreProcessInfo &preInfo);

    APP_ERROR ConvertCircleCoords(MxTools::MxpiOsdInstances& output,
        MxTools::MxpiOsdInstances &oldInstances, MxBase::ImagePreProcessInfo &preInfo);

    APP_ERROR ConvertLineCoords(MxTools::MxpiOsdInstances& output,
        MxTools::MxpiOsdInstances &oldInstances, MxBase::ImagePreProcessInfo &preInfo);
private:
    std::string dataSourceStitchInfo_ = "";
    std::vector<std::string> dataSourceOsds_ = {};
    // channel ids string
    std::string channelIdsString_;
    // channel ids
    std::vector<int> channelIds_;
    bool enableFixedArea_ = false;

    // saved configParamMap
    std::map<std::string, std::shared_ptr<void>>* configParamMap_ = nullptr;
};
}

#endif
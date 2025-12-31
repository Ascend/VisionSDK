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
 * Description: Combine multiple images into a large image.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPLUGINS_MXPICHANNELIMAGESSTITCHER_H
#define MXPLUGINS_MXPICHANNELIMAGESSTITCHER_H

#include "MxBase/Log/Log.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/base/MxPluginBase.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxPlugins {
class MxpiChannelImagesStitcher : public MxTools::MxPluginBase {
public:
    /**
    * @api
    * @brief Initialize configure parameter.
    * @param configParamMap.
    * @return Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>>& configParamMap) override;

    /**
    * @api
    * @brief DeInitialize configure parameter.
    * @return Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @api
    * @brief Process the data of MxpiBuffer.
    * @param mxpiBuffer.
    * @return Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer) override;

    /**
    * @api
    * @brief Definition the parameter of configure properties.
    * @return std::vector<std::shared_ptr<void>>.
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
    struct StitchingArrangement {
        int directionNum = 0;
        long minArea = 0;
        long minAreaWidth = 0;
        long minAreaHeight = 0;
    };

    APP_ERROR InitConfig(std::map<std::string, std::shared_ptr<void>>& configParamMap);

    bool IsValidChannelIds(const std::vector<int>& channelIds);

    APP_ERROR InitDvppWrapper();

    APP_ERROR MallocBackgroundMemory();

    APP_ERROR RefreshChannelIds();

    APP_ERROR CheckAvailableMetaData(std::vector<MxTools::MxpiBuffer *>& mxpiBuffer,
                                     std::map<int, MxTools::MxpiVisionList>& availableVisionLists);

    APP_ERROR CheckImageHW(std::map<int, MxTools::MxpiVisionList>& availableVisionLists, bool& refreshFlag);

    APP_ERROR ConstructPasteConifgs(float bufAspectRatio);

    void UpdateStitchingCoordinates(StitchingArrangement& xArea, StitchingArrangement& yArea);

    void UpdateStitchingCoordinatesYArea(StitchingArrangement& yArea);

    void CalculateBestArrangement(float bufAspectRatio, StitchingArrangement &xArea, StitchingArrangement &yArea);

    APP_ERROR CoreProcess(std::map<int, MxTools::MxpiVisionList>& availableVisionLists,
                          std::shared_ptr<MxTools::MxpiVisionList> &dstMxpiVisionList,
                          std::shared_ptr<MxTools::MxpiVisionList> &shiftInfoList);

    APP_ERROR SetOutputBackground(MxBase::MemoryData& data);

    APP_ERROR VpcStitching(std::map<int, MxTools::MxpiVisionList> &availableVisionLists,
                           std::shared_ptr<MxTools::MxpiVisionList> &shiftInfoList,
                           MxBase::DvppDataInfo &stitcherOutputDataInfo);

    APP_ERROR SendDataAndClearResources(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
                                        std::shared_ptr<MxTools::MxpiVisionList> &dstMxpiVisionList,
                                        std::shared_ptr<MxTools::MxpiVisionList> &shiftInfoList);

    void DestroyBufferAndSendErrorInfo(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, APP_ERROR errorCode,
        std::ostringstream &errorInfo);

    bool IsRefreshBackground(int width, int height, std::string background);

private:
    std::shared_ptr<MxBase::DvppWrapper> dvppWrapper_ = nullptr;
    std::vector<std::string> dataSources_ = {};
    // channel ids string
    std::string channelIdsString_;
    // channel ids
    std::vector<int> channelIds_;
    int outputWidth_ = 0;
    int outputHeight_ = 0;
    std::string background_;

    // saved configparammap
    std::map<std::string, std::shared_ptr<void>> *configParamMap_ = nullptr;

    // saved configparammap
    MxBase::MemoryData backgroundData_;

    // last updated input image aspect ratio
    std::map<int, MxBase::CropRoiConfig> stitchingCoordinates_;

    // last updated input image aspect ratio
    float bufAspectRatio_ = 0.0;
};
}
#endif // MXPLUGINS_MXPICHANNELIMAGESSTITCHER_H


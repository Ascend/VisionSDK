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
 * Description: Plugin for stitching the semantic seg.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#ifndef MX_MXPISEMANTICSEGSTITCHER_H
#define MX_MXPISEMANTICSEGSTITCHER_H

#include <opencv2/opencv.hpp>
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxBase/PostProcessBases/PostProcessDataType.h"

/**
 * This plugin is used for stitching the semantic seg.
 */
namespace MxPlugins {
using namespace MxBase;
using namespace MxTools;
class MxpiSemanticSegStitcher : public MxTools::MxPluginBase {
public:
    /**
    * @description: Init configs.
    * @param configParamMap: config.
    * @return: Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap) override;

    /**
    * @description: DeInit.
    * @return: Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @description: MxpiSemanticSegStitcher plugin define properties.
    * @return: properties.
    */
    static std::vector<std::shared_ptr<void>> DefineProperties();

    /**
    * @description: MxpiSemanticSegStitcher plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer) override;

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
    void GetRelativeRoiInfo(const std::shared_ptr<MxpiObjectList> &cropRoiList,
                            const std::shared_ptr<MxpiObjectList> &mergeRoiList,
                            std::vector<CropRoiBox> &relativeRoiInfo);

    APP_ERROR GetBaseInfo(std::vector<MxpiBuffer*> &mxpiBuffer, std::shared_ptr<MxpiObjectList> &mergeRoiList,
                          std::shared_ptr<MxpiObjectList> &cropRoiList,
                          std::shared_ptr<MxpiImageMaskList> &srcImageMaskList);

    APP_ERROR GetImageSize(MxpiBuffer& buffer);

    APP_ERROR GetOutputProtoMetadata(std::shared_ptr<MxpiImageMaskList> &dstImageMaskList, const cv::Mat &dstImage,
                                const std::shared_ptr<MxpiImageMaskList> &srcImageMaskList);

    APP_ERROR CheckProtobufName(const std::shared_ptr<void> &metadataPtr, const std::string &dataSourceName,
                                std::string &protobufName);

    APP_ERROR GetMergeImage(cv::Mat &dstImage, std::shared_ptr<MxpiImageMaskList> &srcImageMaskList,
                            const std::shared_ptr<MxpiObjectList> &mergeRoiList,
                            std::vector<CropRoiBox> &relativeRoiInfo) const;

    APP_ERROR CheckMetadata(std::shared_ptr<MxpiImageMaskList> &srcImageMaskList,
                            std::shared_ptr<MxpiObjectList> &cropRoiList,
                            std::shared_ptr<MxpiObjectList> &mergeRoiList);

    void DestroyBufferAndSendErrorInfo(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, APP_ERROR errorCode);
    APP_ERROR CheckSemanticSegStitcherMxpiBufferIsValid(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer);

private:
    std::string dataSourceSemanticSeg_ = "";
    std::string dataSourceMergeRoi_ = "";
    std::string dataSourceCropRoi_ = "";
    int imageHeight_ = 0;
    int imageWidth_ = 0;
    std::ostringstream errorInfo_;
};
}

#endif // MX_MXPISEMANTICSEGSTITCHER_H

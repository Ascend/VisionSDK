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
 * Description: Image cropping based on object detection.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXBASE_MPDATASERIALIZE_H
#define MXBASE_MPDATASERIALIZE_H

#include <opencv2/opencv.hpp>
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxPlugins {
 /* namespace MxPlugins */
class MxpiImageCrop : public MxTools::MxPluginBase {
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
    * @param mxpiBuffer.
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

private:
    APP_ERROR JudgeMetadataExists(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer);

    APP_ERROR PrepareCropInput(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer,
        std::vector<MxBase::DvppDataInfo>& inputDataInfoVec);

    APP_ERROR GetInputDataInfoFromDynamicPad(MxTools::MxpiBuffer& buffer,
        std::vector<MxBase::DvppDataInfo>& inputDataInfoVec);

    APP_ERROR GetInputDataInfoFromStaticPad(MxTools::MxpiBuffer& buffer, MxBase::DvppDataInfo& inputDataInfo);

    void ConvertMxpiVisionToDvppDataInfo(const MxTools::MxpiVision& MxpiVision, MxBase::DvppDataInfo& inputDataInfo);

    void SetResizeConfig(MxBase::ResizeConfig& resizeConfig, uint32_t cropHeight, uint32_t cropWidth);

    void PrepareCropOutput(MxTools::MxpiMetadataManager& mxpiMetadataManager,
        std::vector<MxBase::DvppDataInfo>& outputDataInfoVec, std::vector<MxBase::CropRoiConfig>& cropConfigVec,
        std::vector<MxBase::ResizeConfig>& resizeConfigVec, std::vector<uint32_t>& validObjectIds);

    APP_ERROR WriteCropResult(std::shared_ptr<MxTools::MxpiVisionList>& mxpiVisionList,
        std::vector<MxBase::DvppDataInfo>& outputDataInfoVec, const std::vector<uint32_t>& validObjectIds,
        MxTools::MxpiMemoryType mxpiMemoryType);

    // check input image info(type, width/height, datasize) and crop area info
    APP_ERROR CheckInputImageAndCropAreaInfo(const MxBase::DvppDataInfo& inputDataInfo,
        const std::vector<MxBase::CropRoiConfig>& cropConfigVec);

    APP_ERROR CalcCropImageInfo(std::vector<MxBase::CropRoiConfig>& cropConfigVec, MxBase::DvppDataInfo& inputDataInfo,
        std::vector<MxBase::DvppDataInfo>& outputDataInfoVec);
    APP_ERROR GetPasteAreaPosition(MxBase::CropRoiConfig& pasteConfig, const MxBase::CropRoiConfig& item,
        MxBase::DvppDataInfo& data, float& keepAspectRatioScaling);

    APP_ERROR SetOutputImageInfo(MxBase::DvppDataInfo& outputDataInfo);

    APP_ERROR DoCropImage(MxBase::DvppDataInfo& inputDataInfo, std::vector<MxBase::ResizeConfig>& resizeConfigVec,
        std::vector<MxBase::DvppDataInfo>& outputDataInfoVec, std::vector<MxBase::CropRoiConfig>& cropConfigVec);

    APP_ERROR OpencvCropImage(std::vector<MxBase::DvppDataInfo>& inputDataInfoVec,
        std::vector<MxBase::DvppDataInfo>& outputDataInfoVec, std::vector<MxBase::CropRoiConfig>& cropConfigVec);

    APP_ERROR OpencvProcess(std::vector<MxBase::DvppDataInfo>& inputDataInfoVec,
        std::vector<MxBase::DvppDataInfo>& outputDataInfoVec,
        std::vector<MxBase::CropRoiConfig>& cropConfigVec, MxTools::MxpiBuffer& buffer);

    APP_ERROR OpencvCropProcess(std::vector<MxBase::DvppDataInfo>& outputDataInfoVec,
        std::vector<MxBase::CropRoiConfig>& cropConfigVec, MxBase::DvppDataInfo& inputDataInfo);

    // check image is existed
    APP_ERROR CheckImageIsExisted(MxTools::MxpiBuffer& mxpiBuffer, MxTools::MxpiFrame& frameData);

    APP_ERROR CalcTFPasteArea(MxBase::CropRoiConfig& pasteConfig, const MxBase::CropRoiConfig& item,
        MxBase::DvppDataInfo& data, float& keepAspectRatioScaling);

    APP_ERROR CalcMSPasteArea(MxBase::CropRoiConfig& pasteConfig, const MxBase::CropRoiConfig& item,
        MxBase::DvppDataInfo& data, float& keepAspectRatioScaling);

    APP_ERROR CheckCropConfig(const std::vector<MxBase::CropRoiConfig>& cropConfigVec,
        const MxBase::DvppDataInfo& inputDataInfo);

    bool IsNeedResize();

    bool CheckPasteAreaPosition(const MxBase::CropRoiConfig& config, uint32_t width, uint32_t height);

    void CalcPasteAreaPosition(MxBase::CropRoiConfig& pasteConfig);

    void CalcPaddingPasteArea(MxBase::CropRoiConfig& pasteConfig, const MxBase::CropRoiConfig& item,
        MxBase::DvppDataInfo& data, float& keepAspectRatioScaling);

    APP_ERROR InitConfig(std::map<std::string, std::shared_ptr<void>>& configParamMap);

    // to fix codes exceed 50 lines
    APP_ERROR InitProperties(std::map<std::string, std::shared_ptr<void>>& configParamMap);

    void PrintHeightErrorInfo(uint32_t height, unsigned int checkNum, MxBase::CropRoiConfig& cropConfig);

    void PrintWidthErrorInfo(int i, uint32_t width, unsigned int checkNum, MxBase::CropRoiConfig& cropConfig);

    APP_ERROR SetCropOutputConfig(MxTools::MxpiObjectList &mxpiObjectListTemp);

    APP_ERROR ProcessWriteAndSendData(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer,
        std::vector<MxBase::DvppDataInfo>& inputDataInfoVec);

    APP_ERROR DoOpencvCrop(const MxBase::DvppDataInfo& inputDataInfo, const MxBase::CropRoiConfig& cropConfig,
        MxBase::DvppDataInfo& outputDataInfo, cv::Mat& imageRGB) const;

    static void SetProperties(std::vector<std::shared_ptr<void>>& properties);
    bool CheckCropRoiConfig(const MxBase::CropRoiConfig& cropConfig, const MxBase::DvppDataInfo& inputDataInfo) const;
    bool CheckMemoryConfig(MxTools::MxpiMemoryType type);
    void DestoryMemory(std::vector<MxBase::DvppDataInfo>& outputDataInfoVec);

    APP_ERROR IsCropInputValid(int type, std::vector<float>& pointX0, std::vector<float>& pointY0,
        std::vector<float>& pointX1, std::vector<float>& pointY1);

private:
    MxBase::DvppWrapper dvppWrapper_;
    std::string prePluginName_;
    std::string resizeDataSource_;
    uint32_t rawHeight_ = 0;
    uint32_t rawWidth_ = 0;
    uint32_t resizeHeight_ = 0;
    uint32_t resizeWidth_ = 0;
    uint32_t cropHeight_ = 0;
    uint32_t cropWidth_ = 0;
    float leftExpandRatio_ = 0;
    float rightExpandRatio_ = 0;
    float upExpandRatio_ = 0;
    float downExpandRatio_ = 0;
    std::ostringstream errorInfo_;
    uint32_t minDimension_;
    uint32_t maxDimension_;
    uint32_t resizeType_;
    uint32_t paddingType_;
    std::string cropType_;
    std::vector<float> keepAspectRatioScalingVec_;
    std::string background_;
    std::string handleMethod_;
    MxTools::MxpiVisionInfo srcMxpiVisionInfo_;
    std::vector<MxBase::CropResizePasteConfig> cropResizePasteConfigs_ = {};
    int autoDetectFrame_;
    std::string cropPointx0_;
    std::string cropPointx1_;
    std::string cropPointy0_;
    std::string cropPointy1_;
    enum detect_frame {
        USER_INPUT,
        AUTO_INPUT
    };
};
}

#endif

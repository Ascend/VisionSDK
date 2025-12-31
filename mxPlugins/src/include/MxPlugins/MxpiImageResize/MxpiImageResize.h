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
 * Description: Used to complete the zooming of the picture.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXPLUGINS_MXPIVPCRESIZE_H
#define MXPLUGINS_MXPIVPCRESIZE_H

#include <opencv2/opencv.hpp>
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/base/MxPluginBase.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"

namespace MxPlugins {
class MxpiImageResize : public MxTools::MxPluginBase {
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
    * @brief Resize the format of YUV image.
    * @param buffer.
    * @return Error code.
    */
    APP_ERROR ImageResize(MxTools::MxpiBuffer& buffer);
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
    APP_ERROR DvppResize(MxTools::MxpiVisionInfo& dstMxpiVisionInfo, MxTools::MxpiVisionData& dstMxpiVisionData,
                         MxTools::MxpiVisionInfo& srcMxpiVisionInfo, MxTools::MxpiVisionData& srcMxpiVisionData);
    APP_ERROR OpencvResize(MxTools::MxpiVisionInfo& dstMxpiVisionInfo, MxTools::MxpiVisionData& dstMxpiVisionData,
                           MxTools::MxpiVisionInfo& srcMxpiVisionInfo, MxTools::MxpiVisionData& srcMxpiVisionData);
    APP_ERROR OpencvPaddingProcess(cv::Mat& imageRGB, MxTools::MxpiVisionInfo &srcMxpiVisionInfo);
    APP_ERROR CheckOutputImage(uint32_t width, uint32_t height);

    void SetResizeConfig(MxTools::MxpiVisionInfo& srcMxpiVisionInfo);
    void CalcMSPasteArea(MxBase::CropRoiConfig& pasteConfig, uint32_t& dataSize);
    void CalcTFPasteArea(MxBase::CropRoiConfig& pasteConfig, uint32_t& dataSize);
    void CalcPaddingPasteArea(MxBase::CropRoiConfig& pasteConfig, uint32_t& dataSize);
    APP_ERROR KeepAspectRatioResizer();
    APP_ERROR SetDvppOutputDataInfo(MxBase::MemoryData &data, uint32_t dataSize);
    void SetMxVisionInfo(MxTools::MxpiVisionInfo& dstMxpiVisionInfo, MxTools::MxpiVisionData& dstMxpiVisionData,
                         MxTools::MxpiVisionInfo& srcMxpiVisionInfo, MxTools::MxpiVisionData& srcMxpiVisionData);
    APP_ERROR InitDvppWrapper();
    APP_ERROR InitConfig(std::map<std::string, std::shared_ptr<void>>& configParamMap);
    bool CheckPasteAreaPosition(const MxBase::CropRoiConfig& config);
    void CalcPasteAreaPosition(MxBase::CropRoiConfig& pasteConfig);
    void CalcPasteArea(MxBase::CropRoiConfig& pasteConfig, uint32_t& dataSize);
    void GetScalarValue(MxTools::MxpiVisionInfo &srcMxpiVisionInfo, cv::Scalar& value);
    float OpencvRescaleProcess(cv::Mat& resizeRGB, cv::Mat& imageRGB, uint32_t resizeDstWidth,
                               uint32_t resizeDstHeight);
    void OpencvRescaleDoubleProcess(cv::Mat& resizeRGB, cv::Mat& imageRGB, uint32_t resizeDstWidth,
                                    uint32_t resizeDstHeight);
    void OpencvResizeProcess(cv::Mat& resizeRGB, cv::Mat& imageRGB,
        MxTools::MxpiVisionInfo& srcMxpiVisionInfo);
    APP_ERROR CheckOpencvParam(MxTools::MxpiVisionInfo &srcMxpiVisionInfo,
        MxTools::MxpiVisionData &srcMxpiVisionData);
    APP_ERROR SetOutputData(MxTools::MxpiVisionInfo &dstMxpiVisionInfo, MxTools::MxpiVisionData &dstMxpiVisionData,
        MxTools::MxpiVisionInfo &srcMxpiVisionInfo, MxTools::MxpiVisionData &srcMxpiVisionData, cv::Mat& resizeRGB);
    static void SetProperties(std::vector<std::shared_ptr<void>>& properties);
    void OpencvYolov4Process(cv::Mat& resizeRGB, cv::Mat& imageRGB);
    void OpencvPaddleOCR(cv::Mat& resizeRGB, cv::Mat& imageRGB);
    bool CheckOpencvResizeMode();
    void OpencvKeepAspectRatioFit(cv::Mat& resizeRGB, const cv::Mat& imageRGB);
    void OpencvKeepAspectRatioFastRcnn(cv::Mat& resizeRGB, const cv::Mat& imageRGB);
    void CalcFastRcnnHW(const uint32_t inputWidth, const uint32_t inputHeight, uint32_t& outputWidth,
        uint32_t& outputHeight);
    APP_ERROR MxpiVisionPreProcess(const std::shared_ptr<MxTools::MxpiVisionList> &srcMxpiVisionListSptr,
        std::shared_ptr<MxTools::MxpiVisionList> &dstMxpiVisionListSptr);
    void SetdvppInputDataInfo(MxTools::MxpiVisionInfo& srcMxpiVisionInfo);
    APP_ERROR DvppResizeAndCrop(MxBase::CropRoiConfig& pasteConfig);
    void GetResizedOriginalSize(uint32_t &resizedOriginalWidth, uint32_t &resizedOriginalHeight) const;

private:
    MxBase::DvppDataInfo dvppInputDataInfo_;
    MxBase::DvppDataInfo dvppOutputDataInfo_;
    MxBase::ResizeConfig resizeConfig_;
    std::shared_ptr<MxBase::DvppWrapper> dvppWrapper_;
    int removeParentData_;
    uint32_t resizeHeight_;
    uint32_t resizeWidth_;
    uint32_t paddingHeight_;
    uint32_t paddingWidth_;
    uint32_t minDimension_;
    uint32_t maxDimension_;
    uint32_t resizeType_;
    uint32_t paddingType_;
    float paddingColorB_;
    float paddingColorG_;
    float paddingColorR_;
    float keepAspectRatioScaling_;
    std::string background_;
    std::string handleMethod_;
    uint32_t scaleValue_;
    std::ostringstream errorInfo_;
    bool isDynamicImageSizes_;
    MxBase::CropResizePasteConfig cropResizePasteConfig_;
};
}
#endif // MXPLUGINS_VPCRESIZE_H

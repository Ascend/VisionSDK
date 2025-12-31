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
 * Description: Used for image normalization or standardization.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXVISION_MXPIIMAGENORMALIZE_H
#define MXVISION_MXPIIMAGENORMALIZE_H

#include <opencv2/opencv.hpp>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include <MxBase/DvppWrapper/DvppWrapper.h>

/**
 * This plugin is used for image normalize. Temporarily only for host buffer and RGB format.
 */
namespace MxPlugins {
class MxpiImageNormalize : public MxTools::MxPluginBase {
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
    * @description: MxpiImageNormalize plugin define properties.
    * @return: properties.
    */
    static std::vector<std::shared_ptr<void>> DefineProperties();

    /**
    * @description: MxpiImageNormalize plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

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
    APP_ERROR ConvertType(const std::string& type);

    APP_ERROR CheckFormat(const std::string& format);

    APP_ERROR GetAlphaAndBeta(std::string& alpha, std::string& beta);

    APP_ERROR CheckInput(const MxTools::MxpiVision& vision);

    APP_ERROR CheckProcessType(int processType);

    APP_ERROR OpenCVNormalize(size_t idx, MxTools::MxpiVision& srcVision, MxTools::MxpiVision& dstVision);

    APP_ERROR Normalize(cv::Mat &src, cv::Mat &dst, const std::vector<double>& mean, const std::vector<double>& std,
        int type);
    
    APP_ERROR MinMaxNormalize(cv::Mat &src, cv::Mat &dst, int type);

    APP_ERROR DoNormalize(cv::Mat& dst, MxTools::MxpiVision& srcVision);

    void SwitchAlphaAndBeta(const MxTools::MxpiVision& srcVision);

    APP_ERROR ConvertType(const MxTools::MxpiDataType& dataType);

    bool IsNeedConvert(const MxBase::MxbasePixelFormat& format);

    APP_ERROR Mat2MxpiVision(size_t idx, const cv::Mat& mat, MxTools::MxpiVision& vision);

    void SendErrorInfo(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, APP_ERROR errorCode,
        const std::string &errorInfo);

    APP_ERROR IsDescValid(const google::protobuf::Descriptor* desc, std::vector<MxTools::MxpiBuffer *> &mxpiBuffer);

private:
    std::ostringstream errorInfo_;

    std::vector<double> alpha_;

    std::vector<double> beta_;

    std::string outputFormat_;

    int processType_;

    int outputType_;

    int inputType_;

    MxBase::MxbasePixelFormat outputPixelFormat_;

    MxTools::MxpiDataType outputDataType_;
};
}

#endif // MXVISION_MXPIIMAGENORMALIZE_H
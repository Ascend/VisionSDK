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
 * Description: Used to crop the center image.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */
 
#include "MxPlugins/MxpiOpencvCenterCrop/MxpiOpencvCenterCrop.h"
#include "MxBase/Log/Log.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
    const float AVERAGE = 2.f;
}

APP_ERROR MxpiOpencvCenterCrop::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    dataSource_ = MxPluginsAutoDataSource(elementName_, 0, "dataSource", dataSource_, dataSourceKeys_);
    if (dataSource_.empty()) {
        LogError << "Property dataSource is \"\", please check element(" << elementName_ << ")'s previous element."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::vector<std::string> parameterNamesPtr = {"cropHeight", "cropWidth"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }

    cropHeight_ = *std::static_pointer_cast<uint>(configParamMap["cropHeight"]);
    cropWidth_ = *std::static_pointer_cast<uint>(configParamMap["cropWidth"]);
    if (cropHeight_ == 0 || cropWidth_ == 0) {
        LogError << "CropHeight or cropWidth is not set." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    LogInfo << "End to initialize MxpiOpencvCenterCrop(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvCenterCrop::DeInit()
{
    return APP_ERR_OK;
}

void MxpiOpencvCenterCrop::ConvertMxpiVisionToDvppDataInfo(const MxTools::MxpiVision& mxpiVision,
    DvppDataInfo& inputDataInfo)
{
    inputDataInfo.format = (MxBase::MxbasePixelFormat)mxpiVision.visioninfo().format();
    inputDataInfo.width = mxpiVision.visioninfo().width();
    inputDataInfo.height = mxpiVision.visioninfo().height();
    inputDataInfo.widthStride = mxpiVision.visioninfo().widthaligned();
    inputDataInfo.heightStride = mxpiVision.visioninfo().heightaligned();
    inputDataInfo.dataSize = static_cast<uint32_t>(mxpiVision.visiondata().datasize());
    inputDataInfo.data = (uint8_t *)mxpiVision.visiondata().dataptr();
    inputDataInfo.dataType = mxpiVision.visiondata().datatype();
}

APP_ERROR MxpiOpencvCenterCrop::GetInputDataInfoFromStaticPad(MxTools::MxpiBuffer &buffer,
    std::vector<DvppDataInfo>& inputDataInfoVec)
{
    MxTools::MxpiFrame frameData;
    APP_ERROR ret = CheckImageIsExisted(buffer, frameData);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "CheckImageIsExisted failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    for (auto visionvec : frameData.visionlist().visionvec()) {
        if (visionvec.visiondata().memtype() == MxTools::MXPI_MEMORY_DEVICE ||
            visionvec.visiondata().memtype() == MxTools::MXPI_MEMORY_DVPP) {
            errorInfo_ << "Memory type(" << visionvec.visiondata().memtype()<< ") is not host)."
                       << GetErrorInfo(APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH);
            LogError << errorInfo_.str();
            return APP_ERR_MXPLUGINS_MEMEROY_TYPE_MISMATCH;
        }
        DvppDataInfo inputDataInfo;
        ConvertMxpiVisionToDvppDataInfo(visionvec, inputDataInfo);
        inputDataInfoVec.push_back(inputDataInfo);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvCenterCrop::PrepareCropInput(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer,
    std::vector<DvppDataInfo>& inputDataInfoVec)
{
    APP_ERROR ret = GetInputDataInfoFromStaticPad(*mxpiBuffer[0], inputDataInfoVec);
    if (ret != APP_ERR_OK) {
        LogError << "Element(" << elementName_ << ") Get input data from static pad failed." << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "element(" << elementName_ << ") Get input data from static pad.";
    return ret;
}

APP_ERROR MxpiOpencvCenterCrop::WriteCropResult(std::shared_ptr<MxTools::MxpiVisionList> &mxpiVisionList,
    std::vector<DvppDataInfo> &outputDataInfoVec, const std::vector<uint32_t>& validObjectIds,
    MxTools::MxpiMemoryType mxpiMemoryType)
{
    for (uint32_t i = 0; i < outputDataInfoVec.size(); i++) {
        MxTools::MxpiVision *vision = mxpiVisionList->add_visionvec();
        if (CheckPtrIsNullptr(vision, "vision"))  return APP_ERR_COMM_ALLOC_MEM;
        vision->mutable_visioninfo()->set_format(outputDataInfoVec[i].format);
        vision->mutable_visioninfo()->set_width(outputDataInfoVec[i].width);
        vision->mutable_visioninfo()->set_height(outputDataInfoVec[i].height);
        vision->mutable_visioninfo()->set_widthaligned(outputDataInfoVec[i].widthStride);
        vision->mutable_visioninfo()->set_heightaligned(outputDataInfoVec[i].heightStride);
        vision->mutable_visioninfo()->set_keepaspectratioscaling(keepAspectRatioScalingVec_[i]);
        vision->mutable_visiondata()->set_dataptr((uint64_t)outputDataInfoVec[i].data);
        vision->mutable_visiondata()->set_datasize(outputDataInfoVec[i].dataSize);
        vision->mutable_visiondata()->set_deviceid(deviceId_);
        vision->mutable_visiondata()->set_memtype(mxpiMemoryType);
        vision->mutable_visiondata()->set_datatype((MxTools::MxpiDataType)outputDataInfoVec[i].dataType);
        vision->mutable_visiondata()->set_freefunc(0);
        MxTools::MxpiMetaHeader *header = vision->add_headervec();
        if (CheckPtrIsNullptr(header, "header"))  return APP_ERR_COMM_ALLOC_MEM;
        header->set_datasource(dataSource_);
        header->set_memberid(validObjectIds[i]);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvCenterCrop::CheckImageIsExisted(MxTools::MxpiBuffer& mxpiBuffer,
    MxTools::MxpiFrame& mxpiFrame)
{
    // MxpiFrame->MxpiVision
    MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
    auto metadata = mxpiMetadataManager.GetMetadata(dataSource_);
    if (metadata == nullptr) {
        errorInfo_ << "Metadata is null." << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL; // self define the error code
    }
    auto message = (google::protobuf::Message*)metadata.get();
    const google::protobuf::Descriptor* desc = message->GetDescriptor();
    if (!desc) {
        errorInfo_ << "Get input data's descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM; // self define the error code
    }
    if (desc->name() != "MxpiVisionList") {
        errorInfo_ << "Not a MxpiVisionList object." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH; // self define the error code
    }

    auto mxpiVisionListTmp = std::static_pointer_cast<MxpiVisionList>(metadata);
    for (auto visionvec : *mxpiVisionListTmp->mutable_visionvec()) {
        auto mxpiVisionList = mxpiFrame.mutable_visionlist();
        auto mxpiVision = mxpiVisionList->add_visionvec();
        if (CheckPtrIsNullptr(mxpiVision, "mxpiVision"))  return APP_ERR_COMM_ALLOC_MEM;
        auto mxpiVisionInfo = mxpiVision->mutable_visioninfo();
        mxpiVisionInfo->CopyFrom(visionvec.visioninfo());
        auto mxpiVisionData = mxpiVision->mutable_visiondata();
        mxpiVisionData->CopyFrom(visionvec.visiondata());
        if (visionvec.visiondata().datasize() == 0) {
            errorInfo_ << "Get MxpiVisionList failed, datasize(0)." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_FAILURE;
        }
    }

    if (mxpiFrame.visionlist().visionvec_size() == 0) {
        errorInfo_ << "Get MxpiVisionList failed, datasize(0)." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvCenterCrop::OpencvProcess(std::vector<DvppDataInfo>& inputDataInfoVec,
    std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<MxBase::CropRoiConfig> &cropConfigVec,
    MxTools::MxpiBuffer& buffer)
{
    auto ret = OpencvCropCenterProcess(inputDataInfoVec, outputDataInfoVec, cropConfigVec);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Opencv cropping failed." << GetErrorInfo(APP_ERR_DVPP_CROP_FAIL);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(buffer, elementName_, APP_ERR_DVPP_CROP_FAIL, errorInfo_.str());
        return APP_ERR_DVPP_CROP_FAIL;
    }
    MxpiVisionList *mxpiVisionListPtr = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionListPtr == nullptr) {
        for (size_t i = 0; i < outputDataInfoVec.size(); i++) {
            if (outputDataInfoVec[i].data != nullptr) {
                outputDataInfoVec[i].destory(outputDataInfoVec[i].data);
                outputDataInfoVec[i].data = nullptr;
            }
        }
        errorInfo_ << "MxpiVisionList out of memory." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(buffer, elementName_, APP_ERR_COMM_INIT_FAIL, errorInfo_.str());
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::shared_ptr<MxTools::MxpiVisionList> mxpiVisionList(mxpiVisionListPtr, g_deleteFuncMxpiVisionList);
    std::vector<uint32_t> validObjectIds = {};
    for (size_t i = 0; i < outputDataInfoVec.size(); ++i) {
        validObjectIds.push_back(i);
    }
    ret = WriteCropResult(mxpiVisionList, outputDataInfoVec, validObjectIds, MxpiMemoryType::MXPI_MEMORY_HOST_MALLOC);
    if (ret != APP_ERR_OK) {
        for (size_t i = 0; i < outputDataInfoVec.size(); i++) {
            if (outputDataInfoVec[i].data != nullptr) {
                outputDataInfoVec[i].destory(outputDataInfoVec[i].data);
                outputDataInfoVec[i].data = nullptr;
            }
        }
        errorInfo_ << "Add proto metadata failed in Process." << GetErrorInfo(ret);
        SendMxpiErrorInfo(buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    MxTools::MxpiMetadataManager mxpiMetadataManager(buffer);
    ret = mxpiMetadataManager.AddProtoMetadata(elementName_, mxpiVisionList);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add proto metadata failed in Process." << GetErrorInfo(ret);
        SendMxpiErrorInfo(buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    SendData(0, buffer);
    LogDebug << "End to process opencv picture crop(" << elementName_ << ").";
    return ret;
}

APP_ERROR MxpiOpencvCenterCrop::Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiOpencvCenterCrop(" << elementName_ << ").";
    errorInfo_.str("");
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxTools::MxpiBuffer *buffer = mxpiBuffer[0];
    MxTools::MxpiMetadataManager mxpiMetadataManager(*buffer);
    std::vector<DvppDataInfo> inputDataInfoVec;
    ret = PrepareCropInput(mxpiBuffer, inputDataInfoVec);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Crop get input data failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
    }
    std::vector<DvppDataInfo> outputDataInfoVec;
    std::vector<MxBase::CropRoiConfig> cropConfigVec;
    std::vector<ResizeConfig> resizeConfigs;
    std::vector<uint32_t> validObjectIds = {};
    ret = OpencvProcess(inputDataInfoVec, outputDataInfoVec, cropConfigVec, *buffer);
    return ret;
}

APP_ERROR MxpiOpencvCenterCrop::DoOpencvCrop(const DvppDataInfo& inputDataInfo,
    const CropRoiConfig& cropConfig, DvppDataInfo& outputDataInfo, cv::Mat& imageRGB) const
{
    cv::Rect area(cropConfig.x0, cropConfig.y0, cropConfig.x1 - cropConfig.x0, cropConfig.y1 - cropConfig.y0);
    cv::Mat imgCrop = imageRGB(area).clone();
    outputDataInfo.widthStride = static_cast<uint32_t>(imgCrop.cols);
    outputDataInfo.heightStride = static_cast<uint32_t>(imgCrop.rows);
    outputDataInfo.width = static_cast<uint32_t>(imgCrop.cols);
    outputDataInfo.height = static_cast<uint32_t>(imgCrop.rows);
    outputDataInfo.frameId = inputDataInfo.frameId;
    outputDataInfo.format = MXBASE_PIXEL_FORMAT_RGB_888;
    if (inputDataInfo.dataType == MxTools::MXPI_DATA_TYPE_UINT8) {
        outputDataInfo.dataSize = static_cast<uint32_t>(imgCrop.cols) * static_cast<uint32_t>(imgCrop.rows) *
            YUV444_RGB_WIDTH_NU;
    } else if (inputDataInfo.dataType == MxTools::MXPI_DATA_TYPE_FLOAT32) {
        outputDataInfo.dataSize = static_cast<uint32_t>(imgCrop.cols) * static_cast<uint32_t>(imgCrop.rows) *
            YUV444_RGB_WIDTH_NU * sizeof(float);
    }
    outputDataInfo.dataType = inputDataInfo.dataType;
    MemoryData memoryDataDst(outputDataInfo.dataSize, MemoryData::MEMORY_HOST_MALLOC, deviceId_);
    MemoryData memoryDataSrc(imgCrop.data, outputDataInfo.dataSize, MemoryData::MEMORY_HOST_MALLOC);
    APP_ERROR ret = MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
    if (ret != APP_ERR_OK) {
        LogError << "Memory malloc failed." << GetErrorInfo(ret);
        return ret;
    }
    outputDataInfo.destory = [](void* data) {
        free(data);
    };
    outputDataInfo.data = (uint8_t*)memoryDataDst.ptrData;

    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvCenterCrop::IsCropParamCorrect(const CropRoiConfig &cropConfig, const cv::Mat &imageRGB)
{
    if ((uint32_t)imageRGB.cols < cropWidth_ || (uint32_t)imageRGB.rows < cropHeight_) {
        LogError << "Image cols or rows is smaller than cutout size, imageRGB.cols is " << imageRGB.cols
                 << " imageRGB.rows is " << imageRGB.rows << " cropWidth_ is " << cropWidth_ << " cropHeight_ is "
                 << cropHeight_ << " pluginName_:" << pluginName_ << "." << GetErrorInfo(APP_ERR_DVPP_CONFIG_FAIL);
        return APP_ERR_DVPP_CONFIG_FAIL;
    }
    if (cropConfig.x0 > (uint32_t)imageRGB.cols || cropConfig.x1 > (uint32_t)imageRGB.cols ||
        cropConfig.y0 > (uint32_t)imageRGB.rows || cropConfig.y1 > (uint32_t)imageRGB.rows ||
        cropConfig.x1 <= cropConfig.x0 || cropConfig.y1 <= cropConfig.y0) {
        LogError << "Crop params is invalid, imageRGB.cols is " << imageRGB.cols << " imageRGB.rows is "
                 << imageRGB.rows << ", while x0 x1 y0 y1 is " << cropConfig.x0 << " " << cropConfig.x1 << " "
                 << cropConfig.y0 << " " << cropConfig.y1 << "." << GetErrorInfo(APP_ERR_DVPP_CONFIG_FAIL);
        return APP_ERR_DVPP_CONFIG_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiOpencvCenterCrop::OpencvCropCenterProcess(std::vector<DvppDataInfo>& inputDataInfoVec,
    std::vector<DvppDataInfo>& outputDataInfoVec, std::vector<MxBase::CropRoiConfig>&)
{
    keepAspectRatioScalingVec_.resize(inputDataInfoVec.size(), 0.f);
    CropRoiConfig cropConfig;
    DvppDataInfo outputDataInfo;
    for (DvppDataInfo& inputDataInfo : inputDataInfoVec) {
        cv::Mat imageRGB;
        if (inputDataInfo.dataType == MxTools::MXPI_DATA_TYPE_UINT8) {
            imageRGB.create(inputDataInfo.height, inputDataInfo.width, CV_8UC3);
        } else if (inputDataInfo.dataType == MxTools::MXPI_DATA_TYPE_FLOAT32) {
            imageRGB.create(inputDataInfo.height, inputDataInfo.width, CV_32FC3);
        }
        imageRGB.data = (uchar *)inputDataInfo.data;
        cropConfig.x0 = (static_cast<uint32_t>(imageRGB.cols) - cropWidth_) /
            static_cast<uint32_t>(AVERAGE);
        cropConfig.x1 = (static_cast<uint32_t>(imageRGB.cols) - cropWidth_) /
            static_cast<uint32_t>(AVERAGE) + cropWidth_;
        cropConfig.y0 = (static_cast<uint32_t>(imageRGB.rows) - cropHeight_) /
            static_cast<uint32_t>(AVERAGE);
        cropConfig.y1 = (static_cast<uint32_t>(imageRGB.rows) - cropHeight_) /
            static_cast<uint32_t>(AVERAGE) + cropHeight_;
        APP_ERROR ret = IsCropParamCorrect(cropConfig, imageRGB);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        ret = DoOpencvCrop(inputDataInfo, cropConfig, outputDataInfo, imageRGB);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        outputDataInfoVec.push_back(outputDataInfo);
    }

    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiOpencvCenterCrop::DefineProperties()
{
    auto cropHeight = std::make_shared<ElementProperty<uint>>(
        ElementProperty<uint> { UINT, "cropHeight", "height", "the height of crop image", 0, 0, 819200 });
    auto cropWidth = std::make_shared<ElementProperty<uint>>(
        ElementProperty<uint> { UINT, "cropWidth", "width", "the width of resize image", 0, 0, 819200 });
    std::vector<std::shared_ptr<void>> properties = {
        cropHeight, cropWidth
    };
    return properties;
}

MxpiPortInfo MxpiOpencvCenterCrop::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/object", "image/rgb", "image/yuv"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxpiPortInfo MxpiOpencvCenterCrop::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/yuv", "image/rgb"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

namespace {
    MX_PLUGIN_GENERATE(MxpiOpencvCenterCrop)
}
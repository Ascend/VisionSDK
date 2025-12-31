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
 * Description: Obtains the datasource-related attributes of the plug-in.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <map>
#include <cmath>
#include "MxBase/Log/Log.h"
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

namespace {
const int CHANNELS_NUMBER = 3;
const float YUV_Y_R = 0.299;
const float YUV_Y_G = 0.587;
const float YUV_Y_B = 0.114;
const float YUV_U_R = -0.169;
const float YUV_U_G = 0.331;
const float YUV_U_B = 0.500;
const float YUV_V_R = 0.500;
const float YUV_V_G = 0.419;
const float YUV_V_B = 0.081;
const int YUV_DATA_SIZE = 3;
const int YUV_OFFSET = 2;
const int YUV_OFFSET_S = 1;
const int YUV_OFFSET_UV = 128;
const float MAX_YUV_VALUE = 255;
}

namespace MxPlugins {
APP_ERROR TransformVision2TensorPackage(std::shared_ptr<MxTools::MxpiVisionList> foo,
                                        std::shared_ptr<MxTools::MxpiTensorPackageList> tensorPackageList,
                                        MxBase::DataFormat dataFormat)
{
    if (foo == nullptr) {
        LogError << "MxpiVisionList is nullptr! transform failed!" << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    for (int i = 0; i < foo->visionvec_size(); i++) {
        auto tensorPackage = tensorPackageList->add_tensorpackagevec();
        if (CheckPtrIsNullptr(tensorPackage, "tensorPackage"))  return APP_ERR_COMM_ALLOC_MEM;
        auto tensorVec = tensorPackage->add_tensorvec();
        if (CheckPtrIsNullptr(tensorVec, "tensorVec"))  return APP_ERR_COMM_ALLOC_MEM;
        tensorVec->set_tensordataptr(foo->visionvec(i).visiondata().dataptr());
        tensorVec->set_tensordatasize(foo->visionvec(i).visiondata().datasize());
        tensorVec->set_deviceid(foo->visionvec(i).visiondata().deviceid());
        tensorVec->set_memtype(foo->visionvec(i).visiondata().memtype());
        tensorVec->set_datastr(foo->visionvec(i).visiondata().datastr());
        tensorVec->set_freefunc(foo->visionvec(i).visiondata().freefunc());
        auto dataType = foo->visionvec(i).visiondata().datatype();
        if (dataType == MxTools::MXPI_DATA_TYPE_FLOAT32) {
            tensorVec->set_tensordatatype(MxBase::TENSOR_DTYPE_FLOAT32);
        } else if (dataType == MxTools::MXPI_DATA_TYPE_UINT8) {
            tensorVec->set_tensordatatype(MxBase::TENSOR_DTYPE_UINT8);
        }
        tensorVec->add_tensorshape(1);
        if (dataFormat == MxBase::NCHW) {
            tensorVec->add_tensorshape(-1);
            tensorVec->add_tensorshape(foo->visionvec(i).visioninfo().heightaligned());
            tensorVec->add_tensorshape(foo->visionvec(i).visioninfo().widthaligned());
        } else if (dataFormat == MxBase::NHWC) {
            tensorVec->add_tensorshape(foo->visionvec(i).visioninfo().heightaligned());
            tensorVec->add_tensorshape(foo->visionvec(i).visioninfo().widthaligned());
            tensorVec->add_tensorshape(-1);
        }
    }
    return APP_ERR_OK;
}

std::string MxPluginsGetDataSource(const std::string& parentName, const std::string& dataSource,
                                   const std::string& elementName, const std::vector<std::string>& dataSourceKeys)
{
    if (!parentName.empty()) {
        if (dataSource != "auto") {
            LogError << "ParentName and dataSource cannot be set at the same time,dataSource is preferred."
                     << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return "";
        } else {
            return parentName;
        }
    } else {
        if (dataSource != "auto") {
            return dataSource;
        } else if (!dataSourceKeys.empty()) {
            LogInfo << "element(" << elementName << ") not set property dataSource, use default value("
                    << dataSourceKeys[0] << ").";
            return dataSourceKeys[0];
        } else {
            LogError << "Please set dataSource." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return "";
        }
    }
}

std::string MxPluginsAutoDataSource(const std::string& elementName, size_t portId, const std::string& propName,
                                    const std::string& dataSource, const std::vector<std::string>& dataSourceKeys)
{
    if ((portId + 1) > dataSourceKeys.size()) {
        LogError << "The value of portId(" << portId << ") must be in the range [0," << (dataSourceKeys.size() - 1)
                 << "]" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return "";
    }
    if (dataSource == "auto") {
        LogInfo << "element(" << elementName << ") not set property " << propName << ", use auto value("
                << dataSourceKeys[portId] << ").";
        return dataSourceKeys[portId];
    } else {
        return dataSource;
    }
}

APP_ERROR SetImageUVValue(float& yuvU, float& yuvV, const MxBase::DvppDataInfo& dataInfo,
                          std::vector<std::string> vec)
{
    if (vec.size() < CHANNELS_NUMBER) {
        LogError << "Incorrect dimension information." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (dataInfo.format == MxBase::MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420) {
        yuvU = YUV_U_R * std::stof(vec[0].c_str()) - YUV_U_G * std::stof(vec[YUV_OFFSET_S].c_str()) +
               YUV_U_B * std::stof(vec[YUV_OFFSET].c_str()) + YUV_OFFSET_UV;
        yuvV = YUV_V_R * std::stof(vec[0].c_str()) - YUV_V_G * std::stof(vec[YUV_OFFSET_S].c_str()) -
               YUV_V_B * std::stof(vec[YUV_OFFSET].c_str()) + YUV_OFFSET_UV;
    } else if (dataInfo.format == MxBase::MXBASE_PIXEL_FORMAT_YVU_SEMIPLANAR_420) {
        yuvU = YUV_V_R * std::stof(vec[0].c_str()) - YUV_V_G * std::stof(vec[YUV_OFFSET_S].c_str()) -
               YUV_V_B * std::stof(vec[YUV_OFFSET].c_str()) + YUV_OFFSET_UV;
        yuvV = YUV_U_R * std::stof(vec[0].c_str()) - YUV_U_G * std::stof(vec[YUV_OFFSET_S].c_str()) +
               YUV_U_B * std::stof(vec[YUV_OFFSET].c_str()) + YUV_OFFSET_UV;
    } else {
        LogError << "Format[" << dataInfo.format << "] for VPC is not supported, just support NV12 or NV21."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

bool CheckParameterIsOk(std::vector<std::string> dtrVec)
{
    if (dtrVec.size() < CHANNELS_NUMBER) {
        LogError << "Incorrect dimension information." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    try {
        if (std::stof(dtrVec[0].c_str()) > MAX_YUV_VALUE || std::stof(dtrVec[0].c_str()) < 0) {
            return false;
        }
        if (std::stof(dtrVec[YUV_OFFSET_S].c_str()) > MAX_YUV_VALUE || std::stof(dtrVec[YUV_OFFSET_S].c_str()) < 0) {
            return false;
        }
        if (std::stof(dtrVec[YUV_OFFSET].c_str()) > MAX_YUV_VALUE || std::stof(dtrVec[YUV_OFFSET].c_str()) < 0) {
            return false;
        }
    } catch(const std::exception& ex) {
        LogError << "RGBValue cast to float failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return false;
    }
    return true;
}

APP_ERROR SetImageBackground(MxBase::MemoryData& data, const MxBase::DvppDataInfo& dataInfo, const std::string& strRGB)
{
    auto vec = MxBase::StringUtils::Split(strRGB, ',');
    if (vec.size() != YUV_DATA_SIZE) {
        LogError << "Incorrect background color data[" << strRGB
                 <<"]. Please set this parameter in the format of 'R,G,B'." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (!CheckParameterIsOk(vec)) {
        LogError << "The elements in parameter RGBValue must be in range: 0 - 255."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    auto dataPtr = data.ptrData;
    float yuvY = YUV_Y_R * std::stof(vec[0].c_str()) + YUV_Y_G * std::stof(vec[YUV_OFFSET_S].c_str()) +
                 YUV_Y_B * std::stof(vec[YUV_OFFSET].c_str());
    float yuvU = 0;
    float yuvV = 0;
    APP_ERROR ret = SetImageUVValue(yuvU, yuvV, dataInfo, vec);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = MxBase::MemoryHelper::MxbsMemset(data, static_cast<int>(yuvY), data.size);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to memset dvpp memory." << GetErrorInfo(ret);
        return ret;
    }
    int offsetSize = dataInfo.widthStride * dataInfo.heightStride / YUV_OFFSET;
    data.ptrData = (uint8_t *)data.ptrData + dataInfo.widthStride * dataInfo.heightStride;
    ret = MxBase::MemoryHelper::MxbsMemset(data,
        static_cast<int>(yuvU), static_cast<size_t>(offsetSize));
    if (ret != APP_ERR_OK) {
        LogError << "Fail to memset dvpp memory." << GetErrorInfo(ret);
        data.ptrData = dataPtr;
        return ret;
    }
    data.ptrData = (uint8_t *)data.ptrData + YUV_OFFSET_S;
    for (int i = 0; i < offsetSize / YUV_OFFSET; i++) {
        ret = MxBase::MemoryHelper::MxbsMemset(data,
            static_cast<int>(yuvV), static_cast<size_t>(YUV_OFFSET_S));
        if (ret != APP_ERR_OK) {
            LogError << "Fail to memset dvpp memory." << GetErrorInfo(ret);
            data.ptrData = dataPtr;
            return ret;
        }
        data.ptrData = (uint8_t *)data.ptrData + YUV_OFFSET;
    }
    data.ptrData = dataPtr;
    return APP_ERR_OK;
}

int SelectMinAreaShape(std::vector<MxTools::ImageSize>& imageSizes, MxTools::ImageSize& image)
{
    size_t area = static_cast<size_t>(10e15);
    int index = -1;
    for (size_t i = 0; i < imageSizes.size(); i++) {
        if (imageSizes[i].height >= image.height && imageSizes[i].width >= image.width) {
            if (imageSizes[i].area < area) {
                area = imageSizes[i].area;
                index = (int)i;
            }
        }
    }
    return index;
}

int SelectMinScaleShape(std::vector<MxTools::ImageSize>& imageSizes, MxTools::ImageSize& image)
{
    float maxScale = 0;
    int tmpIndex = -1;
    // get min scale index
    for (size_t i = 0; i < imageSizes.size(); i++) {
        float scale = (imageSizes[i].height / float(image.height)) < (imageSizes[i].width / float(image.width)) ?
                      (imageSizes[i].height / float(image.height)) : (imageSizes[i].width / float(image.width));
        if (scale > maxScale) {
            maxScale = scale;
            tmpIndex = (int)i;
        }
    }
    // get min area index from same scale
    int targetIndex = tmpIndex;
    if (tmpIndex + 1 > static_cast<int>(imageSizes.size())) {
        return -1;
    }
    size_t minArea = imageSizes[tmpIndex].area;
    for (size_t i = 0; i < imageSizes.size(); i++) {
        if ((int)i == tmpIndex) {
            continue;
        }
        float scale = (imageSizes[i].height / float(image.height)) < (imageSizes[i].width / float(image.width)) ?
                      (imageSizes[i].height / float(image.height)) : (imageSizes[i].width / float(image.width));
        if (fabs(scale - maxScale) < 1e-15 && imageSizes[i].area < minArea) {
            minArea = imageSizes[i].area;
            targetIndex = (int)i;
        }
    }
    return targetIndex;
}

int SelectTargetShape(std::vector<MxTools::ImageSize>& imageSizes, size_t height, size_t width) // ImageCrop调用点已校验
{
    MxTools::ImageSize imageInfo(height, width);
    int index = SelectMinAreaShape(imageSizes, imageInfo);
    if (index != -1) {
        return index;
    }
    index = SelectMinScaleShape(imageSizes, imageInfo);
    return index;
}

void DestroyBuffers(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, size_t exceptPort)
{
    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        if (i == exceptPort) {
            continue;
        }
        if (mxpiBuffer[i] != nullptr) {
            MxTools::MxpiBufferManager::DestroyBuffer(mxpiBuffer[i]);
            mxpiBuffer[i] = nullptr;
        }
    }
}

APP_ERROR CheckMxpiBufferIsValid(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    if (mxpiBuffer.size() < 1) {
        LogError << "Invalid mxpiBuffer, size must not be equal to 0!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        if (mxpiBuffer[i] == nullptr) {
            DestroyBuffers(mxpiBuffer, 0);
            LogError << "The mxpiBuffer contains nullptr, please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR CheckConfigParamMapIsValid(std::vector<std::string> &parameterNames,
                                     std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    for (std::string parameterName: parameterNames) {
        if (configParamMap.find(parameterName) == configParamMap.end()) {
            LogError << "The parameter: " << parameterName << "is not defined."
                << GetErrorInfo(APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR);
            return APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR;
        } else if (configParamMap[parameterName] == nullptr) {
            LogError << "The parameter: " << parameterName << "is a nullptr"
                     << GetErrorInfo(APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR);
            return APP_ERR_PIPELINE_PROPERTY_CONFIG_ERROR;
        }
    }
    return APP_ERR_OK;
}

bool IsHadDynamicPad(const std::vector<MxTools::MxpiBuffer*>& mxpiBuffer)
{
    return mxpiBuffer.size() > 1 && mxpiBuffer[1] != nullptr;
}
}


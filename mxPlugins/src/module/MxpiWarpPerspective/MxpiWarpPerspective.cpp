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
 * Description: Rotate an angled rectangle to a positive rectangle (affine transformation).
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <cmath>
#include "google/protobuf/text_format.h"
#include "MxBase/Log/Log.h"
#include "MxBase/DvppWrapper/DvppWrapper.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"
#include "MxPlugins/MxpiWarpPerspective/MxpiWarpPerspective.h"

using namespace MxPlugins;
using namespace MxTools;
using namespace MxBase;

namespace {
constexpr uint32_t TEXT_BOX_COORDINATES_NUM = 8;
constexpr uint32_t NUM2 = 2;
constexpr uint32_t MAX_WRITE_IMAGES = 200;

inline bool IsDeviceMemoryType(MxpiMemoryType memoryType)
{
    return memoryType == MXPI_MEMORY_DEVICE || memoryType == MXPI_MEMORY_DVPP;
}
}

APP_ERROR MxpiWarpPerspective::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    LogInfo << "element(" << elementName_ << ") Begin to initialize.";
    if (dataSource_ == "auto" && dataSourceKeys_.size() < 1) {
        LogError << "Invalid dataSourceKeys_, size must not be equal to 0!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    dataSource_ = (dataSource_ == "auto") ? dataSourceKeys_[0] : dataSource_;
    std::vector<std::string> parameterNamesPtr = {"oriImageDataSource", "debugMode"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    oriImageDataSource_ = *std::static_pointer_cast<std::string>(configParamMap["oriImageDataSource"]);
    debugMode_ = *std::static_pointer_cast<std::string>(configParamMap["debugMode"]);
    status_ = MxTools::SYNC;

    LogInfo << "element(" << elementName_ << ") End to initialize.";
    return APP_ERR_OK;
}

APP_ERROR MxpiWarpPerspective::DeInit()
{
    LogInfo << "element(" << elementName_ << ") DeInit.";
    return APP_ERR_OK;
}

APP_ERROR MxpiWarpPerspective::Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer)
{
    LogDebug << "element(" << elementName_ << ") Begin to Process.";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (CheckIsHadErrorInfo(mxpiBuffer) || CheckIsEmptyMetadata(mxpiBuffer)) {
        return APP_ERR_OK;
    }
    MxpiVisionList inputVisionList;
    ret = GetInputVisionList(mxpiBuffer, inputVisionList);
    if (ret != APP_ERR_OK) {
        SendErrorInfo(mxpiBuffer, ret);
        return ret;
    }
    auto& inputVision = inputVisionList.visionvec(0);
    const MxpiVisionData& inputVisionData = inputVision.visiondata();
    if (inputVisionData.datatype() != MxTools::MXPI_DATA_TYPE_UINT8) {
        SendErrorInfo(mxpiBuffer, ret);
        return ret;
    }
    MemoryData inputImgHostMemory(inputVisionData.datasize(), MemoryData::MEMORY_HOST);
    ret = GetVisionHostMemoryData(inputVisionData, inputImgHostMemory);
    if (ret != APP_ERR_OK) {
        SendErrorInfo(mxpiBuffer, ret);
        return ret;
    }

    cv::Mat imgRGB888;
    ret = GetImgRGB888Mat(inputVision, inputImgHostMemory, imgRGB888);
    if (ret != APP_ERR_OK) {
        FreeMemoryAndSendErrorInfo(mxpiBuffer, inputImgHostMemory, inputVisionData, ret);
        return ret;
    }
    MxpiVisionList *mxpiVisionList = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionList == nullptr) {
        FreeMemoryAndSendErrorInfo(mxpiBuffer, inputImgHostMemory, inputVisionData, APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::shared_ptr<MxTools::MxpiVisionList> outputVisionList(mxpiVisionList, g_deleteFuncMxpiVisionList);
    MxpiMetadataManager metadataManager(*mxpiBuffer[0]);
    ret = BuildOutputVisionList(imgRGB888, metadataManager, outputVisionList);
    if (ret != APP_ERR_OK) {
        FreeMemoryAndSendErrorInfo(mxpiBuffer, inputImgHostMemory, inputVisionData, ret);
        return ret;
    }

    if (IsDeviceMemoryType(inputVisionData.memtype())) {
        MemoryHelper::MxbsFree(inputImgHostMemory);
    }
    if (outputVisionList->visionvec_size() > 0) {
        metadataManager.AddProtoMetadata(elementName_, outputVisionList);
    }
    SendData(0, *mxpiBuffer[0]);
    if (IsHadDynamicPad(mxpiBuffer)) {
        DestroyExtraBuffers(mxpiBuffer, 0);
    }

    LogDebug << "element(" << elementName_ << ") End to Process.";
    return APP_ERR_OK;
}

void MxpiWarpPerspective::FreeMemoryAndSendErrorInfo(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer,
    MxBase::MemoryData &inputImgHostMemory, const MxTools::MxpiVisionData &inputVisionData, APP_ERROR errorCode)
{
    if (IsDeviceMemoryType(inputVisionData.memtype())) {
        MemoryHelper::MxbsFree(inputImgHostMemory);
    }
    SendErrorInfo(mxpiBuffer, errorCode);
}

bool MxpiWarpPerspective::CheckIsEmptyMetadata(const std::vector<MxTools::MxpiBuffer*>& mxpiBuffer)
{
    MxpiMetadataManager metadataManager0(*mxpiBuffer[0]);
    if (metadataManager0.GetMetadata(dataSource_) == nullptr) {
        LogDebug << "metadata in element(" << elementName_ << ") is a nullptr.";
        SendData(0, *mxpiBuffer[0]);
        if (IsHadDynamicPad(mxpiBuffer)) {
            DestroyExtraBuffers(const_cast<std::vector<MxTools::MxpiBuffer *> &>(mxpiBuffer), 0);
        }
        return true;
    }
    return false;
}

bool MxpiWarpPerspective::CheckIsHadErrorInfo(const std::vector<MxTools::MxpiBuffer*>& mxpiBuffer)
{
    MxpiMetadataManager metadataManager0(*mxpiBuffer[0]);
    if (metadataManager0.GetErrorInfo() != nullptr) {
        LogDebug << "element(" << elementName_ << ") pad0 had error info.";
        SendData(0, *mxpiBuffer[0]);
        if (IsHadDynamicPad(mxpiBuffer)) {
            DestroyExtraBuffers(const_cast<std::vector<MxTools::MxpiBuffer *> &>(mxpiBuffer), 0);
        }
        return true;
    }

    if (!IsHadDynamicPad(mxpiBuffer)) {
        return false;
    }

    MxpiMetadataManager metadataManager1(*mxpiBuffer[1]);
    if (metadataManager1.GetErrorInfo() != nullptr) {
        LogDebug << "element(" << elementName_ << ") pad1 had error info.";
        SendData(0, *mxpiBuffer[1]);
        const size_t exceptPort = 1;
        DestroyExtraBuffers(const_cast<std::vector<MxTools::MxpiBuffer *> &>(mxpiBuffer), exceptPort);
        return true;
    }
    return false;
}

void MxpiWarpPerspective::SendErrorInfo(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer, APP_ERROR errorCode)
{
    SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, errorCode, errorInfo_.str());
    if (IsHadDynamicPad(mxpiBuffer)) {
        DestroyExtraBuffers(mxpiBuffer, 0);
    }
}

APP_ERROR MxpiWarpPerspective::BuildOutputVisionList(const cv::Mat& imgRGB888, MxpiMetadataManager& metadataManager,
                                                     std::shared_ptr<MxTools::MxpiVisionList>& outputVisionList)
{
    auto metadata = metadataManager.GetMetadataWithType(dataSource_, "MxpiTextObjectList");
    if (metadata == nullptr) {
        errorInfo_ << "Not a MxpiTextObjectList object." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }

    auto textObjList = std::static_pointer_cast<MxpiTextObjectList>(metadata);
    std::string textObjListStr;
    google::protobuf::TextFormat::PrintToString(*textObjList, &textObjListStr);
    LogDebug << "element(" << elementName_ << ") textObjectList=" << textObjListStr;

    for (int i = 0; i < textObjList->objectvec_size(); i++) {
        auto textObject = textObjList->objectvec(i);
        float cropWidth = CalcCropWidth(textObject);
        float cropHeight = CalcCropHeight(textObject);
        size_t dataSize = static_cast<size_t>(static_cast<int>(cropWidth) * static_cast<int>(cropHeight)
                * YUV_BGR_SIZE_CONVERT_3);
        MemoryData warpDstMemoryData(dataSize, MemoryData::MEMORY_HOST);

        APP_ERROR ret = DoWarpPerspective(imgRGB888, textObject, cropWidth, cropHeight, warpDstMemoryData);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to do warpPerspective." << GetErrorInfo(ret);
            return ret;
        }
        ret = BuildOutputVision(i, outputVisionList, static_cast<int>(cropWidth), static_cast<int>(cropHeight),
                warpDstMemoryData);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to do build OutputVision." << GetErrorInfo(ret);
            MemoryHelper::MxbsFree(warpDstMemoryData);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiWarpPerspective::BuildOutputVision(int index, std::shared_ptr<MxTools::MxpiVisionList> &outputVisionList,
                                                 int cropWidth, int cropHeight,
                                                 const MemoryData &warpDstMemoryData) const
{
    auto visionOutput = outputVisionList->add_visionvec();
    if (CheckPtrIsNullptr(visionOutput, "visionOutput"))  return APP_ERR_COMM_ALLOC_MEM;
    auto visionInfoOutput = visionOutput->mutable_visioninfo();
    visionInfoOutput->set_format(MXBASE_PIXEL_FORMAT_RGB_888);
    visionInfoOutput->set_height(cropHeight);
    visionInfoOutput->set_width(cropWidth);

    auto visionDataOutput = visionOutput->mutable_visiondata();
    visionDataOutput->set_memtype(MXPI_MEMORY_HOST);
    visionDataOutput->set_datasize(warpDstMemoryData.size);
    visionDataOutput->set_dataptr((uint64) warpDstMemoryData.ptrData);

    auto mxpiMetaHeader = visionOutput->add_headervec();
    if (CheckPtrIsNullptr(mxpiMetaHeader, "mxpiMetaHeader"))  return APP_ERR_COMM_ALLOC_MEM;
    mxpiMetaHeader->set_memberid(index);
    mxpiMetaHeader->set_datasource(dataSource_);
    return APP_ERR_OK;
}

APP_ERROR MxpiWarpPerspective::DoWarpPerspective(const cv::Mat& imgRGB888, const MxTools::MxpiTextObject& textObject,
                                                 float cropWidth, float cropHeight,
                                                 MxBase::MemoryData& warpDstMemoryData) const
{
    float keyPointAfter[TEXT_BOX_COORDINATES_NUM] = { 0, 0, cropWidth, 0, cropWidth, cropHeight, 0, cropHeight };
    float keyPointBefore[TEXT_BOX_COORDINATES_NUM] = {
        textObject.x0(), textObject.y0(), textObject.x1(), textObject.y1(),
        textObject.x2(), textObject.y2(), textObject.x3(), textObject.y3()
    };
    cv::Point2f srcPoints[TEXT_BOX_COORDINATES_NUM / NUM2];
    cv::Point2f destPoints[TEXT_BOX_COORDINATES_NUM / NUM2];
    for (size_t i = 0; i < TEXT_BOX_COORDINATES_NUM / NUM2; i++) {
        srcPoints[i] = cv::Point2f(keyPointBefore[i * NUM2], keyPointBefore[i * NUM2 + 1]);
        destPoints[i] = cv::Point2f(keyPointAfter[i * NUM2], keyPointAfter[i * NUM2 + 1]);
    }

    // Get perspective transform matrix
    cv::Mat warpMat = cv::getPerspectiveTransform(srcPoints, destPoints);
    auto ret = MemoryHelper::MxbsMalloc(warpDstMemoryData);
    if (ret != APP_ERR_OK) {
        LogError << "Memory malloc failed." << GetErrorInfo(ret);
        return ret;
    }

    cv::Mat warpDst((int) cropHeight, (int) cropWidth, CV_8UC3, warpDstMemoryData.ptrData);
    cv::warpPerspective(imgRGB888, warpDst, warpMat, warpDst.size());

    if (debugMode_ == "true") {
        static uint32_t index = 0;
        std::string filePath = "warpPerspectiveImage/image" + std::to_string(index++) + ".jpg";
        SaveJpg(filePath, warpDst);
        if (index > MAX_WRITE_IMAGES) {
            index = 0;
        }
    }

    return APP_ERR_OK;
}

void MxpiWarpPerspective::SaveJpg(const std::string& filePath, const cv::Mat& imgRGB888) const
{
    std::vector<unsigned char> buff;
    const int compressRate = 95;
    std::vector<int> param;
    param.push_back(cv::IMWRITE_JPEG_QUALITY);
    param.push_back(compressRate); // default(95) 0-100
    cv::imencode(".jpg", imgRGB888, buff, param);
    std::string buffStr((char*) buff.data(), buff.size());
    if (!FileUtils::CreateDirectories("warpPerspectiveImage")) {
        LogError << "MxpiWarpPerspective: Failed to create directory in save jpg process."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
    if (!FileUtils::WriteFileContent(filePath, buffStr)) {
        LogError << "MxpiWarpPerspective: Failed to write file in save jpg process."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
}

APP_ERROR MxpiWarpPerspective::GetImgRGB888Mat(const MxpiVision& vision, MemoryData& hostMemoryData,
                                               cv::Mat& imgRGB888)
{
    const MxpiVisionInfo& visionInfo = vision.visioninfo();
    if (visionInfo.format() == MXBASE_PIXEL_FORMAT_YUV_SEMIPLANAR_420) {
        cv::Mat srcNV12Mat(visionInfo.heightaligned() * YUV_BGR_SIZE_CONVERT_3 / YUV_BGR_SIZE_CONVERT_2,
                           visionInfo.widthaligned(), CV_8UC1, hostMemoryData.ptrData);
        cv::Mat dstRGB888(visionInfo.height(), visionInfo.width(), CV_8UC3);
        cv::cvtColor(srcNV12Mat, dstRGB888, cv::COLOR_YUV2RGB_NV12);
        imgRGB888 = dstRGB888.clone();
    } else if (visionInfo.format() == MXBASE_PIXEL_FORMAT_RGB_888
               || visionInfo.format() == MXBASE_PIXEL_FORMAT_BGR_888) {
        imgRGB888 = cv::Mat(visionInfo.height(), visionInfo.width(), CV_8UC3, hostMemoryData.ptrData);
    } else {
        errorInfo_ << "Pixel format: " << visionInfo.format() << " is not supported.";
        LogWarn << errorInfo_.str() << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }

    if (debugMode_ == "true") {
        const std::string filePath = "warpPerspectiveImage/originImage.jpg";
        SaveJpg(filePath, imgRGB888);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiWarpPerspective::GetVisionHostMemoryData(const MxpiVisionData& visionData,
                                                       MemoryData& dstMemoryData) const
{
    MxpiMemoryType mxpiMemoryType = visionData.memtype();
    if (mxpiMemoryType == MXPI_MEMORY_DEVICE || mxpiMemoryType == MXPI_MEMORY_DVPP) {
        auto memoryType = mxpiMemoryType == MXPI_MEMORY_DEVICE ? MemoryData::MEMORY_DEVICE : MemoryData::MEMORY_DVPP;
        MemoryData srcMemoryData((void*) visionData.dataptr(), visionData.datasize(), memoryType,
                                 visionData.deviceid());
        auto ret = MemoryHelper::MxbsMallocAndCopy(dstMemoryData, srcMemoryData);
        if (ret != APP_ERR_OK) {
            LogError << "Memory malloc failed." << GetErrorInfo(ret);
            return ret;
        }
    } else {
        dstMemoryData.ptrData = (void*) visionData.dataptr();
    }

    return APP_ERR_OK;
}

// Calculate Euclidean Distance
float MxpiWarpPerspective::CalcCropWidth(const MxpiTextObject& textObject) const
{
    float x0 = std::abs(textObject.x1() - textObject.x0());
    float y0 = std::abs(textObject.y1() - textObject.y0());
    float line0 = sqrt(std::pow(x0, NUM2) + std::pow(y0, NUM2));

    float x1 = std::abs(textObject.x2() - textObject.x3());
    float y1 = std::abs(textObject.y2() - textObject.y3());
    float line1 = std::sqrt(std::pow(x1, NUM2) + std::pow(y1, NUM2));

    return line1 > line0 ? line1 : line0;
}

float MxpiWarpPerspective::CalcCropHeight(const MxpiTextObject& textObject) const
{
    float x0 = std::abs(textObject.x0() - textObject.x3());
    float y0 = std::abs(textObject.y0() - textObject.y3());
    float line0 = sqrt(std::pow(x0, NUM2) + std::pow(y0, NUM2));

    float x1 = std::abs(textObject.x1() - textObject.x2());
    float y1 = std::abs(textObject.y1() - textObject.y2());
    float line1 = std::sqrt(std::pow(x1, NUM2) + std::pow(y1, NUM2));

    return line1 > line0 ? line1 : line0;
}

APP_ERROR MxpiWarpPerspective::GetInputVisionList(const std::vector<MxTools::MxpiBuffer*>& mxpiBuffer,
                                                  MxpiVisionList& inputVisionList)
{
    // get origin image from staticPad0 or dynamicPad1
    if (IsHadDynamicPad(mxpiBuffer)) {
        MxpiMetadataManager metadataManager(*mxpiBuffer[1]);
        auto metadata = metadataManager.GetMetadata(oriImageDataSource_);
        if (metadata == nullptr) {
            errorInfo_ << "Dynamic pad metadata is null. dataSource(" << oriImageDataSource_ << ")."
                       << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL);
            LogError << errorInfo_.str();
            return APP_ERR_DVPP_CROP_FAIL;
        }

        auto protobufMessage = std::static_pointer_cast<google::protobuf::Message>(metadata);
        auto desc = protobufMessage->GetDescriptor();
        if (!desc) {
            errorInfo_ << "Get input data's descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_INVALID_PARAM; // self define the error code
        }
        if (desc->name() != "MxpiVisionList") {
            errorInfo_ << "Type is not MxpiVisionList. desc->name(" << desc->name() << ")."
                       << GetErrorInfo(APP_ERR_COMM_FAILURE);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_FAILURE;
        }

        auto visionList = std::static_pointer_cast<MxpiVisionList>(metadata);
        if (visionList->visionvec_size() == 0) {
            errorInfo_ << "MxpiVisionList visionvec is empty." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_FAILURE;
        }

        inputVisionList.CopyFrom(*visionList);
        return APP_ERR_OK;
    }

    MxpiFrame hostMxpiFrame = MxpiBufferManager::GetHostDataInfo(*mxpiBuffer[0]);
    if (hostMxpiFrame.visionlist().visionvec_size() != 0) {
        if (hostMxpiFrame.visionlist().visionvec(0).visiondata().dataptr() != 0) {
            inputVisionList.CopyFrom(hostMxpiFrame.visionlist());
            return APP_ERR_OK;
        }
    }

    MxpiFrame deviceMxpiFrame = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer[0]);
    if (deviceMxpiFrame.visionlist().visionvec_size() == 0) {
        errorInfo_ << "MxpiVisionList visionvec is empty." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_FAILURE;
    }
    inputVisionList.CopyFrom(deviceMxpiFrame.visionlist());
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiWarpPerspective::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto dynamicDataSourceProp = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string>{
        STRING, "oriImageDataSource", "oriImageDataSource", "the name of dynamic pad data source", "", "", ""});
    auto debugModeProp = std::make_shared<ElementProperty<std::string>>(
        ElementProperty<std::string>{STRING, "debugMode", "debugMode", "the name of data source", "false", "", ""});

    properties.push_back(dynamicDataSourceProp);
    properties.push_back(debugModeProp);
    return properties;
}

MxpiPortInfo MxpiWarpPerspective::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/object", "metadata/texts"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    std::vector<std::vector<std::string>> dynamicValue = {{"image/yuv", "image/rgb"}};
    GenerateDynamicInputPortsInfo(dynamicValue, inputPortInfo);
    return inputPortInfo;
}

MX_PLUGIN_GENERATE(MxpiWarpPerspective)
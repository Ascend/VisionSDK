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

#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/Log/Log.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"
#include "MxPlugins/MxpiSemanticSegStitcher/MxpiSemanticSegStitcher.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const int SEMANTIC_SEG_INDEX = 0;
const int CROP_ROI_INDEX = 1;
const int MERGE_ROI_INDEX = 2;
const int IMAGE_MASK_DIM = 2;
const int MXPIBUFFER_NUM = 3;
}

APP_ERROR MxpiSemanticSegStitcher::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    status_ = MxTools::SYNC;
    LogInfo << "Begin to initialize MxpiSemanticSegStitcher(" << elementName_ << ").";

    std::vector<std::string> parameterNamesPtr = {"dataSourceMergeRoi", "dataSourceCropRoi", "dataSourceSemanticSeg"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    if (dataSourceKeys_.size() <= MERGE_ROI_INDEX) {
        LogError << "Invalid dataSourceKeys_, size must not be less than 3!"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    dataSourceMergeRoi_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceMergeRoi"]);
    if (dataSourceMergeRoi_ == "auto") {
        dataSourceMergeRoi_ = dataSourceKeys_[MERGE_ROI_INDEX];
    } else {
        dataSourceKeys_[MERGE_ROI_INDEX] = dataSourceMergeRoi_;
    }

    dataSourceCropRoi_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceCropRoi"]);
    if (dataSourceCropRoi_ == "auto") {
        dataSourceCropRoi_ = dataSourceKeys_[CROP_ROI_INDEX];
    } else {
        dataSourceKeys_[CROP_ROI_INDEX] = dataSourceCropRoi_;
    }

    dataSourceSemanticSeg_ = *std::static_pointer_cast<std::string>(configParamMap["dataSourceSemanticSeg"]);
    if (dataSourceSemanticSeg_ == "auto") {
        dataSourceSemanticSeg_ = dataSourceKeys_[SEMANTIC_SEG_INDEX];
    } else {
        dataSourceKeys_[SEMANTIC_SEG_INDEX] = dataSourceSemanticSeg_;
    }

    doPreErrorCheck_ = true;
    doPreMetaDataCheck_ = true;
    LogInfo << "End to initialize MxpiSemanticSegStitcher(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiSemanticSegStitcher::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiSemanticSegStitcher(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiSemanticSegStitcher(" << elementName_ << ").";
    return APP_ERR_OK;
}

void MxpiSemanticSegStitcher::GetRelativeRoiInfo(const std::shared_ptr<MxpiObjectList>& cropRoiList,
    const std::shared_ptr<MxpiObjectList>& mergeRoiList, std::vector<CropRoiBox> &relativeRoiInfo)
{
    LogDebug << elementName_ << " Begin to get relative roi information";
    for (int i = 0; (i < cropRoiList->objectvec_size()) && (i < mergeRoiList->objectvec_size()); i++) {
        auto mergeRoi = mergeRoiList->objectvec(i);
        auto cropRoi = cropRoiList->objectvec(i);
        CropRoiBox relativeRoi {
                mergeRoi.x0() - cropRoi.x0(), mergeRoi.y0() - cropRoi.y0(),
                mergeRoi.x1() - cropRoi.x0(), mergeRoi.y1() - cropRoi.y0()
        };
        relativeRoiInfo.push_back(relativeRoi);
    }
    LogDebug << elementName_ << " Success to get relative roi information";
}

APP_ERROR MxpiSemanticSegStitcher::GetImageSize(MxpiBuffer& buffer)
{
    MxpiFrame inputMxpiFrame = MxpiBufferManager::GetHostDataInfo(buffer);
    if (inputMxpiFrame.visionlist().visionvec_size() == 0) {
        errorInfo_ << "Buffer is empty." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    auto buffInfo = inputMxpiFrame.visionlist().visionvec(0).visioninfo();
    imageHeight_ = static_cast<int>(buffInfo.height());
    imageWidth_ = static_cast<int>(buffInfo.width());
    return APP_ERR_OK;
}

APP_ERROR MxpiSemanticSegStitcher::CheckProtobufName(const std::shared_ptr<void>& metadataPtr,
    const std::string& dataSourceName, std::string &protobufName)
{
    auto foo = (google::protobuf::Message*)metadataPtr.get();
    auto desc = foo->GetDescriptor();
    if (!desc) {
        errorInfo_ << "Get input data's descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM; // self define the error code
    }
    if (desc->name() != protobufName) {
        errorInfo_ <<  "Metadata from " << dataSourceName << " is  not a " << protobufName
                   << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiSemanticSegStitcher::GetBaseInfo(std::vector<MxpiBuffer *> &mxpiBuffer,
                                               std::shared_ptr<MxpiObjectList> &mergeRoiList,
                                               std::shared_ptr<MxpiObjectList> &cropRoiList,
                                               std::shared_ptr<MxpiImageMaskList> &srcImageMaskList)
{
    LogDebug << elementName_ << " Begin to get the information of crop roi, merge roi and infer result meatadata.";
    // Get the merge roi information.
    auto inferMetadataPtr = MxpiMetadataManager(*mxpiBuffer[SEMANTIC_SEG_INDEX]).GetMetadata(dataSourceSemanticSeg_);
    if (inferMetadataPtr == nullptr) {
        LogError << "The inferMetadataPtr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    std::string protobufName = "MxpiImageMaskList";
    APP_ERROR ret = CheckProtobufName(inferMetadataPtr, dataSourceSemanticSeg_, protobufName);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    srcImageMaskList = std::static_pointer_cast<MxpiImageMaskList>(inferMetadataPtr);
    LogDebug << elementName_ << " Success to get the information of  infer result meatadata.";

    protobufName = "MxpiObjectList";
    // Get the crop roi information.
    auto cropRoiPtr = MxpiMetadataManager(*mxpiBuffer[CROP_ROI_INDEX]).GetMetadata(dataSourceCropRoi_);
    if (cropRoiPtr == nullptr) {
        LogError << "The cropRoiPtr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    ret = CheckProtobufName(cropRoiPtr, dataSourceCropRoi_, protobufName);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    cropRoiList = std::static_pointer_cast<MxpiObjectList>(cropRoiPtr);
    LogDebug << elementName_ << " Success to get the information of  crop roi meatadata.";

    // Get infer results.
    auto mergeRoiPtr = MxpiMetadataManager(*mxpiBuffer[MERGE_ROI_INDEX]).GetMetadata(dataSourceMergeRoi_);
    if (mergeRoiPtr == nullptr) {
        LogError << "The mergeRoiPtr is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    ret = CheckProtobufName(mergeRoiPtr, dataSourceMergeRoi_, protobufName);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    mergeRoiList = std::static_pointer_cast<MxpiObjectList>(mergeRoiPtr);
    LogDebug << elementName_ << " Success to get the information of  merge roi meatadata.";

    // Check whether the input data is valid.
    ret = CheckMetadata(srcImageMaskList, cropRoiList, mergeRoiList);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << elementName_ << " Success to get the base information.";
    return APP_ERR_OK;
}

APP_ERROR MxpiSemanticSegStitcher::GetMergeImage(cv::Mat& dstImage,
    std::shared_ptr<MxpiImageMaskList>& srcImageMaskList, const std::shared_ptr<MxpiObjectList>& mergeRoiList,
    std::vector<CropRoiBox>& relativeRoiInfo) const
{
    LogDebug << elementName_ << " Begin to get merge image.";
    auto dataType = (srcImageMaskList->imagemaskvec(0).datatype() == MxBase::TENSOR_DTYPE_UINT8) ? CV_8UC1 : CV_32SC1;
    dstImage = cv::Mat::zeros(imageHeight_, imageWidth_, dataType);
    for (int i = 0; i < srcImageMaskList->imagemaskvec_size(); i++) {
        auto srcImageMask = srcImageMaskList->imagemaskvec(i);
        const auto& dataStr = srcImageMask.datastr();
        cv::Mat srcRGB(srcImageMask.shape(0), srcImageMask.shape(1), dataType);
        srcRGB.data = (uchar*)dataStr.c_str();
        LogDebug << "No." << i <<" The relative Roi x0: " << relativeRoiInfo[i].x0
                 << "; y0:" << relativeRoiInfo[i].y0 << "; x1: " << relativeRoiInfo[i].x1
                 << "; y1: " << relativeRoiInfo[i].y1;
        cv::Mat roiRGB = srcRGB(cv::Range(relativeRoiInfo[i].y0, relativeRoiInfo[i].y1),
                                cv::Range(relativeRoiInfo[i].x0, relativeRoiInfo[i].x1));
        // set the roi area of the srcImage
        auto mergeRoi = mergeRoiList->objectvec(i);
        LogDebug << "No." << i << " The merge Roi is x0: " << mergeRoi.x0() << "; y0:"
                 << mergeRoi.y0() << "; x1: " << mergeRoi.x1() << "; y1: " << mergeRoi.y1();
        roiRGB.copyTo(dstImage(cv::Rect(mergeRoi.x0(), mergeRoi.y0(), roiRGB.cols, roiRGB.rows)));
    }
    LogDebug << elementName_ << " Success to get merge image.";
    return APP_ERR_OK;
}

APP_ERROR MxpiSemanticSegStitcher::GetOutputProtoMetadata(std::shared_ptr<MxpiImageMaskList> &dstImageMaskList,
    const cv::Mat& dstImage, const std::shared_ptr<MxpiImageMaskList> &srcImageMaskList)
{
    LogDebug << elementName_ << " Begin to set the output proto metadata.";
    auto imageMask = dstImageMaskList->add_imagemaskvec();
    if (CheckPtrIsNullptr(imageMask, "imageMask"))  return APP_ERR_COMM_ALLOC_MEM;
    // Set MetaHeader
    MxpiMetaHeader* segMetaHeader = imageMask->add_headervec();
    if (CheckPtrIsNullptr(segMetaHeader, "segMetaHeader"))  return APP_ERR_COMM_ALLOC_MEM;
    segMetaHeader->set_datasource(dataSourceSemanticSeg_);
    segMetaHeader->set_memberid(0);

    MxpiMetaHeader* mergeMetaHeader = imageMask->add_headervec();
    if (CheckPtrIsNullptr(mergeMetaHeader, "mergeMetaHeader"))  return APP_ERR_COMM_ALLOC_MEM;
    mergeMetaHeader->set_datasource(dataSourceMergeRoi_);
    mergeMetaHeader->set_memberid(0);

    MxpiMetaHeader* cropMetaHeader = imageMask->add_headervec();
    if (CheckPtrIsNullptr(cropMetaHeader, "cropMetaHeader"))  return APP_ERR_COMM_ALLOC_MEM;
    cropMetaHeader->set_datasource(dataSourceCropRoi_);
    cropMetaHeader->set_memberid(0);

    // Set className
    auto className = imageMask->mutable_classname();
    className->CopyFrom(srcImageMaskList->imagemaskvec(0).classname());

    // Set shape
    imageMask->add_shape(imageHeight_);
    imageMask->add_shape(imageWidth_);

    // Set dataType
    imageMask->set_datatype(srcImageMaskList->imagemaskvec(0).datatype());
    // Set  dataStr
    auto dstDataStr = std::string((char *)dstImage.data, (dstImage.dataend - dstImage.datastart) * sizeof(uchar));
    imageMask->set_datastr(dstDataStr);
    LogDebug << elementName_ << " Success to set the output proto metadata.";
    return APP_ERR_OK;
}

APP_ERROR MxpiSemanticSegStitcher::CheckMetadata(std::shared_ptr<MxpiImageMaskList> &srcImageMaskList,
    std::shared_ptr<MxpiObjectList> &cropRoiList, std::shared_ptr<MxpiObjectList> &mergeRoiList)
{
    if (srcImageMaskList->imagemaskvec_size() == 0) {
        errorInfo_ << "No imageMask metadata in ImageMaskList."<< GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    if (srcImageMaskList->imagemaskvec(0).shape_size() != IMAGE_MASK_DIM) {
        errorInfo_ << "Only support single channel images temporarily." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    auto type = (MxBase::TensorDataType)srcImageMaskList->imagemaskvec(0).datatype();
    if ((type != MxBase::TENSOR_DTYPE_UINT8) && (type != MxBase::TENSOR_DTYPE_INT32)) {
        errorInfo_ << "Only support TENSOR_DTYPE_UINT8 and TENSOR_DTYPE_INT32 temporarily."
                   << GetErrorInfo(APP_ERR_COMM_UNREALIZED);
        return APP_ERR_COMM_UNREALIZED;
    }
    // Check the if List srcImageMaskList, cropRoiList and mergeRoiList are equal to each other.
    int imageMaskListSize = srcImageMaskList->imagemaskvec_size();
    int cropRoiListSize = cropRoiList->objectvec_size();
    int mergeRoiListSize = mergeRoiList->objectvec_size();
    if (imageMaskListSize != cropRoiListSize || imageMaskListSize != mergeRoiListSize) {
        errorInfo_ << "The size of image list(" << imageMaskListSize << "), crop list(" << cropRoiListSize
                   << "), merge list(" << mergeRoiListSize << ") do not equal to each other."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    auto srcImage = srcImageMaskList->imagemaskvec(0);
    auto cropRoi = cropRoiList->objectvec(0);
    if (cropRoi.y0() < 0 || cropRoi.x0() < 0 || !IsDenominatorZero(srcImage.shape(0) - (cropRoi.y1() - cropRoi.y0()))
        || !IsDenominatorZero(srcImage.shape(1) - (cropRoi.x1() - cropRoi.x0()))) {
        LogError << "The semantic segmentation image size is not equal to the crop size."
                 << " Please check the resize situation. \n"
                 << " image width: " << srcImage.shape(0) << ", image height: " << srcImage.shape(1)
                 << "\n crop roi width: " << cropRoi.y1() - cropRoi.y0()
                 << ", crop roi height: " << cropRoi.x1() - cropRoi.x0()  << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    return APP_ERR_OK;
}

void MxpiSemanticSegStitcher::DestroyBufferAndSendErrorInfo(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
                                                            APP_ERROR errorCode)
{
    LogError << errorInfo_.str() << GetErrorInfo(APP_ERR_COMM_FAILURE);
    SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, errorCode, errorInfo_.str());
    DestroyExtraBuffers(mxpiBuffer, 0);
}

APP_ERROR MxpiSemanticSegStitcher::CheckSemanticSegStitcherMxpiBufferIsValid(
    std::vector<MxTools::MxpiBuffer*>& mxpiBuffer)
{
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (mxpiBuffer.size() < MXPIBUFFER_NUM) {
        DestroyBufferAndSendErrorInfo(mxpiBuffer, APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiSemanticSegStitcher::Process(std::vector<MxpiBuffer*> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiSemanticSegStitcher(" << elementName_ << ").";
    auto ret = CheckSemanticSegStitcherMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxpiBuffer* buffer = mxpiBuffer[0];
    MxpiMetadataManager mxpiMetadataManager(*buffer);
    // Get the image size.
    ret = GetImageSize(*buffer);
    if (ret != APP_ERR_OK) {
        DestroyBufferAndSendErrorInfo(mxpiBuffer, ret);
        return ret;
    }

    std::shared_ptr<MxpiObjectList> mergeRoiList = MemoryHelper::MakeShared<MxpiObjectList>();
    std::shared_ptr<MxpiObjectList> cropRoiList = MemoryHelper::MakeShared<MxpiObjectList>();
    std::shared_ptr<MxpiImageMaskList> srcImageMaskList = MemoryHelper::MakeShared<MxpiImageMaskList>();
    std::shared_ptr<MxpiImageMaskList> dstImageMaskList = MemoryHelper::MakeShared<MxpiImageMaskList>();
    if (!(mergeRoiList && cropRoiList && srcImageMaskList && dstImageMaskList)) {
        errorInfo_ << "Failed to malloc memory!" << GetErrorInfo(ret = APP_ERR_COMM_FAILURE);
        DestroyBufferAndSendErrorInfo(mxpiBuffer, ret);
        return ret;
    }

    // Get the base information for merge each block image.
    ret = GetBaseInfo(mxpiBuffer, mergeRoiList, cropRoiList, srcImageMaskList);
    if (ret != APP_ERR_OK) {
        DestroyBufferAndSendErrorInfo(mxpiBuffer, ret);
        return ret;
    }

    // Get the relative roi information.
    std::vector<CropRoiBox> relativeRoiInfo;
    GetRelativeRoiInfo(cropRoiList, mergeRoiList, relativeRoiInfo);

    // Get the merge image.
    cv::Mat dstImage;
    ret = GetMergeImage(dstImage, srcImageMaskList, mergeRoiList, relativeRoiInfo);
    if (ret != APP_ERR_OK) {
        DestroyBufferAndSendErrorInfo(mxpiBuffer, ret);
        return ret;
    }

    // Get the dstMxpiIMageMaskList.
    ret = GetOutputProtoMetadata(dstImageMaskList, dstImage, srcImageMaskList);
    if (ret != APP_ERR_OK) {
        DestroyBufferAndSendErrorInfo(mxpiBuffer, ret);
        return ret;
    }
    // AddProtoMetadata and SendData.
    ret = mxpiMetadataManager.AddProtoMetadata(elementName_, dstImageMaskList);
    if (ret != APP_ERR_OK) {
        DestroyBufferAndSendErrorInfo(mxpiBuffer, ret);
        return ret;
    }
    SendData(0, *buffer);
    DestroyExtraBuffers(mxpiBuffer, 0);
    LogDebug << "End to process MxpiSemanticSegStitcher(" << elementName_ << ").";
    return APP_ERR_OK;
}

MxpiPortInfo MxpiSemanticSegStitcher::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {
        {"metadata/semanticseg"}, {"metadata/object"}, {"metadata/object/roi-info"}
    };
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiSemanticSegStitcher::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/semanticseg"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}
std::vector<std::shared_ptr<void>> MxpiSemanticSegStitcher::DefineProperties()
{
    auto dataSourceMergeRoi = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceMergeRoi", "dataSourceLists", "the data source key of merge roi", "auto", "", ""
    });
    auto dataSourceCropRoi = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceCropRoi", "dataSourceLists", "the data source key of crop roi", "auto", "", ""
    });
    auto dataSourceSemanticSeg = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dataSourceSemanticSeg", "dataSourceLists", "the data source key of semanticSeg result", "auto", "", ""
    });
    std::vector<std::shared_ptr<void>> properties = { dataSourceMergeRoi, dataSourceCropRoi, dataSourceSemanticSeg };
    return properties;
}

namespace {
MX_PLUGIN_GENERATE(MxpiSemanticSegStitcher)
}

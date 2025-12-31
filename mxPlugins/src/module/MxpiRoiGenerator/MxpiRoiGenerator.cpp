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
 * Description: Plugin for generator roi for image crop.
 * Author: MindX SDK
 * Create: 2021
 * History: NA
 */

#include "MxPlugins/MxpiRoiGenerator/MxpiRoiGenerator.h"
#include <vector>
#include <map>
#include "MxBase/Utils/StringUtils.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const int SPLIT = 1;
const int SPLIT_AND_STITCHER = 2;
const int COORDINATE_NUM = 4;
const int X0 = 0;
const int Y0 = 1;
const int X1 = 2;
const int Y1 = 3;

std::map<std::string, uint32_t> SPLITTYPE = {
    {"Size_Block", SIZE_BLOCK},
    {"Num_Block", NUM_BLOCK},
    {"Custom", CUSTOM}
};

APP_ERROR CheckRoiBox(MxBase::CropRoiBox &roiBox)
{
    if (roiBox.x0 < 0 || roiBox.x1 < 0 || roiBox.y0 < 0 || roiBox.y1 < 0) {
        LogError << "Coordinate value cannot be negative. x0: " << roiBox.x0 << ", y0: " << roiBox.y0
                 << ", x1: " << roiBox.x1 << ", y1: " << roiBox.y1 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (roiBox.x0 >= roiBox.x1 || roiBox.y0 >= roiBox.y1) {
        LogError << "Invalid Roi box. x0: " << roiBox.x0 << ", y0: " << roiBox.y0
                 << ", x1: " << roiBox.x1 << ", y1: " << roiBox.y1 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR StringToCropBoxVec(std::string &str, std::vector<CropRoiBox> &roiVec)
{
    auto vecs = MxBase::StringUtils::Split(str, '|');
    if (vecs.empty()) {
        LogError << "Incorrect roi data[" << str
                 << "]. Please set this parameter in the format of \"x0,y0,x1,y1|x0,y0,x1,y1|...\"."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (auto &vec : vecs) {
        std::vector<float> roiValue = MxBase::StringUtils::SplitAndCastToFloat(vec, ',');
        if (roiValue.size() != COORDINATE_NUM) {
            LogError << "The coordinate is " << vec
                     << " Number of coordinate point is not equal to four."<< GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        CropRoiBox roi {roiValue[X0], roiValue[Y0], roiValue[X1], roiValue[Y1]};
        APP_ERROR ret = CheckRoiBox(roi);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        roiVec.push_back(roi);
    }
    return APP_ERR_OK;
}

APP_ERROR CheckCropRoiAndMergeRoi(std::vector<MxBase::CropRoiBox> &cropRoiVec,
    std::vector<MxBase::CropRoiBox> mergeRoiVec)
{
    if (cropRoiVec.size() != mergeRoiVec.size()) {
        LogError << "The cropRoi vector size is not equal to mergeRoi vector size. Please check."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    for (size_t i = 0; i < cropRoiVec.size(); i++) {
        if (mergeRoiVec[i].x0 < cropRoiVec[i].x0 || mergeRoiVec[i].y0 < cropRoiVec[i].y0
            || mergeRoiVec[i].x1 > cropRoiVec[i].x1 || mergeRoiVec[i].y1 > cropRoiVec[i].y1) {
            LogError << "No." << i << " mergeRoi box is out of the cropRoi box. Please check."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
    }
    return APP_ERR_OK;
}

MxpiObjectList RoiVecToObjectList(std::vector<MxBase::CropRoiBox> &roiVec)
{
    MxpiObjectList mxpiObjectList;
    for (auto &roi : roiVec) {
        MxpiObject* mxpiObject = mxpiObjectList.add_objectvec();
        if (CheckPtrIsNullptr(mxpiObject, "mxpiObject"))  return mxpiObjectList;
        mxpiObject->set_x0(roi.x0);
        mxpiObject->set_y0(roi.y0);
        mxpiObject->set_x1(roi.x1);
        mxpiObject->set_y1(roi.y1);
    }
    return mxpiObjectList;
}
}

APP_ERROR MxpiRoiGenerator::JudgeUsage()
{
    LogInfo << "The srcPadNum is " << srcPadNum_;
    switch (srcPadNum_) {
        case SPLIT:
            LogInfo << "element(" << elementName_ << "), one output port, do the image blocking only.";
            needMergeRoi_ = false;
            break;
        case SPLIT_AND_STITCHER:
            LogInfo << "element(" << elementName_ << "), two output ports, "
                    << "do the image blocking  at this element and merging in other element.";
            needMergeRoi_ = true;
            break;
        default:
            LogError << "The number of output ports can only be 1 or 2, represent two different usage:\n"
                     << "    srcPadNum = 1 -> do the image blocking only.\n"
                     << "    srcPadNum = 2 -> do the image blocking at this element and merging in other element.\n"
                     << "But actual srcPadNum = " << srcPadNum_
                     << ", please check " << elementName_ << "'s output ports." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiRoiGenerator::JudgeSplitType(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    APP_ERROR ret;
    if (splitType_ == Type::SIZE_BLOCK) {
        LogInfo << "Using Size_Block mode to split the image.";
        std::vector<std::string> parameterNamesPtr = {"blockHeight", "blockWidth"};
        ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
        if (ret != APP_ERR_OK) {
            LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
            return ret;
        }
        blockHeight_ = *std::static_pointer_cast<int>(configParamMap["blockHeight"]);
        blockWidth_ = *std::static_pointer_cast<int>(configParamMap["blockWidth"]);
        if ((blockWidth_ - overlapWidth_) <= 0 || (blockHeight_ - overlapHeight_) <= 0) {
            LogError << "The overlap size can not be larger or equal to the block size."
                     << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
        }
        LogInfo << "The image will be split by a block with Height = " << blockHeight_ << " Width = " << blockWidth_;
    } else if (splitType_ == Type::NUM_BLOCK) {
        LogInfo << "Using Num_Block mode to split the image";
        std::vector<std::string> parameterNamesPtr = {"chessboardHeight", "chessboardWidth"};
        ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
        if (ret != APP_ERR_OK) {
            LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
            return ret;
        }
        chessboardHeight_ = *std::static_pointer_cast<int>(configParamMap["chessboardHeight"]);
        chessboardWidth_ = *std::static_pointer_cast<int>(configParamMap["chessboardWidth"]);
        LogInfo << elementName_ << " The image will be split by "
                << chessboardHeight_ << " * " << chessboardWidth_ << " block.";
    } else if (splitType_ == Type::CUSTOM) {
        LogInfo << "Using Num_Block mode to split the image";
        std::vector<std::string> parameterNamesPtr = {"cropRoi"};
        ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
        if (ret != APP_ERR_OK) {
            LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
            return ret;
        }
        std::string cropRoi =  *std::static_pointer_cast<std::string>(configParamMap["cropRoi"]);
        ret = StringToCropBoxVec(cropRoi, cropRoiVec_);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to get cropRoi vector. Please check the \"cropRoi\" parameter."
                     << GetErrorInfo(ret);
            return ret;
        }
        if (needMergeRoi_) {
            std::vector<std::string> parameterMergeRoiPtr = {"mergeRoi"};
            ret = CheckConfigParamMapIsValid(parameterMergeRoiPtr, configParamMap);
            if (ret != APP_ERR_OK) {
                LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
                return ret;
            }
            std::string mergeRoi = *std::static_pointer_cast<std::string>(configParamMap["mergeRoi"]);
            ret = StringToCropBoxVec(mergeRoi, mergeRoiVec_);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to get mergeRoi vector. Please check the \"mergeRoi\" parameter."
                         << GetErrorInfo(ret);
                return ret;
            }
            ret = CheckCropRoiAndMergeRoi(cropRoiVec_, mergeRoiVec_);
            if (ret != APP_ERR_OK) {
                return ret;
            }
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiRoiGenerator::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiRoiGenerator(" << elementName_ << ").";
    std::vector<std::string> parameterNamesPtr = {"overlapHeight", "overlapWidth", "splitType"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }

    overlapHeight_ = *std::static_pointer_cast<int>(configParamMap["overlapHeight"]);
    overlapWidth_ = *std::static_pointer_cast<int>(configParamMap["overlapWidth"]);
    std::string splitType = (*std::static_pointer_cast<std::string>(configParamMap["splitType"]));
    LogInfo << "The splitType_ is " << splitType;
    if (SPLITTYPE.find(splitType) == SPLITTYPE.end()) {
        LogWarn << "Unknown split mode, using the default mode (Size_Block) instead.";
        splitType_ = Type::SIZE_BLOCK;
    } else {
        splitType_ = static_cast<Type>(SPLITTYPE[splitType]);
    }
    ret = JudgeUsage();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = JudgeSplitType(configParamMap);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    LogInfo << "End to initialize MxpiRoiGenerator(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiRoiGenerator::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiRoiGenerator(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiRoiGenerator(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiRoiGenerator::GetImageSize(MxpiBuffer& buffer)
{
    MxpiFrame inputMxpiFrame = MxpiBufferManager::GetHostDataInfo(buffer);
    if (inputMxpiFrame.visionlist().visionvec_size() == 0) {
        LogError << "Buffer is empty." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return APP_ERR_COMM_OUT_OF_RANGE;
    }

    // Get the image height and width.
    MxpiVision bufferVision = inputMxpiFrame.visionlist().visionvec(0);
    MxpiVisionInfo buffInfo = bufferVision.visioninfo();
    imageHeight_ = static_cast<int>(buffInfo.height());
    imageWidth_ = static_cast<int>(buffInfo.width());
    LogDebug << "(" << elementName_ << ") The height of original image is: " << imageHeight_
             << ". The width of original image is : " << imageWidth_;
    if (blockHeight_ > imageHeight_) {
        LogWarn << "(" << elementName_ << ") The block height(" << blockHeight_
                << ") is greater than the image height(" << imageHeight_ << ")."
                << " Set the block height equal to image height";
        blockHeight_ = imageHeight_;
    }
    if (blockWidth_ > imageWidth_) {
        LogWarn << "(" << elementName_ << ") The block width(" << blockWidth_
                << ") is greater than the image width(" << imageWidth_ << ")."
                << " Set the block width equal to image width";
        blockWidth_ = imageWidth_;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiRoiGenerator::AddMetadataAndSendBuff(MxpiBuffer &buffer,
    MxpiObjectList &cropObjectList, MxpiObjectList &mergeObjectList)
{
    APP_ERROR ret = APP_ERR_OK;
    auto cropObjectListPtr = MemoryHelper::MakeShared<MxpiObjectList>(cropObjectList);
    if (cropObjectListPtr == nullptr) {
        errorInfo_ << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    if (srcPadNum_ > 1) {
        gst_buffer_ref((GstBuffer *)buffer.buffer);
        auto tmpBuffer = new (std::nothrow) MxpiBuffer {buffer.buffer, nullptr};
        if (tmpBuffer == nullptr) {
            errorInfo_ << "Fail to create temporary buffer." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        MxpiMetadataManager manager(*tmpBuffer);
        auto mergeObjectListPtr = MemoryHelper::MakeShared<MxpiObjectList>(mergeObjectList);
        if (mergeObjectListPtr == nullptr) {
            delete tmpBuffer;
            errorInfo_ << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        LogDebug << "(" << elementName_ << ") Begin to add (" << outputDataKeys_[1] << ") proto metadata";
        ret = manager.AddProtoMetadata(outputDataKeys_[1], mergeObjectListPtr);
        if (ret != APP_ERR_OK) {
            delete tmpBuffer;
            errorInfo_ << "Fail to add proto metadata." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
        LogDebug << "(" << elementName_ << ") Begin Send (" << outputDataKeys_[1] << ") metadata";
        SendData(1, *tmpBuffer);
    }
    MxpiMetadataManager manager(buffer);
    LogDebug << "(" << elementName_ << ") Begin to add (" << outputDataKeys_[0] << ") proto metadata";
    ret = manager.AddProtoMetadata(outputDataKeys_[0], cropObjectListPtr);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to add proto metadata." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    LogDebug << "(" << elementName_ << ") Begin Send (" << outputDataKeys_[0] << ") metadata";
    SendData(0, buffer);
    return APP_ERR_OK;
}

APP_ERROR MxpiRoiGenerator::CheckRoiWithImageSize()
{
    for (size_t i = 0; i < cropRoiVec_.size(); i++) {
        if (cropRoiVec_[i].x1 > imageWidth_ || cropRoiVec_[i].y1 > imageHeight_) {
            LogWarn << "element(" << elementName_ << "). No." << i << " cropRoi is out of image size."
                    << "crop roi: x0 = " << cropRoiVec_[i].x0 << ", y0 = " << cropRoiVec_[i].y0
                    << ", x1 = " << cropRoiVec_[i].x1 << ", y1 = " << cropRoiVec_[i].y1 << ".\n"
                    << "imageWidth = " << imageWidth_ << " imageHeight = " << imageHeight_;
        }
        if (mergeRoiVec_[i].x1 > imageWidth_ || mergeRoiVec_[i].y1 > imageHeight_) {
            LogError << "Element(" << elementName_ << "). No." << i << " mergeRoi is out of image size."
                    << "merge roi: x0 = " << mergeRoiVec_[i].x0 << ", y0 = " << mergeRoiVec_[i].y0
                    << ", x1 = " << mergeRoiVec_[i].x1 << ", y1 = " << mergeRoiVec_[i].y1 << ".\n"
                    << "imageWidth = " << imageWidth_ << " imageHeight = " << imageHeight_
                    << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiRoiGenerator::CustomProcess(std::vector<MxpiBuffer*> &mxpiBuffer)
{
    MxpiBuffer* buffer = mxpiBuffer[0];
    APP_ERROR ret = GetImageSize(*buffer);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Failed to get the image size." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        return ret;
    }
    ret = CheckRoiWithImageSize();
    if (ret != APP_ERR_OK) {
        errorInfo_ << "MergeRoi box is out of image size." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        return ret;
    }
    // set the MxpiObjectList for image crop and merge.
    MxpiObjectList cropObjectList = RoiVecToObjectList(cropRoiVec_);
    MxpiObjectList mergeObjectList = RoiVecToObjectList(mergeRoiVec_);
    // Add Proto Metadata and Send Buff.
    ret = AddMetadataAndSendBuff(*buffer, cropObjectList, mergeObjectList);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << " Fail to add metadata and send buff." << GetErrorInfo(ret);
        SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    LogDebug << "End the Block process.";
    return APP_ERR_OK;
}

APP_ERROR MxpiRoiGenerator::Process(std::vector<MxpiBuffer*> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiRoiGenerator(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    if (splitType_ == Type::CUSTOM) {
        return CustomProcess(mxpiBuffer);
    }
    MxpiBuffer* buffer = mxpiBuffer[0];
    // Get the image size.
    ret = GetImageSize(*buffer);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Failed to get the image size." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*mxpiBuffer[0], elementName_, ret, errorInfo_.str());
        return ret;
    }
    // Set the base information of the split method.
    SplitInfoBox splitInfoBox {
        imageWidth_, imageHeight_, blockWidth_, blockHeight_,
        chessboardWidth_, chessboardHeight_, overlapWidth_, overlapHeight_
    };
    // Get the crop information and the merge roi information vector.
    std::vector<MxBase::CropRoiBox> cropRoiVec;
    std::vector<MxBase::CropRoiBox> mergeRoiVec;
    ret = MxBase::SplitterUtils::GetCropAndMergeRoiInfo(splitInfoBox, splitType_,
                                                        cropRoiVec, mergeRoiVec, needMergeRoi_);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to get the and crop and merge roi." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    // set the MxpiObjectList for image crop and merge.
    MxpiObjectList cropObjectList = RoiVecToObjectList(cropRoiVec);
    MxpiObjectList mergeObjectList = RoiVecToObjectList(mergeRoiVec);
    // Add Proto Metadata and Send Buff.
    ret = AddMetadataAndSendBuff(*buffer, cropObjectList, mergeObjectList);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << " Fail to add metadata and send buff." << GetErrorInfo(ret);
        SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    LogDebug << "End the Block process.";
    return APP_ERR_OK;
}

MxpiPortInfo MxpiRoiGenerator::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/yuv"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiRoiGenerator::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/object"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    value = {{"metadata/object/roi-info"}};
    GenerateDynamicOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

std::vector<std::shared_ptr<void>> MxpiRoiGenerator::DefineProperties()
{
    auto block_height = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "blockHeight", "block_height", "the height of block image", 512, 32, 8192
    });
    auto block_width = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "blockWidth", "block_width", "the width of block image", 512, 32, 8192
    });
    auto overlap_height = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "overlapHeight", "overlap_height", "the height of overlap area", 0, 0, 8192
    });
    auto overlap_width = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "overlapWidth", "overlap_width", "the width of overlap image", 0, 0, 8192
    });
    auto chessboard_height = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "chessboardHeight", "chessboard_height", "the height of chessboard", 1, 1, 256
    });
    auto chessboard_width = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "chessboardWidth", "overlap_width", "the width of chessboard", 1, 1, 256
    });
    auto split_type = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "splitType", "split_type", "the type of split method", "Size_Block", "", ""
    });
    auto crop_roi = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "cropRoi", "crop_roi", "the crop roi for user defined", "", "", ""
    });
    auto merge_roi = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
            STRING, "mergeRoi", "merge_roi", "the crop roi for user defined", "", "", ""
    });
    std::vector<std::shared_ptr<void>> properties = {
        block_height, block_width, overlap_height, overlap_width,
        chessboard_height, chessboard_width, split_type, crop_roi, merge_roi
    };
    return properties;
}

namespace {
MX_PLUGIN_GENERATE(MxpiRoiGenerator)
}

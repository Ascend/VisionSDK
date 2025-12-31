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
 * Description: Converts the data structure used by model post-processing to the protobuf data structure.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeConverter.h"

namespace {
template<typename T>
std::string VVector2String(const std::vector<std::vector<T>> &dataSrc)
{
    std::string result;
    if (dataSrc.size() > 0 && dataSrc[0].size() > 0) {
        for (auto &data : dataSrc) {
            result += std::string((char *)data.data(), data.size() * sizeof(T));
        }
    } else {
        result = "";
    }
    return result;
}
}

namespace MxTools {
std::shared_ptr<MxTools::MxpiObjectList> ConstructProtobuf(
    const std::vector<std::vector<MxBase::ObjectInfo>> &objectInfos, std::string dataSource)
{
    std::shared_ptr<MxTools::MxpiObjectList> mxpiObjectList =
        MxBase::MemoryHelper::MakeShared<MxTools::MxpiObjectList>();
    if (mxpiObjectList == nullptr) {
        LogError << "Create MxpiObjectList object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    for (size_t i = 0; i < objectInfos.size(); i++) {
        for (size_t j = 0; j < objectInfos[i].size(); j++) {
            MxTools::MxpiObject *objectData = mxpiObjectList->add_objectvec();
            if (objectData == nullptr) {
                LogError << "objectData is nullptr. Create MxpiObjectList object failed."
                        << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return nullptr;
            }
            objectData->set_x0(objectInfos[i][j].x0);
            objectData->set_y0(objectInfos[i][j].y0);
            objectData->set_x1(objectInfos[i][j].x1);
            objectData->set_y1(objectInfos[i][j].y1);
            MxTools::MxpiClass *classInfo = objectData->add_classvec();
            if (classInfo == nullptr) {
                LogError << "classInfo is nullptr. Create MxpiObjectList object failed."
                        << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return nullptr;
            }
            classInfo->set_classid(objectInfos[i][j].classId);
            classInfo->set_confidence(objectInfos[i][j].confidence);
            classInfo->set_classname(objectInfos[i][j].className);
            MxpiMetaHeader *mxpiMetaHeader = objectData->add_headervec();
            if (mxpiMetaHeader == nullptr) {
                LogError << "mxpiMetaHeader is nullptr. Create MxpiObjectList object failed."
                        << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return nullptr;
            }
            mxpiMetaHeader->set_memberid(i);
            mxpiMetaHeader->set_datasource(dataSource);
            if (objectInfos[i][j].mask.empty()) {
                continue;
            }
            // instance segmentation only support uint8;
            MxTools::MxpiImageMask *mxpiImageMask = objectData->mutable_imagemask();
            mxpiImageMask->set_datatype(MxBase::TENSOR_DTYPE_UINT8);
            std::vector<std::vector<uint8_t>> pixelsSimplified;
            for (auto& row : objectInfos[i][j].mask) {
                std::vector<uint8_t> rowSimplified;
                rowSimplified.assign(row.begin(), row.end());
                pixelsSimplified.push_back(rowSimplified);
            }
            std::string result = VVector2String(pixelsSimplified);
            mxpiImageMask->set_datatype(MxBase::TENSOR_DTYPE_UINT8);
            mxpiImageMask->add_shape(objectInfos[i][j].mask.size());
            if (objectInfos[i][j].mask.size() > 0) {
                mxpiImageMask->add_shape(objectInfos[i][j].mask[0].size());
            }
            mxpiImageMask->set_datastr(result);
        }
    }
    return mxpiObjectList;
}

std::shared_ptr<MxTools::MxpiClassList> ConstructProtobuf(const std::vector<std::vector<MxBase::ClassInfo>>& classInfos,
                                                          std::string dataSource)
{
    std::shared_ptr<MxTools::MxpiClassList> mxpiClassList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiClassList>();
    if (mxpiClassList == nullptr) {
        LogError << "Create MxpiClassList object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    for (size_t i = 0; i < classInfos.size(); i++) {
        for (size_t j = 0; j < classInfos[i].size(); j++) {
            MxpiClass *mxpiClass = mxpiClassList->add_classvec();
            if (mxpiClass == nullptr) {
                LogError << "mxpiClass is nullptr. Create MxpiClassList object failed."
                        << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return nullptr;
            }
            mxpiClass->set_classid(classInfos[i][j].classId);
            mxpiClass->set_classname(classInfos[i][j].className);
            mxpiClass->set_confidence(classInfos[i][j].confidence);
            MxpiMetaHeader *mxpiMetaHeader = mxpiClass->add_headervec();
            if (mxpiMetaHeader == nullptr) {
                LogError << "mxpiMetaHeader is nullptr. Create MxpiClassList object failed."
                        << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return nullptr;
            }
            mxpiMetaHeader->set_memberid(i);
            mxpiMetaHeader->set_datasource(dataSource);
        }
    }
    return mxpiClassList;
}

std::shared_ptr<MxTools::MxpiImageMaskList> ConstructProtobuf(
    const std::vector<MxBase::SemanticSegInfo> &semanticSegInfos, std::string dataSource)
{
    std::shared_ptr<MxTools::MxpiImageMaskList> mxpiImageMaskList =
        MxBase::MemoryHelper::MakeShared<MxTools::MxpiImageMaskList>();
    if (mxpiImageMaskList == nullptr) {
        LogError << "Create MxpiImageMaskList object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    for (size_t i = 0; i < semanticSegInfos.size(); i++) {
        MxTools::MxpiImageMask *mxpiImageMask = mxpiImageMaskList->add_imagemaskvec();
        if (mxpiImageMask == nullptr) {
                LogError << "mxpiImageMask is nullptr. Create MxpiImageMaskList object failed."
                        << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return nullptr;
        }
        for (size_t k = 0; k < semanticSegInfos[i].labelMap.size(); k++) {
            mxpiImageMask->add_classname(semanticSegInfos[i].labelMap[k]);
        }
        // if classNum < 255, use uint instead of int32.
        std::string result;
        if (semanticSegInfos[i].labelMap.size() <= UINT8_MAX) {
            std::vector<std::vector<uint8_t>> pixelsSimplified;
            for (auto& row : semanticSegInfos[i].pixels) {
                std::vector<uint8_t> rowSimplified;
                rowSimplified.assign(row.begin(), row.end());
                pixelsSimplified.push_back(rowSimplified);
            }
            result = VVector2String(pixelsSimplified);
            mxpiImageMask->set_datatype(MxBase::TENSOR_DTYPE_UINT8);
        } else {
            result = VVector2String(semanticSegInfos[i].pixels);
            mxpiImageMask->set_datatype(MxBase::TENSOR_DTYPE_INT32);
        }
        mxpiImageMask->add_shape(semanticSegInfos[i].pixels.size());
        if (semanticSegInfos[i].pixels.size() > 0) {
            mxpiImageMask->add_shape(semanticSegInfos[i].pixels[0].size());
        }
        mxpiImageMask->set_datastr(result);
        MxTools::MxpiMetaHeader *mxpiMetaHeader = mxpiImageMask->add_headervec();
        if (mxpiMetaHeader == nullptr) {
                LogError << "mxpiMetaHeader is nullptr. Create MxpiImageMaskList object failed."
                        << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return nullptr;
        }
        mxpiMetaHeader->set_memberid(i);
        mxpiMetaHeader->set_datasource(dataSource);
    }
    return mxpiImageMaskList;
}

std::shared_ptr<MxTools::MxpiTextsInfoList> ConstructProtobuf(const std::vector<MxBase::TextsInfo>& textsInfo,
                                                              std::string dataSource)
{
    std::shared_ptr<MxTools::MxpiTextsInfoList> mxpiTextsInfoList =
        MxBase::MemoryHelper::MakeShared<MxTools::MxpiTextsInfoList>();
    if (mxpiTextsInfoList == nullptr) {
        LogError << "Create MxpiTextsInfoList object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    for (size_t i = 0; i < textsInfo.size(); i++) {
        MxTools::MxpiTextsInfo *mxpiTextsInfo = mxpiTextsInfoList->add_textsinfovec();
        if (mxpiTextsInfo == nullptr) {
                LogError << "mxpiTextsInfo is nullptr. Create MxpiTextsInfoList object failed."
                        << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return nullptr;
        }
        for (size_t j = 0; j < textsInfo[i].text.size(); j++) {
            mxpiTextsInfo->add_text(textsInfo[i].text[j]);
        }
        MxpiMetaHeader *mxpiMetaHeader = mxpiTextsInfo->add_headervec();
        if (mxpiMetaHeader == nullptr) {
            LogError << "mxpiMetaHeader is nullptr. Create MxpiTextsInfoList object failed."
                    << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return nullptr;
        }
        mxpiMetaHeader->set_memberid(i);
        mxpiMetaHeader->set_datasource(dataSource);
    }
    return mxpiTextsInfoList;
}

std::shared_ptr<MxTools::MxpiTextObjectList> ConstructProtobuf(
    const std::vector<std::vector<MxBase::TextObjectInfo>>& textObjectInfos, std::string dataSource)
{
    std::shared_ptr<MxTools::MxpiTextObjectList> mxpiTextObjectList =
        MxBase::MemoryHelper::MakeShared<MxTools::MxpiTextObjectList>();
    if (mxpiTextObjectList == nullptr) {
        LogError << "Create MxpiTextObjectList object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    for (size_t i = 0; i < textObjectInfos.size(); i++) {
        for (size_t j = 0; j < textObjectInfos[i].size(); j++) {
            MxTools::MxpiTextObject *mxpiTextObject = mxpiTextObjectList->add_objectvec();
            if (mxpiTextObject == nullptr) {
                LogError << "mxpiTextObject is nullptr. Create MxpiTextObjectList object failed."
                        << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return nullptr;
            }
            mxpiTextObject->set_x0(textObjectInfos[i][j].x0);
            mxpiTextObject->set_y0(textObjectInfos[i][j].y0);
            mxpiTextObject->set_x1(textObjectInfos[i][j].x1);
            mxpiTextObject->set_y1(textObjectInfos[i][j].y1);
            mxpiTextObject->set_x2(textObjectInfos[i][j].x2);
            mxpiTextObject->set_y2(textObjectInfos[i][j].y2);
            mxpiTextObject->set_x3(textObjectInfos[i][j].x3);
            mxpiTextObject->set_y3(textObjectInfos[i][j].y3);
            mxpiTextObject->set_confidence(textObjectInfos[i][j].confidence);
            if (!textObjectInfos[i][j].result.empty()) {
                mxpiTextObject->set_text(textObjectInfos[i][j].result);
            }
            MxpiMetaHeader *mxpiMetaHeader = mxpiTextObject->add_headervec();
            if (mxpiMetaHeader == nullptr) {
                LogError << "mxpiMetaHeader is nullptr. Create MxpiTextObjectList object failed."
                        << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return nullptr;
            }
            mxpiMetaHeader->set_memberid(i);
            mxpiMetaHeader->set_datasource(dataSource);
        }
    }
    return mxpiTextObjectList;
}

std::shared_ptr<MxTools::MxpiPoseList> ConstructProtobuf(
    const std::vector<std::vector<MxBase::KeyPointDetectionInfo>>& keyPointInfos, std::string dataSource)
{
    std::shared_ptr<MxTools::MxpiPoseList> mxpiPoseList = MxBase::MemoryHelper::MakeShared<MxTools::MxpiPoseList>();
    if (mxpiPoseList == nullptr) {
        LogError << "Create MxpiPoseList object failed. Failed to allocate memory."
                 << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    for (size_t i = 0; i < keyPointInfos.size(); i++) {
        for (size_t j = 0; j < keyPointInfos[i].size(); j++) {
            MxTools::MxpiPose *mxpiPose = mxpiPoseList->add_posevec();
            if (mxpiPose == nullptr) {
                LogError << "Create mxpiPose object failed. Create MxpiPoseList object failed."
                        << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return nullptr;
            }
            std::map<int, std::vector<float>>::const_iterator iter;
            for (iter = keyPointInfos[i][j].keyPointMap.begin();
                iter != keyPointInfos[i][j].keyPointMap.end(); iter++) {
                MxTools::MxpiKeyPoint *mxpiKeyPoint = mxpiPose->add_keypointvec();
                if (mxpiKeyPoint == nullptr) {
                    LogError << "Create mxpiKeyPoint object failed. Create MxpiPoseList object failed."
                            << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                    return nullptr;
                }
                auto coordinates = iter->second;
                const size_t two = 2;
                if (coordinates.size() != two) {
                    LogError << "coordinates size is not 2."
                             << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return nullptr;
                }
                mxpiKeyPoint->set_x(coordinates[0]);
                mxpiKeyPoint->set_y(coordinates[1]);
                mxpiKeyPoint->set_name(iter->first);
                mxpiKeyPoint->set_score(keyPointInfos[i][j].scoreMap.find(iter->first)->second);
            }
            mxpiPose->set_score(keyPointInfos[i][j].score);
            MxpiMetaHeader *mxpiMetaHeader = mxpiPose->add_headervec();
            if (mxpiMetaHeader == nullptr) {
                LogError << "Create mxpiMetaHeader object failed. Create MxpiPoseList object failed."
                        << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return nullptr;
            }
            mxpiMetaHeader->set_memberid(i);
            mxpiMetaHeader->set_datasource(dataSource);
        }
    }
    return mxpiPoseList;
}

void StackMxpiVisionPreProcess(MxTools::MxpiVisionInfo &dstMxpiVisionInfo,
                               const MxTools::MxpiVisionInfo &srcMxpiVisionInfo,
                               const MxBase::CropResizePasteConfig &cropResizePasteConfig,
                               const std::string& elementName)
{
    auto preprocessinfos = dstMxpiVisionInfo.mutable_preprocessinfo();
    preprocessinfos->CopyFrom(srcMxpiVisionInfo.preprocessinfo());
    MxTools::MxpiVisionPreProcess* preprocessInfo = preprocessinfos->Add();
    preprocessInfo->set_widthsrc(srcMxpiVisionInfo.width());
    preprocessInfo->set_heightsrc(srcMxpiVisionInfo.height());
    preprocessInfo->set_cropleft(cropResizePasteConfig.cropLeft);
    preprocessInfo->set_cropright(cropResizePasteConfig.cropRight);
    preprocessInfo->set_croptop(cropResizePasteConfig.cropTop);
    preprocessInfo->set_cropbottom(cropResizePasteConfig.cropBottom);
    preprocessInfo->set_pasteleft(cropResizePasteConfig.pasteLeft);
    preprocessInfo->set_pasteright(cropResizePasteConfig.pasteRight);
    preprocessInfo->set_pastetop(cropResizePasteConfig.pasteTop);
    preprocessInfo->set_pastebottom(cropResizePasteConfig.pasteBottom);
    preprocessInfo->set_interpolation(cropResizePasteConfig.interpolation);
    preprocessInfo->set_interpolation(cropResizePasteConfig.interpolation);
    preprocessInfo->set_elementname(elementName);
}

APP_ERROR ConstrutImagePreProcessInfo(MxTools::MxpiVisionInfo& visionInfo, MxBase::ImagePreProcessInfo& positionInfo)
{
    APP_ERROR ret = APP_ERR_OK;
    auto preInfo = visionInfo.preprocessinfo();
    LogDebug << "start to calculate transformation after ("
             << visionInfo.preprocessinfo_size() << ") preprocessors.";
    positionInfo.imageWidth = visionInfo.width();
    positionInfo.imageHeight = visionInfo.height();
    positionInfo.originalWidth = preInfo.empty() ? positionInfo.imageWidth : preInfo[0].widthsrc();
    positionInfo.originalHeight = preInfo.empty() ? positionInfo.imageHeight : preInfo[0].heightsrc();
    positionInfo.x1Valid = positionInfo.originalWidth;
    positionInfo.y1Valid = positionInfo.originalHeight;
    for (int j = 0; j < preInfo.size(); j++) {
        auto info = visionInfo.preprocessinfo(j);
        if ((info.cropright() <= info.cropleft()) || (info.cropbottom() <= info.croptop()) ||
            (info.pasteright() <= info.pasteleft()) || (info.pastebottom() <= info.pastetop())) {
            ret = APP_ERR_COMM_INVALID_PARAM;
            LogError << "invalid ImagePreProcessInfo. cropRight/cropBottom must be larger than cropLeft/cropTop."
                     << "pasteRight/pasteBottom must be larger than pasteLeft/pasteTop." << GetErrorInfo(ret);
            return ret;
        }
        auto a1 = (info.pasteright() - info.pasteleft()) / float(info.cropright() - info.cropleft());
        auto b1 = (signed) info.pasteleft() - (signed) info.cropleft();
        auto a2 = (info.pastebottom() - info.pastetop()) / float(info.cropbottom() - info.croptop());
        auto b2 = (signed) info.pastetop() - (signed) info.croptop();
        positionInfo.xRatio = a1 * positionInfo.xRatio;
        positionInfo.xBias = a1 * positionInfo.xBias + b1;
        positionInfo.yRatio = a2 * positionInfo.yRatio;
        positionInfo.yBias = a2 * positionInfo.yBias + b2;
        positionInfo.x0Valid = a1 * std::max((float)info.cropleft(), positionInfo.x0Valid) + b1;
        positionInfo.y0Valid = a2 * std::max((float)info.croptop(), positionInfo.y0Valid) + b2;
        positionInfo.x1Valid = a1 * std::min((float)info.cropright(), positionInfo.x1Valid) + b1;
        positionInfo.y1Valid = a2 * std::min((float)info.cropbottom(), positionInfo.y1Valid) + b2;
    }
    return APP_ERR_OK;
}
}
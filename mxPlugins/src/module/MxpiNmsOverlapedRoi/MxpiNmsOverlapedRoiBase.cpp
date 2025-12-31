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
 * Description: Used to filter duplicate targets in the overlapping area after partitioning and adjust ports.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"
#include "MxPlugins/MxpiNmsOverlapedRoi/MxpiNmsOverlapedRoiBase.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
    const float CENTER_OFFSET_DIVISOR = 2.f;
}

namespace MxPlugins {
APP_ERROR MxpiNmsOverlapedRoiBase::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to process (" << elementName_ << ") init.";
    previousPluginName_ = dataSource_;
    std::vector<std::string> parameterNamesPtr = {"blockName", "nmsThreshold"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    blockPluginName_ = *std::static_pointer_cast<std::string>(configParamMap["blockName"]);
    nmsValue_ = *std::static_pointer_cast<float>(configParamMap["nmsThreshold"]);
    LogInfo << "End to process (" << elementName_ << ") init.";
    return APP_ERR_OK;
}

APP_ERROR MxpiNmsOverlapedRoiBase::DeInit()
{
    LogInfo << "Begin to process (" << elementName_ << ") deinit.";
    mxpiObjectMap_.clear();
    LogInfo << "End to process (" << elementName_ << ") deinit.";
    return APP_ERR_OK;
}

APP_ERROR MxpiNmsOverlapedRoiBase::Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process (" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    errorInfo_.str("");
    // resize image info
    MxTools::MxpiBuffer *buffer = mxpiBuffer[0];
    MxTools::MxpiMetadataManager mxpiMetadataManager(*buffer);

    if (blockPluginName_ == previousPluginName_) {
        LogWarn << "The parentName [" << previousPluginName_ << "] and the blockName [" <<
                blockPluginName_ << "] are the same";
        SendData(0, *buffer);
        return APP_ERR_OK;
    }
    if (mxpiMetadataManager.GetMetadata(blockPluginName_) == nullptr ||
        mxpiMetadataManager.GetMetadata(previousPluginName_) == nullptr) {
        LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << " metadata is null.";
        SendData(0, *buffer);
        return APP_ERR_MXPLUGINS_METADATA_IS_NULL;
    }
    ret = GetBlockDataInfo(*buffer);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        SendMxpiErrorInfo(*buffer, elementName_, ret, errorInfo_.str());
        return ret;
    }
    LogDebug << "End to process  (" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiNmsOverlapedRoiBase::GetBlockDataInfo(MxTools::MxpiBuffer &buffer)
{
    MxTools::MxpiMetadataManager mxpiMetadataManager(buffer);
    // check if data structure is target.
    auto blockMetadata = mxpiMetadataManager.GetMetadataWithType(blockPluginName_, "MxpiObjectList");
    if (blockMetadata == nullptr) {
        errorInfo_ << "Not MxpiObjectList object." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    auto objectMetadata = mxpiMetadataManager.GetMetadataWithType(previousPluginName_, "MxpiObjectList");
    if (objectMetadata == nullptr) {
        errorInfo_ << "Not MxpiObjectList object." << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
        LogError << errorInfo_.str();
        return APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    }
    auto mxpiObjectList =
            std::static_pointer_cast<MxTools::MxpiObjectList>(mxpiMetadataManager.GetMetadata(blockPluginName_));
    auto mxpiObjectListPre =
            std::static_pointer_cast<MxTools::MxpiObjectList>(mxpiMetadataManager.GetMetadata(previousPluginName_));
    if (mxpiObjectList == nullptr || mxpiObjectListPre == nullptr) {
        LogError << "Fail to get meta data." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    CompareBlockObject(mxpiObjectList);
    auto metaDataPtr = MemoryHelper::MakeShared<MxTools::MxpiObjectList>();
    if (metaDataPtr == nullptr) {
        errorInfo_ << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_ALLOC_MEM;
    }
    APP_ERROR ret =  FilterRepeatObject(mxpiObjectListPre, metaDataPtr);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Filter repeat object failed when getting block DataInfo." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    ret = mxpiMetadataManager.AddProtoMetadata(elementName_, metaDataPtr);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Add proto metadata failed when getting block DataInfo." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    SendData(0, buffer);
    return APP_ERR_OK;
}

void MxpiNmsOverlapedRoiBase::CompareBlockObject(std::shared_ptr<MxTools::MxpiObjectList> mxpiObjectList)
{
    std::map<int, MxTools::MxpiObject> mxpiObject;
    for (int i = 0; i < mxpiObjectList->objectvec_size(); i++) {
        mxpiObject[i] = mxpiObjectList->objectvec(i);
    }
    if (mxpiObjectMap_.size() == mxpiObject.size()) {
        for (size_t i = 0; i < mxpiObjectMap_.size(); i++) {
            if (!IsIntersecting(mxpiObject, i)) {
                mxpiObjectMap_.clear();
                mxpiObjectMap_.insert(mxpiObject.begin(), mxpiObject.end());
                CalcEffectiveArea();
            }
        }
    } else {
        mxpiObjectMap_.clear();
        mxpiObjectMap_.insert(mxpiObject.begin(), mxpiObject.end());
        CalcEffectiveArea();
    }
}

void MxpiNmsOverlapedRoiBase::CalcEffectiveArea()
{
    for (size_t i = 0; i < mxpiObjectMap_.size(); i++) {
        for (size_t j = i + 1; j < mxpiObjectMap_.size(); j++) {
            if (IsDenominatorZero(mxpiObjectMap_[i].y0() - mxpiObjectMap_[j].y0())) {
                CalcAreaOffsetX(i, j);
            }
            if (IsDenominatorZero(mxpiObjectMap_[i].x0() - mxpiObjectMap_[j].x0())) {
                CalcAreaOffsetY(i, j);
            }
        }
    }
}

void MxpiNmsOverlapedRoiBase::CalcAreaOffsetY(int i, int j)
{
    if (mxpiObjectMap_[i].y1() > mxpiObjectMap_[j].y0() &&
        mxpiObjectMap_[j].y0() > mxpiObjectMap_[i].y0()) {
        float offsetY = mxpiObjectMap_[i].y1() + mxpiObjectMap_[j].y0();
        mxpiObjectMap_[i].set_y1(offsetY / CENTER_OFFSET_DIVISOR);
        mxpiObjectMap_[j].set_y0(offsetY / CENTER_OFFSET_DIVISOR);
    } else if (mxpiObjectMap_[j].y1() > mxpiObjectMap_[i].y0() &&
               mxpiObjectMap_[i].y0() > mxpiObjectMap_[j].y0()) {
        float offsetY = mxpiObjectMap_[j].y1() + mxpiObjectMap_[i].y0();
        mxpiObjectMap_[j].set_y1(offsetY / CENTER_OFFSET_DIVISOR);
        mxpiObjectMap_[i].set_y0(offsetY / CENTER_OFFSET_DIVISOR);
    }
}

void MxpiNmsOverlapedRoiBase::CalcAreaOffsetX(int i, int j)
{
    if (mxpiObjectMap_[i].x1() > mxpiObjectMap_[j].x0() &&
        mxpiObjectMap_[j].x0() > mxpiObjectMap_[i].x0()) {
        float offsetX = mxpiObjectMap_[i].x1() + mxpiObjectMap_[j].x0();
        mxpiObjectMap_[i].set_x1(offsetX / CENTER_OFFSET_DIVISOR);
        mxpiObjectMap_[j].set_x0(offsetX / CENTER_OFFSET_DIVISOR);
    } else if (mxpiObjectMap_[i].x0() > mxpiObjectMap_[j].x0() &&
               mxpiObjectMap_[j].x1() > mxpiObjectMap_[i].x0()) {
        float offsetX = mxpiObjectMap_[i].x0() + mxpiObjectMap_[j].x1();
        mxpiObjectMap_[i].set_x0(offsetX / CENTER_OFFSET_DIVISOR);
        mxpiObjectMap_[j].set_x1(offsetX / CENTER_OFFSET_DIVISOR);
    }
}

APP_ERROR MxpiNmsOverlapedRoiBase::FilterRepeatObject(std::shared_ptr<MxTools::MxpiObjectList> mxpiObjectList,
                                                      std::shared_ptr<MxTools::MxpiObjectList> metaDataPtr)
{
    std::vector<MxTools::MxpiObject> filterObjectVec;
    for (const auto &item : mxpiObjectList->objectvec()) {
        if (!IsExceedingEffectiveArea(item)) {
            filterObjectVec.push_back(item);
        }
    }
    std::vector<MxBase::DetectBox> detectBoxes;
    ConvertObjectToDetectBox(detectBoxes, filterObjectVec);
    MxBase::NmsSortByArea(detectBoxes, nmsValue_, MxBase::IOUMethod::MIN);
    APP_ERROR ret = ConvertDetectBoxToObject(detectBoxes, metaDataPtr);
    if (ret != APP_ERR_OK) {
        LogError << "Convert detectBox to object failed"<<  GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

void MxpiNmsOverlapedRoiBase::ConvertObjectToDetectBox(std::vector<MxBase::DetectBox> &detectBoxes,
                                                       std::vector<MxTools::MxpiObject> filterObjectVec)
{
    for (const auto &object : filterObjectVec) {
        MxBase::DetectBox detectBox {};
        float width = object.x1() - object.x0();
        float height = object.y1() - object.y0();
        detectBox.x = object.x0() + width / CENTER_OFFSET_DIVISOR;
        detectBox.y = object.y0() + height / CENTER_OFFSET_DIVISOR;
        detectBox.width = width;
        detectBox.height = height;
        detectBox.classID = object.classvec(0).classid();
        detectBox.prob = object.classvec(0).confidence();
        detectBox.className = object.classvec(0).classname();
        detectBoxes.push_back(detectBox);
    }
}

APP_ERROR MxpiNmsOverlapedRoiBase::ConvertDetectBoxToObject(const std::vector<MxBase::DetectBox> &detectBoxes,
                                                            std::shared_ptr<MxTools::MxpiObjectList> metaDataPtr)
{
    for (size_t i = 0; i < detectBoxes.size(); i++) {
        float x0 = detectBoxes[i].x - detectBoxes[i].width / CENTER_OFFSET_DIVISOR;
        float x1 = detectBoxes[i].x + detectBoxes[i].width / CENTER_OFFSET_DIVISOR;
        float y0 = detectBoxes[i].y - detectBoxes[i].height / CENTER_OFFSET_DIVISOR;
        float y1 = detectBoxes[i].y + detectBoxes[i].height / CENTER_OFFSET_DIVISOR;
        MxTools::MxpiObject *objectData = metaDataPtr->add_objectvec();
        if (CheckPtrIsNullptr(objectData, "objectData"))  return APP_ERR_COMM_ALLOC_MEM;
        objectData->set_x0(x0);
        objectData->set_y0(y0);
        objectData->set_x1(x1);
        objectData->set_y1(y1);
        MxTools::MxpiClass *classInfo = objectData->add_classvec();
        if (CheckPtrIsNullptr(classInfo, "classInfo"))  return APP_ERR_COMM_ALLOC_MEM;
        classInfo->set_classid(detectBoxes[i].classID);
        classInfo->set_confidence(detectBoxes[i].prob);
        classInfo->set_classname(detectBoxes[i].className);
        MxTools::MxpiMetaHeader *header = objectData->add_headervec();
        if (CheckPtrIsNullptr(header, "header"))  return APP_ERR_COMM_ALLOC_MEM;
        header->set_datasource(previousPluginName_);
        header->set_memberid(i);
    }
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiNmsOverlapedRoiBase::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto blockName = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "blockName", "name", "the key of block data", "", "", ""
    });
    auto nmsValue = std::make_shared<ElementProperty<float>>(ElementProperty<float> {
        FLOAT, "nmsThreshold", "nms", "the threshold of nms", 0.45f, 0.f, 1.f
    });

    properties.push_back(blockName);
    properties.push_back(nmsValue);
    return properties;
}

MxTools::MxpiPortInfo MxpiNmsOverlapedRoiBase::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/object"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxTools::MxpiPortInfo MxpiNmsOverlapedRoiBase::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/object"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

bool MxpiNmsOverlapedRoiBase::IsExceedingEffectiveArea(const MxTools::MxpiObject &object)
{
    if (object.headervec_size() == 0) {
        LogError << "Protobuf message vector is invalid." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return true;
    }
    return (object.x0() > mxpiObjectMap_[object.headervec(0).memberid()].x1() ||
            object.y0() > mxpiObjectMap_[object.headervec(0).memberid()].y1() ||
            object.x1() < mxpiObjectMap_[object.headervec(0).memberid()].x0() ||
            object.y1() < mxpiObjectMap_[object.headervec(0).memberid()].y0());
}

bool MxpiNmsOverlapedRoiBase::IsIntersecting(std::map<int, MxTools::MxpiObject> &mxpiObject, int index)
{
    return (IsDenominatorZero(mxpiObjectMap_[index].x0() - mxpiObject[index].x0()) &&
            IsDenominatorZero(mxpiObjectMap_[index].x1() - mxpiObject[index].x1()) &&
            IsDenominatorZero(mxpiObjectMap_[index].y0() - mxpiObject[index].y0()) &&
            IsDenominatorZero(mxpiObjectMap_[index].y1() - mxpiObject[index].y1()));
}
}
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
 * Description: Sends data of a specified class or channel to different ports.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiDistributor/MxpiDistributor.h"
#include "MxBase/Log/Log.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const std::string OBJECT_LIST_KEY = "MxpiObjectList";
const std::string CLASS_LIST_KEY = "MxpiClassList";
const std::string OBJECT_KEY = "MxpiObject";
const std::string CLASS_KEY = "MxpiClass";
const std::string PROPERTY_DEFAULT_VALUE = "";
const char LEVEL_FIRST_SPLIT_RULE = ',';
const char LEVEL_SECOND_SPLIT_RULE = '|';

bool IsElementInVector(std::vector<std::string> v, const std::string& e)
{
    return !(std::find(v.begin(), v.end(), e) == v.end());
}

std::vector<std::string> SplitWithRemoveBlank(std::string& str, const char rule)
{
    MxBase::StringUtils::Trim(str);
    std::vector<std::string> strVec = MxBase::StringUtils::Split(str, rule);
    for (size_t i = 0; i < strVec.size(); i++) {
        strVec[i] = MxBase::StringUtils::Trim(strVec[i]);
    }
    return strVec;
}

std::vector<std::vector<std::string>> SplitVecMember(std::vector<std::string>& strVec, const char rule)
{
    std::vector<std::vector<std::string>> res;
    res.resize(strVec.size());
    for (size_t i = 0; i < strVec.size(); i++) {
        std::vector<std::string> tmp = SplitWithRemoveBlank(strVec[i], rule);
        res[i] = tmp;
    }
    return res;
}
} // namespace

APP_ERROR MxpiDistributor::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiDistributor(" << elementName_ << ").";
    // get parameters from configParamMap.
    std::vector<std::string> parameterNamesPtr = {"channelIds", "classIds", "distributeAll"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    channelId_ = *std::static_pointer_cast<std::string>(configParamMap["channelIds"]);
    classId_ = *std::static_pointer_cast<std::string>(configParamMap["classIds"]);
    parentName_ = dataSource_;
    parentName_ = MxPluginsAutoDataSource(elementName_, 0, "dataSource_", parentName_, dataSourceKeys_);
    if (parentName_.empty()) {
        LogError << "The property dataSource is \"\"," << " please check element(" << elementName_
                 << ")'s previous element." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::string distributeAll = *std::static_pointer_cast<std::string>(configParamMap["distributeAll"]);
    ret = CheckProperty();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = SetDistributeAllMode(distributeAll);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    LogInfo << "End to initialize MxpiDistributor(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiDistributor::CheckProperty()
{
    // num of src pad can not less than one
    if (srcPadNum_ == 0) {
        LogError << "The number of output ports is zero. please check your pipeline."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    // property(classIds) & property(channelIds) can not set at the same time
    if ((channelId_ != PROPERTY_DEFAULT_VALUE) && (classId_ != PROPERTY_DEFAULT_VALUE)) {
        LogError << "We only support distribute buffer by class id or channel id. fist usage: set property(channelId),"
                 << "second usage: set property(classIds) and property(parentName)."
                 << " So you can't set property(channelIds) and property(classIds) at the same time."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    // property(classIds) & property(channelIds) cannot using default at the same time
    if ((channelId_ == PROPERTY_DEFAULT_VALUE) && (classId_ == PROPERTY_DEFAULT_VALUE)) {
        LogError << "We only support distribute buffer by class id or channel id,"
                 << "so you should set property(channelIds) or property(classId)."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    // property(classIds) & property(parentName) should set at the same time
    if ((classId_ != PROPERTY_DEFAULT_VALUE) && (parentName_ == PROPERTY_DEFAULT_VALUE)) {
        LogError << "You select distribute buffer by class id,"
                 << "so you should set property(classIds) and property(parentName)."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    // distribute buffer by channel id or class id
    if (channelId_ != PROPERTY_DEFAULT_VALUE) {
        LogInfo << "You select common usage, element(" << elementName_ << ") will distribute buffer by channel id.";
        firstLevelIdVec_ = SplitWithRemoveBlank(channelId_, LEVEL_FIRST_SPLIT_RULE);
        secondLevelIdVec_ = SplitVecMember(firstLevelIdVec_, LEVEL_SECOND_SPLIT_RULE);
        channelIdFlag_ = true;
    } else {
        LogInfo << "You select special usage, element(" << elementName_ << ") will distribute buffer by class id.";
        firstLevelIdVec_ = SplitWithRemoveBlank(classId_, LEVEL_FIRST_SPLIT_RULE);
        secondLevelIdVec_ = SplitVecMember(firstLevelIdVec_, LEVEL_SECOND_SPLIT_RULE);
        channelIdFlag_ = false;
    }
    // num of key should equal to num of input
    if (firstLevelIdVec_.size() != srcPadNum_) {
        LogError << "The number(" << srcPadNum_ << ") of output ports is not equal to the number("
                 << firstLevelIdVec_.size() << ") of key."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiDistributor::SetDistributeAllMode(const std::string& distributeAll)
{
    if (distributeAll == "yes") {
        distributeAll_ = true;
    } else if (distributeAll == "no") {
        distributeAll_ = false;
    } else {
        LogError << R"(Invalid config of distributeAll, supports "yes", "no" only.)"
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiDistributor::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiDistributor(" << elementName_ << ").";
    LogInfo << "End to deinitialize MxpiDistributor(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiDistributor::Process(std::vector<MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiDistributor(" << elementName_ << ").";
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    MxpiBuffer* inputMxpiBuffer = mxpiBuffer[0];
    MxTools::MxpiMetadataManager mxpiMetadataManager(*inputMxpiBuffer);
    // previous error handle
    if (mxpiMetadataManager.GetErrorInfo() != nullptr) {
        LogDebug << "Input data is invalid, element(" << elementName_ <<") plugin will not be executed rightly.";
        for (size_t i = 0; i < firstLevelIdVec_.size(); i++) {
            gst_buffer_ref((GstBuffer*) inputMxpiBuffer->buffer);
            auto* tmpBuffer = new (std::nothrow) MxpiBuffer {inputMxpiBuffer->buffer, nullptr};
            if (tmpBuffer == nullptr) {
                return MallocFailedHandle(inputMxpiBuffer);
            }
            SendData(i, *tmpBuffer);
        }
        MxpiBufferManager::DestroyBuffer(inputMxpiBuffer);
        return APP_ERR_OK;
    }
    if (channelIdFlag_) {
        ret = DistributeByChannelId(inputMxpiBuffer);
    } else {
        ret = DistributeByClassId(inputMxpiBuffer);
    }
    LogDebug << "End to process MxpiDistributor(" << elementName_ << ").";
    return ret;
}

std::vector<std::shared_ptr<void>> MxpiDistributor::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto channelId = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "channelIds", "channel id", "rule of distribute", PROPERTY_DEFAULT_VALUE, "NULL", "NULL"
    });
    auto classId = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "classIds", "class id", "rule of distribute", PROPERTY_DEFAULT_VALUE, "NULL", "NULL"
    });
    auto distributeAll = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "distributeAll", "distributeAll", "send or not send data if input has not target data", "no", "", ""
    });
    properties = { channelId, classId, distributeAll };
    return properties;
}

MxpiPortInfo MxpiDistributor::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiDistributor::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"ANY"}};
    GenerateDynamicOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

APP_ERROR MxpiDistributor::DistributeByChannelId(MxpiBuffer* mxpiBuffer)
{
    MxpiFrame inputMxpiFrame = MxpiBufferManager::GetDeviceDataInfo(*mxpiBuffer);
    uint32_t channelId = inputMxpiFrame.frameinfo().channelid();
    int idx = -1;
    for (size_t i = 0; i < firstLevelIdVec_.size(); i++) {
        if (IsElementInVector(secondLevelIdVec_[i], std::to_string(channelId))) {
            idx = static_cast<int>(i);
            break;
        }
    }
    if (idx >= 0) {
        MxpiMetadataManager manager(*mxpiBuffer);
        auto lastMetadata = manager.GetMetadata(parentName_);
        if (lastMetadata != nullptr) {
            APP_ERROR ret = manager.AddProtoMetadata(outputDataKeys_[idx], lastMetadata);
            if (ret != APP_ERR_OK) {
                return MxpiBufferManager::DestroyBuffer(mxpiBuffer);
            }
        }
        LogDebug << "element(" << elementName_ << ") distribute buffer by channel id." << channelId;
        SendData(idx, *mxpiBuffer);
    } else {
        LogDebug << "element(" << elementName_ << ") found no buffer that's channel id(" << channelId
                 << ") belongs to the specified category.";
        MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiDistributor::DistributeByClassId(MxpiBuffer* mxpiBuffer)
{
    APP_ERROR ret = APP_ERR_OK;
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    std::shared_ptr<void> metadataPtr = mxpiMetadataManager.GetMetadata(parentName_);
    if (metadataPtr == nullptr) {
        return NullptrHandle(mxpiBuffer);
    }
    auto* foo = (google::protobuf::Message*)metadataPtr.get();
    const google::protobuf::Descriptor* desc = foo->GetDescriptor();
    if (!desc) {
        LogError << "Get buffer's descriptor failed." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE; // self define the error code
    }
    if (desc->name() == OBJECT_LIST_KEY) {
        ret = DistributeObjectListByClassId(metadataPtr, mxpiBuffer);
    } else if (desc->name() == CLASS_LIST_KEY) {
        ret = DistributeClassListByClassId(metadataPtr, mxpiBuffer);
    } else if (desc->name() == OBJECT_KEY) {
        ret = DistributeObjectByClassId(metadataPtr, mxpiBuffer);
    } else if (desc->name() == CLASS_KEY) {
        ret = DistributeClassByClassId(metadataPtr, mxpiBuffer);
    } else {
        ret = OtherHandle(metadataPtr, mxpiBuffer);
    }
    return ret;
}

bool MxpiDistributor::NoDataOfInterested(std::vector<std::shared_ptr<MxTools::MxpiObjectList>> &objlistVec)
{
    for (size_t i = 0; i < objlistVec.size(); i++) {
        if (objlistVec[i]->objectvec().size() != 0) {
            return false;
        }
    }
    return true;
}

bool MxpiDistributor::NoDataOfInterested(std::vector<std::shared_ptr<MxTools::MxpiClassList>> &clslistVec)
{
    for (size_t i = 0; i < clslistVec.size(); i++) {
        if (clslistVec[i]->classvec().size() != 0) {
            return false;
        }
    }
    return true;
}

APP_ERROR MxpiDistributor::CopyMxpiObject(size_t i, MxTools::MxpiObject &dst, const MxTools::MxpiObject &src)
{
    dst.CopyFrom(src);
    dst.clear_headervec();
    MxTools::MxpiMetaHeader *header = dst.add_headervec();
    if (CheckPtrIsNullptr(header, "header"))  return APP_ERR_COMM_ALLOC_MEM;
    header->set_datasource(parentName_);
    header->set_memberid(i);
    return APP_ERR_OK;
}

APP_ERROR MxpiDistributor::GetObjectListVec(std::vector<std::shared_ptr<MxTools::MxpiObjectList>> &objectListVec,
    const std::shared_ptr<void> dataPtr)
{
    std::shared_ptr<MxTools::MxpiObjectList> objectList = std::static_pointer_cast<MxTools::MxpiObjectList>(dataPtr);
    for (size_t i = 0; i < firstLevelIdVec_.size(); i++) {
        objectListVec[i] = MemoryHelper::MakeShared<MxTools::MxpiObjectList>();
        if (objectListVec[i] == nullptr) {
            LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    for (int i = 0; i < objectList->objectvec().size(); i++) {
        int32_t classId = objectList->objectvec(i).classvec(0).classid();
        for (size_t j = 0; j < firstLevelIdVec_.size(); j++) {
            if (!IsElementInVector(secondLevelIdVec_[j], std::to_string(classId))) {
                continue;
            }
            MxTools::MxpiObject* mxpiObject = objectListVec[j]->add_objectvec();
            if (CheckPtrIsNullptr(mxpiObject, "mxpiObject"))  return APP_ERR_COMM_ALLOC_MEM;
            APP_ERROR ret = CopyMxpiObject(i, *mxpiObject, objectList->objectvec(i));
            if (ret != APP_ERR_OK) {
                LogError << "Fail to copy mxpiObject." << GetErrorInfo(ret);
                return ret;
            }
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiDistributor::DistributeObjectListByClassId(const std::shared_ptr<void> dataPtr,
    MxTools::MxpiBuffer* mxpiBuffer)
{
    std::vector<std::shared_ptr<MxTools::MxpiObjectList>> objectListVec(firstLevelIdVec_.size());
    APP_ERROR ret = GetObjectListVec(objectListVec, dataPtr);
    if (ret != APP_ERR_OK) {
        return MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    }
    if (!distributeAll_ && NoDataOfInterested(objectListVec)) {
        LogDebug << "element(" << elementName_
                 << ") found no object that's class id belongs to specified category.";
        return MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    }
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    for (size_t i = 0; i < firstLevelIdVec_.size(); i++) {
        std::string name = elementName_ + "_" + std::to_string(i);
        LogDebug << "element(" << elementName_
                 << ") distibute data(data structure:MxpiObjectList) by class id."
                 << "object number: " << objectListVec[i]->objectvec().size();
        if (objectListVec[i]->objectvec().size() != 0) {
            std::shared_ptr<void> metadata = std::static_pointer_cast<void>(objectListVec[i]);
            ret = mxpiMetadataManager.AddProtoMetadata(name, metadata);
            if (ret != APP_ERR_OK) {
                LogDebug << "element(" << elementName_ << ") add proto metadata failed.";
                return MxpiBufferManager::DestroyBuffer(mxpiBuffer);
            }
        }
        if (distributeAll_ || objectListVec[i]->objectvec().size() != 0) {
            gst_buffer_ref((GstBuffer*) mxpiBuffer->buffer);
            MxTools::MxpiBuffer* tmpBuffer = new(std::nothrow) MxpiBuffer {mxpiBuffer->buffer, nullptr};
            if (tmpBuffer == nullptr) {
                return MallocFailedHandle(mxpiBuffer);
            }
            SendData(i, *tmpBuffer);
        }
    }
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    return APP_ERR_OK;
}

APP_ERROR MxpiDistributor::DistributeObjectByClassId(const std::shared_ptr<void> dataPtr,
    MxTools::MxpiBuffer* mxpiBuffer)
{
    std::shared_ptr<MxTools::MxpiObject> objectPtr = std::static_pointer_cast<MxTools::MxpiObject>(dataPtr);
    int32_t classId = objectPtr->classvec(0).classid();
    LogDebug << "element(" << elementName_
             << ") distribute data(data structure:MxpiObject) by class id. class id: " << classId
             << ", object number: 1";
    for (size_t i = 0; i < firstLevelIdVec_.size(); i++) {
        if (IsElementInVector(secondLevelIdVec_[i], std::to_string(classId))) {
            std::string name = elementName_ + "_" + std::to_string(i);
            std::shared_ptr<void> metadata = std::static_pointer_cast<void>(objectPtr);
            MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
            APP_ERROR ret = mxpiMetadataManager.AddProtoMetadata(name, metadata);
            if (ret != APP_ERR_OK) {
                LogDebug << "element(" << elementName_ << ") add proto metadata failed. class id: " << classId << ".";
                return MxpiBufferManager::DestroyBuffer(mxpiBuffer);
            }
        }
        if (distributeAll_ || IsElementInVector(secondLevelIdVec_[i], std::to_string(classId))) {
            gst_buffer_ref((GstBuffer*) mxpiBuffer->buffer);
            MxTools::MxpiBuffer* tmpBuffer = new(std::nothrow) MxpiBuffer {mxpiBuffer->buffer, nullptr};
            if (tmpBuffer == nullptr) {
                return MallocFailedHandle(mxpiBuffer);
            }
            SendData(i, *tmpBuffer);
        }
    }
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    return APP_ERR_OK;
}

APP_ERROR MxpiDistributor::CopyMxpiClass(size_t i, MxTools::MxpiClass &dst, const MxTools::MxpiClass &src)
{
    dst.CopyFrom(src);
    dst.clear_headervec();
    MxTools::MxpiMetaHeader *header = dst.add_headervec();
    if (CheckPtrIsNullptr(header, "header"))  return APP_ERR_COMM_ALLOC_MEM;
    header->set_datasource(parentName_);
    header->set_memberid(i);
    return APP_ERR_OK;
}

APP_ERROR MxpiDistributor::GetclassListVec(std::vector<std::shared_ptr<MxTools::MxpiClassList>> &classListVec,
    const std::shared_ptr<void> dataPtr)
{
    std::shared_ptr<MxTools::MxpiClassList> classList = std::static_pointer_cast<MxTools::MxpiClassList>(dataPtr);
    for (size_t i = 0; i < firstLevelIdVec_.size(); i++) {
        classListVec[i] = MemoryHelper::MakeShared<MxTools::MxpiClassList>();
        if (classListVec[i] == nullptr) {
            LogError << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }
    for (int i = 0; i < classList->classvec().size(); i++) {
        int32_t classId = classList->classvec(i).classid();
        for (size_t j = 0; j < firstLevelIdVec_.size(); j++) {
            if (!IsElementInVector(secondLevelIdVec_[j], std::to_string(classId))) {
                continue;
            }
            MxTools::MxpiClass* mxpiClass = classListVec[j]->add_classvec();
            if (CheckPtrIsNullptr(mxpiClass, "mxpiClass"))  return APP_ERR_COMM_ALLOC_MEM;
            APP_ERROR ret = CopyMxpiClass(i, *mxpiClass, classList->classvec(i));
            if (ret != APP_ERR_OK) {
                LogError << "Fail to copy mxpiClass." << GetErrorInfo(ret);
                return ret;
            }
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiDistributor::DistributeClassListByClassId(const std::shared_ptr<void> dataPtr,
    MxTools::MxpiBuffer* mxpiBuffer)
{
    std::vector<std::shared_ptr<MxTools::MxpiClassList>> classListVec(firstLevelIdVec_.size());
    APP_ERROR ret = GetclassListVec(classListVec, dataPtr);
    if (ret != APP_ERR_OK) {
        return MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    }
    if (!distributeAll_ && NoDataOfInterested(classListVec)) {
        LogDebug << "element(" << elementName_
                 << ") found no object that's class id belongs to specified category.";
        return MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    }
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    for (size_t i = 0; i < firstLevelIdVec_.size(); i++) {
        std::string name = elementName_ + "_" + std::to_string(i);
        LogDebug << "element(" << elementName_
                 << ") distribute data(data structure:MxpiClassList) by class id."
                 << ", object number: " << classListVec[i]->classvec().size();
        if (classListVec[i]->classvec().size() != 0) {
            std::shared_ptr<void> metadata = std::static_pointer_cast<void>(classListVec[i]);
            ret = mxpiMetadataManager.AddProtoMetadata(name, metadata);
            if (ret != APP_ERR_OK) {
                LogDebug << "element(" << elementName_ << ") add proto metadata failed.";
                return MxpiBufferManager::DestroyBuffer(mxpiBuffer);
            }
        }
        if (distributeAll_ || classListVec[i]->classvec().size() != 0) {
            gst_buffer_ref((GstBuffer*) mxpiBuffer->buffer);
            MxTools::MxpiBuffer* tmpBuffer = new(std::nothrow) MxpiBuffer {mxpiBuffer->buffer, nullptr};
            if (tmpBuffer == nullptr) {
                return MallocFailedHandle(mxpiBuffer);
            }
            SendData(i, *tmpBuffer);
        }
    }
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    return ret;
}

APP_ERROR MxpiDistributor::DistributeClassByClassId(const std::shared_ptr<void> dataPtr,
    MxTools::MxpiBuffer* mxpiBuffer)
{
    std::shared_ptr<MxTools::MxpiClass> classPtr = std::static_pointer_cast<MxTools::MxpiClass>(dataPtr);
    int32_t classId = classPtr->classid();
    LogDebug << "element(" << elementName_
             << ") distribute data(structure:MxpiClass) by class id. class id: " << classId << ", object number: 1";
    for (size_t i = 0; i < firstLevelIdVec_.size(); i++) {
        if (IsElementInVector(secondLevelIdVec_[i], std::to_string(classId))) {
            std::string name = elementName_ + "_" + std::to_string(i);
            std::shared_ptr<void> metadata = std::static_pointer_cast<void>(classPtr);
            MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
            APP_ERROR ret = mxpiMetadataManager.AddProtoMetadata(name, metadata);
            if (ret != APP_ERR_OK) {
                LogDebug << "element(" << elementName_ << ") add proto metadata failed. class id: " << classId << ".";
                return MxpiBufferManager::DestroyBuffer(mxpiBuffer);
            }
        }
        if (distributeAll_ || IsElementInVector(secondLevelIdVec_[i], std::to_string(classId))) {
            gst_buffer_ref((GstBuffer*) mxpiBuffer->buffer);
            MxTools::MxpiBuffer* tmpBuffer = new(std::nothrow) MxpiBuffer {mxpiBuffer->buffer, nullptr};
            if (tmpBuffer == nullptr) {
                return MallocFailedHandle(mxpiBuffer);
            }
            SendData(i, *tmpBuffer);
        }
    }
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    return APP_ERR_OK;
}

APP_ERROR MxpiDistributor::OtherHandle(const std::shared_ptr<void>, MxTools::MxpiBuffer* mxpiBuffer)
{
    MxTools::MxpiErrorInfo mxpiErrorInfo;
    mxpiErrorInfo.ret = APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH;
    mxpiErrorInfo.errorInfo = "Not a MxpiObjectList or MxpiClassList or MxpiObject or MxpiClass object.";
    LogError << mxpiErrorInfo.errorInfo << GetErrorInfo(APP_ERR_MXPLUGINS_PROTOBUF_NAME_MISMATCH);
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer);
    APP_ERROR ret = mxpiMetadataManager.AddErrorInfo(elementName_, mxpiErrorInfo);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to AddErrorInfo." << GetErrorInfo(ret);
        MxpiBufferManager::DestroyBuffer(mxpiBuffer);
        return ret;
    }
    for (size_t i = 0; i < firstLevelIdVec_.size(); i++) {
        gst_buffer_ref((GstBuffer*) mxpiBuffer->buffer);
        MxTools::MxpiBuffer* tmpBuffer = new(std::nothrow) MxpiBuffer {mxpiBuffer->buffer, nullptr};
        if (tmpBuffer == nullptr) {
            return MallocFailedHandle(mxpiBuffer);
        }
        SendData(i, *tmpBuffer);
    }
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    return ret;
}

APP_ERROR MxpiDistributor::MallocFailedHandle(MxTools::MxpiBuffer* mxpiBuffer)
{
    LogError << "Allocate memory with \"new MxpiBuffer\" failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
    gst_buffer_unref((GstBuffer*) mxpiBuffer->buffer);
    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    return APP_ERR_COMM_ALLOC_MEM;
}

APP_ERROR MxpiDistributor::NullptrHandle(MxpiBuffer* mxpiBuffer)
{
    LogDebug << GetErrorInfo(APP_ERR_MXPLUGINS_METADATA_IS_NULL, elementName_) << "metadata is null.";
    for (size_t i = 0; i < firstLevelIdVec_.size(); i++) {
        gst_buffer_ref((GstBuffer*) mxpiBuffer->buffer);
        auto* tmpBuffer = new(std::nothrow) MxpiBuffer {mxpiBuffer->buffer, nullptr};
        if (tmpBuffer == nullptr) {
            return MallocFailedHandle(mxpiBuffer);
        }
        SendData(i, *tmpBuffer);
    }

    MxpiBufferManager::DestroyBuffer(mxpiBuffer);
    return APP_ERR_OK;
}

namespace {
    MX_PLUGIN_GENERATE(MxpiDistributor)
}
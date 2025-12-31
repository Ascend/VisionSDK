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
 * Description: Complete plug-in data download.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <google/protobuf/util/json_util.h>
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxTools/Proto/MxpiInternalType.pb.h"
#include "MxTools/PluginToolkit/base/MxpiBufferDump.h"

using namespace MxBase;

namespace MxTools {
std::string MxpiBufferDump::DoDump(MxTools::MxpiBuffer& mxpiBuffer, const std::vector<std::string>& filterKeys,
                                   const std::vector<std::string>& requiredKeys)
{
    std::string jsonString;
    MxTools::MxpiDumpData dumpData;

    // build buffer data
    BuildBufferData(mxpiBuffer, dumpData);
    MxTools::MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
    auto metaMap = mxpiMetadataManager.GetAllMetaData();

    bool isDumpMemory = metaMap.find("=dumpMemory=") == metaMap.end();
    for (auto& item : metaMap) {
        const std::string& metaKey = item.first;
        if (MxBase::StringUtils::HasInvalidChar(metaKey)) {
            LogError << "The metaData key has invalid char" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return jsonString;
        } else {
            LogDebug << "The metaData key=" << metaKey;
        }

        if (IsMetaDataKeyFilter(filterKeys, requiredKeys, metaKey)) {
            continue;
        }
        auto dumpMetaData = dumpData.add_metadata();
        dumpMetaData->set_key(metaKey);
        if (metaKey == "ReservedVisionList") {
            auto reservedVisionListKey = std::static_pointer_cast<std::string>(item.second);
            dumpMetaData->set_content(*reservedVisionListKey);
            continue;
        }
        APP_ERROR ret = HandleProtoDataType(item, metaKey, dumpMetaData, isDumpMemory);
        if (ret != APP_ERR_OK) {
            LogError << "Handle protobuf data type failed." << GetErrorInfo(ret);
            return jsonString;
        }
    }

    google::protobuf::util::JsonPrintOptions options;
    options.always_print_primitive_fields = true;
    if (!google::protobuf::util::MessageToJsonString(dumpData, &jsonString, options).ok()) {
        LogError << "The message convert to jsonString error" << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }

    return jsonString;
}

MxTools::MxpiBuffer* MxpiBufferDump::DoLoad(MxTools::MxpiBuffer& mxpiBuffer, int deviceId)
{
    MxTools::MxpiDumpData dumpData;
    APP_ERROR ret = BuildMxpiBuffer(dumpData, mxpiBuffer);
    if (ret != APP_ERR_OK) {
        LogError << "Build mxpiBuffer object failed." << GetErrorInfo(ret);
        return nullptr;
    }

    const std::string& bufferData = dumpData.buffer().bufferdata();
    InputParam inputParam;
    inputParam.dataSize = static_cast<int>(bufferData.size());
    inputParam.ptrData = (void*) bufferData.c_str();
    MxpiBuffer* resultBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    if (resultBuffer == nullptr) {
        LogError << "Create host buffer failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    MxpiMetadataManager manager(*resultBuffer);
    manager.RemoveMetadata("ReservedFrameInfo");
    ret = BuildMetaData(*resultBuffer, dumpData, deviceId);
    if (ret != APP_ERR_OK) {
        MxpiBufferManager::DestroyBuffer(resultBuffer);
        LogError << "Build meta data failed." << GetErrorInfo(ret);
        return nullptr;
    }
    return resultBuffer;
}

MxTools::MxpiBuffer* MxpiBufferDump::DoLoad(const std::string& filePath, int deviceId)
{
    MxpiDumpData dumpData;
    std::string fileRegPath;
    if (!FileUtils::RegularFilePath(filePath, fileRegPath) || !FileUtils::IsFileValid(fileRegPath, false)) {
        LogError << "Check file failed, check list: regular, exist, size, owner, permission."
                 << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return nullptr;
    }
    std::string fileContent = FileUtils::ReadFileContent(fileRegPath);

    auto status = google::protobuf::util::JsonStringToMessage(fileContent, &dumpData);
    if (!status.ok()) {
        std::string errMsg = StringUtils::HasInvalidChar(status.ToString()) ?
                "The raw content contains invalid character." : status.ToString();
        LogError << "Parse json file failed. error: \"" << errMsg << "\"" << GetErrorInfo(APP_ERR_COMM_READ_FAIL);
        return nullptr;
    }
    const std::string& bufferData = dumpData.buffer().bufferdata();
    InputParam inputParam;
    inputParam.dataSize = static_cast<int>(bufferData.size());
    inputParam.ptrData = (void*) bufferData.c_str();
    MxpiBuffer* resultBuffer = MxpiBufferManager::CreateHostBufferAndCopyData(inputParam);
    if (resultBuffer == nullptr) {
        LogError << "Create host buffer failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return nullptr;
    }
    MxpiMetadataManager manager(*resultBuffer);
    manager.RemoveMetadata("ReservedFrameInfo");
    APP_ERROR ret = BuildMetaData(*resultBuffer, dumpData, deviceId);
    if (ret != APP_ERR_OK) {
        MxpiBufferManager::DestroyBuffer(resultBuffer);
        LogError << "Build meta data failed." << GetErrorInfo(ret);
        return nullptr;
    }
    return resultBuffer;
}

APP_ERROR MxpiBufferDump::HandleProtoDataType(std::pair<const std::string, std::shared_ptr<void>>& item,
                                              const std::string& metaKey, MetaData* dumpMetaData,
                                              bool isDumpMemoryData)
{
    if (StringUtils::HasInvalidChar(item.first)) {
        LogError << "The protobuf data has key which contains invalid character."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (item.second == nullptr) {
        LogError << "Handle protobuf data type failed. Invalid input message with key(" << item.first << ")."
                 << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL);
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
    }
    auto protobufMessage = std::static_pointer_cast<google::protobuf::Message>(item.second);
    auto desc = protobufMessage->GetDescriptor();
    if (desc == nullptr) {
        LogError << "Invalid protobuf message descriptor." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (isDumpMemoryData) {
        if (desc->name() == "MxpiVisionList") {
            BuildMxpiVisionData(item);
        } else if (desc->name() == "MxpiTensorPackageList") {
            BuildTensorPackageList(item);
        } else if (desc->name() == "MxpiContentString") {
            auto srcMxpiContentString = std::static_pointer_cast<MxTools::MxpiContentString>(item.second);
            dumpMetaData->set_content(srcMxpiContentString->content());
            return APP_ERR_OK;
        }
        dumpMetaData->set_protodatatype(desc->name());
    }

    google::protobuf::util::JsonPrintOptions options;
    options.always_print_primitive_fields = true;
    std::string serializeAsJsonString;
    if (!google::protobuf::util::MessageToJsonString(*protobufMessage, &serializeAsJsonString, options).ok()) {
        std::string errMsg = MxBase::StringUtils::HasInvalidChar(metaKey) ?
            "The message convert to jsonString error." : "The message convert to jsonString error. key = " + metaKey;
        LogError << errMsg << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_MESSAGE_TO_STRING_FAILED);
        return APP_ERR_PLUGIN_TOOLKIT_MESSAGE_TO_STRING_FAILED;
    }

    if (serializeAsJsonString != "{}") {
        dumpMetaData->set_content(serializeAsJsonString);
    }

    return APP_ERR_OK;
}

void MxpiBufferDump::BuildTensorPackageList(const std::pair<const std::string, std::shared_ptr<void>>& item)
{
    std::shared_ptr<MxpiTensorPackageList> srcMxpiTensorList =
        std::static_pointer_cast<MxpiTensorPackageList>(item.second);
    LogInfo << "The metadata key(" << item.first << ") start to dump.";
    for (int i = 0; i < srcMxpiTensorList->tensorpackagevec_size(); ++i) {
        auto tensorPackage = srcMxpiTensorList->mutable_tensorpackagevec(i);
        for (int j = 0; j < tensorPackage->tensorvec_size(); ++j) {
            auto tensor = tensorPackage->mutable_tensorvec(j);
            std::string memoryStr = GetHostMemoryData((void*) tensor->tensordataptr(),
                                                      tensor->tensordatasize(), tensor->memtype(),
                                                      tensor->deviceid());
            tensor->set_datastr(memoryStr);
        }
    }
}

bool MxpiBufferDump::IsMetaDataKeyFilter(const std::vector<std::string>& filterKeys,
                                         const std::vector<std::string>& requiredKeys, const std::string& metaKey)
{
    // if the requiredKeys is not empty, only the requiredKeys are dumped. The filterKeys would not take effect.
    if (!requiredKeys.empty()) {
        for (auto& key : requiredKeys) {
            std::string temp = key;
            if (MxBase::StringUtils::Trim(temp) == metaKey) {
                return false;
            }
        }

        return true;
    }

    std::vector<std::string> filterMetadataKey = {
        "ReserveMetadataGraph", "MxstFrameExternalInfo", "ErrorInfo", "=dumpMemory="
    };
    if (!filterKeys.empty()) {
        for (const auto& key : filterKeys) {
            std::string temp = key;
            filterMetadataKey.push_back(MxBase::StringUtils::Trim(temp));
        }
    }

    auto iter = std::find(filterMetadataKey.begin(), filterMetadataKey.end(), metaKey);
    if (iter != filterMetadataKey.end()) {
        return true;
    }

    return false;
}

APP_ERROR MxpiBufferDump::BuildMxpiVisionData(std::pair<const std::string, std::shared_ptr<void>>& item)
{
    std::shared_ptr<MxTools::MxpiVisionList> srcMxpiVisionList =
        std::static_pointer_cast<MxTools::MxpiVisionList>(item.second);
    LogInfo << "The metadata key(" << item.first << ") start to dump.";
    for (auto& mxpiVision : (*srcMxpiVisionList->mutable_visionvec())) {
        auto visionData = mxpiVision.mutable_visiondata();
        std::string memoryStr = GetHostMemoryData((void*) visionData->dataptr(), visionData->datasize(),
                                                  visionData->memtype(), visionData->deviceid());
        visionData->set_datastr(memoryStr);
    }
    return APP_ERR_OK;
}

std::string MxpiBufferDump::GetHostMemoryData(void* dataPtr, int dataSize, const MxpiMemoryType& memoryType,
                                              unsigned int deviceId)
{
    LogInfo << "The dataSize=" << dataSize << ", memType=" << memoryType << ", deviceId=" << deviceId;
    if ((dataPtr == nullptr) || (dataSize == 0)) {
        LogWarn << "MxpiBufferDump: cannot get host memory data, dataptr is null or datasize is 0.";
        return "";
    }

    if (memoryType == MXPI_MEMORY_HOST) {
        std::string result((char *) dataPtr, dataSize);
        return result;
    }

    MemoryData memoryDataDst(dataSize, MemoryData::MEMORY_HOST);
    MemoryData memoryDataSrc(dataPtr, dataSize, MemoryData::MEMORY_DEVICE, deviceId);
    auto ret = MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
    if (ret != APP_ERR_OK) {
        LogError << "Memory malloc failed." << GetErrorInfo(ret);
        return "";
    }

    std::string result((char *) memoryDataDst.ptrData, memoryDataDst.size);
    MemoryHelper::MxbsFree(memoryDataDst);

    return result;
}

void MxpiBufferDump::BuildBufferData(MxTools::MxpiBuffer& mxpiBuffer, MxTools::MxpiDumpData& dumpData)
{
    auto dumpBuffer = dumpData.mutable_buffer();
    MxTools::MxpiFrame inputMxpiFrame = MxTools::MxpiBufferManager::GetHostDataInfo(mxpiBuffer);
    if (inputMxpiFrame.visionlist().visionvec_size() == 0) {
        return;
    }
    MxTools::MxpiVisionData inputVisionData = inputMxpiFrame.visionlist().visionvec(0).visiondata();
    if (((char *)inputVisionData.dataptr() != nullptr) && (inputVisionData.datasize() > 0)) {
        std::string bufferData((char *)inputVisionData.dataptr(), inputVisionData.datasize());
        dumpBuffer->set_bufferdata(bufferData);
    }
}

APP_ERROR MxpiBufferDump::BuildMxpiBuffer(MxTools::MxpiDumpData& dumpData, MxTools::MxpiBuffer& mxpiBuffer)
{
    MxpiFrame inputMxpiFrame = MxpiBufferManager::GetHostDataInfo(mxpiBuffer);
    if (inputMxpiFrame.visionlist().visionvec_size() == 0) {
        LogError << "The protobuf message vector is empty." << GetErrorInfo(APP_ERR_COMM_OUT_OF_RANGE);
        return APP_ERR_COMM_OUT_OF_RANGE;
    }
    MxpiVisionData inputVisionData = inputMxpiFrame.visionlist().visionvec(0).visiondata();
    if (inputVisionData.datasize() <= 0) {
        LogWarn << "The visionData data size is " << inputVisionData.datasize();
        return APP_ERR_OK;
    }
    std::string bufferData((char *) inputVisionData.dataptr(), inputVisionData.datasize());

    auto status = google::protobuf::util::JsonStringToMessage(bufferData, &dumpData);
    if (!status.ok()) {
        std::string errMsg = StringUtils::HasInvalidChar(status.ToString()) ?
                "The raw content contains invalid character." : status.ToString();
        LogError << "parse json string failed. error: \"" << errMsg << "\""
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiBufferDump::BuildMetaData(MxTools::MxpiBuffer& mxpiBuffer, MxTools::MxpiDumpData& dumpData, int deviceId)
{
    MxpiMetadataManager mxpiMetadataManager(mxpiBuffer);
    for (auto& metaData : dumpData.metadata()) {
        if (metaData.content().empty()) {
            continue;
        } else if (metaData.protodatatype().empty() && metaData.key() == "ReservedVisionList") {
            auto metaDataValue = MxBase::MemoryHelper::MakeShared<std::string>(metaData.content());
            if (metaDataValue == nullptr) {
                LogError << "Create string object failed. Failed to allocate."<< GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
                return APP_ERR_COMM_ALLOC_MEM;
            }
            mxpiMetadataManager.AddMetadata(metaData.key(), std::static_pointer_cast<void>(metaDataValue));
            continue;
        } else if (metaData.protodatatype().empty()) {
            auto ret = BuildStringData(&mxpiMetadataManager, metaData);
            if (ret != APP_ERR_OK) {
                return ret;
            }
            continue;
        } else if (metaData.protodatatype() == "MxpiVisionList") {
            auto ret = BuildVisionData(&mxpiMetadataManager, metaData, deviceId);
            if (ret != APP_ERR_OK) {
                return ret;
            }
            continue;
        } else if (metaData.protodatatype() == "MxpiTensorPackageList") {
            auto ret = BuildTensorPackage(&mxpiMetadataManager, metaData, deviceId);
            if (ret != APP_ERR_OK) {
                return ret;
            }
            continue;
        }
        std::shared_ptr<google::protobuf::Message> msg(CreateMessage("MxTools." + metaData.protodatatype()));
        if (msg == nullptr) {
            LogError << "Invalid metaData.key or metaData.protodatatype, it may contain invalid character."
                     << GetErrorInfo(APP_ERR_COMM_FAILURE);
            continue;
        }
        auto status = google::protobuf::util::JsonStringToMessage(metaData.content(), msg.get());
        if (!status.ok()) {
            std::string errMsg = StringUtils::HasInvalidChar(status.ToString()) ?
                "The raw content contains invalid character." : status.ToString();
            LogError << "parse json string failed. error: \"" << errMsg << "\"" << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
        mxpiMetadataManager.AddProtoMetadata(metaData.key(), std::static_pointer_cast<void>(msg));
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiBufferDump::BuildStringData(void* mxpiMetadataManager,
                                          const MxTools::MetaData& metaData)
{
    auto mxpiContentString = new (std::nothrow) MxpiContentString;
    if (mxpiContentString == nullptr) {
        LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    auto g_deleteFuncMxpiContentString = [](MxpiContentString* mxpiContentString) {
        if (mxpiContentString == nullptr) {
            LogWarn << "The g_deleteFuncMxpiContentString: Pointer mxpiVisionList is nullptr.";
            return;
        }
        delete mxpiContentString;
        mxpiContentString = nullptr;
    };
    std::shared_ptr<MxpiContentString> mxpiString(mxpiContentString, g_deleteFuncMxpiContentString);
    mxpiString->set_content(metaData.content());
    ((MxTools::MxpiMetadataManager*)mxpiMetadataManager)->AddProtoMetadata(metaData.key(),
        std::static_pointer_cast<void>(mxpiString));
    return APP_ERR_OK;
}

APP_ERROR MxpiBufferDump::BuildVisionData(void* mxpiMetadataManager,
                                          const MxTools::MetaData& metaData,
                                          int deviceId)
{
    auto mxpiVisionList = new (std::nothrow) MxpiVisionList;
    if (mxpiVisionList == nullptr) {
        LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::shared_ptr<MxpiVisionList> mxpiList(mxpiVisionList, g_deleteFuncMxpiVisionList);
    auto status = google::protobuf::util::JsonStringToMessage(metaData.content(), mxpiList.get());
    if (!status.ok()) {
        std::string errMsg = StringUtils::HasInvalidChar(status.ToString()) ?
            "The raw content contains invalid character." : status.ToString();
        LogError << "Parse json string failed. error: \"" << errMsg << "\"" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    ((MxTools::MxpiMetadataManager*)mxpiMetadataManager)->AddProtoMetadata(metaData.key(),
                                                                           std::static_pointer_cast<void>(mxpiList));
    for (auto& vision: *(mxpiList->mutable_visionvec())) {
        auto visionData = vision.mutable_visiondata();
        if (visionData->datastr().empty()) {
            continue;
        }
        void *ptrData = nullptr;
        auto ret = LoadDataToMemory(visionData->datastr(), visionData->memtype(), ptrData, deviceId);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        visionData->set_deviceid(deviceId);
        visionData->set_dataptr((google::protobuf::uint64) ptrData);
        visionData->set_datastr("");
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiBufferDump::BuildTensorPackage(void* mxpiMetadataManager, const MxTools::MetaData& metaData, int deviceId)
{
    MxpiTensorPackageList *mxpiTensorPackageList = new (std::nothrow) MxpiTensorPackageList;
    if (mxpiTensorPackageList == nullptr) {
        LogError << "The pointer is null." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    std::shared_ptr<MxpiTensorPackageList> mxpiList(mxpiTensorPackageList, g_deleteFuncMxpiTensorPackageList);
    auto status = google::protobuf::util::JsonStringToMessage(metaData.content(), mxpiList.get());
    if (!status.ok()) {
        std::string errMsg = StringUtils::HasInvalidChar(status.ToString()) ?
            "The raw content contains invalid character." : status.ToString();
        LogError << "Parse json string failed. error: \"" << errMsg << "\"" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    ((MxTools::MxpiMetadataManager*)mxpiMetadataManager)->AddProtoMetadata(metaData.key(),
                                                                           std::static_pointer_cast<void>(mxpiList));
    for (auto& tensorPackage: *(mxpiList->mutable_tensorpackagevec())) {
        for (auto& tensor: *(tensorPackage.mutable_tensorvec())) {
            if (tensor.datastr().empty()) {
                continue;
            }
            void *ptrData = nullptr;
            auto ret = LoadDataToMemory(tensor.datastr(), tensor.memtype(), ptrData, deviceId);
            if (ret != APP_ERR_OK) {
                return ret;
            }
            tensor.set_deviceid(deviceId);
            tensor.set_tensordataptr((google::protobuf::uint64) ptrData);
            tensor.set_datastr("");
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiBufferDump::LoadDataToMemory(const std::string& content,
                                           MxTools::MxpiMemoryType memoryType,
                                           void* &dstPtrData,
                                           int deviceId)
{
    MxBase::MemoryData memoryDataSrc((void *) content.c_str(),
                                     content.size(),
                                     MxBase::MemoryData::MEMORY_HOST,
                                     deviceId);
    MxBase::MemoryData memoryDataDst(content.size(), (MxBase::MemoryData::MemoryType) memoryType, deviceId);
    auto ret = MxBase::MemoryHelper::MxbsMallocAndCopy(memoryDataDst, memoryDataSrc);
    if (ret != APP_ERR_OK) {
        LogError << "Malloc and copy failed." << GetErrorInfo(ret);
        return ret;
    }
    dstPtrData = memoryDataDst.ptrData;
    return APP_ERR_OK;
}

google::protobuf::Message* MxpiBufferDump::CreateMessage(const std::string& typeName)
{
    google::protobuf::Message* message = nullptr;
    auto descriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(typeName);
    if (descriptor) {
        auto prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
        if (prototype) {
            message = prototype->New();
        }
    }
    return message;
}
}
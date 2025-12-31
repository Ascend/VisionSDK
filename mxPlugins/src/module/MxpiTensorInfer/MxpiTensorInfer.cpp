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
 * Description: Infer on the input tensor.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiTensorInfer/MxpiTensorInfer.h"
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <dlfcn.h>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxBase/Utils/StringUtils.h"
#include "MxBase/GlobalManager/GlobalManager.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxBase;
using namespace MxTools;
using namespace MxPlugins;

namespace {
const int CHECKFREQUENCY = 10;
const uint32_t MAX_INTERVAL = 100000;
const float SEC2MS = 1000.0;
const int NHWC_H_DIM = 1;
const int NHWC_W_DIM = 2;
const int NHWC_C_DIM = 3;
const int MAX_QUEUE_SIZE = 32;
const int QUEUE_SLEEP_TIME = 100;
static long g_startEmptyTime = time(nullptr);
const int TIME_WARN_SECONDS = 30;

void ShowQueueState(std::queue<std::pair<MxpiBuffer*, bool>> queue, std::string elementName)
{
    std::ostringstream queueInfo;
    auto size = queue.size();
    while (!queue.empty()) {
        queueInfo << queue.front().second << " ";
        queue.pop();
    }
    LogDebug << "outputBufferQueue in plugin(" << elementName << ") size: " << size << ", content: " << queueInfo.str();
}

auto continuousDeleter = [](MxpiTensorPackageList* mxpiTensorPackageList) {
    MxBase::DeviceManager *deviceManager = MxBase::DeviceManager::GetInstance();
    MxBase::DeviceContext deviceContextOld;
    bool curDeviceIsOk = true;
    APP_ERROR ret = deviceManager->GetCurrentDevice(deviceContextOld);
    if (ret != APP_ERR_OK) {
        curDeviceIsOk = false;
    }
    if (mxpiTensorPackageList->tensorpackagevec_size() == 0) {
        SetDeviceID(deviceManager, deviceContextOld, curDeviceIsOk);
        delete mxpiTensorPackageList;
        return;
    }
    for (const auto& itTensor : mxpiTensorPackageList->tensorpackagevec(0).tensorvec()) {
        MxBase::MemoryData memoryData;
        MxBase::DeviceContext deviceContext;
        deviceContext.devId = itTensor.deviceid();
        memoryData.size = itTensor.tensordatasize();
        memoryData.deviceId = itTensor.deviceid();
        memoryData.type = (MxBase::MemoryData::MemoryType)itTensor.memtype();
        if (memoryData.type == MxBase::MemoryData::MemoryType::MEMORY_DEVICE ||
            memoryData.type == MxBase::MemoryData::MemoryType::MEMORY_DVPP) {
            SetDeviceID(deviceManager, deviceContext, true);
        }
        memoryData.ptrData = (void *)itTensor.tensordataptr();
        if (memoryData.ptrData == nullptr || memoryData.size == 0) {
            continue;
        }
        ret = MxBase::MemoryHelper::MxbsFree(memoryData);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to free memory of MxpiTensorPackageList, pointer("
                     << memoryData.ptrData << "), type(" << memoryData.type << ")." << GetErrorInfo(ret);
            continue;
        }
    }
    SetDeviceID(deviceManager, deviceContextOld, curDeviceIsOk);
    delete mxpiTensorPackageList;
};
} // namespace

APP_ERROR MxpiTensorInfer::Init(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    LogInfo << "Begin to initialize MxpiTensorInfer(" << elementName_ << ").";
    // Set previous plugin name
    APP_ERROR ret = InitConfig(configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Init configs failed." << GetErrorInfo(ret);
        return ret;
    }
    // model dynamic HW init.
    for (const auto& dynamicImageSize : model_.GetDynamicImageSizes()) {
        MxTools::ImageSize imageSize(dynamicImageSize.height, dynamicImageSize.width);
        dynamicHWs_.push_back(imageSize);
        maxArea_ = std::max(dynamicImageSize.height * dynamicImageSize.width, maxArea_);
    }
    elementDynamicImageSize_[GetElementNameWithObjectAddr()] = dynamicHWs_;

    LogModelTensorsShape(modelDesc_);

    // malloc input memory
    ret = TensorMemoryMalloc();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to malloc memory for modelInfer." << GetErrorInfo(ret);
        return ret;
    }

    // decide singleBatch or multiBatch
    if (!singleBatchInfer_ && (modelDesc_.batchSizes[0] != 1 || modelDesc_.batchSizes.size() != 1)) {
        CreateThread();
        maxBatchSize_ = static_cast<int>(modelDesc_.batchSizes.back());
    } else {
        maxBatchSize_ = 1;
    }

    if (IsDenominatorZero(maxBatchSize_)) {
        LogError << "The value of maxBatchSize_ must not equal to 0!" << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    PerformanceStatisticsManager::GetInstance()->ModelInferenceStatisticsRegister(streamName_, elementName_);
    LogInfo << "End to initialize MxpiTensorInfer(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::InitConfig(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    dataSourcesString_ = dataSource_; // check property
    APP_ERROR ret = CheckInputPortsStatus();
    if (ret != APP_ERR_OK) {
        LogError << "DataSources init failed." << GetErrorInfo(ret);
        return ret;
    }

    std::vector<std::string> parameterNamesPtr = {"outputDeviceId", "waitingTime", "dynamicStrategy", "skipModelCheck",
                                                  "singleBatchInfer", "outputHasBatchDim", "modelPath"};
    ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    // Set outputDeviceId
    outputDeviceId_ = *std::static_pointer_cast<int>(configParamMap["outputDeviceId"]);
    // Set waitingTime
    timeTravel_ = *std::static_pointer_cast<uint32_t>(configParamMap["waitingTime"]);
    // Set dynamicStrategy
    std::string dynamicStrategy = *std::static_pointer_cast<std::string>(configParamMap["dynamicStrategy"]);
    if (DYNAMIC_STRATEGY.find(dynamicStrategy) == DYNAMIC_STRATEGY.end()) {
        LogError << "Unknown dynamicStrategy [" << dynamicStrategy << "]." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    dynamicStrategy_ = static_cast<int>(DYNAMIC_STRATEGY[dynamicStrategy]);
    // Set skipModelCheck
    skipModelCheck_ = *std::static_pointer_cast<int>(configParamMap["skipModelCheck"]);
    // Set singleBatchInfer
    singleBatchInfer_ = *std::static_pointer_cast<bool>(configParamMap["singleBatchInfer"]);
    // Set outputHasBatchDim
    outputHasBatchDim_ = *std::static_pointer_cast<bool>(configParamMap["outputHasBatchDim"]);

    // model init
    ret = model_.Init(*std::static_pointer_cast<std::string>(configParamMap["modelPath"]));
    if (ret != APP_ERR_OK) {
        LogError << "Failed to init model inference." << GetErrorInfo(ret);
        return ret;
    }
    modelDesc_ = model_.GetModelDesc();
    dynamicInfo_.dynamicType = model_.GetDynamicType();

    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::Process(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    LogDebug << "Begin to process MxpiTensorInfer(" << elementName_ << ").";
    errorInfo_.str("");
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // Check Error info and metadata exist.
    ret = CheckMetaData(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        LogDebug << "Can not get metaData from (" << dataSourcesString_ << ").";
        return ret;
    }
    std::unique_lock<std::mutex> uniqueLock(mtx_);
    // Get tensors from dataSources, assemble then check validity, and finally apply for memory.
    ret = DataPreparation(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        ClearResourcesAndSendErrorInfo(ret);
        uniqueLock.unlock();
        notifyFlag_ = false;
        conditionVariable_.notify_one();
        return ret;
    }

    int batchSize = maxBatchSize_;
    // Do DataProcess when size of data in buffer is larger than max batchsize.
    if (IsDenominatorZero(batchSize)) {
        LogError << "The batchSize must not equal to 0!" << GetErrorInfo(ret = APP_ERR_COMM_INVALID_PARAM);
        ClearResourcesAndSendErrorInfo(ret);
        uniqueLock.unlock();
        notifyFlag_ = false;
        conditionVariable_.notify_one();
        return ret;
    }
    if ((int)dataQueue_.size() >= batchSize) {
        ret = DataProcess(dataQueue_.size() / batchSize, batchSize);
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            ClearResourcesAndSendErrorInfo(ret);
            uniqueLock.unlock();
            notifyFlag_ = false;
            conditionVariable_.notify_one();
            return ret;
        }
    }
    notifyFlag_ = true;
    uniqueLock.unlock();
    conditionVariable_.notify_one();
    LogDebug << "End to process MxpiTensorInfer(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::DeInit()
{
    LogInfo << "Begin to deinitialize MxpiTensorInfer(" << elementName_ << ").";
    APP_ERROR ret;
    if (!singleBatchInfer_ && (modelDesc_.batchSizes[0] != 1 || modelDesc_.batchSizes.size() != 1)) {
        runningFlag_ = false;
        notifyFlag_ = false;
        conditionVariable_.notify_one();
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    // release input memory
    for (size_t i = 0; i < inputTensors_.size(); i++) {
        MxBase::MemoryData memory;
        memory.type = MxBase::MemoryData::MEMORY_DEVICE;
        memory.ptrData = inputTensors_[i].buf;
        ret = MxBase::MemoryHelper::MxbsFree(memory);
        if (ret != APP_ERR_OK) {
            LogError << "Dvpp memory free failed." << GetErrorInfo(ret);
        }
    }
    // release output memory
    for (size_t i = 0; i < outputTensors_.size(); i++) {
        MxBase::MemoryData memory;
        memory.type = MxBase::MemoryData::MEMORY_DEVICE;
        memory.ptrData = outputTensors_[i].buf;
        ret = MxBase::MemoryHelper::MxbsFree(memory);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to free dvpp memory." << GetErrorInfo(ret);
        }
    }
    ret = model_.DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to deinitialize modelInfer module." << GetErrorInfo(ret);
    }

    LogInfo << "End to deinitialize MxpiTensorInfer(" << elementName_ << ").";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiTensorInfer::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    std::shared_ptr<void> modelPath = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "modelPath", "model", "model path", "", "", ""
    });
    std::shared_ptr<void> dynamicStrategy = std::make_shared<ElementProperty<std::string>>(
        ElementProperty<std::string> { STRING, "dynamicStrategy", "dynamic strategy",
        "strategy to choose batchsize using dynamic model: Nearest, Upper, Lower", "Nearest", "", ""
    });
    std::shared_ptr<void> timeTravel = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "waitingTime", "timeTravel",
        "maximun time for model infering with largest batchsize to wait for data", 5000, 1, 1000000
    });
    std::shared_ptr<void> outputDeviceId = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "outputDeviceId", "outputDeviceId",
        "the deviceId which model output tensor will be copied to. Specially, "
        "when set as -1, model output tensor will be copied to host", -1, -1, 7
    });
    std::shared_ptr<void> skipModelCheck = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "skipModelCheck", "skipModelCheck",
        "skip ModelCheck if set as 1 (not suggested).", 0, 0, 1
    });
    std::shared_ptr<void> singleBatchInfer = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
            UINT, "singleBatchInfer", "singleBatchInfer",
            "model input tensor shape has batch dimension, yes:1, no:0", 0, 0, 1
    });
    std::shared_ptr<void> outputHasBatchDim = std::make_shared<ElementProperty<uint>>(ElementProperty<uint> {
        UINT, "outputHasBatchDim", "outputHasBatchDim",
        "model output tensor shape has batch dimension, yes:1, no:0", 1, 0, 1
    });
    properties.push_back(modelPath);
    properties.push_back(dynamicStrategy);
    properties.push_back(timeTravel);
    properties.push_back(outputDeviceId);
    properties.push_back(skipModelCheck);
    properties.push_back(singleBatchInfer);
    properties.push_back(outputHasBatchDim);

    return properties;
}

MxpiPortInfo MxpiTensorInfer::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/yuv", "metadata/tensor"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);
    value = {{"ANY"}};
    GenerateDynamicInputPortsInfo(value, inputPortInfo);
    return inputPortInfo;
}

MxpiPortInfo MxpiTensorInfer::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {{"metadata/tensor"}};
    GenerateStaticOutputPortsInfo(value, outputPortInfo);
    return outputPortInfo;
}

APP_ERROR MxpiTensorInfer::CheckInputPortsStatus()
{
    status_ = MxTools::SYNC;

    if ((dataSourcesString_ == "auto") || (dataSourcesString_.empty())) {
        dataSources_ = dataSourceKeys_;
        if (dataSources_.empty()) {
            LogError << "Property dataSource is \"\", please check element(" << elementName_
                     << ")'s previous element." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
        }
        LogInfo << "dataSources are read automatically in plugin(" << elementName_ << ").";
        return APP_ERR_OK;
    }

    LogInfo << "dataSources (" << dataSourcesString_ << "are read from config file in plugin(" << elementName_ << ").";
    dataSources_ = MxBase::StringUtils::SplitWithRemoveBlank(dataSourcesString_, ',');
    // Check equality between input port num and specified dataSources_ num.
    if (sinkPadNum_ != dataSources_.size()) {
        LogError << "SinkPadNum (" << sinkPadNum_ << ") must be equal to (" << dataSources_.size()
                 << ") of dataSources (" << dataSourcesString_ << ")." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

void MxpiTensorInfer::LogModelTensorsShape(const MxBase::ModelDesc& modelDesc)
{
    for (size_t j = 0; j < modelDesc.outputTensors.size(); j++) {
        LogInfo << "Shape of output tensor: " << j << " (name: " << modelDesc.outputTensors[j].tensorName <<
                ", dtype: " << TensorDataTypeStr[model_.GetOutputDataType()[j]] << ") of model is as follow: ";
        for (size_t m = 0; m < modelDesc.outputTensors[j].tensorDims.size(); m++) {
            LogInfo << "   dim " << m << ": " << modelDesc.outputTensors[j].tensorDims[m];
        }
    }

    for (size_t j = 0; j < modelDesc.inputTensors.size(); j++) {
        LogInfo << "Shape of input tensor: " << j << " (name: " << modelDesc.inputTensors[j].tensorName <<
                ", dtype: " << TensorDataTypeStr[model_.GetInputDataType()[j]] << ") of model is as follow: ";
        for (size_t m = 0; m < modelDesc.inputTensors[j].tensorDims.size(); m++) {
            LogInfo << "   dim " << m << ": " << modelDesc.inputTensors[j].tensorDims[m];
        }
    }
}

void MxpiTensorInfer::FreeData()
{
    for (size_t j = 0; j < inputTensors_.size(); j++) {
        void* ptrData = inputTensors_[j].buf;
        size_t dataSize = inputTensors_[j].size;
        MxBase::MemoryData memoryFree(ptrData, dataSize, MxBase::MemoryData::MEMORY_DEVICE, deviceId_);
        MxBase::MemoryHelper::MxbsFree(memoryFree);
    }
    std::vector<MxBase::BaseTensor>().swap(inputTensors_);
    for (size_t j = 0; j < outputTensors_.size(); j++) {
        void* ptrData = outputTensors_[j].buf;
        size_t dataSize = outputTensors_[j].size;
        MxBase::MemoryData memoryFree(ptrData, dataSize, MxBase::MemoryData::MEMORY_DEVICE, deviceId_);
        MxBase::MemoryHelper::MxbsFree(memoryFree);
    }
    std::vector<MxBase::BaseTensor>().swap(outputTensors_);
}

APP_ERROR MxpiTensorInfer::TensorMemoryMalloc()
{
    LogDebug << "Input size : " << modelDesc_.inputTensors.size();
    for (size_t i = 0; i < modelDesc_.inputTensors.size(); i++) {
        MxBase::MemoryData memory;
        memory.size = modelDesc_.inputTensors[i].tensorSize;
        memory.deviceId = deviceId_;
        memory.type = MxBase::MemoryData::MEMORY_DEVICE;
        APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(memory);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to allocate dvpp memory." << GetErrorInfo(ret);
            FreeData();
            return ret;
        }
        MxBase::BaseTensor tensor;
        tensor.buf = memory.ptrData;
        tensor.size = memory.size;
        inputTensors_.push_back(tensor);
    }

    for (size_t i = 0; i < modelDesc_.outputTensors.size(); i++) {
        MxBase::MemoryData memory;
        memory.size = modelDesc_.outputTensors[i].tensorSize;
        memory.deviceId = deviceId_;
        memory.type = MxBase::MemoryData::MEMORY_DEVICE;
        APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(memory);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to allocate dvpp memory." << GetErrorInfo(ret);
            FreeData();
            return ret;
        }
        MxBase::BaseTensor tensor;
        tensor.buf = memory.ptrData;
        tensor.size = memory.size;
        outputTensors_.push_back(tensor);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::CheckMetaData(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    APP_ERROR ret = APP_ERR_OK;

    while (outputQueue_.size() > MAX_QUEUE_SIZE) {
        usleep(QUEUE_SLEEP_TIME);
    }
    queueMtx_.lock();
    outputQueue_.push(std::make_pair(mxpiBuffer[0], false));

    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        // Check error message.
        MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[i]);
        if (mxpiMetadataManager.GetErrorInfo() != nullptr) {
            ret = APP_ERR_COMM_FAILURE;
            LogDebug << GetErrorInfo(ret, elementName_) << "Input Port(" << i << ") has errorInfos, element("
                     << elementName_ << ") will not be executed.";
            DestroyExtraBuffers(mxpiBuffer, i);
            outputQueue_.back().first = mxpiBuffer[i];
            outputQueue_.back().second = true;
            queueMtx_.unlock();
            SendReadyData();
            return ret;
        }
        // Check metadata exist.
        if (mxpiMetadataManager.GetMetadata(dataSources_[i]) == nullptr) {
            ret = APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
            LogDebug << GetErrorInfo(ret, elementName_) << "metadata is null in input port(" << i << ").";
            DestroyExtraBuffers(mxpiBuffer, 0);
            outputQueue_.back().second = true;
            queueMtx_.unlock();
            SendReadyData();
            return ret;
        }
    }
    queueMtx_.unlock();

    return APP_ERR_OK;
}

void MxpiTensorInfer::SendReadyData()
{
    queueMtx_.lock();
    while (!outputQueue_.empty() && outputQueue_.front().second) {
        ShowQueueState(outputQueue_, elementName_);
        SendData(0, *outputQueue_.front().first);
        outputQueue_.pop();
    }
    ShowQueueState(outputQueue_, elementName_);
    queueMtx_.unlock();
}

APP_ERROR MxpiTensorInfer::ConstructTensorPackageList(
    std::shared_ptr<MxTools::MxpiTensorPackageList> &tensorPackageList,
    std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, size_t dataSourceIndex,
    std::vector<bool> &isTensorPkgFlags)
{
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[dataSourceIndex]);
    auto foo = mxpiMetadataManager.GetMetadata(dataSources_[dataSourceIndex]);
    if (foo == nullptr) {
        LogError << "Fail to get meta data." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    const google::protobuf::Descriptor* desc = ((google::protobuf::Message*)foo.get())->GetDescriptor();
    if (!desc) {
        errorInfo_ << "Get input data's descriptor failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM; // self define the error code
    }
    if (desc->name() != "MxpiTensorPackageList" && desc->name() != "MxpiVisionList") {
        errorInfo_ << "Neither MxpiVisionList nor MxpiTensorPackageList is provided from input port("
                   << dataSourceIndex << ")." << GetErrorInfo(APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST);
        LogError << errorInfo_.str();
        return APP_ERR_PLUGIN_TOOLKIT_METADATA_KEY_NOEXIST;
    }

    if (desc->name() == "MxpiTensorPackageList") {
        LogDebug << "MxpiTensorPackageList is read from input port(" << dataSourceIndex << ") successfully.";
        tensorPackageList = std::static_pointer_cast<MxTools::MxpiTensorPackageList>(foo);
        isTensorPkgFlags.push_back(true);
    } else if (desc->name() == "MxpiVisionList") {
        LogDebug << "MxpiVisionList is provided rather than MxpiTensorPackageList from inputPort(" << dataSourceIndex
                 << "). Therefore, it will be transformed into MxpiTensorPackageList automatically.";
        APP_ERROR ret = TransformVision2TensorPackage(std::static_pointer_cast<MxTools::MxpiVisionList>(foo),
                                                      tensorPackageList, model_.GetDataFormat());
        if (ret != APP_ERR_OK) {
            LogError << "Transformed into MxpiTensorPackageList Failed" << GetErrorInfo(ret);
            return ret;
        }
        isTensorPkgFlags.push_back(false);
    }
    if (tensorPackageList->tensorpackagevec_size() == 0 ||
        tensorPackageList->tensorpackagevec(0).tensorvec_size() == 0) {
        errorInfo_ << "Invalid MxpiTensorPackageList is provided from input port(" << dataSourceIndex << ")."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::GetTensorPackageLists(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
    std::vector<std::shared_ptr<MxTools::MxpiTensorPackageList>>& tensorPackageLists,
    std::vector<bool>& isTensorPkgFlags)
{
    APP_ERROR ret = APP_ERR_OK;
    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        std::shared_ptr<MxTools::MxpiTensorPackageList> tensorPackageList =
            MemoryHelper::MakeShared<MxTools::MxpiTensorPackageList>();
        if (tensorPackageList == nullptr) {
            errorInfo_ << "Fail to allocate memory." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            LogError << errorInfo_.str();
            return APP_ERR_COMM_ALLOC_MEM;
        }
        ret = ConstructTensorPackageList(tensorPackageList, mxpiBuffer, i, isTensorPkgFlags);
        if (ret != APP_ERR_OK) {
            return ret;
        }
        if (dynamicInfo_.dynamicType == MxBase::DYNAMIC_HW) {
            auto type = model_.GetDataFormat();
            auto tensorVec = tensorPackageList->tensorpackagevec(0).tensorvec(0);
            dynamicInfo_.imageSize.height =
                (unsigned int)tensorVec.tensorshape((type == MxBase::NHWC) ? NHWC_H_DIM : NHWC_W_DIM);
            dynamicInfo_.imageSize.width =
                (unsigned int)tensorVec.tensorshape((type == MxBase::NHWC) ? NHWC_W_DIM : NHWC_C_DIM);
            if (IsDenominatorZero((double)maxArea_)) {
                LogError << "The value of maxArea_ must not equal to 0!" << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
            }
            dynamicHWDiscount_ = dynamicInfo_.imageSize.height * dynamicInfo_.imageSize.width / (double)maxArea_;
        }
        tensorPackageLists.push_back(tensorPackageList);
    }
    return ret;
}

APP_ERROR MxpiTensorInfer::AssembleTensorPackageLists(
    std::vector<std::shared_ptr<MxTools::MxpiTensorPackageList>> &tensorPackageLists,
    std::vector<bool> &isTensorPkgFlags)
{
    APP_ERROR ret = APP_ERR_OK;
    int tensorPackageNumber = tensorPackageLists[0]->tensorpackagevec_size();
    for (int j = 0; j < tensorPackageNumber; j++) {
        MxpiTensorPackage mxpiTensorPackage;
        std::vector<bool> isTensorFlags = {};
        for (size_t i = 0; i < tensorPackageLists.size(); i++) {
            if (tensorPackageLists[i]->tensorpackagevec_size() != tensorPackageNumber) {
                errorInfo_ << "Number of tensorPackages should be same on inputPorts."
                           << GetErrorInfo(APP_ERR_COMM_FAILURE);
                LogError << errorInfo_.str();
                return APP_ERR_COMM_FAILURE;
            }
            for (int k = 0; k < tensorPackageLists[i]->tensorpackagevec(j).headervec_size(); k++) {
                MxpiMetaHeader *mxpiMetaHeader = mxpiTensorPackage.add_headervec();
                if (CheckPtrIsNullptr(mxpiMetaHeader, "mxpiMetaHeader"))  return APP_ERR_COMM_ALLOC_MEM;
                mxpiMetaHeader->CopyFrom(tensorPackageLists[i]->tensorpackagevec(j).headervec(k));
            }
            for (int k = 0; k < tensorPackageLists[i]->tensorpackagevec(j).tensorvec_size(); k++) {
                MxpiTensor *mxpiTensor = mxpiTensorPackage.add_tensorvec();
                if (CheckPtrIsNullptr(mxpiTensor, "mxpiTensor"))  return APP_ERR_COMM_ALLOC_MEM;
                mxpiTensor->CopyFrom(tensorPackageLists[i]->tensorpackagevec(j).tensorvec(k));
                isTensorFlags.push_back(isTensorPkgFlags[i]);
            }
        }

        // Compare between concated tensorPackage and model input tensors.
        ret = CheckInputTensors(mxpiTensorPackage, isTensorFlags);
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            return ret;
        }
        dataQueue_.push(mxpiTensorPackage);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::DataPreparation(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    APP_ERROR ret;
    // Put current buffer into buffersQueue.
    buffersQueue_.push(BuffersInfo {0, 0, mxpiBuffer, nullptr});

    std::vector<std::shared_ptr<MxTools::MxpiTensorPackageList>> tensorPackageLists;
    std::vector<bool> isTensorPkgFlags = {};
    ret = GetTensorPackageLists(mxpiBuffer, tensorPackageLists, isTensorPkgFlags);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }

    ret = AssembleTensorPackageLists(tensorPackageLists, isTensorPkgFlags);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }

    ret = ConstructBuffersInfo(tensorPackageLists[0]->tensorpackagevec_size());
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::ConstructBuffersInfo(int tensorPackageNumber)
{
    BuffersInfo& buffersInfo = buffersQueue_.back();
    buffersInfo.size = (unsigned int)tensorPackageNumber;
    auto mxpiTensorPackageList = new (std::nothrow) MxTools::MxpiTensorPackageList;
    if (mxpiTensorPackageList == nullptr) {
        errorInfo_ << "MxpiTensorPackageList out of memory." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INIT_FAIL;
    }
    buffersInfo.metaDataPtr.reset(mxpiTensorPackageList, continuousDeleter);

    for (int j = 0; j < tensorPackageNumber; j++) {
        auto tensorPackage = buffersInfo.metaDataPtr->add_tensorpackagevec();
        if (CheckPtrIsNullptr(tensorPackage, "tensorPackage"))  return APP_ERR_COMM_ALLOC_MEM;
    }
    for (size_t i = 0; i < modelDesc_.outputTensors.size(); i++) {
        size_t eachDatasize = static_cast<size_t>((static_cast<double>(modelDesc_.outputTensors[i].tensorSize)
            / maxBatchSize_) * dynamicHWDiscount_);
        MxBase::MemoryData memory;
        memory.size = eachDatasize * (unsigned int)tensorPackageNumber;
        memory.deviceId = (outputDeviceId_ == -1) ? 0 : outputDeviceId_;
        memory.type = (outputDeviceId_ == -1) ? MxBase::MemoryData::MEMORY_HOST_MALLOC :
            MxBase::MemoryData::MEMORY_DEVICE;
        APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(memory);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Fail to allocate memory." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
        LogDebug << "eachDatasize of outputTensor[" << i << "]: " << eachDatasize
                 << ". Total memory.size:" << memory.size;

        ret = ConstructTensorPackage(buffersInfo, eachDatasize, memory, tensorPackageNumber, i);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Fail to construct tensor package." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            MxBase::MemoryHelper::MxbsFree(memory);
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::ConstructTensorPackage(MxPlugins::BuffersInfo &buffersInfo, size_t eachDatasize,
    MxBase::MemoryData &memory, int tensorPackageNumber, unsigned int i)
{
    for (int j = 0; j < tensorPackageNumber; j++) {
        for (size_t k = 0; k < dataSources_.size(); k++) {
            MxTools::MxpiMetaHeader *header = buffersInfo.metaDataPtr->mutable_tensorpackagevec(j)->add_headervec();
            if (CheckPtrIsNullptr(header, "header"))  return APP_ERR_COMM_ALLOC_MEM;
            header->set_datasource(dataSources_[k]);
            header->set_memberid(j);
        }
        MxTools::MxpiTensor *mxpiTensor = buffersInfo.metaDataPtr->mutable_tensorpackagevec(j)->add_tensorvec();
        if (CheckPtrIsNullptr(mxpiTensor, "mxpiTensor"))  return APP_ERR_COMM_ALLOC_MEM;
        mxpiTensor->set_tensordataptr((uint64_t) memory.ptrData + j * eachDatasize);
        mxpiTensor->set_tensordatasize(eachDatasize);
        mxpiTensor->set_tensordatatype(model_.GetOutputDataType()[i]);
        mxpiTensor->set_deviceid((outputDeviceId_ == -1) ? 0 : outputDeviceId_);
        mxpiTensor->set_memtype((outputDeviceId_ == -1) ?
            MxTools::MxpiMemoryType::MXPI_MEMORY_HOST_MALLOC : MxTools::MxpiMemoryType::MXPI_MEMORY_DEVICE);
        mxpiTensor->set_freefunc(0);
        mxpiTensor->add_tensorshape(1);
        for (size_t k = outputHasBatchDim_; k < modelDesc_.outputTensors[i].tensorDims.size(); k++) {
            mxpiTensor->add_tensorshape(modelDesc_.outputTensors[i].tensorDims[k]);
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::CheckTensorShape(MxpiTensorPackage& tensorPackage, std::vector<bool>& isTensorFlags)
{
    APP_ERROR ret = APP_ERR_OK;
    for (int j = 0; j < tensorPackage.tensorvec_size(); j++) {
        auto modelTensorDims = modelDesc_.inputTensors[j].tensorDims;
        auto inputTensors = tensorPackage.tensorvec(j);
        if ((size_t)inputTensors.tensorshape_size() != modelTensorDims.size()) {
            ret = APP_ERR_COMM_FAILURE;
            errorInfo_ << "The shape of concated inputTensors[" << j << "] does not match model inputTensors[" << j
                       << "]" << GetErrorInfo(ret);
            for (int i = 0; i < inputTensors.tensorshape_size(); i++) {
                LogWarn << "The dimension [" << i << "] of tensor[" << j << "] is " << inputTensors.tensorshape(i);
            }
            if (isTensorFlags[j]) {
                return APP_ERR_COMM_FAILURE;
            } else {
                return ret;
            }
        }
        for (size_t k = 1; k < modelTensorDims.size() - 1; k++) {
            if ((modelTensorDims[k] == -1) || (inputTensors.tensorshape(k) == -1) ||
                (modelTensorDims[k] == inputTensors.tensorshape(k))) {
                continue;
            }
            ret = APP_ERR_COMM_FAILURE;
            errorInfo_ << "The shape of concat inputTensors[" << j
                       << "] does not match model inputTensors[" << j << "]" << std::endl << GetErrorInfo(ret);
            for (int i = 0; i < inputTensors.tensorshape_size(); i++) {
                LogWarn << "The dimension [" << i << "] of tensor[" << j << "] is " << inputTensors.tensorshape(i);
            }
            if (isTensorFlags[j]) {
                return APP_ERR_COMM_FAILURE;
            }
        }
    }
    return ret;
}

APP_ERROR MxpiTensorInfer::CheckTensorDatasize(MxpiTensorPackage& tensorPackage)
{
    APP_ERROR ret = APP_ERR_OK;
    for (int j = 0; j < tensorPackage.tensorvec_size(); j++) {
        if (dynamicInfo_.dynamicType == MxBase::DYNAMIC_HW) {
            size_t modelDatasize = static_cast<size_t>(modelDesc_.inputTensors[j].tensorSize * dynamicHWDiscount_);
            size_t tensorDatasize = (size_t)tensorPackage.tensorvec(j).tensordatasize();
            LogDebug << "modelDatasize: " << modelDatasize << ", tensorDatasize: " << tensorDatasize;
            if (modelDatasize != tensorDatasize) {
                ret = APP_ERR_COMM_FAILURE;
                errorInfo_ << "The datasize of concated inputTensors[" << j << "]("
                           << tensorDatasize << ") does not match model inputTensors[" << j << "](" << modelDatasize
                           << "). Tensor Dtype: " << TensorDataTypeStr[tensorPackage.tensorvec(j).tensordatatype()]
                           << ", model Dtype: " << TensorDataTypeStr[model_.GetInputDataType()[j]] << "."
                           << GetErrorInfo(ret);
                LogError << errorInfo_.str();
                return ret;
            }
        } else {
            size_t modelDatasize = modelDesc_.inputTensors[j].tensorSize / static_cast<size_t>(maxBatchSize_);
            size_t tensorDatasize = (size_t)tensorPackage.tensorvec(j).tensordatasize();
            LogDebug << "modelDatasize: " << modelDatasize << ", tensorDatasize: " << tensorDatasize;
            if (tensorDatasize != modelDatasize) {
                errorInfo_ << "The datasize of concated inputTensors[" << j << "]("
                           << tensorDatasize << ") does not match model inputTensors[" << j << "](" << modelDatasize
                           << "). Tensor Dtype: " << TensorDataTypeStr[tensorPackage.tensorvec(j).tensordatatype()]
                           << ", model Dtype: " << TensorDataTypeStr[model_.GetInputDataType()[j]] << "."
                           << GetErrorInfo(APP_ERR_COMM_FAILURE);
                LogError << errorInfo_.str();
                return APP_ERR_COMM_FAILURE;
            }
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::CheckInputTensors(MxpiTensorPackage& tensorPackage, std::vector<bool>& isTensorFlags)
{
    APP_ERROR ret;
    if (skipModelCheck_) {
        LogWarn << "CheckInputTensors is skipped as skipModelCheck_ set as 1. Make sure your model is correct.";
        return APP_ERR_OK;
    }
    int validInputTensorsNum = static_cast<int>((dynamicInfo_.dynamicType != MxBase::DynamicType::STATIC_BATCH) ?
                               (modelDesc_.inputTensors.size() - 1) : modelDesc_.inputTensors.size());
    if (tensorPackage.tensorvec_size() != validInputTensorsNum) {
        errorInfo_ << "The shape of concated inputTensors does not match model inputTensors."
                   << GetErrorInfo(APP_ERR_COMM_FAILURE);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_FAILURE;
    }

    ret = CheckTensorShape(tensorPackage, isTensorFlags);
    if (ret == APP_ERR_COMM_FAILURE) {
        LogWarn << errorInfo_.str();
        LogModelTensorsShape(modelDesc_);
        LogWarn << GetErrorInfo(ret)
            << "CheckTensorShape failed, which means modelInfer maybe not be executed correctly.";
    } else if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    ret = CheckTensorDatasize(tensorPackage);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }

    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::DataProcess(int rounds, int batchSize)
{
    LogDebug << "Begin to process (" << elementName_ << ") rounds:" << rounds << " batchSize:" << batchSize << ".";
    APP_ERROR ret = APP_ERR_OK;
    // realNumber means the real number of tensorPackages to send to modelInfer.
    int realNumber = (int)dataQueue_.size() <= batchSize ? (int)dataQueue_.size() : batchSize;
    for (int j = 0; j < rounds; j++) {
        for (int i = 0; i < batchSize; i++) {
            if (i >= realNumber)
                break;
            ret = DeviceMemCpy(dataQueue_.front(), i);
            dataQueue_.pop();
            if (ret != APP_ERR_OK) {
                LogError << errorInfo_.str() << GetErrorInfo(ret);
                return ret;
            }
        }
        ret = ModelInfer(batchSize, realNumber);
        if (ret != APP_ERR_OK) {
            LogError << errorInfo_.str() << GetErrorInfo(ret);
            return ret;
        }
    }
    LogDebug << "End to process DataProcess(" << elementName_ << ").";
    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::DeviceMemCpy(MxTools::MxpiTensorPackage &tensorPackage, int curPosition)
{
    for (int i = 0; i < tensorPackage.tensorvec_size(); i++) {
        MxBase::MemoryData memorySrc;
        memorySrc.size = static_cast<unsigned int>(tensorPackage.tensorvec(i).tensordatasize());
        memorySrc.ptrData = (void *)tensorPackage.tensorvec(i).tensordataptr();
        memorySrc.type = (MxBase::MemoryData::MemoryType)tensorPackage.tensorvec(i).memtype();

        MxBase::MemoryData memoryDst;
        memoryDst.size = inputTensors_[i].size / (unsigned int)maxBatchSize_;
        memoryDst.ptrData = (void *)((uint8_t *)inputTensors_[i].buf + memorySrc.size * (unsigned int)curPosition);
        memoryDst.type = MxBase::MemoryData::MEMORY_DEVICE;

        APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(memoryDst, memorySrc, memorySrc.size);
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Fail to copy to device memory." << GetErrorInfo(ret);
            LogError << errorInfo_.str();
            return ret;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::ModelInfer(int batchSize, int realNumber)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    if (dynamicInfo_.dynamicType == (unsigned int)MxBase::DYNAMIC_BATCH) {
        dynamicInfo_.batchSize = static_cast<size_t>(batchSize);
    }
    PerformanceStatisticsManager::GetInstance()->ModelInferenceStatisticsSetStartTime(streamName_, elementName_);
    APP_ERROR ret = model_.ModelInference(inputTensors_, outputTensors_, dynamicInfo_);
    PerformanceStatisticsManager::GetInstance()->ModelInferenceStatisticsSetEndTime(streamName_, elementName_);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Model inference is failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    double costTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    LogDebug << "Model inference time: " << costTime << "ms";

    ret = ConstructOutputTensor((realNumber < batchSize) ? realNumber : batchSize);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        FreeData();
        return ret;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::ConstructOutputTensor(int inferredNumber)
{
    int startIndex = 0;
    while (inferredNumber > 0) {
        int bufferRemainNumber = static_cast<int>(buffersQueue_.front().size) - buffersQueue_.front().memberId;
        bool fullFlag = inferredNumber >= bufferRemainNumber;
        int copyNumber = fullFlag ? bufferRemainNumber : inferredNumber;
        for (size_t i = 0; i < outputTensors_.size(); i++) {
            size_t eachDatasize = static_cast<size_t>(outputTensors_[i].size * dynamicHWDiscount_ / maxBatchSize_);
            MxBase::MemoryData memorySrc;
            memorySrc.size = eachDatasize * static_cast<unsigned int>(copyNumber);
            memorySrc.ptrData = (void *)((uint8_t *)outputTensors_[i].buf +
                outputTensors_[i].size / (unsigned int)maxBatchSize_ * (unsigned int)startIndex);
            memorySrc.type = MxBase::MemoryData::MEMORY_DEVICE;

            MxBase::MemoryData memoryDst;
            memoryDst.size = static_cast<unsigned int>(
                buffersQueue_.front().metaDataPtr->tensorpackagevec(0).tensorvec(i).tensordatasize() *
                bufferRemainNumber);
            memoryDst.ptrData = (void *)((uint8_t *)(buffersQueue_.front().metaDataPtr->
                tensorpackagevec(0).tensorvec(i).tensordataptr()) + eachDatasize *
                (unsigned int)buffersQueue_.front().memberId);
            memoryDst.deviceId = (outputDeviceId_ == -1) ? 0 : outputDeviceId_;
            memoryDst.type = (outputDeviceId_ == -1) ?
                             MxBase::MemoryData::MEMORY_HOST_MALLOC : MxBase::MemoryData::MEMORY_DEVICE;
            APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(memoryDst, memorySrc, memorySrc.size);
            if (ret != APP_ERR_OK) {
                errorInfo_ << "Fail to copy device memory to host for outputTensors." << GetErrorInfo(ret);
                return ret;
            }

            // actual tensorshape is only provided after model infer under DYNAMIC_HW condition.
            if (dynamicInfo_.dynamicType != MxBase::DYNAMIC_HW) {
                continue;
            }
            auto mxpiTensor = buffersQueue_.front().metaDataPtr->mutable_tensorpackagevec(0)->mutable_tensorvec(i);
            auto outputTensors = model_.GetModelDesc().outputTensors;
            for (size_t k = 1; k < outputTensors[i].tensorDims.size(); k++) {
                mxpiTensor->set_tensorshape(k, outputTensors[i].tensorDims[k]);
            }
        }
        buffersQueue_.front().memberId += copyNumber;

        if (fullFlag) {
            APP_ERROR ret = SendDataToNextPlugin(buffersQueue_.front());
            buffersQueue_.pop();
            if (ret != APP_ERR_OK) {
                return ret;
            }
        }

        startIndex += copyNumber;
        inferredNumber -= copyNumber;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiTensorInfer::SendDataToNextPlugin(MxPlugins::BuffersInfo &buffersInfo)
{
    APP_ERROR ret;
    MxTools::MxpiMetadataManager mxpiMetadataManager(*buffersInfo.buffers[0]);
    ret = mxpiMetadataManager.AddProtoMetadata(elementName_, buffersInfo.metaDataPtr);
    if (ret != APP_ERR_OK) {
        errorInfo_.str("");
        errorInfo_ << "Add metaData failed from infer results." << GetErrorInfo(ret);
        SendMxpiErrorInfo(*buffersInfo.buffers[0], elementName_, ret, errorInfo_.str());
        return ret;
    }
    SendData(0, *buffersInfo.buffers[0]);
    DestroyExtraBuffers(buffersInfo.buffers, 0);
    queueMtx_.lock();
    outputQueue_.pop();
    queueMtx_.unlock();
    SendReadyData();
    return APP_ERR_OK;
}

void MxpiTensorInfer::DestroyExtraBuffers(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, size_t exceptPort)
{
    for (size_t i = 0; i < mxpiBuffer.size(); i++) {
        if (i == exceptPort) {
            continue;
        }
        MxpiBufferManager::DestroyBuffer(mxpiBuffer[i]);
    }
}

void MxpiTensorInfer::ClearResourcesAndSendErrorInfo(APP_ERROR ret)
{
    while (!dataQueue_.empty()) {
        dataQueue_.pop();
    }
    size_t size = buffersQueue_.size();
    for (size_t index = 0; index < size; index++) {
        SendMxpiErrorInfo(*buffersQueue_.front().buffers[0], elementName_, ret, errorInfo_.str());
        DestroyExtraBuffers(buffersQueue_.front().buffers, 0);
        buffersQueue_.pop();
        queueMtx_.lock();
        outputQueue_.pop();
        queueMtx_.unlock();

        SendReadyData();
    }
    while (!buffersQueue_.empty()) {
        buffersQueue_.pop();
    }
}

void MxpiTensorInfer::CreateThread()
{
    std::function<void()> timeCall = std::bind(&MxpiTensorInfer::TimeCall, this);
    thread_ = std::thread(timeCall);
    LogInfo << "End to initialize MxpiTensorInfer(" << elementName_ << ").";
}

void MxpiTensorInfer::TimeCall(void)
{
    MxBase::DeviceManager *deviceManager = MxBase::DeviceManager::GetInstance();
    MxBase::DeviceContext deviceContext;
    deviceContext.devId = deviceId_;
    while (runningFlag_) {
        std::unique_lock<std::mutex> uniqueLock(mtx_);
        conditionVariable_.wait_until(uniqueLock, std::chrono::steady_clock::now() +
            std::chrono::milliseconds(timeTravel_), [this] {
            if (this->notifyFlag_) {
                notifyFlag_ = false;
                return false;
            }
            return true;
        });
        if (dataQueue_.size() > 0) {
            g_startEmptyTime = time(nullptr);
            APP_ERROR ret = deviceManager->SetDevice(deviceContext);
            if (ret != APP_ERR_OK) {
                LogError << "Element(" << elementName_ << ") Failed to set deviceId." << GetErrorInfo(ret);
            }
            ret = TimeoutProcess();
            if (ret != APP_ERR_OK) {
                LogError << "Element(" << elementName_ << ") Failed to process ProcessSubstitute."
                         << GetErrorInfo(ret);
            }
        } else {
            time_t now = time(nullptr);
            long interval = now - g_startEmptyTime;
            if (interval >= TIME_WARN_SECONDS) {
                LogInfo << "element(" << elementName_ << ") data queue is empty, device failure might occur.";
                g_startEmptyTime = time(nullptr);
            }
        }
        usleep(1);
    }
}

APP_ERROR MxpiTensorInfer::TimeoutProcess()
{
    APP_ERROR ret = APP_ERR_OK;
    if (dataQueue_.empty()) {
        return ret;
    }
    LogDebug << "Begin to ProcessSubstitute (" << elementName_ << ") deal with (" << dataQueue_.size() << ") data.";

    if (modelDesc_.batchSizes.empty()) {
        LogError << elementName_ << " model batchSizes are empty." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return 1;
    }
    int batchSize;
    switch (dynamicStrategy_) {
        case 0:
            batchSize = GetNearestBatchSize();
            break;
        case 1:
            batchSize = GetUpperBatchSize();
            break;
        default:
            batchSize = GetLowerBatchSize();
            break;
    }
    LogInfo << "The number of tensors (" << dataQueue_.size() << ") in buffer of plugin (" << elementName_
            << ") is not enough in given time (" << timeTravel_ << " ms) for infer with maximum batchsize."
            << " Therefore the batchsize is switched to (" << batchSize << ").";

    ret = DataProcess(1, batchSize);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        ClearResourcesAndSendErrorInfo(ret);
        return ret;
    }
    LogDebug << "End to process ProcessSubstitute (" << elementName_ << ").";
    return APP_ERR_OK;
}

int MxpiTensorInfer::GetNearestBatchSize()
{
    int diff = maxBatchSize_;
    int cur = 0;
    for (size_t i = 0; i < modelDesc_.batchSizes.size(); i++) {
        int absValue = (((int)dataQueue_.size() - (int)modelDesc_.batchSizes[i]) > 0) ?
            static_cast<int>(dataQueue_.size() - modelDesc_.batchSizes[i]) :
            static_cast<int>(modelDesc_.batchSizes[i] - dataQueue_.size());
        if (absValue <= diff) {
            diff = absValue;
            cur = static_cast<int>(i);
        }
    }
    return modelDesc_.batchSizes[cur];
}

int MxpiTensorInfer::GetUpperBatchSize()
{
    for (size_t i = 0; i < modelDesc_.batchSizes.size(); i++) {
        if (modelDesc_.batchSizes[i] >= dataQueue_.size()) {
            return modelDesc_.batchSizes[i];
        }
    }
    return maxBatchSize_;
}

int MxpiTensorInfer::GetLowerBatchSize()
{
    if (modelDesc_.batchSizes.size() <= 1) {
        return modelDesc_.batchSizes[0];
    }
    for (size_t i = 1; i < modelDesc_.batchSizes.size(); i++) {
        if (modelDesc_.batchSizes[i] > dataQueue_.size()) {
            return modelDesc_.batchSizes[i - 1];
        }
    }
    return maxBatchSize_;
}

namespace {
MX_PLUGIN_GENERATE(MxpiTensorInfer)
}
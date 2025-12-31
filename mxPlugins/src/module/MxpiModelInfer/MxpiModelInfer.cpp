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
 * Description: For target classification or detection.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxPlugins/MxpiModelInfer/MxpiModelInfer.h"
#include <iomanip>
#include <dlfcn.h>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxTools;
using namespace MxPlugins;

namespace {
const int CHECKFREQUENCY = 10;
const float SEC2MS = 1000.0;
const uint32_t MAX_INTERVAL = 100000;
const uint32_t DEINIT_WAITING_TIME = 5000000;
}

APP_ERROR MxpiModelInfer::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    // previous plugin name
    std::vector<std::string> parameterNamesPtr = {"parentName", "checkImageAlignInfo", "outputDeviceId"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    std::string parentName = *std::static_pointer_cast<std::string>(configParamMap["parentName"]);
    dataSource_ = MxPluginsGetDataSource(parentName, dataSource_, elementName_, dataSourceKeys_);
    if (dataSource_.empty()) {
        LogError << "Property dataSource is \"\", please check element(" << elementName_ << ")'s previous element."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    checkImageAlignInfo_ = *std::static_pointer_cast<std::string>(configParamMap["checkImageAlignInfo"]);
    // check image align info or not.
    if (checkImageAlignInfo_ != "on" && checkImageAlignInfo_ != "off") {
        LogError << "Format adaptation_ is " << checkImageAlignInfo_ << ", not on/off."
                 << GetErrorInfo(APP_ERR_DVPP_INVALID_FORMAT);
        return APP_ERR_DVPP_INVALID_FORMAT;
    }
    outputDeviceId_ = *std::static_pointer_cast<int>(configParamMap["outputDeviceId"]);

    ret = InitModelInfer(configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Init model inference failed." << GetErrorInfo(ret);
        return ret;
    }

    return APP_ERR_OK;
}

APP_ERROR MxpiModelInfer::InitModelInfer(std::map<std::string, std::shared_ptr<void>> &configParamMap)
{
    std::vector<std::string> parameterNamesPtr = {"modelPath", "waitingTime", "dynamicStrategy"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    ret = modelInfer_.Init(*std::static_pointer_cast<std::string>(configParamMap["modelPath"]));
    if (ret != APP_ERR_OK) {
        LogError << "Failed to init model inference." << GetErrorInfo(ret);
        return ret;
    }
    modelDesc_ = modelInfer_.GetModelDesc();
    if (!modelInfer_.GetDynamicImageSizes().empty()) {
        std::vector<ImageSize> imageSizeVec;
        for (const auto& dynamicImageSize : modelInfer_.GetDynamicImageSizes()) {
            ImageSize imageSize(dynamicImageSize.height, dynamicImageSize.width);
            imageSizeVec.push_back(imageSize);
        }
        elementDynamicImageSize_[GetElementNameWithObjectAddr()] = imageSizeVec;
    }
    LogModelTensorsShape();

    // malloc input and output device memory
    ret = MallocTensorMemory();
    if (ret != APP_ERR_OK) {
        return ret;
    }

    dataFormat_ = modelInfer_.GetDataFormat();

    timeTravel_ = *std::static_pointer_cast<uint32_t>(configParamMap["waitingTime"]);
    std::string dynamicStrategy = *std::static_pointer_cast<std::string>(configParamMap["dynamicStrategy"]);
    if (DYNAMIC_STRATEGY.find(dynamicStrategy) == DYNAMIC_STRATEGY.end()) {
        LogError << "Unknown dynamicStrategy [" << dynamicStrategy << "]."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        FreeData();
        return APP_ERR_COMM_INIT_FAIL;
    }
    dynamicStrategy_ = static_cast<int>(DYNAMIC_STRATEGY[dynamicStrategy]);
    // create thread to handle multiBatch
    CreateMultiBatchHandleThread(configParamMap);
    return APP_ERR_OK;
}

void MxpiModelInfer::LogModelTensorsShape()
{
    for (size_t j = 0; j < modelDesc_.outputTensors.size(); j++) {
        LogInfo << "Shape of output tensor: " << j << " (name: " << modelDesc_.outputTensors[j].tensorName <<
                ") of model is as follow: ";
        std::vector<int> outputTensorShape = {};
        for (size_t m = 0; m < modelDesc_.outputTensors[j].tensorDims.size(); m++) {
            LogInfo << "   dim " << m << ": " << modelDesc_.outputTensors[j].tensorDims[m];
            outputTensorShape.push_back(modelDesc_.outputTensors[j].tensorDims[m]);
        }
    }

    for (size_t j = 0; j < modelDesc_.inputTensors.size(); j++) {
        LogInfo << "Shape of input tensor: " << j << " (name: " << modelDesc_.inputTensors[j].tensorName <<
                ") of model is as follow: ";
        std::vector<int> inputTensorShape = {};
        for (size_t m = 0; m < modelDesc_.inputTensors[j].tensorDims.size(); m++) {
            LogInfo << "   dim " << m << ": " << modelDesc_.inputTensors[j].tensorDims[m];
            inputTensorShape.push_back(modelDesc_.inputTensors[j].tensorDims[m]);
        }
    }
}

APP_ERROR MxpiModelInfer::DeInit()
{
    if (GetDataSize() > 0) {
        LogDebug<< "There is " << GetDataSize() << "data still remaining in the dataList.";
        usleep(DEINIT_WAITING_TIME);
    }
    if (modelDesc_.batchSizes.size() > 1 || modelDesc_.batchSizes[0] > 1) {
        runningFlag_ = false;
        notifyFlag_ = false;
        conditionVariable_.notify_one();
        if (multiBatchHandleThread_.joinable()) {
            multiBatchHandleThread_.join();
        }
    }

    APP_ERROR ret = modelInfer_.DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to deInitialize modelInfer module." << GetErrorInfo(ret);
    }

    // release input and output memory
    for (const auto &inputTensor : inputTensors_) {
        MxBase::MemoryData memory(inputTensor.buf, inputTensor.size, MxBase::MemoryData::MEMORY_DEVICE);
        ret = MxBase::MemoryHelper::MxbsFree(memory);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to free device memory." << GetErrorInfo(ret);
        }
    }
    for (const auto &outputTensor : outputTensors_) {
        MxBase::MemoryData memory(outputTensor.buf, outputTensor.size, MxBase::MemoryData::MEMORY_DEVICE);
        ret = MxBase::MemoryHelper::MxbsFree(memory);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to free device memory." << GetErrorInfo(ret);
        }
    }

    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiModelInfer::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties;
    auto modelPath = std::make_shared<ElementProperty<std::string>>(
        ElementProperty<std::string> { STRING, "modelPath", "model", "model path", "", "", ""});
    auto parentName = std::make_shared<ElementProperty<std::string>>(
        ElementProperty<std::string> { STRING, "parentName", "name", "the key of input data", "", "", ""});
    auto dynamicStrategy = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "dynamicStrategy", "dynamic strategy",
        "strategy to choose batchsize using dynamic model: Nearest, Upper, Lower", "Nearest", "", ""
    });
    auto timeTravel = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
        INT, "waitingTime", "timeTravel", "maximun time for model infering with largest batchsize to wait for data",
        5000, 1, 1000000
    });
    auto outputDeviceId = std::make_shared<ElementProperty<int>>(ElementProperty<int> {
            INT, "outputDeviceId", "outputDeviceId",
            "the deviceId which model output tensor will be copied to. Specially, "
            "when set as -1, model output tensor will be copied to host", -1, -1, 7
    });
    auto checkImageAlignInfo = (std::make_shared<ElementProperty<std::string>>)(ElementProperty<std::string> {
        STRING, "checkImageAlignInfo", "check image align info", "yes or no to check image align info", "on", "", ""
    });
    properties = {modelPath, parentName, checkImageAlignInfo, timeTravel, dynamicStrategy, outputDeviceId};
    return properties;
}

void MxpiModelInfer::FreeData()
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

APP_ERROR MxpiModelInfer::MallocTensorMemory()
{
    for (const auto& inputTensor : modelDesc_.inputTensors) {
        MxBase::MemoryData memory(inputTensor.tensorSize, MxBase::MemoryData::MEMORY_DEVICE, deviceId_);
        APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(memory);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to allocate dvpp memory." << GetErrorInfo(ret);
            FreeData();
            return ret;
        }

        MxBase::BaseTensor tensor;
        tensor.buf = memory.ptrData;
        tensor.size = inputTensor.tensorSize;
        inputTensors_.push_back(tensor);
    }

    for (const auto& outputTensor : modelDesc_.outputTensors) {
        MxBase::MemoryData memory(outputTensor.tensorSize, MxBase::MemoryData::MEMORY_DEVICE, deviceId_);
        APP_ERROR ret = MxBase::MemoryHelper::MxbsMalloc(memory);
        if (ret != APP_ERR_OK) {
            LogError << "Fail to allocate dvpp memory." << GetErrorInfo(ret);
            FreeData();
            return ret;
        }

        MxBase::BaseTensor tensor;
        tensor.buf = memory.ptrData;
        tensor.size = outputTensor.tensorSize;
        outputTensors_.push_back(tensor);
    }
    return APP_ERR_OK;
}

void MxpiModelInfer::CreateMultiBatchHandleThread(std::map<std::string, std::shared_ptr<void>>&)
{
    if (modelDesc_.batchSizes.size() == 1 && modelDesc_.batchSizes[0] == 1) {
        LogInfo << "element(" << elementName_ << ") the model is static batch mode, batchSize="
                << modelDesc_.batchSizes[0] << ", it's not need to CreateMultiBatchHandleThread.";
        return;
    }

    std::function<void()> multiBatchHandleThreadFunc = std::bind(&MxpiModelInfer::MultiBatchHandleThreadFunc, this);
    multiBatchHandleThread_ = std::thread(multiBatchHandleThreadFunc);
    LogInfo << "element(" << elementName_ << ") End to create multiple batch thread.";
}

void MxpiModelInfer::MultiBatchHandleThreadFunc()
{
    auto deviceManager = MxBase::DeviceManager::GetInstance();
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
        if ((GetDataSize() > 0)) {
            APP_ERROR ret = deviceManager->SetDevice(deviceContext);
            if (ret != APP_ERR_OK) {
                LogError << "Element(" << elementName_ << ") Failed to set deviceId." << GetErrorInfo(ret);
            }
            ret = TimeoutProcess();
            if (ret != APP_ERR_OK) {
                LogError << "Element(" << elementName_ << ") Failed to process TimeoutProcess." << GetErrorInfo(ret);
            }
        }
        usleep(1);
    }
}

APP_ERROR MxpiModelInfer::TimeoutProcess()
{
    APP_ERROR ret = APP_ERR_OK;
    if (GetDataSize() == 0) {
        return ret;
    }

    errorInfo_.str("");
    LogDebug << "element(" << elementName_ << ") Begin to TimeoutProcess deal with (" << GetDataSize() << ") data.";

    size_t batchSize = GetBatchSize(dynamicStrategy_, modelDesc_.batchSizes, GetDataSize());
    LogInfo << "element(" << elementName_ << ") The number of images (" << GetDataSize() << ") in buffer of plugin "
        << "is not enough in given time (" << timeTravel_
        << " ms) for infer with maximum batchSize. Therefore the batchSize is switched to (" << batchSize << ").";

    size_t processSize = (GetDataSize() > batchSize) ? batchSize : GetDataSize();
    ret = DataProcess(processSize, batchSize);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    LogDebug << "element(" << elementName_ << ") End to process TimeoutProcess.";
    return APP_ERR_OK;
}

size_t MxpiModelInfer::GetBatchSize(int dynamicStrategy, const std::vector<size_t>& batchSizes, size_t dataSize)
{
    size_t batchSize = 1;
    if (batchSizes.empty()) {
        LogError << "Model batchSizes are empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return 1;
    }
    switch (dynamicStrategy) {
        case 0:
            batchSize = GetNearestBatchSize(batchSizes, dataSize);
            break;
        case 1:
            batchSize = GetUpperBatchSize(batchSizes, dataSize);
            break;
        default:
            batchSize = GetLowerBatchSize(batchSizes, dataSize);
            break;
    }
    return batchSize;
}

size_t MxpiModelInfer::GetNearestBatchSize(const std::vector<size_t>& batchSizes, size_t dataSize)
{
    int diff = batchSizes.back();
    size_t cur = 0;
    for (size_t i = 0; i < batchSizes.size(); i++) {
        int absValue = (((int)dataSize - (int)batchSizes[i]) > 0) ?
                       static_cast<int>((dataSize - batchSizes[i])) :
                       static_cast<int>((batchSizes[i] - dataSize));
        if (absValue <= diff) {
            diff = absValue;
            cur = i;
        }
    }
    return batchSizes[cur];
}

size_t MxpiModelInfer::GetUpperBatchSize(const std::vector<size_t>& batchSizes, size_t dataSize)
{
    for (size_t i = 0; i < batchSizes.size(); i++) {
        if (batchSizes[i] >= dataSize) {
            return batchSizes[i];
        }
    }
    return batchSizes.back();
}

size_t MxpiModelInfer::GetLowerBatchSize(const std::vector<size_t>& batchSizes, size_t dataSize)
{
    if (batchSizes.size() <= 1) {
        return batchSizes[0];
    }
    for (size_t i = 1; i < batchSizes.size(); i++) {
        if (batchSizes[i] > dataSize) {
            return batchSizes[i - 1];
        }
    }
    return batchSizes.back();
}

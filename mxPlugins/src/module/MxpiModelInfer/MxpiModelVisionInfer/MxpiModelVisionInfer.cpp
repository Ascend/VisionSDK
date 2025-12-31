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

#include "MxPlugins/MxpiModelInfer/MxpiModelVisionInfer/MxpiModelVisionInfer.h"
#include <iomanip>
#include <dlfcn.h>
#include <chrono>
#include "MxBase/Log/Log.h"
#include "MxBase/Utils/FileUtils.h"
#include "MxPlugins/ModelPostProcessors/ModelPostProcessorBase/MxpiModelPostProcessorBase.h"
#include "MxTools/PluginToolkit/MxpiDataTypeWrapper/MxpiDataTypeDeleter.h"
#include "MxTools/PluginToolkit/PerformanceStatistics/PerformanceStatisticsManager.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

using namespace MxTools;
using namespace MxPlugins;

namespace {
const int NHWC_H_DIM = 1;
const int NHWC_W_DIM = 2;
const int NHWC_C_DIM = 3;
const float SEC2MS = 1000.0;
const int MAX_QUEUE_SIZE = 128;
const int QUEUE_SLEEP_TIME = 100;

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
}

void MxpiModelVisionInfer::SendReadyData()
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

APP_ERROR MxpiModelVisionInfer::Init(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    LogInfo << "element(" << elementName_ << ") Begin to initialize.";
    std::vector<std::string> parameterNamesPtr = {"pictureCropName"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    // previous crop plugin name
    cropDataSource_ = *std::static_pointer_cast<std::string>(configParamMap["pictureCropName"]);
    ret = MxpiModelInfer::Init(configParamMap);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    ret = LoadPostProcessLib(configParamMap);
    if (ret != APP_ERR_OK) {
        return ret;
    }

    PerformanceStatisticsManager::GetInstance()->ModelInferenceStatisticsRegister(streamName_, elementName_);
    PerformanceStatisticsManager::GetInstance()->PostProcessorStatisticsRegister(streamName_, elementName_);
    LogInfo << "element(" << elementName_ << ") end to initialize.";
    return APP_ERR_OK;
}

APP_ERROR MxpiModelVisionInfer::CheckMetaData(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer)
{
    APP_ERROR ret = APP_ERR_OK;

    while (outputQueue_.size() > MAX_QUEUE_SIZE) {
        usleep(QUEUE_SLEEP_TIME);
    }
    queueMtx_.lock();
    outputQueue_.push(std::make_pair(mxpiBuffer[0], false));

    // Check error message.
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);
    if (mxpiMetadataManager.GetErrorInfo() != nullptr) {
        ret = APP_ERR_COMM_FAILURE;
        LogDebug << GetErrorInfo(ret, elementName_) << "Input Port(0) has errorInfos, element("
                 << elementName_ << ") will not be executed.";
        outputQueue_.back().second = true;
        queueMtx_.unlock();
        SendReadyData();
        return ret;
    }
    // Check metadata exist.
    if (mxpiMetadataManager.GetMetadata(dataSource_) == nullptr) {
        ret = APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
        LogDebug << GetErrorInfo(ret, elementName_) << "metadata is null in input port(0).";
        outputQueue_.back().second = true;
        queueMtx_.unlock();
        SendReadyData();
        return ret;
    }
    // Check metadata empty.
    auto visionList = std::static_pointer_cast<MxTools::MxpiVisionList>(
        mxpiMetadataManager.GetMetadataWithType(dataSource_, "MxpiVisionList"));
    if ((visionList != nullptr) && (visionList->visionvec_size() == 0)) {
        ret = APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
        LogDebug << GetErrorInfo(ret, elementName_) << "metadata is empty in input port(0).";
        outputQueue_.back().second = true;
        queueMtx_.unlock();
        SendReadyData();
        return ret;
    }
    queueMtx_.unlock();
    return APP_ERR_OK;
}

APP_ERROR MxpiModelVisionInfer::Process(std::vector<MxTools::MxpiBuffer *>& mxpiBuffer)
{
    LogDebug << "element(" << elementName_ << ") Begin to process.";
    errorInfo_.str("");
    auto ret = CheckMxpiBufferIsValid(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        return ret;
    }
    // Check Error info and metadata exist.
    ret = CheckMetaData(mxpiBuffer);
    if (ret != APP_ERR_OK) {
        LogDebug << "Can not get metaData from (" << dataSource_ << ").";
        return ret;
    }
    MxTools::MxpiMetadataManager mxpiMetadataManager(*mxpiBuffer[0]);
    auto visionList = std::static_pointer_cast<MxTools::MxpiVisionList>(
        mxpiMetadataManager.GetMetadataWithType(dataSource_, "MxpiVisionList"));
    ret = CheckImageInfo(visionList);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        mxpiMetadataManager.AddErrorInfo(elementName_, MxpiErrorInfo {ret, errorInfo_.str()});
        queueMtx_.lock();
        outputQueue_.back().second = true;
        queueMtx_.unlock();
        SendReadyData();
        return ret;
    }
    mtx_.lock();
    ConstructBufferList(visionList, *mxpiBuffer[0]);
    auto batchSize = modelDesc_.batchSizes.back();
    if (dataList_.size() >= batchSize) {
        int loop = static_cast<int>(dataList_.size() / batchSize);
        for (int i = 0; i < loop; ++i) {
            ret = DataProcess(batchSize, batchSize);
            if (ret != APP_ERR_OK) {
                LogError << errorInfo_.str() << GetErrorInfo(ret);
                notifyFlag_ = false;
                conditionVariable_.notify_one();
                mtx_.unlock();
                return ret;
            }
        }
    }
    notifyFlag_ = true;
    mtx_.unlock();
    conditionVariable_.notify_one();
    LogDebug << "element(" << elementName_ << ") Uninfered number of images:" << dataList_.size() << " end.";
    return ret;
}

APP_ERROR MxpiModelVisionInfer::DeInit()
{
    LogInfo << "element(" << elementName_ << ") Begin to deInitialize.";
    MxpiModelInfer::DeInit();

    if (instance_ != nullptr) {
        APP_ERROR ret = APP_ERR_OK;
        try {
            ret = instance_->DeInit();
            if (ret != APP_ERR_OK) {
                LogError << "Fail to deInitialize post process." << GetErrorInfo(ret);
            }
        } catch (const std::exception& e) {
            ret = APP_ERR_COMM_FAILURE;
            LogError << "When calling the DeInit function in the post-processing so, an exception was thrown."
                     << GetErrorInfo(ret);
        }
        // dlclose would release all resource, smart_ptr should reset will dlclose
        instance_.reset();
        dlclose(handle_);
        return ret;
    }
    LogInfo << "element(" << elementName_ << ") End to deInitialize.";
    return APP_ERR_OK;
}

std::vector<std::shared_ptr<void>> MxpiModelVisionInfer::DefineProperties()
{
    std::vector<std::shared_ptr<void>> properties = MxpiModelInfer::DefineProperties();

    auto model = std::make_shared<ElementProperty<int>>(
        ElementProperty<int> { INT, "tensorFormat", "format", "model style: NHWC-0,NCHW-1", 0, 0, 1 });
    auto cropName = std::make_shared<ElementProperty<std::string>>(
        ElementProperty<std::string> { STRING, "pictureCropName", "name", "pictureCrop name", "", "", ""});
    auto configPath = std::make_shared<ElementProperty<std::string>>(
        ElementProperty<std::string> { STRING, "postProcessConfigPath", "config", "config path", "", "", ""});
    auto configContent = std::make_shared<ElementProperty<std::string>>(ElementProperty<std::string> {
        STRING, "postProcessConfigContent", "config", "config content, its type is [json::object]", "", "", ""
    });
    auto labelPath = std::make_shared<ElementProperty<std::string>>(
        ElementProperty<std::string> { STRING, "labelPath", "label", "label path", "", "", ""});
    auto dynamicPath = std::make_shared<ElementProperty<std::string>>(
        ElementProperty<std::string> { STRING, "postProcessLibPath", "lib", "post processor lib path", "", "", ""});

    properties.push_back(model);
    properties.push_back(cropName);
    properties.push_back(configPath);
    properties.push_back(configContent);
    properties.push_back(labelPath);
    properties.push_back(dynamicPath);

    return properties;
}

MxpiPortInfo MxpiModelVisionInfer::DefineInputPorts()
{
    MxpiPortInfo inputPortInfo;
    std::vector<std::vector<std::string>> value = {{"image/yuv"}};
    GenerateStaticInputPortsInfo(value, inputPortInfo);

    return inputPortInfo;
}

MxpiPortInfo MxpiModelVisionInfer::DefineOutputPorts()
{
    MxpiPortInfo outputPortInfo;
    std::vector<std::vector<std::string>> value = {
        {"metadata/object", "metadata/class", "metadata/attribute", "metadata/feature-vector", "metadata/tensor"}
    };
    GenerateStaticOutputPortsInfo(value, outputPortInfo);

    return outputPortInfo;
}

APP_ERROR MxpiModelVisionInfer::CheckImageAlignInfo(std::shared_ptr<MxTools::MxpiVisionList>& datalist,
    int index, int indexH, int indexW)
{
    APP_ERROR ret = APP_ERR_OK;
    if (checkImageAlignInfo_ == "on") {
        if (modelDesc_.inputTensors[0].tensorDims[indexW] != datalist->visionvec(index).visioninfo().widthaligned()) {
            errorInfo_ << "Input image width("
                       << datalist->visionvec(index).visioninfo().width() << ") and model width("
                       << modelDesc_.inputTensors[0].tensorDims[indexW] << ") has to be the same as aligned width("
                       << datalist->visionvec(index).visioninfo().widthaligned()
                       << "). Please re-generate the input image, e.g., set the resize width to "
                       << datalist->visionvec(index).visioninfo().widthaligned()
                       << ". Then re-convert the model with the width("
                       << datalist->visionvec(index).visioninfo().widthaligned() << ")."
                       << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            LogWarn << errorInfo_.str();
            ret = APP_ERR_COMM_INVALID_PARAM;
        }
        if (modelDesc_.inputTensors[0].tensorDims[indexH] != datalist->visionvec(index).visioninfo().heightaligned()) {
            errorInfo_ << "Input image height("
                       << datalist->visionvec(index).visioninfo().height() << ") and model height("
                       << modelDesc_.inputTensors[0].tensorDims[indexH] << ") has to be the same as aligned height("
                       << datalist->visionvec(index).visioninfo().heightaligned()
                       << "). Please re-generate the input image, e.g., set the resize height to "
                       << datalist->visionvec(index).visioninfo().heightaligned()
                       << ". Then re-convert the model with the height("
                       << datalist->visionvec(index).visioninfo().heightaligned() << ")."
                       << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            LogWarn << errorInfo_.str();
            ret = APP_ERR_COMM_INVALID_PARAM;
        }
    }
    if (ret != APP_ERR_OK) {
        ret = CheckImageHWInfo(datalist, index, indexH, indexW);
    }
    return ret;
}

APP_ERROR MxpiModelVisionInfer::CheckImageHWInfo(std::shared_ptr<MxTools::MxpiVisionList>& datalist,
    int index, int indexH, int indexW)
{
    if (datalist->visionvec(index).visioninfo().width() != modelDesc_.inputTensors[0].tensorDims[indexW]) {
        errorInfo_ << "Input image width is [" << datalist->visionvec(index).visioninfo().width()
                   << "]. Model width is [" << modelDesc_.inputTensors[0].tensorDims[indexW]
                   << "]. Please check whether the property(tensorFormat: NHWC-0,NCHW-1) settings are correct."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (datalist->visionvec(index).visioninfo().height() != modelDesc_.inputTensors[0].tensorDims[indexH]) {
        errorInfo_ << "Input image height is [" << datalist->visionvec(index).visioninfo().height()
                   << "]. Model height is [" << modelDesc_.inputTensors[0].tensorDims[indexH]
                   << "]. Please check whether the property(tensorFormat: NHWC-0,NCHW-1) settings are correct."
                   << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        LogError << errorInfo_.str();
        return APP_ERR_COMM_INVALID_PARAM;
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiModelVisionInfer::CheckImageInfo(std::shared_ptr<MxTools::MxpiVisionList>& dataList)
{
    APP_ERROR ret = APP_ERR_OK;
    if (dataList == nullptr) {
        ret = APP_ERR_PLUGIN_TOOLKIT_METADATA_IS_NULL;
        errorInfo_ << "Not a MxpiObjectList object in metadata." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    if (modelInfer_.GetDynamicType() == MxBase::DYNAMIC_HW) {
        return APP_ERR_OK;
    }
    const size_t tensorSize = 4;
    if (modelDesc_.inputTensors.size() == 0) {
        LogError << "InputTensors is empty." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    if (modelDesc_.inputTensors[0].tensorDims.size() != tensorSize) {
        return APP_ERR_OK;
    }
    int indexW = (dataFormat_ == MxBase::NHWC) ? NHWC_W_DIM : NHWC_C_DIM;
    int indexH = (dataFormat_ == MxBase::NHWC) ? NHWC_H_DIM : NHWC_W_DIM;
    for (int i = 0; i < dataList->visionvec_size(); i++) {
        if ((int)(modelDesc_.inputTensors[0].tensorSize / modelDesc_.batchSizes.back()) !=
            dataList->visionvec(i).visiondata().datasize()) {
            errorInfo_ << "'Input image size'["
                       << dataList->visionvec(i).visiondata().datasize() << "] should be equal to 'Model image size'["
                       << modelDesc_.inputTensors[0].tensorSize / modelDesc_.batchSizes.back()
                       << "]. 'Model height'[" << modelDesc_.inputTensors[0].tensorDims[indexH]
                       << "] should be equal to either 'Input image aligned height'["
                       << dataList->visionvec(i).visioninfo().heightaligned() << "], or 'Input image height'["
                       << dataList->visionvec(i).visioninfo().height()
                       << "]. 'Model width'[" << modelDesc_.inputTensors[0].tensorDims[indexW]
                       << "] should be equal to either 'Input image aligned width'["
                       << dataList->visionvec(i).visioninfo().widthaligned() << "], or 'Input image width'["
                       << dataList->visionvec(i).visioninfo().width() << "].";
            return APP_ERR_COMM_INVALID_PARAM;
        } else {
            ret = CheckImageAlignInfo(dataList, i, indexH, indexW);
            if (ret != APP_ERR_OK) {
                return ret;
            }
        }
    }
    return APP_ERR_OK;
}

void MxpiModelVisionInfer::ConstructBufferList(std::shared_ptr<MxTools::MxpiVisionList>& visionList,
    MxTools::MxpiBuffer& buffer)
{
    BufferInfo bufferInfo;
    bufferInfo.size = static_cast<unsigned int>(visionList->visionvec_size());
    bufferInfo.buffer = &buffer;
    bufferInfo.memberId = 0;
    bufferInfo.metaDataPtr = nullptr;
    bufferInfoList_.push_back(bufferInfo);
    for (int i = 0; i < visionList->visionvec_size(); i++) {
        dataList_.push_back(visionList->visionvec(i));
    }
}

size_t MxpiModelVisionInfer::GetDataSize()
{
    return dataList_.size();
}

APP_ERROR MxpiModelVisionInfer::DataProcess(size_t processSize, size_t batchSize)
{
    LogDebug << "element(" << elementName_ << ") processSize:" << processSize << " batchSize:" << batchSize << ".";
    APP_ERROR ret = APP_ERR_OK;
    for (size_t i = 0; i < processSize; i++) {
        ret = DeviceMemCpy(dataList_[i], i);
        if (ret != APP_ERR_OK) {
            ClearResourcesAndSendErrorInfo(ret);
            return ret;
        }
    }

    struct timeval inferStartTime = {};
    struct timeval inferEndTime = {};
    gettimeofday(&inferStartTime, nullptr);
    ret = ModelInfer(batchSize, processSize);
    gettimeofday(&inferEndTime, nullptr);
    double inferCostTime = SEC2MS * (inferEndTime.tv_sec - inferStartTime.tv_sec) +
                           (inferEndTime.tv_usec - inferStartTime.tv_usec) / SEC2MS;
    LogDebug << "element(" << elementName_ << ") Time of ModelInfer: (" << inferCostTime << " ms) in plugin.";
    if (ret != APP_ERR_OK) {
        ClearResourcesAndSendErrorInfo(ret);
        return ret;
    }

    dataList_.erase(dataList_.begin(), dataList_.begin() + processSize);

    ret = SendDataToNextPlugin();
    if (ret != APP_ERR_OK) {
        return ret;
    }
    LogDebug << "element(" << elementName_ << ") End to process DataProcess.";
    return ret;
}

void MxpiModelVisionInfer::ClearResourcesAndSendErrorInfo(APP_ERROR errorCode)
{
    dataList_.clear();
    for (size_t index = 0; index < bufferInfoList_.size(); index++) {
        SendMxpiErrorInfo(*bufferInfoList_[index].buffer, elementName_, errorCode, errorInfo_.str());
        SendReadyData();
    }
    bufferInfoList_.clear();
}

APP_ERROR MxpiModelVisionInfer::SendDataToNextPlugin()
{
    APP_ERROR ret = APP_ERR_OK;
    for (int i = 0; i < static_cast<int>(bufferInfoList_.size()); i++) {
        if (bufferInfoList_[i].size == 0) {
            MxTools::MxpiMetadataManager mxpiMetadataManager(*bufferInfoList_[i].buffer);
            ret = mxpiMetadataManager.AddProtoMetadata(elementName_, bufferInfoList_[i].metaDataPtr);
            if (ret != APP_ERR_OK) {
                errorInfo_.str("");
                errorInfo_ << "Add metaData failed from infer results.";
                SendMxpiErrorInfo(*bufferInfoList_[i].buffer, elementName_, ret, errorInfo_.str());
                bufferInfoList_.erase(bufferInfoList_.begin(), bufferInfoList_.begin() + 1);
                i -= 1;
                continue;
            }
            SendData(0, *bufferInfoList_[i].buffer);
            bufferInfoList_.erase(bufferInfoList_.begin(), bufferInfoList_.begin() + 1);
            queueMtx_.lock();
            outputQueue_.pop();
            queueMtx_.unlock();
            SendReadyData();
            i -= 1;
        }
    }
    return ret;
}

APP_ERROR MxpiModelVisionInfer::DeviceMemCpy(const MxTools::MxpiVision& mxpiImageDatas, size_t index)
{
    MxBase::MemoryData memorySrc;
    memorySrc.size = static_cast<unsigned int>(mxpiImageDatas.visiondata().datasize());
    memorySrc.ptrData = (void *)mxpiImageDatas.visiondata().dataptr();
    memorySrc.type = (MxBase::MemoryData::MemoryType)mxpiImageDatas.visiondata().memtype();
    MxBase::MemoryData memoryDst;
    if (inputTensors_.empty()) {
        LogError << "InputTensors are empty. Please check." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    memoryDst.size = inputTensors_[0].size - memorySrc.size * index;
    memoryDst.ptrData = (void *)((uint8_t *)inputTensors_[0].buf + memorySrc.size * index);
    memoryDst.type = MxBase::MemoryData::MEMORY_DEVICE;
    APP_ERROR ret = MxBase::MemoryHelper::MxbsMemcpy(memoryDst, memorySrc, memorySrc.size);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Fail to copy dvpp memory." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
    }
    return ret;
}

APP_ERROR MxpiModelVisionInfer::ModelInfer(size_t batchSize, size_t processSize)
{
    MxBase::DynamicInfo dynamicInfo;
    dynamicInfo.dynamicType = modelInfer_.GetDynamicType();
    if (modelInfer_.GetDynamicType() == MxBase::DYNAMIC_HW) {
        dynamicInfo.imageSize.height = dataList_[0].visioninfo().height();
        dynamicInfo.imageSize.width = dataList_[0].visioninfo().width();
    } else if (modelInfer_.GetDynamicType() == MxBase::DYNAMIC_BATCH) {
        dynamicInfo.batchSize = batchSize;
    }

    PerformanceStatisticsManager::GetInstance()->ModelInferenceStatisticsSetStartTime(streamName_, elementName_);
    APP_ERROR ret = modelInfer_.ModelInference(inputTensors_, outputTensors_, dynamicInfo);
    PerformanceStatisticsManager::GetInstance()->ModelInferenceStatisticsSetEndTime(streamName_, elementName_);
    if (ret != APP_ERR_OK) {
        errorInfo_ << "Model inference is failed." << GetErrorInfo(ret);
        LogError << errorInfo_.str();
        return ret;
    }
    ret = ConstructOutputTensor(processSize);
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    ret = PostProcess();
    if (ret != APP_ERR_OK) {
        LogError << errorInfo_.str() << GetErrorInfo(ret);
        return ret;
    }
    return ret;
}

APP_ERROR MxpiModelVisionInfer::ConstructOutputTensor(int processSize)
{
    for (int i = 0; i < processSize; i++) {
        size_t bufferInfoIndex = 0;
        for (; bufferInfoIndex < bufferInfoList_.size(); bufferInfoIndex++) {
            if (bufferInfoList_[bufferInfoIndex].size != 0) {
                MxTools::MxpiMetaHeader head;
                head.set_datasource(dataSource_);
                head.set_memberid(bufferInfoList_[bufferInfoIndex].memberId++);
                bufferInfoList_[bufferInfoIndex].headerList.push_back(head);
                bufferInfoList_[bufferInfoIndex].size--;
                break;
            }
        }

        std::vector<MxBase::BaseTensor> tensorList;
        for (size_t j = 0; j < outputTensors_.size(); j++) {
            MxBase::BaseTensor tensor;
            tensor.buf =
                (void *)((uint8_t *)outputTensors_[j].buf +
                outputTensors_[j].size / modelDesc_.batchSizes.back() * static_cast<unsigned int>(i));
            tensor.size = outputTensors_[j].size / modelDesc_.batchSizes.back();
            for (size_t m = 0; m < modelDesc_.outputTensors[j].tensorDims.size(); m++) {
                tensor.shape.push_back(modelDesc_.outputTensors[j].tensorDims[m]);
            }
            tensorList.push_back(tensor);
        }
        bufferInfoList_[bufferInfoIndex].tensorsList.push_back(tensorList);
        MxBase::AspectRatioPostImageInfo postImageInfo;
        postImageInfo.widthResize = dataList_[i].visioninfo().width();
        postImageInfo.heightResize = dataList_[i].visioninfo().height();
        postImageInfo.keepAspectRatioScaling = 1;
        postImageInfo.resizeType = MxBase::AspectRatioPostImageInfo::RESIZER_STRETCHING;
        APP_ERROR ret = ConstructPostImageInfo(bufferInfoIndex, i, postImageInfo);
        if (ret != APP_ERR_OK) {
            LogError << "ConstructPostImageInfo failed." << GetErrorInfo(ret);
            return ret;
        }
        bufferInfoList_[bufferInfoIndex].postImageInfoList.push_back(postImageInfo);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiModelVisionInfer::ConstructPostImageInfo(size_t bufferInfoIndex, size_t dataIndex,
    MxBase::AspectRatioPostImageInfo& postImageInfo)
{
    // Get Original Image HW.
    MxTools::MxpiFrame frameData = MxpiBufferManager::GetDeviceDataInfo(*bufferInfoList_[bufferInfoIndex].buffer);
    if (frameData.visionlist().visionvec_size() == 0) {
        LogDebug << GetErrorInfo(APP_ERR_COMM_FAILURE, elementName_) << "Get Original image info failed.";
        postImageInfo.widthOriginal = postImageInfo.widthResize;
        postImageInfo.heightOriginal = postImageInfo.heightResize;
    } else {
        postImageInfo.widthOriginal = frameData.visionlist().visionvec(0).visioninfo().width();
        postImageInfo.heightOriginal = frameData.visionlist().visionvec(0).visioninfo().height();
    }
    if (dataList_[dataIndex].headervec_size() == 0) {
        return APP_ERR_OK;
    }
    auto header = dataList_[dataIndex].headervec(0);
    MxTools::MxpiMetadataManager mxpiMetadataManager(*bufferInfoList_[bufferInfoIndex].buffer);
    std::shared_ptr<MxTools::MxpiVisionList> visionListPreviousPlugin =
            std::static_pointer_cast<MxTools::MxpiVisionList>(mxpiMetadataManager.GetMetadata(dataSource_));
    if (visionListPreviousPlugin != nullptr) {
        postImageInfo.keepAspectRatioScaling =
                visionListPreviousPlugin->visionvec(header.memberid()).visioninfo().keepaspectratioscaling();
        postImageInfo.resizeType = (MxBase::AspectRatioPostImageInfo::ResizeType)
                visionListPreviousPlugin->visionvec(header.memberid()).visioninfo().resizetype();
    }
    if (!cropDataSource_.empty()) {
        std::shared_ptr<MxTools::MxpiObjectList> mxpiObjectList = std::static_pointer_cast<MxTools::MxpiObjectList>(
            mxpiMetadataManager.GetMetadataWithType(cropDataSource_, "MxpiObjectList"));
        if (mxpiObjectList == nullptr) {
            errorInfo_ << "Get previous plugin pointer failed." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
            return APP_ERR_COMM_INVALID_POINTER;
        }
        postImageInfo.x0 = mxpiObjectList->objectvec(header.memberid()).x0();
        postImageInfo.y0 = mxpiObjectList->objectvec(header.memberid()).y0();
        postImageInfo.x1 = mxpiObjectList->objectvec(header.memberid()).x1();
        postImageInfo.y1 = mxpiObjectList->objectvec(header.memberid()).y1();
        postImageInfo.heightOriginal = static_cast<unsigned int>(postImageInfo.y1 - postImageInfo.y0);
        postImageInfo.widthOriginal = static_cast<unsigned int>(postImageInfo.x1 - postImageInfo.x0);
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiModelVisionInfer::VersionProcess(unsigned int idx)
{
    auto getVersion = (MxPlugins::GetVersionFunc) dlsym(handle_, "GetCurVersion");
    if (getVersion != nullptr) {
        try {
            instance_->SetOutputTensorShapes(modelInfer_.GetModelDesc().outputTensors);
            instance_->SetAspectRatioImageInfo(bufferInfoList_[idx].postImageInfoList);
        } catch (const std::exception& e) {
            LogError << "When calling the SetOutputTensorShapes or SetAspectRatioImageInfo function in the "
                     << "post-processing so, an exception was thrown." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR MxpiModelVisionInfer::PostProcess()
{
    APP_ERROR ret = APP_ERR_OK;
    for (size_t i = 0; i < bufferInfoList_.size(); i++) {
        if (bufferInfoList_[i].tensorsList.empty()) {
            continue;
        }
        struct timeval inferStartTime = {};
        struct timeval inferEndTime = {};
        gettimeofday(&inferStartTime, nullptr);
        if (skipPostProcess_) {
            LogDebug << "As postProcessLibPath is not set, the output Tensor of modelInfer in this plugin"
                     << " will be written in metadata.";
            ret = MoveOutputTensorToHost(bufferInfoList_[i].metaDataPtr, bufferInfoList_[i].headerList,
                                         bufferInfoList_[i].tensorsList);
        } else {
            MxBase::PostProcessorImageInfo postProcessorImageInfo;
            postProcessorImageInfo.useMpPictureCrop = !cropDataSource_.empty();
            for (const auto& item : bufferInfoList_[i].postImageInfoList) {
                postProcessorImageInfo.postImageInfoVec.push_back(item);
            }
            ret = VersionProcess(i);
            if (ret != APP_ERR_OK) {
                return ret;
            }
            PerformanceStatisticsManager::GetInstance()->PostProcessorStatisticsSetStartTime(streamName_, elementName_);
            try {
                ret = instance_->Process(bufferInfoList_[i].metaDataPtr, postProcessorImageInfo,
                                         bufferInfoList_[i].headerList, bufferInfoList_[i].tensorsList);
            } catch (const std::exception& e) {
                LogError << "When calling the Process function in the post-processing so, an exception was thrown."
                         << GetErrorInfo(APP_ERR_COMM_FAILURE);
                return APP_ERR_COMM_FAILURE;
            }
            PerformanceStatisticsManager::GetInstance()->PostProcessorStatisticsSetEndTime(streamName_, elementName_);
        }
        gettimeofday(&inferEndTime, nullptr);
        double inferCostTime = SEC2MS * (inferEndTime.tv_sec - inferStartTime.tv_sec) +
                               (inferEndTime.tv_usec - inferStartTime.tv_usec) / SEC2MS;

        LogDebug << "Time of postProcess: (" << inferCostTime << " ms) in plugin (" << elementName_ << ").";
        bufferInfoList_[i].tensorsList.clear();
        bufferInfoList_[i].headerList.clear();
        bufferInfoList_[i].postImageInfoList.clear();
        if (ret != APP_ERR_OK) {
            errorInfo_ << "Model post processor is failed." << GetErrorInfo(ret);
            return ret;
        }
    }
    return ret;
}

APP_ERROR MxpiModelVisionInfer::MoveOutputTensorToHost(std::shared_ptr<void>& metaDataPtr,
    std::vector<MxTools::MxpiMetaHeader>& headerVec, std::vector<std::vector<MxBase::BaseTensor>>& tensors)
{
    if (metaDataPtr == nullptr) {
        metaDataPtr = std::static_pointer_cast<MxTools::MxpiTensorPackageList>(metaDataPtr);
        MxTools::MxpiTensorPackageList *mxpiTensorPackageList = new (std::nothrow) MxTools::MxpiTensorPackageList;
        if (mxpiTensorPackageList == nullptr) {
            errorInfo_ << "mxpiTensorPackageList out of memory." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
        }
        metaDataPtr.reset(mxpiTensorPackageList, g_deleteFuncMxpiTensorPackageList);
        metaDataPtr = std::static_pointer_cast<void>(metaDataPtr);
    }
    std::shared_ptr<MxTools::MxpiTensorPackageList> mxpiTensorPackageList =
        std::static_pointer_cast<MxTools::MxpiTensorPackageList>(metaDataPtr);

    MxTools::MxpiTensorPackage* mxpiTensorPackage = mxpiTensorPackageList->add_tensorpackagevec();
    if (CheckPtrIsNullptr(mxpiTensorPackage, "mxpiTensorPackage"))  return APP_ERR_COMM_INIT_FAIL;
    MxTools::MxpiMetaHeader* header = mxpiTensorPackage->add_headervec();
    if (CheckPtrIsNullptr(header, "header"))  return APP_ERR_COMM_INIT_FAIL;
    for (size_t i = 0; i < tensors.size(); i++) {
        header->set_datasource(headerVec[i].datasource());
        header->set_memberid(headerVec[i].memberid());
        for (size_t j = 0; j < tensors[i].size(); j++) {
            MxBase::MemoryData memorySrc(tensors[i][j].buf, tensors[i][j].size, MxBase::MemoryData::MEMORY_DEVICE);
            MxBase::MemoryData memoryDst(tensors[i][j].size, (outputDeviceId_ == -1) ?
                MxBase::MemoryData::MEMORY_HOST_MALLOC : MxBase::MemoryData::MEMORY_DEVICE,
                (outputDeviceId_ == -1) ? 0 : outputDeviceId_);
            APP_ERROR ret = MxBase::MemoryHelper::MxbsMallocAndCopy(memoryDst, memorySrc);
            if (ret != APP_ERR_OK) {
                errorInfo_ << "Fail to copy device memory to host for outputTensors." << GetErrorInfo(ret);
                return ret;
            }
            MxTools::MxpiTensor* mxpiTensor = mxpiTensorPackage->add_tensorvec();
            if (CheckPtrIsNullptr(mxpiTensor, "mxpiTensor")) {
                MxBase::MemoryHelper::MxbsFree(memoryDst);
                return APP_ERR_COMM_INIT_FAIL;
            }
            mxpiTensor->set_tensordataptr((uint64_t)memoryDst.ptrData);
            mxpiTensor->set_tensordatasize((int32_t)memoryDst.size);
            mxpiTensor->set_deviceid((outputDeviceId_ == -1) ? 0 : outputDeviceId_);
            mxpiTensor->set_memtype((outputDeviceId_ == -1) ?
                MxTools::MxpiMemoryType::MXPI_MEMORY_HOST_MALLOC : MxTools::MxpiMemoryType::MXPI_MEMORY_DEVICE);
            mxpiTensor->set_freefunc(0);
            mxpiTensor->add_tensorshape(1);
            for (size_t k = 1; k < tensors[i][j].shape.size(); k++) {
                mxpiTensor->add_tensorshape(tensors[i][j].shape[k]);
            }
        }
    }
    LogDebug << "element(" << elementName_
             << ") OutputTensor of modelInfer is moved back to host and written in metadata successfully.";
    return APP_ERR_OK;
}

std::string MxpiModelVisionInfer::GetPostCfgFile(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    std::string config;
    std::vector<std::string> parameterConfigContentPtr = {"postProcessConfigContent"};
    auto ret = CheckConfigParamMapIsValid(parameterConfigContentPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogWarn << "Config parameter map is invalid." << GetErrorInfo(ret);
        config = "";
    } else {
        config = *std::static_pointer_cast<std::string>(configParamMap["postProcessConfigContent"]);
    }

    if (config == "") {
        LogDebug << "element(" << elementName_
                 << ") not set property(postProcessConfigContent), try to read content from cfg file.";
        std::vector<std::string> parameterConfigPathPtr = {"postProcessConfigPath"};
        ret = CheckConfigParamMapIsValid(parameterConfigPathPtr, configParamMap);
        if (ret != APP_ERR_OK) {
            LogWarn << "Config parameter map is invalid." << GetErrorInfo(ret);
            return "";
        }
        config = *std::static_pointer_cast<std::string>(configParamMap["postProcessConfigPath"]);
        if (!MxBase::FileUtils::CheckFileExists(config)) {
            LogWarn << GetErrorInfo(APP_ERR_COMM_NO_EXIST, elementName_)
                    << "The Configuration file of postprocessing does not exist.";
        }
    }
    return config;
}

APP_ERROR MxpiModelVisionInfer::LoadPostProcessLib(std::map<std::string, std::shared_ptr<void>>& configParamMap)
{
    std::vector<std::string> parameterNamesPtr = {"postProcessLibPath", "labelPath"};
    auto ret = CheckConfigParamMapIsValid(parameterNamesPtr, configParamMap);
    if (ret != APP_ERR_OK) {
        LogError << "Config parameter map is invalid." << GetErrorInfo(ret);
        return ret;
    }
    std::string dynamicPath = *std::static_pointer_cast<std::string>(configParamMap["postProcessLibPath"]);
    if (dynamicPath.empty()) {
        LogWarn << "element(" << elementName_ << ") As postProcessLibPath is not set, the modelPostProcess will be "
                << "skipped and the output tensor of modelInfer will be moved back to host.";
        skipPostProcess_ = true;
        return APP_ERR_OK;
    }

    if (!CheckAndSetPostProcessLibPath(dynamicPath)) {
        return APP_ERR_COMM_NO_EXIST;
    }
    std::string realPath;
    if (!MxBase::FileUtils::RegularFilePath(dynamicPath, realPath) || !MxBase::FileUtils::IsFileValid(realPath)) {
        return APP_ERR_COMM_FAILURE;
    }
    handle_ = dlopen((realPath).c_str(), RTLD_LAZY);
    if (handle_ == nullptr) {
        LogError << "Open dynamic lib fail. [" << dlerror() << "]." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }
    std::string config = GetPostCfgFile(configParamMap);
    std::string labelPath = *std::static_pointer_cast<std::string>(configParamMap["labelPath"]);
    if (!MxBase::FileUtils::CheckFileExists(labelPath)) {
        LogWarn << GetErrorInfo(APP_ERR_COMM_NO_EXIST, elementName_)
                << "The label file of postprocessing does not exist.";
    }
    auto GetPostProcessInstance = (MxPlugins::GetInstanceFunc) dlsym(handle_, "GetInstance");
    if (GetPostProcessInstance == nullptr) {
        LogError << "Model postprocess so does not have GetInstance function which can be found by dlsym."
                 << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }
    instance_ = GetPostProcessInstance();
    if (instance_ == nullptr) {
        LogError << "Get post process instance failed." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
        return APP_ERR_COMM_OPEN_FAIL;
    }

    try {
        ret = instance_->Init(config, labelPath, modelDesc_);
        if (ret != APP_ERR_OK) {
            LogError << "Postprocessing init failed." << GetErrorInfo(ret);
            return ret;
        }
    } catch (const std::exception& e) {
        LogError << "When calling the init function in the post-processing so, an exception was thrown."
                 << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        return APP_ERR_COMM_INIT_FAIL;
    }
    return APP_ERR_OK;
}

bool MxpiModelVisionInfer::CheckAndSetPostProcessLibPath(std::string& filePath)
{
    if (MxBase::FileUtils::CheckFileExists(filePath)) {
        return true;
    }

    std::string::size_type pos = filePath.rfind('/');
    if (pos == std::string::npos) {
        LogInfo << "Postprocessing DLL of plugin (" << elementName_ << ")";
        std::string sdkHome = "/usr/local";
        auto sdkHomeEnv = std::getenv("MX_SDK_HOME");
        if (sdkHomeEnv) {
            sdkHome = sdkHomeEnv;
        }
        filePath = sdkHome + std::string("/lib/") + filePath;
        if (MxBase::FileUtils::CheckFileExists(filePath)) {
            return true;
        }
        pos = filePath.rfind('/');
    }

    LogError << "The postprocessing DLL does not exist." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
    return false;
}

namespace {
    MX_PLUGIN_GENERATE(MxpiModelVisionInfer)
}
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

#ifndef MXPI_TENSOR_INFER_H
#define MXPI_TENSOR_INFER_H

#include <queue>
#include <vector>
#include <thread>
#include <unistd.h>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxPlugins/MxpiPluginsUtils/MxpiPluginsUtils.h"

class MxpiTensorInfer : public MxTools::MxPluginBase {
public:
    /**
    * @api
    * @param configParamMap.
    * @return Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>>& configParamMap) override;

    /**
    * @api
    * @return Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @description: MxpiTensorInfer plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer) override;

    /**
    * @api
    * @brief Definition the parameter of configure properties.
    * @return std::vector<std::shared_ptr<void>>
    */
    static std::vector<std::shared_ptr<void>> DefineProperties();

    /**
    * @api
    * @brief Define the number and data type of input ports.
    * @return MxTools::MxpiPortInfo.
    */
    static MxTools::MxpiPortInfo DefineInputPorts();

    /**
    * @api
    * @brief Define the number and data type of output ports.
    * @return MxTools::MxpiPortInfo.
    */
    static MxTools::MxpiPortInfo DefineOutputPorts();

private:
    // Functions for Init.
    APP_ERROR CheckInputPortsStatus();

    APP_ERROR InitConfig(std::map<std::string, std::shared_ptr<void>> &configParamMap);

    void LogModelTensorsShape(const MxBase::ModelDesc& modelDesc);

    void FreeData();

    APP_ERROR TensorMemoryMalloc();

    // Functions for Process.
    APP_ERROR CheckMetaData(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer);

    APP_ERROR DataPreparation(std::vector<MxTools::MxpiBuffer *>& mxpiBuffer);

    APP_ERROR ConstructTensorPackageList(std::shared_ptr<MxTools::MxpiTensorPackageList>& tensorPackageList,
        std::vector<MxTools::MxpiBuffer *> &mxpiBuffer, size_t dataSourceIndex, std::vector<bool>& isTensorPkgFlags);

    APP_ERROR GetTensorPackageLists(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer,
        std::vector<std::shared_ptr<MxTools::MxpiTensorPackageList>>& tensorPackageLists,
        std::vector<bool>& isTensorPkgFlags);

    APP_ERROR AssembleTensorPackageLists(
        std::vector<std::shared_ptr<MxTools::MxpiTensorPackageList>>& tensorPackageLists,
        std::vector<bool>& isTensorPkgFlags);

    APP_ERROR CheckInputTensors(MxTools::MxpiTensorPackage& tensorPackage, std::vector<bool>& isTensorFlags);

    APP_ERROR CheckTensorShape(MxTools::MxpiTensorPackage& tensorPackage, std::vector<bool>& isTensorFlags);

    APP_ERROR CheckTensorDatasize(MxTools::MxpiTensorPackage& tensorPackage);

    APP_ERROR ConstructBuffersInfo(int tensorPackageNumber);

    APP_ERROR DataProcess(int rounds, int batchSize);

    APP_ERROR DeviceMemCpy(MxTools::MxpiTensorPackage& tensorPackage, int curPosition);

    APP_ERROR ModelInfer(int batchsize, int realNumber);

    APP_ERROR ConstructOutputTensor(int inferredNumber);

    APP_ERROR SendDataToNextPlugin(MxPlugins::BuffersInfo& buffersInfo);

    void DestroyExtraBuffers(std::vector<MxTools::MxpiBuffer *>& mxpiBuffer, size_t exceptPort);

    void ClearResourcesAndSendErrorInfo(APP_ERROR errorCode);

    // Functions for multi/dynamic batch modelInfer.
    void SendReadyData();

    void CreateThread();

    void TimeCall(void);

    APP_ERROR TimeoutProcess();

    int GetNearestBatchSize();

    int GetUpperBatchSize();

    int GetLowerBatchSize();

    APP_ERROR ConstructTensorPackage(MxPlugins::BuffersInfo &buffersInfo, size_t eachDatasize,
                                     MxBase::MemoryData &memory, int tensorPackageNumber, unsigned int i);

private:
    MxBase::ModelInferenceProcessor model_ = {};                // Infer model
    MxBase::ModelDesc modelDesc_ = {};                          // Model description
    std::queue<MxTools::MxpiTensorPackage> dataQueue_ = {};     // queue of MxpiTensorPackage
    std::queue<MxPlugins::BuffersInfo> buffersQueue_ = {};      // queue of buffer information
    std::vector<MxBase::BaseTensor> inputTensors_ = {};         // Input tensors of model
    std::vector<MxBase::BaseTensor> outputTensors_ = {};        // Output tensors of model
    std::string dataSourcesString_ = "";                        // string of dataSources
    std::vector<std::string> dataSources_ = {};                 // vector of dataSources
    std::ostringstream errorInfo_;                              // Error info logger
    int outputDeviceId_ = -1;                                   // device id of model output tensors, -1 means host.
    bool singleBatchInfer_ = true;                              // model input tensor shape has batch dimension or not
    bool outputHasBatchDim_ = true;                             // model output tensor shape has batch dimension or not
    int maxBatchSize_ = 1;

    // Members for multi/dynamic batch modelInfer.
    int runningFlag_ = true;
    uint32_t timeTravel_ = 0;
    std::thread thread_;
    std::mutex mtx_;

    bool notifyFlag_ = false;
    std::condition_variable conditionVariable_;                 // condition variable to control thread_ to check data
    int dynamicStrategy_ = 0;                                   // strategy to choose batchsize
    int skipModelCheck_ = 0;                                    // skip model check if 1
    std::queue<std::pair<MxTools::MxpiBuffer*, bool>> outputQueue_ = {}; // queue of buffers to send
    std::mutex queueMtx_;                                       // mutex of queue of buffers to send

    // Members for dynamic HW image modelInfer.
    MxBase::DynamicInfo dynamicInfo_;
    std::vector<MxTools::ImageSize> dynamicHWs_ = {};
    size_t maxArea_ = 1;
    double dynamicHWDiscount_ = 1.0;
};

#endif

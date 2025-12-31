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

#ifndef MP_MODEL_INFER_H
#define MP_MODEL_INFER_H

#include <vector>
#include <sys/time.h>
#include <thread>
#include <unistd.h>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/MemoryHelper/MemoryHelper.h"
#include "MxBase/ModelInfer/ModelInferenceProcessor.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxBase/DeviceManager/DeviceManager.h"

struct BufferInfo {
    size_t size;
    int memberId;
    MxTools::MxpiBuffer* buffer;
    std::shared_ptr<void> metaDataPtr;
    std::vector<MxTools::MxpiMetaHeader> headerList;
    std::vector<std::vector<MxBase::BaseTensor>> tensorsList;
    std::vector<MxBase::AspectRatioPostImageInfo> postImageInfoList;
};

class MxpiModelInfer : public MxTools::MxPluginBase {
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
    * @api
    * @brief Definition the parameter of configure properties.
    * @return Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*>& mxpiBuffer) = 0;

    /**
    * @api
    * @brief Definition the parameter of configure properties.
    * @return std::vector<std::shared_ptr<void>>
    */
    static std::vector<std::shared_ptr<void>> DefineProperties();

    /**
     * get infer data size
     * @return data size
     */
    virtual size_t GetDataSize() = 0;

    virtual APP_ERROR DataProcess(size_t processSize, size_t batchSize) = 0;

protected:
    static size_t GetBatchSize(int dynamicStrategy, const std::vector<size_t>& batchSizes, size_t dataSize);

protected:
    MxBase::ModelInferenceProcessor modelInfer_ = {};        // Infer model
    MxBase::ModelDesc modelDesc_ = {};
    std::vector<MxBase::BaseTensor> inputTensors_;           // input information
    std::vector<MxBase::BaseTensor> outputTensors_;          // output tensor
    int dynamicStrategy_ = 0;      // strategy to choose batchSize.
    std::ostringstream errorInfo_;
    bool runningFlag_ = true;
    struct timeval startTime_ = {UINT32_MAX, 0};
    uint32_t timeTravel_;
    std::thread multiBatchHandleThread_;
    std::mutex mtx_;
    bool notifyFlag_ = false;
    std::condition_variable conditionVariable_;   // condition variable to control the threads
    int outputDeviceId_ = -1;
    MxBase::DataFormat dataFormat_;
    std::string checkImageAlignInfo_;

private:
    APP_ERROR InitModelInfer(std::map<std::string, std::shared_ptr<void>> &configParamMap);

    void LogModelTensorsShape();

    void FreeData();

    APP_ERROR MallocTensorMemory();

    // Create a thread when the batchSize of Model is greater than 1.
    void CreateMultiBatchHandleThread(std::map<std::string, std::shared_ptr<void>>& configParamMap);

    // Function for modelInfer if existing data is not enough to form a batch.
    void MultiBatchHandleThreadFunc();

    APP_ERROR TimeoutProcess();

    static size_t GetNearestBatchSize(const std::vector<size_t>& batchSizes, size_t dataSize);

    static size_t GetUpperBatchSize(const std::vector<size_t>& batchSizes, size_t dataSize);

    static size_t GetLowerBatchSize(const std::vector<size_t>& batchSizes, size_t dataSize);
};

#endif

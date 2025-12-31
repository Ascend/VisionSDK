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

#ifndef MP_MODEL_VISION_INFER_H
#define MP_MODEL_VISION_INFER_H

#include "MxPlugins/MxpiModelInfer/MxpiModelInfer.h"
#include "MxPlugins/ModelPostProcessors/ModelPostProcessorBase/MxpiModelPostProcessorBase.h"

class MxpiModelVisionInfer : public MxpiModelInfer {
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

    /**
     * get infer data size
     * @return data size
     */
    size_t GetDataSize() override;

    /**
     * handle infer data
     * @param processSize process image size
     * @param batchSize batch size
     */
    APP_ERROR DataProcess(size_t processSize, size_t batchSize) override;

private:
    APP_ERROR CheckMetaData(std::vector<MxTools::MxpiBuffer *> &mxpiBuffer);

    APP_ERROR CheckImageInfo(std::shared_ptr<MxTools::MxpiVisionList>& dataList);

    void ConstructBufferList(std::shared_ptr<MxTools::MxpiVisionList>& versionList, MxTools::MxpiBuffer& buffer);

    APP_ERROR DeviceMemCpy(const MxTools::MxpiVision& mxpiImageDatas, size_t index);

    APP_ERROR ModelInfer(size_t batchSize, size_t processSize);

    APP_ERROR ConstructOutputTensor(int processSize);

    APP_ERROR ConstructPostImageInfo(size_t bufferInfoIndex, size_t dataIndex,
                                     MxBase::AspectRatioPostImageInfo& postImageInfo);

    APP_ERROR PostProcess();

    APP_ERROR MoveOutputTensorToHost(std::shared_ptr<void>& metaDataPtr,
                                     std::vector<MxTools::MxpiMetaHeader>& headerVec,
                                     std::vector<std::vector<MxBase::BaseTensor>>& tensors);

    APP_ERROR TimeoutProcess();

    APP_ERROR SendDataToNextPlugin();

    APP_ERROR CheckImageAlignInfo(std::shared_ptr<MxTools::MxpiVisionList>& datalist, int index, int indexH,
                                  int indexW);

    APP_ERROR CheckImageHWInfo(std::shared_ptr<MxTools::MxpiVisionList>& datalist, int index, int indexH, int indexW);

    APP_ERROR LoadPostProcessLib(std::map<std::string, std::shared_ptr<void>>& configParamMap);

    // If only the lib name is configured, the value of the ${MX_SDK_HOME}+"/lib" is added to the path
    bool CheckAndSetPostProcessLibPath(std::string& filePath);

    void ClearResourcesAndSendErrorInfo(APP_ERROR errorCode);

    void SendReadyData();

    std::string GetPostCfgFile(std::map<std::string, std::shared_ptr<void>>& configParamMap);

    APP_ERROR VersionProcess(unsigned int idx);
private:
    std::vector<BufferInfo> bufferInfoList_ = {};   // Model description
    std::string cropDataSource_;          // PictureCrop plugin name
    std::vector<MxTools::MxpiVision> dataList_;
    void* handle_ = nullptr;               // Dlopen handle
    bool skipPostProcess_ = false;  // whether to skip PostProcess()
    std::shared_ptr<MxPlugins::MxpiModelPostProcessorBase> instance_;
    std::queue<std::pair<MxTools::MxpiBuffer*, bool>> outputQueue_ = {};
    std::mutex queueMtx_;
};

#endif

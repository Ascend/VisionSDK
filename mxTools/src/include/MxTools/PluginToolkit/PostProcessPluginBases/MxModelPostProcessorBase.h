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
 * Description: Interface of the base class of the model post-processing plug-in.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#ifndef MXMODELPOSTPROCESSOR_PLUGINBASE
#define MXMODELPOSTPROCESSOR_PLUGINBASE

#include <atomic>
#include <tuple>
#include <dlfcn.h>
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"
#include "MxBase/Tensor/TensorBase/TensorBase.h"
#include "MxBase/PostProcessBases/PostProcessBase.h"
#include "MxTools/Proto/MxpiDataType.pb.h"
#include "MxTools/PluginToolkit/base/MxPluginGenerator.h"
#include "MxTools/PluginToolkit/buffer/MxpiBufferManager.h"
#include "MxTools/PluginToolkit/metadata/MxpiMetadataManager.h"

namespace MxTools {
class MxModelPostProcessorBase : public MxTools::MxPluginBase {
public:
    /**
    * @description: Init configs, get postProcess instance.
    * @param configParamMap: config.
    * @return: Error code.
    */
    APP_ERROR Init(std::map<std::string, std::shared_ptr<void>> &configParamMap) override;

    /**
    * @description: DeInit postProcess instance.
    * @return: Error code.
    */
    APP_ERROR DeInit() override;

    /**
    * @description: MxModelPostProcessorBase plugin process.
    * @param mxpiBuffer: data receive from the previous.
    * @return: Error code.
    */
    APP_ERROR Process(std::vector<MxTools::MxpiBuffer*> &mxpiBuffer) override;

    /**
    * @description: MxModelPostProcessorBase plugin define properties.
    * @return: properties.
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

protected:
    APP_ERROR OpenPostProcessLib(std::map<std::string, std::shared_ptr<void>> &configParamMap);
    APP_ERROR OpenPostProcessLib(
        std::map<std::string, std::shared_ptr<void>> &configParamMap, const std::string &postProcessDllName);
    bool CheckPostProcessLibPath(std::string &filePath);
    APP_ERROR InitConfig(std::map<std::string, std::shared_ptr<void>> &configParamMap);
    APP_ERROR ConstructTensor(std::shared_ptr<MxTools::MxpiTensorPackageList>& tensorPackageList,
            std::vector<MxBase::TensorBase>& tensors);

    template<typename T>
    APP_ERROR InitPostProcessInstance(std::map<std::string, std::shared_ptr<void>> &,
                                      std::string getInstanceName)
    {
        APP_ERROR ret = APP_ERR_OK;
        auto GetPostProcessInstance = (T) dlsym(handle_, getInstanceName.c_str());
        if (GetPostProcessInstance == nullptr) {
            LogError << "Model postprocess so does not have corresponding function."
                     << "Please check the correspondence between modelpostprocessor plugin and postprocess .so."
                     << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
            return APP_ERR_COMM_OPEN_FAIL;
        }
        instance_ = GetPostProcessInstance();
        if (instance_ == nullptr) {
            LogError << "Get post process instance failed." << GetErrorInfo(APP_ERR_COMM_OPEN_FAIL);
            return APP_ERR_COMM_OPEN_FAIL;
        }
        try {
            ret = instance_->Init(postConfigParamMap_);
            if (ret != APP_ERR_OK) {
                LogError << "Postprocessing init failed." << GetErrorInfo(ret);
                return ret;
            }
        } catch (const std::exception& e) {
            LogError << " When calling the Init function in the "
                     << "post-processing so, an exception was thrown." << GetErrorInfo(APP_ERR_COMM_FAILURE) ;
            return APP_ERR_COMM_FAILURE;
        }
        return APP_ERR_OK;
    }

protected:
    std::ostringstream errorInfo_;
    void* handle_ = nullptr;               // Dlopen handle
    std::shared_ptr<MxBase::PostProcessBase> instance_;
    std::vector<MxBase::TensorBase> tensors_ = {};
    std::string funcLanguage_;
    std::string className_;
    std::string pythonModule_;
    std::string configPath_;
    std::string labelPath_;
    std::string postProcessLibPath_;
    std::map<std::string, std::string> postConfigParamMap_ = {};
};
}
#endif // MXMODELPOSTPROCESSOR_PLUGINBASE
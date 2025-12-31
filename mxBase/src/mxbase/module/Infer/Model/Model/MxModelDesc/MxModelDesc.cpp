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
 * Description: Private interface of the Model for internal use only.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#include "acl/acl.h"
#include "acl/acl_base.h"
#include "MxModelDesc.h"

namespace MxBase {
APP_ERROR MxModelDesc::Init(ModelLoadOptV2 &, const int32_t)
{
    LogWarn << "Init has not been re-written.";
    return APP_ERR_OK;
}

APP_ERROR MxModelDesc::SetDevice()
{
    LogWarn << "SetDevice has not been re-written.";
    return APP_ERR_OK;
}

APP_ERROR MxModelDesc::Infer(std::vector<Tensor>&, std::vector<Tensor>&,
                             AscendStream&)
{
    LogWarn << "Infer has not been re-written.";
    return APP_ERR_OK;
}

std::vector<Tensor> MxModelDesc::Infer(std::vector<Tensor>&)
{
    LogWarn << "Infer has not been re-written.";
    return {};
}

uint32_t MxModelDesc::GetInputTensorNum() const
{
    LogWarn << "GetInputTensorNum has not been re-written.";
    return 0;
}

uint32_t MxModelDesc::GetOutputTensorNum() const
{
    LogWarn << "GetOutputTensorNum has not been re-written.";
    return 0;
}

std::vector<int64_t> MxModelDesc::GetInputTensorShape(uint32_t) const
{
    LogWarn << "GetInputTensorShape has not been re-written.";
    return {};
}

std::vector<uint32_t> MxModelDesc::GetOutputTensorShape(uint32_t) const
{
    LogWarn << "GetOutputTensorShape has not been re-written.";
    return {};
}

MxBase::TensorDType MxModelDesc::GetInputTensorDataType(uint32_t) const
{
    LogWarn << "GetInputTensorDataType has not been re-written.";
    return MxBase::TensorDType::UNDEFINED;
}

MxBase::TensorDType MxModelDesc::GetOutputTensorDataType(uint32_t) const
{
    LogWarn << "GetOutputTensorDataType has not been re-written.";
    return MxBase::TensorDType::UNDEFINED;
}

MxBase::VisionDataFormat MxModelDesc::GetInputFormat() const
{
    LogWarn << "GetOutputTensorDataType has not been re-written.";
    return MxBase::VisionDataFormat::NCHW;
}

std::vector<std::vector<uint64_t>> MxModelDesc::GetDynamicGearInfo() const
{
    LogWarn << "GetDynamicGearInfo has not been re-written.";
    return {};
}

VisionDynamicType MxModelDesc::GetDynamicType() const
{
    LogWarn << "GetDynamicType has not been re-written.";
    return {};
}

MxModelDesc::~MxModelDesc()
{
}

APP_ERROR CheckDeviceId(const int32_t deviceId)
{
    uint32_t deviceCount = 0;
    APP_ERROR ret = aclrtGetDeviceCount(&deviceCount);
    if (ret != APP_ERR_OK) {
        LogError << "Get device count failed." << GetErrorInfo(ret, "aclrtGetDeviceCount");
        return APP_ERR_ACL_FAILURE;
    }
    if (deviceId < 0 || deviceId >= (int32_t)deviceCount) {
        if (deviceId == -1) {
            LogError << "Device Id(-1) is reserved text, please set device Id in range[0, "
                << (deviceCount - 1) << "]." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        } else {
            LogError << "Get wrong device Id(" << deviceId << "), which should be in range[0, "
                << (deviceCount - 1) << "]." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }
    }
    return APP_ERR_OK;
}

APP_ERROR CheckFile(std::string& filePath)
{
    std::string realPath;
    if (!FileUtils::RegularFilePath(filePath, realPath)) {
        LogError << "Failed to get model, the model path is invalidate." << GetErrorInfo(APP_ERR_COMM_NO_EXIST);
        return APP_ERR_COMM_NO_EXIST;
    }

    filePath = realPath;
    if (!FileUtils::IsFileValid(filePath, false)) {
        LogError << "Please check the size of model." << GetErrorInfo(APP_ERR_ACL_INVALID_FILE);
        return APP_ERR_ACL_INVALID_FILE;
    }
    return APP_ERR_OK;
}

std::string GetPathExtension(std::string& filePath)
{
    size_t position = filePath.rfind(".");
    if (position == std::string::npos) {
        return "";
    }
    return filePath.substr(position, filePath.length());
}

}
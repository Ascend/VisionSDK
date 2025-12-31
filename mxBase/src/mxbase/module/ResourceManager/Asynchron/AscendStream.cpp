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
* Description: Manage Acl Stream.
* Author: MindX SDK
* Create: 2023
* History: NA
*/
#include <list>
#include <mutex>
#include <unistd.h>
#include <algorithm>
#include <unordered_map>
#include "acl/acl.h"
#include "acl/dvpp/hi_dvpp.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "ResourceManager/StreamTensorManager/StreamTensorManager.h"
#include "MxBase/Asynchron/AscendStream.h"


namespace MxBase {
constexpr int SLEEP_TIME = 500;
constexpr size_t WAIT_TIMES = 100;
constexpr int CALLBACK_WAIT_TIMES = 100;
constexpr char MAGIC_NUMBER = 'A';
static std::mutex g_ascendStreamMtx;

// MagicNumber is used to verify whether mxBaseUserData is corrupted
// MagicNumber and reserved array are used to substitute original corrupted data
struct MxBaseUserData {
    Tensor lastTensor;
    aclrtStream stream;
    char magicNumber = 'A';
    char reserved[15];
};

void RecycleTensorCallback(void *userData)
{
    MxBaseUserData *mxBaseData = static_cast<MxBaseUserData*>(userData);

    if (mxBaseData->magicNumber != MAGIC_NUMBER) {
        LogError << "Memory corruption detected in MxBaseUserData: magicNumber="
                 << mxBaseData->magicNumber << ". Expected: " << MAGIC_NUMBER
                 << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }

    if (StreamTensorManager::GetInstance()->DeleteTensor(mxBaseData->stream, mxBaseData->lastTensor) != APP_ERR_OK) {
        LogError << "Fail to delete tensor in MxBaseCallback." << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
    delete mxBaseData;
}

AscendStream::AscendStream(int32_t deviceId):AscendStream(deviceId, FlagType::LAUNCH_SYNC)
{
}

AscendStream::AscendStream(int32_t deviceId, AscendStream::FlagType flag)
{
    deviceId_ = deviceId;
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->CheckDeviceId(deviceId);
    if (ret != APP_ERR_OK) {
        LogError << "Device id is out of range, current deviceId is " << deviceId << "." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    if (flag < AscendStream::FlagType::DEFAULT || flag > AscendStream::FlagType::LAUNCH_SYNC) {
        LogError << "Flag must be in FlagType, failed to create ascend stream." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    DeviceContext device = {};
    device.devId = deviceId;
    ret = MxBase::DeviceManager::GetInstance()->SetDevice(device);
    if (ret != APP_ERR_OK) {
        LogError << "Set current context failed." << GetErrorInfo(ret);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    LogDebug << "SetDevice ret is " << ret << ", deviceId is " << deviceId;
    ret = aclrtCreateStreamWithConfig(&stream, 0, flag);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to create ascend stream." << GetErrorInfo(ret, "aclrtCreateStreamWithConfig");
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
    param_ = std::make_shared<CallbackParam>();
    ret = StreamTensorManager::GetInstance()->AddStream(stream);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to add ascend stream for StreamTensorManager." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        throw std::runtime_error(GetErrorInfo(APP_ERR_COMM_INIT_FAIL));
    }
}

AscendStream::~AscendStream()
{
    APP_ERROR ret = APP_ERR_OK;

    if (param_->isSuccessSetContext && td_.use_count() == 1) {
        ret = aclrtUnSubscribeReport(tid, stream);
        param_->isExit = true;
        if (ret != APP_ERR_OK) {
            LogError << "AclrtUnSubscribeReport is failed." << GetErrorInfo(ret, "aclrtUnSubscribeReport");
        }
        if (td_->joinable()) {
            td_->join();
        }
        td_.reset();
        param_->isSuccessSetContext = false;
    }
}

APP_ERROR AscendStream::GetChannel(int* channelId)
{
    if (channelId == nullptr) {
        LogError << "Failed to get AscendStream channel." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    *channelId = chnId_;
    return APP_ERR_OK;
}

int32_t AscendStream::GetDeviceId() const
{
    return deviceId_;
}

APP_ERROR AscendStream::CreateChannel()
{
    if (!DeviceManager::IsAscend310P() && !DeviceManager::IsAscend310B() && !DeviceManager::IsAtlas800IA2()) {
        LogError << "CreateChannel() is supported on device 310P/310B/Atlas800IA2 now, current device is "
                 << DeviceManager::GetSocName() << ".";
        return APP_ERR_COMM_INIT_FAIL;
    }
    DeviceContext device = {};
    device.devId = deviceId_;
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->SetDevice(device);
    if (ret != APP_ERR_OK) {
        LogError << "SetDevice failed when create channel." << GetErrorInfo(ret);
        return APP_ERR_COMM_INIT_FAIL;
    }
    if (chnId_ == -1) {
        ret = hi_mpi_sys_init();
        if (ret != APP_ERR_OK) {
            LogError << "Failed to initialize dvpp system." << GetErrorInfo(ret, "hi_mpi_sys_init");
            return APP_ERR_COMM_INIT_FAIL;
        }

        hi_vpc_chn_attr stChnAttr;
        hi_vpc_chn channelId;
        stChnAttr.attr = 0;

        ret = hi_mpi_vpc_sys_create_chn(&channelId, &stChnAttr);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to create vpc hi_mpi_vpc_sys." << GetErrorInfo(ret, "hi_mpi_vpc_sys_create_chn");
            return APP_ERR_ACL_FAILURE;
        }

        chnId_ = channelId;

        LogInfo << "Create vpc channel success. channel id is " << chnId_ << ".";
    }
    return ret;
}

APP_ERROR AscendStream::DoProcessCallback()
{
    param_->isExit = false;
    param_->isSuccessSetContext = false;
    param_->deviceId = deviceId_;

    td_ = std::make_shared<std::thread>(ProcessCallback, param_.get());
    if (td_.get() == nullptr) {
        LogError << "Create td ptr failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    int ret = pthread_setname_np(td_->native_handle(), "mx_stream");
    if (ret != 0) {
        LogError << "Failed to set stream listen thread name." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return APP_ERR_COMM_FAILURE;
    }
    std::ostringstream oss;
    oss << td_->get_id();
    tid = std::stoull(oss.str());

    return APP_ERR_OK;
}

APP_ERROR AscendStream::CreateAscendStream()
{
    if (td_ != nullptr) {
        LogError << "Do not call the CreateAscendStream interface repeatedly."
                 << GetErrorInfo(APP_ERR_COMM_REPEAT_INITIALIZE);
        return APP_ERR_COMM_REPEAT_INITIALIZE;
    }
    APP_ERROR ret = DoProcessCallback();
    if (ret != APP_ERR_OK) {
        LogError << "Do ProcessCallback failed." << GetErrorInfo(ret);
        return ret;
    }

    ret = aclrtSubscribeReport(static_cast<uint64_t>(tid), stream);
    if (ret != APP_ERR_OK) {
        LogError << "AclrtSubscribeReport failed." << GetErrorInfo(ret, "aclrtSubscribeReport");
        param_->isExit = true;
        td_->join();
        td_.reset();
        return APP_ERR_ACL_FAILURE;
    }

    // Wait for setting context
    for (size_t i = 0; i < WAIT_TIMES; i++) {
        if (param_->isSuccessSetContext) {
            return ret;
        }
        usleep(SLEEP_TIME);
    }

    // Release resource when setting context is failed
    param_->isExit = true;
    td_->join();
    td_.reset();

    return APP_ERR_COMM_TIMEOUT;
}

void AscendStream::ProcessCallback(void *arg)
{
    CallbackParam *param = static_cast<CallbackParam*>(arg);
    DeviceContext device = {};
    device.devId = param->deviceId;
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->SetDevice(device);
    if (ret != APP_ERR_OK) {
        LogError << "Acl set current context failed." << GetErrorInfo(ret);
        return;
    } else {
        param->isSuccessSetContext = true;
    }
    while (true) {
        (void) aclrtProcessReport(CALLBACK_WAIT_TIMES);
        if (param->isExit) {
            param->isSuccessSetContext = false;
            return;
        }
    }
}

AscendStream &AscendStream::DefaultStream()
{
    static bool isFirstInit = true;
    static DeviceContext device = {};
    if (isFirstInit) {
        std::lock_guard<std::mutex> lock(g_ascendStreamMtx);
        device.devId = 0;
        APP_ERROR ret = MxBase::DeviceManager::GetInstance()->GetCurrentDevice(device);
        if (ret != APP_ERR_OK) {
            LogDebug << "Fail to get current device." << GetErrorInfo(ret);
        }
    }

    static AscendStream defaultStream(device.devId);

    if (isFirstInit) {
        std::lock_guard<std::mutex> lock(g_ascendStreamMtx);
        defaultStream.isDefault_ = true;
        isFirstInit = false;
    }

    // in case that default stream be destroyed by DestroyStream or MxDeInit
    if (defaultStream.stream == nullptr) {
        std::lock_guard<std::mutex> lock(g_ascendStreamMtx);
        defaultStream = AscendStream(device.devId);
        defaultStream.isDefault_ = true;
        if (defaultStream.stream == nullptr) {
            LogError << "Failed to recreate default ascend stream." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
        }
    }
    return defaultStream;
}

static void FinishedProcess(void *)
{
    LogInfo << "Ready to destroy stream.";
}

APP_ERROR AscendStream::DestroyAscendStream()
{
    DeviceContext device = {};
    device.devId = deviceId_;
    APP_ERROR ret = MxBase::DeviceManager::GetInstance()->SetDevice(device);
    if (ret != APP_ERR_OK) {
        LogError << "SetDevice failed when destroy stream." << GetErrorInfo(ret);
    }
    // wait user stream complete
    if (Synchronize() != APP_ERR_OK) {
        LogError << "User stream synchronize failed when destroy stream." << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
    if (param_->isSuccessSetContext) {
        param_->isExit = true;
        ret = aclrtLaunchCallback(FinishedProcess, nullptr, ACL_CALLBACK_NO_BLOCK, stream);
        if (ret != APP_ERR_OK) {
            LogError << "Execute launch callback failed in finish process." << GetErrorInfo(ret, "aclrtLaunchCallback");
        }
        if (Synchronize() != APP_ERR_OK) {
            LogError << "Synchronize failed in finish process." << GetErrorInfo(ret);
        }
        td_->join();
        ret = aclrtUnSubscribeReport(tid, stream);
        if (ret != APP_ERR_OK) {
            LogError << "AclrtUnSubscribeReport is failed." << GetErrorInfo(ret, "aclrtUnSubscribeReport");
        }
        td_.reset();
    }

    if (chnId_ != -1) {
        LogInfo << "DeInit with mode [DVPP_CHNMODE_VPC]";

        ret = hi_mpi_vpc_destroy_chn(chnId_);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to destroy Vpc channel." << GetErrorInfo(ret, "hi_mpi_vpc_destroy_chn");
        }
        ret = hi_mpi_sys_exit();
        if (ret != APP_ERR_OK) {
            LogError << "Failed to exit dvpp system." << GetErrorInfo(ret, "hi_mpi_sys_exit");
        }
        chnId_ = -1;
    }
    if (StreamTensorManager::GetInstance()->DeleteStream(stream) != APP_ERR_OK) {
        LogError << "Fail to delete stream resource in StreamTensorManager" << GetErrorInfo(APP_ERR_COMM_FAILURE);
    }
    if (stream != nullptr) {
        ret = aclrtDestroyStream(stream);
        stream = nullptr;
        if (ret != APP_ERR_OK) {
            LogError << "AclrtDestroyStream execution failed." << GetErrorInfo(ret, "aclrtDestroyStream");
            return APP_ERR_ACL_FAILURE;
        }
    }
    return ret;
}

APP_ERROR AscendStream::Synchronize() const
{
    APP_ERROR ret = aclrtSynchronizeStream(stream);
    if (ret != APP_ERR_OK) {
        LogError << "Synchronize stream execution failed." << GetErrorInfo(ret, "aclrtSynchronizeStream");
        return APP_ERR_ACL_FAILURE;
    }
    ret = StreamTensorManager::GetInstance()->ClearTensorsByStream(stream);
    if (ret != APP_ERR_OK) {
        LogError << "Fail to clean tensors of stream in synchronize" << GetErrorInfo(ret);
    }
    return ret;
}

void AscendStream::SetErrorCode(APP_ERROR errCode)
{
    if (isDefault_) {
        return;
    }
    if (errCodeLogger.first == APP_ERR_OK) {
        errCodeLogger.first = errCode;
    } else {
        errCodeLogger.second = errCode;
    }
}

std::pair<APP_ERROR, APP_ERROR> AscendStream::GetErrorCode()
{
    return errCodeLogger;
}

APP_ERROR AscendStream::LaunchCallBack(aclrtCallback fn, void* userData)
{
    if (fn == nullptr) {
        LogError << "Function is nullptr."<< GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    if (stream == nullptr) {
        LogError << "AclrtStream is nullptr." << GetErrorInfo(APP_ERR_COMM_INVALID_POINTER);
        return APP_ERR_COMM_INVALID_POINTER;
    }
    APP_ERROR ret = aclrtLaunchCallback(fn, userData, ACL_CALLBACK_BLOCK, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Execute aclrtLaunchCallback failed for user callback" << GetErrorInfo(ret, "aclrtLaunchCallback");
        return APP_ERR_ACL_FAILURE;
    }

    MxBaseUserData *mxBaseData = new MxBaseUserData();
    mxBaseData->stream = stream;
    ret = StreamTensorManager::GetInstance()->GetStreamTensorListLastTensor(stream, mxBaseData->lastTensor);
    if (ret != APP_ERR_OK) {
        LogError << "Failed to GetStreamTensorListLastTensor." << GetErrorInfo(ret);
        delete mxBaseData;
        return APP_ERR_COMM_FAILURE;
    }
    ret = aclrtLaunchCallback(RecycleTensorCallback, mxBaseData, ACL_CALLBACK_BLOCK, stream);
    if (ret != APP_ERR_OK) {
        LogError << "Execute aclrtLaunchCallback failed for recycle callback."
                 << GetErrorInfo(ret, "aclrtLaunchCallback");
        delete mxBaseData;
        return APP_ERR_ACL_FAILURE;
    }
    return ret;
}

APP_ERROR AscendStream::AddTensorRefPtr(const Tensor& inputTensor)
{
    APP_ERROR ret = StreamTensorManager::GetInstance()->AddTensor(stream, inputTensor);
    if (ret != APP_ERR_OK) {
        LogWarn << "Fail to add tensor in StreamTensorManager." << GetErrorInfo(ret);
    }
    return APP_ERR_OK;
}
}

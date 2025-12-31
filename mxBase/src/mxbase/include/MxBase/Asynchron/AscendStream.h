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
#ifndef MXBASE_ASCENDSTREAM_H
#define MXBASE_ASCENDSTREAM_H

#include <thread>
#include <mutex>
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxBase {
class Tensor;
class AscendStream {
    typedef void (*aclrtCallback)(void *userData);
public:
    enum FlagType {
        DEFAULT = 0,
        FAST_LAUNCH = 1,
        FAST_SYNC = 2,
        LAUNCH_SYNC = 3,
    };

public:
    explicit AscendStream(int32_t deviceId = 0);

    explicit AscendStream(int32_t deviceId, FlagType flag);

    ~AscendStream();

    APP_ERROR DestroyAscendStream();

    APP_ERROR Synchronize() const;

    APP_ERROR CreateAscendStream();

    static AscendStream &DefaultStream();

    APP_ERROR GetChannel(int* channelId);

    APP_ERROR CreateChannel();

    int32_t GetDeviceId() const;

    void SetErrorCode(APP_ERROR errCode);

    std::pair<APP_ERROR, APP_ERROR> GetErrorCode();

    void* stream = nullptr;

    bool isDefault_ = false;

    APP_ERROR LaunchCallBack(aclrtCallback fn, void* userData);

    APP_ERROR AddTensorRefPtr(const Tensor& inputTensor);

private:
    struct CallbackParam {
        bool isExit;
        bool isSuccessSetContext;
        int32_t deviceId;
    };

    static void ProcessCallback(void *arg);

    APP_ERROR DoProcessCallback();

    int chnId_ = -1;
    uint64_t tid;
    int32_t deviceId_;
    std::shared_ptr<CallbackParam> param_;
    std::shared_ptr<std::thread> td_;
    std::pair<APP_ERROR, APP_ERROR> errCodeLogger = std::make_pair(APP_ERR_OK, APP_ERR_OK);
};
}

#endif

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
 * Description: Manage the tensor of AscendStream.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef MXBASE_STREAMTENSORMANAGER_H
#define MXBASE_STREAMTENSORMANAGER_H
#include <list>
#include <mutex>
#include "acl/acl.h"
#include "MxBase/E2eInfer/Tensor/Tensor.h"
#include "MxBase/Log/Log.h"
#include "MxBase/ErrorCode/ErrorCode.h"

namespace MxBase {
    class StreamTensorManager {

    public:
        /**
         * @description: Default Destructor function.
         */
        ~StreamTensorManager();

        /**
         * @description: Add stream to StreamTensorManager object for management.
         * @param stream: stream.
         */
        APP_ERROR AddStream(aclrtStream stream);

        /**
         * @description: Get stream mutex according to stream.
         * @param stream: stream.
         */
        std::shared_ptr<std::mutex> GetStreamMutex(aclrtStream stream);

        /**
         * @description: Delete specific stream resource in StreamTensorManager.
         * stream: stream.
         */
        APP_ERROR DeleteStream(aclrtStream stream);

        /**
         * @description: Clear the tensors relative to the given stream.
         * @param stream: stream.
         */
        APP_ERROR ClearTensorsByStream(aclrtStream stream);

        /**
         * @description: Add the tensor in the given stream to garantte the life span of the tensor in callback func.
         * @param stream: stream.
         * @param tensor: tensor.
         */
        APP_ERROR AddTensor(aclrtStream stream, const MxBase::Tensor& tensor);

        /**
         * @description: Delete the tensor in the give stream to make sure the deconstruction after the callback func.
         * @param stream: stream.
         * @param tensor: tensor.
         */
        APP_ERROR DeleteTensor(aclrtStream stream, MxBase::Tensor& tensor);

        /**
        * @description: Destroy the resource of StreamTensorManager.
        */
        APP_ERROR DeInit();

        static StreamTensorManager* GetInstance()
        {
            return &instance_;
        }

        std::shared_ptr<std::mutex> unusedMtx_ = std::make_shared<std::mutex>();

        /**
        * @description: Get the last tensor of stream tensor list.
        */
        APP_ERROR GetStreamTensorListLastTensor(aclrtStream stream, MxBase::Tensor& tensor);

    private:
        /**
        * @description: Construction function.
        */
        StreamTensorManager() noexcept;

        /**
        * @description: Check whether the stream resource is ok.
        */
        bool CheckStreamResource(aclrtStream stream);

        StreamTensorManager(const StreamTensorManager&) = delete;

        StreamTensorManager& operator=(const StreamTensorManager&) = delete;

    private:
        static StreamTensorManager instance_;
        std::mutex streamMtx_;
        std::map<aclrtStream, std::shared_ptr<std::list<Tensor>>> streamTensorMap_;
        std::map<aclrtStream, std::shared_ptr<std::mutex>> streamMutexMap_;
    };
}
#endif // MXBASE_STREAMTENSORMANAGER_H

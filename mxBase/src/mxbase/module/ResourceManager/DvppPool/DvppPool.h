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
 * Description: DVPP channel pool.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#ifndef MXBASE_DVPPPOOL_H
#define MXBASE_DVPPPOOL_H

#include <cstdint>
#include <sys/types.h>
#include "acl/dvpp/hi_dvpp.h"
#include "MxBase/Log/Log.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "MxBase/BlockingQueue/BlockingQueue.h"
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"
#include "MxBase/DvppWrapper/DvppWrapperDataType.h"

namespace MxBase {
    enum class DvppChnType {
        VPC = 0,
        JPEGD,
        PNGD,
        JPEGE
    };
    class DvppPool {
    public:
        static DvppPool &GetInstance()
        {
            static DvppPool dvppPool;
            return dvppPool;
        }

        /**
        * @description: Init for dvpp channel pool.
        * @param deviceId: device Id.
        * @param chnType: the channel type of the dvpp pool.
        */
        APP_ERROR Init(int32_t deviceId, DvppChnType chnType = DvppChnType::VPC);

        /**
         * @description: Channel pool on device has init success or not.
         * @param deviceId: device id.
         * @param chnType: the channel type of the dvpp pool.
         */
        static bool IsInited(int32_t deviceId, DvppChnType chnType = DvppChnType::VPC);

        /**
         * @description: Set dvpp pool size.
         * @param globalCfg: input AppGlobalCfg struct.
         */
        void SetChnNum(const AppGlobalCfgExtra &globalCfgExtra);

        /**
         * @description: Destroy channel pool, free the resources.
         */
        APP_ERROR DeInit();

        /**
        * @description: Get channel from pool.
        * @param deviceId: device id.
        * @param chnId: channel id.
        * @param chnType: the channel type of the dvpp pool.
        */
        APP_ERROR GetChn(int32_t deviceId, hi_s32 &chnId, DvppChnType chnType = DvppChnType::VPC);

        /**
         * @description: Put back channel to pool.
         * @param deviceId: device id.
         * @param chnId: channel id.
         * @param chnType: the channel type of the dvpp pool.
         */
        APP_ERROR PutChn(int32_t deviceId, hi_s32 &chnId, DvppChnType chnType = DvppChnType::VPC);

    private:
        DvppPool() = default;

        ~DvppPool() = default;

        DvppPool(const DvppPool&) = delete;

        DvppPool &operator=(const DvppPool&) = delete;

        static APP_ERROR InitChnPoolOnDevice(int32_t deviceId, DvppChnType chnType = DvppChnType::VPC);

        static APP_ERROR InitVpcPool(int32_t deviceId, uint32_t size);

        static APP_ERROR DeInitVpcPool();

        static APP_ERROR InitJpegdPool(int32_t deviceId, uint32_t size);

        static APP_ERROR SetJpegdChnParam(hi_vdec_chn channelId);

        static APP_ERROR DeInitJpegdPool();

        static APP_ERROR InitJpegePool(int32_t deviceId, uint32_t size);

        static APP_ERROR DeInitJpegePool();

        static APP_ERROR InitPngdPool(int32_t deviceId, uint32_t size);

        static APP_ERROR DeInitPngdPool();

        static APP_ERROR HimpiSysExit();

    private:
        static AppGlobalCfgExtra dvppPoolCfg_;
        static std::map <int32_t, std::shared_ptr<MxBase::BlockingQueue<hi_vpc_chn>>> vpcChnQueueMap_;
        static std::map <int32_t, std::shared_ptr<MxBase::BlockingQueue<hi_vdec_chn>>> jpegdChnQueueMap_;
        static std::map <int32_t, std::shared_ptr<MxBase::BlockingQueue<hi_pngd_chn>>> pngdChnQueueMap_;
        static std::map <int32_t, std::shared_ptr<MxBase::BlockingQueue<hi_venc_chn>>> jpegeChnQueueMap_;
    };
}

#endif

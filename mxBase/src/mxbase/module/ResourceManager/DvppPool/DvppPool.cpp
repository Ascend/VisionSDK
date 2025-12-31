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

#include "DvppPool.h"
#include <set>

namespace MxBase {
    static bool g_chnInitStatus = false;
    static std::mutex g_chnMtx;
    static std::set<int32_t> g_himpiIsInitedDevices;

    AppGlobalCfgExtra DvppPool::dvppPoolCfg_;
    std::map<int32_t, std::shared_ptr<MxBase::BlockingQueue<hi_vpc_chn>>> DvppPool::vpcChnQueueMap_;
    std::map<int32_t, std::shared_ptr<MxBase::BlockingQueue<hi_vdec_chn>>> DvppPool::jpegdChnQueueMap_;
    std::map<int32_t, std::shared_ptr<MxBase::BlockingQueue<hi_pngd_chn>>> DvppPool::pngdChnQueueMap_;
    std::map<int32_t, std::shared_ptr<MxBase::BlockingQueue<hi_venc_chn>>> DvppPool::jpegeChnQueueMap_;

    void DvppPool::SetChnNum(const AppGlobalCfgExtra &globalCfgExtra)
    {
        std::lock_guard<std::mutex> lck(g_chnMtx);
        LogDebug << "Begin to set channel num.";
        if (g_chnInitStatus) {
            return;
        }
        dvppPoolCfg_.vpcChnNum = globalCfgExtra.vpcChnNum;
        dvppPoolCfg_.jpegdChnNum = globalCfgExtra.jpegdChnNum;
        dvppPoolCfg_.jpegeChnNum = globalCfgExtra.jpegeChnNum;
        dvppPoolCfg_.pngdChnNum = globalCfgExtra.pngdChnNum;
        LogDebug << "vpcChnNum is " << dvppPoolCfg_.vpcChnNum << ", jpegdChnNum is " << dvppPoolCfg_.jpegdChnNum
            << ", jpegeChnNum is " << dvppPoolCfg_.jpegeChnNum << ", pngdChnNum is " << dvppPoolCfg_.pngdChnNum << ".";
        g_chnInitStatus = true;
    }

    APP_ERROR DvppPool::InitChnPoolOnDevice(int32_t deviceId, DvppChnType chnType)
    {
        LogInfo << "Begin to initialize dvpp channel pool on device(" << deviceId << ").";
        APP_ERROR ret = APP_ERR_OK;
        if (g_himpiIsInitedDevices.find(deviceId) == g_himpiIsInitedDevices.end()) {
            ret = hi_mpi_sys_init();
            if (ret != APP_ERR_OK) {
                LogError << "Failed to initialize dvpp system." << GetErrorInfo(ret, "hi_mpi_sys_init");
                return APP_ERR_COMM_INIT_FAIL;
            }
            g_himpiIsInitedDevices.insert(deviceId);
        }
        switch (chnType) {
            case DvppChnType::VPC: {
                ret = InitVpcPool(deviceId, dvppPoolCfg_.vpcChnNum);
                if (ret != APP_ERR_OK) {
                    LogError << "Failed to InitVpcPool on device[" << deviceId << "]." << GetErrorInfo(ret);
                }
                break;
            }
            case DvppChnType::JPEGD: {
                ret = InitJpegdPool(deviceId, dvppPoolCfg_.jpegdChnNum);
                if (ret != APP_ERR_OK) {
                    LogError << "Failed to InitJpegdPool on device[" << deviceId << "]." << GetErrorInfo(ret);
                }
                break;
            }
            case DvppChnType::PNGD: {
                ret = InitPngdPool(deviceId, dvppPoolCfg_.pngdChnNum);
                if (ret != APP_ERR_OK) {
                    LogError << "Failed to InitPngdPool on device[" << deviceId << "]." << GetErrorInfo(ret);
                }
                break;
            }
            case DvppChnType::JPEGE: {
                ret = InitJpegePool(deviceId, dvppPoolCfg_.jpegeChnNum);
                if (ret != APP_ERR_OK) {
                    LogError << "Failed to InitJpegePool on device[" << deviceId << "]." << GetErrorInfo(ret);
                }
                break;
            }
            default:
                LogError << "InitChnPoolOnDevice: not supported chnType( " << static_cast<int>(chnType) << ")."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
        }
        return ret;
    }

    APP_ERROR DvppPool::InitVpcPool(int32_t deviceId, uint32_t size)
    {
        LogInfo << "Begin to initialize vpc pool.";
        auto iter = vpcChnQueueMap_.find(deviceId);
        if (iter == vpcChnQueueMap_.end()) {
            LogError << "Failed to find ChannelQueue from device(" << deviceId << ")."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        for (uint32_t i = 0; i < size; i++) {
            hi_vpc_chn_attr vpcChnAttr;
            hi_vpc_chn channelId;
            vpcChnAttr.attr = 0;
            APP_ERROR ret = hi_mpi_vpc_sys_create_chn(&channelId, &vpcChnAttr);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to create vpc hi_mpi_vpc_sys_chn."
                    << GetErrorInfo(ret, "hi_mpi_vpc_sys_create_chn");
                return APP_ERR_COMM_INIT_FAIL;
            }
            ret = iter->second->Push(channelId);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to push channel to pool." << GetErrorInfo(ret);
                return APP_ERR_COMM_INIT_FAIL;
            }
        }
        LogInfo << "Initialized vpc channel pool successful on device(" << deviceId << "), pool size: "
                << iter->second->GetSize() << ".";
        return APP_ERR_OK;
    }

    APP_ERROR DvppPool::SetJpegdChnParam(hi_vdec_chn channelId)
    {
        hi_vdec_chn_param chnParam;
        APP_ERROR ret = hi_mpi_vdec_get_chn_param(channelId, &chnParam);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to get jpeg decode channel parameter."
                     << GetErrorInfo(ret, "hi_mpi_vdec_get_chn_param");
            hi_mpi_vdec_destroy_chn(channelId);
            return APP_ERR_ACL_FAILURE;
        }
        chnParam.pic_param.pixel_format = HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
        chnParam.pic_param.alpha = HI_MPI_PIC_PARAM_ALPHA;
        chnParam.display_frame_num = 0;
        ret = hi_mpi_vdec_set_chn_param(channelId, &chnParam);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to set jpeg decode channel parameter."
                     << GetErrorInfo(ret, "hi_mpi_vdec_set_chn_param");
            hi_mpi_vdec_destroy_chn(channelId);
            return APP_ERR_ACL_FAILURE;
        }
        return APP_ERR_OK;
    }

    APP_ERROR DvppPool::InitJpegdPool(int32_t deviceId, uint32_t size)
    {
        LogInfo << "Begin to initialize jpegd pool.";
        auto iter = jpegdChnQueueMap_.find(deviceId);
        if (iter == jpegdChnQueueMap_.end()) {
            LogError << "Failed to find ChannelQueue from device(" << deviceId << ")."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        hi_vdec_chn channelId = 0;
        for (uint32_t i = 0; i < size; i++) {
            hi_vdec_chn_attr attr;
            attr.type = HI_PT_JPEG;
            attr.mode = HI_VDEC_SEND_MODE_FRAME;
            attr.pic_width = MAX_JPEGD_WIDTH;
            attr.pic_height = MAX_JPEGD_HEIGHT;
            attr.stream_buf_size = MAX_JPEGD_WIDTH * MAX_JPEGD_HEIGHT;
            attr.frame_buf_cnt = 0;
            attr.frame_buf_size = 0;
            APP_ERROR ret = APP_ERR_OK;
            while (channelId <= MAX_HIMPI_CHN_NUM) {
                ret = hi_mpi_vdec_create_chn(channelId, &attr);
                if (ret == APP_ERR_OK) {
                    LogDebug << "Create jpeg decode channel successfully. channel id is " << channelId << ".";
                    break;
                } else if (channelId == MAX_HIMPI_CHN_NUM) {
                    LogError << "Failed to create jpeg decode channel, All channels are occupied."
                             << GetErrorInfo(ret, "hi_mpi_vdec_create_chn");
                    return APP_ERR_ACL_FAILURE;
                } else {
                    channelId++;
                }
            }
            if (DvppPool::SetJpegdChnParam(channelId) != APP_ERR_OK) {
                return APP_ERR_COMM_INIT_FAIL;
            }
            ret = iter->second->Push(channelId);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to push vdec channel to pool." << GetErrorInfo(ret);
                return APP_ERR_COMM_INIT_FAIL;
            }
            channelId++;
        }
        LogInfo << "Initialized vdec channel pool successful on device(" << deviceId << "), pool size: "
                << iter->second->GetSize() << ".";
        return APP_ERR_OK;
    }

    APP_ERROR DvppPool::InitPngdPool(int32_t deviceId, uint32_t size)
    {
        auto iter = pngdChnQueueMap_.find(deviceId);
        if (iter == pngdChnQueueMap_.end()) {
            LogError << "Failed to find ChannelQueue from device(" << deviceId << ")."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        hi_pngd_chn channelId = 0;
        APP_ERROR ret = APP_ERR_OK;
        for (uint32_t i = 0; i < size; i++) {
            hi_pngd_chn_attr attr;
            while (channelId <= MAX_HIMPI_PNGD_CHN_NUM) {
                ret = hi_mpi_pngd_create_chn(channelId, &attr);
                if (ret == APP_ERR_OK) {
                    LogDebug << "Create pngd channel successfully. channel id is " << channelId << ".";
                    break;
                } else if (channelId == MAX_HIMPI_PNGD_CHN_NUM) {
                    LogError << "Failed to create video decode channel, All channels are occupied."
                             << GetErrorInfo(ret, "hi_mpi_pngd_create_chn");
                    return APP_ERR_ACL_FAILURE;
                } else {
                    channelId++;
                }
            }
            ret = iter->second->Push(channelId);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to push pngd channel to pool." << GetErrorInfo(ret);
                return APP_ERR_COMM_INIT_FAIL;
            }
            channelId++;
        }
        LogInfo << "Initialized pngd channel pool successful on device(" << deviceId << "), pool size: "
                << iter->second->GetSize() << ".";
        return ret;
    }

    APP_ERROR DvppPool::InitJpegePool(int32_t deviceId, uint32_t size)
    {
        auto iter = jpegeChnQueueMap_.find(deviceId);
        if (iter == jpegeChnQueueMap_.end()) {
            LogError << "Failed to find ChannelQueue from device(" << deviceId << ")."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return APP_ERR_COMM_INVALID_PARAM;
        }
        JpegEncodeChnConfig config;
        hi_venc_chn channelId = 0;
        APP_ERROR ret = APP_ERR_OK;
        for (uint32_t i = 0; i < size; i++) {
            uint32_t channelWidth = config.maxPicWidth;
            uint32_t channelHeight = config.maxPicHeight;
            channelWidth = (channelWidth + VENC_STRIDE_WIDTH - 1) / VENC_STRIDE_WIDTH * VENC_STRIDE_WIDTH;
            channelHeight = (channelHeight + VENC_STRIDE_HEIGHT - 1) / VENC_STRIDE_HEIGHT * VENC_STRIDE_HEIGHT;
            // channel height should be no less than width or the acl will return error code.
            channelHeight = channelHeight < channelWidth ? channelWidth : channelHeight;
            hi_venc_chn_attr attr;
            attr.venc_attr.type = HI_PT_JPEG;
            attr.venc_attr.profile = 0;
            attr.venc_attr.max_pic_width = channelWidth;
            attr.venc_attr.max_pic_height = channelHeight;
            attr.venc_attr.pic_width = channelWidth;
            attr.venc_attr.pic_height = channelHeight;
            attr.venc_attr.buf_size =  channelWidth * channelHeight * HI_ODD_NUM_3 / HI_ODD_NUM_2;
            attr.venc_attr.is_by_frame = HI_TRUE;
            attr.venc_attr.jpeg_attr.dcf_en = HI_FALSE;
            attr.venc_attr.jpeg_attr.recv_mode = HI_VENC_PIC_RECV_SINGLE;
            attr.venc_attr.jpeg_attr.mpf_cfg.large_thumbnail_num = 0;
            while (channelId <= MAX_HIMPI_CHN_NUM) {
                ret = hi_mpi_venc_create_chn(channelId, &attr);
                if (ret == APP_ERR_OK) {
                    LogDebug << "Create jpeg encode channels successfully. channel id is " << channelId << ".";
                    break;
                } else if (channelId == MAX_HIMPI_CHN_NUM) {
                    LogError << "Failed to create jpeg encode channel, All channels are occupied."
                             << GetErrorInfo(ret, "hi_mpi_venc_create_chn");
                    return APP_ERR_ACL_FAILURE;
                } else {
                    channelId++;
                }
            }
            ret = iter->second->Push(channelId);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to push venc channel to pool." << GetErrorInfo(ret);
                return APP_ERR_COMM_INIT_FAIL;
            }
            channelId++;
        }
        LogInfo << "Initialized jpege channel pool successful on device(" << deviceId << "), pool size: "
                << iter->second->GetSize() << ".";
        return ret;
    }

    APP_ERROR DvppPool::Init(int32_t deviceId, DvppChnType chnType)
    {
        static std::mutex initMtx;
        std::lock_guard<std::mutex> lck(initMtx);
        APP_ERROR ret = APP_ERR_OK;
        if (!(DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2())) {
            LogError << "Vpc Channel Pool only supported on device 310P or Atlas800IA2 now, current is "
                     << DeviceManager::GetSocName() << "." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return APP_ERR_COMM_FAILURE;
        }

        if (DvppPool::IsInited(deviceId, chnType)) {
            return APP_ERR_OK;
        }

        DeviceContext device = {};
        device.devId = deviceId;
        ret = MxBase::DeviceManager::GetInstance()->SetDevice(device);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to set device." << GetErrorInfo(ret);
            return ret;
        }

        switch (chnType) {
            case DvppChnType::VPC: {
                vpcChnQueueMap_[deviceId] = std::make_shared<MxBase::BlockingQueue<hi_vpc_chn>>();
                break;
            }
            case DvppChnType::JPEGD: {
                jpegdChnQueueMap_[deviceId] = std::make_shared<MxBase::BlockingQueue<hi_vdec_chn>>();
                break;
            }
            case DvppChnType::PNGD: {
                pngdChnQueueMap_[deviceId] = std::make_shared<MxBase::BlockingQueue<hi_pngd_chn>>();
                break;
            }
            case DvppChnType::JPEGE: {
                jpegeChnQueueMap_[deviceId] = std::make_shared<MxBase::BlockingQueue<hi_venc_chn>>();
                break;
            }
            default:
                LogError << "DvppPool Inited: not supported chnType( " << static_cast<int>(chnType) << ")."
                         << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                return APP_ERR_COMM_INVALID_PARAM;
        }
        ret = InitChnPoolOnDevice(deviceId, chnType);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to Init Channel Pool on device(" << deviceId << ")." << GetErrorInfo(ret);
        }
        return ret;
    }

    bool DvppPool::IsInited(int32_t deviceId, DvppChnType chnType)
    {
        bool isInited = true;
        switch (chnType) {
            case DvppChnType::VPC: {
                if (vpcChnQueueMap_.find(deviceId) == vpcChnQueueMap_.end()) {
                    isInited = false;
                }
                break;
            }
            case DvppChnType::JPEGD: {
                if (jpegdChnQueueMap_.find(deviceId) == jpegdChnQueueMap_.end()) {
                    isInited = false;
                }
                break;
            }
            case DvppChnType::PNGD: {
                if (pngdChnQueueMap_.find(deviceId) == pngdChnQueueMap_.end()) {
                    isInited = false;
                }
                break;
            }
            case DvppChnType::JPEGE: {
                if (jpegeChnQueueMap_.find(deviceId) == jpegeChnQueueMap_.end()) {
                    isInited = false;
                }
                break;
            }
            default:
                LogError << "IsInited: not supported chnType( " << static_cast<int>(chnType) << ").";
                isInited = false;
                break;
        }
        return isInited;
    }

    APP_ERROR DvppPool::DeInitVpcPool()
    {
        LogInfo << "Start to DeInit Vpc Chn Pool.";
        APP_ERROR ret = APP_ERR_OK;
        APP_ERROR deInitVpcRet = APP_ERR_OK;
        for (auto iter : vpcChnQueueMap_) {
            DeviceContext device = {};
            device.devId = iter.first;
            ret = MxBase::DeviceManager::GetInstance()->SetDevice(device);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to SetDevice in DvppPool DeInitVpcPool Operation." << GetErrorInfo(ret);
                return ret;
            }
            while (!iter.second->IsEmpty()) {
                hi_vpc_chn chnId = 0;
                iter.second->Pop(chnId);
                ret = hi_mpi_vpc_destroy_chn(chnId);
                if (ret != APP_ERR_OK) {
                    LogError << "Failed to destroy vpc channel on device(" << iter.first << ")."
                             << GetErrorInfo(ret, "hi_mpi_vpc_destroy_chn");
                    deInitVpcRet = APP_ERR_COMM_FAILURE;
                }
            }
        }
        vpcChnQueueMap_.clear();
        LogInfo << "DeInit Vpc Chn Pool Finished.";
        return deInitVpcRet;
    }

    APP_ERROR DvppPool::DeInitJpegdPool()
    {
        LogInfo << "Start to DeInit Jpegd Chn Pool.";
        APP_ERROR ret = APP_ERR_OK;
        APP_ERROR deInitJpegdRet = APP_ERR_OK;
        for (auto iter : jpegdChnQueueMap_) {
            DeviceContext device = {};
            device.devId = iter.first;
            ret = MxBase::DeviceManager::GetInstance()->SetDevice(device);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to SetDevice in DvppPool DeInitJpegdPool Operation." << GetErrorInfo(ret);
                return ret;
            }
            while (!iter.second->IsEmpty()) {
                hi_vdec_chn chnId = 0;
                iter.second->Pop(chnId);
                ret = hi_mpi_vdec_destroy_chn(chnId);
                if (ret != APP_ERR_OK) {
                    LogError << "Failed to destroy jpegd channel on device(" << iter.first << ")."
                             << GetErrorInfo(ret, "hi_mpi_vdec_destroy_chn");
                    deInitJpegdRet = APP_ERR_COMM_FAILURE;
                }
            }
        }
        jpegdChnQueueMap_.clear();
        LogInfo << "DeInit Jpegd Chn Pool Finished.";
        return deInitJpegdRet;
    }

    APP_ERROR DvppPool::DeInitPngdPool()
    {
        LogInfo << "Start to DeInit Pngd Chn Pool.";
        APP_ERROR ret = APP_ERR_OK;
        APP_ERROR deInitPngdRet = APP_ERR_OK;
        for (auto iter : pngdChnQueueMap_) {
            DeviceContext device = {};
            device.devId = iter.first;
            ret = MxBase::DeviceManager::GetInstance()->SetDevice(device);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to SetDevice in DvppPool DeInitPngdPool Operation." << GetErrorInfo(ret);
                return ret;
            }
            while (!iter.second->IsEmpty()) {
                hi_pngd_chn chnId = 0;
                iter.second->Pop(chnId);
                ret = hi_mpi_pngd_destroy_chn(chnId);
                if (ret != APP_ERR_OK) {
                    LogError << "Failed to destroy pngd channel on device(" << iter.first << ")."
                             << GetErrorInfo(ret, "hi_mpi_pngd_destroy_chn");
                    deInitPngdRet = APP_ERR_COMM_FAILURE;
                }
            }
        }
        pngdChnQueueMap_.clear();
        LogInfo << "DeInit Pngd Chn Pool Finished.";
        return deInitPngdRet;
    }

    APP_ERROR DvppPool::DeInitJpegePool()
    {
        LogInfo << "Start to DeInit Jpege Chn Pool.";
        APP_ERROR ret = APP_ERR_OK;
        APP_ERROR deInitJpegeRet = APP_ERR_OK;
        for (auto iter : jpegeChnQueueMap_) {
            DeviceContext device = {};
            device.devId = iter.first;
            ret = MxBase::DeviceManager::GetInstance()->SetDevice(device);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to SetDevice in DvppPool DeInitJpegePool Operation." << GetErrorInfo(ret);
                return ret;
            }
            while (!iter.second->IsEmpty()) {
                hi_venc_chn chnId = 0;
                iter.second->Pop(chnId);
                ret = hi_mpi_venc_stop_chn(chnId);
                if (ret != APP_ERR_OK) {
                    LogError << "Failed to stop channel on device(" << iter.first << ")."
                             << GetErrorInfo(ret, "hi_mpi_venc_stop_chn");
                    deInitJpegeRet = APP_ERR_COMM_FAILURE;
                }
                ret = hi_mpi_venc_destroy_chn(chnId);
                if (ret != APP_ERR_OK) {
                    LogError << "Failed to destroy venc channel on device(" << iter.first << ")."
                             << GetErrorInfo(ret, "hi_mpi_venc_destroy_chn");
                    deInitJpegeRet = APP_ERR_COMM_FAILURE;
                }
            }
        }
        jpegeChnQueueMap_.clear();
        LogInfo << "DeInit Jpege Chn Pool Finished.";
        return deInitJpegeRet;
    }

    APP_ERROR DvppPool::HimpiSysExit()
    {
        APP_ERROR himpiSysExitRet = APP_ERR_OK;
        for (const auto& deviceId : g_himpiIsInitedDevices) {
            DeviceContext device = {};
            device.devId = deviceId;
            APP_ERROR ret = MxBase::DeviceManager::GetInstance()->SetDevice(device);
            if (ret != APP_ERR_OK) {
                LogError << "Failed to set device, the devideId is: " << deviceId << "." << GetErrorInfo(ret);
                himpiSysExitRet = APP_ERR_COMM_FAILURE;
                continue;
            }
            ret = hi_mpi_sys_exit();
            if (ret != APP_ERR_OK) {
                LogError << "DeviceId " << deviceId << ": failed to exit dvpp system."
                    << GetErrorInfo(ret, "hi_mpi_sys_exit");
                himpiSysExitRet = APP_ERR_COMM_FAILURE;
                continue;
            }
        }
        return himpiSysExitRet;
    }

    APP_ERROR DvppPool::DeInit()
    {
        std::lock_guard<std::mutex> lck(g_chnMtx);
        APP_ERROR deInitRet = APP_ERR_OK;
        APP_ERROR ret = DeInitVpcPool();
        if (ret != APP_ERR_OK) {
            LogError << "Failed to DeInit VpcPool." << GetErrorInfo(ret);
            deInitRet = APP_ERR_COMM_FAILURE;
        }
        ret = DeInitJpegdPool();
        if (ret != APP_ERR_OK) {
            LogError << "Failed to DeInit JpegdPool." << GetErrorInfo(ret);
            deInitRet = APP_ERR_COMM_FAILURE;
        }
        ret = DeInitPngdPool();
        if (ret != APP_ERR_OK) {
            LogError << "Failed to DeInit PngdPool." << GetErrorInfo(ret);
            deInitRet = APP_ERR_COMM_FAILURE;
        }
        ret = DeInitJpegePool();
        if (ret != APP_ERR_OK) {
            LogError << "Failed to DeInit JpegePool." << GetErrorInfo(ret);
            deInitRet = APP_ERR_COMM_FAILURE;
        }
        ret = HimpiSysExit();
        if (ret != APP_ERR_OK) {
            LogError << "Failed to HimpiSysExit." << GetErrorInfo(ret);
            deInitRet = APP_ERR_COMM_FAILURE;
        }
        g_himpiIsInitedDevices.clear();
        LogInfo << "Success to destroy chn in dvpp channel pool on all devices.";
        return deInitRet;
    }
    APP_ERROR DvppPool::GetChn(int32_t deviceId, hi_s32& chnId, DvppChnType chnType)
    {
        // init channel pool, if pool has inited, return OK.
        APP_ERROR ret = Init(deviceId, chnType);
        if (ret != APP_ERR_OK) {
            LogError << "Failed to init DvppPool." << GetErrorInfo(ret);
            return ret;
        }
        switch (chnType) {
            case DvppChnType::VPC: {
                ret = vpcChnQueueMap_[deviceId]->Pop(chnId);
                if (ret == APP_ERR_OK) {
                    LogDebug << "Get vpc chnId: " << chnId << " from pool on device(" << deviceId << ") successful.";
                } else {
                    LogError<< "Get vpc chnId: " << chnId << " from pool on device(" << deviceId << ") failed.";
                }
                break;
            }
            case DvppChnType::JPEGD: {
                ret = jpegdChnQueueMap_[deviceId]->Pop(chnId);
                if (ret == APP_ERR_OK) {
                    LogDebug << "Get jpegd chnId: " << chnId << " from pool on device(" << deviceId << ") successful.";
                } else {
                    LogError<< "Get jpegd chnId: " << chnId << " from pool on device(" << deviceId << ") failed.";
                }
                break;
            }
            case DvppChnType::PNGD: {
                ret = pngdChnQueueMap_[deviceId]->Pop(chnId);
                if (ret == APP_ERR_OK) {
                    LogDebug << "Get pngd chnId: " << chnId << " from pool on device(" << deviceId << ") successful.";
                } else {
                    LogError<< "Get pngd chnId: " << chnId << " from pool on device(" << deviceId << ") failed.";
                }
                break;
            }
            case DvppChnType::JPEGE: {
                ret = jpegeChnQueueMap_[deviceId]->Pop(chnId);
                if (ret == APP_ERR_OK) {
                    LogDebug << "Get jpege chnId: " << chnId << " from pool on device(" << deviceId << ") successful.";
                } else {
                    LogError<< "Get jpege chnId: " << chnId << " from pool on device(" << deviceId << ") failed.";
                }
                break;
            }
            default:
                LogError << "GetChn: not supported chnType( " << static_cast<int>(chnType) << ").";
                return APP_ERR_COMM_INVALID_PARAM;
        }
        return ret;
    }

    APP_ERROR DvppPool::PutChn(int32_t deviceId, hi_s32& chnId, DvppChnType chnType)
    {
        APP_ERROR ret = APP_ERR_OK;
        switch (chnType) {
            case DvppChnType::VPC: {
                auto iterVpc = vpcChnQueueMap_.find(deviceId);
                if (iterVpc == vpcChnQueueMap_.end()) {
                    LogError << "Failed to find vpc chn pool on deviceId: " << deviceId << "."
                        << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                ret = iterVpc->second->Push(chnId);
                if (ret != APP_ERR_OK) {
                    LogError << "Failed to push vpc channel to pool, channelId is: " << chnId << "."
                        << GetErrorInfo(ret);
                    return APP_ERR_COMM_FAILURE;
                }
                break;
            }
            case DvppChnType::JPEGD: {
                auto iterVdec = jpegdChnQueueMap_.find(deviceId);
                if (iterVdec == jpegdChnQueueMap_.end()) {
                    LogError << "Failed to find jpegd chn pool on deviceId: " << deviceId << "."
                        << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                ret = iterVdec->second->Push(chnId);
                if (ret != APP_ERR_OK) {
                    LogError << "Failed to push jpegd channel to pool, channelId is: " << chnId << "."
                        << GetErrorInfo(ret);
                    return APP_ERR_COMM_FAILURE;
                }
                break;
            }
            case DvppChnType::PNGD: {
                auto iterPngd = pngdChnQueueMap_.find(deviceId);
                if (iterPngd == pngdChnQueueMap_.end()) {
                    LogError << "Failed to find pngd chn pool on deviceId: " << deviceId << "."
                        << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                ret = iterPngd->second->Push(chnId);
                if (ret != APP_ERR_OK) {
                    LogError << "Failed to push pngd channel to pool, channelId is: " << chnId << "."
                        << GetErrorInfo(ret);
                    return APP_ERR_COMM_FAILURE;
                }
                break;
            }
            case DvppChnType::JPEGE: {
                auto iterJpege = jpegeChnQueueMap_.find(deviceId);
                if (iterJpege == jpegeChnQueueMap_.end()) {
                    LogError << "Failed to find jpege chn pool on deviceId: " << deviceId << "."
                        << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
                    return APP_ERR_COMM_INVALID_PARAM;
                }
                ret = iterJpege->second->Push(chnId);
                if (ret != APP_ERR_OK) {
                    LogError << "Failed to push jpege channel to pool, channelId is: " << chnId << "."
                        << GetErrorInfo(ret);
                    return APP_ERR_COMM_FAILURE;
                }
                break;
            }
            default:
                LogError << "DvppPool PutChn: not supported chnType.";
                return APP_ERR_COMM_INVALID_PARAM;
        }
        return ret;
    }
}
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
 * Description: Global Init of MxVision Application.
 * Author: MindX SDK
 * Create: 2022
 * History: NA
 */

#include "acl/acl.h"
#include "OperationPreload/GlobalOpMap.h"
#include "MxBase/DeviceManager/DeviceManager.h"
#include "LogManager.hpp"
#include "ResourceManager/DvppPool/DvppPool.h"
#include "MxBase/Asynchron/AscendStream.h"
#include "ResourceManager/StreamTensorManager/StreamTensorManager.h"
#include "MxBase/E2eInfer/GlobalInit/GlobalInit.h"

namespace MxBase {

namespace {
    constexpr uint32_t MIN_CHN_POOL_SIZE = 1;
    constexpr uint32_t MAX_VPC_CHN_POOL_SIZE = 128;
    constexpr uint32_t MAX_JPEGD_CHN_POOL_SIZE = 64;
    constexpr uint32_t MAX_JPEGE_CHN_POOL_SIZE = 48;
    constexpr uint32_t MAX_PNGD_CHN_POOL_SIZE = 64;
}

APP_ERROR MxDeInit()
{
    LogInfo << "Enter MxDeInit.";
    AscendStream& stream = AscendStream::DefaultStream();
    APP_ERROR deinitRet = APP_ERR_OK;
    APP_ERROR ret = stream.DestroyAscendStream();
    if (ret != APP_ERR_OK) {
        LogError << "Failed to DestroyAscendStream." << GetErrorInfo(ret);
        deinitRet = APP_ERR_COMM_FAILURE;
    }

    ret = MxBase::DeviceManager::GetInstance()->DestroyDevices();
    if (ret != APP_ERR_OK) {
        LogError << "DestroyDevices failed." << GetErrorInfo(ret, "aclFinalize");
        deinitRet = APP_ERR_COMM_FAILURE;
    }
    ret = StreamTensorManager::GetInstance()->DeInit();
    if (ret != APP_ERR_OK) {
        LogError << "Fail to destroy the resources of StreamTensorManager." << GetErrorInfo(ret);
        deinitRet = APP_ERR_COMM_FAILURE;
    }
    LogInfo << "MxDeInit finished.";
    ret = MxBase::Log::Deinit();
    if (ret != APP_ERR_OK) {
        LogError << "Deinitialize log failed." << GetErrorInfo(ret);
        deinitRet = APP_ERR_COMM_FAILURE;
    }
    return deinitRet;
}

APP_ERROR MxInit()
{
    APP_ERROR ret = MxBase::Log::Init();
    if (ret != APP_ERR_OK) {
        LogError << "Initialize log failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = MxBase::LogManager::GetInstance()->LogRotateStart();
    if (ret != APP_ERR_OK) {
        LogError << "Init log rotate thread failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = MxBase::DeviceManager::GetInstance()->InitDevices();
    if (ret != APP_ERR_OK) {
        LogError << "Init devices failed." << GetErrorInfo(ret);
        return ret;
    }

    if (DeviceManager::IsAscend310P() || DeviceManager::IsAtlas800IA2()) {
        AppGlobalCfgExtra appGlobalCfgExtra;
        MxBase::DvppPool::GetInstance().SetChnNum(appGlobalCfgExtra);
    } else {
        LogWarn << "Vpc Channel Pool not initialized, it's only supported on device 310P/Atlas 800I A2 now, current is "
                << DeviceManager::GetSocName() << ", the tensor dvpp functions will not available.";
    }
    return APP_ERR_OK;
}

APP_ERROR MxInit(const AppGlobalCfg &globalCfg)
{
    AppGlobalCfgExtra appGlobalCfgExtra;
    appGlobalCfgExtra.vpcChnNum = globalCfg.vpcChnNum;
    return MxInit(appGlobalCfgExtra);
}

APP_ERROR MxInit(const AppGlobalCfgExtra &globalCfgExtra)
{
    if (!DeviceManager::IsAscend310P() && !DeviceManager::IsAtlas800IA2()) {
        LogError << "Dvpp pool only supported on device 310P/Atlas 800I A2 now,"
                 << " current device is " << DeviceManager::GetSocName() << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (globalCfgExtra.vpcChnNum < MIN_CHN_POOL_SIZE || globalCfgExtra.vpcChnNum > MAX_VPC_CHN_POOL_SIZE) {
        LogError << "Vpc channel pool size must in range [" << MIN_CHN_POOL_SIZE << ", "
                 << MAX_VPC_CHN_POOL_SIZE << "], input is: " << globalCfgExtra.vpcChnNum << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (globalCfgExtra.jpegdChnNum < MIN_CHN_POOL_SIZE || globalCfgExtra.jpegdChnNum > MAX_JPEGD_CHN_POOL_SIZE) {
        LogError << "Jpegd channel pool size must in range [" << MIN_CHN_POOL_SIZE << ", "
                 << MAX_JPEGD_CHN_POOL_SIZE << "], input is: " << globalCfgExtra.jpegdChnNum << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (globalCfgExtra.pngdChnNum < MIN_CHN_POOL_SIZE || globalCfgExtra.pngdChnNum > MAX_PNGD_CHN_POOL_SIZE) {
        LogError << "Pngd channel pool size must in range [" << MIN_CHN_POOL_SIZE << ", "
                 << MAX_PNGD_CHN_POOL_SIZE << "], input is: " << globalCfgExtra.pngdChnNum << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    if (globalCfgExtra.jpegeChnNum < MIN_CHN_POOL_SIZE || globalCfgExtra.jpegeChnNum > MAX_JPEGE_CHN_POOL_SIZE) {
        LogError << "Jpege channel pool size must in range [" << MIN_CHN_POOL_SIZE << ", "
                 << MAX_JPEGE_CHN_POOL_SIZE << "], input is: " << globalCfgExtra.jpegeChnNum << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    APP_ERROR ret = MxBase::Log::Init();
    if (ret != APP_ERR_OK) {
        LogError << "Initialize log failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = MxBase::LogManager::GetInstance()->LogRotateStart();
    if (ret != APP_ERR_OK) {
        LogError << "Initialize log rotate thread failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = MxBase::DeviceManager::GetInstance()->InitDevices();
    if (ret != APP_ERR_OK) {
        LogError << "Initialize devices failed." << GetErrorInfo(ret);
        return ret;
    }
    MxBase::DvppPool::GetInstance().SetChnNum(globalCfgExtra);
    return ret;
}

APP_ERROR LoadConfig(const std::string &path)
{
    GlobalOpMap opMap;

    // 1. Check op json config path validity.
    std::string realPath;
    if (!FileUtils::RegularFilePath(path, realPath)) {  // Check link and file exists.
        LogError << "MxInitFromConfig: GlobalOpMap check file path failed." << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return APP_ERR_COMM_INVALID_PATH;
    }
    if (!FileUtils::IsFileValid(realPath)) {  // Check file validity and owner.
        LogError << "MxInitFromConfig: GlobalOpMap check file path valid failed."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PATH);
        return APP_ERR_COMM_INVALID_PATH;
    }

    std::string::size_type fast = realPath.find_last_of('.');
    if (fast == std::string::npos) {
        LogError << "MxInitFromConfig: GlobalOpMap split filename failed."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    std::string fileFormat = realPath.substr(fast + 1, realPath.size() - fast);
    if (fileFormat != "json") {
        LogError << "MxInitFromConfig: GlobalOpMap configfile Only support json format."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // 2. Load config into opMap.
    APP_ERROR ret = opMap.LoadConfig(realPath);  // Load config.
    if (ret != APP_ERR_OK) {
        LogError << "MxInitFromConfig: GlobalOpMap loadConfig failed." << GetErrorInfo(ret);
        return ret;
    }
    ret = opMap.InitLoaderMap();
    if (ret != APP_ERR_OK) {
        LogError << "MxInitFromConfig: InitLoaderMap failed." << GetErrorInfo(ret);
        return ret;
    }

    // 3. Load all operations.
    LogInfo << "Start running LoadAllOperations.";
    ret = opMap.LoadAllOperations();
    if (ret != APP_ERR_OK) {
        LogError << "MxInitFromConfig: LoadAllOperations failed." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR MxInitFromConfig(const std::string &path)
{
    if (!DeviceManager::IsAscend310P() && !DeviceManager::IsAscend310B()) {
        LogError << "MxInitFromConfig is only supported on device 310P/310B now,"
                 << " current device is " << DeviceManager::GetSocName() << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // 1. Run MxInit.
    APP_ERROR ret = MxInit();
    if (ret != APP_ERR_OK) {
        LogError << "MxInitFromConfig: MxInit failed." << GetErrorInfo(ret);
        return ret;
    }

    // 2. Load config.
    ret = LoadConfig(path);
    if (ret != APP_ERR_OK) {
        LogError << "MxInitFromConfig: LoadConfig failed." << GetErrorInfo(ret);
    }
    return ret;
}

APP_ERROR MxInitFromConfig(const std::string &configFile, const AppGlobalCfg &globalCfg)
{
    AppGlobalCfgExtra appGlobalCfgExtra;
    appGlobalCfgExtra.vpcChnNum = globalCfg.vpcChnNum;
    return MxInitFromConfig(configFile, appGlobalCfgExtra);
}

APP_ERROR MxInitFromConfig(const std::string &configFile, const AppGlobalCfgExtra &globalCfgExtra)
{
    if (!DeviceManager::IsAscend310P() && !DeviceManager::IsAscend310B()) {
        LogError << "MxInitFromConfig is only supported on device 310P/310B now,"
                 << " current device is " << DeviceManager::GetSocName() << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }

    // 1. Run MxInit.
    APP_ERROR ret = MxInit(globalCfgExtra);
    if (ret != APP_ERR_OK) {
        LogError << "MxInitFromConfig: MxInit failed." << GetErrorInfo(ret);
        return ret;
    }

    // 2. Load config.
    ret = LoadConfig(configFile);
    if (ret != APP_ERR_OK) {
        LogError << "MxInitFromConfig: LoadConfig failed." << GetErrorInfo(ret);
    }
    return ret;
}
}
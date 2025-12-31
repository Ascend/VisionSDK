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

#ifndef GLOBAL_INIT_H
#define GLOBAL_INIT_H

#include "MxBase/Log/Log.h"

namespace MxBase {

namespace {
    constexpr uint32_t DEFAULT_VPC_CHN_NUM = 48;
    constexpr uint32_t DEFAULT_JPEGD_CHN_NUM = 24;
    constexpr uint32_t DEFAULT_JPEGE_CHN_NUM = 24;
    constexpr uint32_t DEFAULT_PNGD_CHN_NUM = 24;
}

struct AppGlobalCfg {
    uint32_t vpcChnNum = DEFAULT_VPC_CHN_NUM;
};

struct AppGlobalCfgExtra {
    uint32_t vpcChnNum = DEFAULT_VPC_CHN_NUM;
    uint32_t jpegdChnNum = DEFAULT_JPEGD_CHN_NUM;
    uint32_t pngdChnNum = DEFAULT_PNGD_CHN_NUM;
    uint32_t jpegeChnNum = DEFAULT_JPEGE_CHN_NUM;
    virtual ~AppGlobalCfgExtra() = default;
};

APP_ERROR MxInit();

/**
 * @description: MxInit for vpc channel pool.
 * @param globalCfg: AppGlobalCfg struct.
 */
APP_ERROR MxInit(const AppGlobalCfg &globalCfg);

/**
 * @description: MxInit for dvpp channel pool.
 * @param globalCfgExtra: AppGlobalCfgExtra struct.
 */
APP_ERROR MxInit(const AppGlobalCfgExtra &globalCfgExtra);

APP_ERROR MxInitFromConfig(const std::string &configFile);

/**
 * @description: MxInitFromConfig for op preload and vpc channel pool.
 * @param configFile: op preload json config file path.
 * @param globalCfg: AppGlobalCfg struct.
 */
APP_ERROR MxInitFromConfig(const std::string &configFile, const AppGlobalCfg &globalCfg);

/**
 * @description: MxInitFromConfig for op preload and dvpp channel pool.
 * @param configFile: op preload json config file path.
 * @param globalCfgExtra: AppGlobalCfgExtra struct.
 */
APP_ERROR MxInitFromConfig(const std::string &configFile, const AppGlobalCfgExtra &globalCfgExtra);

APP_ERROR MxDeInit();

}
    
#endif
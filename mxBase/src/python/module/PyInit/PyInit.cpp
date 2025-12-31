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
 * Description: Python init apis.
 * Author: MindX SDK
 * Create: 2024
 * History: NA
 */

#include "PyInit/PyInit.h"

namespace PyBase {
APP_ERROR mx_init()
{
    return MxBase::MxInit();
}
APP_ERROR mx_init(const AppGlobalCfgExtra& config)
{
    MxBase::AppGlobalCfgExtra globalCfgExtra;
    globalCfgExtra.jpegdChnNum = config.jpegdChnNum;
    globalCfgExtra.jpegeChnNum = config.jpegeChnNum;
    globalCfgExtra.pngdChnNum = config.pngdChnNum;
    globalCfgExtra.vpcChnNum = config.vpcChnNum;
    return MxBase::MxInit(globalCfgExtra);
}
APP_ERROR mx_deinit()
{
    return MxBase::MxDeInit();
}
} // namespace PyBase
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
 * Description: Global Operation Map for operator preload.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#ifndef MXBASE_GLOBALOPMAP_H
#define MXBASE_GLOBALOPMAP_H

#include "OperationLoaders/ConfigLoader.h"
#include "OperationLoaders/OpLoader.h"
#include "OperationLoaders/OpLoaderSplit.h"
#include "OperationLoaders/OpLoaderClip.h"
#include "OperationLoaders/OpLoaderCompare.h"
#include "OperationLoaders/OpLoaderMerge.h"
#include "OperationLoaders/OpLoaderHstack.h"
#include "OperationLoaders/OpLoaderVstack.h"
#include "OperationLoaders/OpLoaderTile.h"
#include "OperationLoaders/OpLoaderRotate.h"
#include "OperationLoaders/OpLoaderTranspose.h"
#include "OperationLoaders/OpLoaderConvertTo.h"
#include "OperationLoaders/OpLoaderReduce.h"
#include "OperationLoaders/OpLoaderSortIdx.h"
#include "OperationLoaders/OpLoaderSort.h"
#include "OperationLoaders/OpLoaderDivide.h"
#include "OperationLoaders/OpLoaderMultiply.h"

namespace MxBase {

    class GlobalOpMap {
    public:
        GlobalOpMap();

        explicit GlobalOpMap(std::string configFile);

        GlobalOpMap(const GlobalOpMap&) = delete;

        GlobalOpMap& operator=(const GlobalOpMap&) = delete;

        ~GlobalOpMap();

        APP_ERROR LoadConfig(std::string configFile);

        APP_ERROR InitLoaderMap();

        APP_ERROR LoadAllOperations();

    private:
        std::map<std::string, OpLoader *> g_LoaderMap;
        ConfigLoader *configLoader = nullptr;
        OpLoader *opLoaderAdd = nullptr;
        OpLoader *opLoaderSub = nullptr;
        OpLoaderDivide *opLoaderDiv = nullptr;
        OpLoaderMultiply *opLoaderMul = nullptr;
        OpLoader *opLoaderPow = nullptr;
        OpLoader *opLoaderSqr = nullptr;
        OpLoader *opLoaderExp = nullptr;
        OpLoader *opLoaderLog = nullptr;
        OpLoaderSplit *opLoaderSplit = nullptr;
        OpLoaderClip *opLoaderClip = nullptr;
        OpLoaderCompare *opLoaderCompare = nullptr;
        OpLoader *opLoaderBitwiseAnd = nullptr;
        OpLoader *opLoaderBitwiseOr = nullptr;
        OpLoader *opLoaderBitwiseXor = nullptr;
        OpLoaderMerge *opLoaderMerge = nullptr;
        OpLoaderHstack *opLoaderHstack = nullptr;
        OpLoaderVstack *opLoaderVstack = nullptr;
        OpLoaderTile *opLoaderTile = nullptr;
        OpLoaderRotate *opLoaderRotate = nullptr;
        OpLoaderTranspose *opLoaderTranspose = nullptr;
        OpLoader *opLoaderSqrt = nullptr;
        OpLoader *opLoaderThresholdBinary = nullptr;
        OpLoader *opLoaderAddWeighted = nullptr;
        OpLoader *opLoaderAbsDiff = nullptr;
        OpLoader *opLoaderMRGBA = nullptr;
        OpLoader *opLoaderAbs = nullptr;
        OpLoader *opLoaderScaleAdd = nullptr;
        OpLoader *opLoaderMin = nullptr;
        OpLoader *opLoaderMax = nullptr;
        OpLoaderConvertTo *opLoaderConvertTo = nullptr;
        OpLoaderReduce *opLoaderReduce = nullptr;
        OpLoaderSortIdx *opLoaderSortIdx = nullptr;
        OpLoaderSort *opLoaderSort = nullptr;
        OpLoader *opLoaderRescale = nullptr;
        bool useConfig = false;
    };
}
#endif

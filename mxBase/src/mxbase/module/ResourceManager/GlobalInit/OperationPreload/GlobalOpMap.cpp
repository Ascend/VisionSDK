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
 * Description: Config load for preload jason config.
 * Author: MindX SDK
 * Create: 2023
 * History: NA
 */

#include <map>
#include "OperationLoaders/ConfigLoader.h"
#include "OperationLoaders/OpLoader.h"
#include "GlobalOpMap.h"

namespace MxBase {
    GlobalOpMap::GlobalOpMap() {}

    GlobalOpMap::GlobalOpMap(std::string configFile)
    {
        try {
            configLoader = new ConfigLoader(configFile);
        } catch (const std::runtime_error &e) {
            LogError << "GlobalOpMap LoadConfig failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return;
        }
    }

    GlobalOpMap::~GlobalOpMap()
    {
        delete configLoader;
        configLoader = nullptr;
        for (auto it: g_LoaderMap) {
            if (it.second != nullptr) {
                delete it.second;
                it.second = nullptr;
            }
        }
    }

    APP_ERROR GlobalOpMap::LoadConfig(std::string configFile)
    {
        try {
            configLoader = new ConfigLoader(configFile);
        } catch (const std::runtime_error &e) {
            LogError << "GlobalOpMap LoadConfig failed." << GetErrorInfo(APP_ERR_COMM_INIT_FAIL);
            return APP_ERR_COMM_INIT_FAIL;
        }
        return APP_ERR_OK;
    }

    APP_ERROR GlobalOpMap::InitLoaderMap()
    {
        opLoaderAdd = new OpLoader("Add");
        opLoaderSub = new OpLoader("Subtract");
        opLoaderDiv = new OpLoaderDivide("Divide");
        opLoaderMul = new OpLoaderMultiply("Multiply");
        opLoaderPow = new OpLoader("Pow");
        opLoaderSqr = new OpLoader("Sqr");
        opLoaderExp = new OpLoader("Exp");
        opLoaderLog = new OpLoader("Log");
        opLoaderSplit = new OpLoaderSplit("Split");
        opLoaderClip = new OpLoaderClip("Clip");
        opLoaderCompare = new OpLoaderCompare("Compare");
        opLoaderBitwiseAnd = new OpLoader("BitwiseAnd");
        opLoaderBitwiseOr = new OpLoader("BitwiseOr");
        opLoaderBitwiseXor = new OpLoader("BitwiseXor");
        opLoaderMerge = new OpLoaderMerge("Merge");
        opLoaderHstack = new OpLoaderHstack("Hstack");
        opLoaderVstack = new OpLoaderVstack("Vstack");
        opLoaderTile = new OpLoaderTile("Tile");
        opLoaderRotate = new OpLoaderRotate("Rotate");
        opLoaderTranspose = new OpLoaderTranspose("Transpose");
        opLoaderSqrt = new OpLoader("Sqrt");
        opLoaderThresholdBinary = new OpLoader("ThresholdBinary");
        opLoaderAddWeighted = new OpLoader("AddWeighted");
        opLoaderAbsDiff = new OpLoader("AbsDiff");
        opLoaderMRGBA = new OpLoader("MRGBA");
        opLoaderAbs = new OpLoader("Abs");
        opLoaderScaleAdd = new OpLoader("ScaleAdd");
        opLoaderMin = new OpLoader("Min");
        opLoaderMax = new OpLoader("Max");
        opLoaderConvertTo = new OpLoaderConvertTo("ConvertTo");
        opLoaderReduce = new OpLoaderReduce("Reduce");
        opLoaderSortIdx = new OpLoaderSortIdx("SortIdx");
        opLoaderSort = new OpLoaderSort("Sort");
        opLoaderRescale = new OpLoader("Rescale");
        g_LoaderMap["Add"] = opLoaderAdd;
        g_LoaderMap["Subtract"] = opLoaderSub;
        g_LoaderMap["Divide"] = opLoaderDiv;
        g_LoaderMap["Multiply"] = opLoaderMul;
        g_LoaderMap["Pow"] = opLoaderPow;
        g_LoaderMap["Sqr"] = opLoaderSqr;
        g_LoaderMap["Exp"] = opLoaderExp;
        g_LoaderMap["Log"] = opLoaderLog;
        g_LoaderMap["Split"] = opLoaderSplit;
        g_LoaderMap["Clip"] = opLoaderClip;
        g_LoaderMap["Compare"] = opLoaderCompare;
        g_LoaderMap["BitwiseAnd"] = opLoaderBitwiseAnd;
        g_LoaderMap["BitwiseOr"] = opLoaderBitwiseOr;
        g_LoaderMap["BitwiseXor"] = opLoaderBitwiseXor;
        g_LoaderMap["Merge"] = opLoaderMerge;
        g_LoaderMap["Hstack"] = opLoaderHstack;
        g_LoaderMap["Vstack"] = opLoaderVstack;
        g_LoaderMap["Tile"] = opLoaderTile;
        g_LoaderMap["Rotate"] = opLoaderRotate;
        g_LoaderMap["Transpose"] = opLoaderTranspose;
        g_LoaderMap["Sqrt"] = opLoaderSqrt;
        g_LoaderMap["ThresholdBinary"] = opLoaderThresholdBinary;
        g_LoaderMap["AddWeighted"] = opLoaderAddWeighted;
        g_LoaderMap["AbsDiff"] = opLoaderAbsDiff;
        g_LoaderMap["MRGBA"] = opLoaderMRGBA;
        g_LoaderMap["Abs"] = opLoaderAbs;
        g_LoaderMap["ScaleAdd"] = opLoaderScaleAdd;
        g_LoaderMap["Min"] = opLoaderMin;
        g_LoaderMap["Max"] = opLoaderMax;
        g_LoaderMap["ConvertTo"] = opLoaderConvertTo;
        g_LoaderMap["Reduce"] = opLoaderReduce;
        g_LoaderMap["SortIdx"] = opLoaderSortIdx;
        g_LoaderMap["Sort"] = opLoaderSort;
        g_LoaderMap["Rescale"] = opLoaderRescale;
        return APP_ERR_OK;
    }

    APP_ERROR GlobalOpMap::LoadAllOperations()
    {
        APP_ERROR ret = APP_ERR_OK;
        std::vector<std::string> opNames;
        ret = configLoader->GetAllOpNames(opNames);
        if (ret != APP_ERR_OK) {
            LogError << "ConfigLoader getAllOpNames failed." << GetErrorInfo(ret);
        }
        for (std::string opName: opNames) {
            LogInfo << "Start Loading op " << opName << ".";
            JsonPtr jsonPtr;
            ret = configLoader->GetOpInfoByName(opName, jsonPtr);
            if (ret != APP_ERR_OK) {
                LogError << "LoadOpHandles: failed to get info for op: " << opName << "." << GetErrorInfo(ret);
                continue;
            }
            if (g_LoaderMap.find(opName) == g_LoaderMap.end()) {
                LogWarn << "LoadOpHandles: op [" << opName << "] not subscribed in Preload Op Map."
                         << GetErrorInfo(APP_ERR_COMM_FAILURE);
                continue;
            }
            ret = g_LoaderMap[opName]->LoadOpHandles(jsonPtr);
            if (ret != APP_ERR_OK) {
                LogError << "LoadOpHandles: failed to load op: " << opName << "." << GetErrorInfo(ret);
                continue;
            }
        }

        return ret;
    }
}
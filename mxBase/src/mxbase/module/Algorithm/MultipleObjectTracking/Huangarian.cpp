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
 * Description: Initialize the matching matrix of the Hungarian algorithm.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include "MxBase/CV/MultipleObjectTracking/Huangarian.h"
#include "dvpp/securec.h"

namespace MxBase {
const int HANDLE_MAX = 8192;
APP_ERROR HungarianHandleInit(HungarianHandle &handle, const int row, const int cols)
{
    if (row <= 0 || row > HANDLE_MAX || cols <= 0 || cols > HANDLE_MAX) {
        LogError << "Input params: row and col must be larger than 0 and smaller than or equal to " << HANDLE_MAX
                 << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return APP_ERR_COMM_INVALID_PARAM;
    }
    handle.max = (row > cols) ? row : cols;
    void* adjMat = malloc(handle.max * handle.max * sizeof(int));
    if (adjMat == nullptr) {
        LogError << "HungarianHandleInit Malloc failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        return APP_ERR_COMM_ALLOC_MEM;
    }
    APP_ERROR ret = memset_s(adjMat, handle.max * handle.max * sizeof(int), 0, handle.max * handle.max * sizeof(int));
    if (ret != APP_ERR_OK) {
        LogError << "HungarianHandleInit Memset failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
        free(adjMat);
        adjMat = nullptr;
        return APP_ERR_COMM_ALLOC_MEM;
    }
    handle.adjMat.reset((int*) adjMat, free);

    void* ptr[HUNGARIAN_CONTENT] = {nullptr};
    for (int i = 0; i < HUNGARIAN_CONTENT; ++i) {
        ptr[i] = malloc(handle.max * sizeof(int));
        if (ptr[i] == nullptr) {
            for (int j = 0; j < i; ++j) {
                free(ptr[j]);
                ptr[j] = nullptr;
            }
            LogError << "HungarianHandleInit Malloc failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            return APP_ERR_COMM_ALLOC_MEM;
        }
        ret = memset_s(ptr[i], handle.max * sizeof(int), 0, handle.max * sizeof(int));
        if (ret != APP_ERR_OK) {
            LogError << "HungarianHandleInit Memset failed." << GetErrorInfo(APP_ERR_COMM_ALLOC_MEM);
            for (int j = 0; j <= i; ++j) {
                free(ptr[j]);
                ptr[j] = nullptr;
            }
            return APP_ERR_COMM_ALLOC_MEM;
        }
    }

    handle.xMatch.reset((int*) ptr[X_MATCH_OFFSET], free);
    handle.yMatch.reset((int*) ptr[Y_MATCH_OFFSET], free);
    handle.xValue.reset((int*) ptr[X_VALUE_OFFSET], free);
    handle.yValue.reset((int*) ptr[Y_VALUE_OFFSET], free);
    handle.slack.reset((int*) ptr[SLACK_OFFSET], free);
    handle.xVisit.reset((int*) ptr[X_VISIT_OFFSET], free);
    handle.yVisit.reset((int*) ptr[Y_VISIT_OFFSET], free);

    return APP_ERR_OK;
}

void HungarianInit(HungarianHandle &handle, const std::vector<std::vector<int>> &cost, const int rows, const int cols)
{
    int value = 0;
    if (rows > cols) {
        handle.transpose = true;
        handle.cols = rows;
        handle.rows = cols;
        handle.resX = handle.yMatch.get();
        handle.resY = handle.xMatch.get();
    } else {
        handle.transpose = false;
        handle.rows = rows;
        handle.cols = cols;
        handle.resX = handle.xMatch.get();
        handle.resY = handle.yMatch.get();
    }

    for (int i = 0; i < handle.rows; ++i) {
        handle.xValue.get()[i] = 0;
        handle.xMatch.get()[i] = -1;
        for (int j = 0; j < handle.cols; ++j) {
            if (handle.transpose) {
                value = cost[j][i];
            } else {
                value = cost[i][j];
            }
            handle.adjMat.get()[i * handle.cols + j] = value;
            if (handle.xValue.get()[i] < value) {
                handle.xValue.get()[i] = value;
            }
        }
    }

    for (int i = 0; i < handle.cols; ++i) {
        handle.yValue.get()[i] = 0;
        handle.yMatch.get()[i] = -1;
    }
}

bool Match(const HungarianHandle &handle, const int id)
{
    int delta;
    handle.xVisit.get()[id] = VISITED;
    for (int j = 0; j < handle.cols; ++j) {
        if (handle.yVisit.get()[j] == VISITED) {
            continue;
        }
        delta = handle.xValue.get()[id] + handle.yValue.get()[j] - handle.adjMat.get()[id * handle.cols + j];
        if (delta == 0) {
            handle.yVisit.get()[j] = VISITED;
            if (handle.yMatch.get()[j] == -1 || Match(handle, handle.yMatch.get()[j])) {
                handle.yMatch.get()[j] = id;
                handle.xMatch.get()[id] = j;
                return true;
            }
        } else if (delta < handle.slack.get()[j]) {
            handle.slack.get()[j] = delta;
        }
    }
    return false;
}

bool ObjectAssociated(const HungarianHandle &handle, int &delta, const int id)
{
    while (true) {
        std::fill(handle.xVisit.get(), handle.xVisit.get() + handle.rows, 0);
        std::fill(handle.yVisit.get(), handle.yVisit.get() + handle.cols, 0);

        for (int j = 0; j < handle.cols; ++j) {
            handle.slack.get()[j] = INF;
        }
        if (Match(handle, id)) {
            return true;
        }
        delta = INF;
        for (int j = 0; j < handle.cols; ++j) {
            if (handle.yVisit.get()[j] != VISITED && delta > handle.slack.get()[j]) {
                delta = handle.slack.get()[j];
            }
        }
        if (delta == INF) {
            LogDebug << "Hungarian solve is invalid!";
            return false;
        }
        for (int j = 0; j < handle.rows; ++j) {
            if (handle.xVisit.get()[j] == VISITED) {
                handle.xValue.get()[j] -= delta;
            }
        }
        for (int j = 0; j < handle.cols; ++j) {
            if (handle.yVisit.get()[j] == VISITED) {
                handle.yValue.get()[j] += delta;
            }
        }
    }
}

APP_ERROR HungarianSolve(HungarianHandle &handle, const std::vector<std::vector<int>> &cost,
                         const int rows, const int cols)
{
    if (rows <= 0 || cols <= 0) {
        LogError << "Input params: rows and cols must be larger than 0." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return -1;
    }
    if (rows > handle.max || cols > handle.max) {
        LogError << "Param rows or cols is out of range, rows = " << rows << ", cols = " << cols
                 << ", the required param must be less than or equal to " << handle.max << "."
                 << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return -1;
    }
    if (cost.empty()) {
        LogError << "The rows of cost matrix is empty." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return -1;
    }
    if (size_t(rows) != cost.size() || size_t(cols) != cost[0].size()) {
        LogError << "Param rows or cols is not equal to the rows or cols of cost matrix, rows = " << rows << ", cols = "
                 << cols << ", the rows of cost matrix = " << cost.size() << ", the cols of cost matrix = "
                 << cost[0].size() << "." << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
        return -1;
    }
    for (size_t i = 0; i < cost.size(); i++) {
        if (static_cast<size_t>(cols) != cost[i].size()) {
            LogError << "Param cols is not equal to the cols of cost matrix."
                     << GetErrorInfo(APP_ERR_COMM_INVALID_PARAM);
            return -1;
        }
    }
    if (handle.xMatch == nullptr || handle.yMatch == nullptr || handle.xValue == nullptr || handle.yValue == nullptr ||
        handle.slack == nullptr || handle.xVisit == nullptr || handle.yVisit == nullptr || handle.adjMat == nullptr) {
        LogError << "The handle is uninitialized." << GetErrorInfo(APP_ERR_COMM_NOT_INIT);
        return -1;
    }

    HungarianInit(handle, cost, rows, cols);
    int delta;
    for (int i = 0; i < handle.rows; ++i) {
        if (!ObjectAssociated(handle, delta, i)) {
            return -1;
        }
    }
    return handle.rows;
}
}
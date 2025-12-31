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
 * Description: The Sort Method of Python Implemented in C++.
 * Author: MindX SDK
 * Create: 2020
 * History: NA
 */

#include <algorithm>
#include <cmath>
#include "MxBase/Maths/NpySort.h"

NpySort::NpySort(std::vector<float> preSortVec, std::vector<int> sortIdx)
    : preSortVec_(std::move(preSortVec)), sortIdx_(std::move(sortIdx))
{}

void NpySort::NpyArgQuickSort(bool reverse)
{
    if (!CheckVectors()) {
        return;
    }
    int valLength = static_cast<int>(preSortVec_.size());
    int pl = 0;
    int pr = valLength - 1;
    std::vector<int> sptr;
    std::vector<int> psdepth;
    int cdepth = NpyGetMsb(valLength) * HEAP_SORT_TWO;
    for (;;) {
        if (cdepth < 0) {
            NpyArgHeapSort(pl, pr - pl + 1);
            if (sptr.empty()) {
                break;
            }
            pr = sptr.back();
            sptr.pop_back();
            pl = sptr.back();
            sptr.pop_back();
            cdepth = psdepth.back();
            psdepth.pop_back();
            continue;
        }
        while ((pr - pl) > SMALL_QUICKSORT) {
            QuickSort(pl, pr, sptr, psdepth, cdepth);
        }
        InsertSort(pl, pr);
        if (sptr.empty()) {
            break;
        }
        pr = sptr.back();
        sptr.pop_back();
        pl = sptr.back();
        sptr.pop_back();
        cdepth = psdepth.back();
        psdepth.pop_back();
    }
    if (reverse) {
        std::reverse(std::begin(sortIdx_), std::end(sortIdx_));
    }
}

void NpySort::NpyArgHeapSort(int tosort, int n)
{
    if (!CheckVectors()) {
        return;
    }
    int l;
    int tmp;
    std::vector<int> sortIdxHeap;
    if (tosort < 1 || tosort + n + 1 > int(sortIdx_.size()) || n < MIN_N) {
        LogWarn << "The sort algorithm of post process error! ";
        return;
    }
    sortIdxHeap.assign(sortIdx_.begin() + tosort - 1, sortIdx_.begin() + tosort + n + 1);
    for (l = n / HEAP_SORT_TWO; l > 0; --l) {
        tmp = sortIdxHeap[l];
        SwapValueForHeapSort(l, l * HEAP_SORT_TWO, n, tmp, sortIdxHeap);
    }
    for (; n > 1;) {
        tmp = sortIdxHeap[n];
        sortIdxHeap[n] = sortIdxHeap[1];
        n -= 1;
        SwapValueForHeapSort(1, HEAP_SORT_TWO, n, tmp, sortIdxHeap);
    }
}

std::vector<int> NpySort::GetSortIdx()
{
    return sortIdx_;
}

void NpySort::SwapValueForHeapSort(int iV, int jV, int n, int tmp, std::vector<int> &sortIdxHeap)
{
    int i = iV;
    int j = jV;
    for (; j <= n;) {
        if (j < n && Compare(preSortVec_[sortIdxHeap[j]], preSortVec_[sortIdxHeap[j + 1]]) < 0) {
            ++j;
        }
        if (Compare(preSortVec_[tmp], preSortVec_[sortIdxHeap[j]]) < 0) {
            sortIdxHeap[i] = sortIdxHeap[j];
            i = j;
            j += j;
        } else {
            break;
        }
    }
    sortIdxHeap[i] = tmp;
}
int NpySort::Compare(float pa, float pb)
{
    return (pa < pb) ? -1 : (std::fabs(pa - pb) < EPSILON ? 0 : 1);
}

int NpySort::NpyGetMsb(uint unum)
{
    int depth_limit = 0;
    while (unum >>= 1) {
        depth_limit++;
    }
    return depth_limit;
}
void NpySort::QuickSort(int &pl, int &pr, std::vector<int> &sptr, std::vector<int> &psdepth, int &cdepth)
{
    size_t pminus = static_cast<size_t>(pr - pl);
    int pm = pl + static_cast<int>((pminus) >> 1);
    if (Compare(preSortVec_[sortIdx_[pm]], preSortVec_[sortIdx_[pl]]) < 0) {
        std::swap(sortIdx_[pm], sortIdx_[pl]);
    }
    if (Compare(preSortVec_[sortIdx_[pr]], preSortVec_[sortIdx_[pm]]) < 0) {
        std::swap(sortIdx_[pr], sortIdx_[pm]);
    }
    if (Compare(preSortVec_[sortIdx_[pm]], preSortVec_[sortIdx_[pl]]) < 0) {
        std::swap(sortIdx_[pm], sortIdx_[pl]);
    }
    float vp = preSortVec_[sortIdx_[pm]];
    int pi = pl;
    int pj = pr - 1;
    std::swap(sortIdx_[pm], sortIdx_[pj]);
    for (;;) {
        do {
            ++pi;
        } while (Compare(preSortVec_[sortIdx_[pi]], vp) < 0 && pi < pj);
        do {
            --pj;
        } while (Compare(vp, preSortVec_[sortIdx_[pj]]) < 0 && pi < pj);
        if (pi >= pj) {
            break;
        }
        std::swap(sortIdx_[pi], sortIdx_[pj]);
    }
    int pk = pr - 1;
    std::swap(sortIdx_[pi], sortIdx_[pk]);
    // push largest partition into vector
    if (pi - pl < pr - pi) {
        sptr.push_back(pi + 1);
        sptr.push_back(pr);
        pr = pi - 1;
    } else {
        sptr.push_back(pl);
        sptr.push_back(pi - 1);
        pl = pi + 1;
    }
    psdepth.push_back(--cdepth);
}
void NpySort::InsertSort(int &pl, int &pr)
{
    for (int pi = pl + 1; pi <= pr; ++pi) {
        int vi = sortIdx_[pi];
        float vp = preSortVec_[vi];
        int pj = pi;
        int pk = pi - 1;
        while (pj > pl && pk >=0 && Compare(vp, preSortVec_[sortIdx_[pk]]) < 0) {
            sortIdx_[pj] = sortIdx_[pk];
            pj--;
            pk--;
        }
        sortIdx_[pj] = vi;
    }
}
bool NpySort::CheckVectors()
{
    int valLength = static_cast<int>(preSortVec_.size());
    int idxLength = static_cast<int>(sortIdx_.size());
    if (valLength != idxLength || valLength == 0) {
        LogError << "Sort function error, please check it." << GetErrorInfo(APP_ERR_COMM_FAILURE);
        return false;
    }
    for (int idx : sortIdx_) {
        if (idx >= idxLength || idx < 0) {
            LogError << "Sort idx is out of range." << GetErrorInfo(APP_ERR_COMM_FAILURE);
            return false;
        }
    }
    return true;
}

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

#ifndef MINDXSDK_NPYAQUICKSORT_H
#define MINDXSDK_NPYAQUICKSORT_H

#include <vector>
#include "MxBase/Log/Log.h"

const int SMALL_QUICKSORT = 15;
const int HEAP_SORT_TWO = 2;
const int MIN_N = -2;
const float EPSILON = 1e-6;

class NpySort {
public:
    explicit NpySort() = default;

    NpySort(std::vector<float> preSortVec, std::vector<int> sortIdx);

    ~NpySort() {};

    void NpyArgQuickSort(bool reverse);

    void NpyArgHeapSort(int tosort, int n);

    std::vector<int> GetSortIdx();

private:
    std::vector<float> preSortVec_;
    std::vector<int> sortIdx_;
    void SwapValueForHeapSort(int iV, int jV, int n, int tmp, std::vector<int> &sortIdxHeap);

    int Compare(float pa, float pb);

    int NpyGetMsb(uint unum);

    void QuickSort(int &pl, int &pr, std::vector<int> &sptr, std::vector<int> &psdepth, int &cdepth);

    void InsertSort(int &pl, int &pr);

    bool CheckVectors();
};


#endif // MINDXSDK_NPYAQUICKSORT_H

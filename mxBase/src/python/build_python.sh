#!/bin/bash

# -------------------------------------------------------------------------
#  This file is part of the Vision SDK project.
# Copyright (c) 2025 Huawei Technologies Co.,Ltd.
#
# Vision SDK is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#
#           http://license.coscl.org.cn/MulanPSL2
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
# See the Mulan PSL v2 for more details.
# -------------------------------------------------------------------------
# Description: Build python so for mxbase.
# Author: MindX SDK
# Create: 2024
# History: NA

set -e
# check params
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <work_space> <frame_project>"
    exit 1
fi

work_space=$1
frame_project=$2
build_type=""
cxx_compiler=""
c_compiler=""
coverage=""
build_tests=""
extra_option=""
# get compile config from mxBase
while IFS= read -r line; do
    if [[ "$line" =~ ^CMAKE_BUILD_TYPE:[^=]*=(.*) ]]; then
        build_type="${BASH_REMATCH[1]}"
    fi
    if [[ "$line" =~ ^CMAKE_CXX_COMPILER:[^=]*=(.*) ]]; then
        cxx_compiler="${BASH_REMATCH[1]}"
    fi
    if [[ "$line" =~ ^CMAKE_C_COMPILER:[^=]*=(.*) ]]; then
        c_compiler="${BASH_REMATCH[1]}"
    fi
    if [[ "$line" =~ ^BUILD_TESTS:[^=]*=(.*) ]]; then
        build_tests="${BASH_REMATCH[1]}"
    fi
    if [[ "$line" =~ ^COVERAGE:[^=]*=(.*) ]]; then
        coverage="${BASH_REMATCH[1]}"
    fi
done < "$work_space/mxBase/build_result/$frame_project/CMakeCache.txt"

extra_option=" -DCMAKE_BUILD_TYPE=$build_type -DCMAKE_CXX_COMPILER=$cxx_compiler -DCMAKE_C_COMPILER=$c_compiler "

if [ "$coverage" == "ON" ]; then
    extra_option+=" -DCOVERAGE=ON "
fi

if [ "$build_tests" == "ON" ]; then
    extra_option+=" -DBUILD_TESTS=ON "
fi

cmake $work_space/mxBase/src/python/ -B $work_space/mxBase/build_result/$frame_project/python -DPYTHON_VERSION=3.11 -DFRAME_PROJECT=$frame_project $extra_option
cd $work_space/mxBase/build_result/$frame_project/python && make -j16 && make install
patch < $work_space/mxBase/src/python/patches/base.patch $work_space/mxBase/mindx/sdk/base/base.py
patch < $work_space/mxBase/src/python/patches/dvpp.patch $work_space/mxBase/mindx/sdk/base/dvpp/dvpp.py
patch < $work_space/mxBase/src/python/patches/log.patch $work_space/mxBase/mindx/sdk/base/log/log.py
patch < $work_space/mxBase/src/python/patches/post.patch $work_space/mxBase/mindx/sdk/base/post/post.py
echo "Build python so for mxbase successfully." 
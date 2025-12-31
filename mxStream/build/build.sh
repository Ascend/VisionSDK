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
# Description: Build script of mxStream.
# Author: mxStream
# Create: 2025
# History: NA
set -e
echo "==============Start building mxStream=============="
work_space="$(cd "$(dirname "$(readlink -f "$0")")" && pwd)/../.."
frame_project=$1
cd ${work_space}
if [ "${frame_project}" != "arm-gcc4" ] && [ "${frame_project}" != "x86-gcc4" ]; then
    echo "Error: Invalid platform type."
    exit 1
fi
if [ -d "${work_space}/mxStream/build_result" ]; then
    rm -rf "${work_space}/mxStream/build_result"
fi
mkdir -p ${work_space}/mxStream/build_result/$frame_project && cd ${work_space}/mxStream/build_result/$frame_project
if [[ x"$2" == x"test" ]]; then
    cmake ../.. -DFRAME_PROJECT=${frame_project} -B $work_space/mxStream/build_result/$frame_project  --no-warn-unused-cli -DBUILD_TESTS=ON -DCOVERAGE=ON
else
    cmake ../.. -DFRAME_PROJECT=${frame_project} -B $work_space/mxStream/build_result/$frame_project --no-warn-unused-cli
fi
make -j12 || { echo "make failed"; exit 1; }
make install
cd ${work_space}
bash ${work_space}/mxStream/src/python/build_python.sh ${work_space} ${frame_project}
cd ${work_space}/mxStream/output/${frame_project}/mxStream
tar -zcf ${work_space}/mxStream/output/mxStream-${frame_project}.tar.gz ./*
echo "==============Building mxStream successfully=============="
exit 0
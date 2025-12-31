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
# Description: Build script of mxBase.
# Author: mxBase
# Create: 2025
# History: NA
set -e
echo "==============Start building mxBase=============="
work_space="$(cd "$(dirname "$(readlink -f "$0")")" && pwd)/../.."
frame_project=$1
cd ${work_space}
if [ "${frame_project}" != "arm-gcc4" ] && [ "${frame_project}" != "x86-gcc4" ]; then
    echo "Error: Invalid platform type."
    exit 1
fi
bash ${work_space}/mxBase/src/operators/operatorascendc/build_op_ascendc.sh ${work_space}/mxBase/src/operators/operatorascendc/
bash ${work_space}/mxBase/src/operators/operatorsdsl/build_op_dsl.sh ${work_space}/mxBase/src/operators/operatorsdsl/
. /usr/local/Ascend/ascend-toolkit/set_env.sh && cd ${work_space}/mxBase/src/operators/warpaffine && python3.11 warp_affine.py ${work_space}/mxBase/src/operators/warpaffine
. /usr/local/Ascend/ascend-toolkit/set_env.sh && cd ${work_space}/mxBase/src/operators/warpperspective && python3.11 warp_perspective.py ${work_space}/mxBase/src/operators/warpperspective
if [ -d "${work_space}/mxBase/build_result" ]; then
    rm -rf "${work_space}/mxBase/build_result"
fi
mkdir -p ${work_space}/mxBase/build_result/$frame_project && cd ${work_space}/mxBase/build_result/$frame_project
if [[ x"$2" == x"test" ]]; then
    cmake ../.. -DFRAME_PROJECT=${frame_project} -B $work_space/mxBase/build_result/$frame_project  --no-warn-unused-cli -DBUILD_TESTS=ON -DCOVERAGE=ON
else
    cmake ../.. -DFRAME_PROJECT=${frame_project} -B $work_space/mxBase/build_result/$frame_project --no-warn-unused-cli
fi
make -j12 || { echo "make failed"; exit 1; }
make install
cd ${work_space}
bash ${work_space}/mxBase/src/python/build_python.sh ${work_space} ${frame_project}
cp -r ${work_space}/mxBase/mindx ${work_space}/mxBase/output/${frame_project}/mxBase
cd ${work_space}/mxBase/output/${frame_project}/mxBase
tar -zcf ${work_space}/mxBase/output/mxBase-${frame_project}.tar.gz ./*
echo "==============Building mxBase successfully=============="
exit 0
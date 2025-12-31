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
# Description: Executing mxbase Repository Test Cases.
# Author: MindX SDK
# Create: 2020
# History: NA

set -e

G_MINDX_HOME=$1
G_ASCEND_HOME="/usr/local/Ascend"
G_DT_COMMON="/home/dtcommon"

echo "init env"
export ASCEND_HOME="${G_ASCEND_HOME}"
export ASCEND_VERSION=ascend-toolkit/latest
export ARCH_PATTERN=x86_64-linux
export LD_LIBRARY_PATH=${ASCEND_HOME}/${ASCEND_VERSION}/${ARCH_PATTERN}/acllib/lib64:${ASCEND_HOME}/${ASCEND_VERSION}/${ARCH_PATTERN}/atc/lib64

export MINDX_HOME="${G_MINDX_HOME}"
export OPENSOURCE_LIB_DIR="${MINDX_HOME}/mxbase/dist/opensource/lib"
export MXBASE_LIB_DIR="${MINDX_HOME}/mxbase/dist/lib"
export MXTOOLS_LIB_DIR="${MINDX_HOME}/mxtools/dist/lib"
export MXSTREAM_LIB_DIR="${MINDX_HOME}/mxstream/dist/lib"
export MXPLUGINS_LIB_DIR="${MINDX_HOME}/mxplugins/dist/lib"
export LD_LIBRARY_PATH="${OPENSOURCE_LIB_DIR}:${MXBASE_LIB_DIR}:${MXTOOLS_LIB_DIR}:${MXSTREAM_LIB_DIR}:${MXPLUGINS_LIB_DIR}:${LD_LIBRARY_PATH}"

echo "clear build folder"
MXBASE_BUILD_DIR="${MINDX_HOME}/mxbase/build"
if [[ -d "${MXBASE_BUILD_DIR}/build" ]]; then
    echo "rm ${MXBASE_BUILD_DIR}/build"
    rm -fr ${MXBASE_BUILD_DIR}/build
fi

echo "prepare opensource"
cp -rf ${G_DT_COMMON}/opensource.tar.gz ${MINDX_HOME}/mxbase/dist/

echo "compile"
cd ${MXBASE_BUILD_DIR}
bash ./build_original.sh --enable-test

echo "prepare mxbase test data"
cp -rf ${G_DT_COMMON}/ModelInfer ${MINDX_HOME}/mxbase/test/dist/

echo "run mxbase dt"
cd ${MXBASE_BUILD_DIR}
bash ./build_original.sh --run-test

echo "generate code coverage report"
lcov -c -d build/src/CMakeFiles/mxbase.dir/module -o all.info --rc lcov_branch_coverage=1
genhtml all.info --branch-coverage -o mxbase_result

exit 0
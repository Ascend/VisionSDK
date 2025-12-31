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
# Description: Build ascendc ops.
# Author: MindX SDK
# Create: 2024
# History: NA

set -e

echo "Start installing Ascendc op."

export LD_LIBRARY_PATH=/usr/lib64:${LD_LIBRARY_PATH}
sourcedir=$1/../../

if [[ -z "$ASCEND_HOME_PATH" ]]; then
    ASCEND_HOME_PATH="/usr/local/Ascend/ascend-toolkit/latest"
else
    ASCEND_HOME_PATH=$ASCEND_HOME_PATH
fi
. "${ASCEND_HOME_PATH}"/../set_env.sh
op_json_path="$sourcedir/operators/operatorsdsl/test.json"

if [[ ! -f "$ASCEND_HOME_PATH/python/site-packages/bin/msopgen" ]]; then
    echo "Install op ERROR:  $ASCEND_HOME_PATH/python/site-packages/bin/msopgen not exit."
    exit 1
fi

if [[ -d "$sourcedir/operators/operatorascendc/op" ]]; then
    rm -fr "$sourcedir/operators/operatorascendc/op"
fi

"$ASCEND_HOME_PATH"/python/site-packages/bin/msopgen gen -i "$op_json_path" -f tf -c ai_core-ascend310P3 -lan cpp -out "$sourcedir"/operators/operatorascendc/op

if [[ $? -ne 0 ]]; then
    echo "Install op ERROR: generate custom op project failed."
    exit 1
fi

if [[ -f "$sourcedir/operators/operatorascendc/op/op_host/test.cpp" ]]; then
    rm -rf "$sourcedir/operators/operatorascendc/op/op_host/test.cpp"
fi

if [[ -f "$sourcedir/operators/operatorascendc/op/op_host/test_tiling.h" ]]; then
    rm -rf "$sourcedir/operators/operatorascendc/op/op_host/test_tiling.h"
fi

if [[ -f "$sourcedir/operators/operatorascendc/op/op_kernel/test.cpp" ]]; then
    rm -rf "$sourcedir/operators/operatorascendc/op/op_kernel/test.cpp"
fi

if [[ ! -f "$sourcedir/operators/operatorascendc/vector_scheduler.h" ]]; then
    echo "$sourcedir/operators/operatorascendc/vector_scheduler.h not exit."
    return
fi
cp -rf "$sourcedir/operators/operatorascendc/vector_scheduler.h"  "$sourcedir/operators/operatorascendc/op/op_kernel"

op_root_path="$sourcedir/operators/operatorascendc/*"
for file in $op_root_path;
do
if [[ -d "$file/op_host" ]];then
    if [[ "$file/op_host" != "$sourcedir/operators/operatorascendc/op/op_host" ]]; then
        cp -rf "$file/op_host" /"$sourcedir"/operators/operatorascendc/op
    fi
fi

if [[ -d "$file/op_kernel" ]];then
    if [[ "$file/op_kernel" != "$sourcedir/operators/operatorascendc/op/op_kernel" ]]; then
        cp -rf "$file/op_kernel" /"$sourcedir"/operators/operatorascendc/op
    fi
fi
done

cd "$sourcedir/operators/operatorascendc/op"
sed -i "s|/usr/local/Ascend/latest|$ASCEND_HOME_PATH|g" "CMakePresets.json"
sed -i "s|--header|--notemp --header|g" "$sourcedir/operators/operatorascendc/op/cmake/makeself.cmake"
sed -i 's|--nomd5|--nomd5 --nocrc|g' "$sourcedir/operators/operatorascendc/op/cmake/makeself.cmake"
sed -i "s|nnopbase|nnopbase opapi|g" "$sourcedir/operators/operatorascendc/op/op_host/CMakeLists.txt"
sed -i 's/-g/ -Wfloat-equal -pipe -fno-common/g' "$sourcedir/operators/operatorascendc/op/op_host/CMakeLists.txt"
sed -i '/add_library(cust_opapi SHARED ${aclnn_src})/a target_compile_options(cust_opapi PRIVATE -Wfloat-equal -pipe -fno-common)' "$sourcedir/operators/operatorascendc/op/op_host/CMakeLists.txt"
sed -i '/include(cmake\/intf.cmake)/a add_compile_options(-Wfloat-equal -pipe -fno-common)' "$sourcedir/operators/operatorascendc/op/CMakeLists.txt"
sed -i 's/cmake \.\./cmake -DCMAKE_CXX_FLAGS="-Wfloat-equal -pipe -fno-common" -DCMAKE_C_FLAGS="-Wfloat-equal -pipe -fno-common " \.\./' "$sourcedir/operators/operatorascendc/op/build.sh"
sed -i 's/execute_process(COMMAND ${CMAKE_COMPILE} -g -fPIC -shared -std=c++11 /execute_process(COMMAND ${CMAKE_COMPILE} -g -fPIC -shared -std=c++11 -Wfloat-equal -pipe -fno-common /' "$sourcedir/operators/operatorascendc/op/cmake/func.cmake"
bash build.sh

if [[ $? -ne 0 ]]; then
    echo "Install op ERROR: build op failed."
    exit 1
fi
op_install_path="$sourcedir/operators/ascendc"
if [[ ! -d "$op_install_path" ]]; then
    echo "$op_install_path not exit, now create it."
    mkdir "$op_install_path"
fi
./build_out/custom_opp_*.run --install-path="$op_install_path"

find "$sourcedir/operators/operatorascendc/op/build_out/" -maxdepth 1 -type f -name "custom_opp_*.run" -exec cp -f {} "$sourcedir/operators/" \;
if [[ $? -ne 0 ]]; then
    echo "Install op ERROR: cp custom_opp_*.run failed."
    exit 1
fi

rm -fr "$sourcedir/operators/operatorascendc/op"
find "$sourcedir/operators/" -maxdepth 1 -type f -name "custom_opp_*.run" -exec mv -f {} "$sourcedir/operators/operatorascendc" \;
if [[ $? -ne 0 ]]; then
    echo "Install op ERROR: mv custom_opp_*.run failed."
    exit 1
fi
rm -fr "$sourcedir/operators/ascendc/vendors/customize/bin"
rm -fr "$sourcedir/operators/ascendc/vendors/customize/framework"
rm -fr "$sourcedir/operators/ascendc/vendors/customize/op_impl"
rm -fr "$sourcedir/operators/ascendc/vendors/customize/op_proto"
rm "$sourcedir/operators/ascendc/vendors/customize/version.info"

echo "Install Ascendc op success."
exit 0
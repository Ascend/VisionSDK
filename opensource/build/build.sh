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
echo "==============Start building opensource=============="

work_space="$(cd "$(dirname "$(readlink -f "$0")")/.." && pwd)"
frame_project=$1
output_path="${work_space}/output/${frame_project}/opensource"
output_device_path="${work_space}/output/${frame_project}/opensource-device"
if [ "${frame_project}" != "arm-gcc4" ] && [ "${frame_project}" != "x86-gcc4" ]; then
    echo "Error: Invalid platform type."
    exit 1
fi
mkdir -p ${output_path}/bin
mkdir -p ${output_path}/include
mkdir -p ${output_path}/lib
mkdir -p ${output_device_path}
build_glog() {
    cd ${work_space}/opensource
    sh ${work_space}/opensource/build_glog.sh
    cp -r ${work_space}/opensource/tmp/glog/* ${output_path}
}

build_openssl() {
    cd ${work_space}/opensource
    sh ${work_space}/opensource/build_openssl.sh
    cp -r ${work_space}/opensource/tmp/openssl/* ${output_path}
}

build_opencv() {
    cd ${work_space}/opensource
    sh ${work_space}/opensource/build_opencv.sh
    cp -r ${work_space}/opensource/tmp/opencv/* ${output_path}
}

build_opencv_device() {
    cd ${work_space}/opensource
    sh ${work_space}/opensource/build_opencv_device.sh
    cp -r ${work_space}/opensource/tmp/opencv-device/* ${output_device_path}
}

build_protobuf() {
    cd ${work_space}/opensource
    sh ${work_space}/opensource/build_protobuf.sh
    cp -r ${work_space}/opensource/tmp/protobuf/* ${output_path}
}

build_libffi() {
    cd ${work_space}/opensource
    sh ${work_space}/opensource/build_libffi.sh
    cp -r ${work_space}/opensource/tmp/libffi/* ${output_path}
}

build_gstreamer() {
    cd ${work_space}/opensource
    sh ${work_space}/opensource/build_gstreamer.sh
    cp -r ${work_space}/opensource/tmp/gstreamer/* ${output_path}
}

build_glib() {
    cd ${work_space}/opensource
    sh ${work_space}/opensource/build_glib.sh
    cp -r ${work_space}/opensource/tmp/glib/* ${output_path}
}

build_json() {
    cd ${work_space}/opensource
    cp -R ${work_space}/opensource/json/single_include/nlohmann ${output_path}/include
}

build_libsrtp() {
    cd ${work_space}/opensource
    sh ${work_space}/opensource/build_libsrtp.sh
}
# Parallel compiling components without dependencies first
# pids=()
build_glog
build_libffi
build_openssl
build_opencv_device
build_json
build_libsrtp
build_protobuf
# for pid in "${pids[@]}"; do
#     if ! wait "$pid"; then
#         echo "Subprocess $pid failed"
#         exit 1
#     fi
# done
# Compiling components with dependencies
build_glib
build_gstreamer
build_opencv
if [[ x"$2" == x"test" ]]; then
    echo " ============================================================================"
    echo "COMPLING MOCK CPP GTEST"
    cd ${work_space}/opensource/mockcpp
    PATCH="${work_space}/opensource/mockcpp_patch/mockcpp-2.7_py3-h3.patch"
    if [ -f "$PATCH" ]; then
        patch -p1 -d ${work_space}/opensource/mockcpp/ < $PATCH
        if [ $? -ne 0 ]; then
            echo "Apply mockcpp patch $PATCH failed."
            exit 254
        fi
    else
        echo "Cannot find mockcpp patch: $PATCH"
        exit 254
    fi
    rm -rf build
    mkdir build && cd build && cmake .. \
    -DCMAKE_INSTALL_PREFIX=/usr/local/mockcpp
    make -j$(nproc)
    make install
    cd ${work_space}/opensource/googletest
    rm -rf build
    mkdir build && cd build && cmake .. \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_INSTALL_PREFIX=/usr/local
    make -j$(nproc)
    make install
fi

# Packing
cd ${work_space}
bash ${work_space}/opensource/cp_required_header.sh ${frame_project}
cp -r ${output_path}/lib64/* ${output_path}/lib/
rm -rf ${output_path}/lib64
rm -rf ${output_path}/share && rm ${output_path}/include/glib-2.0/gobject/gobjectnotifyqueue.c
cp ${work_space}/build/3rdparty.txt ${output_path}
cd ${output_path}
tar -zcf ${work_space}/output/opensource-${frame_project}.tar.gz ./*
cd ${output_device_path}
tar -zcf ${work_space}/output/opensource-device-${frame_project}.tar.gz ./*
echo "==============Building opensource successfully=============="
exit 0
#!/usr/bin/env bash

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

SYSTEM="${1:-}"
PLATFORM="${2:-}"
#VERSION="${3:-}"
RUN_TEST="${3:-}"

CONFIG_FILE="../ci/config/config.ini"
VERSION=$(grep -m 1 "^version:" "${CONFIG_FILE}" | cut -d ':' -f 2 | tr -d '[:space:]')

export CC=/opt/rh/devtoolset-7/root/usr/bin/gcc
export CXX=/opt/rh/devtoolset-7/root/usr/bin/g++
export GCC_HOME=/opt/rh/devtoolset-7/root/usr
export PATH=/opt/rh/devtoolset-7/root/usr/bin:$PATH
export MSLITE_TOOL_HOME=/opt/buildtools/mindspore-lite-2.1.0
. /usr/local/Ascend/ascend-toolkit/set_env.sh
export LD_LIBRARY_PATH=/usr/local/Ascend/driver/lib64:/usr/local/Ascend/driver/lib64/common:/usr/local/Ascend/driver/lib64/driver:$LD_LIBRARY_PATH
if [[ -z "${SYSTEM}" || -z "${PLATFORM}" || -z "${VERSION}" || -z "${RUN_TEST}" ]]; then
    echo "build_all receive invalid input"
    echo "usage: $0 <system> <platform> <version> <test|notest>"
    exit 1
fi

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "========================================"
echo " Vision SDK Build All"
echo " ROOT_DIR : ${ROOT_DIR}"
echo " SYSTEM   : ${SYSTEM}"
echo " PLATFORM : ${PLATFORM}"
echo " VERSION  : ${VERSION}"
echo " TEST     : ${RUN_TEST}"
echo "========================================"


run_script() {
    local script_path="$1"
    shift

    if [[ ! -f "${script_path}" ]]; then
        echo "scripe not foud: ${script_path}"
        exit 1
    fi

    echo "Executing: ${script_path} $*"
    (cd "$(dirname "${script_path}")" && bash "$(basename "${script_path}")" "$@")
}


# run_script "${ROOT_DIR}/opensource/build/build.sh" "${SYSTEM}" "${RUN_TEST}"
TAR_NAME="opensource-${SYSTEM}.tar.gz"
TAR_DEVICE_NAME="opensource-device-${SYSTEM}.tar.gz"
mkdir -p "${ROOT_DIR}/opensource/output/${SYSTEM}/opensource"
mkdir -p "${ROOT_DIR}/opensource/output/${SYSTEM}/opensource-device"
cp "${ROOT_DIR}/${TAR_NAME}" "${ROOT_DIR}/opensource/output/"
cp "${ROOT_DIR}/${TAR_DEVICE_NAME}" "${ROOT_DIR}/opensource/output/"
tar -xzf "${ROOT_DIR}/opensource/output/${TAR_NAME}" \
    -C "${ROOT_DIR}/opensource/output/${SYSTEM}/opensource"

tar -xzf "${ROOT_DIR}/opensource/output/${TAR_DEVICE_NAME}" \
    -C "${ROOT_DIR}/opensource/output/${SYSTEM}/opensource-device"

if [[ x"$RUN_TEST" == x"test" ]]; then
    echo "Preparing Test Dependencies"
    cp "/usr/include/boost" "${ROOT_DIR}/opensource/opensource/mockcpp/include" -rf
    cd ${ROOT_DIR}/opensource/opensource/mockcpp
    rm -rf build
    mkdir build && cd build && cmake .. \
    -DCMAKE_INSTALL_PREFIX=/usr/local/mockcpp
    make -j$(nproc)
    make install
    cd ${ROOT_DIR}/opensource/opensource/googletest
    rm -rf build
    mkdir build && cd build && cmake .. \
    -DBUILD_SHARED_LIBS=ON \
    -DCMAKE_INSTALL_PREFIX=/usr/local
    make -j$(nproc)
    make install
fi

export LD_LIBRARY_PATH=${ROOT_DIR}/opensource/output/${SYSTEM}/opensource/lib:$LD_LIBRARY_PATH
run_script "${ROOT_DIR}/mxBase/build/build.sh" "${SYSTEM}" "${RUN_TEST}"

run_script "${ROOT_DIR}/mxTools/build/build.sh" "${SYSTEM}"

run_script "${ROOT_DIR}/mxStream/build/build.sh" "${SYSTEM}" "${RUN_TEST}"

run_script "${ROOT_DIR}/mxPlugins/build/build.sh" "${SYSTEM}" "${RUN_TEST}"

run_script "${ROOT_DIR}/mxBase/build/package.sh" "${SYSTEM}" "${PLATFORM}" "${VERSION}"

echo "==============Building VisionSDK successfully=============="

# TEST
if [[ x"$RUN_TEST" == x"test" ]]; then
    echo "==============Installing VisionSDK=============="
    RUN_PACKAGE_PATH="${ROOT_DIR}/output/Software/mxVision/Ascend-mindxsdk-mxvision_${VERSION}_linux-${PLATFORM}.run"
    if [[ ! -f "${RUN_PACKAGE_PATH}" ]]; then
            echo "Run Package not foud: ${RUN_PACKAGE_PATH}"
            exit 1
    fi
    chmod +x "${RUN_PACKAGE_PATH}"
    "${RUN_PACKAGE_PATH}" --install --install-path=${ROOT_DIR}/output/Software/mxVision
    # . ${ROOT_DIR}/output/Software/mxVision/mxVision/set_env.sh
    sdk_path=${ROOT_DIR}/output/Software/mxVision/mxVision
    export MX_SDK_HOME="$sdk_path"
    export GST_PLUGIN_SCANNER="${MX_SDK_HOME}/opensource/libexec/gstreamer-1.0/gst-plugin-scanner"
    export GST_PLUGIN_PATH="${MX_SDK_HOME}/opensource/lib/gstreamer-1.0":"${MX_SDK_HOME}/lib/plugins"
    export LD_LIBRARY_PATH="${MX_SDK_HOME}/operators/ascendc/vendors/customize/op_api/lib":"${MX_SDK_HOME}/opensource/lib":"${MX_SDK_HOME}/opensource/lib64":${LD_LIBRARY_PATH}
    export PYTHONPATH=${MX_SDK_HOME}/python:$PYTHONPATH
    export ASCEND_CUSTOM_OPP_PATH=${MX_SDK_HOME}/operators/ascendc/vendors/customize:$ASCEND_CUSTOM_OPP_PATH
    export PATH=/usr1/local/bin:$PATH
    export LD_LIBRARY_PATH=/opt/buildtools/mindspore-lite-2.1.0/mindspore-lite-2.1.0-linux-aarch64/runtime/lib/:$LD_LIBRARY_PATH
    export LD_LIBRARY_PATH=/home/buildtools/mindspore-lite-2.4.0-linux-aarch64/runtime/lib/:$LD_LIBRARY_PATH
    export LD_LIBRARY_PATH=/home/buildtools/mindspore-lite-2.4.0-linux-aarch64/tools/converter/lib/:$LD_LIBRARY_PATH
    export LD_LIBRARY_PATH=${ROOT_DIR}/mxBase/build_result/arm-gcc4/src/mxbase:$LD_LIBRARY_PATH
    export LD_LIBRARY_PATH=${ROOT_DIR}/mxBase/output/arm-gcc4/mxBase/lib/modelpostprocessors:$LD_LIBRARY_PATH
    export PATH=/home/buildtools/lcov_2.0/bin:$PATH
    echo "==============Installing VisionSDK Successfully=============="
    run_script "${ROOT_DIR}/mxTools/build/build.sh" "${SYSTEM}" test
    # mxBase
    echo "==============Tesing mxBase=============="
    ln -s /home/simon/models ${ROOT_DIR}/mxBase/test/models
    chmod 640 ${ROOT_DIR}/mxBase/test/gtest/E2eInfer/GlobalInit/TestInit/*
    chmod 640 ${ROOT_DIR}/mxBase/test/gtest/dist/E2eInfer/GlobalInit/TestInit/*
    ls -l ${ROOT_DIR}/mxBase/test/gtest/dist/E2eInfer/GlobalInit/TestInit/
    cp ${ROOT_DIR}/mxBase/test/sift_model.om ${ROOT_DIR}/output/Software/mxVision/mxVision/bin
    TEST_DIR="${ROOT_DIR}/mxBase/build_result/${SYSTEM}"
    cd ${TEST_DIR}
    CTEST_PARALLEL_LEVEL=2 make test ARGS="-E ^DvppEncodeTest$" || TEST_RC=$?
    # 无论成功失败，都打印日志
    cat ${ROOT_DIR}/mxBase/build_result/arm-gcc4/Testing/Temporary/LastTest.log
    # 如果测试失败，最后再失败退出
    if [ -n "${TEST_RC:-}" ]; then
        exit $TEST_RC
    fi
    make mxbase-lcov
    ln -s ${ROOT_DIR}/mxBase/build_result/arm-gcc4/coverage-html ${ROOT_DIR}/mxBase/build/coverage
    python3 ${ROOT_DIR}/mxBase/build/testcases_xml_report.py ${ROOT_DIR}/mxBase/test ${ROOT_DIR}/mxBase/build/coverage
    # cat ${ROOT_DIR}/mxBase/build_result/arm-gcc4/Testing/Temporary/LastTest.log
    echo "==============Tesing mxBase Successfully=============="

    # mxPlugins
    # echo "==============Tesing mxPlugins=============="
    # # bash ${ROOT_DIR}/output/Software/mxVision/mxVision/operators/opencvosd/generate_osd_om.sh
    # cd ${ROOT_DIR}/mxPlugins/build_result/arm-gcc4/
    # make test ARGS="-R MxpiSemanticSegPostProcessor_deeplabv3_py"
    # make mxplugins-lcov
    # ln -s ${ROOT_DIR}/mxPlugins/build_result/arm-gcc4/coverage-html ${ROOT_DIR}/mxPlugins/build/coverage
    # python3 ${ROOT_DIR}/mxPlugins/build/testcases_xml_report.py ${ROOT_DIR}/mxPlugins/test ${ROOT_DIR}/mxPlugins/build/coverage
    # cat ${ROOT_DIR}/mxPlugins/build_result/arm-gcc4/Testing/Temporary/LastTest.log
    # echo "==============Tesing mxPlugins Successfully=============="

    # mxStream
    echo "==============Tesing mxStream=============="
    export GST_PLUGIN_SCANNER=${ROOT_DIR}/opensource/output/arm-gcc4/opensource/libexec/gstreamer-1.0/gst-plugin-scanner && export GST_PLUGIN_PATH=${ROOT_DIR}/opensource/output/arm-gcc4/opensource/lib/gstreamer-1.0:${GST_PLUGIN_PATH}
    cd ${ROOT_DIR}/mxStream/build_result/arm-gcc4/
    make test ARGS="-R DataTypeTest"
    make mxstream-lcov
    ln -s ${ROOT_DIR}/mxStream/build_result/arm-gcc4/coverage-html ${ROOT_DIR}/mxStream/build/coverage
    python3 ${ROOT_DIR}/mxStream/build/testcases_xml_report.py ${ROOT_DIR}/mxStream/test ${ROOT_DIR}/mxStream/build/coverage
    cat ${ROOT_DIR}/mxStream/build_result/arm-gcc4/Testing/Temporary/LastTest.log
    echo "==============Tesing mxStream Successfully=============="
    
    # mxTools
    echo "==============Tesing mxTools=============="
    cd ${ROOT_DIR}/mxTools/build_result/arm-gcc4/
    make test || TEST_RC=$?
    cat ${ROOT_DIR}/mxTools/build_result/arm-gcc4/Testing/Temporary/LastTest.log
    if [ -n "${TEST_RC:-}" ]; then
        exit $TEST_RC
    fi
    make mxtools-lcov
    ln -s ${ROOT_DIR}/mxTools/build_result/arm-gcc4/coverage-html ${ROOT_DIR}/mxTools/build/coverage
    python3 ${ROOT_DIR}/mxTools/build/testcases_xml_report.py ${ROOT_DIR}/mxTools/test ${ROOT_DIR}/mxTools/build/coverage
    cat ${ROOT_DIR}/mxTools/build_result/arm-gcc4/Testing/Temporary/LastTest.log
    echo "==============Tesing mxTools Successfully=============="
    echo "======Testing Done!========"
fi

exit 0


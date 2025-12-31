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
# Description: Build dsl ops.
# Author: MindX SDK
# Create: 2024
# History: NA

set -e

echo "Start installing dsl op."

if [[ -z "$ASCEND_HOME_PATH" ]]; then
    export ASCEND_HOME_PATH="/usr/local/Ascend/ascend-toolkit/latest"
fi

. "${ASCEND_HOME_PATH}"/../set_env.sh

export LD_LIBRARY_PATH=/usr/lib64:${LD_LIBRARY_PATH}
ASCEND_HOME_PATH=$ASCEND_HOME_PATH
sourcedir=$1/../../

op_gen() {

    op_json_path="$sourcedir/operators/operatorsdsl/test.json"

    if [[ ! -f "$ASCEND_HOME_PATH/python/site-packages/bin/msopgen" ]]; then
        echo "Install op ERROR:  $ASCEND_HOME_PATH/python/site-packages/bin/msopgen not exit."
        exit 1
    fi

    if [[ -d "$sourcedir/operators/operatorsdsl/op" ]]; then
        rm -fr "$sourcedir/operators/operatorsdsl/op"
    fi

    "$ASCEND_HOME_PATH"/python/site-packages/bin/msopgen gen -i "$op_json_path" -f tf -c ai_core-ascend310B2 -out "$sourcedir"/operators/operatorsdsl/op

    if [[ $? -ne 0 ]]; then
        echo "Install op ERROR: generate custom op project failed."
        exit 1
    fi

    rm -rf "$sourcedir/operators/operatorsdsl/op/op_proto/test.h" \
       "$sourcedir/operators/operatorsdsl/op/op_proto/test.cc" \
       "$sourcedir/operators/operatorsdsl/op/tbe/impl/test.py" \
       "$sourcedir/operators/operatorsdsl/op/tbe/op_info_cfg/ai_core/ascend310p/test.ini"
}

modify_dst_name() {
    if [[ ! -d "$ASCEND_HOME_PATH/compiler/include" ]]; then
        echo "Install op ERROR: ASCEND_HOME_PATH/compiler/include not exit."
        exit 1
    fi

    sed -i '19i\export ASCEND_TENSOR_COMPILER_INCLUDE=$ASCEND_HOME_PATH/compiler/include' /"$sourcedir"/operators/operatorsdsl/op/build.sh
    if [[ $? -ne 0 ]]; then
        echo "Install op ERROR: sed build.sh op project failed."
        exit 1
    fi
    makeself_path=/"$sourcedir"/operators/operatorsdsl/op/cmake/util/makeself/makeself-header.sh
    line_count=$(awk "/echo 'You should try option --extract=<path>' >&2/{f=1} f && /^fi/ {f=0;print NR+1}" "${makeself_path}")
    if [ -z "${line_count}" ]; then
        echo "Install op ERROR: line_count is empty."
        exit 1
    fi
    sed -i "${line_count}"'ims_check_user() {\n	userid=\`id -u\` \n	tmpdir_uid=\`stat -c %u \$tmpdir\` \n	if test x"\$userid" != x"\$tmpdir_uid"; then \n	user_name=\`stat -c %U \$tmpdir\` \n	failed_message="Run package was modified by user \$user_name, please check security. Install dsl op failed." \n	echo \$failed_message \n	exit 1 \n	fi \n} \nms_check_user \n	\' "${makeself_path}"
    result1=$?
    sed -i "${line_count}"'s/\\n/\n/g' "${makeself_path}"
    result2=$?
    if [[ $result1 -ne 0 || $result2 -ne 0 ]]; then
        echo "Install op ERROR: sed makeself-header.sh op project failed."
        exit 1
    fi
    sed -i 's/-D_FORTIFY_SOURCE=2 -O2/-D_FORTIFY_SOURCE=2 -O2 -Wall -Wextra -Wfloat-equal -pipe -fno-common/' "${sourcedir}/operators/operatorsdsl/op/op_proto/CMakeLists.txt"
}

prepare_output_dir() {

    mkdir -p op/tmp/vendors

    if [[ ! -d "$ASCEND_HOME_PATH/opp" ]]; then
        echo "Install op ERROR: ASCEND_HOME_PATH/opp not exit."
        exit 1
    fi

    cd "$ASCEND_HOME_PATH/opp"

    mkdir -p vendors

    op_root_path="$sourcedir/operators/operatorsdsl/*"
    for file in $op_root_path;
    do
    if [[ -d "$file/op_proto" ]];then
        if [[ "$file/op_proto" != "$sourcedir/operators/operatorsdsl/op/op_proto" ]]; then 
            cp -rf "$file/op_proto" /"$sourcedir"/operators/operatorsdsl/op
        fi
    fi

    if [[ -d "$file/tbe" ]];then
        if [[ "$file/tbe" != "$sourcedir/operators/operatorsdsl/op/tbe" ]]; then
            cp -rf "$file/tbe" /"$sourcedir"/operators/operatorsdsl/op
        fi
    fi
    done
}

build_op() {
	cd "$sourcedir/operators/operatorsdsl/op"

    sed -i "s/vendor_name=customize/vendor_name=customize_vision/" build.sh

    bash build.sh

    if [[ $? -ne 0 ]]; then
        echo "Install op ERROR: build op failed."
        exit 1
    fi

    find "$sourcedir/operators/operatorsdsl/op/build_out/" -type f -name "custom_opp_*.run" -exec cp -f {} "$sourcedir/operators/operatorsdsl/" \;
    if [[ $? -ne 0 ]]; then
        echo "Install op ERROR: mv custom_opp_*.run failed."
        exit 1
    fi

    rm -fr "$sourcedir/operators/operatorsdsl/op"
    mkdir -p "$sourcedir/operators/operatorsdsl/op/build_out"

    find "$sourcedir/operators/operatorsdsl/" -maxdepth 1 -type f -name "custom_opp_*.run" -exec cp -f {} "$sourcedir/operators/operatorsdsl/op/build_out/" \;
    if [[ $? -ne 0 ]]; then
        echo "Install op ERROR: mv back custom_opp_*.run failed."
        exit 1
    fi
}

op_gen
modify_dst_name
prepare_output_dir
build_op

echo "Install dsl op success."
exit 0
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
# Description: run.sh.
# Author: MindX SDK
# Create: 2021
# History: NA

set -e

# Simple log helper functions
info() { echo -e "\033[1;34m[INFO ][MxStream] $1\033[1;37m" ; }
error() { echo >&2 -e "\033[1;31m[ERROR ][MxStream] $1\033[1;37m" ; }

CUR_PATH=$(cd "$(dirname "$0")" || { error "Failed to check path/to/run.sh" ; exit 1; } ; pwd)

if [ ! -O "$0" ]; then
    error "Error: run.sh is not belong to the current user."
    exit 1
fi

if [ $(id -u) -eq 0 ]; then
    info "Info: This shell is executed by root."
fi

cd "${CUR_PATH}" || { error "Failed to change directory. " ; exit 1; }
. /usr/local/Ascend/ascend-toolkit/set_env.sh
. ../../../set_env.sh

python3 main.py
exit 0
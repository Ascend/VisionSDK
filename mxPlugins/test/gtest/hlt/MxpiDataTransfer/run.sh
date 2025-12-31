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
CUR_PATH=$(cd "$(dirname "$0")" || { warn "Failed to check path/to/run.sh" ; exit ; } ; pwd)

# Simple log helper functions
info() { echo -e "\033[1;34m[INFO ][MxStream] $1\033[1;37m" ; }
warn() { echo >&2 -e "\033[1;31m[WARN ][MxStream] $1\033[1;37m" ; }

handle_arguments() {
  ver="release"
  for var in $@; do
    if [ "$var" == "-d" ]; then
      ver="debug"
    fi
  done
}

prepare_environment() {
  if [[ "${ver}" == "debug" ]]; then
    info "set debug path."
    mindx_home=${CUR_PATH}/../../..
  else
    info "set release path."
    mindx_home=${CUR_PATH}/../../..
  fi
}

handle_arguments "$@"
prepare_environment

export MX_SDK_HOME=${CUR_PATH}
export LD_LIBRARY_PATH=../../../../mxbase/dist/lib/:../../../../mxtools/dist/lib/:../../../../mxstream/dist/lib/:../../../../mxbase/dist/opensource/lib:../../../../mxbase/dist/opensource/lib64:/usr/local/Ascend/ascend-toolkit/latest/acllib/lib64:${LD_LIBRARY_PATH}
export GST_PLUGIN_SCANNER=../../../../mxbase/dist/opensource/libexec/gstreamer-1.0/gst-plugin-scanner
export GST_PLUGIN_PATH=../../../../mxbase/dist/opensource/lib/gstreamer-1.0:../../../../mxplugins/dist/lib/plugins

chmod +x "${CUR_PATH}"/MxpiDataTransferTest
"${CUR_PATH}"/MxpiDataTransferTest

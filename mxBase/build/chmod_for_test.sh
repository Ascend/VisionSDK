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
# Description: SDK uninstallation tool.
# Author: MindX SDK
# Create: 2020
# History: NA

set -e

chmod_for_test()
{
  path=$1
  if [[ ! -d $path ]]; then
    echo "path($path) is invalid"
    return
  fi

  mod_value=640

  #
  find $path/ -perm /u+x -type f -exec chmod 500 {} +

  # mxbase
  find $path/ -type f -name '*.jpg' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.config' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.txt' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.info' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.yuv' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.data' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.log' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.conf' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.om' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.npy' -exec chmod $mod_value {} +

  find $path/ -type f -name 'encode_cat' -exec chmod $mod_value {} +

  # mxstream
  find $path/ -type f -name '*.pipeline' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.pipecont' -exec chmod $mod_value {} +

  # mxplugins
  find $path/ -type f -name '*.input*' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.output*' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.json*' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.bmp' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.names' -exec chmod $mod_value {} +
  find $path/ -type f -name '*.cfg' -exec chmod $mod_value {} +

  find $path/ -type f -name '*.so' -exec chmod $mod_value {} +
}

chmod_for_test ~/log/mindxsdk/tmp/logs/
chmod_for_test $1
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
set -e

# Simple log helper functions
info() { echo -e "\033[1;34m[INFO ][Depend  ] $1\033[1;37m" ; }
warn() { echo >&2 -e "\033[1;31m[WARN ][Depend  ] $1\033[1;37m" ; }

# Build
fileName="pybind11"
packageFQDN="pybind11/pybind11@2.6.2"
packageName="pybind11"
cd "$fileName" || {
  warn "cd to ./opensource/${fileName} failed"
  exit 254
}
info "Building dependency: $packageFQDN."
if [[ -d "./build" ]];then
  rm -r ./build
fi
mkdir build
cd build || {
  warn "PyBind needs a separate directory for building"
  exit 253
}
cmake \
  -DCMAKE_INSTALL_PREFIX="$(pwd)/../../tmp/$packageName" \
  -DPYBIND11_TEST=OFF \
  -G "Unix Makefiles" .. || {
  warn "Building $packageFQDN failed during cmake"
  exit 254
}
make -j12 || {
  warn "Building $packageFQDN failed during make"
  exit 254
}
make install -j || {
  warn "Building $packageFQDN failed during make install"
  exit 254
}
cd ../..
info "Build $packageFQDN done."

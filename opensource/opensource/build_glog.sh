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
# Simple log helper functions
info() { echo -e "\033[1;34m[INFO ][Depend  ] $1\033[1;37m" ; }
warn() { echo >&2 -e "\033[1;31m[WARN ][Depend  ] $1\033[1;37m" ; }

# save current path
CUR_PATH=$(cd "$(dirname "$0")" || { warn "Failed to check path/to/build.sh" ; exit ; } ; pwd)

# Build
fileName="glog"
packageFQDN="google/glog@0.7.0"
packageName="glog"
cd "$fileName" || {
  warn "cd to ./opensource/${fileName} failed"
  exit 254
} # in glog-0.4.0
if [ -d "build" ]; then
    rm -r build || {
      warn "Remove old build directory failed"
      exit 254
    }
fi

cmake . -B build -DBUILD_TESTING=OFF -DBUILD_SHARED_LIBS=ON -DWITH_GFLAGS=OFF -DCMAKE_BUILD_TYPE=Release \
/DGLOG_USE_GLOG_EXPORT \
-DCMAKE_CXX_FLAGS="-std=c++14 -fPIE -D_GLIBCXX_USE_CXX11_ABI=0 -fstack-protector-all -fPIC -Wall -Dgoogle=mindxsdk_private" \
-DCMAKE_SHARED_LINKER_FLAGS="-Wl,-z,relro,-z,now -pie" \
-DCMAKE_INSTALL_PREFIX="${CUR_PATH}/tmp/$packageName" \
-DCMAKE_INSTALL_LIBDIR="${CUR_PATH}/tmp/$packageName/lib" || {
  warn "Building $packageFQDN failed during cmake"
  exit 254
}

cd build
make -s -j12 || {
  warn "Building $packageFQDN failed during make"
  exit 254
}
make install || {
  warn "Building $packageFQDN failed during make install"
  exit 254
}
# Manually delete the symbol table.
if [[ -f "${CUR_PATH}/tmp/glog/lib/libglog.so.0.7.0" ]];then
  strip ${CUR_PATH}/tmp/glog/lib/libglog.so.0.7.0
fi
cd "${CUR_PATH}"
info "Build $packageFQDN done."

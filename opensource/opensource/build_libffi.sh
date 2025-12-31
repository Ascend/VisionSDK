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
work_space="$(cd "$(dirname "$(readlink -f "$0")")" && pwd)/"
cd ${work_space}/
# Build
fileName="libffi"
packageFQDN="libffi@3.4.4"
packageName="libffi"

cd "$fileName" || {
  warn "cd to ./opensource/$fileName failed"
  exit 254
}
info "Building dependency $packageFQDN."
tar -zxvf libffi-3.4.4.tar.gz
info "Patch install start for libffi."
SPEC_FILE="libffi.spec"
PATCH_FILES=$(grep -oP '^\s*Patch\d+\s*:\s*\K.*' "$SPEC_FILE")
for PATCH in $PATCH_FILES; do
    if [ -f "$PATCH" ]; then
        patch -p1 -d ${work_space}/libffi/libffi-3.4.4 < "$PATCH"
        if [ $? -ne 0 ]; then
            echo "Apply libffi-3.4.4 patch $PATCH failed."
            exit 254
        fi
    else
        echo "Cannot find libffi-3.4.4 patch: $PATCH"
        exit 254
    fi
done
info "Patch install finish for libffi-3.4.4."
cd libffi-3.4.4
chmod u+x configure
export LDFLAGS="-Wl,-z,noexecstack,-z,relro,-z,now,-s" && \
export CFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIE -fstack-protector-all -fPIC -Wall" && \
./configure \
  --prefix="$(pwd)/../../tmp/$packageName" \
  --disable-docs \
  --enable-shared \
  --enable-portable-binary \
  --disable-static || {
  warn "Building $packageFQDN failed during autogen"
  exit 254
}
sed -i '/#ifdef FFI_EXEC_STATIC_TRAMP/a extern int open_temp_exec_file(void);' ./src/tramp.c
make -s -j || {
  warn "Building $packageFQDN failed during make"
  exit 254
}
make install -j || {
  warn "Building $packageFQDN failed during make install"
  exit 254
}
cd ..
info "Build $packageFQDN done."

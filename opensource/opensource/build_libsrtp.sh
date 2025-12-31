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
info() { echo -e "\033[1;34m[INFO ][Depend  ] $1\033[1;37m"; }
warn() { echo >&2 -e "\033[1;31m[WARN ][Depend  ] $1\033[1;37m"; }

# save current path
CUR_PATH=$(
    cd "$(dirname "$0")" || {
        warn "Failed to check path/to/build.sh"
        exit
    }
    pwd
)

# Build
fileName="libsrtp"
packageFQDN="libsrtp@v2.6.0"
packageName="libsrtp"
cd "$fileName" || {
    warn "cd to ./opensource/${fileName} failed"
    exit 254
}
if [ -d "build" ]; then
    rm -r build || {
        warn "Remove old build directory failed"
        exit 254
    }
fi

export LDFLAGS="-Wl,-z,noexecstack,-z,relro,-z,now,-s"
export CFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIE -fstack-protector-all -fPIC -Wall"
./configure --prefix="$(pwd)/../tmp/libsrtp" --disable-docs --enable-static || {
    warn "Building $packageFQDN failed during cmake"
    exit 254
}

make -j || {
    warn "Building $packageFQDN failed during make"
    exit 254
}
make install || {
    warn "Building $packageFQDN failed during make install"
    exit 254
}

info "Build $packageFQDN done."

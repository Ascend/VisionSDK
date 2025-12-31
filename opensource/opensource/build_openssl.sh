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

# Build
fileName="openssl"
packageFQDN="openssl@1.1.1k"
packageName="openssl"
gcc485="4.8.5"
gccVersion=$(gcc --version | grep gcc | sed 's/.* //g')
cd "$fileName" || {
  warn "cd to ./opensource/$fileName failed"
  exit 254
}
info "Building dependency $packageFQDN."
chmod 777 config && dos2unix config
export LDFLAGS="-Wl,-z,relro,-z,now -s"
if [[ $gccVersion == *$gcc485* ]]; then
  export CFLAGS="-fPIE -fstack-protector-all -fPIC -Wall -std=gnu99"
else
  export CFLAGS="-fPIE -fstack-protector-all -fPIC -Wall"
fi

./config shared enable-ssl3 enable-ssl3-method no-afalgeng \
  --prefix="$(pwd)/../tmp/$packageName" \
  --openssldir="$(pwd)/../tmp/$packageName" \
  --libdir=lib || {
  warn "Building $packageFQDN failed during config"
  exit 254
}
make -s -j12 || {
  warn "Building $packageFQDN failed during make"
  exit 254
}
make install_dev -sj || {
  warn "Building $packageFQDN failed during make install"
  exit 254
}
# rm unused file, with scan safe compile problem
if [[ -f "../tmp/openssl/lib/libssl.a" ]];then
  rm "../tmp/openssl/lib/libssl.a"
fi
if [[ -f "../tmp/openssl/lib/libcrypto.a" ]];then
  rm "../tmp/openssl/lib/libcrypto.a"
fi
cd ..
info "Build $packageFQDN done."

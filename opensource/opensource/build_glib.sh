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
#build pcre2
cd ${work_space}/pcre2
mkdir -p ${work_space}/tmp/pcre2
export CFLAGS="-std=gnu99 -fPIC"
tar -xvjf pcre2-10.42.tar.bz2
info "Patch install start for pcre2."
SPEC_FILE="pcre2.spec"
PATCH_FILES=$(grep -oP '^\s*Patch\d+\s*:\s*\K.*' "$SPEC_FILE")
for PATCH in $PATCH_FILES; do
    if [ -f "$PATCH" ]; then
        patch -p1 -d ${work_space}/pcre2/pcre2-10.42 < "$PATCH"
        if [ $? -ne 0 ]; then
            echo "Apply pcre2 patch $PATCH failed."
            exit 254
        fi
    else
        echo "Cannot find pcre2 patch: $PATCH"
        exit 254
    fi
done
info "Patch install finish for pcre2."
cd pcre2-10.42
./configure --prefix=${work_space}/tmp/pcre2
make
make install
cd ${work_space}/

# Build
tar -xvf ${work_space}/glib/glib-2.78.3.tar.xz
cp -R ${work_space}/glib-2.78.3/* ${work_space}/glib/

fileName="glib"
packageFQDN="glib@2.78.3"
packageName="glib"
addPieArray=(gobject-query gio-launch-desktop glib-compile-schemas glib-compile-resources gdbus gtester gio-querymodules)
addPieArrayGio=(gresource gapplication gio gsettings)
addPieArrayGioTests=(gsettings gapplication)
arch=$(uname -m)
normalCompileText="-Wl,-z,relro,-z,now -s"
pieCompileText="-Wl,-z,relro,-z,now -s -pie -D_GLIBCXX_USE_CXX11_ABI=0"
cd "$fileName" || {
  warn "cd to ./opensource/$fileName failed"
  exit 254
}

info "Patch install start for glib2."
SPEC_FILE="glib2.spec"
PATCH_FILES=$(grep -oP '^\s*(?i)Patch\d+\s*:\s*\K.*' "$SPEC_FILE")
for PATCH in $PATCH_FILES; do
    if [ -f "$PATCH" ]; then
        patch -p1 -d ${work_space}/glib/ < "$PATCH"
        if [ $? -ne 0 ]; then
            echo "Apply glib2 patch $PATCH failed."
            exit 254
        fi
    else
        echo "Cannot find glib2 patch: $PATCH"
        exit 254
    fi
done
info "Patch install finish for glib2."

# need to update when change version to static link pcre2
sed -i "/pcre2 = dependency('libpcre2-8', version: pcre2_req, required: false, default_options: \['default_library=static'\])/a pcre2 = cc.find_library('libpcre2-8', dirs: '${work_space}/tmp/pcre2/lib', required: false, static: true)\\nuse_pcre2_static_flag = true" ${work_space}/glib/meson.build
if [[ -d "./build" ]]; then
  rm -r ./build
fi
info "Building dependency $packageFQDN."
export LD_LIBRARY_PATH="$(pwd)/../tmp/libffi/lib64:$LD_LIBRARY_PATH"
PKG_CONFIG_PATH="$(pwd)/../tmp/libffi/lib/pkgconfig:$(pwd)/../tmp/pcre2/lib/pkgconfig" \
LDFLAGS="-Wl,-z,relro,-z,now -s -L${work_space}/tmp/pcre2/lib -L$(pwd)/../tmp/libffi/lib64" CFLAGS="-D_GLIBCXX_USE_CXX11_ABI=0 -fPIE -fstack-protector-all -fPIC -Wall -I${work_space}/tmp/pcre2/include" \
meson --strip \
  --prefix "$(pwd)/../tmp/$packageName" \
  --libdir lib \
  --optimization s \
  -Dlibmount=disabled \
  -Dnls=disabled \
  -Dselinux=disabled \
  build || {
  warn "Building $packageFQDN failed during meson build"
  exit 254
}
buildFile=./build/build.ninja
sed -i "s/-Wl,-rpath-link,\/usr\/lib\/aarch64-linux-gnu//g" $buildFile
sed -i "s/-Wl,-rpath-link,\/usr\/lib\/x86_64-linux-gnu//g" $buildFile
if [[ -f "$buildFile" ]]; then
  echo "begin to add pie."
  for name in ${addPieArray[@]}
  do
    searchText="/$name: c_LINKER/="
    lineNum=$(sed -n "$searchText" $buildFile)
    if [[ -z "$lineNum" ]]; then
      echo "glib failed to find $searchText."
    else
      let modLines=$lineNum+1
      sed -i "${modLines}s/${normalCompileText}/${pieCompileText}/" $buildFile
      sed -n "${modLines}p" $buildFile
    fi
  done
  for name in ${addPieArrayGio[@]}
  do
    searchText="/gio\/$name: c_LINKER/="
    lineNum=$(sed -n "$searchText" $buildFile)
    if [[ -z "$lineNum" ]]; then
      echo "glib failed to find $searchText."
    else
      let modLines=$lineNum+1
      sed -i "${modLines}s/${normalCompileText}/${pieCompileText}/" $buildFile
      sed -n "${modLines}p" $buildFile
    fi
  done
  for name in ${addPieArrayGioTests[@]}
  do
    searchText="/gio\/tests\/$name: c_LINKER/="
    lineNum=$(sed -n "$searchText" $buildFile)
    if [[ -z "$lineNum" ]]; then
      echo "glib failed to find $searchText."
    else
      let modLines=$lineNum+1
      sed -i "${modLines}s/${normalCompileText}/${pieCompileText}/" $buildFile
      sed -n "${modLines}p" $buildFile
    fi
  done
  echo "end to add pie."
fi
ninja -C build || {
  warn "Building $packageFQDN failed during ninja build"
  exit 254
}
ninja -C build install || {
  warn "Building $packageFQDN failed during ninja build install"
  exit 254
}
modifyFile="$(pwd)/../tmp/glib/bin/glib-mkenums"
if [[ -f "${modifyFile}" ]];then
  dos2unix "${modifyFile}"
  chmod +x "${modifyFile}"
  echo "Modify chmod and dos2unix success."
else
  echo "Modify file(${modifyFile})\`s chmod and dos2unix failed."
fi
cd ..
info "Build $packageFQDN done."

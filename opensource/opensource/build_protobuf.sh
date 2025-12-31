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
# except cpp python
removeLanguages="csharp java js objectivec php ruby"
removeLiteralName="C# Java JavaScript Objective objc PHP Ruby"
cd ${work_space}/
#build abseil-cpp
cd ${work_space}/abseil-cpp
tar -zxf abseil-cpp-20230802.1.tar.gz
info "Patch install start for abseil-cpp."
SPEC_FILE="abseil-cpp.spec"
PATCH_FILES=$(grep -oP '^\s*Patch\d+\s*:\s*\K.*' "$SPEC_FILE")
for PATCH in $PATCH_FILES; do
    if [ -f "$PATCH" ]; then
        patch -p1 -d ${work_space}/abseil-cpp/abseil-cpp-20230802.1 < "$PATCH"
        if [ $? -ne 0 ]; then
            echo "Apply abseil-cpp patch $PATCH failed."
            exit 254
        fi
    else
        echo "Cannot find abseil-cpp patch: $PATCH"
        exit 254
    fi
done
info "Patch install finish for abseil-cpp."
cd ${work_space}/

# Build protobuf
fileName="protobuf"
packageFQDN="google/protobuf@4.25.2"
packageName="protobuf"

cd "$fileName" || {
  warn "cd to ./opensource/${fileName} failed"
  exit 254
}

info "Building dependency: $packageFQDN"
tar -zxf protobuf-all-25.1.tar.gz
info "Patch install start for protobuf."
SPEC_FILE="protobuf.spec"
PATCH_FILES=$(grep -oP '^\s*Patch\d+\s*:\s*\K.*' "$SPEC_FILE")
for PATCH in $PATCH_FILES; do
    if [ -f "$PATCH" ]; then
        patch -p1 -d ${work_space}/protobuf/protobuf-25.1 < "$PATCH"
        if [ $? -ne 0 ]; then
            echo "Apply protobuf patch $PATCH failed."
            exit 254
        fi
    else
        echo "Cannot find protobuf patch: $PATCH"
        exit 254
    fi
done
info "Patch install finish for protobuf."
cd protobuf-25.1
if [[ -d "./build" ]];then
  rm -rf "./build"
fi
mkdir build
cp -r ${work_space}/abseil-cpp/abseil-cpp-20230802.1/* ./third_party/abseil-cpp
mapFileList=("./src/libprotobuf.map" "./src/libprotoc.map" "./src/libprotobuf-lite.map")
for file in "${mapFileList[@]}"; do
  if [[ -f "$file" ]]; then
    sed -i 's/\*google\*;/\*google\*;\*mindxsdk_private\*;/g' "$file"
    echo "File $file process."
  else
    echo "File $file not found, skipping."
  fi
done
cd build || {
  warn "cd to ./opensource/${fileName}/build failed"
  exit 254
}
arch=$(uname -m)
export CPPFLAGS="-I/opt/rh/devtoolset-7/root/usr/include -std=c++14 -fPIE -fstack-protector-all -fPIC -Wall" && \
export LDFLAGS="-Wl,-z,relro,-z,now,-z,noexecstack -s -pie -lpthread" && \
cmake \
  -Dprotobuf_BUILD_EXAMPLES=OFF \
  -Dprotobuf_BUILD_TESTS=OFF \
  -Dprotobuf_BUILD_SHARED_LIBS=ON \
  -Dprotobuf_BUILD_PROTOC_BINARIES=ON \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_SKIP_RPATH=TRUE \
  -DCMAKE_HAVE_THREADS_LIBRARY=1 \
  -DCMAKE_USE_WIN32_THREADS_INIT=0 \
  -DTHREADS_PREFER_PTHREAD_FLAG=ON \
  -DCMAKE_CXX_STANDARD=14 \
  -DCMAKE_C_COMPILER=/opt/rh/devtoolset-7/root/usr/bin/gcc \
  -DCMAKE_CXX_COMPILER=/opt/rh/devtoolset-7/root/usr/bin/g++ \
  -DCMAKE_CXX_FLAGS="-Dgoogle=mindxsdk_private -D_GLIBCXX_USE_CXX11_ABI=0 -std=c++14 -fPIE -fstack-protector-all -fPIC -Wall" \
  -DCMAKE_SHARED_LINKER_FLAGS="-Wl,-z,relro,-z,now,-z,noexecstack -s -pie -lpthread" \
  -DCMAKE_EXE_LINKER_FLAGS="-Wl,-z,relro,-z,now,-z,noexecstack -s -pie -lpthread" \
  -DLIB_PREFIX="mindxsdk_" \
  -DCMAKE_INSTALL_PREFIX="${work_space}/tmp/$packageName" \
  -DCMAKE_INSTALL_LIBDIR="lib" \
  -G "Unix Makefiles" .. || {
  warn "Building $packageFQDN failed during cmake"
  exit 254
}
make -s -j || {
  warn "Building $packageFQDN failed during make"
  exit 254
}
make install/strip -j || {
  warn "Building $packageFQDN failed during make install"
  exit 254
}
find  ${work_space} -type f -name "libutf8_*.a" -exec rm -f {} \;
: << COMMENT
DIST_LANG=cpp ./autogen.sh || {
  warn "Building $packageFQDN failed during autogen"
  exit 254
}
DIST_LANG=cpp ./configure \
  --prefix="$(pwd)/../$packageName" \
  --enable-static=no \
  CXXFLAGS="-s -DNDEBUG" || {
  warn "Building $packageFQDN failed during configure"
  exit 254
}
make dist -j4 || {
  warn "Building $packageFQDN failed during make"
  exit 254
}
cp ./protobuf-3.9.0.tar.gz ../
cd ..
rm -rf protobuf-3.9.0

# The second build
info "Extracting ${fileName} (second time)"
tar zxvf "${fileName}.tar.gz" > /dev/null || {
  warn "tar zxvf failed"
  warn "Previous build for cpp may be failed"
  exit 254
}
cd "${fileName}" || {
  warn "cd to ./opensource/${fileName} failed"
  exit 254
} # in protobuf-3.9.0
info "Building dependency: $packageFQDN. (second time: real build)"
DIST_LANG=cpp ./configure \
  --prefix="$(pwd)/../$packageName" \
  --enable-static=no \
  CXXFLAGS="-s -DNDEBUG" || {
  warn "Building $packageFQDN failed during configure"
  exit 254
}
make -j4 || {
  warn "Building $packageFQDN failed during make"
  exit 254
}
make install -j || {
  warn "Building $packageFQDN failed during make install"
  exit 254
}
COMMENT
cd ../..
info "Build $packageFQDN done."

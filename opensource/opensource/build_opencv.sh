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
fileName="opencv"
packageFQDN="opencv/opencv@4.11.0"
packageName="opencv"
cd "$fileName" || {
  warn "cd to ./opensource/${fileName} failed"
  exit 254
} # in opencv-4.11.0
info "Building dependency: $packageFQDN."
if [[ -d "_build" ]];then
  rm -r _build
fi
sed -i '/ocv_set_platform_flags(platform_flags)/a unset(platform_flags)' cmake/OpenCVCompilerOptions.cmake
mkdir _build
cd _build || {
  warn "OpenCV needs a separate directory for building"
  exit 253
} # in opencv-4.11.0/_build
if [[ $(uname -m) == aarch64 ]]; then
  opencv_aarch64_cmake_arg="-DCMAKE_CXX_FLAGS="'"'"-march=armv8-a"'"'
fi

# rm host status info
make_file=../CMakeLists.txt
text="status(\"    Host:\"             \${CMAKE_HOST_SYSTEM_NAME} \${CMAKE_HOST_SYSTEM_VERSION} \${CMAKE_HOST_SYSTEM_PROCESSOR})"
if [[ -f $make_file ]];then
  sed -i "/$text/d" $make_file
  if [ $? != 0 ]; then
    echo "change host status info failed."
  fi
fi
export LD_LIBRARY_PATH=$(pwd)/../../tmp/gstreamer/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$(pwd)/../../tmp/libffi/lib64:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$(pwd)/../../tmp/libffi/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$(pwd)/../../tmp/glib/lib:$LD_LIBRARY_PATH
export LDFLAGS="-Wl,--build-id=none -Wl,-z,relro,-z,now -s -pie" && \
PKG_CONFIG_PATH="$(pwd)/../../tmp/glib/lib/pkgconfig:$(pwd)/../../tmp/gstreamer/lib/pkgconfig" \
cmake \
  $opencv_aarch64_cmake_arg \
  -DCMAKE_INSTALL_PREFIX="$(pwd)/../../tmp/$packageName" \
  -DCMAKE_INSTALL_LIBDIR="$(pwd)/../../tmp/$packageName/lib" \
  -DCMAKE_SKIP_RPATH=TRUE \
  -DBUILD_INFO_SKIP_TIMESTAMP=ON \
  -DCMAKE_CXX_FLAGS_RELEASE="-O2 -DNDEBUG -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=0 -fPIE -fstack-protector-all -fPIC -Wall" \
  -DCMAKE_C_FLAGS_RELEASE="-O2 -DNDEBUG -fPIE -fstack-protector-all -fPIC -Wall" \
  -DGSTREAMER_DIR="$(pwd)/../../tmp/gstreamer" \
  -DPROTOBUF_DIR="$(pwd)/../../tmp/protobuf" \
  -DBUILD_opencv_world=ON \
  -DBUILD_ZLIB=ON \
  -DWITH_TIFF=OFF \
  -DWITH_ITT=OFF \
  -DBUILD_PERF_TESTS=OFF \
  -DBUILD_SHARED_LIBS=ON \
  -DBUILD_TESTS=OFF \
  -DWITH_ADE=OFF \
  -DWITH_GSTREAMER=OFF \
  -DWITH_QUIRC=OFF \
  -DWITH_IPP=OFF \
  -DWITH_LAPACK=OFF \
  -DWITH_WEBP=OFF \
  -DENABLE_NEON=OFF \
  -DCPU_NEON_SUPPORTED=OFF \
  -DCPU_BASELINE_REQUIRE=";" \
  -DCPU_BASELINE_DISABLE="NEON" \
  -DENABLE_LIBJPEG_TURBO_SIMD=0 \
  -DPNG_ARM_NEON="off" \
  -DCV_DISABLE_OPTIMIZATION="ON" \
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

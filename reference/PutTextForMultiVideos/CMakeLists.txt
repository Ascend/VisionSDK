#
# Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
# Description: CMakeLists for this project.
# Author: Vision SDK
# Create: 2024
# History: NA
#

cmake_minimum_required(VERSION 3.5.2)
project(PutTextForMultiVideos)

set(MX_SDK_HOME $ENV{MX_SDK_HOME})
set(FFMPEG_PATH $ENV{FFMPEG_PATH})
set(ASCEND_HOME_PATH $ENV{ASCEND_TOOLKIT_HOME})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--copy-dt-needed-entries")

# Add compile options
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_definitions(-Dgoogle=mindxsdk_private)
add_compile_options(-std=c++14 -fPIC -fstack-protector-all -Wall -g -O0)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro,-z,now,-z,noexecstack -pie")

# Add header path
include_directories(
        ${MX_SDK_HOME}/include
        ${MX_SDK_HOME}/opensource/include
        ${MX_SDK_HOME}/opensource/include/opencv4
        ${FFMPEG_PATH}/include
        ${ASCEND_HOME_PATH}/include
)

# Add lib path
link_directories(
        ${MX_SDK_HOME}/lib
        ${MX_SDK_HOME}/opensource/lib
        ${MX_SDK_HOME}/opensource/lib64
        ${FFMPEG_PATH}/lib
        ${ASCEND_HOME_PATH}/lib64
)

add_executable(main main.cpp ConfigParser/ConfigParser.h ConfigParser/ConfigParser.cpp
               PutText/CaptionGenManager.h PutText/CaptionGenManager.cpp PutText/CaptionGeneration.cpp
               PutText/CaptionGeneration.h PutText/CaptionImpl.cpp PutText/CaptionImpl.h PutText/CaptionPool.h PutText/CaptionPool.cpp)

target_link_libraries(main mxbase -lpthread -lm avformat avcodec avdevice avutil swresample)

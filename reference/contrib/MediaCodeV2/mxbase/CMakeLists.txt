cmake_minimum_required(VERSION 3.10)
project(mediacodecV2)
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(TARGET mediacodecV2)
add_compile_options(-std=c++14 -fPIE -fstack-protector-all )
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0 -Dgoogle=mindxsdk_private)

include_directories(
        $ENV{MX_SDK_HOME}/include
        $ENV{MX_SDK_HOME}/opensource/include
        $ENV{MX_SDK_HOME}/opensource/include/opencv4
        $ENV{FFMPEG_PATH}/include
)

link_directories(
        $ENV{MX_SDK_HOME}/lib
        $ENV{MX_SDK_HOME}/opensource/lib
        $ENV{MX_SDK_HOME}/lib/modelpostprocessors
        $ENV{ASCEND_TOOLKIT_HOME}/lib64
        $ENV{ASCEND_HOME}/driver/lib64/
        $ENV{FFMPEG_PATH}/lib
)


add_executable(mediacodecV2 mainV2.cpp MediaCodecV2.cpp)
target_link_libraries(mediacodecV2
        glog
        mxbase
        opencv_world
        pthread
        avcodec
        avdevice
        avfilter
        avformat
        avutil
        swresample
        swscale
        )

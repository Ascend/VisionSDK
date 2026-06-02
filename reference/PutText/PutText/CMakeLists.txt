cmake_minimum_required(VERSION 3.0)
project(PutTextDemo)
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

add_definitions(-Dgoogle=mindxsdk_private)
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--copy-dt-needed-entries")
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
include_directories(
        $ENV{MX_SDK_HOME}/include
        $ENV{MX_SDK_HOME}/opensource/include
        $ENV{MX_SDK_HOME}/opensource/include/opencv4
)

link_directories(
        $ENV{MX_SDK_HOME}/lib
        $ENV{MX_SDK_HOME}/include
        $ENV{MX_SDK_HOME}/opensource/lib
        $ENV{MX_SDK_HOME}/lib/modelpostprocessors
)

set(CMAKE_CXX_STANDARD 17)

add_executable(PutTextDemo main.cpp CaptionGenManager.h CaptionGenManager.cpp CaptionGeneration.cpp
               CaptionGeneration.h CaptionImpl.cpp CaptionImpl.h CaptionPool.cpp CaptionPool.h)

target_link_libraries(
        PutTextDemo
        mxbase
        glog
)

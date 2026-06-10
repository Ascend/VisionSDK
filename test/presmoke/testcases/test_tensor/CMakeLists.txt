cmake_minimum_required(VERSION 3.5.2)

# project information
project(Vision_SDK_Test_Tensor)

set(MX_SDK_HOME $ENV{MX_SDK_HOME})
message(${MX_SDK_HOME})
set(ASCEND_TOOLKIT_HOME $ENV{ASCEND_TOOLKIT_HOME})

set(PROJECT_SRC_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_MODULE_PATH ${PROJECT_SRC_ROOT}/CMake)
set(CMAKE_SKIP_BUILD_RPATH True)

if (NOT DEFINED ENV{MX_SDK_HOME})
    string(REGEX REPLACE "(.*)/(.*)/(.*)/(.*)" "\\1" MX_SDK_HOME  ${CMAKE_CURRENT_SOURCE_DIR})
    message(STATUS "set default MX_SDK_HOME: ${MX_SDK_HOME}")
else ()
    message(STATUS "env MX_SDK_HOME: ${MX_SDK_HOME}")
endif()

# Compile options
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_definitions(-Dgoogle=mindxsdk_private)
add_compile_options(-fPIC -fstack-protector-all -Wall -O0)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro,-z,now,-z,noexecstack -pie ")
set(CMAKE_SKIP_RPATH TRUE)

include_directories(
        ${ASCEND_TOOLKIT_HOME}/include/
        ${MX_SDK_HOME}/include/
        ${MX_SDK_HOME}/include/MxBase/postprocess/include/
        ${MX_SDK_HOME}/opensource/include/
        ${MX_SDK_HOME}/opensource/include/opencv4

)

set(OPENSOURCE_DIR ${MX_SDK_HOME}/opensource)

include_directories(
        ${OPENSOURCE_DIR}/include
)
link_directories(${OPENSOURCE_DIR}/lib)

# add host lib path
link_directories(
        ${ASCEND_TOOLKIT_HOME}/lib64/
        ${MX_SDK_HOME}/lib/
        ${MX_SDK_HOME}/lib/modelpostprocessors/
        ${MX_SDK_HOME}/opensource/lib/
        ${MX_SDK_HOME}/opensource/lib64/

)

add_executable(test_tensor_ops test_tensor_ops.cpp)
target_link_libraries(test_tensor_ops glog mxbase ctpnpostprocess -lpthread  opencv_world ascendcl)
install(TARGETS test_tensor_ops DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

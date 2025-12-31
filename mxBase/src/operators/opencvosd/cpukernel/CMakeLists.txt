# CMake minimum version number requirements
cmake_minimum_required(VERSION 3.14.1)
project(opencvosd)

if(COVERAGE)
    get_directory_property(CUR_OPTS COMPILE_OPTIONS)
    list(REMOVE_ITEM CUR_OPTS -fprofile-arcs -ftest-coverage -O2)
    set_directory_properties(PROPERTIES COMPILE_OPTIONS "${CUR_OPTS}")
endif()

# Check opensource-device package whether exists
if(NOT DEFINED OPENSOURCE_DEV_DIR)
    message(FATAL_ERROR "please define variable:OPENSOURCE_DEV_DIR")
endif()

if(NOT EXISTS ${OPENSOURCE_DEV_DIR})
    message(FATAL_ERROR "OPENSOURCE_DEV_DIR: ${OPENSOURCE_DEV_DIR} not exists, please install it first.")
endif()

message(STATUS "OPENSOURCE_DEV_DIR=${OPENSOURCE_DEV_DIR}")
include_directories(${OPENSOURCE_DEV_DIR}/include/opencv4)
link_directories(${OPENSOURCE_DEV_DIR}/lib)
link_directories(${OPENSOURCE_DEV_DIR}/lib/opencv4/3rdparty/)

if(DEFINED ENV{ASCEND_HOME})
    set(ASCEND_HOME $ENV{ASCEND_HOME})
else()
    set(ASCEND_HOME /usr/local/Ascend)
    message(WARNING "ASCEND_HOME is not set, use default value ${ASCEND_HOME}")
endif()

if(DEFINED ENV{ASCEND_VERSION})
    set(ASCEND_VERSION $ENV{ASCEND_VERSION})
else()
    set(ASCEND_VERSION ascend-toolkit/latest)
    message(WARNING "ASCEND_VERSION is not set, use default value ${ASCEND_VERSION}")
endif()

set(ASCEND_OPP_PATH "${ASCEND_HOME}/${ASCEND_VERSION}/opp")
include_directories(${ASCEND_OPP_PATH}/built-in/op_impl/aicpu/aicpu_kernel/inc)
include_directories(${ASCEND_OPP_PATH}/op_impl/built-in/aicpu/aicpu_kernel/inc)

# Specify cross compiler
set(TOOLCHAIN_DIR "${ASCEND_HOME}/${ASCEND_VERSION}/toolkit/toolchain/hcc")
if(NOT EXISTS ${TOOLCHAIN_DIR})
    message(FATAL_ERROR "TOOLCHAIN_DIR: ${TOOLCHAIN_DIR} not exists, please install it first.")
endif()
message(STATUS "TOOLCHAIN_DIR=${TOOLCHAIN_DIR}")

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/aarch64-target-linux-gnu-g++)
set(CMAKE_C_COMPILER   ${TOOLCHAIN_DIR}/bin/aarch64-target-linux-gnu-gcc)

# Set library name
set(AICPU_KERNEL_TARGET cust_aicpu_kernels)
aux_source_directory(./impl/ KERNELS_SRCS)
add_library(${AICPU_KERNEL_TARGET} SHARED ${KERNELS_SRCS})

set(AICPU_SOC_VERSION Ascend310)
message(STATUS "AICPU_SOC_VERSION=${AICPU_SOC_VERSION}")
if(EXISTS "${ASCEND_OPP_PATH}/built-in/op_impl/aicpu/aicpu_kernel/lib/${AICPU_SOC_VERSION}/libascend_protobuf.a")
    target_link_libraries(${AICPU_KERNEL_TARGET} PRIVATE
            -Wl,--whole-archive
            ${ASCEND_OPP_PATH}/built-in/op_impl/aicpu/aicpu_kernel/lib/${AICPU_SOC_VERSION}/libascend_protobuf.a
            -Wl,--no-whole-archive
            -s
            -Wl,-Bsymbolic
            -Wl,--exclude-libs=libascend_protobuf.a
            )
endif()

if(EXISTS "${ASCEND_OPP_PATH}/op_impl/built-in/aicpu/aicpu_kernel/lib/${AICPU_SOC_VERSION}/libascend_protobuf.a")
    target_link_libraries(${AICPU_KERNEL_TARGET} PRIVATE
            -Wl,--whole-archive
            ${ASCEND_OPP_PATH}/op_impl/built-in/aicpu/aicpu_kernel/lib/${AICPU_SOC_VERSION}/libascend_protobuf.a
            -Wl,--no-whole-archive
            -s
            -Wl,-Bsymbolic
            -Wl,--exclude-libs=libascend_protobuf.a
            )
endif()

if(EXISTS "${ASCEND_OPP_PATH}/built-in/op_impl/aicpu/aicpu_kernel/lib/${AICPU_SOC_VERSION}/libcpu_kernels_context.a")
    target_link_libraries(${AICPU_KERNEL_TARGET} PRIVATE
        -Wl,--whole-archive
        ${ASCEND_OPP_PATH}/built-in/op_impl/aicpu/aicpu_kernel/lib/${AICPU_SOC_VERSION}/libcpu_kernels_context.a
        -Wl,--no-whole-archive
    )
else()
    if(EXISTS "${ASCEND_OPP_PATH}/built-in/op_impl/aicpu/aicpu_kernel/lib/libcpu_kernels_context.a")
        target_link_libraries(${AICPU_KERNEL_TARGET} PRIVATE
            -Wl,--whole-archive
            ${ASCEND_OPP_PATH}/built-in/op_impl/aicpu/aicpu_kernel/lib/libcpu_kernels_context.a
            -Wl,--no-whole-archive
        )
    endif()
endif()

if(EXISTS "${ASCEND_OPP_PATH}/op_impl/built-in/aicpu/aicpu_kernel/lib/${AICPU_SOC_VERSION}/libcpu_kernels_context.a")
    target_link_libraries(${AICPU_KERNEL_TARGET} PRIVATE
        -Wl,--whole-archive
        ${ASCEND_OPP_PATH}/op_impl/built-in/aicpu/aicpu_kernel/lib/${AICPU_SOC_VERSION}/libcpu_kernels_context.a
        -Wl,--no-whole-archive
    )
else()
    if(EXISTS "${ASCEND_OPP_PATH}/op_impl/built-in/aicpu/aicpu_kernel/lib/libcpu_kernels_context.a")
        target_link_libraries(${AICPU_KERNEL_TARGET} PRIVATE
            -Wl,--whole-archive
            ${ASCEND_OPP_PATH}/op_impl/built-in/aicpu/aicpu_kernel/lib/libcpu_kernels_context.a
            -Wl,--no-whole-archive
        )
    endif()
endif()

target_link_libraries(${AICPU_KERNEL_TARGET} PRIVATE
    -Wl,--whole-archive
    libopencv_world.a liblibjpeg-turbo.a liblibpng.a liblibopenjp2.a libzlib.a
    -Wl,--no-whole-archive
)

install(TARGETS ${AICPU_KERNEL_TARGET} DESTINATION ${CMAKE_INSTALL_PREFIX}/operators/opencvosd/cust_aicpu/aicpu_kernel/custom_impl/)
install(FILES ${PROJECT_SOURCE_DIR}/op_info_cfg/aicpu_kernel/cust_aicpu_kernel.json DESTINATION ${CMAKE_INSTALL_PREFIX}/operators/opencvosd/cust_aicpu/config)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/../config DESTINATION ${CMAKE_INSTALL_PREFIX}/operators/opencvosd/)
install(FILES ${PROJECT_SOURCE_DIR}/script/generate_osd_om.sh DESTINATION ${CMAKE_INSTALL_PREFIX}/operators/opencvosd/)
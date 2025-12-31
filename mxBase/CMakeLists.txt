cmake_minimum_required(VERSION 3.14.1)
project(MXBASE
        LANGUAGES CXX C
        VERSION 1.0.0
)
option(COVERAGE "enable code coverage" OFF)

# compile cache acceleration
find_program(CCACHE_FOUND ccache)
if (CCACHE_FOUND)
    message("CCACHE FOUNDED")
    set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
endif (CCACHE_FOUND)

get_filename_component(MXBASE_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
set(CMAKE_CXX_STANDARD 14)
add_compile_options(-std=c++14 -fPIE -fstack-protector-all -fPIC -Wall -Wfloat-equal -pipe -fno-common -Wextra)
set(CMAKE_CXX_FLAGS "-Wno-error=deprecated-declarations -Wno-deprecated-declarations -Wno-error=pointer-arith -Wno-pointer-arith -fstack-protector-all")

# test settings
if (HIGH_TESTS)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE hitestwrapper)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK hitestwrapper)
endif ()

if (BUILD_TESTS)
    # Add compile option -g when build test
    add_compile_options(-g -O2)
    # Remove link option -s when build test
    add_link_options(-Wl,-z,relro,-z,now,-z,noexecstack -pie)
    # Add mockcpp when build test, if not exists, need install
    include_directories(/usr/local/mockcpp/include)
    include_directories(/usr/local/include/)
    link_directories(/usr/local/mockcpp/lib)
    link_directories(/usr/local/lib)
else ()
    add_link_options(-Wl,-z,relro,-z,now,-z,noexecstack -s -pie)
    add_compile_options(-O2)
    if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
        message("MXBASE_ROOT_PATH: ${MXBASE_ROOT_PATH}")
        add_link_options(-Wl,-T${MXBASE_ROOT_PATH}/build/ld.lds)
    endif ()
endif ()

add_compile_options(-Wno-deprecated-declarations)
add_definitions(-DENABLE_DVPP_INTERFACE)
add_definitions(-DBITWIDE=64)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
# Set namespace of glog from google to mindxsdk_private
add_definitions(-Dgoogle=mindxsdk_private)

# self include
include_directories(src/mxbase/include)
include_directories(src/mxbase/module)
include_directories(src/postprocess/include)
include_directories(src/python/include)

# ascendc operators custom dependency
include_directories(src/operators/ascendc/vendors/customize/op_api/include)
link_directories(src/operators/ascendc/vendors/customize/op_api/lib)

# opensource dependency
set(OPENSOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../opensource/output/${FRAME_PROJECT}/opensource)
set(OPENSOURCE_DEV_DIR ${OPENSOURCE_DIR}/../opensource-device)
include_directories(${OPENSOURCE_DIR}/include)
include_directories(${OPENSOURCE_DIR}/include/opencv4)
link_directories(${OPENSOURCE_DIR}/lib)
link_directories(${OPENSOURCE_DIR}/lib64)

# ascend dependency
set(ASCEND_HOME /usr/local/Ascend)
set(ASCEND_VERSION ascend-toolkit/latest)
set(ASCEND_LIB $ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/acllib)
include_directories(${ASCEND_LIB}/include)
include_directories($ENV{ASCEND_HOME_PATH}/include)
include_directories($ENV{ASCEND_HOME}/driver/include)
include_directories(/usr/local/Ascend/driver/include/)
set(MSL_ROOT /opt/buildtools/mindspore-lite-2.1.0/mindspore-lite-2.1.0-linux)
if(EXISTS "${MSL_ROOT}-x64/runtime")
    set(MSL_RUNTIME "${MSL_ROOT}-x64/runtime")
elseif(EXISTS "${MSL_ROOT}-aarch64/runtime")
    set(MSL_RUNTIME "${MSL_ROOT}-aarch64/runtime")
else()
    message(FATAL_ERROR "MindSpore-Lite runtime directory not found!")
endif()
include_directories("${MSL_RUNTIME}/include")
include_directories("${MSL_RUNTIME}")
link_directories("${MSL_RUNTIME}/lib")
include_directories($ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/$ENV{ARCH_PATTERN}/include)
include_directories($ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/opp/include)
link_directories($ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/opp/lib64)
link_directories($ENV{ASCEND_HOME_PATH}/lib64)
link_directories($ENV{ASCEND_HOME}/driver/lib64)
link_directories($ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/compiler/lib64)

# mslite dependency
set(MSLITE_RUNTIME_HOME $ENV{MSLITE_RUNTIME_HOME})
include_directories(${MSLITE_RUNTIME_HOME})
include_directories(${MSLITE_RUNTIME_HOME}/include)
link_directories(${MSLITE_RUNTIME_HOME}/lib)

set(MSLITE_TOOL_HOME $ENV{MSLITE_TOOL_HOME})
link_directories(${MSLITE_TOOL_HOME}/converter/lib)

# install path
if (DEFINED MXSDK_INSTALL_PATH)
    set(CMAKE_INSTALL_PREFIX ${MXSDK_INSTALL_PATH})
else ()
    set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/output/${FRAME_PROJECT}/mxBase)
endif ()

if (NOT EXISTS ${CMAKE_INSTALL_PREFIX})
    file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX})
    message("make install directory: ${CMAKE_INSTALL_PREFIX}")
endif ()

if (COVERAGE)
    add_compile_options(-fprofile-arcs -ftest-coverage)
    set(test_deps gcov)
endif ()

add_subdirectory(src)

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif ()

add_custom_target(mxbase-lcov
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_custom_command(TARGET mxbase-lcov
        COMMAND echo "========= LCOV html is creating ... ===="
        COMMAND lcov --rc lcov_excl_br_line=Log* --rc lcov_branch_coverage=1 -c -d \"${CMAKE_SOURCE_DIR}\" --filter branch --ignore-errors gcov --ignore-errors source -o all.info>/dev/null
        COMMAND lcov --rc lcov_branch_coverage=1 -e all.info \"${CMAKE_SOURCE_DIR}/*\" --filter branch --ignore-errors gcov --ignore-errors source -o project.info
        COMMAND lcov --rc lcov_branch_coverage=1 -r project.info \"${PROJECT_SOURCE_DIR}/test/*\" \"${OPENSOURCE_DIR}/*\" \"${CMAKE_SOURCE_DIR}/build_result/*\" \"${PROJECT_SOURCE_DIR}/src/mxbase/module/MbCV/Video/VideoEncoder/VideoEncoderCheck.h\" \"${PROJECT_SOURCE_DIR}src/mxbase/module/ResourceManager/DvppWrapper/DvppConfig/ParamCheckBase.h\" \"${PROJECT_SOURCE_DIR}/src/python/include/PyPostProcess/PyPostProcess.h\" \"${PROJECT_SOURCE_DIR}/src/python/include/PyVideoEncoder/PyVideoEncoder.h\" --filter branch --ignore-errors gcov --ignore-errors source --ignore-errors unused -o coverage.info
        COMMAND genhtml --rc genhtml_branch_coverage=1 coverage.info --filter branch --ignore-errors category -o coverage-html
        COMMAND echo "========= LCOV html is created successfully. ===="
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

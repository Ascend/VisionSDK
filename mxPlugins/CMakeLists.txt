cmake_minimum_required(VERSION 3.14.1)
project(MXPLUGINS
    LANGUAGES CXX C
    VERSION 1.0.0
)

option(COVERAGE "enable code coverage" OFF)
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    message("CCACHE FOUNDED")
    set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
endif(CCACHE_FOUND)
set(CMAKE_CXX_STANDARD 14) 
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_compile_options(-std=c++14 -fPIE -fstack-protector-all -fPIC -Wall -Wextra -Wfloat-equal -pipe -fno-common)
add_compile_options(-O2 -Wno-deprecated-declarations)
get_filename_component(MXPLUGINS_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)

add_definitions(-DENABLE_DVPP_INTERFACE)

if(HIGH_TESTS)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE hitestwrapper)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK hitestwrapper)
endif()

if(BUILD_TESTS)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed -Wl,--copy-dt-needed-entries")
    # Add compile option -g
    add_compile_options(-g)
    # Remove link option -s
    add_link_options(-Wl,-z,relro,-z,now,-z,noexecstack -pie)
    # Add mockcpp when build test, if not exists, need install
    include_directories(/usr/local/mockcpp/include)
    include_directories(/usr/local/include/)
    link_directories(/usr/local/mockcpp/lib)
    link_directories(/usr/local/lib)
else()
    add_link_options(-Wl,-z,relro,-z,now,-z,noexecstack -s -pie)
endif()
add_definitions(-DBITWIDE=64)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")
# Set namespace from google to mindxsdk_private
add_definitions(-Dgoogle=mindxsdk_private)
# Protobuf library has been renamed with prefix mindxsdk_
set(LIB_PREFIX "mindxsdk_")

# self dependency
include_directories(src/include)

# opensource dependency
set(OPENSOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../opensource/output/${FRAME_PROJECT}/opensource)
include_directories(${OPENSOURCE_DIR}/include)
include_directories(${OPENSOURCE_DIR}/include/opencv4)
include_directories(${OPENSOURCE_DIR}/include/gstreamer-1.0)
include_directories(${OPENSOURCE_DIR}/include/glib-2.0)
include_directories(${OPENSOURCE_DIR}/lib/glib-2.0/include)
link_directories(${OPENSOURCE_DIR}/lib)
link_directories(${OPENSOURCE_DIR}/lib64)

# mxbase dependency
set(MXBASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../mxBase/output/${FRAME_PROJECT}/)
include_directories(${MXBASE_DIR}/mxBase/include)
link_directories(${MXBASE_DIR}/mxBase/lib)

# mxTools dependency
set(MXTOOLS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../mxTools/output/${FRAME_PROJECT}/)
include_directories(${MXTOOLS_DIR}/mxTools/include)
link_directories(${MXTOOLS_DIR}/mxTools/lib)

# mxTools dependency
set(MXSTREAM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../mxStream/output/${FRAME_PROJECT}/)
include_directories(${MXSTREAM_DIR}/mxStream/include)
link_directories(${MXSTREAM_DIR}/mxStream/lib)

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

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/output/${FRAME_PROJECT}/mxPlugins)
if (NOT EXISTS ${CMAKE_INSTALL_PREFIX})
    file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX})
    message("make install directory: ${CMAKE_INSTALL_PREFIX}")
endif ()

add_subdirectory(src)

if(BUILD_TESTS)
    enable_testing()
    include_directories(test/gtest)
    add_subdirectory(test/gtest/hlt)
    add_subdirectory(test/gtest/llt)
endif()

add_custom_target(mxplugins-lcov
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_command(TARGET mxplugins-lcov
    COMMAND echo "========= LCOV html is creating ... ===="
    COMMAND lcov --rc lcov_excl_br_line=Log* --rc lcov_branch_coverage=1 -c -d \"${CMAKE_SOURCE_DIR}\" --filter branch --ignore-errors gcov --ignore-errors source -o all.info >/dev/null
    COMMAND lcov --rc lcov_branch_coverage=1 -e all.info \"${CMAKE_SOURCE_DIR}/*\" --filter branch --ignore-errors gcov --ignore-errors source -o project.info
    COMMAND lcov --rc lcov_branch_coverage=1 -r project.info \"${PROJECT_SOURCE_DIR}/test/*\" \"${OPENSOURCE_DIR}/*\" \"${MXBASE_DIR}/*\" \"${MXTOOLS_DIR}/*\" \"${MXSTREAM_DIR}/*\" --filter branch --ignore-errors gcov --ignore-errors source --ignore-errors unused -o coverage.info
    COMMAND genhtml --rc genhtml_branch_coverage=1 coverage.info --filter branch -o coverage-html
    COMMAND echo "========= LCOV html is created successfully. ===="
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

install(DIRECTORY src/include/MxPlugins/ModelPostProcessors/ModelPostProcessorBase DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MxPlugins/ModelPostProcessors)
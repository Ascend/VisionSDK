cmake_minimum_required(VERSION 3.14.1)
project(MXSTREAM
    LANGUAGES CXX C
    VERSION 1.0.0
)
cmake_policy(SET CMP0086 NEW)
cmake_policy(SET CMP0078 NEW)
option(COVERAGE "enable code coverage" OFF)
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    message("CCACHE FOUNDED")
    set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
endif(CCACHE_FOUND)
set(CMAKE_CXX_STANDARD 14) 
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

get_filename_component(MXSTREAM_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR} ABSOLUTE)
add_compile_options(-fPIE -fstack-protector-all -fPIC -Wall -Wextra -Wfloat-equal -pipe -fno-common)
add_compile_options(-Wno-deprecated-declarations)

if(HIGH_TESTS)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE hitestwrapper)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK hitestwrapper)
endif()

if (BUILD_TESTS)
    add_compile_options(-g)
    add_link_options(-Wl,-z,relro,-z,now,-z,noexecstack -pie)
    # Add mockcpp when build test, if not exists, need install
    include_directories(/usr/local/mockcpp/include)
    include_directories(/usr/local/include/)
    link_directories(/usr/local/mockcpp/lib)
    link_directories(/usr/local/lib)
else()
    add_compile_options(-O3)
    add_link_options(-Wl,-z,relro,-z,now,-z,noexecstack -s -pie)
endif()

# Set namespace from google to mindxsdk_private
add_definitions(-Dgoogle=mindxsdk_private)
# Protobuf library has been renamed with prefix mindxsdk_
set(LIB_PREFIX "mindxsdk_")
add_definitions(-DBITWIDE=64)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG")

# self dependency
include_directories(src/include)
include_directories(src/include/Internal)
include_directories(src/module/StreamManager)
include_directories(src/python)
include_directories(src/python/include)

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

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/output/${FRAME_PROJECT}/mxStream)
if (NOT EXISTS ${CMAKE_INSTALL_PREFIX})
    file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX})
    message("make install directory: ${CMAKE_INSTALL_PREFIX}")
endif ()

add_subdirectory(src)

if (BUILD_TESTS)
    set(MXSTREAM_TEST_COMMON_DEP_LIBS glog gtest ${LIB_PREFIX}protobuf streammanager)
    enable_testing()
    add_subdirectory(test)
endif()

add_custom_target(mxstream-lcov
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_command(TARGET mxstream-lcov
    COMMAND echo "========= LCOV html is creating ... ===="
    COMMAND lcov --rc lcov_branch_coverage=1 -c -d \"${CMAKE_SOURCE_DIR}\" --filter branch --ignore-errors gcov --ignore-errors source -o all.info >/dev/null
    COMMAND lcov --rc lcov_branch_coverage=1 -e all.info \"${CMAKE_SOURCE_DIR}/*\" --filter branch --ignore-errors gcov --ignore-errors source -o project.info
    COMMAND lcov --rc lcov_branch_coverage=1 -r project.info \"${PROJECT_SOURCE_DIR}/test/*\" \"${OPENSOURCE_DIR}/*\" \"${MXBASE_DIR}/*\" \"${MXTOOLS_DIR}/*\" --filter branch --ignore-errors gcov --ignore-errors source --ignore-errors unused -o coverage.info
    COMMAND genhtml --rc genhtml_branch_coverage=1 coverage.info --filter branch -o coverage-html
    COMMAND echo "========= LCOV html is created successfully. ===="
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

install(DIRECTORY src/include/MxStream DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(DIRECTORY samples/config DESTINATION ${CMAKE_INSTALL_PREFIX}/)
install(DIRECTORY samples/models DESTINATION ${CMAKE_INSTALL_PREFIX}/samples/mxManufacture)
install(DIRECTORY samples/models DESTINATION ${CMAKE_INSTALL_PREFIX}/samples/mxVision)
install(DIRECTORY samples/pipeline DESTINATION ${CMAKE_INSTALL_PREFIX}/samples/mxManufacture)
install(DIRECTORY samples/pipeline DESTINATION ${CMAKE_INSTALL_PREFIX}/samples/mxVision)
install(DIRECTORY samples/config DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY samples/streamserver DESTINATION ${CMAKE_INSTALL_PREFIX}/samples/mxManufacture)
install(DIRECTORY samples/streamserver DESTINATION ${CMAKE_INSTALL_PREFIX}/samples/mxVision)
install(DIRECTORY samples/C++ DESTINATION ${CMAKE_INSTALL_PREFIX}/samples/mxManufacture)
install(DIRECTORY samples/C++ DESTINATION ${CMAKE_INSTALL_PREFIX}/samples/mxVision)
install(DIRECTORY samples/python DESTINATION ${CMAKE_INSTALL_PREFIX}/samples/mxManufacture)
install(DIRECTORY samples/python DESTINATION ${CMAKE_INSTALL_PREFIX}/samples/mxVision)

cmake_minimum_required(VERSION 3.14.1)
project(
  MXSTREAM
  LANGUAGES CXX C
  VERSION 1.0.0)

option(COVERAGE "enable code coverage" OFF)
option(BUILD_TESTS "Build the tests" OFF)
set(CMAKE_CXX_STANDARD 14)
add_compile_options(-std=c++14 -fPIE -fstack-protector-all -fPIC -Wall -Wfloat-equal -pipe -fno-common)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
if(HIGH_TESTS)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE hitestwrapper)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK hitestwrapper)
endif()
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
if(BUILD_TESTS)
  # Add compile option -g when build test
  add_compile_options(-g -O2)
  # Remove link option -s when build test
  add_link_options(-Wl,-z,relro,-z,now,-z,noexecstack -pie)
else()
  add_link_options(-Wl,-z,relro,-z,now,-z,noexecstack -s -pie)
  add_compile_options(-O2)
endif()

add_compile_options(-Wno-deprecated-declarations)
# Set namespace from google to mindxsdk_private
add_definitions(-Dgoogle=mindxsdk_private)
add_definitions(-DPy_LIMITED_API=0x03090000)
# Protobuf library has been renamed with prefix mindxsdk_
set(LIB_PREFIX "mindxsdk_")
set(INSTALL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../output/${FRAME_PROJECT}/mxStream)
# add mxstream dependency
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include/Internal)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../module/StreamManager)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
link_directories(${INSTALL_PATH}/lib)

# add opensource mxbase mxtools dependency
get_filename_component(
  OPENSOURCE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../opensource/output/${FRAME_PROJECT}/opensource
  ABSOLUTE)
get_filename_component(
  MXBASE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../mxBase/output/${FRAME_PROJECT}/mxBase
  ABSOLUTE)
get_filename_component(
  MXTOOLS_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../mxTools/output/${FRAME_PROJECT}/mxTools
  ABSOLUTE)

include_directories(${OPENSOURCE_DIR}/include)
include_directories(${OPENSOURCE_DIR}/include/gstreamer-1.0)
include_directories(${OPENSOURCE_DIR}/include/glib-2.0)
include_directories(${OPENSOURCE_DIR}/lib/glib-2.0/include)
include_directories(${OPENSOURCE_DIR}/include/gstreamer-1.0/)
link_directories(${OPENSOURCE_DIR}/lib)
link_directories(${OPENSOURCE_DIR}/lib64)

include_directories(${MXBASE_DIR}/include)
link_directories(${MXBASE_DIR}/lib)

include_directories(${MXTOOLS_DIR}/include)
link_directories(${MXTOOLS_DIR}/lib)

# ascend include/lib add
set(ASCEND_LIB $ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/acllib)
include_directories(${ASCEND_LIB}/include)
include_directories($ENV{ASCEND_HOME}/ascend-toolkit/latest/include)
include_directories($ENV{ASCEND_HOME}/driver/include)
include_directories(
  $ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/$ENV{ARCH_PATTERN}/include)
include_directories($ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/opp/include)
link_directories($ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/opp/lib64)
link_directories(${ASCEND_LIB}/lib64)
link_directories($ENV{ASCEND_HOME}/driver/lib64)
link_directories($ENV{ASCEND_HOME}/$ENV{ASCEND_VERSION}/compiler/lib64)
link_directories($ENV{ASCEND_HOME}/driver/lib64/driver)

set(test_deps)
if(COVERAGE)
  list(APPEND test_deps gcov)
endif()

# add stream target
set(PYTHON_STREAM_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/module/PyUtils/PyDataHelper.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/module/Stream/PyPluginNode/PyPluginNode.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/module/Stream/PyStream/FunctionalStream.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/module/Stream/PyStream/SequentialStream.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/module/Stream/PyStream/StreamUtils.cpp
                            )

set(STREAM_TARGET_LIBRARY stream)
set_property(SOURCE SwigInterface/Stream.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE SwigInterface/Stream.i PROPERTY SWIG_MODULE_NAME
                                                ${STREAM_TARGET_LIBRARY})

find_package(SWIG REQUIRED)
include(UseSWIG)
find_package(Python3 ${PYTHON_VERSION} EXACT REQUIRED COMPONENTS Interpreter
                                                                 Development)
swig_add_library(
  ${STREAM_TARGET_LIBRARY}
  LANGUAGE python
  SOURCES SwigInterface/Stream.i ${PYTHON_STREAM_SRC_FILES})
target_include_directories(${STREAM_TARGET_LIBRARY} PRIVATE ${Python3_INCLUDE_DIRS})
target_link_directories(${STREAM_TARGET_LIBRARY} PRIVATE ${Python3_LIBRARIES})
swig_link_libraries(${STREAM_TARGET_LIBRARY} streammanager ${test_deps})

# add StreamManagerApi target
set(STREAM_MGR_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/module/PyUtils/PyDataHelper.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/module/StreamManagerApi/StreamManagerApi.cpp
                            ${CMAKE_CURRENT_SOURCE_DIR}/module/StreamManagerApi/StreamManagerInner.cpp
                            )
set(SWIG_TARGET_LIBRARY StreamManagerApi)
set_property(SOURCE SwigInterface/StreamManagerApi.i PROPERTY CPLUSPLUS ON)
set_property(SOURCE SwigInterface/StreamManagerApi.i PROPERTY SWIG_MODULE_NAME
                                                ${SWIG_TARGET_LIBRARY})
swig_add_library(
  ${SWIG_TARGET_LIBRARY}
  LANGUAGE python
  SOURCES SwigInterface/StreamManagerApi.i ${STREAM_MGR_SRC_FILES})
target_include_directories(${SWIG_TARGET_LIBRARY} PRIVATE ${Python3_INCLUDE_DIRS})
target_link_directories(${SWIG_TARGET_LIBRARY} PRIVATE ${Python3_LIBRARIES})
swig_link_libraries(${SWIG_TARGET_LIBRARY} mxbase
        streammanager
        ${test_deps})

add_custom_command(
  TARGET ${SWIG_TARGET_LIBRARY}
  COMMAND echo "${SWIG_TARGET_LIBRARY} only support python3.7 or greater"
  COMMAND
    sed -i '/from sys import version_info as _swig_python_version_info/a\\if _swig_python_version_info < \(3, 7, 0\):\\n\ \ \ \ raise Exception\(\"StreamManagerApi only support python3.7 or greater\"\)'
    ${CMAKE_CURRENT_BINARY_DIR}/StreamManagerApi.py
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS ${STREAM_TARGET_LIBRARY} LIBRARY
        DESTINATION ${INSTALL_PATH}/python)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/StreamManagerApi.py
        DESTINATION ${INSTALL_PATH}/python)
install(TARGETS ${SWIG_TARGET_LIBRARY} LIBRARY
        DESTINATION ${INSTALL_PATH}/python)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/stream.py
        DESTINATION ${INSTALL_PATH}/python)

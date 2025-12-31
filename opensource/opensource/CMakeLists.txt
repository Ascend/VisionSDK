cmake_minimum_required(VERSION 3.14.1)
project(opensource)

# Set source code directory for opensource software
set(OPENSOURCE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED MXSDK_INSTALL_PATH)
    set(OPENSOURCE_DIR  ${MXSDK_INSTALL_PATH}/opensource)
else()
    set(OPENSOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dist)
endif()
message("OPENSOURCE_DIR: ${OPENSOURCE_DIR}")

if(NOT EXISTS ${OPENSOURCE_DIR})
    file(MAKE_DIRECTORY ${OPENSOURCE_DIR})
    message("make directory: ${OPENSOURCE_DIR}")
endif()

include(cmake/utils.cmake)
include(cmake/glog.cmake)
include(cmake/openssl.cmake)

# Delete the share directory under OPENSOURCE_DIR
file(REMOVE_RECURSE ${OPENSOURCE_DIR}/share)
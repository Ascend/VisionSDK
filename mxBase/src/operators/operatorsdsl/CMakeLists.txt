# CMake minimum version number requirements
cmake_minimum_required(VERSION 3.14.1)
project(operatorsdsl)

install(
    DIRECTORY
        ${PROJECT_SOURCE_DIR}/../operatorsdsl/op/build_out
    DESTINATION
        ${CMAKE_INSTALL_PREFIX}/operators/operatorsdsl
)
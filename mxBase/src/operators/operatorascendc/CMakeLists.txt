# CMake minimum version number requirements
cmake_minimum_required(VERSION 3.14.1)
project(operatorascendc)


install(
        DIRECTORY
        ${PROJECT_SOURCE_DIR}/../operatorascendc
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/operators/
)
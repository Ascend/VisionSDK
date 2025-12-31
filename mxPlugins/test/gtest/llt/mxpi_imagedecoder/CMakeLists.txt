cmake_minimum_required(VERSION 3.14.1)

# *need modify: plugin name*
set(PLUGIN_NAME "mxpi_imagedecoder")

add_compile_options("-DPLUGIN_NAME=${PLUGIN_NAME}")
set(TARGET_EXECUTABLE "${PLUGIN_NAME}_test")

PROJECT(${TARGET_EXECUTABLE})

file(GLOB_RECURSE SRCS *.cpp)

# *need modify: plugin source code and test work dir*
set(DUT_SRCS ${CMAKE_SOURCE_DIR}/src/module/MxpiImageDecoder/MxpiImageDecoder.cpp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist/dt/MxpiImageDecoder)

add_executable(
    ${TARGET_EXECUTABLE}
    ${SRCS}
    ${DUT_SRCS}
)

target_link_libraries(
    ${TARGET_EXECUTABLE}
    ${GTEST_LIBRARY}
    ${GTEST_MAIN_LIBRARY}
    ${MXPLUGINS_TEST_COMMON_DEP_LIBS}
)

# *need modify: plugin test resource*
file(GLOB_RECURSE TEST_FILES ${CMAKE_SOURCE_DIR}/test/hlt/MxpiImageDecoder/*.output)

install(FILES ${TEST_FILES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

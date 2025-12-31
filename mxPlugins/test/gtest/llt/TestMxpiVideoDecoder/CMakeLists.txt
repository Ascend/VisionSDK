set(CMAKE_VERBOSE_MAKEFILE on)
set(PLUGIN_NAME "mxpi_videodecoder")
set(TARGET_EXECUTABLE TestMxpiVideoDecoder)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${TARGET_EXECUTABLE})

find_package(GTest REQUIRED)

add_compile_definitions(ENABLE_DVPP_INTERFACE)
add_compile_definitions(GST_STATIC_COMPILATION)
add_compile_options("-DPLUGIN_NAME=${PLUGIN_NAME}")

add_executable( ${TARGET_EXECUTABLE}
        TestMxpiVideoDecoder.cpp
        )
target_link_libraries(${TARGET_EXECUTABLE} ${MXPLUGINS_TEST_COMMON_DEP_LIBS} mxpi_videodecoder)

file(GLOB_RECURSE TEST_FILE config/logging.conf)
install(FILES ${TEST_FILE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config)
install(FILES h264_vdec.input h264_vdec.output DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
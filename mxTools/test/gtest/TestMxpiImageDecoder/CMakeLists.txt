set(CMAKE_VERBOSE_MAKEFILE on)
set(PLUGIN_NAME "mxpi_imagedecoder")
set(TARGET_EXECUTABLE TestMxpiImageDecoder)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${TARGET_EXECUTABLE})

add_compile_definitions(ENABLE_DVPP_INTERFACE)
add_compile_definitions(GST_STATIC_COMPILATION)
add_compile_options("-DPLUGIN_NAME=${PLUGIN_NAME}")

add_executable( ${TARGET_EXECUTABLE}
        TestDvppImageDecoder.cpp
        )

target_link_libraries(${TARGET_EXECUTABLE} ${MXTOOLS_TEST_COMMON_DEP_LIBS} mxpi_imagedecoder gtest gtest_main)

file(GLOB_RECURSE TEST_FILES *.json)
file(GLOB_RECURSE TEST_FILE config/logging.conf)

install(FILES ${TEST_FILES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
install(FILES ${TEST_FILE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
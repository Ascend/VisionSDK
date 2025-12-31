set(CMAKE_VERBOSE_MAKEFILE on)
set(PLUGIN_NAME "mxpi_imagenormalize")
set(TARGET_EXECUTABLE TestMxpiImageNormalize)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${TARGET_EXECUTABLE})

find_package(GTest REQUIRED)

add_compile_definitions(ENABLE_DVPP_INTERFACE)
add_compile_definitions(GST_STATIC_COMPILATION)
add_compile_options("-DPLUGIN_NAME=${PLUGIN_NAME}")

add_executable( ${TARGET_EXECUTABLE}
        TestMxpiImageNormalize.cpp
        )

target_link_libraries(${TARGET_EXECUTABLE} ${MXPLUGINS_TEST_COMMON_DEP_LIBS} mxpi_imagenormalize)

file(GLOB_RECURSE INPUT_FILES input/*)
file(GLOB_RECURSE OUTPUT_FILES output/*)
install(FILES ${INPUT_FILES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/input)
install(FILES ${OUTPUT_FILES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/output)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
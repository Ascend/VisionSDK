set(CMAKE_VERBOSE_MAKEFILE on)
set(PLUGIN_NAME "mxpi_datatransfer")
set(TARGET_EXECUTABLE TestMxpiDataTransfer)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${TARGET_EXECUTABLE})

find_package(GTest REQUIRED)

add_compile_definitions(ENABLE_DVPP_INTERFACE)
add_compile_definitions(GST_STATIC_COMPILATION)
add_compile_options("-DPLUGIN_NAME=${PLUGIN_NAME}")

add_executable(${TARGET_EXECUTABLE} TestMxpiDataTransfer.cpp)

target_link_libraries(${TARGET_EXECUTABLE} ${MXPLUGINS_TEST_COMMON_DEP_LIBS} mxpi_datatransfer)

file(GLOB_RECURSE INPUT_FILES input/*)

install(FILES ${INPUT_FILES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/input)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
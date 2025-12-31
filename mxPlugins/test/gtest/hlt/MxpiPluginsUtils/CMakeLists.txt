set(TARGET_EXECUTABLE "MxpiPluginsUtilsTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/MxpiPluginsUtils)
file(GLOB_RECURSE SRCS MxpiPluginsUtilsTest.cpp)
add_executable(${TARGET_EXECUTABLE} ${SRCS})
add_dependencies(${TARGET_EXECUTABLE} mxpi_modelinfer)
target_link_libraries(${TARGET_EXECUTABLE} ${MXPLUGINS_HLT_TEST_COMMON_DEP_LIBS} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../../../../../build_result/arm-gcc4/src/module/MxpiModelInfer/libmxpi_modelinfer.so)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
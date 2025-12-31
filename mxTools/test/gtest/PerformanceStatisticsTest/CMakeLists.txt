set(TARGET_EXECUTABLE "PerformanceStatisticTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/PerformanceStatistic)
file(GLOB_RECURSE SRCS PerformanceStatisticTest.cpp)
include_directories(${PROJECT_SOURCE_DIR}/../../src/module/PluginToolkit/PerformanceStatistics)
include_directories(${PROJECT_SOURCE_DIR}/../../../mxBase/src/mxbase/include)
include_directories(/usr/local/mockcpp/include)
link_directories(/usr/local/mockcpp/lib)
add_executable(${TARGET_EXECUTABLE} ${SRCS})

target_link_libraries(${TARGET_EXECUTABLE} ${MXTOOLS_TEST_COMMON_DEP_LIBS} gtest mockcpp)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

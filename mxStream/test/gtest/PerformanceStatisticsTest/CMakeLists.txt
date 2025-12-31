cmake_minimum_required(VERSION 3.14.1)
set(TARGET_EXECUTABLE "PerformanceStatisticsTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/PerformanceStatisticsTest)

file(GLOB_RECURSE SRCS ./*.cpp)

add_executable(${TARGET_EXECUTABLE} ${SRCS})

target_link_libraries(${TARGET_EXECUTABLE} ${MXSTREAM_TEST_COMMON_DEP_LIBS})

install(FILES PerformanceStatistics.pipeline DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
install(FILES config/sdk.conf DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

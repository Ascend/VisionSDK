set(TARGET_EXECUTABLE "SsdMobilenetv1FpnPostProcessTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/PostProcessUT/SsdMobilenetv1FpnPostProcess)

file(GLOB_RECURSE SRCS *.cpp)
include_directories(${PROJECT_SOURCE_DIR}/../../src/)
add_compile_definitions(ENABLE_POST_PROCESS_INSTANCE)
add_executable(${TARGET_EXECUTABLE} ${SRCS})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest ssdmobilenetv1fpnpostprocess mockcpp)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

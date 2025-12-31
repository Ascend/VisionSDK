set(TARGET_EXECUTABLE "Yolov3PostProcessTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/PostProcessUT/Yolov3PostProcess)

file(GLOB_RECURSE SRCS *.cpp)
include_directories(${PROJECT_SOURCE_DIR}/../../src/)
add_compile_definitions(ENABLE_POST_PROCESS_INSTANCE)
add_executable(${TARGET_EXECUTABLE} ${SRCS})

target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest yolov3postprocess mockcpp)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

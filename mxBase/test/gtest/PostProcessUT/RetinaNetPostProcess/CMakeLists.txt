set(TARGET_EXECUTABLE "RetinaNetPostProcessTest")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/PostProcessUT/RetinaNetPostProcess)

file(GLOB_RECURSE SRCS *.cpp)
include_directories(${PROJECT_SOURCE_DIR}/../../src/)
add_compile_definitions(ENABLE_POST_PROCESS_INSTANCE)
add_executable(${TARGET_EXECUTABLE} ${SRCS})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest retinanetpostprocess mockcpp)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

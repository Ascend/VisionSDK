set(TARGET_EXECUTABLE "TensorBufferTest")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/Tensor/TensorBufferTest)

file(GLOB_RECURSE SRCS *.cpp)
include_directories(${PROJECT_SOURCE_DIR}/../../src/mxbase/module/MbCV/Tensor/Tensor/TensorBuffer)
add_executable(${TARGET_EXECUTABLE} ${SRCS})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

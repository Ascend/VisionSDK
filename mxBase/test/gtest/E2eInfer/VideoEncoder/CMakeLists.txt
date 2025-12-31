set(TARGET_EXECUTABLE "VideoEncoderTest")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/E2eInfer/VideoEncoder/VideoEncoderTest)

file(GLOB_RECURSE SRCS *.cpp)
add_executable(${TARGET_EXECUTABLE} ${SRCS})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest mockcpp)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/E2eInfer/VideoEncoder/TestPicture/
        DESTINATION ${PROJECT_SOURCE_DIR}/dist/E2eInfer/VideoEncoder/VideoEncoderTest)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
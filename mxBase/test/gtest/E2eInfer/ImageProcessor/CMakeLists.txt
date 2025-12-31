set(TARGET_EXECUTABLE "ImageProcessorTest")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/E2eInfer/ImageProcessor/ImageProcessorTest)

file(GLOB_RECURSE SRCS *.cpp)
add_executable(${TARGET_EXECUTABLE} ${SRCS})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest mockcpp)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/E2eInfer/ImageProcessor/TestPicture/
        DESTINATION ${PROJECT_SOURCE_DIR}/dist/E2eInfer/ImageProcessor/ImageProcessorTest)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
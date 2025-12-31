set(TARGET_EXECUTABLE "VideoDecoderInnerTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/Module/MbCV/Video/VideoDecoderInner)

file(GLOB_RECURSE SOURCE_FILES ./*.cpp)

add_executable(${TARGET_EXECUTABLE} ${SOURCE_FILES})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest mockcpp)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/Module/MbCV/Video/VideoDecoderInner)

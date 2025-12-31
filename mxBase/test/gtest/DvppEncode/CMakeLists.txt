set(TARGET_EXECUTABLE "DvppEncodeTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/DvppEncode)

file(GLOB_RECURSE SOURCE_FILES ./*.cpp)

add_executable(${TARGET_EXECUTABLE} ${SOURCE_FILES})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest)

file(GLOB_RECURSE TEST_FILE *.jpg)

install(FILES ${TEST_FILE} DESTINATION ${PROJECT_SOURCE_DIR}/dist/DvppEncode)


add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/DvppEncode)

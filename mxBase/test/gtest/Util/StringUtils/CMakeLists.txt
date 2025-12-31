set(TARGET_EXECUTABLE "StringUtilsTest")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/Utils/StringUtils)

file(GLOB_RECURSE SRCS *.cpp)
add_executable(${TARGET_EXECUTABLE} ${SRCS})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest mockcpp)

install(FILES ./test.pipeline DESTINATION ${PROJECT_SOURCE_DIR}/dist/Utils/StringUtils)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

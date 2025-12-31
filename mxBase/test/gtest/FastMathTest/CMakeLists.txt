set(TARGET_EXECUTABLE "FastMathTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/FastMathTest)

file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/FastMathTest/FastMathTest.cpp)
add_executable(${TARGET_EXECUTABLE} ${SOURCE_FILES})

target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/FastMathTest)

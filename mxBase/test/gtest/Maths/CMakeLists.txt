set(TARGET_EXECUTABLE "NpySortTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/Maths)

include_directories(${PROJECT_SOURCE_DIR}/../../src/mxbase)
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/Maths/NpySortTest.cpp)
add_executable(${TARGET_EXECUTABLE} ${SOURCE_FILES})

target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest mockcpp)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/Maths)

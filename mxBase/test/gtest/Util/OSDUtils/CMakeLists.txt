set(TARGET_EXECUTABLE "OSDUtilsTest")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/Utils/OSDUtilsTest)

file(GLOB_RECURSE SRCS *.cpp)
add_executable(${TARGET_EXECUTABLE} ${SRCS})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest mockcpp)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

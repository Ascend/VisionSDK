set(TARGET_EXECUTABLE "HuangarianTest")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/CV/HuangarianTest)

include_directories(${PROJECT_SOURCE_DIR}/../../src/mxbase)

file(GLOB_RECURSE SRCS *.cpp)
add_executable(${TARGET_EXECUTABLE} ${SRCS})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest mockcpp)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/Util/TestPicture/ DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

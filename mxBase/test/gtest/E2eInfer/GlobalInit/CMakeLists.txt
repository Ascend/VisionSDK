set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/E2eInfer/GlobalInit)

set(TARGET_EXECUTABLE "GlobalInitTest")

file(GLOB_RECURSE SRCS *.cpp)
add_executable(${TARGET_EXECUTABLE} ${SRCS})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest mockcpp -pthread)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/E2eInfer/GlobalInit/TestInit
        DESTINATION ${PROJECT_SOURCE_DIR}/dist/E2eInfer/GlobalInit)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(TARGET_EXECUTABLE "OpenPosePostProcessTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/PostProcessUT/OpenPosePostProcess)

file(GLOB_RECURSE SRC *.cpp)
include_directories(${PROJECT_SOURCE_DIR}/../../src/)
add_executable(${TARGET_EXECUTABLE} ${SRC})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest openposepostprocess)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

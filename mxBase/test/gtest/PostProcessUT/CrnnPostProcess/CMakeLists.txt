set(TARGET_EXECUTABLE "CrnnPostProcessTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/PostProcessUT/CrnnPostProcess)

file(GLOB_RECURSE SRC *.cpp)
add_executable(${TARGET_EXECUTABLE} ${SRC})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest crnnpostprocess)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

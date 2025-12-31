set(TARGET_EXECUTABLE "Ssdvgg16PostProcessTest")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/PostProcessUT/Ssdvgg16PostProcess)

file(GLOB_RECURSE SRC *.cpp)
add_compile_definitions(ENABLE_POST_PROCESS_INSTANCE)
add_executable(${TARGET_EXECUTABLE} ${SRC})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest ssdvgg16postprocess)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
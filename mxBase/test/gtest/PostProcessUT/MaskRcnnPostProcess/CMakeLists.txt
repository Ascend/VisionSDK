set(TARGET_EXECUTABLE "MaskRcnnMindsporePostProcessTest")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/PostProcessUT/MaskRcnnMindsporePostProcessTest)
include_directories(${PROJECT_SOURCE_DIR}/../../src/mxbase)

include_directories(${PROJECT_SOURCE_DIR}/../../src/postprocess/module/ObjectPostProcessors/MaskRcnnMindsporePost)

file(GLOB_RECURSE SRCS *.cpp)
add_compile_definitions(ENABLE_POST_PROCESS_INSTANCE)
add_executable(${TARGET_EXECUTABLE} ${SRCS})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest maskrcnnmindsporepost mockcpp)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

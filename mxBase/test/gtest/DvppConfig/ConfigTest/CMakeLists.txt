set(TARGET_EXECUTABLE "DvppConfigTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/DvppConfig/DvppConfig)

file(GLOB_RECURSE SOURCE_FILES ./*.cpp)
include_directories(${PROJECT_SOURCE_DIR}/../../src/mxbase/module/ResourceManager/DvppWrapper/DvppConfig/Config/ConfigAtlas800IA2)

add_executable(${TARGET_EXECUTABLE} ${SOURCE_FILES})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest)
add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/DvppConfig/DvppConfig)

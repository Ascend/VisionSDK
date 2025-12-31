set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/Module/ResourceManager)
include_directories(${PROJECT_SOURCE_DIR}/../../src/mxbase)

set(TARGET_EXECUTABLE "AclApiTest")
file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/Module/ResourceManager/AclApiTest.cpp)
add_executable(${TARGET_EXECUTABLE} ${SOURCE_FILES})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest mockcpp dl)
add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

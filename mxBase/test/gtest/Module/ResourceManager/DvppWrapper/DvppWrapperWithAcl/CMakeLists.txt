set(TARGET_EXECUTABLE "DvppWrapperWithAclTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/Module/ResourceManager)

include_directories(${PROJECT_SOURCE_DIR}/../../src/mxbase)

file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/Module/ResourceManager/DvppWrapper/DvppWrapperWithAcl/DvppWrapperWithAclTest.cpp)

add_executable(${TARGET_EXECUTABLE} ${SOURCE_FILES})

target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest mockcpp -pthread)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
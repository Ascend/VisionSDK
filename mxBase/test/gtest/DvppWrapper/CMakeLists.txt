set(TARGET_EXECUTABLE "DvppWrapperTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/DvppWrapper)

file(GLOB_RECURSE SOURCE_FILES ./*.cpp)

add_executable(${TARGET_EXECUTABLE} ${SOURCE_FILES})
target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest mockcpp)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/DvppWrapper/TestPicture/ DESTINATION ${PROJECT_SOURCE_DIR}/dist/DvppWrapper)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/DvppWrapper/TestVideo/ DESTINATION ${PROJECT_SOURCE_DIR}/dist/DvppWrapper/TestVideo)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/DvppWrapper)

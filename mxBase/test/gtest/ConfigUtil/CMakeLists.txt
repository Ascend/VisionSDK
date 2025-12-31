set(TARGET_EXECUTABLE "ConfigUtilTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/ConfigUtil)

file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/ConfigUtil/ConfigUtilTest.cpp)
add_executable(${TARGET_EXECUTABLE} ${SOURCE_FILES})

target_link_libraries(${TARGET_EXECUTABLE} mxbase gtest) 

install(FILES ${PROJECT_SOURCE_DIR}/ConfigUtil/facedemoReg.config DESTINATION ${PROJECT_SOURCE_DIR}/dist/ConfigUtil)
install(FILES ./invalid.config DESTINATION ${PROJECT_SOURCE_DIR}/dist/ConfigUtil)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/ConfigUtil)

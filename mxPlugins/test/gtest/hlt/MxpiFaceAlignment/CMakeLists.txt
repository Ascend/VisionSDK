set(TARGET_EXECUTABLE "MxpiFaceAlignmentTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/MxpiFaceAlignmentTest)

add_compile_options(-std=c++14 -fPIC -fstack-protector-all -g -Wl,-z,relro,-z,now,-z,noexecstack -pie -O2 -Wall)

set(CMAKE_CXX_COMPILER "g++")

include_directories(${MXSTREAM_DIR}/include)

link_directories(${MXSTREAM_DIR}/lib)

file(GLOB_RECURSE SRCS MxpiFaceAlignmentTest.cpp)

add_executable(${TARGET_EXECUTABLE} ${SRCS})

target_link_libraries(${TARGET_EXECUTABLE} ${MXPLUGINS_HLT_TEST_COMMON_DEP_LIBS})

file(GLOB_RECURSE TEST_FILES *.jpg *.pipeline *.output)
file(GLOB_RECURSE TEST_FILE config/logging.conf)

install(FILES ${TEST_FILES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
install(FILES ${TEST_FILE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

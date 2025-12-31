set(TARGET_EXECUTABLE "MxPluginBaseTest")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/MxPluginBaseTest)
include_directories(${PROJECT_SOURCE_DIR}/../../src/module/PluginToolkit/base)
include_directories(${PROJECT_SOURCE_DIR}/../../../mxBase/src/mxbase/include)
include_directories(/usr1/mxVision/opensource/include/)
include_directories(/usr/local/mockcpp/include)
link_directories(/usr/local/mockcpp/lib)
add_executable(${TARGET_EXECUTABLE} MxPluginBaseTest.cpp)

target_link_libraries(${TARGET_EXECUTABLE} ${MXTOOLS_TEST_COMMON_DEP_LIBS} gtest mockcpp)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


set(TARGET_EXECUTABLE_GST "MxGstBaseTest")
add_link_options(-Wl,--no-as-needed -Wl,--copy-dt-needed-entries)
add_executable(${TARGET_EXECUTABLE_GST} MxGstBaseTest.cpp)
target_link_libraries(${TARGET_EXECUTABLE_GST} ${MXTOOLS_TEST_COMMON_DEP_LIBS} gtest mockcpp mindxsdk_protobuf)

add_test(NAME ${TARGET_EXECUTABLE_GST}
        COMMAND ${TARGET_EXECUTABLE_GST} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(TARGET_EXECUTABLE_BUFFER "MxpiBufferDumpTest")
add_definitions(-Dgoogle=mindxsdk_private)
add_link_options(-Wl,--no-as-needed -Wl,--copy-dt-needed-entries)
add_executable(${TARGET_EXECUTABLE_BUFFER} MxpiBufferDumpTest.cpp)
target_link_libraries(${TARGET_EXECUTABLE_BUFFER} ${MXTOOLS_TEST_COMMON_DEP_LIBS} gtest mockcpp mindxsdk_protobuf)

add_test(NAME ${TARGET_EXECUTABLE_BUFFER}
        COMMAND ${TARGET_EXECUTABLE_BUFFER} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
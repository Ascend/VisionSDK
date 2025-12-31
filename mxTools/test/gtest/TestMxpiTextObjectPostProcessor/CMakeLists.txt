set(CMAKE_VERBOSE_MAKEFILE on)
set(PLUGIN_NAME "mxpi_textobjectpostprocessor")
set(TARGET_EXECUTABLE TestMxpiTextObjectPostProcessor)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/dist/${TARGET_EXECUTABLE})

find_package(GTest REQUIRED)

add_compile_definitions(ENABLE_DVPP_INTERFACE)
add_compile_definitions(GST_STATIC_COMPILATION)
add_compile_options("-DPLUGIN_NAME=${PLUGIN_NAME}")

include_directories(${MXTOOLS_ROOT_PATH}/../mxPlugins/output/${PROJECT}/mxPlugins/include)
link_directories(${MXTOOLS_ROOT_PATH}/../mxPlugins/output/${PROJECT}/mxPlugins/lib/plugins/)
include_directories(${MXTOOLS_ROOT_PATH}/../mxPlugins/test/gtest/MxpiCommon)

add_executable( ${TARGET_EXECUTABLE}
        TestMxpiTextObjectPostProcessor.cpp
        )

target_link_libraries(${TARGET_EXECUTABLE} plugintoolkit ${MXTOOLS_TEST_COMMON_DEP_LIBS} mxpi_textobjectpostprocessor)

file(GLOB_RECURSE TEST_FILES *.json*)
file(GLOB_RECURSE TEST_FILE config/logging.conf)

install(FILES ${TEST_FILES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/)
install(FILES ${TEST_FILE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config)
install(FILES ${MXBASE_DIR}/output/${PROJECT}/mxBase/lib/modelpostprocessors/libctpnpostprocess.so
        DESTINATION ${PROJECT_SOURCE_DIR}/dist/lib
        PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ
)

add_test(NAME ${TARGET_EXECUTABLE}
        COMMAND ${TARGET_EXECUTABLE} --gtest_output=xml
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
project(mxToolsUnitCases)
add_subdirectory(buffer)
add_subdirectory(MetadataGraph)
add_subdirectory(MetaDataManagerTest)
add_subdirectory(PluginDemo/demoa)
add_subdirectory(PluginDemo/demob)
add_subdirectory(DataTypeDeleterTest)
add_subdirectory(DataTypeWrapperTest)
add_subdirectory(PostProcessorBaseTest)

# Create the gcov target. Run coverage tests with 'make gcovr'
add_custom_target(mxtools-gcovr
        COMMAND mkdir -p coverage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_command(TARGET mxtools-gcovr
        COMMAND echo "========= GCOVR html is creating ... ===="
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --exclude="${OPENSOURCE_DIR}" --exclude="${MXBASE_DIR}" --branches --html --html-details -o mxTools-details.html
        COMMAND echo "========= GCOVR html is created successfully. ===="
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage)

add_custom_target(mxtools-gcovr-xml
        COMMAND mkdir -p coverage-xml
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_command(TARGET mxtools-gcovr-xml
        COMMAND echo "========= GCOVR xml is creating ... ===="
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --exclude="${OPENSOURCE_DIR}" --exclude="${MXBASE_DIR}" --branches --xml --xml-pretty -o coverage.xml
        COMMAND echo "========= GCOVR xml is created successfully. ===="
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage-xml)

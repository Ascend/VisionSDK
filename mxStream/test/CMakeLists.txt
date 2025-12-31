project(mxStreamUnitCases)

add_compile_options(-std=c++14 -fPIC -fstack-protector-all -g -Wl,-z,relro,-z,now,-z,noexecstack -pie -O2 -Wall)

add_subdirectory(gtest/AppsinkOutputTest)
add_subdirectory(gtest/MxStreamTest)
add_subdirectory(gtest/MxStreamManagerTest)
add_subdirectory(gtest/MxStreamManagerTest2)
add_subdirectory(gtest/PerformanceStatisticsTest)
add_subdirectory(gtest/SetElementPropertyTest)
add_subdirectory(gtest/StreamTest)
add_subdirectory(gtest/DataTypeTest)
add_subdirectory(gtest/DataTransformTest)
add_subdirectory(gtest/InternalClassTest)
add_subdirectory(python)

# Create the gcov target. Run coverage tests with 'make gcovr'
add_custom_target(mxstream-gcovr
        COMMAND mkdir -p coverage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_command(TARGET mxstream-gcovr
        COMMAND echo "========= GCOVR html is creating ... ===="
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --exclude="${OPENSOURCE_DIR}" --exclude="${MXBASE_DIR}" --exclude="${MXTOOLS_DIR}" --exclude="${CMAKE_SOURCE_DIR}/build/" --branches --html --html-details -o mxStream-details.html
        COMMAND echo "========= GCOVR html is created successfully. ===="
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage)

add_custom_target(mxstream-gcovr-xml
        COMMAND mkdir -p coverage-xml
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_command(TARGET mxstream-gcovr-xml
        COMMAND echo "========= GCOVR xml is creating ... ===="
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --exclude="${OPENSOURCE_DIR}" --exclude="${MXBASE_DIR}" --exclude="${MXTOOLS_DIR}" --exclude="${CMAKE_SOURCE_DIR}/build/" --branches --xml --xml-pretty -o coverage.xml
        COMMAND echo "========= GCOVR xml is created successfully. ===="
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage-xml)

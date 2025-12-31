project(mxBaseGtestUnitCases)

add_subdirectory(ConfigUtil)
add_subdirectory(DvppWrapper)
add_subdirectory(MemoryHelper)
add_subdirectory(ModelInfer)
add_subdirectory(Util)
add_subdirectory(ErrorCode)
add_subdirectory(Log)
add_subdirectory(NmsTest)
add_subdirectory(FastMathTest)
add_subdirectory(CV)
add_subdirectory(DvppEncode)
add_subdirectory(DeviceManager)
add_subdirectory(PostProcess)
add_subdirectory(PyAPITest)
add_subdirectory(Common)
add_subdirectory(Maths)
add_subdirectory(Tensor)
add_subdirectory(PostProcessUT)
add_subdirectory(E2eInfer)
add_subdirectory(DvppConfig)
add_subdirectory(Module)

# Create the gcov target. Run coverage tests with 'make mxbase-gcovr'
add_custom_target(mxbase-gcovr
        COMMAND mkdir -p coverage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_command(TARGET mxbase-gcovr
        COMMAND echo "========= GCOVR html is creating ... ===="
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --exclude="${OPENSOURCE_DIR}" --branches --html --html-details -o mxBase-details.html
        COMMAND echo "========= GCOVR html is created successfully. ===="
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage)

add_custom_target(mxbase-gcovr-xml
        COMMAND mkdir -p coverage-xml
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_command(TARGET mxbase-gcovr-xml
        COMMAND echo "========= GCOVR xml is creating ... ===="
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --exclude="${OPENSOURCE_DIR}" --branches --xml --xml-pretty -o coverage.xml
        COMMAND echo "========= GCOVR xml is created successfully. ===="
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage-xml)

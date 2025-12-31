project(mxPluginsUnitCases)

add_compile_options(-std=c++14 -fPIC -fstack-protector-all -g -Wl,-z,relro,-z,now,-z,noexecstack -pie -O2 -Wall)

if(${MXSTREAM_DIR})
    message("MXSTREAM_DIR is not defined, please set it first.")
else()
    message("MXSTREAM_DIR=${MXSTREAM_DIR}")
endif()
add_definitions(-DGLOG_USE_GLOG_EXPORT)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${MXSTREAM_DIR}/src/include)
link_directories(${MXSTREAM_DIR}/output/${PROJECT}/mxStream/lib)

# set common linker libraries for hlt test
set(MXPLUGINS_HLT_TEST_COMMON_DEP_LIBS mxbase glog gtest
    glib-2.0 gstreamer-1.0 gobject-2.0 gstbase-1.0 gmodule-2.0 gstapp-1.0
    ${LIB_PREFIX}protobuf plugintoolkit mxpidatatype streammanager)
add_subdirectory(CrnnModel)
add_subdirectory(DynamicBatchsize)
add_subdirectory(FaceLandmarkModel)
add_subdirectory(FasterRcnnModel)
add_subdirectory(MpDumpData)
add_subdirectory(MpLoadData)
add_subdirectory(MpPortType)
add_subdirectory(MultiBatchsize)
add_subdirectory(MxpiDataTransfer)
add_subdirectory(MxpiDistributor)
add_subdirectory(MxpiFaceAlignment)
add_subdirectory(MxpiImageDecoder)
add_subdirectory(MxpiModelInfer)
#add_subdirectory(MxpiMotSimpleSort)
add_subdirectory(MxpiParallel2Serial)
add_subdirectory(MxpiPictureCrop)
add_subdirectory(MxpiPictureResize)
add_subdirectory(MxpiPluginsUtils)
add_subdirectory(MxpiSynchronize)
#add_subdirectory(SetDeviceId)
add_subdirectory(SsdMobilenetFpnModel)
add_subdirectory(SsdvggModel)
add_subdirectory(MxpiVideoEncode)
#add_subdirectory(KeepAspectRatioResizer)
add_subdirectory(MxpiImageEncoder)
add_subdirectory(AutoDataSource)
add_subdirectory(SetPluginProperty)
add_subdirectory(MxpiRoiGenerator)
add_subdirectory(MxpiNmsOverlapedRoiV2)
add_subdirectory(MxpiClass2OsdInstances)
add_subdirectory(MxpiObject2OsdInstances)
add_subdirectory(MxpiOpencvOsd)
add_subdirectory(MxpiChannelImagesStitcher)
add_subdirectory(MxpiChannelSelector)
add_subdirectory(MxpiOsdInstanceMerger)
add_subdirectory(MxpiChannelOsdCoordsConverter)
add_subdirectory(MxpiQualityDetection)
add_subdirectory(MxpiClassPostProcessor)
#add_subdirectory(MxpiObjectPostProcessor)
add_subdirectory(MxpiTextGenerationPostProcessor)
add_subdirectory(MxpiTextObjectPostProcessor)
add_subdirectory(MxpiKeyPointPostProcessor)
add_subdirectory(MxpiSemanticSegPostProcessor)
add_subdirectory(MxpiObjectSelector)
add_subdirectory(PyPostProcessTest)

# Create the gcov target. Run coverage tests with 'make gcovr'
add_custom_target(mxplugins-gcovr
        COMMAND mkdir -p coverage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_command(TARGET mxplugins-gcovr
        COMMAND echo "========= GCOVR html is creating ... ===="
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --exclude="${OPENSOURCE_DIR}" --exclude="${MXBASE_DIR}" --exclude="${MXTOOLS_DIR}" --exclude="${MXSTREAM_DIR}" --branches --html --html-details -o mxPlugins-details.html
        COMMAND echo "========= GCOVR html is created successfully. ===="
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage)

add_custom_target(mxplugins-gcovr-xml
        COMMAND mkdir -p coverage-xml
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

add_custom_command(TARGET mxplugins-gcovr-xml
        COMMAND echo "========= GCOVR xml is creating ... ===="
        COMMAND gcovr -r ${CMAKE_SOURCE_DIR} --exclude="${OPENSOURCE_DIR}" --exclude="${MXBASE_DIR}" --exclude="${MXTOOLS_DIR}" --exclude="${MXSTREAM_DIR}" --branches --xml --xml-pretty -o coverage.xml
        COMMAND echo "========= GCOVR xml is created successfully. ===="
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage-xml)

file(GLOB_RECURSE TEST_FILE test_pictures/*.jpg)
install(FILES ${TEST_FILE} DESTINATION ${PROJECT_SOURCE_DIR}/dist/test_pictures)
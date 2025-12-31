cmake_minimum_required(VERSION 3.14.1)

project(mxPluginsDTCases)
include(DTFrameHelper.cmake)
add_compile_definitions(GST_STATIC_COMPILATION)
set(CMAKE_CXX_STANDARD 14) 
add_definitions(-DGLOG_USE_GLOG_EXPORT)
find_package(GTest REQUIRED)
include_directories(GTEST_INCLUDE_DIRS)

set(MXPLUGINS_TEST_COMMON_DEP_LIBS
        mxbase gtest ${LIB_PREFIX}protobuf glog glib-2.0 gstreamer-1.0 gobject-2.0 gstbase-1.0
        gmodule-2.0 gstapp-1.0 dl opencv_world plugintoolkit mxpidatatype)

# add all sub directory project at default
# - if specify one, then just add that one
# - exclude "build;test;dist;..." directory
# - mxpi_imagedecoder: demo plugin test for MxpiImageDecoder
set(EXCLUDEDIRS build test dist mxpi_imagedecoder)
SUBDIRLIST_WITH_EXCLUDE(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR} "${EXCLUDEDIRS}")
if(SPECIFY)
  ADD_SUBDIRECTORY(${SPECIFY})
else()
  FOREACH(subdir ${SUBDIRS})
    ADD_SUBDIRECTORY(${subdir})
  ENDFOREACH()
endif()

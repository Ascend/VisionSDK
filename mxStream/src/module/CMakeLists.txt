set(test_deps)
if (COVERAGE)
    list(APPEND test_deps gcov)
endif ()
set(MXSTREAM_COMMON_DEP_LIBS
        glib-2.0
        gstreamer-1.0
        gobject-2.0
        gstbase-1.0
        gmodule-2.0
        mxbase
        mxpidatatype
        plugintoolkit
        mindxsdk_protobuf
        crypto
        ssl
        pthread
        glog
        gstapp-1.0
        ${test_deps}
)
add_library(streammanager SHARED)
add_subdirectory(DataType)
add_subdirectory(Stream)
add_subdirectory(Packet)
add_subdirectory(StreamManager)
target_link_libraries(streammanager ${MXSTREAM_COMMON_DEP_LIBS})
install(TARGETS streammanager DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
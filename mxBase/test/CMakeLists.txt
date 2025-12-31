project(mxBaseUnitCases)
if(COVERAGE)
    get_directory_property(CUR_OPTS COMPILE_OPTIONS)
    set_directory_properties(PROPERTIES COMPILE_OPTIONS "${CUR_OPTS}")
endif()

add_definitions(-DGLOG_USE_GLOG_EXPORT)
add_subdirectory(gtest)
# add_subdirectory(dtframe)
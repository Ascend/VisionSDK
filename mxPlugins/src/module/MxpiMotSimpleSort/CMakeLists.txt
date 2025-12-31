# The mxpi_motsimplesort plugin no need to do dt test because it will be deprecated
# The mxpi_motsimplesortV2 plugin is used in AllObjectsStructuring reference, 
# which is not suitable for dt test because of the long test time.
if(BUILD_TESTS)
    get_directory_property(CUR_OPTS COMPILE_OPTIONS)
    list(REMOVE_ITEM CUR_OPTS -fprofile-arcs -ftest-coverage)
    set_directory_properties(PROPERTIES COMPILE_OPTIONS "${CUR_OPTS}")
endif()
add_subdirectory(MxpiMotSimpleSortV1)
add_subdirectory(MxpiMotSimpleSortV2)
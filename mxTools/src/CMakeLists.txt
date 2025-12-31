if(COVERAGE)
  add_compile_options(-fprofile-arcs -ftest-coverage)
endif()
set(test_deps)
if(COVERAGE)
  list(APPEND test_deps gcov)
endif()
set(MXTOOLS_COMMON_DEP_LIBS
    glib-2.0
    gstreamer-1.0
    gobject-2.0
    gstbase-1.0
    gmodule-2.0
    mxbase
    ${test_deps})
add_subdirectory(include)
add_subdirectory(module)
install(DIRECTORY include/MxTools DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
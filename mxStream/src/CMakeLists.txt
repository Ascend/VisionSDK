if(COVERAGE)
    add_compile_options(-fprofile-arcs -ftest-coverage)
endif()
add_subdirectory(module)
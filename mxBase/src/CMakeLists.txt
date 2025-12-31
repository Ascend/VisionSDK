add_subdirectory(mxbase)
add_subdirectory(postprocess)
add_subdirectory(operators)
install(
    DIRECTORY postprocess/include
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/MxBase/postprocess
)
install(
    DIRECTORY mxbase/include/MxBase
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
)
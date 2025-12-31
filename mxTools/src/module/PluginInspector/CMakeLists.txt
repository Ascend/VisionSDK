add_compile_options(-Wno-deprecated-declarations)
file(GLOB CUR_DIR_SRCS "*.cpp")
add_executable(InspectAllPlugins ${CUR_DIR_SRCS})
target_link_libraries(InspectAllPlugins glog acl_op_compiler c_sec
                      ${MXTOOLS_COMMON_DEP_LIBS})
install(TARGETS InspectAllPlugins
        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/ PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_WRITE GROUP_EXECUTE)

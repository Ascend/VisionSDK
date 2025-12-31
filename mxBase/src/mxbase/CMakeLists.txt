# blk_set_selector(MXBASE_PUB_TYPE SRC)
add_library(mxbase SHARED)
add_subdirectory(module)
target_link_libraries(
  mxbase
  glog
  ascendcl
  acl_dvpp
  acl_cblas
  acl_dvpp_mpi
  opencv_world
  c_sec
  ${test_deps})
install(TARGETS mxbase DESTINATION  ${CMAKE_INSTALL_PREFIX}/lib)

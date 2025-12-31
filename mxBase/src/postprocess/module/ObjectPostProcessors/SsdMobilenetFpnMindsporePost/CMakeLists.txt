# blk_set_selector(MXBASE_PUB_TYPE SSDMIND  PYTHON POST) # pypost
add_library(SsdMobilenetFpn_MindsporePost SHARED SsdMobilenetFpnMindsporePost.cpp)
target_link_libraries(SsdMobilenetFpn_MindsporePost mxbase ${test_deps})
install(TARGETS SsdMobilenetFpn_MindsporePost DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)

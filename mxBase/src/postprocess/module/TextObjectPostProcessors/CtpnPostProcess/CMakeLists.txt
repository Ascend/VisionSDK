# blk_set_selector(MXBASE_PUB_TYPE CTPN PYTHON POST) # pypost
add_library(ctpnpostprocess SHARED CtpnPostProcess.cpp)
target_link_libraries(ctpnpostprocess mxbase ${test_deps})
install(TARGETS ctpnpostprocess DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)
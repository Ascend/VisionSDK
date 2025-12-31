# blk_set_selector(MXBASE_PUB_TYPE SSDMOBI  PYTHON POST) # pypost
add_library(ssdmobilenetv1fpnpostprocess SHARED SsdMobilenetv1FpnPostProcess.cpp)
target_link_libraries(ssdmobilenetv1fpnpostprocess mxbase ${test_deps})
install(TARGETS ssdmobilenetv1fpnpostprocess DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)


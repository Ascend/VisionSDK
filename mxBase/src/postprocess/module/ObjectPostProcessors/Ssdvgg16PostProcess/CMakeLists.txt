# blk_set_selector(MXBASE_PUB_TYPE SSDVGG PYTHON POST) # pypost
add_library(ssdvgg16postprocess SHARED Ssdvgg16PostProcess.cpp)
target_link_libraries(ssdvgg16postprocess mxbase ${test_deps})
install(TARGETS ssdvgg16postprocess DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)
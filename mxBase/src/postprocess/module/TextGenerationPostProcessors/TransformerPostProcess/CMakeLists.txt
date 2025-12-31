# blk_set_selector(MXBASE_PUB_TYPE TRANSFORM PYTHON POST) # pypost
add_library(transformerpostprocess SHARED TransformerPostProcess.cpp)
target_link_libraries(transformerpostprocess mxbase ${test_deps})
install(TARGETS transformerpostprocess DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)
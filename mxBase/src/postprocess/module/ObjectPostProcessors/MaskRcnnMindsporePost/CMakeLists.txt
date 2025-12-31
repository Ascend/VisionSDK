# blk_set_selector(MXBASE_PUB_TYPE MASKRC PYTHON POST) # pypost
add_library(maskrcnnmindsporepost SHARED MaskRcnnMindsporePost.cpp)
target_link_libraries(maskrcnnmindsporepost mxbase ${test_deps})
install(TARGETS maskrcnnmindsporepost DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)
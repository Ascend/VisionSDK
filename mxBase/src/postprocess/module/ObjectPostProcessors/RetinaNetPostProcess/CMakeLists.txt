# blk_set_selector(MXBASE_PUB_TYPE RETINANET PYTHON POST) # pypost
add_library(retinanetpostprocess SHARED RetinaNetPostProcess.cpp)
target_link_libraries(retinanetpostprocess mxbase ${test_deps})
install(TARGETS retinanetpostprocess DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)
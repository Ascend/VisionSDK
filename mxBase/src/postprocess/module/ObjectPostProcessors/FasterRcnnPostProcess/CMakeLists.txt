# blk_set_selector(MXBASE_PUB_TYPE FASTER PYTHON POST) # pypost
add_library(fasterrcnnpostprocess SHARED FasterRcnnPostProcess.cpp)
target_link_libraries(fasterrcnnpostprocess mxbase ${test_deps})
install(TARGETS fasterrcnnpostprocess DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)
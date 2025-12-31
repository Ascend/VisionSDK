# blk_set_selector(MXBASE_PUB_TYPE UNETMIND PYTHON POST) # pypost
add_library(unetmindsporepostprocess SHARED UNetMindSporePostProcess.cpp)
target_link_libraries(unetmindsporepostprocess mxbase ${test_deps})
install(TARGETS unetmindsporepostprocess DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)
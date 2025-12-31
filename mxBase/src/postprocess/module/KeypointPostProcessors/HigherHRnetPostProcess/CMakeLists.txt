# blk_set_selector(MXBASE_PUB_TYPE HIGHERHRNET PYTHON POST) # pypost
add_library(higherhrnetpostprocess SHARED HigherHRnetPostProcess.cpp)
target_link_libraries(higherhrnetpostprocess mxbase ${test_deps})
install(TARGETS higherhrnetpostprocess DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)
# blk_set_selector(MXBASE_PUB_TYPE CRNN PYTHON POST) # pypost
add_library(crnnpostprocess SHARED CrnnPostProcess.cpp)
target_link_libraries(crnnpostprocess mxbase ${test_deps})
install(TARGETS crnnpostprocess DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)

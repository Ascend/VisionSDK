# blk_set_selector(MXBASE_PUB_TYPE PSENET PYTHON POST) # pypost
add_library(psenetpostprocess SHARED PSENetPostProcess.cpp)
target_link_libraries(psenetpostprocess mxbase ${test_deps})
install(TARGETS psenetpostprocess DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)
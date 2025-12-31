# blk_set_selector(MXBASE_PUB_TYPE DEEPPOST PYTHON POST) # pypost
add_library(deeplabv3post SHARED Deeplabv3Post.cpp)
target_link_libraries(deeplabv3post mxbase ${test_deps})
install(TARGETS deeplabv3post DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)
# blk_set_selector(MXBASE_PUB_TYPE RESNET PYTHON POST) # pypost

add_library(resnet50postprocess SHARED Resnet50PostProcess.cpp)
target_link_libraries(resnet50postprocess mxbase ${test_deps})
install(TARGETS resnet50postprocess DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)
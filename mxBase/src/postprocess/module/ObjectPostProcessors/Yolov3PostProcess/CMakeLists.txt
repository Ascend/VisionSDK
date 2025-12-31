# blk_set_selector(MXBASE_PUB_TYPE YOLOV PYTHON POST) # pypost
add_library(yolov3postprocess SHARED Yolov3PostProcess.cpp)
target_link_libraries(yolov3postprocess mxbase ${test_deps})
install(TARGETS yolov3postprocess DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)

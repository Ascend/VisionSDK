# blk_set_selector(MXBASE_PUB_TYPE OPENPOSE PYTHON POST) # pypost
add_library(openposepostprocess SHARED OpenPosePostProcess.cpp)
target_link_libraries(openposepostprocess mxbase ${test_deps})
install(TARGETS openposepostprocess DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/modelpostprocessors)
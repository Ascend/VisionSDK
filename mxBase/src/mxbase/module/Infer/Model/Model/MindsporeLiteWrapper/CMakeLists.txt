# blk_set_selector(MXBASE_PUB_TYPE MSLITE) # mindsporelitewrapper
add_library(mindsporelitewrapper SHARED MindsporeLiteImp.cpp)
target_link_libraries(mindsporelitewrapper mxbase dl mindspore-lite ${test_deps})
install(TARGETS mindsporelitewrapper DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
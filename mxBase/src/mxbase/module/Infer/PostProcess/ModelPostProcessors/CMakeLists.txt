# The postprocess which work with the old plugin postprocess framework will be deprecated,
# no need to compile with -fprofile-arcs -ftest-coverage
if(BUILD_TESTS)
    get_directory_property(CUR_OPTS COMPILE_OPTIONS)
    list(REMOVE_ITEM CUR_OPTS -fprofile-arcs -ftest-coverage)
    set_directory_properties(PROPERTIES COMPILE_OPTIONS "${CUR_OPTS}")
endif()
# blk_set_selector(MXBASE_PUB_TYPE SRC) # 重载，该目录下的只参与SRC构建
add_subdirectory(AttributePostProcessors)
add_subdirectory(ClassPostProcessors)
add_subdirectory(ModelPostProcessorBase)
add_subdirectory(ObjectPostProcessors)
add_subdirectory(OcrPostProcessors)


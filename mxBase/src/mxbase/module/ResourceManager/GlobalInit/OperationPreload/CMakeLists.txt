add_subdirectory(OperationLoaders)
file(GLOB CUR_DIR_SRCS "*.cpp")
target_sources(mxbase PRIVATE ${CUR_DIR_SRCS})
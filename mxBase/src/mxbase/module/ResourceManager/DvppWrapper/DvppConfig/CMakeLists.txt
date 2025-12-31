add_subdirectory(Config)
add_subdirectory(Checker)
file(GLOB CUR_DIR_SRCS "*.cpp")
target_sources(mxbase PRIVATE ${CUR_DIR_SRCS})
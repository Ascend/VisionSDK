add_subdirectory(ErrorCode)
add_subdirectory(Log)
file(GLOB CUR_DIR_SRCS "*.cpp")
target_sources(mxbase PRIVATE ${CUR_DIR_SRCS})
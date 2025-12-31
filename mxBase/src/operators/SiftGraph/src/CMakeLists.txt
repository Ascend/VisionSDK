cmake_minimum_required(VERSION 3.14.1)
project(Mxbase_SiftGraph)

if(DEFINED ENV{ASCEND_HOME})
    set(ASCEND_HOME $ENV{ASCEND_HOME})
else()
    set(ASCEND_HOME /usr/local/Ascend)
    message(WARNING "ASCEND_HOME is not set, use default value ${ASCEND_HOME}")
endif()

set(ASCEND_TOOLKIT_LATEST_PATH ${ASCEND_HOME}/ascend-toolkit/latest)

set(common_compile_options
        -std=c++14
        -Wall
        )

set(common_compile_definitions
        _GLIBCXX_USE_CXX11_ABI=0
        )

############ main ############
add_executable(sift main.cpp)

target_compile_options(sift PRIVATE
        ${common_compile_options}
        )

target_compile_definitions(sift PRIVATE
        ${common_compile_definitions}
        )

target_include_directories(sift PRIVATE
        ${ASCEND_TOOLKIT_LATEST_PATH}/compiler/include
        ${ASCEND_TOOLKIT_LATEST_PATH}/opp/built-in/op_proto/inc
        ${ASCEND_TOOLKIT_LATEST_PATH}/compiler/include/graph
        ${ASCEND_TOOLKIT_LATEST_PATH}/compiler/include/ge
        ${ASCEND_TOOLKIT_LATEST_PATH}/compiler/include/parser
        ${ASCEND_TOOLKIT_LATEST_PATH}/aarch64-linux/include
        )

link_directories(${OPENSOURCE_DIR}/lib)

target_link_directories(sift PRIVATE
        ${ASCEND_TOOLKIT_LATEST_PATH}/aarch64-linux/lib64
        ${ASCEND_HOME}/driver/lib64
        ${ASCEND_HOME}/driver/lib64/driver
        )

target_link_libraries(sift PRIVATE
        -Wl,--build-id=none
        -Wl,--no-as-needed
        -Wl,--copy-dt-needed-entries
        mxbase
        glog
        ascend_hal
        graph
        ge_compiler
        fmk_parser
        -Wl,--as-needed
        gcov
        --coverage
        )

install(TARGETS sift DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES ${PROJECT_SOURCE_DIR}/../data/generate_sift_weights.py DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

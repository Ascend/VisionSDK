# CMake minimum version number requirements
cmake_minimum_required(VERSION 3.14.1)
project(tik_inplace)

if(DEFINED ENV{ASCEND_HOME_PATH})
    set(ASCEND_HOME_PATH $ENV{ASCEND_HOME_PATH})
else()
    set(ASCEND_HOME_PATH "/usr/local/Ascend/ascend-toolkit/latest")
endif()
 
 
add_custom_command(OUTPUT TIK_INPLACE
    COMMAND . ${ASCEND_HOME_PATH}/../set_env.sh && python3.11 ${PROJECT_SOURCE_DIR}/tensor_inplace.py ${CMAKE_CURRENT_BINARY_DIR}
)
 
add_custom_target(txt_2_bkg_u8.o ALL DEPENDS TIK_INPLACE)
add_custom_target(txt_2_bkg_fp16.o ALL DEPENDS TIK_INPLACE)
 
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kernel_meta/txt_2_bkg_u8.o DESTINATION ${CMAKE_INSTALL_PREFIX}/operators/kernel_meta/)
 
if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/kernel_meta/txt_2_bkg_fp16.o")
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kernel_meta/txt_2_bkg_fp16.o DESTINATION ${CMAKE_INSTALL_PREFIX}/operators/kernel_meta/)
else()
    message(STATUS "Cannot find op file in kernel_meta, find it in /tmp")
    install(FILES /tmp/txt_2_bkg_fp16.o DESTINATION ${CMAKE_INSTALL_PREFIX}/operators/kernel_meta/)
endif()
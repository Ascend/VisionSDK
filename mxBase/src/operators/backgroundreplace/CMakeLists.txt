# CMake minimum version number requirements
cmake_minimum_required(VERSION 3.14.1)
project(background_replace_op)

if(DEFINED ENV{ASCEND_HOME_PATH})
    set(ASCEND_HOME_PATH $ENV{ASCEND_HOME_PATH})
else()
    set(ASCEND_HOME_PATH "/usr/local/Ascend/ascend-toolkit/latest")
endif()
 
add_custom_command(OUTPUT BACKGROUND_REPLACE_OP
    COMMAND . ${ASCEND_HOME_PATH}/../set_env.sh && python3.11 ${PROJECT_SOURCE_DIR}/background_replace.py
)
add_custom_target(background_replace.o ALL DEPENDS BACKGROUND_REPLACE_OP)
 
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kernel_meta/background_replace_u8.o DESTINATION ${CMAKE_INSTALL_PREFIX}/operators/kernel_meta/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kernel_meta/background_replace_f16.o DESTINATION ${CMAKE_INSTALL_PREFIX}/operators/kernel_meta/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kernel_meta/background_replace_normalize_u8.o DESTINATION ${CMAKE_INSTALL_PREFIX}/operators/kernel_meta/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kernel_meta/background_replace_normalize_f16.o DESTINATION ${CMAKE_INSTALL_PREFIX}/operators/kernel_meta/)
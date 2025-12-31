# CMake minimum version number requirements
cmake_minimum_required(VERSION 3.14.1)
project(tik_caption)
 
if(DEFINED ENV{ASCEND_HOME_PATH})
    set(ASCEND_HOME_PATH $ENV{ASCEND_HOME_PATH})
else()
    set(ASCEND_HOME_PATH "/usr/local/Ascend/ascend-toolkit/latest")
endif()
 
add_custom_command(OUTPUT TIK_CAPTION
    COMMAND . ${ASCEND_HOME_PATH}/../set_env.sh && python3.11 ${PROJECT_SOURCE_DIR}/blend_img_caption.py
)
 
add_custom_target(blend_image_caption.o ALL DEPENDS TIK_CAPTION)
 
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kernel_meta/blend_image_caption.o DESTINATION ${CMAKE_INSTALL_PREFIX}/operators/kernel_meta/)
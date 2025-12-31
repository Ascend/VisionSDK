set(PLUGIN_NAME "demob")
set(TARGET_LIBRARY ${PLUGIN_NAME})

add_compile_options("-fPIE")
add_compile_options("-std=c++14")
add_compile_options("-fPIC")
add_compile_options("-Wno-deprecated-declarations")
add_compile_options("-DPLUGIN_NAME=${PLUGIN_NAME}")
add_link_options("-pie")

add_library(${TARGET_LIBRARY} SHARED DemoB.cpp)

target_link_libraries(${TARGET_LIBRARY} glib-2.0 gstreamer-1.0 gobject-2.0 gstbase-1.0 gmodule-2.0)
target_link_libraries(${TARGET_LIBRARY} plugintoolkit mxbase)

set(PLUGIN_NAME "demoa")
set(TARGET_LIBRARY ${PLUGIN_NAME})

add_compile_options("-DPLUGIN_NAME=${PLUGIN_NAME}")

add_library(${TARGET_LIBRARY} SHARED DemoA.cpp)
target_link_libraries(${TARGET_LIBRARY} glib-2.0 gstreamer-1.0 gobject-2.0 gstbase-1.0 gmodule-2.0)
target_link_libraries(${TARGET_LIBRARY} plugintoolkit mxbase)

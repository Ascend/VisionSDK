# Usage:
# assemble <curdir>'s subdirs to <result> list, except <exclude> list
#
# example:
#  below will assemble CMAKE_CURRENT_SOURCE_DIR's subdir to SUBDIRS
#  exclude directory that names "build | test"
#  SUBDIRS will be "mxpi_imagedecoder"
#
# CMAKE_CURRENT_SOURCE_DIR
# ├-- build
# ├-- test
# └-- mxpi_imagedecoder
#
#  set(EXCLUDEDIRS build test)
#  SUBDIRLIST_WITH_EXCLUDE(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR} "${EXCLUDEDIRS}")
MACRO(SUBDIRLIST_WITH_EXCLUDE result curdir exclude)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)

  FOREACH(child ${exclude})
    LIST(REMOVE_ITEM children ${child})
  ENDFOREACH()

  SET(dirlist "")

  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

cmake_minimum_required(VERSION 3.8)
project(hotot NONE)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

find_package(Gettext REQUIRED)

option(WITH_QT "Enable Qt Version" On)

find_program(INTLTOOL_MERGE intltool-merge)

if(NOT INTLTOOL_MERGE)
  message(FATAL_ERROR "intltool-merge required for i18n generation")
endif()

set(INTLTOOL_PO_DIR "${PROJECT_SOURCE_DIR}/po")

function(intltool_merge_translation infile outfile)
  add_custom_command(
    OUTPUT ${outfile}
    COMMAND LC_ALL=C ${INTLTOOL_MERGE} -d -u "${INTLTOOL_PO_DIR}"
    ${infile} ${outfile}
    DEPENDS ${infile})
endfunction()

# uninstall target
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

function(hotot_convert_flags __flags_var __defs_var)
  get_directory_property(_old_flags COMPILE_FLAGS)
  get_directory_property(_old_defs COMPILE_DEFINITIONS)
  set_directory_properties(PROPERTIES
    COMPILE_FLAGS ""
    COMPILE_DEFINITIONS "")
  add_definitions(${ARGN})
  get_directory_property(_new_flags COMPILE_FLAGS)
  get_directory_property(_new_defs COMPILE_DEFINITIONS)
  set_directory_properties(PROPERTIES
    COMPILE_FLAGS "${_old_flags}"
    COMPILE_DEFINITIONS "${_old_defs}")
endfunction()
if(NOT DEFINED LOCALEDIR)
  set(LOCALEDIR ${CMAKE_INSTALL_PREFIX}/share/locale)
endif()

##################### Qt Version ######################

if(WITH_QT)
  add_subdirectory(qt)
endif()

add_subdirectory(po)
add_subdirectory(misc)

project(hotot_qt VERSION 0.10.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT
  NAMES Qt6
  COMPONENTS
    Core
    Gui
    Widgets
    WebEngineCore
    WebEngineWidgets
    Sql
  REQUIRED
)
find_package(Qt${QT_VERSION_MAJOR}
  COMPONENTS
    Core
    Gui
    Widgets
    WebEngineCore
    WebEngineWidgets
    Sql
  REQUIRED
)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-sign-compare -Wno-unused-parameter -fvisibility=hidden ${CMAKE_CXX_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed ${CMAKE_SHARED_LINKER_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--as-needed ${CMAKE_MODULE_LINKER_FLAGS}")

configure_file(config.h.in config.h)

set(Hotot_QT_SRCS
  formpost.cpp
  hototrequest.cpp
  main.cpp
  mainwindow.cpp
  hototwebpage.cpp
  qttraybackend.cpp
  )

set(HOTOT_QT_LINKS
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTWEBKIT_LIBRARY}
  ${QT_QTNETWORK_LIBRARY}
  )

qt_add_executable(hotot-qt MANUAL_FINALIZATION ${Hotot_QT_SRCS})
target_link_libraries(hotot-qt
  PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::WebEngineCore
    Qt${QT_VERSION_MAJOR}::WebEngineWidgets
    Qt${QT_VERSION_MAJOR}::Sql
)
set_target_properties(hotot-qt PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER nilsding.org
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
qt_finalize_executable(hotot-qt)

install(TARGETS hotot-qt DESTINATION bin)
set(HOTOT_QT_BIN hotot-qt)
configure_file("${PROJECT_SOURCE_DIR}/../misc/hotot-qt.desktop.in.in"
  hotot-qt.desktop.in @ONLY)
intltool_merge_translation(
  "${CMAKE_CURRENT_BINARY_DIR}/hotot-qt.desktop.in"
  "${CMAKE_CURRENT_BINARY_DIR}/hotot-qt.desktop")
add_custom_target(desktopfile-qt ALL
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/hotot-qt.desktop")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/hotot-qt.desktop"
  DESTINATION share/applications/)

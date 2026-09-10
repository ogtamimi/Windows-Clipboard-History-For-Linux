cmake_minimum_required(VERSION 3.21)
project(ClipboardHistory VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Sql Widgets)
find_package(X11)

qt_add_executable(clipboard-history
    src/main.cpp
    src/clipboardmodel.cpp
    src/clipboardmodel.h
    src/globalhotkey.cpp
    src/globalhotkey.h
)
qt_add_qml_module(clipboard-history
    URI ClipboardHistory
    VERSION 1.0
    QML_FILES qml/Main.qml
)
target_link_libraries(clipboard-history PRIVATE
    Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2 Qt6::Sql Qt6::Widgets
)

if(X11_FOUND)
    target_compile_definitions(clipboard-history PRIVATE CLIPBOARD_HAS_X11)
    target_include_directories(clipboard-history PRIVATE ${X11_INCLUDE_DIR})
    target_link_libraries(clipboard-history PRIVATE ${X11_LIBRARIES})
    find_library(XTST_LIBRARY Xtst REQUIRED)
    target_link_libraries(clipboard-history PRIVATE ${XTST_LIBRARY})
endif()

set(CMAKE_INSTALL_SYSCONFDIR "/etc")
include(GNUInstallDirs)

install(TARGETS clipboard-history RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES resources/clipboard-history.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES resources/clipboard-history.desktop DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/xdg/autostart)
install(FILES resources/clipboard-history.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/clipboard-history RENAME copyright)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "clipboard-history")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VENDOR "Ogtamimi")
set(CPACK_PACKAGE_CONTACT "Ogtamimi <ogtamimi@users.noreply.github.com>")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/ogtamimi/clipboard-history")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A fast, Windows-style clipboard history manager for Linux (X11)")
set(CPACK_PACKAGE_DESCRIPTION "
Windows-style clipboard history manager that lives in the system tray.
A Ctrl+Alt+V popup lists recent text, images and files; click an entry to
paste it into the previously focused window, search history and pin
favorites.
X11 only -- Hotkey, paste injection and outside-click forwarding need X.org.")
set(CPACK_PACKAGE_FILE_NAME "clipboard-history_${PROJECT_VERSION}_amd64")
set(CPACK_DEBIAN_PACKAGE_NAME "clipboard-history")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Ogtamimi <ogtamimi@users.noreply.github.com>")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libqt6core6t64 (>= 6.4) | libqt6core6 (>= 6.4), \
libqt6gui6t64 (>= 6.4) | libqt6gui6 (>= 6.4), \
libqt6qml6 (>= 6.4), \
libqt6qmlmodels6 (>= 6.4), \
libqt6quick6 (>= 6.4), \
libqt6sql6t64 (>= 6.4) | libqt6sql6 (>= 6.4), \
libqt6widgets6t64 (>= 6.4) | libqt6widgets6 (>= 6.4), \
libqt6network6t64 (>= 6.4) | libqt6network6 (>= 6.4), \
libqt6dbus6t64 (>= 6.4) | libqt6dbus6 (>= 6.4), \
libqt6opengl6t64 (>= 6.4) | libqt6opengl6 (>= 6.4), \
libqt6sql6-sqlite, \
libqt6svg6, \
libx11-6, \
libxtst6, \
qml6-module-qtquick, \
qml6-module-qtquick-controls, \
qml6-module-qtquick-layouts")

include(CPack)
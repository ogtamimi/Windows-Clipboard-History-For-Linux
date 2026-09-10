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

include(GNUInstallDirs)
install(TARGETS clipboard-history RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES resources/clipboard-history.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES resources/clipboard-history.desktop DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/xdg/autostart)
install(FILES resources/clipboard-history.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
cmake_minimum_required(VERSION 3.19)
project(gui LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories("widgets")

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(MainWindow_SOURCES
    windows/MAIN/mainwindow.cpp
    windows/MAIN/mainwindow.h
    windows/MAIN/mainwindow.ui
)

qt_add_executable(gui
    WIN32 MACOSX_BUNDLE
    main.cpp

    ${MainWindow_SOURCES}
    utils/utils.h
    utils/utils.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/utils/lexbor/source)

add_library(lexbor STATIC IMPORTED)
set_target_properties(
    lexbor PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/lexbor/liblexbor_static.a
)

target_link_libraries(gui
    PRIVATE
         Qt::Core
         Qt::Widgets
         lexbor
)

install(TARGETS gui
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET gui
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

cmake_minimum_required(VERSION 4.3)

project(Mozaik)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

include_directories(src/backend src/ui)

add_executable(Mozaik
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/backend/hypr_client.cpp
    src/backend/hypr_client.h
    resources/resources.qrc
)

target_link_libraries(Mozaik PRIVATE
    Qt6::Core
    Qt6::Widgets
)

install(TARGETS Mozaik DESTINATION bin)
install(FILES resources/mozaik.desktop DESTINATION share/applications)
install(FILES resources/icons/mozaik.svg
        DESTINATION share/icons/hicolor/scalable/apps)

cmake_minimum_required(VERSION 4.3)

project(HyprMonitor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

include_directories(src/backend src/ui)

add_executable(HyprMonitor
    src/main.cpp
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/backend/hypr_client.cpp
    src/backend/hypr_client.h
    resources/resources.qrc
)

target_link_libraries(HyprMonitor PRIVATE
    Qt6::Core
    Qt6::Widgets
)

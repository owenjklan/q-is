cmake_minimum_required(VERSION 3.20)
project(q_is)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS
        Core
        Gui
        Network
        Widgets
        REQUIRED)

add_executable(q_is main.cpp QISWidget.hpp QISWidget.cpp TabbedResultWidget.cpp TabbedResultWidget.hpp)
target_link_libraries(q_is
        Qt5::Core
        Qt5::Gui
        Qt5::Network
        Qt5::Widgets
        )


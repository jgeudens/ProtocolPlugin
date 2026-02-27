cmake_minimum_required(VERSION 3.21)

project(ProtocolPlugin VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
)

qt_standard_project_setup()

add_subdirectory(Plugins)

set(PROJECT_SOURCES
        Scope/main.cpp
        Scope/mainwindow.cpp
        Scope/mainwindow.h
        Scope/mainwindow.ui
        Scope/PluginManager/PluginManager.cpp
)

qt_add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Scope
)

# add_subdirectory(tests)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    example_plugin
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

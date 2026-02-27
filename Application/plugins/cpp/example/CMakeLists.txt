cmake_minimum_required(VERSION 3.21)

project(example_plugin LANGUAGES CXX)


add_library(example_plugin SHARED
    ExamplePlugin.cpp
)

target_include_directories(example_plugin PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/ProtocolCore
)

target_link_libraries(example_plugin PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
)

add_executable(example_plugin_test
    ExamplePluginTest.cpp
)

target_include_directories(example_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/ProtocolCore
)

target_link_libraries(example_plugin_test PRIVATE
    example_plugin
    Qt${QT_VERSION_MAJOR}::Core
)

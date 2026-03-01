cmake_minimum_required(VERSION 3.21)

add_executable(example_plugin_test
    ExamplePluginTest.cpp
)

target_include_directories(example_plugin_test PRIVATE
    ${CMAKE_SOURCE_DIR}/PluginInterface
)

target_link_libraries(example_plugin_test PRIVATE
    example_plugin
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Test
)

add_test(NAME example_plugin_test COMMAND example_plugin_test)

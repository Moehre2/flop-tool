cmake_minimum_required(VERSION 3.26)

project(flop-tool DESCRIPTION "Show FLOP values of FlopAnalysis for SDFG JSON")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(_CMAKE_INSTALL_DEFAULT_COMPONENT_NAME ${CMAKE_INSTALL_DEFAULT_COMPONENT_NAME})
set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME runtime)

add_subdirectory(sdfglib)

set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME ${_CMAKE_INSTALL_DEFAULT_COMPONENT_NAME})

set(SOURCE_FILES
    src/flop_tool.cpp
    src/traverse.cpp
)

add_executable(flop-tool ${SOURCE_FILES})
target_include_directories(flop-tool PRIVATE ./include/)
target_link_libraries(flop-tool sdfglib)

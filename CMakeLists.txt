cmake_minimum_required(VERSION 4.0)
project(Vertix)

set(CMAKE_CXX_STANDARD 20)

set(INCLUDES_DIR ${CMAKE_SOURCE_DIR}/includes)
set(LIBRARIES_DIR ${CMAKE_SOURCE_DIR}/libraries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

link_directories(AFTER ${LIBRARIES_DIR})

add_subdirectory(Vertix)
add_subdirectory(Vertix.Engine)
add_subdirectory(Examples/D3D12GameDemo)
add_subdirectory(Examples/D3D12RenderPipelineDemo)
cmake_minimum_required(VERSION 4.0)
project(Vertix)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)

include(dependencies/Vertix.Dependencies.cmake)
include(cmake-modules/AddHlslShaders.cmake)
link_directories(AFTER ${VX_LIBRARIES})

add_subdirectory(Vertix)
add_subdirectory(Vertix.Engine)
add_subdirectory(Examples/D3D12GameDemo)
add_subdirectory(Examples/D3D12ImGuiDemo)
add_subdirectory(Examples/D3D12RaytracingDemo)

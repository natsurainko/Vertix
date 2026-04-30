cmake_minimum_required(VERSION 4.0)
project(Vertix)

set(CMAKE_CXX_STANDARD 20)

set(INCLUDES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/includes)
set(LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libraries)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin/)

link_directories(AFTER ${LIBRARIES_DIR})

add_library(d3d12 INTERFACE IMPORTED GLOBAL)
find_library(D3D12_LIB d3d12)
set_target_properties(d3d12 PROPERTIES
        INTERFACE_LINK_LIBRARIES      "${D3D12_LIB}"
        INTERFACE_INCLUDE_DIRECTORIES "${INCLUDES_DIR}/d3d12"
)

add_library(assimp SHARED IMPORTED GLOBAL)
set_target_properties(assimp PROPERTIES
        IMPORTED_IMPLIB               "${LIBRARIES_DIR}/assimp-vc145-mt.lib"
        IMPORTED_LOCATION             "${LIBRARIES_DIR}/assimp-vc145-mt.dll"
        INTERFACE_INCLUDE_DIRECTORIES "${INCLUDES_DIR}"
)

add_library(GameInput SHARED IMPORTED GLOBAL)
set_target_properties(GameInput PROPERTIES
        IMPORTED_IMPLIB               "${LIBRARIES_DIR}/GameInput.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${INCLUDES_DIR}"
)

add_library(DirectXTK12 STATIC IMPORTED GLOBAL)
set_target_properties(DirectXTK12 PROPERTIES
        IMPORTED_LOCATION_DEBUG       "${LIBRARIES_DIR}/DirectXTK12-vc145-MDd.lib"
        IMPORTED_LOCATION_RELEASE     "${LIBRARIES_DIR}/DirectXTK12-vc145-MD.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${INCLUDES_DIR}"
)

add_library(imgui STATIC IMPORTED GLOBAL)
set_target_properties(imgui PROPERTIES
        IMPORTED_LOCATION             "${LIBRARIES_DIR}/imgui_win32_dx12-vc145.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${INCLUDES_DIR}"
)

include(cmake-modules/AddHlslShaders.cmake)

add_subdirectory(Vertix)
add_subdirectory(Vertix.Engine)
add_subdirectory(Examples/D3D12GameDemo)
add_subdirectory(Examples/D3D12ImGuiDemo)
add_subdirectory(Examples/D3D12RaytracingDemo)

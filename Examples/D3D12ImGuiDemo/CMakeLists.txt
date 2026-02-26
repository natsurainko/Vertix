add_executable(D3D12ImGuiDemo WIN32
        src/Main.cpp
        src/MainWindow.cpp
        src/MainWindow.h
)

add_dependencies(D3D12ImGuiDemo Vertix)
add_dependencies(D3D12ImGuiDemo Vertix.Engine)

target_link_libraries(D3D12ImGuiDemo PRIVATE Vertix Vertix.Engine imgui_win32_dx12-vc145.lib)
target_include_directories(Vertix.Engine BEFORE PUBLIC ${INCLUDES_DIR})
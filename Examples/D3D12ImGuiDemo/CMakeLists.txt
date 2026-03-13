add_executable(D3D12ImGuiDemo WIN32
        src/Main.cpp
        src/MainWindow.cpp
        src/MainWindow.h
)

add_dependencies(D3D12ImGuiDemo Vertix Vertix.Engine)
target_link_libraries(D3D12ImGuiDemo PRIVATE Vertix Vertix.Engine imgui)

add_custom_command(TARGET D3D12ImGuiDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:D3D12ImGuiDemo>
        $<TARGET_FILE_DIR:D3D12ImGuiDemo>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLLs..."
)
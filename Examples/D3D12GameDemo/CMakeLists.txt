add_executable(D3D12GameDemo WIN32
        src/main.cpp
        src/DemoMainWindow.cpp
        src/DemoMainWindow.h
        src/SimpleMath.cpp
)

add_dependencies(D3D12GameDemo Vertix)
add_dependencies(D3D12GameDemo Vertix.Engine)

target_link_libraries(D3D12GameDemo PRIVATE Vertix Vertix.Engine)

add_custom_command(TARGET D3D12GameDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:D3D12GameDemo>/assets
        COMMENT "Copying assets folder to output folder.."
)
add_executable(D3D12RaytracingDemo WIN32
        src/Controlling/KeyboardControllerInput.h
        src/Controlling/MouseControllerInput.h
        src/Rendering/RenderContext.h
        src/Rendering/Passes/GeometryPass.cpp
        src/Rendering/Passes/GeometryPass.h
        src/Rendering/Passes/ImGuiPass.cpp
        src/Rendering/Passes/ImGuiPass.h
        src/Rendering/Passes/LightingPass.cpp
        src/Rendering/Passes/LightingPass.h
        src/Rendering/Passes/RayTracingShadowPass.cpp
        src/Rendering/Passes/RayTracingShadowPass.h
        src/MainWindow.cpp
        src/MainWindow.h
        src/Main.cpp
)

set(HLSL_SHADER_FILES
        shaders/GeometryPass.hlsl
        shaders/LightingPass.hlsl
        shaders/RaytracingShadowPass.hlsl
)

set_source_files_properties(shaders/GeometryPass.hlsl PROPERTIES
        ShaderModel  "6_6"
        VSMain       "VSMain"
        PSMain       "PSMain"
        ByteCodeName "GEOMETRY_PASS"
)
set_source_files_properties(shaders/LightingPass.hlsl PROPERTIES
        ShaderModel  "6_0"
        VSMain       "VSMain"
        PSMain       "PSMain"
        ByteCodeName "LIGHTING_PASS"
)
set_source_files_properties(shaders/RaytracingShadowPass.hlsl PROPERTIES
        ShaderModel  "6_6"
        ByteCodeName "RAYTRACING_SHADOW_PASS"
        DxrExports   "True"
)

add_hlsl_shaders(D3D12RaytracingDemo FILES ${HLSL_SHADER_FILES})
add_dependencies(D3D12RaytracingDemo Vertix Vertix.Engine)
target_include_directories(D3D12RaytracingDemo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
target_link_libraries(D3D12RaytracingDemo PRIVATE Vertix Vertix.Engine imgui)

add_custom_command(TARGET D3D12RaytracingDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:D3D12RaytracingDemo>
        $<TARGET_FILE_DIR:D3D12RaytracingDemo>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLLs..."
)
add_custom_command(TARGET D3D12RaytracingDemo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:D3D12RaytracingDemo>/assets
        COMMENT "Copying assets folder to output folder.."
)

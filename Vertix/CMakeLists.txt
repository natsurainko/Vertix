add_library(Vertix SHARED
        include/Exceptions/HResultException.h
        include/Graphics/DescriptorHeap.h
        include/Graphics/FrameCommandList.h
        include/Graphics/GraphicsCommandList.h
        include/Graphics/GraphicsDevice.h
        include/Graphics/ResourceUploadHeap.h
        include/Graphics/SwapChain.h
        include/Graphics/Buffers/VertexBuffer.h
        include/Graphics/Buffers/IndexBuffer.h
        include/Graphics/Buffers/ConstantBuffer.h
        include/Graphics/Buffers/ConstantBufferPageArray.h
        include/Graphics/Buffers/StructuredBuffer.h
        include/Hosting/GameApplication.h
        include/Hosting/GameApplicationBuilder.h
        include/Math/Vector2D.h
        include/Primitive/Vertex.h
        include/Primitive/Mesh.h
        include/Primitive/Model.h
        include/Primitive/Material.h
        include/Rendering/DepthStencilView.h
        include/Rendering/RenderTargetView.h
        include/Rendering/HlslShader.h
        include/Rendering/RenderPass.h
        include/Rendering/RenderPipeline.h
        include/Rendering/Texture.h
        include/Windowing/GameWindow.h
        include/Windowing/WindowOptions.h

        src/Graphics/DescriptorHeap.cpp
        src/Graphics/FrameCommandList.cpp
        src/Graphics/GraphicsCommandList.cpp
        src/Graphics/GraphicsDevice.cpp
        src/Graphics/SwapChain.cpp
        src/Graphics/Buffers/VertexBuffer.cpp
        src/Hosting/GameApplication.cpp
        src/Primitive/Mesh.cpp
        src/Primitive/Model.cpp
        src/Rendering/DepthStencilView.cpp
        src/Rendering/HlslShader.cpp
        src/Rendering/RenderTargetView.cpp
        src/Rendering/Texture.cpp
        src/Windowing/GameWindow.cpp
)

target_compile_definitions(Vertix PRIVATE UNICODE _UNICODE)
target_link_libraries(Vertix PUBLIC d3d12 d3dcompiler dxcompiler dxgi dxguid dwmapi)
target_link_libraries(Vertix PUBLIC
        $<$<CONFIG:Debug>: DirectXTK12-vc145-MDd.lib>
        $<$<CONFIG:Release>: DirectXTK12-vc145-MD.lib>
)

target_include_directories(Vertix BEFORE PUBLIC ${INCLUDES_DIR})
target_include_directories(Vertix PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (MSVC)
    set_target_properties(Vertix PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif ()
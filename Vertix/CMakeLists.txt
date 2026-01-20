add_library(Vertix SHARED
        include/Exceptions/HResultException.h
        include/Graphics/FrameCommandList.h
        include/Graphics/GraphicsCommandList.h
        include/Graphics/GraphicsDevice.h
        include/Graphics/SwapChain.h
        include/Graphics/TempGraphicsResourceHeap.h
        include/Graphics/Buffers/VertexBuffer.h
        include/Graphics/Buffers/IndexBuffer.h
        include/Graphics/Buffers/ConstantBuffer.h
        include/Hosting/GameApplication.h
        include/Hosting/GameApplicationBuilder.h
        include/Math/Vector2D.h
        include/Primitive/Vertex.h
        include/Primitive/Mesh.h
        include/Primitive/Model.h
        include/Rendering/DepthStencilView.h
        include/Rendering/HlslShader.h
        include/Windowing/GameWindow.h

        src/Graphics/FrameCommandList.cpp
        src/Graphics/GraphicsCommandList.cpp
        src/Graphics/GraphicsDevice.cpp
        src/Graphics/SwapChain.cpp
        src/Hosting/GameApplication.cpp
        src/Primitive/Mesh.cpp
        src/Primitive/Model.cpp
        src/Rendering/DepthStencilView.cpp
        src/Rendering/HlslShader.cpp
        src/Windowing/GameWindow.cpp
)

target_compile_definitions(Vertix PRIVATE UNICODE _UNICODE)
target_link_libraries(Vertix PUBLIC d3d12 dxgi dxguid d3dcompiler)

target_include_directories(Vertix BEFORE PUBLIC ${INCLUDES_DIR})
target_include_directories(Vertix PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (MSVC)
    set_target_properties(Vertix PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
    )
endif ()
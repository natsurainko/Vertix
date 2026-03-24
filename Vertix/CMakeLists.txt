add_library(Vertix SHARED)
set_target_properties(Vertix PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(Vertix PRIVATE Vertix_EXPORTS UNICODE _UNICODE)
target_sources(Vertix
    PRIVATE
        src/Graphics/DescriptorHeap.cpp
        src/Graphics/FrameCommandList.cpp
        src/Graphics/GraphicsCommandList.cpp
        src/Graphics/GraphicsDevice.cpp
        src/Graphics/SwapChain.cpp
        src/Graphics/Buffers/VertexBuffer.cpp
        src/Graphics/Buffers/IndexBuffer.cpp
        src/Hosting/GameApplication.cpp
        src/Primitive/Mesh.cpp
        src/Primitive/Model.cpp
        src/Rendering/DepthStencilView.cpp
        src/Rendering/HlslShader.cpp
        src/Rendering/RenderTargetView.cpp
        src/Windowing/GameWindow.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/VERTIX_EXPORT.h

            include/Dispatching/DispatcherQueue.hpp
            include/Exceptions/HResultException.h
            include/Graphics/DescriptorHeap.h
            include/Graphics/FrameCommandList.h
            include/Graphics/GraphicsCommandList.h
            include/Graphics/GraphicsDevice.h
            include/Graphics/ResourceUploadHeap.hpp
            include/Graphics/SwapChain.h
            include/Graphics/Buffers/VertexBuffer.h
            include/Graphics/Buffers/IndexBuffer.h
            include/Graphics/Buffers/ConstantBuffer.hpp
            include/Graphics/Buffers/ConstantBufferPageArray.hpp
            include/Graphics/Buffers/StructuredBuffer.hpp
            include/Helpers/FormatsHelper.h
            include/Hosting/GameApplication.h
            include/Hosting/GameApplicationBuilder.hpp
            include/Math/Vector2D.hpp
            include/Mixin/IFillConstants.h
            include/Mixin/IResourceUploading.h
            include/Pool/ResourceHandle.h
            include/Pool/ResourcePool.hpp
            include/Pool/DynamicResourcePool.h
            include/Pool/MaterialPool.hpp
            include/Pool/ModelPool.hpp
            include/Pool/TexturePool.hpp
            include/Primitive/Vertex.h
            include/Primitive/Mesh.h
            include/Primitive/Model.h
            include/Primitive/Material.h
            include/Primitive/Texture.h
            include/Rendering/DepthStencilView.h
            include/Rendering/RenderTargetView.h
            include/Rendering/HlslShader.h
            include/Rendering/RenderPass.hpp
            include/Rendering/RenderPipeline.hpp
            include/Windowing/GameWindow.h
)

target_include_directories(Vertix BEFORE PUBLIC ${INCLUDES_DIR})
target_link_libraries(Vertix PUBLIC d3d12 DirectXTK12 d3dcompiler dxcompiler dxgi dxguid dwmapi)

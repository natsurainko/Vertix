add_library(Vertix SHARED)
set_target_properties(Vertix PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(Vertix
    PRIVATE   "VERTIX_API=__declspec(dllexport)" VERTIX_D3D12_ENABLE_DEBUG_LAYER
    INTERFACE "VERTIX_API=__declspec(dllimport)"
)
target_precompile_headers(Vertix
    PRIVATE
        <windows.h>
        <wrl/client.h>
        <d3d12/d3dx12.h>
        "include/Vertix/Exceptions/HResultException.h"
        "include/Vertix/D3D12Interface.h"
)
target_sources(Vertix
    PRIVATE
        src/Dispatching/DispatcherQueue.cpp

        src/Graphics/Command/CommandAllocator.cpp
        src/Graphics/Command/CommandList.cpp
        src/Graphics/Command/CommandQueue.cpp

        src/Graphics/Descriptor/DescriptorHeap.cpp
        src/Graphics/Descriptor/DescriptorHeapSet.cpp
        src/Graphics/Descriptor/DescriptorRange.cpp

        src/Graphics/Hlsl/D3DCompiler.cpp
        src/Graphics/Hlsl/DXCompiler.cpp

        src/Graphics/GraphicsDevice.cpp

        src/Hosting/GameApplication.cpp
        src/Hosting/GameApplicationBuilder.cpp

        src/Primitive/Mesh.cpp
        src/Primitive/Model.cpp

        src/Rendering/Buffers/AccelerationStructure.cpp
        src/Rendering/Buffers/ConstantBuffer.cpp
        src/Rendering/Buffers/StructuredBuffer.cpp
        src/Rendering/Buffers/VertexBuffer.cpp
        src/Rendering/Buffers/IndexBuffer.cpp

        src/Rendering/Pipeline/RenderPipeline.cpp
        src/Rendering/Pipeline/RenderPipelineBuilder.cpp

        src/Rendering/RenderResource.cpp
        src/Rendering/RenderTexture.cpp

        src/Windowing/GameWindow.cpp
        src/Windowing/SwapChain.cpp
        src/Windowing/SwapChainBuffer.cpp

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
        include/Vertix/Dispatching/DispatcherQueue.h
        include/Vertix/Exceptions/HResultException.h

        include/Vertix/Graphics/GraphicsDevice.h
        include/Vertix/Graphics/ResourceUploadHeap.hpp
            include/Vertix/Graphics/Command/CommandAllocator.h
            include/Vertix/Graphics/Command/CommandList.h
            include/Vertix/Graphics/Command/CommandQueue.h
            include/Vertix/Graphics/Descriptor/IDescriptorAllocator.h
            include/Vertix/Graphics/Descriptor/DescriptorHandle.h
            include/Vertix/Graphics/Descriptor/DescriptorHeap.h
            include/Vertix/Graphics/Descriptor/DescriptorHeapSet.h
            include/Vertix/Graphics/Descriptor/DescriptorRange.h
            include/Vertix/Graphics/Descriptor/DescriptorView.h
            include/Vertix/Graphics/Hlsl/D3DCompiler.h
            include/Vertix/Graphics/Hlsl/DXCompiler.h

        include/Vertix/Hosting/GameApplication.h
        include/Vertix/Hosting/GameApplicationBuilder.h

        include/Vertix/Helpers/FormatsHelper.h
        include/Vertix/Helpers/MemoryHelper.h

        include/Vertix/Math/Vector2D.hpp

        include/Vertix/Mixin/IFillConstants.h
        include/Vertix/Mixin/IResourceUploading.h
        include/Vertix/Mixin/IPassBinding.h

        include/Vertix/Pool/ResourceHandle.h
        include/Vertix/Pool/ResourcePool.hpp
        include/Vertix/Pool/DynamicResourcePool.hpp
        include/Vertix/Pool/MaterialPool.hpp
        include/Vertix/Pool/ModelPool.hpp
        include/Vertix/Pool/TexturePool.hpp

        include/Vertix/Primitive/Vertex.h
        include/Vertix/Primitive/Mesh.h
        include/Vertix/Primitive/Model.h
        include/Vertix/Primitive/Material.h
        include/Vertix/Primitive/Texture.h

        include/Vertix/Rendering/RenderTexture.h
        include/Vertix/Rendering/RenderBuffer.h
        include/Vertix/Rendering/RenderResource.h
        include/Vertix/Rendering/RenderResourceUsage.h
            include/Vertix/Rendering/Buffers/AccelerationStructure.h
            include/Vertix/Rendering/Buffers/IndexBuffer.h
            include/Vertix/Rendering/Buffers/VertexBuffer.h
            include/Vertix/Rendering/Buffers/ConstantBuffer.h
            include/Vertix/Rendering/Buffers/StructuredBuffer.h
            include/Vertix/Rendering/Pipeline/RenderGraph.h
            include/Vertix/Rendering/Pipeline/RenderPass.h
            include/Vertix/Rendering/Pipeline/RenderPipeline.h
            include/Vertix/Rendering/Pipeline/RenderPipelineBuilder.h
            include/Vertix/Rendering/Pipeline/PassDeclaration.h

        include/Vertix/Windowing/GameWindow.h
        include/Vertix/Windowing/SwapChain.h
        include/Vertix/Windowing/SwapChainBuffer.h

        include/Vertix/D3D12Interface.h
)

target_include_directories(Vertix BEFORE PUBLIC ${VX_INCLUDES})
target_link_libraries(Vertix PUBLIC d3d12 DirectXTK12 d3dcompiler dxcompiler dxgi dxguid dwmapi)

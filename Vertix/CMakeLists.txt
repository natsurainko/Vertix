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
        src/Graphics/Raytracing/BottomLevelAccelerationStructure.cpp
        src/Graphics/Raytracing/TopLevelAccelerationStructure.cpp
        src/Hosting/GameApplication.cpp
        src/Primitive/Mesh.cpp
        src/Primitive/Model.cpp
        src/Rendering/HlslShader.cpp
        src/Rendering/RenderResource.cpp
        src/Windowing/GameWindow.cpp
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS include
        FILES
            include/Vertix/VERTIX_EXPORT.h

            include/Vertix/Dispatching/DispatcherQueue.hpp
            include/Vertix/Exceptions/HResultException.h
            include/Vertix/Graphics/DescriptorHeap.h
            include/Vertix/Graphics/FrameCommandList.h
            include/Vertix/Graphics/GraphicsCommandList.h
            include/Vertix/Graphics/GraphicsDevice.h
            include/Vertix/Graphics/ResourceUploadHeap.hpp
            include/Vertix/Graphics/SwapChain.h
            include/Vertix/Graphics/Buffers/VertexBuffer.h
            include/Vertix/Graphics/Buffers/IndexBuffer.h
            include/Vertix/Graphics/Buffers/ConstantBuffer.hpp
            include/Vertix/Graphics/Buffers/ConstantBufferPageArray.hpp
            include/Vertix/Graphics/Buffers/StructuredBuffer.hpp
            include/Vertix/Graphics/Raytracing/BottomLevelAccelerationStructure.h
            include/Vertix/Graphics/Raytracing/TopLevelAccelerationStructure.h
            include/Vertix/Helpers/FormatsHelper.h
            include/Vertix/Hosting/GameApplication.h
            include/Vertix/Hosting/GameApplicationBuilder.hpp
            include/Vertix/Math/Vector2D.hpp
            include/Vertix/Mixin/IFillConstants.h
            include/Vertix/Mixin/IResourceUploading.h
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
            include/Vertix/Rendering/HlslShader.h
            include/Vertix/Rendering/RenderTexture.hpp
            include/Vertix/Rendering/RenderResource.h
            include/Vertix/Rendering/RenderResourceAccessor.h
            include/Vertix/Rendering/RenderResourceView.h
            include/Vertix/Rendering/RenderResourceViewAllocator.hpp
            include/Vertix/Rendering/RenderResourceViewDesc.h
            include/Vertix/Rendering/Pipeline/RenderPass.h
            include/Vertix/Rendering/Pipeline/RenderPipeline.h
            include/Vertix/Rendering/Pipeline/RenderPipelineBuilder.h
            include/Vertix/Rendering/Pipeline/PassInitializationContext.h
            include/Vertix/Rendering/Pipeline/PassRequirementBuilder.h
            include/Vertix/Windowing/GameWindow.h
)

target_include_directories(Vertix BEFORE PUBLIC ${INCLUDES_DIR})
target_link_libraries(Vertix PUBLIC d3d12 DirectXTK12 d3dcompiler dxcompiler dxgi dxguid dwmapi)

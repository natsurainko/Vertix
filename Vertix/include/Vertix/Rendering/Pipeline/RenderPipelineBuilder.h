//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_RENDERPIPELINEBUILDER_H
#define VERTIX_RENDERPIPELINEBUILDER_H

#include <functional>
#include <memory>
#include <ranges>
#include <unordered_set>

#include "RenderGraph.h"
#include "RenderPipeline.h"
#include "Vertix/Rendering/RenderBuffer.h"
#include "Vertix/Rendering/RenderResource.h"
#include "Vertix/Rendering/Buffers/ConstantBuffer.hpp"
#include "Vertix/Rendering/Buffers/StructuredBuffer.hpp"

namespace Vertix {
    class RenderPipelineBuilder {
        using Op = PassResourceDeclaration::PassResourceOperation;
        using Md = PassResourceDeclaration::PassResourceUsingMethod;

        struct ResourceDeclaration {
            RenderResourceKind    resourceKind = RenderResourceKind::None;
            D3D12_RESOURCE_DESC   resourceDesc;
            D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            D3D12_CLEAR_VALUE     clearValue;

            bool resizable     = true;
            bool optimizeClear = true;

            RenderBufferUsage bufferUsage = RenderBufferUsage::None;
            size_t            bufferTDataSize = 0;
        };
    public:
        VERTIX_API explicit RenderPipelineBuilder(
            GraphicsDevice* graphicsDevice,
            FrameCommandList* frameCommandList);

        class BufferCollection {
        public:
            VERTIX_API void Add(
                const std::string &resourceName,
                const D3D12_RESOURCE_DESC &resourceDesc) const;

            template<typename T>
            void ConstantBuffer(
                const std::string &resourceName,
                const D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)) const
            {
                func(resourceName, ResourceDeclaration {
                    .resourceKind    = RenderResourceKind::Buffer,
                    .resourceDesc    = Vertix::ConstantBuffer<T>::DESC(),
                    .heapProps       = heapProps,
                    .resizable       = false,
                    .optimizeClear   = false,
                    .bufferUsage     = RenderBufferUsage::ConstantBuffer,
                    .bufferTDataSize = sizeof(T)
                });
            }

            template<typename T>
            void StructuredBuffer(
                const std::string &resourceName,
                const D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT)) const
            {
                func(resourceName, ResourceDeclaration {
                    .resourceKind    = RenderResourceKind::Buffer,
                    .resourceDesc    = Vertix::StructuredBuffer<T>::DESC(),
                    .heapProps       = heapProps,
                    .resizable       = false,
                    .optimizeClear   = false,
                    .bufferUsage     = RenderBufferUsage::StructuredBuffer,
                    .bufferTDataSize = sizeof(T)
                });
            }

        private:
            friend class RenderPipelineBuilder;
            std::function<void(const std::string&, const ResourceDeclaration&)> func;
        } Buffers;
        class TextureCollection {
        public:
            VERTIX_API void Add(
                const std::string &resourceName,
                const D3D12_RESOURCE_DESC &resourceDesc,
                bool resizable = true) const;

            VERTIX_API void Add(
                const std::string &resourceName,
                const D3D12_RESOURCE_DESC &resourceDesc,
                const D3D12_CLEAR_VALUE   &clearValue,
                bool resizable = true) const;

        private:
            friend class RenderPipelineBuilder;
            std::function<void(const std::string&, const ResourceDeclaration&)> func;
        } Textures;
        class PassCollection {
        public:
            template<RenderPassType TRenderPass, typename TDeclareFunc, typename... CtorArgs>
                requires std::invocable<TDeclareFunc, PassDeclarationBuilder<TRenderPass>&>
            void Add(
                TDeclareFunc&& declareFunc,
                CtorArgs&&... ctorArgs) noexcept
            {
                PassDeclarationBuilder<TRenderPass> builder;
                builder.Construct(ctorArgs...);
                declareFunc(builder);
                decl.emplace_back(builder.Build());
            }

        private:
            friend class RenderPipelineBuilder;
            std::vector<PassDeclaration> decl;
            std::set<std::type_index> registeredTypes;
        } Passes;

        struct SwapChainDeclaration {
            SwapChain*  ptr = nullptr;
            std::string resourceName;
            RenderResourceViewDesc resourceViewDesc = {
                .type = RenderResourceViewType::RenderTarget
            };
        } SwapChain = {};
        struct DescriptorDeclaration {
            UINT reservedRTVDescriptorCount = 0;
            UINT reservedDSVDescriptorCount = 0;
            UINT reservedSharedDescriptorCount = 0;

            std::function<void(const RenderResourceViewAllocator*)> onAllocatorCreated = nullptr;
        } Descriptors = {};

        VERTIX_API std::unique_ptr<RenderPipeline> Build();

    private:
        GraphicsDevice*   graphicsDevice   = nullptr;
        FrameCommandList* frameCommandList = nullptr;

        std::unordered_map<std::string, ResourceDeclaration> resourceDeclarations;

        class EndRenderPass : public RenderPass {
        public:
            void Initialize(ID3D12Device10* device) override {}
            void Execute(ID3D12GraphicsCommandList5* commandList) override {}
        };

        VERTIX_API void InitializePipelineGraph(RenderPipeline* renderPipeline);

        VERTIX_API void InitializePipelineResourceStates(
            RenderPipeline* renderPipeline,
            std::unordered_set<std::string> &usedResources) const;

        VERTIX_API void InitializePipelineResources(
            RenderPipeline* renderPipeline,
            const std::unordered_set<std::string>& usedResources);

        VERTIX_API void InitializePipelineResourceViews(RenderPipeline* renderPipeline) const;
        VERTIX_API void InitializePipelinePasses(RenderPipeline* renderPipeline) const;

        VERTIX_API static D3D12_RESOURCE_STATES GetWriterState(const PassResourceDeclaration &declaration);
        VERTIX_API static D3D12_RESOURCE_STATES GetReaderState(const PassResourceDeclaration &declaration);

        VERTIX_API static void GetResourceFlags(
            const RenderPipeline* renderPipeline,
            const std::string &resourceName,
            ResourceDeclaration &declaration);
    };
}

#endif //VERTIX_RENDERPIPELINEBUILDER_H

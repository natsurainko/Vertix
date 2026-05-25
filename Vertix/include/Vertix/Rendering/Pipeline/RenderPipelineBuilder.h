//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_RENDERPIPELINEBUILDER_H
#define VERTIX_RENDERPIPELINEBUILDER_H

#include <algorithm>
#include <functional>
#include <memory>
#include <ranges>
#include <unordered_set>

#include "RenderGraph.h"
#include "RenderPipeline.h"
#include "Vertix/Graphics/DescriptorHandle.h"
#include "Vertix/Rendering/RenderResource.h"
#include "Vertix/Rendering/RenderResourceUsage.h"
#include "Vertix/Rendering/Buffers/ConstantBuffer.hpp"
#include "Vertix/Rendering/Buffers/StructuredBuffer.hpp"

namespace Vertix {
    class RenderPipelineBuilder {
        using Md = PassResourceDeclaration::PassResourceUsingMethod;

        using ResourceFactoryMethod = std::function<std::unique_ptr<RenderResource>(
            ID3D12Device*,
            RenderResourceUsage,
            D3D12_RESOURCE_STATES)>;

        using ViewFactoryMethod = std::function<void(
            ID3D12Device*,
            DescriptorHandle,
            const std::unordered_map<std::string, std::unique_ptr<RenderResource>>&)>;

        struct ViewRegistry {
            RenderResourceUsage usage;
            std::optional<std::string> counterResource;
            DescriptorViewDesc viewDesc;
            ViewFactoryMethod factoryMethod;

            DescriptorHandle handle;
            std::vector<std::weak_ptr<IPassBinding>> bindings;
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
            void ConstantBuffer(const std::string &resourceName) const {
                registerResource(resourceName, [=](
                    ID3D12Device* device,
                    const RenderResourceUsage allUsages,
                    const D3D12_RESOURCE_STATES initialState)
                {
                    const auto heapProps = DeriveHeapProperties(allUsages);
                    auto desc = Vertix::ConstantBuffer<T>::DESC();
                    desc.Flags = DeriveResourceFlags(allUsages);

                    Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
                    ThrowIfFailed(device->CreateCommittedResource(
                        &heapProps,
                        D3D12_HEAP_FLAG_NONE,
                        &desc,
                        initialState,
                        nullptr,
                        IID_PPV_ARGS(&d3d12Resource)
                    ));

                    return std::make_unique<ConstantBufferBase>(d3d12Resource, initialState, sizeof(T));
                });
            }

            template<typename T>
            void StructuredBuffer(
                const std::string &resourceName,
                const uint32_t elementCount) const
            {
                registerResource(resourceName, [=](
                    ID3D12Device* device,
                    const RenderResourceUsage allUsages,
                    const D3D12_RESOURCE_STATES initialState)
                {
                    const auto name = resourceName;

                    Microsoft::WRL::ComPtr<ID3D12Resource> d3d12Resource;
                    Microsoft::WRL::ComPtr<ID3D12Resource> stagingBuffer;
                    const auto desc = Vertix::StructuredBuffer<T>::DESC(elementCount);

                    {
                        const auto heapProps = DeriveHeapProperties(allUsages);
                        auto bufferDesc = desc;
                        bufferDesc.Flags = DeriveResourceFlags(allUsages);
                        ThrowIfFailed(device->CreateCommittedResource(
                            &heapProps,
                            D3D12_HEAP_FLAG_NONE,
                            &bufferDesc,
                            initialState,
                            nullptr,
                            IID_PPV_ARGS(&d3d12Resource)
                        ));
                    }

                    {
                        const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
                        ThrowIfFailed(device->CreateCommittedResource(
                            &heapProps,
                            D3D12_HEAP_FLAG_NONE,
                            &desc,
                            D3D12_RESOURCE_STATE_GENERIC_READ,
                            nullptr,
                            IID_PPV_ARGS(&stagingBuffer)));
                    }

                    return std::make_unique<StructuredBufferBase>(
                        d3d12Resource,
                        initialState,
                        stagingBuffer,
                        elementCount,
                        sizeof(T));
                });
            }

            template<typename T>
            void ConstantBufferArray(
                const std::string& resourceName,
                const uint32_t arraySize) const
            {
                for (uint32_t i = 0; i < arraySize; ++i) {
                    ConstantBuffer<T>(resourceName + "[" + std::to_string(i) + "]");
                }
            }

            template<typename T>
            void StructuredBufferArray(
                const std::string& resourceName,
                const uint32_t arraySize,
                const uint32_t elementCount) const
            {
                for (uint32_t i = 0; i < arraySize; ++i) {
                    StructuredBuffer<T>(resourceName + "[" + std::to_string(i) + "]", elementCount);
                }
            }

            void AddArray(
                const std::string& resourceName,
                const uint32_t arraySize,
                const D3D12_RESOURCE_DESC& resourceDesc) const
            {
                for (uint32_t i = 0; i < arraySize; ++i) {
                    Add(resourceName + "[" + std::to_string(i) + "]", resourceDesc);
                }
            }

        private:
            friend class RenderPipelineBuilder;
            std::function<void(const std::string&, ResourceFactoryMethod)> registerResource;
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
            std::function<void(const std::string&)> registerResizableResource;
            std::function<void(const std::string&, ResourceFactoryMethod)> registerResource;
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
                builder.registerView = [&](
                    const std::string& resourceName,
                    const RenderResourceUsage usage,
                    const std::optional<std::string>& counterResourceName,
                    const DescriptorViewDesc& viewDesc,
                    const std::weak_ptr<IPassBinding> &binding,
                    const ViewFactoryMethod& factoryMethod)
                {
                    auto &registries = viewRegistries[resourceName];
                    auto it = std::ranges::find_if(registries, [&](const ViewRegistry& r) {
                        return (r.usage == usage || ((r.usage == RenderResourceUsage::DepthRead || r.usage == RenderResourceUsage::DepthWrite) && (usage == RenderResourceUsage::DepthRead || usage == RenderResourceUsage::DepthWrite))) &&
                               r.counterResource == counterResourceName &&
                               r.viewDesc == viewDesc;
                    });

                    if (it != registries.end()) {
                        it->bindings.push_back(binding);
                        return;
                    }

                    auto &r = registries.emplace_back(usage, counterResourceName, viewDesc, factoryMethod);
                    r.bindings.push_back(binding);
                };
                declareFunc(builder);
                decl.emplace_back(builder.Build());
            }

        private:
            friend class RenderPipelineBuilder;

            explicit PassCollection(std::unordered_map<std::string, std::vector<ViewRegistry>> &viewRegistries) : viewRegistries(viewRegistries) {}

            std::vector<PassDeclaration> decl = {};
            std::unordered_map<std::string, std::vector<ViewRegistry>> &viewRegistries;
        } Passes { viewRegistries };

        struct SwapChainDeclaration {
            SwapChain*  ptr = nullptr;
            std::string swapChainResourceName;
            std::optional<D3D12_RENDER_TARGET_VIEW_DESC> swapChainViewDesc = std::nullopt;
        } SwapChain = {};
        struct DescriptorDeclaration {
            UINT reservedRTVDescriptorCount = 0;
            UINT reservedDSVDescriptorCount = 0;
            UINT reservedSamplerDescriptorCount = 0;
            UINT reservedSharedDescriptorCount = 0;

            std::function<void(const DescriptorHeapSet*)> onDescriptorSetCreated = nullptr;
        } Descriptors = {};

        VERTIX_API std::unique_ptr<RenderPipeline> Build();

    private:
        GraphicsDevice*   graphicsDevice   = nullptr;
        FrameCommandList* frameCommandList = nullptr;

        std::unordered_map<std::string, ResourceFactoryMethod>     resourceRegistries;
        std::unordered_map<std::string, std::vector<ViewRegistry>> viewRegistries;
        std::unordered_set<std::string> resizableTextures;

        class EndRenderPass : public RenderPass {
        public:
            void Initialize(ID3D12Device10* device) override {}
            void Execute(ID3D12GraphicsCommandList5* commandList) override {}
        };

        void InitializePipelineGraph(RenderPipeline* renderPipeline);

        void InitializePipelineResourceStates(
            RenderPipeline* renderPipeline,
            std::unordered_set<std::string> &usedResources) const;

        void InitializePipelineResources(
            RenderPipeline* renderPipeline,
            const std::unordered_set<std::string>& usedResources) const;

        void InitializePipelineResourceViews(
            RenderPipeline* renderPipeline,
            const std::unordered_set<std::string>& usedResources);

        void InitializePipelinePasses(RenderPipeline* renderPipeline) const;

        static void TraceDataflowEdges(
            PipelineGraphNode* startNode,
            PipelineGraph& graph,
            const std::unordered_map<std::string, std::vector<std::shared_ptr<PipelineGraphNode>>>& writerMap,
            std::unordered_set<PipelineGraphNode*>& visited);
    };
}

#endif //VERTIX_RENDERPIPELINEBUILDER_H

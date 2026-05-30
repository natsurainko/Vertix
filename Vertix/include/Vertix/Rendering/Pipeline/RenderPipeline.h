//
// Created by Natsurainko on 2026/5/7.
//

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <d3d12/d3dx12_core.h>

#include "RenderGraph.h"
#include "Vertix/D3D12Interface.h"
#include "Vertix/Math/Vector2D.hpp"

namespace Vertix {
    template <typename>
    class ConstantBuffer;

    template <typename>
    class StructuredBuffer;

    class RenderTexture;
    class DescriptorHeapSet;
    class SwapChain;

    class RenderPipeline {
        friend class RenderPipelineBuilder;

        using ViewFactoryMethod = std::function<void(
            ID3D12Device*,
            const DescriptorHandle &,
            const std::unordered_map<std::string, std::unique_ptr<RenderResource>> &)>;

        using SwapChainRTVDesc = std::optional<D3D12_RENDER_TARGET_VIEW_DESC>;

        std::unique_ptr<DescriptorHeapSet>                                                       descriptorHeapSet;
        std::unordered_map<std::string, std::unordered_map<DescriptorHandle, ViewFactoryMethod>> descriptorViews;

        PipelineGraph                   pipelineGraph;
        std::vector<PipelineGraphNode*> pipelineGraphNodes;
        std::vector<RenderPass*>        passes;

        std::unordered_map<std::string, std::unique_ptr<RenderResource>> resources;
        std::unordered_map<std::string, RenderResourceUsage>             resourcesAllUsages;
        std::unordered_map<std::string, D3D12_RESOURCE_STATES>           resourcesInitialStates;

        std::unordered_map<std::string, RenderTexture*> resizableTextures;

        SwapChain*       swapChain = nullptr;
        std::string_view swapChainResourceName;
        SwapChainRTVDesc swapChainViewDesc;

        DescriptorHandle nullHandle;

        std::unique_ptr<DescriptorView<RenderResourceUsage::RenderTarget>[]> frameRTVs;
        const DescriptorView<RenderResourceUsage::RenderTarget>*             currentFrameRTV = nullptr;

        std::vector<std::shared_ptr<IPassBinding>> frameInjectors;

        std::vector<std::vector<D3D12_RESOURCE_BARRIER>> prePassBarriers;
        std::vector<D3D12_RESOURCE_BARRIER>              postPassBarriers;

        CD3DX12_VIEWPORT viewport { 0.f, 0.f, 0.f, 0.f };
        CD3DX12_RECT     scissorRect {};

    public:
        VERTIX_API void Execute(D3D12Interface::CommandList* commandList);
        VERTIX_API void Resize(
            D3D12Interface::Device*   device,
            const Vector2D<uint32_t> &size);

        template <size_t I = 0, size_t N, typename T>
        void GetConstantBufferArray(
            const std::string &  resourceName,
            ConstantBuffer<T>* (&field)[N]) const {
            for (size_t i = 0; i < N; ++i) {
                field[i] = static_cast<ConstantBuffer<T>*>(resources.at(resourceName + "[" + std::to_string(i + I) + "]").get());
            }
        }

        template <typename T>
        [[nodiscard]] ConstantBuffer<T>* GetConstantBuffer(const std::string &resourceName) const { return static_cast<ConstantBuffer<T>*>(resources.at(resourceName).get()); }

        template <typename T>
        [[nodiscard]] StructuredBuffer<T>* GetStructuredBuffer(const std::string &resourceName) const { return static_cast<StructuredBuffer<T>*>(resources.at(resourceName).get()); }

        [[nodiscard]] RenderResource*       GetResource(const std::string &resourceName) const { return resources.at(resourceName).get(); }
        [[nodiscard]] DescriptorHeapSet*    GetDescriptorHeapSet() const noexcept { return descriptorHeapSet.get(); }
        [[nodiscard]] const D3D12_VIEWPORT* GetD3D12Viewport() const noexcept { return &viewport; }
        [[nodiscard]] const D3D12_RECT*     GetD3D12ScissorRect() const noexcept { return &scissorRect; }

    private:
        explicit RenderPipeline(SwapChain* swapChain);
        void     CompileBarriers();
    };
}

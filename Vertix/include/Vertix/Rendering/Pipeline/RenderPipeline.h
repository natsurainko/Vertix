//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_RENDERPIPELINE_H
#define VERTIX_RENDERPIPELINE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <d3d12/d3d12.h>
#include <d3d12/d3dx12_barriers.h>
#include <d3d12/d3dx12_core.h>
#include <wrl/client.h>

#include "RenderGraph.h"
#include "Vertix/Graphics/DescriptorHeapSet.h"
#include "Vertix/Graphics/FrameCommandList.h"
#include "Vertix/Graphics/SwapChain.h"
#include "Vertix/Math/Vector2D.hpp"
#include "Vertix/Rendering/RenderTexture.h"

namespace Vertix {
    template<typename>
    class ConstantBuffer;

    template<typename>
    class StructuredBuffer;

    class RenderPipeline {
        using ViewFactoryMethod = std::function<void(
            ID3D12Device*,
            const DescriptorHandle&,
            const std::unordered_map<std::string, std::unique_ptr<RenderResource>>&)>;
    public:
        VERTIX_API void Execute();
        VERTIX_API void Resize(const Vector2D<UINT>& size);

        template<typename T>
        [[nodiscard]] ConstantBuffer<T>* GetConstantBuffer(const std::string &resourceName) const { return static_cast<ConstantBuffer<T>*>(resources.at(resourceName).get()); }
        template<size_t I = 0, size_t N, typename T>
        void GetConstantBufferArray(const std::string &resourceName, ConstantBuffer<T>* (&field)[N]) const {
            for (size_t i = 0; i < N; ++i) {
                field[i] = static_cast<ConstantBuffer<T>*>(resources.at(resourceName + "[" + std::to_string(i + I) + "]").get());
            }
        }

        template<typename T>
        [[nodiscard]] StructuredBuffer<T>* GetStructuredBuffer(const std::string &resourceName) const { return static_cast<StructuredBuffer<T>*>(resources.at(resourceName).get()); }

        [[nodiscard]] RenderResource*       GetResource(const std::string &resourceName) const { return resources.at(resourceName).get(); }
        [[nodiscard]] DescriptorHeapSet*    GetDescriptorHeapSet() const noexcept { return descriptorHeapSet.get(); }
        [[nodiscard]] const D3D12_VIEWPORT* GetD3D12Viewport() const noexcept { return &viewport; }
        [[nodiscard]] const D3D12_RECT*     GetD3D12ScissorRect() const noexcept { return &scissorRect; }

    private:
        explicit RenderPipeline(
            GraphicsDevice* graphicsDevice,
            FrameCommandList* frameCommandList,
            SwapChain* swapChain)
        : graphicsDevice(graphicsDevice), frameCommandList(frameCommandList), swapChain(swapChain)
        {
            d3d12Device = graphicsDevice->GetD3D12Device().Get();
            d3d12CommandList = frameCommandList->GetD3D12GraphicsCommandList().Get();

            const auto frameSize = swapChain->GetFrameSize();
            viewport.Width = static_cast<float>(frameSize.X);
            viewport.Height = static_cast<float>(frameSize.Y);
            scissorRect.right = static_cast<LONG>(frameSize.X);
            scissorRect.bottom = static_cast<LONG>(frameSize.Y);
        }

        void CompileBarriers();

        friend class RenderPipelineBuilder;

        std::unique_ptr<DescriptorHeapSet> descriptorHeapSet;
        std::unordered_map<std::string, std::unordered_map<DescriptorHandle, ViewFactoryMethod>> descriptorViews;

        PipelineGraph                   pipelineGraph;
        std::vector<PipelineGraphNode*> pipelineGraphNodes;
        std::vector<RenderPass*>        passes;

        std::unordered_map<std::string, std::unique_ptr<RenderResource>> resources;
        std::unordered_map<std::string, RenderResourceUsage>             resourcesAllUsages;
        std::unordered_map<std::string, D3D12_RESOURCE_STATES>           resourcesInitialStates;

        std::unordered_map<std::string, RenderTexture*> resizableTextures;

        std::string                                  swapChainResourceName;
        std::optional<D3D12_RENDER_TARGET_VIEW_DESC> swapChainViewDesc;

        DescriptorHandle nullHandle;

        std::unique_ptr<DescriptorView<RenderResourceUsage::RenderTarget>[]> frameRTVs;
        const DescriptorView<RenderResourceUsage::RenderTarget>* currentFrameRTV = nullptr;

        std::vector<std::shared_ptr<IPassBinding>> frameInjectors;

        std::vector<std::vector<CD3DX12_RESOURCE_BARRIER>> prePassBarriers;
        std::vector<CD3DX12_RESOURCE_BARRIER>              postPassBarriers;

        GraphicsDevice*   graphicsDevice   = nullptr;
        FrameCommandList* frameCommandList = nullptr;
        SwapChain*        swapChain        = nullptr;

        CD3DX12_VIEWPORT viewport{0.f,0.f, 0.f, 0.f};
        CD3DX12_RECT     scissorRect{};

        ID3D12Device10* d3d12Device = nullptr;
        ID3D12GraphicsCommandList5* d3d12CommandList = nullptr;
    };
}

#endif //VERTIX_RENDERPIPELINE_H

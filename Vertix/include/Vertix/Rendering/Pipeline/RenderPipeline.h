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
#include "Vertix/Graphics/FrameCommandList.h"
#include "Vertix/Graphics/SwapChain.h"
#include "Vertix/Math/Vector2D.hpp"
#include "Vertix/Rendering/RenderResourceView.h"
#include "Vertix/Rendering/RenderResourceViewAllocator.h"
#include "Vertix/Rendering/RenderTexture.h"

namespace Vertix {
    class RenderPipeline {
    public:
        VERTIX_API void Execute();
        VERTIX_API void Resize(const Vector2D<UINT>& size);

        [[nodiscard]] RenderResourceViewAllocator* GetViewAllocator() const noexcept { return viewAllocator.get(); }
        [[nodiscard]] const D3D12_VIEWPORT*        GetD3D12Viewport() const noexcept { return &viewport; }
        [[nodiscard]] const D3D12_RECT*            GetD3D12ScissorRect() const noexcept { return &scissorRect; }

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
        void RecreateTextureView(
            const RenderTexture* resource,
            const RenderResourceViewDesc* viewDesc,
            const DescriptorHeapHandle &handle) const;

        struct BarrierRecipe {
            std::string           textureId;
            D3D12_RESOURCE_STATES beforeState;
            D3D12_RESOURCE_STATES afterState;
        };

        friend class RenderPipelineBuilder;

        std::unique_ptr<RenderResourceViewAllocator>                      viewAllocator;
        std::unordered_map<RenderResourceViewDesc*, DescriptorHeapHandle> views;

        PipelineGraph                   pipelineGraph;
        std::vector<PipelineGraphNode*> pipelineGraphNodes;
        std::vector<RenderPass*>        passes;

        std::unordered_map<std::string, std::unique_ptr<RenderResource>>      resources;
        std::unordered_map<std::string, std::vector<RenderResourceViewDesc*>> resourcesViewDescs;
        std::unordered_map<std::string, D3D12_RESOURCE_STATES>                resourcesInitialStates;

        std::unordered_map<std::string, RenderTexture*> resizableTextures;

        std::string            swapChainResourceName;
        RenderResourceViewDesc swapChainViewDesc;

        DescriptorHeapHandle nullHandle;

        std::vector<RenderResourceView<RenderResourceViewType::RenderTarget>> frameRTVs;
        const RenderResourceView<RenderResourceViewType::RenderTarget>*       frameRTV = nullptr;
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

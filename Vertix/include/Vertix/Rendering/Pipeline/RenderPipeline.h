//
// Created by Natsurainko on 2026/5/7.
//

#ifndef VERTIX_RENDERPIPELINE_H
#define VERTIX_RENDERPIPELINE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <d3d12/d3d12.h>
#include <wrl/client.h>

#include "RenderPass.h"
#include "Vertix/Graphics/FrameCommandList.h"
#include "Vertix/Graphics/SwapChain.h"
#include "Vertix/Math/Vector2D.hpp"
#include "Vertix/Rendering/RenderTexture.hpp"
#include "Vertix/Rendering/RenderResourceView.h"
#include "Vertix/Rendering/RenderResourceViewAllocator.hpp"

namespace Vertix {
    template<typename TContext>
    class RenderPipelineBuilder;

    template<typename TContext>
    class RenderPipeline {
    public:
        void Execute() {
            auto* currentFrameResource = swapChain->GetCurrentBuffer()->GetResource();
            const auto barrierToRT = CD3DX12_RESOURCE_BARRIER::Transition(
                currentFrameResource,
                D3D12_RESOURCE_STATE_PRESENT,
                D3D12_RESOURCE_STATE_RENDER_TARGET);
            const auto barrierToPresent = CD3DX12_RESOURCE_BARRIER::Transition(
                currentFrameResource,
                D3D12_RESOURCE_STATE_RENDER_TARGET,
                D3D12_RESOURCE_STATE_PRESENT);

            d3d12CommandList->RSSetViewports(1, &viewport);
            d3d12CommandList->RSSetScissorRects(1, &scissorRect);
            d3d12CommandList->ResourceBarrier(1, &barrierToRT);
            currentFrameRTV = &frameRTVs[swapChain->GetCurrentFrameIndex()];
            {
                for (UINT i = 0; i < passes.size(); ++i) {
                    auto &preBarriers = prePassBarriers[i];
                    if (!preBarriers.empty()) {
                        d3d12CommandList->ResourceBarrier(static_cast<UINT>(preBarriers.size()), preBarriers.data());
                    }

                    passes[i]->Execute(d3d12CommandList);
                }
                d3d12CommandList->ResourceBarrier(static_cast<UINT>(postPassBarriers.size()), postPassBarriers.data());
            }
            d3d12CommandList->ResourceBarrier(1, &barrierToPresent);
        }

        void Resize(const Vector2D<UINT>& size) {
            frameCommandList->WaitForCommand();

            viewport.Width = static_cast<float>(size.X);
            viewport.Height = static_cast<float>(size.Y);
            scissorRect.right = static_cast<LONG>(size.X);
            scissorRect.bottom = static_cast<LONG>(size.Y);

            swapChain->Resize(size);
            for (UINT i = 0; i < swapChain->GetFrameCount(); ++i) {
                frameRTVs[i].Reuse(d3d12Device, swapChain->GetBuffer(i)->GetResource());
            }

            for (auto& [texId, tex] : textures) {
                if (resizableTextureIds.contains(texId))
                    tex->Resize(d3d12Device, size);
            }

            for (auto& [viewId, view] : rtvViews) {
                if (const auto& texId = viewIdToTextureId.at(viewId); resizableTextureIds.contains(texId))
                    view.Reuse(d3d12Device, textures.at(texId)->GetResource());
            }
            for (auto& [viewId, view] : dsvViews) {
                if (const auto& texId = viewIdToTextureId.at(viewId); resizableTextureIds.contains(texId))
                    view.Reuse(d3d12Device, textures.at(texId)->GetResource());
            }
            for (auto& [viewId, view] : uavViews) {
                if (const auto& texId = viewIdToTextureId.at(viewId); resizableTextureIds.contains(texId))
                    view.Reuse(d3d12Device, textures.at(texId)->GetResource());
            }
            for (auto& [viewId, view] : srvViews) {
                if (const auto& texId = viewIdToTextureId.at(viewId); resizableTextureIds.contains(texId))
                    view.Reuse(d3d12Device, textures.at(texId)->GetResource());
            }

            RebuildBarriers();
        }

        void RebuildBarriers() {
            for (size_t i = 0; i < prePassBarrierRecipes.size(); ++i) {
                prePassBarriers[i].clear();
                for (auto& recipe : prePassBarrierRecipes[i]) {
                    prePassBarriers[i].push_back(CD3DX12_RESOURCE_BARRIER::Transition(textures.at(recipe.textureId)->GetResource(), recipe.beforeState, recipe.afterState));
                }
            }

            postPassBarriers.clear();
            for (auto& recipe : postPassBarrierRecipes) {
                postPassBarriers.push_back(CD3DX12_RESOURCE_BARRIER::Transition(textures.at(recipe.textureId)->GetResource(), recipe.beforeState, recipe.afterState));
            }
        }

        [[nodiscard]] TContext*                    GetRenderContext() noexcept { return renderContext; }
        [[nodiscard]] RenderResourceViewAllocator* GetViewAllocator() const noexcept { return viewAllocator.get(); }

    private:
        explicit RenderPipeline(
            GraphicsDevice* graphicsDevice,
            FrameCommandList* frameCommandList,
            SwapChain* swapChain,
            TContext* context)
        : graphicsDevice(graphicsDevice), frameCommandList(frameCommandList), swapChain(swapChain), renderContext(context)
        {
            d3d12Device = graphicsDevice->GetD3D12Device().Get();
            d3d12CommandList = frameCommandList->GetD3D12GraphicsCommandList().Get();

            const auto frameSize = swapChain->GetFrameSize();
            viewport.Width = static_cast<float>(frameSize.X);
            viewport.Height = static_cast<float>(frameSize.Y);
            scissorRect.right = static_cast<LONG>(frameSize.X);
            scissorRect.bottom = static_cast<LONG>(frameSize.Y);
        }

        struct BarrierRecipe {
            std::string           textureId;
            D3D12_RESOURCE_STATES beforeState;
            D3D12_RESOURCE_STATES afterState;
        };

        friend class RenderPipelineBuilder<TContext>;

        std::unordered_map<std::string, std::string> viewIdToTextureId;
        std::unordered_set<std::string>              resizableTextureIds;
        std::unordered_map<std::string, std::unique_ptr<RenderTextureBase>> textures;

        std::unique_ptr<RenderResourceViewAllocator> viewAllocator;
        std::unordered_map<std::string, RenderResourceView<RenderTarget>>    rtvViews;
        std::unordered_map<std::string, RenderResourceView<DepthStencil>>    dsvViews;
        std::unordered_map<std::string, RenderResourceView<UnorderedAccess>> uavViews;
        std::unordered_map<std::string, RenderResourceView<ShaderResource>>  srvViews;

        std::vector<RenderResourceView<RenderTarget>> frameRTVs;
        const RenderResourceView<RenderTarget>* currentFrameRTV = nullptr;

        std::vector<std::unique_ptr<RenderPass<TContext>>> passes;

        std::vector<std::vector<BarrierRecipe>> prePassBarrierRecipes;
        std::vector<BarrierRecipe>              postPassBarrierRecipes;
        std::vector<std::vector<D3D12_RESOURCE_BARRIER>> prePassBarriers;
        std::vector<D3D12_RESOURCE_BARRIER>              postPassBarriers;

        GraphicsDevice*   graphicsDevice   = nullptr;
        FrameCommandList* frameCommandList = nullptr;
        SwapChain*        swapChain        = nullptr;
        TContext*         renderContext    = nullptr;

        CD3DX12_VIEWPORT viewport{0.f,0.f, 0.f, 0.f};
        CD3DX12_RECT     scissorRect{};

        ID3D12Device10* d3d12Device = nullptr;
        ID3D12GraphicsCommandList5* d3d12CommandList = nullptr;
    };
}

#endif //VERTIX_RENDERPIPELINE_H

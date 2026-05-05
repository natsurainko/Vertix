//
// Created by Natsurainko on 2026/3/28.
//

#include "RenderPipelineImp.h"

#include "Passes/GeometryPass.h"
#include "Passes/ImGuiPass.h"
#include "Passes/LightingPass.h"
#include "Passes/RayTracingShadowPass.h"
#include "Vertix/Graphics/SwapChain.h"
#include "Vertix/Windowing/GameWindow.h"

RenderPipelineImp::RenderPipelineImp(
    Vertix::GraphicsDevice* graphicsDevice,
    Vertix::FrameCommandList* commandList,
    Vertix::GameWindow* gameWindow): RenderPipeline(graphicsDevice, commandList->GetD3D12GraphicsCommandList())
{
    swapChain = gameWindow->GetSwapChain();
    window = gameWindow;
    frameCommandList = commandList;

    renderContext = new RenderContext(graphicsDevice);
    renderContext->SetWindowSize(window->GetWindowSize());

    constexpr auto rtvDesc = D3D12_RENDER_TARGET_VIEW_DESC {
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
    };
    for (UINT i = 0; i < swapChain->GetFrameCount(); ++i) {
        renderContext->renderTargetViews[i] = renderContext->renderTextureAllocator->CreateRenderTargetView(swapChain->GetRenderTarget(i), &rtvDesc);
    }

    // release after command list executed
    Vertix::ResourceUploadHeap resourceUploadHeap {};
    frameCommandList->BeginCommand(nullptr);
    renderContext->fullScreenVertex = std::unique_ptr<Vertix::VertexBuffer>(Vertix::VertexBuffer::CreateFullScreenRect(graphicsDevice, frameCommandList, resourceUploadHeap));
    frameCommandList->EndCommand();
    frameCommandList->WaitForCommand();

    CreateAddPass<GeometryPass>();
    CreateAddPass<RayTracingShadowPass>();
    CreateAddPass<LightingPass>(swapChain);
    CreateAddPass<ImGuiPass>(window);
}

void RenderPipelineImp::Execute() {
    if (window->GetWindowState() == Vertix::Minimized) return;

    renderContext->currentRenderTargetView = &renderContext->renderTargetViews[swapChain->GetCurrentFrameIndex()];
    const auto scopedTransition = swapChain->GetCurrentFrameRenderTarget()->ScopedTransition(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    {
        commandList->RSSetViewports(1, &renderContext->viewport);
        commandList->RSSetScissorRects(1, &renderContext->scissorRect);
        RenderPipeline::Execute();
    }
}

void RenderPipelineImp::Resize(const Vertix::Vector2D<unsigned> &size) {
    frameCommandList->WaitForCommand();
    renderContext->SetWindowSize(size);

    swapChain->Resize(size);
    for (UINT i = 0; i < swapChain->GetFrameCount(); ++i) {
        renderContext->renderTargetViews[i].Reuse(graphicsDevice->GetD3D12Device(), swapChain->GetRenderTarget(i)->GetResource());
    }

    for (const auto &pass : renderPasses) {
        pass->Resize(size);
    }
}

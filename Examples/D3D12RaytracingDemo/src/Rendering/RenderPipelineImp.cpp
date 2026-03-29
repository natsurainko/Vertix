//
// Created by Natsurainko on 2026/3/28.
//

#include "RenderPipelineImp.h"

#include "Graphics/SwapChain.h"
#include "Passes/GeometryPass.h"
#include "Passes/ImGuiPass.h"
#include "Passes/LightingPass.h"
#include "Passes/RayTracingShadowPass.h"
#include "Windowing/GameWindow.h"

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

    const auto rtvResource = swapChain->GetCurrentFrameRenderTargetResource().Get();
    renderContext->currentFrameBuffer = rtvResource;

    const auto toRTBarrier = CD3DX12_RESOURCE_BARRIER::Transition(rtvResource, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    const auto toPresBarrier = CD3DX12_RESOURCE_BARRIER::Transition(rtvResource, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    commandList->RSSetViewports(1, &renderContext->viewport);
    commandList->RSSetScissorRects(1, &renderContext->scissorRect);
    commandList->ResourceBarrier(1, &toRTBarrier);
    RenderPipeline::Execute();
    commandList->ResourceBarrier(1, &toPresBarrier);
}

void RenderPipelineImp::Resize(const Vertix::Vector2D<unsigned> &size) {
    frameCommandList->WaitForCommand();
    swapChain->Resize(size);
    renderContext->SetWindowSize(size);

    for (const auto &pass : renderPasses) {
        pass->Resize(size);
    }
}

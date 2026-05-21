//
// Created by Natsurainko on 2026/3/28.
//

#include "MainWindow.h"

#include <filesystem>
#include <Vertix/Rendering/Pipeline/RenderPipelineBuilder.h>
#include <Vertix.Engine/Content/ModelLoader.h>

#include "Rendering/Passes/GeometryPass.h"
#include "Rendering/Passes/ImGuiPass.h"
#include "Rendering/Passes/LightingPass.h"
#include "Rendering/Passes/RayTracingShadowPass.h"

void MainWindow::BuildRenderPipeline() {
    const auto windowSize = GetWindowSize();

    renderContext = std::make_unique<RenderContext>(graphicsDevice, frameCommandList);
    renderContext->SetWindowSize(windowSize);

    Vertix::RenderPipelineBuilder builder { graphicsDevice, frameCommandList};
    {
        // Configure SwapChain
        builder.SwapChain.ptr = swapChain;
        builder.SwapChain.swapChainResourceName = "SwapChainBackBuffer";

        builder.Descriptors.reservedSharedDescriptorCount = 1;
        builder.Descriptors.onDescriptorSetCreated = [&](const Vertix::DescriptorHeapSet* heapSet) {
            renderContext->sharedDescriptorHeap = (*heapSet)[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
        };

        builder.Textures.Add("GBuffer.PositionDepth", CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_R32G32B32A32_FLOAT, VERTIX_VECTOR2D_EXPAND(windowSize)), D3D12_CLEAR_VALUE{ .Format = DXGI_FORMAT_R32G32B32A32_FLOAT, .Color = { 0.0f, 0.0f, 0.0f, 0.0f }});
        builder.Textures.Add("GBuffer.NormalRoughness", CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_R32G32B32A32_FLOAT, VERTIX_VECTOR2D_EXPAND(windowSize)), D3D12_CLEAR_VALUE{ .Format = DXGI_FORMAT_R32G32B32A32_FLOAT, .Color = { 0.0f, 0.0f, 0.0f, 0.0f }});
        builder.Textures.Add("GBuffer.Depth", CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_D32_FLOAT, VERTIX_VECTOR2D_EXPAND(windowSize)), D3D12_CLEAR_VALUE{ .Format = DXGI_FORMAT_D32_FLOAT, .DepthStencil = { 1.0f, 0 }});
        builder.Textures.Add("RT.ShadowMask", CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_R32G32B32A32_FLOAT, VERTIX_VECTOR2D_EXPAND(windowSize)));

        builder.Buffers.ConstantBuffer<FrameConstants>("Constants.Frame");
        builder.Buffers.ConstantBuffer<LightConstants>("Constants.Light");

        builder.Passes.Add<GeometryPass>([](auto &builder) { builder
            .Read("Constants.Frame", &GeometryPass::frameConstants, Vertix::RenderResourceUsage::ConstantBuffer)
            .Write("GBuffer.PositionDepth", &GeometryPass::gPositionDepthRTV)
            .Write("GBuffer.NormalRoughness", &GeometryPass::gNormalRoughnessRTV)
            .Write("GBuffer.Depth", &GeometryPass::gDepthDSV)
            .SideEffect();
        }, renderContext.get());

        builder.Passes.Add<RayTracingShadowPass>([](auto &builder) { builder
            .Read("Constants.Light", &RayTracingShadowPass::lightConstantsAddress, Vertix::RenderResourceUsage::ConstantBuffer)
            .Read("GBuffer.PositionDepth", &RayTracingShadowPass::gPositionDepthSRV)
            .Read("GBuffer.NormalRoughness", &RayTracingShadowPass::gNormalRoughnessSRV)
            .Write("RT.ShadowMask", &RayTracingShadowPass::shadowMaskUAV);
        }, renderContext.get());

        builder.Passes.Add<LightingPass>([](auto &builder) { builder
            .Read("RT.ShadowMask", &LightingPass::shadowMaskSRV)
            .Write("SwapChainBackBuffer", &LightingPass::currentFrameRTV);
        }, renderContext.get());

        builder.Passes.Add<ImGuiPass>([](auto &builder) { builder
            .Write("SwapChainBackBuffer", &ImGuiPass::currentFrameRTV)
            .template DependsAfter<LightingPass>();
        }, this, renderContext.get());
    }
    renderPipeline = builder.Build();
    renderContext->frameConstantsBuffer = renderPipeline->GetConstantBuffer<FrameConstants>("Constants.Frame");
    renderContext->lightConstantsBuffer = renderPipeline->GetConstantBuffer<LightConstants>("Constants.Light");
}

void MainWindow::OnInitialize() {
    BuildRenderPipeline();
    imGuiIO = &ImGui::GetIO();

    defaultPositionController.AttachObject(&renderContext->perspectiveCamera);
    defaultRotationController.AttachObject(&renderContext->perspectiveCamera);

    defaultPositionController.Speed *= 3.0;
    defaultRotationController.Sensitivity *= 1.5;

    graphicsDevice->CreateCommandQueue(copyCommandQueue, { .Type = D3D12_COMMAND_LIST_TYPE_COPY, .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE });
    graphicsDevice->CreateCommandQueue(computeCommandQueue, { .Type = D3D12_COMMAND_LIST_TYPE_COMPUTE, .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE });

    Vertix::Engine::ModelAsyncLoader modelAsyncLoader {&renderContext->modelPool, graphicsDevice, copyCommandQueue, computeCommandQueue, nullptr, true};
    modelAsyncLoader.LoadModelAsync("assets/block.fbx", {}, [
        modelPool = &renderContext->modelPool,
        sceneObjects = &renderContext->sceneObjects
    ] (const Vertix::ModelHandle handle) -> void {
        auto* model = modelPool->Get(handle);
        auto* sceneObject = sceneObjects->emplace_back(std::make_shared<Vertix::Engine::SceneObject3D>()).get();

        sceneObject->SceneModel = model;
        sceneObject->SetScale(model->Transformation.Scale);
        sceneObject->SetPosition({0, 0.5f, 0});
        sceneObject->SetOrientation(model->Transformation.Orientation);
    });
    modelAsyncLoader.LoadModelAsync("assets/plane.fbx", {}, [
        modelPool = &renderContext->modelPool,
        sceneObjects = &renderContext->sceneObjects
    ] (const Vertix::ModelHandle handle) -> void {
        auto* model = modelPool->Get(handle);
        auto* sceneObject = sceneObjects->emplace_back(std::make_shared<Vertix::Engine::SceneObject3D>()).get();

        sceneObject->SceneModel = model;
        sceneObject->SetScale(model->Transformation.Scale);
        sceneObject->SetPosition(model->Transformation.Position);
        sceneObject->SetOrientation(model->Transformation.Orientation);
    });
    modelAsyncLoader.ExecuteAsync(&dispatcherQueue, [&]() -> void { renderContext->BuildTLASAsync(computeCommandQueue); });
}

void MainWindow::OnRender(const double deltaTime) {
    if (GetWindowState() == Vertix::Minimized) return;

    dispatcherQueue.FlushQueue();
    renderContext->OnFrameUpdate();
    renderPipeline->Execute();
}

void MainWindow::OnUpdate(const double deltaTime) {
    if (!GetFocusingState()) return;

    if (!imGuiIO || !imGuiIO->WantCaptureMouse) mouseControllerInput.Update(deltaTime);
    if (!imGuiIO || !imGuiIO->WantCaptureKeyboard) keyboardControllerInput.Update(deltaTime);
}

void MainWindow::OnResized(const Vertix::Vector2D<unsigned> &size) {
    renderContext->SetWindowSize(size);
    renderPipeline->Resize(size);
}

void MainWindow::OnFocusLost() {
    if (mouseControllerInput.EnableRotating) {
        ShowCursor(true);
        mouseControllerInput.EnableRotating = false;
    }
}

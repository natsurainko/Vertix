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
    renderContext = std::make_unique<RenderContext>(graphicsDevice, frameCommandList);
    renderContext->SetWindowSize(GetWindowSize());
    Vertix::RenderPipelineBuilder renderPipelineBuilder {graphicsDevice, frameCommandList, renderContext.get()};

    // Configure SwapChain
    renderPipelineBuilder.SwapChain.swapChainPtr = swapChain;
    renderPipelineBuilder.SwapChain.frameRTVDesc = D3D12_RENDER_TARGET_VIEW_DESC {
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
        .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
    };

    renderPipelineBuilder.Descriptor.reservedSharedDescriptorCount = 1;

    constexpr auto colorClearValue = D3D12_CLEAR_VALUE { .Color = { 0.0f, 0.0f, 0.0f, 0.0f } };
    constexpr auto depthClearValue = D3D12_CLEAR_VALUE { .DepthStencil = { .Depth = 1.0f, .Stencil = 0} };

    renderPipelineBuilder.Textures.Add<Vertix::DrawColorSampleAccessor>("GBuffer.PositionDepth", CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_R32G32B32A32_FLOAT, VERTIX_VECTOR2D_EXPAND(renderContext->windowSize)), true, &colorClearValue);
    renderPipelineBuilder.Textures.Add<Vertix::DrawColorSampleAccessor>("GBuffer.NormalRoughness", CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_R32G32B32A32_FLOAT, VERTIX_VECTOR2D_EXPAND(renderContext->windowSize)), true, &colorClearValue);
    renderPipelineBuilder.Textures.Add<Vertix::DrawDepthSampleAccessor>("GBuffer.Depth", CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_D32_FLOAT, VERTIX_VECTOR2D_EXPAND(renderContext->windowSize)), true, &depthClearValue);
    renderPipelineBuilder.Textures.Add<Vertix::UnorderedAccessSampleAccessor>("RT.ShadowMask", CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_R32G32B32A32_FLOAT, VERTIX_VECTOR2D_EXPAND(renderContext->windowSize)), true);

    renderPipelineBuilder.Views.Add<Vertix::RenderTarget>("GBuffer.PositionDepth.RTV", "GBuffer.PositionDepth");
    renderPipelineBuilder.Views.Add<Vertix::RenderTarget>("GBuffer.NormalRoughness.RTV", "GBuffer.NormalRoughness");
    renderPipelineBuilder.Views.Add<Vertix::DepthStencil>("GBuffer.Depth.DSV", "GBuffer.Depth");
    renderPipelineBuilder.Views.Add<Vertix::ShaderResource>("GBuffer.PositionDepth.SRV", "GBuffer.PositionDepth");
    renderPipelineBuilder.Views.Add<Vertix::ShaderResource>("GBuffer.NormalRoughness.SRV", "GBuffer.NormalRoughness");

    renderPipelineBuilder.Views.Add<Vertix::UnorderedAccess>("RT.ShadowMask.UAV", "RT.ShadowMask");
    renderPipelineBuilder.Views.Add<Vertix::ShaderResource>("RT.ShadowMask.SRV", "RT.ShadowMask");

    renderPipelineBuilder.Passes.Add<GeometryPass>([](Vertix::PassRequirementBuilder &builder) {
        builder.RequireView<Vertix::RenderTarget>("GBuffer.PositionDepth.RTV", D3D12_RESOURCE_STATE_RENDER_TARGET);
        builder.RequireView<Vertix::RenderTarget>("GBuffer.NormalRoughness.RTV", D3D12_RESOURCE_STATE_RENDER_TARGET);
        builder.RequireView<Vertix::DepthStencil>("GBuffer.Depth.DSV", D3D12_RESOURCE_STATE_DEPTH_WRITE);
    });

    renderPipelineBuilder.Passes.Add<RayTracingShadowPass>([](Vertix::PassRequirementBuilder &builder) {
        builder.RequireView<Vertix::ShaderResource>("GBuffer.PositionDepth.SRV", D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        builder.RequireView<Vertix::ShaderResource>("GBuffer.NormalRoughness.SRV", D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        builder.RequireView<Vertix::UnorderedAccess>("RT.ShadowMask.UAV", D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    });

    renderPipelineBuilder.Passes.Add<LightingPass>([](Vertix::PassRequirementBuilder &builder) {
        builder.RequireView<Vertix::ShaderResource>("RT.ShadowMask.SRV", D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }, swapChain);

    renderPipelineBuilder.Passes.Add<ImGuiPass>([](auto &) {}, this);

    renderPipeline = renderPipelineBuilder.Build();
    renderPipeline->GetViewAllocator()->GetShaderResourceDescriptorHeap()->AllocDescriptorHandle(renderContext->tlasSrvHandle, renderContext->tlasSrvGpuHandle);
}

void MainWindow::OnInitialize() {
    BuildRenderPipeline();
    imGuiIO = &ImGui::GetIO();

    defaultPositionController.AttachObject(&renderContext->perspectiveCamera);
    defaultRotationController.AttachObject(&renderContext->perspectiveCamera);

    defaultPositionController.Speed *= 3.0;
    defaultRotationController.Sensitivity *= 1.5;

    graphicsDevice->CreateCommandQueue(copyCommandQueue, {
        .Type = D3D12_COMMAND_LIST_TYPE_COPY,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
    });
    graphicsDevice->CreateCommandQueue(computeCommandQueue, {
        .Type = D3D12_COMMAND_LIST_TYPE_COMPUTE,
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE
    });

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
    renderPipeline->Execute();
}

void MainWindow::OnUpdate(const double deltaTime) {
    if (!GetFocusingState()) return;

    if (!imGuiIO || !imGuiIO->WantCaptureMouse) mouseControllerInput.Update(deltaTime);
    if (!imGuiIO || !imGuiIO->WantCaptureKeyboard) keyboardControllerInput.Update(deltaTime);

    renderContext->UpdateFrameConstants();
    renderContext->UpdateLightConstants();
    renderContext->UpdateObjectConstants();
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

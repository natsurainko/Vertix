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

    renderPipelineBuilder.Textures.Add<Vertix::DrawColorSampleAccessor>("GBuffer.PositionDepth", CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_R32G32B32A32_FLOAT, VERTIX_VECTOR2D_EXPAND(renderContext->windowSize)), true);
    renderPipelineBuilder.Textures.Add<Vertix::DrawColorSampleAccessor>("GBuffer.NormalRoughness", CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_R32G32B32A32_FLOAT, VERTIX_VECTOR2D_EXPAND(renderContext->windowSize)), true);
    renderPipelineBuilder.Textures.Add<Vertix::DrawDepthSampleAccessor>("GBuffer.Depth", CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_D32_FLOAT, VERTIX_VECTOR2D_EXPAND(renderContext->windowSize)), true);
    renderPipelineBuilder.Textures.Add<Vertix::UnorderedAccessSampleAccessor>("RT.ShadowMask", CD3DX12_RESOURCE_DESC::Tex2D(
        DXGI_FORMAT_R32G32B32A32_FLOAT, VERTIX_VECTOR2D_EXPAND(renderContext->windowSize)), true);

    renderPipelineBuilder.Passes.Add<GeometryPass>([](Vertix::PassDeclarationBuilder &builder) {
        builder.DeclareWrite<Vertix::RenderTarget>("GBuffer.PositionDepth")
               .DeclareWrite<Vertix::RenderTarget>("GBuffer.NormalRoughness")
               .DeclareWrite<Vertix::DepthStencil>("GBuffer.Depth");
    });

    renderPipelineBuilder.Passes.Add<RayTracingShadowPass>([](Vertix::PassDeclarationBuilder &builder) {
        builder.DeclareRead<Vertix::ShaderResource>("GBuffer.PositionDepth")
               .DeclareRead<Vertix::ShaderResource>("GBuffer.NormalRoughness")
               .DeclareWrite<Vertix::UnorderedAccess>("RT.ShadowMask");
    });

    renderPipelineBuilder.Passes.Add<LightingPass>([](Vertix::PassDeclarationBuilder &builder) {
        builder.DeclareRead<Vertix::ShaderResource>("RT.ShadowMask");
    }, swapChain);

    renderPipelineBuilder.Passes.Add<ImGuiPass>([](auto &) {}, this);

    renderPipeline = renderPipelineBuilder.Build();
    renderContext->viewAllocator = renderPipeline->GetViewAllocator();
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

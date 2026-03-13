//
// Created by Natsurainko on 2025/12/23.
//

#include "DemoMainWindow.h"

#include <d3d12/d3dx12_barriers.h>

#include "Content/ModelImporter.h"
#include "Exceptions/HResultException.h"
#include "Graphics/FrameCommandList.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/SwapChain.h"

void DemoMainWindow::OnInitialize() {
    const auto &device = graphicsDevice->GetD3D12Device();
    const auto windowSize = GetWindowSize();

    {
        commandList = frameCommandList->GetD3D12GraphicsCommandList();
        frameCommandList->BeginCommand(nullptr);

        // release after command list executed
        Vertix::ResourceUploadHeap resourceUploadHeap;
        //if (Vertix::Engine::ModelImporter::TryLoadFromFile(cubeModel, "assets/models/block.fbx")) {
        //    cubeModel.UploadToGPU(graphicsDevice, frameCommandList, resourceUploadHeap);
        //}

        frameCommandList->EndCommand();
        frameCommandList->WaitForCommand();
    }

    {
        CD3DX12_ROOT_PARAMETER rootParameters[1];
        rootParameters[0].InitAsConstantBufferView(0);

        CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init(
            1,
            rootParameters,
            0,
            nullptr,
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
        );

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;

        ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
        ThrowIfFailed(device->CreateRootSignature(0,signature->GetBufferPointer(),signature->GetBufferSize(),IID_PPV_ARGS(&rootSignature)));
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
            .NumDescriptors = 1
        };
        ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&depthStencilDescriptorHeap)));

        dsvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(depthStencilDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
        depthStencilView = new Vertix::DepthStencilView(graphicsDevice,
            CD3DX12_RESOURCE_DESC::Tex2D(
                DXGI_FORMAT_D24_UNORM_S8_UINT,
                windowSize.X, windowSize.Y,
                1, 0, 1, 0,
                D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
            ),
            dsvHandle
        );
    }

    {
        vertexShader.Compile("VSMain", "vs_5_0");
        pixelShader.Compile("PSMain", "ps_5_0");

        D3D12_INPUT_ELEMENT_DESC inputElementDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertix::Vertex, Position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertix::Vertex, Normal), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

        // describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDesc, _countof(inputElementDesc) };
        psoDesc.pRootSignature = rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.GetShaderBlob());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.GetShaderBlob());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC2(D3D12_DEFAULT);
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        psoDesc.NumRenderTargets = 1;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleMask = UINT_MAX;
        ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
    }

    {
        perspectiveCamera.SetAspect(static_cast<float>(windowSize.X) / static_cast<float>(windowSize.Y));
        perspectiveCamera.Move({-5,0,0});
        perspectiveCamera.GetProjectionMatrix(projectionMatrix);
        perspectiveCamera.GetViewMatrix(viewMatrix);

        constantBuffer = new Vertix::ConstantBuffer<RootConstants>(graphicsDevice);
        FillConstantBuffer();
    }
}

void DemoMainWindow::OnUpdate(const double deltaTime) {
#ifndef NDEBUG
    frameCounter.IncrementCounter(deltaTime);
#endif

    if (!GetFocusingState()) return;

    if (mouseDevice.IsInitialized()) {
        mouseDevice.Update();

        if (mouseDevice.WasButtonReleased(GameInput::v3::GameInputMouseRightButton)
            && HitTest(mouseDevice.GetPosition().Cast<UINT>()) == HTCLIENT
            && enableRotating) {
            ShowCursor(true);
            enableRotating = false;
        }
        if (mouseDevice.WasButtonReleased(GameInput::v3::GameInputMouseLeftButton)
            && HitTest(mouseDevice.GetPosition().Cast<UINT>()) == HTCLIENT
            && !enableRotating) {
            ShowCursor(false);
            enableRotating = true;
        }

        if (enableRotating) {
            const Vertix::Vector2D<float> mouseDeltaOffset = -mouseDevice.GetDeltaOffset().Cast<float>();
            const DirectX::SimpleMath::Vector3 rotationOffset {mouseDeltaOffset.Y, mouseDeltaOffset.X, 0.0f};
            perspectiveCamera.Rotate(rotationOffset * 0.002);
            SetCursorCenterWindow();
        }
    }

    DirectX::SimpleMath::Vector3 movingOffset = DirectX::SimpleMath::Vector3::Zero;
    if (keyboardDevice.IsKeyPressed('W')) movingOffset.x += 1;
    if (keyboardDevice.IsKeyPressed('S')) movingOffset.x -= 1;
    if (keyboardDevice.IsKeyPressed(VK_SPACE)) movingOffset.y += 1;
    if (keyboardDevice.IsKeyPressed(VK_LSHIFT)) movingOffset.y -= 1;
    if (keyboardDevice.IsKeyPressed('D')) movingOffset.z += 1;
    if (keyboardDevice.IsKeyPressed('A')) movingOffset.z -= 1;
    perspectiveCamera.Move(movingOffset * static_cast<float>(deltaTime * 1.5));
    perspectiveCamera.GetViewMatrix(viewMatrix);
}

void DemoMainWindow::OnRender(const double deltaTime) {
    FillConstantBuffer();
    constexpr float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    const auto rtvResource = swapChain->GetCurrentFrameRenderTargetResource();
    const auto rtvHandle = swapChain->GetCurrentFrameRenderTargetHandle();

    const auto presentToRenderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        rtvResource.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    const auto renderTargetToPresentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        rtvResource.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);
    commandList->ResourceBarrier(1, &presentToRenderTargetBarrier);
    commandList->SetGraphicsRootSignature(rootSignature.Get());
    commandList->SetGraphicsRootConstantBufferView(0, constantBuffer->GetD3D12Resource()->GetGPUVirtualAddress());
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH , 1.0f, 0, 0, nullptr);
    commandList->SetPipelineState(pipelineState.Get());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cubeModel.Draw(commandList);
    commandList->ResourceBarrier(1, &renderTargetToPresentBarrier);
}

void DemoMainWindow::OnResized(const Vertix::Vector2D<UINT> &size) {
    GetD3D12ViewportRectSize(viewport, scissorRect);

    frameCommandList->WaitForCommand();
    swapChain->Resize(size);
    depthStencilView->Resize(size);

    perspectiveCamera.SetAspect(static_cast<float>(size.X) / static_cast<float>(size.Y));
    perspectiveCamera.GetProjectionMatrix(projectionMatrix);
}

void DemoMainWindow::OnFocusLost() {
    if (enableRotating) {
        ShowCursor(true);
        enableRotating = false;
    }
}

void DemoMainWindow::FillConstantBuffer() const {
    RootConstants constants = {};
    constants.WorldViewProjection = worldMatrix * viewMatrix * projectionMatrix;
    worldMatrix.Invert(constants.WorldInverseTranspose);
    constants.WorldInverseTranspose.Transpose(constants.WorldInverseTranspose);

    constantBuffer->Fill(constants);
}
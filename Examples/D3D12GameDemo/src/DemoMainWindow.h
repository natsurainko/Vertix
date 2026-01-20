//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_DEMOMAINWINDOW_H
#define VERTIX_DEMOMAINWINDOW_H

#include "Camera/PerspectiveCamera.h"
#include "d3d12/d3dx12_core.h"
#include "d3d12/d3dx12_root_signature.h"
#include "Graphics/Buffers/ConstantBuffer.h"
#include "Helpers/FrameCounter.h"
#include "Input/GameInputInterface.h"
#include "Input/GeneralKeyboardDevice.h"
#include "Input/GeneralMouseDevice.h"
#include "Primitive/Model.h"
#include "Rendering/DepthStencilView.h"
#include "Rendering/HlslShader.h"
#include "Windowing/GameWindow.h"

using Microsoft::WRL::ComPtr;

struct RootConstants {
    DirectX::SimpleMath::Matrix WorldViewProjection;
    DirectX::SimpleMath::Matrix WorldInverseTranspose;
};

class DemoMainWindow : public Vertix::GameWindow {
public:
    DemoMainWindow() {
        SetWindowTitle(L"D3D12GameDemo.MainWindow");
        GetD3D12ViewportAndScissorRect(viewport, scissorRect);
    }

    ~DemoMainWindow() override {
        delete depthStencilView;
        delete constantBuffer;
    }
private:
    void OnInitialize() override;

    void OnUpdate(double deltaTime) override;
    void OnRender(double deltaTime) override;
    void OnResized(Vertix::Vector2D<UINT> size) override;

    void OnFocusLost() override {
        if (enableRotating) {
            ShowCursor(true);
            enableRotating = false;
        }
    }

    void FillConstantBuffer() const {
        RootConstants constants = {};
        constants.WorldViewProjection = worldMatrix * viewMatrix * projectionMatrix;
        worldMatrix.Invert(constants.WorldInverseTranspose);
        constants.WorldInverseTranspose.Transpose(constants.WorldInverseTranspose);

        constantBuffer->Fill(constants);
    }

    bool enableRotating = false;

    DirectX::SimpleMath::Matrix worldMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix viewMatrix;
    DirectX::SimpleMath::Matrix projectionMatrix;

    Vertix::Model cubeModel;
    Vertix::Engine::PerspectiveCamera perspectiveCamera;

    Vertix::HlslShader vertexShader{L"assets/shaders/Simple3dShader.hlsl"};
    Vertix::HlslShader pixelShader{L"assets/shaders/Simple3dShader.hlsl"};
    Vertix::ConstantBuffer<RootConstants>* constantBuffer = nullptr;
    Vertix::DepthStencilView* depthStencilView = nullptr;

    ComPtr<ID3D12PipelineState> pipelineState;
    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12GraphicsCommandList5> commandList;
    ComPtr<ID3D12DescriptorHeap> depthStencilDescriptorHeap;

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{};
    CD3DX12_VIEWPORT viewport{0.0,0.0,800.0,600.0};
    CD3DX12_RECT scissorRect{};

    Vertix::Engine::GameInputInterface inputInterface;
    Vertix::Engine::GeneralKeyboardDevice keyboardDevice {inputInterface};
    Vertix::Engine::GeneralMouseDevice mouseDevice {inputInterface};

#ifndef NDEBUG
    FrameCounter frameCounter;
#endif
};

#endif //VERTIX_DEMOMAINWINDOW_H
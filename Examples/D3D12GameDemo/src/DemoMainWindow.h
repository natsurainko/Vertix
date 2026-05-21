//
// Created by Natsurainko on 2025/12/23.
//

#ifndef VERTIX_DEMOMAINWINDOW_H
#define VERTIX_DEMOMAINWINDOW_H

#include <d3d12/d3dx12_core.h>
#include <d3d12/d3dx12_root_signature.h>

#include "Vertix/Rendering/Buffers/ConstantBuffer.hpp"
#include "Vertix.Engine/Camera/PerspectiveCamera.h"
#include "Vertix.Engine/Helpers/FrameCounter.hpp"
#include "Vertix.Engine/Input/GameInputInterface.h"
#include "Vertix.Engine/Input/GeneralKeyboardDevice.hpp"
#include "Vertix.Engine/Input/GeneralMouseDevice.hpp"
#include "Vertix/Graphics/DescriptorHeapSet.h"
#include "Vertix/Graphics/DescriptorView.h"
#include "Vertix/Primitive/Model.h"
#include "Vertix/Rendering/RenderTexture.h"
#include "Vertix/Windowing/GameWindow.h"

using Microsoft::WRL::ComPtr;

struct RootConstants {
    DirectX::SimpleMath::Matrix WorldViewProjection;
    DirectX::SimpleMath::Matrix WorldInverseTranspose;
};

class DemoMainWindow : public Vertix::GameWindow {
public:
    explicit DemoMainWindow(const Vertix::WindowOptions &options) : GameWindow(options) {
        GetD3D12ViewportRectSize(viewport, scissorRect);
    }

    void OnInitialize() override;

protected:
    void OnUpdate(double deltaTime) override;
    void OnRender(double deltaTime) override;
    void OnResized(const Vertix::Vector2D<UINT> &size) override;
    void OnFocusLost() override;

private:
    void FillConstantBuffer() const;

    bool enableRotating = false;

    DirectX::SimpleMath::Matrix worldMatrix = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Matrix viewMatrix;
    DirectX::SimpleMath::Matrix projectionMatrix;

    Vertix::Model cubeModel;
    Vertix::Engine::PerspectiveCamera perspectiveCamera;

    std::unique_ptr<Vertix::ConstantBuffer<RootConstants>> constantBuffer;
    std::unique_ptr<Vertix::RenderTexture2D> depthStencilTexture;

    std::unique_ptr<Vertix::DescriptorHeapSet> descriptorHeapSet;
    Vertix::DescriptorView<Vertix::RenderResourceUsage::DepthWrite> depthStencilView{};
    Vertix::DescriptorView<Vertix::RenderResourceUsage::RenderTarget> renderTargetViews[2] = {};

    ComPtr<ID3D12PipelineState> pipelineState;
    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12GraphicsCommandList5> commandList;

    CD3DX12_VIEWPORT viewport{0.0,0.0, 0.0, 0.0};
    CD3DX12_RECT scissorRect{};

    Vertix::Engine::GameInputInterface inputInterface;
    Vertix::Engine::GeneralKeyboardDevice keyboardDevice {inputInterface};
    Vertix::Engine::GeneralMouseDevice mouseDevice {inputInterface};
#ifndef NDEBUG
    Vertix::Engine::FrameCounter frameCounter;
#endif
};

#endif //VERTIX_DEMOMAINWINDOW_H
